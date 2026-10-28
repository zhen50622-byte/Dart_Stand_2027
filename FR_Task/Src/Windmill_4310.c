//
// Created by 28416 on 2026/10/10.
//
#include "Windmill_4310.h"
uint8_t flag_servo = 0;
uint8_t flag_4310_stop = 0;//4310 if 判断停下标志位
void Task_4310_start(void *argument)
{
    /* USER CODE BEGIN DM_Motor_Task_Start */
    /* Infinite loop */
    t0 = osKernelGetTickCount();//获取当前时间
    Windmill_Target = 0.0f;//第一次位置为初始位置
    for(;;)
    {
        switch (Windmill_flag)
        {
        case  0:
            DM_Motor_Setposition(&hcan2,0x51,POSITION,Windmill_Target,7.0f);//右,正方向/大风车的方向到时候具体看
            if (fabsf(DM_4310_position - Windmill_Target) < 0.05f && flag_4310_stop == 0)//当误差小于0.05时 判定为真
            {
                //当达妙电机转120°时 2006开始下降到目标位置
                target_position_2006 = -2000.0f;
                if (fabsf(actuall_position_2006 - target_position_2006) < 0.5f)
                {
                    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1,200.0f);//放飞镖
                    flag_servo = 1;
                    flag_4310_stop = 1;//这样第一个if只会执行一次
                    t0 = osKernelGetTickCount();//获取当前时间
                }//当误差小于0.05时 2006到达了指定位置 可以放飞镖
                //舵机代码+暂时延时处理

            }
            if (osKernelGetTickCount()-t0>=3000u && flag_servo == 1)//当放下飞镖 且经过3s后 舵机恢复初始状态
            {
                __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1,320.0f);//爪子恢复初始状态
                //3s 过后 2006可以回到初始位置
                target_position_2006 = 0.0f;

                //t0 = osKernelGetTickCount();//获取当前时间

                if (fabsf(actuall_position_2006 - target_position_2006) < 0.5f)
                {
                    Windmill_Target = Windmill_Target + (2*PI)/3.0f;//因为达妙电机带多圈处理 所以位置目标值在每次的基础上加三分之一圈 放在这儿 防止一直进if判断一直加角度
                    Windmill_flag = 1;
                }//当误差小于0.05时 即2006回到原点时 DM电机标志位更新 转到240°

            }
            break;
        case  1:
            DM_Motor_Setposition(&hcan2,0x51,POSITION,Windmill_Target,7.0f);//右,正方向/大风车的方向到时候具体看
            //舵机代码+暂时延时处理
            if (fabsf(DM_4310_position - Windmill_Target) < 0.05f && flag_4310_stop == 1)//当误差小于0.05时 判定为真
            {
                //当达妙电机转240°时 2006开始下降到目标位置
                target_position_2006 = -2000.0f;
                if (fabsf(actuall_position_2006 - target_position_2006) < 0.5f)
                {
                    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2,1000.0f);//继续爪松开飞镖
                    flag_servo = 2;
                    flag_4310_stop = 2;
                    t0 = osKernelGetTickCount();//获取当前时间
                }//当误差小于0.05时 2006到达了指定位置 可以放飞镖
                //舵机代码+暂时延时处理
            }

            if (osKernelGetTickCount()-t0>=3000u && flag_servo ==2)//当时间为1秒时或太短时 电机运行时间恰为1s就会把__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2,680.0f);覆盖掉
            {
                __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2,700.0f);//爪子恢复初始状态
                //3s 过后 2006可以回到初始位置
                target_position_2006 = 0.0f;


                if (fabsf(actuall_position_2006 - target_position_2006) < 0.5f)
                {
                    Windmill_Target = Windmill_Target + (2*PI)/3.0f;
                    Windmill_flag = 2;
                }//当误差小于0.05时 即2006回到原点时 DM电机标志位更新 转到360°
            }
            break;
        case  2:
            DM_Motor_Setposition(&hcan2,0x51,POSITION,Windmill_Target,7.0f);//右,正方向/大风车的方向到时候具体看
            if (fabsf(DM_4310_position - Windmill_Target) < 0.05f && flag_4310_stop == 2)//当误差小于0.05时 判定为真
            {
                //当达妙电机转360°时 2006开始下降到目标位置
                target_position_2006 = -2000.0f;
                //舵机代码+暂时延时处理
                if (fabsf(actuall_position_2006 - target_position_2006) < 0.5f)
                {
                    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3,630.0f);//机械爪松开飞镖
                    flag_servo = 3;
                    flag_4310_stop = 0;
                    t0 = osKernelGetTickCount();//获取当前时间
                }//当误差小于0.05时 2006到达了指定位置 可以放飞镖
            }
            if (osKernelGetTickCount()-t0>=3000u && flag_servo == 3)//
            {
                __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3,400.0f);//爪子恢复初始状态
                //3s 过后 2006可以回到初始位置
                target_position_2006 = 0.0f;


                if (fabsf(actuall_position_2006 - target_position_2006) < 0.5f)
                {
                    Windmill_Target = Windmill_Target + (2*PI)/3.0f;
                    Windmill_flag = 0;
                }//当误差小于0.05时 即2006回到原点时
            }
            break;
        }
        ////VOFA_Send(&huart7,1,DM_4310_position);//发送数据得是浮点数
        osDelay(1);
    }
    /* USER CODE END DM_Motor_Task_Start */
}