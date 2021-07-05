#include "encoder.h"
#include <stm32f10x_tim.h>

static GPIO_InitTypeDef GPIO_InitStructure = {
    .GPIO_Speed = GPIO_Speed_2MHz, // default
    .GPIO_Mode = GPIO_Mode_IN_FLOATING // default
};
static NVIC_InitTypeDef NVIC_InitStructure;
static /*const*/ TIM_TimeBaseInitTypeDef TIM_TBS = {
    .TIM_ClockDivision = TIM_CKD_DIV1, //default 设置时钟分割: 不分频
    .TIM_CounterMode = TIM_CounterMode_Up, //default TIM向上计数模式
    .TIM_Period = ENCODER_TIM_PERIOD, //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    .TIM_Prescaler = 0x0000, //default 设置用来作为TIMx时钟频率除数的预分频值  不分频
    .TIM_RepetitionCounter = 0x0000 // default + not functional
};
static TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {
    .TIM_ClockDivision = TIM_CKD_DIV1, //default
    .TIM_CounterMode = TIM_CounterMode_Up, //default
    .TIM_RepetitionCounter = 0x0000 // default + not functional
};
static /*const*/ TIM_ICInitTypeDef TIM_ICIS = {
    .TIM_Channel = TIM_Channel_1, // default
    .TIM_ICPolarity = TIM_ICPolarity_Rising, // default
    .TIM_ICSelection = TIM_ICSelection_DirectTI, // default
    .TIM_ICPrescaler = TIM_ICPSC_DIV1, // default
    .TIM_ICFilter = 0x0A // PREFER
};

void TIM2_InitEncoder(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能定时器2的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);// 需要使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PB端口时钟
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);//引脚重映射

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	//端口配置
    GPIO_Init(GPIOA, &GPIO_InitStructure); //根据设定参数初始化GPIOA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//端口配置
    GPIO_Init(GPIOB, &GPIO_InitStructure); //根据设定参数初始化GPIOB 

    // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM2, &TIM_TBS);
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3

    // TIM_ICStructInit(&TIM_ICInitStructure);
    // TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM2, &TIM_ICIS);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除TIM的更新标志位

    //Reset counter
    TIM_SetCounter(TIM2,0);
    TIM_Cmd(TIM2, ENABLE); 
}

void TIM3_Init(u16 arr, u16 psc)
{

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能定时器3的时钟

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级 3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级 3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道使能
    NVIC_Init(&NVIC_InitStructure); //中断优先级初始化

    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void TIM4_InitEncoder(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能定时器4的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);// 需要使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//端口配置
    GPIO_Init(GPIOB, &GPIO_InitStructure); //根据设定参数初始化GPIOB 

    // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM4, &TIM_TBS);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3

    // TIM_ICStructInit(&TIM_ICInitStructure);
    // TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM4, &TIM_ICIS);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM的更新标志位

    //Reset counter
    TIM_SetCounter(TIM4,0);
    TIM_Cmd(TIM4, ENABLE); 
}

s16 ENCODER_Read(u8 TIMX)
{
    static s16 Encoder_TIM;    
    switch(TIMX)
    {
        case 2:
            Encoder_TIM = (short)TIM2 -> CNT;
            TIM2 -> CNT = 0;
            break;
        case 4:
            Encoder_TIM = (short)TIM4 -> CNT;
            TIM4 -> CNT = 0;
            break;	
        default:
            Encoder_TIM = 0;
            break;
    }
    return Encoder_TIM;
}
