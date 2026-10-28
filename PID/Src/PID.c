//
// Created by 28416 on 2026/8/1.
//
#include "PID.h"
void PID_Init(PID_Struct*pid)
{
    pid->error_accumulate=0.0f;
    pid->error_last=0.0f;
}
float PID_SET (PID_Struct*pid,float target,float current,float KP,float KI,float KD,float limit_KI,float limit_out)
{
    float error_now=0.0f;
    float out=0.0f;
    //记录当前返回数据
    error_now=target-current;
    //计算累计误差
    pid->error_accumulate=pid->error_accumulate+error_now;
    //积分限幅
    if (pid->error_accumulate>limit_KI)
    {
        pid->error_accumulate=limit_KI;
    }
    if (pid->error_accumulate<(-limit_KI))
    {
        pid->error_accumulate=(-limit_KI);
    }
    //计算out值
    out=KP*error_now+KI*(pid->error_accumulate)+KD*(error_now-pid->error_last);
    //更新上次误差
    pid->error_last=error_now;
    //输出限幅
    if (out>limit_out){out=limit_out;}
    if (out<(-limit_out)){out=(-limit_out);}
    //返回计算结果
    return out;
}