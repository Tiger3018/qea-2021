#ifndef __TIMER_H
#define __TIMER_H
#include <sys.h>	 
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define TRIPA PAout(11)//A路超声波 PA11接触发引脚 PA0接回波引脚
#define TRIPB PAout(4)//B路超声波 PA4接触发引脚 PA1接回波引脚
#define TRIPC PCout(5)//C路超声波 PC5接触发引脚 PA2接回波引脚
#define TRIPD PBout(12)//D路超声波 PB12接触发引脚 PA3接回波引脚

/*-------------超声波测距程序使用-----------
超声波初始化之后，在control.c的中断服务函数里面可以直接通过Read_Distane函数读取
放在下列的变量里面
Distance_A,Distance_B,Distance_C,Distance_D;//超声波相关变量 
-----------超声波测距程序使用-----------*/

/*-------------航模遥控程序使用和接线-----------
TIM3的 4个通道可以接航模遥控
PA6 PA7 PB0 PB1对应4个通道，具体的对应关系取决于控制代码
代码仅仅是采集，具体需要如使用这个指令可以编程设定
初始化航模遥控，即可通过中断读取航模遥控的脉宽，放在下列的变量里面
Remoter_Ch1,Remoter_Ch2,Remoter_Ch3,Remoter_Ch4;//航模遥控采集相关变量
-----------航模遥控接线-----------*/
void TIM5_Cap_Init(u16 arr,u16 psc)	;  
void Read_Distane(void);
void TIM5_IRQHandler(void);
void TIM3_Cap_Init(u16 arr, u16 psc);
void TIM3_IRQHandler(void);
#endif
