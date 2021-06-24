#ifndef __USART_H
#define __USART_H			  	 

#include <stm32f10x_usart.h>

#if defined ( __CC_ARM )
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)       
/* Arm Compiler above 6.10.1 (armclang) */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
/* GNU Compiler */
#elif defined ( __GNUC__ )
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif

#define USART_PRINT USART3

void USART3_Init(s32);
void UART5_Init(s32);
void USART3_Send(s32, ...);
void UART5_Send(s32, ...);
void UART5_Interact(s16);

/*
void USART1_Init(void);
void USART1_Interact(int);
*/


#endif /* __USART_H */