#ifndef __USRATX_H
#define __USRATX_H 
#include "sys.h"	  	
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
void uart5_send(u8 data);
void uart5_init(u32 bound);
void UART5_IRQHandler(void);
void UART_TX(void);
#endif

