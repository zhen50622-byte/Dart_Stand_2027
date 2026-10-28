//
// Created by 28416 on 2026/8/1.
//

#ifndef TEST__DM_MOTOR_H
#define TEST__DM_MOTOR_H
#include "can.h"
# define SPEED 0x200
# define POSITION 0x100
void DM_Motor_set_model_speed(CAN_HandleTypeDef *hcan,uint16_t motor_id);
void DM_Motor_set_model_position(CAN_HandleTypeDef *hcan,uint16_t motor_id);
void DM_Motor_Enable(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id);
void DM_Motor_Disable(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id);
void DM_Motor_Init(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id);
void DM_Motor_Setspeed(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id,float v);//该函数最好以1ms的周期进行发送，速度的正负
void DM_Motor_Setposition(CAN_HandleTypeDef *hcan,uint16_t motor_id,uint16_t model_id,float position,float v);//v是最高限速而不是目标速度
#endif //TEST__DM_MOTOR_H
//达妙电机s2325速度函数传值是轴端的,单位是rad/s 传 2Π 就是1s一圈，反馈的速度也是轴端，且单位也是rad/s
//达妙电机s2325上电后其固定位置为0.0rad