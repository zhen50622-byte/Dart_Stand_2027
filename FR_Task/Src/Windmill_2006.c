//
// Created by 28416 on 2026/10/13.
//
#include "cmsis_os2.h"
#include "Windmil_2006.h"
/* ---- 全局变量定义（Windmil_2006.h 里已 extern 声明，定义必须放文件作用域） ---- */
PID_Struct DJI_2006_speed    = {0};//2006 速度变量初始化 里面包含上次误差和累加误差
PID_Struct DJI_2006_position = {0};//和上面同理 每个变量的累加误差和上次误差都不一样 所以每个变量单独有一个PID_Struct结构体
volatile float actuall_position_2006 = 0.0f;   // CAN 中断里写、任务里读
volatile float actuall_speed_2006    = 0.0f;
volatile uint8_t flag_FIF0           = 0;      // CAN 收到 2006 反馈时置 1
volatile uint16_t actuall_position_2006_raw = 0;
volatile float actuall_speed_2006_raw = 0.0f;
/* ---- 2006 状态变量：放文件作用域，避免 unused 警告，也便于跨函数使用 ---- */
float Out_output           = 0.0f;
float Iner_output          = 0.0f;
float target_position_2006 = 0.0f;//由实际的机械机构决定
float target_speed_2006    = 0.0f;
float angle_l              = 0.0f;//这是上一次的角度
int16_t flag_2006          = 0;   //这是2006多圈处理专用变量
uint8_t first_sample       = 0;   //第一帧初始化标志
uint8_t Send_Data_2006[8]  = {0};

void Task_2006_start(void *argument)
{
    /* ↓↓↓ 这些变量已移到文件作用域定义（见文件顶部），这里注释掉，避免遮蔽全局变量 ↓↓↓
    PID_Struct DJI_2006_speed = {0};
    PID_Struct DJI_2006_position = {0};
    //采用串级PID控制2006位置
    float Out_output = 0.0f;
    float Iner_output = 0.0f;
    float target_position_2006 = 0.0f;
    float actuall_position_2006 = 0.0f;
    float target_speed_2006 = 0.0f;
    float actuall_speed_2006 = 0.0f;
    float angle_l = 0.0f;
    uint8_t flag = 0.0f;
    uint8_t first_sample=0;//第一帧初始化标志
    uint8_t Send_Data_2006[8]={0};
    ↑↑↑ 注释结束 ↑↑↑ */
    for (;;)//关于内外环的调控周期 此处暂时让内外环调控周期相同
    {
        //多圈处理代码
        if (flag_FIF0==1)//当解析到新的角度后 开始解析角度数据
        {
            uint16_t raw = actuall_position_2006_raw;              // 本地拷贝，避免被中断覆盖
            float rotor_deg = raw * 360.0f / 8192.0f;   // 转子角 0~360

            if (first_sample == 0)
            {
                angle_l = rotor_deg;
                first_sample = 1;
            }//该行代码是防止第一次上电时 angle_l = 0 当rotor_deg角度大于180°时就会造成误判跨圈 因为我电机实际的上次角度并不是0°
            else
            {
                if (angle_l - rotor_deg > 180.0f)      flag_2006++;  // 正转跨圈
                else if (rotor_deg - angle_l > 180.0f) flag_2006--;  // 反转跨圈
            }

            angle_l = rotor_deg;
            actuall_position_2006 = (rotor_deg + flag_2006 * 360.0f) / 36.0f; // 输出轴多圈角度 36.0f是2006的减速比
            flag_FIF0 = 0;
        }
        //外环速度环代码
         target_speed_2006 = PID_SET(&DJI_2006_position,target_position_2006,actuall_position_2006,5.6f,0.0f,127.0f,5000.0f,485.0f);
        //内环速度环代码
        actuall_speed_2006 = actuall_speed_2006_raw/36.0f;
        Iner_output = PID_SET(&DJI_2006_speed,target_speed_2006,actuall_speed_2006,57.12f,0.15f,0.0f,8000.0f,8000.0f);
        int16_t cmd = (int16_t)Iner_output;
        Send_Data_2006[2] = (uint8_t)(cmd >> 8);//数组第几个得看ID是多少
        Send_Data_2006[3] = (uint8_t)(cmd & 0xFF);
        CAN_Send_Data(&hcan2, 0x200, Send_Data_2006, 8);

        osDelay(1);
    }
}

