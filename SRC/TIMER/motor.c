#include "motor.h"

static GPIO_InitTypeDef GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
static TIM_OCInitTypeDef TIM_OCInitStructure;

/**
  * @brief 函数功能：舵机PWM以及定时中断初始化
  * @param arr：自动重装值
  * @param psc：时钟预分频数 
  * @return void
  */
void PWM_Init(u16 arr, u16 psc)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 | RCC_APB2Periph_GPIOC, ENABLE); //使能GPIO外设时钟使能

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; //TIM_CH1 //TIM_CH4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = arr;                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //设置用来作为TIMx时钟频率除数的预分频值  不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(MOTOR_TIM, &TIM_TimeBaseStructure);             //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0;                            //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高
    TIM_OC1Init(MOTOR_TIM, &TIM_OCInitStructure); //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
    TIM_OC2Init(MOTOR_TIM, &TIM_OCInitStructure); //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
    TIM_OC3Init(MOTOR_TIM, &TIM_OCInitStructure); //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
    TIM_OC4Init(MOTOR_TIM, &TIM_OCInitStructure); //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

    TIM_CtrlPWMOutputs(MOTOR_TIM, ENABLE); //MOE 主输出使能

    TIM_OC1PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable); //CH1预装载使能
    TIM_OC2PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable); //CH1预装载使能
    TIM_OC3PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable); //CH1预装载使能
    TIM_OC4PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable); //CH4预装载使能

    TIM_ARRPreloadConfig(MOTOR_TIM, ENABLE); //使能TIMx在ARR上的预装载寄存器

    TIM_Cmd(MOTOR_TIM, ENABLE); //使能TIM
}

void MOTOR_Set(s16 motor_a, s16 motor_b)
{
    if(!(motor_a | motor_b))
    {
        PWMA1 = PWMA2 = PWMB1 = PWMB2 = 0;
        return;
    }

    if(motor_a > 0)
        PWMA1 = 7200, PWMA2 = 7200 - motor_a;
    else
        PWMA2 = 7200, PWMA1 = 7200 + motor_a;

    if(motor_b > 0)
        PWMB1 = 7200, PWMB2 = 7200 - motor_b;
    else
        PWMB2 = 7200, PWMB1 = 7200 + motor_b;
}