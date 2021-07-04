#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"
#include <stm32f10x_tim.h>

#define MOTOR_TIM TIM8
#define PWMA1   MOTOR_TIM->CCR3
#define PWMA2   MOTOR_TIM->CCR1
#define SERVO   TIM1->CCR1  //舵机引脚
#define PWMB1   MOTOR_TIM->CCR4
#define PWMB2   MOTOR_TIM->CCR2

void PWM_Init(u16 arr,u16 psc);
void PWM_Set(s32, s32);
//void Servo_PWM_Init(u16 arr,u16 psc);

#endif /* __MOTOR_H */
