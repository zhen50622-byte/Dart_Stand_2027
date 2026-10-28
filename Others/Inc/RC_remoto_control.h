//
// Created by 28416 on 2026/8/28.
//

#ifndef OLD_DART_STAND_RC_REMOTO_CONTROL_H
#define OLD_DART_STAND_RC_REMOTO_CONTROL_H

#include "stdint.h"
typedef struct RC_Ctl_t
{
    struct
    {
        uint16_t ch0;
        uint16_t ch1;
        uint16_t ch2;
        uint16_t ch3;
        uint16_t dial;//拨轮
        uint8_t  s1;
        uint8_t  s2;
    }rc;
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    }mouse;
    struct
    {
        uint16_t v;
    }key;
}RC_Ctl_t;

extern  RC_Ctl_t RC_CtrlData;
#endif //OLD_DART_STAND_RC_REMOTO_CONTROL_H