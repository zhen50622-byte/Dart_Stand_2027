//
// Created by 28416 on 2026/7/24.
//
#include "vofa.h"
#include <string.h>
#include <stdarg.h>

void VOFA_Send(UART_HandleTypeDef *huart, uint8_t count, ...)
{
    static uint8_t frame[10 * sizeof(float) + 4];  // 最多10个float + 帧尾
    va_list args;
    uint8_t i;
    uint16_t len;

    // 限制最大通道数
    if (count > 10)
    {
        count = 10;
    }
    if (count == 0)
    {
        return;
    }
    // 读取可变参数
    va_start(args, count);
    for (i = 0; i < count; i++)
    {
        float f = (float)va_arg(args, double);  // float在可变参数中提升为double
        memcpy(&frame[i * sizeof(float)], &f, sizeof(float));
    }
    va_end(args);

    // 帧尾
    len = count * sizeof(float);
    frame[len + 0] = 0x00;
    frame[len + 1] = 0x00;
    frame[len + 2] = 0x80;
    frame[len + 3] = 0x7F;

    // DMA 发送 (数据+帧尾一帧发出)
    HAL_UART_Transmit_DMA(huart, frame, len + 4);
}
