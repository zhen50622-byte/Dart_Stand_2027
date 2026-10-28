//
// Created by 28416 on 2026/7/24.
//

#ifndef __VOFA_H
#define __VOFA_H

#include "main.h"
#include "usart.h"
#include <stdarg.h>

// 发送指定数量的float数据到指定串口
// huart: 串口句柄，如 &huart7
// count: 要发送的float数据个数（1-10）
// ...: 可变参数，要发送的float数据
void VOFA_Send(UART_HandleTypeDef *huart, uint8_t count, ...);

#endif