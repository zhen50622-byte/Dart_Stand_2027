//
// Created by 28416 on 2026/8/1.
//

#ifndef TEST__PID_H
#define TEST__PID_H
typedef  struct
{
   float error_last;
   float error_accumulate;
}PID_Struct;//不同电机结构体
void PID_Init(PID_Struct*pid);
float PID_SET (PID_Struct*pid,float target,float current,float KP,float KI,float KD,float limit_KI,float limit_out);//结尾不带{}而是;
//使用案例// main.c 或模块顶层
//PID_Struct pid_speed = {0};
//PID_Init(&pid_speed);              // 初始化一次，别放在循环里

// 周期调用（定时器中断/主循环）
//float speed_out = PID_SET(&pid_speed,
//                          1000.0f,        // 目标转速
//                          current_speed,  // 编码器读到的实际转速
//                          1.5f,           // KP
//                          0.02f,          // KI
//                          0.1f,           // KD
//                          500.0f,         // 积分限幅
//                          3000.0f);       // 输出限幅（你的PWM/电流上限）
#endif //TEST__PID_H