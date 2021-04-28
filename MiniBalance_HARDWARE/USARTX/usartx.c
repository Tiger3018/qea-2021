#include "usartx.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Uart5_Receive;
/**************************ʵ�ֺ���**********************************************
*��    ��:		usart����һ���ֽ�
*********************************************************************************/
void uart5_send(u8 data)
{
	UART5->DR = data;
	while((UART5->SR&0x40)==0);	
}


/**************************************************************************
�������ܣ�����5��ʼ��
��ڲ����� bound:������
����  ֵ����
**************************************************************************/
void uart5_init(u32 bound)
{  	 
	  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);// ��Ҫʹ��AFIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��GPIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��GPIOʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//ʹ��USARTʱ��

	//USART_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //Pc12
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  //USART_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//Pd2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  //UsartNVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART5, &USART_InitStructure);     //��ʼ������
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ���
}

/**************************************************************************
�������ܣ�����5�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
void UART5_IRQHandler(void)
{	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //���յ�����
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
�������ܣ�ͨ�����ڰ�����Ĵ��������ͳ�ȥ
**************************************************************************/
void UART_TX(void)
{
        u8 Direction_Left,Direction_Right;//���ʹ������ݵ��ⲿ
	      u16 Temp_GZ,Temp_Roll;
	           if(Encoder_Left>0) Direction_Left=0;
        else if(Encoder_Left<0) Direction_Left=2;
	      else                 		Direction_Left=1;
		         if(Encoder_Right>0) Direction_Right=0;
        else if(Encoder_Right<0) Direction_Right=2;
	      else                 		 Direction_Right=1;     

      	Temp_GZ=Gryo_Z+32768;//���ٶ����ݴ���
				Temp_Roll=Roll*100+15000;//ǰ������Ƕ�
				uart5_send(0xff); //֡ͷ
				uart5_send(0xfe); //֡ͷ
				uart5_send(abs(Encoder_Left));//�����ٶ�
				uart5_send(Direction_Left);	//��������
				uart5_send(abs(Encoder_Right));	//�ҵ���ٶ�	
				uart5_send(Direction_Right);	//�ҵ������
				uart5_send(Temp_Roll>>8);	//ǰ������Ƕ����ݸ�8λ	
				uart5_send(Temp_Roll&0x00ff);	//ǰ������Ƕ����ݵ�8λ
				uart5_send(Temp_GZ>>8);	//���������ݸ�8λ	
				uart5_send(Temp_GZ&0x00ff);	//���������ݵ�8λ
}
