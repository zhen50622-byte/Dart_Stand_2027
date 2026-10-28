//
// Created by 28416 on 2026/10/13.
//

#ifndef DART_STAND_2027_WINDMIL_2006_H
#define DART_STAND_2027_WINDMIL_2006_H
#include "PID.h"
#include "drv_can.h"
extern PID_Struct DJI_2006_speed;
extern PID_Struct DJI_2006_position;
extern volatile float actuall_position_2006;   // CAN中断里写、任务里读
extern volatile float actuall_speed_2006;
extern volatile uint8_t flag_FIF0;
extern volatile uint16_t actuall_position_2006_raw;
extern volatile float actuall_speed_2006_raw;
extern float target_position_2006;
#endif //DART_STAND_2027_WINDMIL_2006_H