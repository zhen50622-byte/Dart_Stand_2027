#include <stdint.h>
#include "drv_can.h"
#include "DM_Motor.h"
//
// Created by 28416 on 2026/8/1.
//
void DM_Motor_set_model_speed(CAN_HandleTypeDef *hcan,uint16_t motor_id)
{
    uint8_t can_id_l = motor_id & 0xFF;         // 低 8 位
    uint8_t can_id_h = (motor_id >> 8) & 0x07;  // 高 3 位
    uint8_t Send_Data[8]={can_id_l,can_id_h,0x55,10,3,0,0,0};
    CAN_Send_Data(hcan,0x7FF,Send_Data,8);
}

void DM_Motor_set_model_position(CAN_HandleTypeDef *hcan,uint16_t motor_id)
{
    uint8_t can_id_l = motor_id & 0xFF;         // 低 8 位
    uint8_t can_id_h = (motor_id >> 8) & 0x07;  // 高 3 位
    uint8_t Send_Data[8]={can_id_l,can_id_h,0x55,10,2,0,0,0};
    CAN_Send_Data(hcan,0x7FF,Send_Data,8);
}

void DM_Motor_Enable (CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id)
{
    uint16_t ID=model_id+motor_id;//ID是uint8_t时,不能表示0x200以上的数,超出8位表示的最大值
    uint8_t Enable_Data[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC};
    CAN_Send_Data(hcan,ID,Enable_Data,8);
}

void DM_Motor_Disable(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id)
{
    uint16_t ID=model_id+motor_id;//ID是uint8_t时,不能表示0x200以上的数,超出8位表示的最大值
    uint8_t Disable_Data[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFD};
    CAN_Send_Data(hcan,ID,Disable_Data,8);
}

void DM_Motor_Init(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id)
{
    switch (model_id)
    {
        case SPEED:
        HAL_Delay(1000);   // 等电机上电自检完成
        DM_Motor_set_model_speed(hcan,motor_id);
        HAL_Delay(10);   // 等模式切换完成
        DM_Motor_Enable(hcan,motor_id,model_id);
        break;
    case POSITION:
        HAL_Delay(1000);   // 等电机上电自检完成
        DM_Motor_set_model_position(hcan,motor_id);
        HAL_Delay(10);   // 等模式切换完成
        DM_Motor_Enable(hcan,motor_id,model_id);
        break;
    }
}

void DM_Motor_Setspeed(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id,float v)
{
    uint16_t ID=model_id+motor_id;
    uint8_t*p=(uint8_t*)&v;//浮点数是小端存储&v是低位的地址
    uint8_t Send_Data[4]={0x00};
    Send_Data[0]=*p;
    Send_Data[1]=*(p+1);
    Send_Data[2]=*(p+2);
    Send_Data[3]=*(p+3);
    CAN_Send_Data(hcan,ID,Send_Data,4);
}

void DM_Motor_Setposition(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id,float position,float v)
{
    uint16_t ID=model_id+motor_id;
    uint8_t*p=(uint8_t*)&v;//浮点数是小端存储&v是低位的地址
    uint8_t*p0=(uint8_t*)&position;
    uint8_t Send_Data[8]={0x00};
    //写入位置
    Send_Data[0]=*p0;
    Send_Data[1]=*(p0+1);
    Send_Data[2]=*(p0+2);
    Send_Data[3]=*(p0+3);
    //写入速度
    Send_Data[4]=*p;
    Send_Data[5]=*(p+1);
    Send_Data[6]=*(p+2);
    Send_Data[7]=*(p+3);
    CAN_Send_Data(hcan,ID,Send_Data,8);
}
