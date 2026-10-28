//
// Created by 28416 on 2026/10/10.
//

#ifndef DART_STAND_2027_DM_MOTOR_TASK_H
#define DART_STAND_2027_DM_MOTOR_TASK_H
#include "cmsis_os2.h"
#include "DM_Motor.h"
#include "tim.h"
#include "math.h"
#include "vofa.h"
#include "Windmil_2006.h"
#define PI 3.1415926f
extern  float DM_4310_position;
extern  uint32_t t0;
extern  uint8_t Windmill_flag;
extern  float Windmill_Target;
#endif //DART_STAND_2027_DM_MOTOR_TASK_H