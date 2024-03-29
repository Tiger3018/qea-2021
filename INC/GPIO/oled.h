#ifndef __OLED_H
#define __OLED_H			  	 

#include "sys.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
//-----------------OLED端口定义---------------- 
#define OLED_RST_Clr() PCout(15)=0   //RST
#define OLED_RST_Set() PCout(15)=1   //RST

#define OLED_RS_Clr() PCout(0)=0    //DC
#define OLED_RS_Set() PCout(0)=1    //DC

#define OLED_SCLK_Clr()  PCout(13)=0  //SCL
#define OLED_SCLK_Set()  PCout(13)=1   //SCL

#define OLED_SDIN_Clr()  PCout(14)=0   //SDA
#define OLED_SDIN_Set()  PCout(14)=1   //SDA

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

// 控制用函数

void OLED_WRByte(u8 dat,u8 cmd);	    
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_RefreshGram(void);		   				   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8, u8, const s8*);	 
#endif /* __OLED_H */