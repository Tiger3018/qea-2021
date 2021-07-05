#include "sys.h"

u8 nullVar; // For empty use.
static u8  fac_us = 1;      //us延时倍乘数			   
static u16 fac_ms = 0;      //ms延时倍乘数,在ucos下,代表每个节拍的ms数

void SYS_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  
#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
    reload=SystemCoreClock/8000000;				//每秒钟的计数次数 单位为M  
    reload*=1000000/delay_ostickspersec;		//根据delay_ostickspersec设定溢出时间
                                                //reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
    fac_ms=1000/delay_ostickspersec;			//代表OS可以延时的最少单位	   

    SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
    SysTick->LOAD=reload; 						//每1/delay_ostickspersec秒中断一次	
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    
#else
    fac_ms = (u16)fac_us * 1000;					//非OS下,代表每个ms需要的systick时钟数   
#endif /* SYSTEM_SUPPORT_OS */
}

void DELAY_ms(u16 timeValue)
{
    u32 temp;		   
    SysTick -> LOAD = timeValue * fac_ms;				//时间加载(SysTick->LOAD为24bit)
    SysTick->VAL =0x00;							//清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
    do
    {
        temp=SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
    SysTick->VAL =0X00;       					//清空计数器	  	    
}