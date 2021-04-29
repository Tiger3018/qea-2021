#include "show.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
unsigned char i;          //计数变量
unsigned char Send_Count; //串口需要发送的数据个数
/**************************************************************************
函数功能：OLED显示
入口参数：无
返回  值：无
**************************************************************************/
void oled_show(void)
{
						if(Flag_Way==0)	//遥控模式
					{
					 //=============第1行显示3轴角度===============//	
								OLED_ShowString(0,0,"X:");
								if(Pitch<0)		OLED_ShowNumber(15,0,Pitch+360,3,12);
								else					OLED_ShowNumber(15,0,Pitch,3,12);	
									 
								OLED_ShowString(40,0,"Y:");
								if(Roll<0)		OLED_ShowNumber(55,0,Roll+360,3,12);
								else					OLED_ShowNumber(55,0,Roll,3,12);	
							
								 OLED_ShowString(80,0,"Z:");
								if(Yaw<0)		  OLED_ShowNumber(95,0,Yaw+360,3,12);
								else					OLED_ShowNumber(95,0,Yaw,3,12);		
							//=============第二行Z轴陀螺仪和目标速度===============//	
																		OLED_ShowString(00,10,"GZ");
							if( gyro[2]<0)      	OLED_ShowString(20,10,"-"),
																		OLED_ShowNumber(30,10,-gyro[2],5,12);
							else                 	OLED_ShowString(20,10,"+"),
																		OLED_ShowNumber(30,10, gyro[2],5,12);		//z轴陀螺仪数据	
																		OLED_ShowString(70,10,"V");
																		OLED_ShowNumber(80,10, RC_Velocity,5,12);	//运动速度
					}
					else if(Flag_Way==1)//PS2模式
					{
						OLED_ShowString(00,0,"LX");
						OLED_ShowNumber(15,0, PS2_LX,3,12);  //PS2的数据			
						OLED_ShowString(40,0,"LY");
						OLED_ShowNumber(55,0, PS2_LY,3,12);  //PS2的数据
						OLED_ShowString(80,0,"RX");
						OLED_ShowNumber(95,0, PS2_RX,3,12);
						OLED_ShowString(0,10,"KEY");
						OLED_ShowNumber(25,10,PS2_KEY,2,12);
					}
					else if(Flag_Way==2)//ccd模式
					{
						OLED_Show_CCD(); 
						OLED_ShowString(00,10,"Mid");
						OLED_ShowNumber(25,10, CCD_Zhongzhi,3,12);
						OLED_ShowString(65,10,"Light");
						OLED_ShowNumber(105,10, CCD_Yuzhi,3,12);
					}
					else if(Flag_Way==3)//电磁巡线模式
					{
						OLED_ShowString(00,0,"L");
						OLED_ShowNumber(10,0,Sensor_Left,4,12);	
						OLED_ShowString(40,0,"M");
						OLED_ShowNumber(50,0,Sensor_Middle,4,12);
						OLED_ShowString(80,0,"R");
						OLED_ShowNumber(90,0,Sensor_Right,4,12);
						OLED_ShowString(0,10,"Mid");
						OLED_ShowNumber(25,10,Sensor,4,12);		  
					}									
					else if(Flag_Way == 4)
					{
					 //=============第1行显示3轴角度===============//	
						OLED_ShowString(0,0,"X:");
						if(Pitch<0)		OLED_ShowNumber(15,0,Pitch+360,3,12);
						else					OLED_ShowNumber(15,0,Pitch,3,12);	
								
						OLED_ShowString(40,0,"Y:");
						if(Roll<0)		OLED_ShowNumber(55,0,Roll+360,3,12);
						else					OLED_ShowNumber(55,0,Roll,3,12);	
					
						OLED_ShowString(80,0,"Z:");
						if(Yaw<0)		  OLED_ShowNumber(95,0,Yaw+360,3,12);
						else					OLED_ShowNumber(95,0,Yaw,3,12);		

						OLED_ShowString(00, 10, "OurMis SeaOfStar");
					}
					//=============第3行显示左电机的状态=======================//	
						if( Target_Left<0)		  OLED_ShowString(00,20,"-"),
																OLED_ShowNumber(15,20,-Target_Left,5,12);
					else                 	OLED_ShowString(0,20,"+"),
																OLED_ShowNumber(15,20, Target_Left,5,12); 			
					if( Encoder_Left<0)		  OLED_ShowString(80,20,"-"),
																OLED_ShowNumber(95,20,-Encoder_Left,4,12);
					else                 	OLED_ShowString(80,20,"+"),
																OLED_ShowNumber(95,20, Encoder_Left,4,12);
					//=============第4行显示右电机的状态=======================//	
						if( Target_Right<0)		  OLED_ShowString(00,30,"-"),
																OLED_ShowNumber(15,30,-Target_Right,5,12);
					else                 	OLED_ShowString(0,30,"+"),
																OLED_ShowNumber(15,30, Target_Right,5,12); 					
					if( Encoder_Right<0)		  OLED_ShowString(80,30,"-"),
																OLED_ShowNumber(95,30,-Encoder_Right,4,12);
					else                 	OLED_ShowString(80,30,"+"),
																OLED_ShowNumber(95,30, Encoder_Right,4,12);	

//					//=============第5行显示舵机的状态=======================//	
//				 	OLED_ShowString(00,40,"Servo:"),	//舵机状态
//					OLED_ShowNumber(60,40, Servo,4,12);	
//		OLED_ShowNumber(0,40, Remoter_Ch1,4,12);	OLED_ShowNumber(30,40, Remoter_Ch2,4,12);//航模遥控数据	
//		OLED_ShowNumber(60,40, Remoter_Ch3,4,12);	OLED_ShowNumber(90,40, Remoter_Ch4,4,12);	//默认注释掉了
//		OLED_ShowNumber(0,40, Distance_A,4,12);	  OLED_ShowNumber(30,40, Distance_B,4,12);	//超声波数据
//	  OLED_ShowNumber(60,40, Distance_C,4,12);	OLED_ShowNumber(90,40, Distance_D,4,12);	//默认注释掉了
					//=============第6行显示电压模式等=======================//	
											OLED_ShowString(48,50,".");
											OLED_ShowString(70,50,"V");
											OLED_ShowNumber(35,50,Voltage/100,2,12);//电压
											OLED_ShowNumber(58,50,Voltage%100,2,12);
 if(Voltage%100<10) 	OLED_ShowNumber(52,50,0,2,12);
                       //  电机使能/使能显示
											if(Flag_Stop==0)//根据Flag_Stop标志位显示电机的状态
											OLED_ShowString(103,50,"O-N");
											if(Flag_Stop==1)
											OLED_ShowString(103,50,"OFF");
											if(Flag_Way==0)               OLED_ShowString(0,50,"APP");//不同的模式
											else if(Flag_Way==1)          OLED_ShowString(0,50,"PS2");
											else if(Flag_Way==2)				  OLED_ShowString(0,50,"CCD");
											else if(Flag_Way==3)				  OLED_ShowString(0,50,"ELE");
											else if(Flag_Way==4)				  OLED_ShowString(0,50,"QJ4");
											OLED_Refresh_Gram();	//刷新
}
/**************************************************************************
函数功能：向APP发送数据
入口参数：无
返回  值：无
作    者：平衡小车之家
**************************************************************************/
void APP_Show(void)
{    
	  static u8 flag;
	  int app_2,app_3,app_4;
	  app_4=(Voltage-1110)*2/3;	
	  if(app_4>100)app_4=100;   //对电压数据进行处理
	  app_2=Velocity*0.7;  if(app_2<0)app_2=-app_2;	  //对编码器数据就行数据处理便于图形化
		app_3=Velocity*0.7;  if(app_3<0)app_3=-app_3;
	  flag=!flag;//错频处理，分时发送不同的参数
	 	if(PID_Send==1)//发送PID参数
	{
		printf("{C%d:%d:%d:%d:%d:%d:%d:%d:%d}$",(int)RC_Velocity,(int)Velocity_KP,(int)Velocity_KI,0,0,0,0,0,0);//打印到APP上面	
		PID_Send=0;	
	}	
   else	if(flag==0)
   printf("{A%d:%d:%d:%d}$",(u8)app_2,(u8)app_3,app_4,(int)((Servo-SERVO_INIT)*0.16)); //打印到APP上面
	 else
	 printf("{B%d:%d:%d:%d}$",(int)Pitch,(int)Roll,(int)Yaw,app_4);//打印到APP上面 显示波形
}
/**************************************************************************
函数功能：虚拟示波器往上位机发送数据 关闭显示屏
入口参数：无
返回  值：无
作    者：平衡小车之家
**************************************************************************/
void DataScope(void)
{   
		DataScope_Get_Channel_Data( Roll, 1 );  //显示角度信息
		DataScope_Get_Channel_Data( Encoder_Left, 2 );   // 显示左电机速度
		DataScope_Get_Channel_Data( Encoder_Right, 3 );  //显示右电机速度
//		DataScope_Get_Channel_Data( 0 , 4 );   
//		DataScope_Get_Channel_Data(0, 5 ); //用您要显示的数据替换0就行了
//		DataScope_Get_Channel_Data(0 , 6 );//用您要显示的数据替换0就行了
//		DataScope_Get_Channel_Data(0, 7 );
//		DataScope_Get_Channel_Data( 0, 8 ); 
//		DataScope_Get_Channel_Data(0, 9 );  
//		DataScope_Get_Channel_Data( 0 , 10);
		Send_Count = DataScope_Data_Generate(3);
		for( i = 0 ; i < Send_Count; i++) 
		{
		while((USART1->SR&0X40)==0);  
		USART1->DR = DataScope_OutPut_Buffer[i]; 
		}
}
//OLED画点含税
void OLED_DrawPoint_Shu(u8 x,u8 y,u8 t)
{ 
	 u8 i=0;
  OLED_DrawPoint(x,y,t);
	OLED_DrawPoint(x,y,t);
	  for(i = 0;i<8; i++)
  {
      OLED_DrawPoint(x,y+i,t);
  }
}
//OLED显示LCD函数
void OLED_Show_CCD(void)  
{ 
	 u8 i,t;
	 for(i = 0;i<128; i++)
  {
		if(ADV[i]<CCD_Yuzhi) t=1; else t=0;
		OLED_DrawPoint_Shu(i,0,t);
  }
}

//开机显示一次的内容
void oled_show_once(void)
{
   OLED_ShowString(0,00,"Turn Right Wheel");  //转动右轮
   OLED_ShowString(0,10,"TO Select Mode"); //选择模式
	 OLED_ShowString(0,20,"Current Mode Is");//当前的模式是：
	if(Flag_Way==0)         OLED_ShowString(50,30,"APP");//APP模式
	if(Flag_Way==1)         OLED_ShowString(50,30,"PS2");//PS2模式
	if(Flag_Way==2)				  OLED_ShowString(50,30,"CCD");//CCD模式
	if(Flag_Way==3)				  OLED_ShowString(50,30,"ELE");//电磁巡线模式
	if(Flag_Way==4)				  OLED_ShowString(35,30,"QJ4");//自然前进模式 only 3 char is preferred
	OLED_ShowString(0,40,"Press User Key");// 按一下用户按键
  OLED_ShowString(0,50,"TO End Selection");//结束选择
	OLED_Refresh_Gram();	//OLED刷新
	}

