#ifndef __ENCODER_H
#define __ENCODER_H

#include "sys.h"

#define ENCODER_TIM_PERIOD (u16)(0xFFFF)   //default 不可大于65535 因为F103的定时器是16位的。

void TIM2_InitEncoder(void);
void TIM3_Init(u16, u16);
void TIM4_InitEncoder(void);
u16 ENCODER_Read(u8);

#endif /* __ENCODER_H */