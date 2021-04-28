#include "usartx.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Uart5_Receive;
/**************************实现函数**********************************************
*功    能:		usart发送一个字节
*********************************************************************************/
void uart5_send(u8 data)
{
	UART5->DR = data;
	while((UART5->SR&0x40)==0);	
}


/**************************************************************************
函数功能：串口5初始化
入口参数： bound:波特率
返回  值：无
**************************************************************************/
void uart5_init(u32 bound)
{  	 
	  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);// 需要使能AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//使能GPIO时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//使能USART时钟

	//USART_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //Pc12
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  //USART_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//Pd2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  //UsartNVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART5, &USART_InitStructure);     //初始化串口
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART5, ENABLE);                    //使能串口
}

/**************************************************************************
函数功能：串口5接收中断
入口参数：无
返回  值：无
**************************************************************************/
void UART5_IRQHandler(void)
{	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //接收到数据
	{	  
				u8 temp;
				static u8 count,last_data,last_last_data,Usart_ON_Count;
		    if(Usart_ON_Flag==0)
				{	
		    if(++Usart_ON_Count>10)Usart_ON_Flag=1;
				}
				temp=UART5->DR;
				 if(Usart_Flag==0)
					{	
					if(last_data==0xfe&&last_last_data==0xff) 
					Usart_Flag=1,count=0;	
					}
				 if(Usart_Flag==1)
					{	
						Urxbuf[count]=temp;     
						count++;                
						if(count==8)Usart_Flag=0;
					}
					last_last_data=last_data;
					last_data=temp;
	}  											 
} 


/**************************************************************************
函数功能：通过串口把自身的传感器发送出去
**************************************************************************/
void UART_TX(void)
{
        u8 Direction_Left,Direction_Right;//发送串口数据到外部
	      u16 Temp_GZ,Temp_Roll;
	           if(Encoder_Left>0) Direction_Left=0;
        else if(Encoder_Left<0) Direction_Left=2;
	      else                 		Direction_Left=1;
		         if(Encoder_Right>0) Direction_Right=0;
        else if(Encoder_Right<0) Direction_Right=2;
	      else                 		 Direction_Right=1;     

      	Temp_GZ=Gryo_Z+32768;//角速度数据处理
				Temp_Roll=Roll*100+15000;//前进方向角度
				uart5_send(0xff); //帧头
				uart5_send(0xfe); //帧头
				uart5_send(abs(Encoder_Left));//左电机速度
				uart5_send(Direction_Left);	//左电机方向
				uart5_send(abs(Encoder_Right));	//右电机速度	
				uart5_send(Direction_Right);	//右电机方向
				uart5_send(Temp_Roll>>8);	//前进方向角度数据高8位	
				uart5_send(Temp_Roll&0x00ff);	//前进方向角度数据低8位
				uart5_send(Temp_GZ>>8);	//陀螺仪数据高8位	
				uart5_send(Temp_GZ&0x00ff);	//陀螺仪数据低8位
}
