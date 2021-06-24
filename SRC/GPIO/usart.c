#include "sys.h"
#include "usart.h"
#include "oled.h"
#include <stdarg.h>
#include <stm32f10x_dma.h>

#define EN_UART5_RX 0
#define EN_UART5_DMA 1

USART_InitTypeDef USART_InitStructure = {
    .USART_WordLength = USART_WordLength_8b, //字长为 8 位
    .USART_StopBits = USART_StopBits_1, //一个停止位
    .USART_Parity = USART_Parity_No, //无奇偶校验位
    .USART_HardwareFlowControl = USART_HardwareFlowControl_None, //无硬件数据流控制
    .USART_Mode = USART_Mode_Tx | USART_Mode_Rx // 收发模式
};
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

void USART3_Init(s32 baud)
{
    //时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);//引脚重映射
    
    USART_DeInit(USART3);
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3_TX PC.10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3_RX PC.11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    USART_InitStructure.USART_BaudRate = baud; //波特率设置
    USART_Init(USART3, &USART_InitStructure); //初始化串口

    #if EN_USART3_RX 
    #endif /* EN_USART3_RX */

    USART_Cmd(USART3, ENABLE); 
}

void UART5_Init(s32 baud)
{
    //时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    
    USART_DeInit(UART5);
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //UART5_TX PC.12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //UART5_RX PD.2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    USART_InitStructure.USART_BaudRate = baud; //波特率设置
    USART_Init(UART5, &USART_InitStructure); //初始化串口

    #if EN_UART5_RX 
    //⑤ 初始化 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ; //抢占优先级 3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级 3
    NVIC_InitStruc ture.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道使能
    NVIC_Init(&NVIC_InitStructure); //中断优先级初始化
    //⑤开启中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启中断
    #endif /* EN_UART5_RX */

    #if EN_UART5_DMA
    DMA_DeInit(DMA1_Channel1);
    #endif /* EN_UART5_DMA */

    USART_Cmd(UART5, ENABLE); 
}

inline void UART_Send(USART_TypeDef* UNAME, s32 num, ...)
{
    static s8 now_num; 
    static s32 temp; 
    va_list list_num; 
    temp = num; 
    va_start(list_num, num); 
    do 
    { 
        //while(USART_GetFlagStatus(UNAME, USART_FLAG_TXE) != SET) nullVar = UNAME -> DR; 
        if((now_num = num >> 24)) 
        { 
            USART_SendData(UNAME, now_num); 
            while(USART_GetFlagStatus(UNAME, USART_FLAG_TC) != SET); 
        } 
        else return; 
        if((now_num = (num >> 16) & 0xFF)) 
        { 
            USART_SendData(UNAME, now_num); 
            while(USART_GetFlagStatus(UNAME, USART_FLAG_TC) != SET); 
        } 
        else return; 
        if((now_num = (num >> 8) & 0xFF)) 
        { 
            USART_SendData(UNAME, now_num); 
            while(USART_GetFlagStatus(UNAME, USART_FLAG_TC) != SET); 
        } 
        else return; 
        if((now_num = num & 0xFF)) 
        { 
            USART_SendData(UNAME, now_num); 
            while(USART_GetFlagStatus(UNAME, USART_FLAG_TC) != SET); 
        } 
        else return; 
    }while((now_num = va_arg(list_num, s32))); 
}


void UART5_Interact(s16 num)
{

    USART_SendData(UART5, num);
    USART_SendData(USART3, num);
    while(USART_GetFlagStatus(UART5, USART_FLAG_TC) != SET)
    {
        //OLED_Clear();
        OLED_ShowString(25, 25, "1waiting...");
        OLED_RefreshGram();
    }
    UART5 -> DR = num;
    while(USART_GetFlagStatus(UART5, USART_FLAG_TC) != SET)
    {
        //OLED_Clear();
        OLED_ShowString(25, 25, "2waiting...");
        OLED_RefreshGram();
    }
    printf("This is a test text for printf function!!\nMy lucky number is %d, yes?\n", num);
    /*while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
    {
        //OLED_Clear();
        OLED_ShowString(25, 25, "3waiting...");
        OLED_RefreshGram();
    }*/
    while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != SET)
    {
        //OLED_Clear();
        OLED_ShowString(25, 25, "4saysomething...");
        OLED_RefreshGram();
    }
    OLED_Clear();
    OLED_ShowNumber(25,  5, USART3 -> DR, 2, 12);
    OLED_ShowNumber(25, 15, USART_GetFlagStatus(UART5, USART_FLAG_RXNE), 2, 12);
    OLED_RefreshGram();
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  * @see https://bbs.21ic.com/icview-3071492-1-1.html
  */
PUTCHAR_PROTOTYPE
{  
  USART_SendData(USART_PRINT, ch);
  while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC) == RESET);   
  return ch;
}
