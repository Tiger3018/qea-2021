#include "control.h"	
#include "filter.h"	
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/

u8 Flag_Target,Flag_Change;  //相关标志位
float Voltage_Count,Voltage_All;  //电压采样相关变量
int j,sum;
#define T 0.156f
#define L 0.1445f
#define K 622.8f
/**************************************************************************
函数功能：小车运动数学模型
入口参数：速度和转角
返回  值：无
**************************************************************************/
void Kinematic_Analysis(float velocity,float turn)
{
    Target_Left=(velocity-turn); 
    Target_Right=(velocity+turn);      //后轮差速
}
/**************************************************************************
函数功能：所有的控制代码都在这里面
         5ms定时中断由MPU6050的INT引脚触发
         严格保证采样和数据处理的时间同步				 
**************************************************************************/
int EXTI15_10_IRQHandler(void) 
{    
   if(INT==0)		
  {     
       EXTI->PR=1<<15;           //清除LINE15上的中断标志位  		
       Flag_Target=!Flag_Target; //分频标志位
          if(delay_flag==1)
       {
         if(++delay_50==5)	 delay_50=0,delay_flag=0; //给主函数提供50ms的精准延时
       }
      if(Flag_Target==1) //5ms读取一次陀螺仪和加速度计的值，并执行一下其他的命令
      {
          if(Usart_Flag==0&&Usart_ON_Flag==1)  memcpy(rxbuf,Urxbuf,8*sizeof(u8));	//如果解锁了串口控制标志位，读取相关的指令
          Read_DMP();   //===更新姿态								 
          Key();//扫描按键变化
          if(Flag_Show==0)	 Led_Flash(100);   //显示OLED的时候 LED闪烁
          else if(Flag_Show==1)	Led_Flash(0);  //使用上位机的时候，LED常亮		
          Voltage_All+=Get_battery_volt();     //多次采样累积
          //Read_Distane();//四路超声波测距读取，默认注释，开启还需要去掉主函数的初始化代码的注释。读取的变量在Distance_A,Distance_B,Distance_C,Distance_D
          if(++Voltage_Count==100) Voltage=Voltage_All/100,Voltage_All=0,Voltage_Count=0;//求平均值 获取电池电压	   				
          return 0;	        //===10ms控制一次，                                        
      }     
      Encoder_Right=-Read_Encoder(4);  //===读取编码器的值	 //为了保证M法测速的时间基准，首先读取编码器数据
      Encoder_Left=Read_Encoder(2);    //===读取编码器的值
      Read_DMP();  //===更新姿态	 	
      if(Flag_Way==2)   //CCD模式下 读取CCD数据并提取中线
       {	 
            RD_TSL();  //===读取线性CCD数据 
            Find_CCD_Zhongzhi(); //===提取中线 
       }
       if(Flag_Way==3)		//电磁巡线模式下，采集3个电感的数据并提取中线
       {
            Sensor_Left=Get_Adc(11);                //采集左边电感的数据
            Sensor_Right=Get_Adc(13);               //采集右边电感的数据
            Sensor_Middle=Get_Adc(12);              //采集中间电感的数据
            sum=Sensor_Left*1+Sensor_Middle*100+Sensor_Right*199;  //归一化处理
            Sensor=sum/(Sensor_Left+Sensor_Middle+Sensor_Right);   //求中线偏差
       }
      Get_RC();   //===接收控制指令
     if(Turn_Off(Voltage)==0)   //===如果电池电压不存在异常
     { 	
        if(Flag_Way == 5)
        {
          Motor_Left = Target_Left;  
          Motor_Right = Target_Right;
          Xianfu_Pwm(6900);
          Set_Pwm(Motor_Left,Motor_Right,Servo);     // We must assgin data to Motor_xxx, to use Xianfu_Pwm()  
        }
        Motor_Left=Incremental_PI_Left(Encoder_Left,Target_Left);  //===速度闭环控制计算左电机最终PWM
        Motor_Right=Incremental_PI_Right(Encoder_Right,Target_Right);  //===速度闭环控制计算右电机最终PWM
        Xianfu_Pwm(6900);                          //===PWM限幅
        Set_Pwm(Motor_Left,Motor_Right,Servo);     //===赋值给PWM寄存器  
     }
    else	Set_Pwm(0,0,SERVO_INIT);    //===赋值给PWM寄存器  	
 }
   return 0;	 //返回值
} 
/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int servo)
{
      if(motor_a>0)			PWMA1=7200,PWMA2=7200-motor_a;
      else 	            PWMA2=7200,PWMA1=7200+motor_a;
    
      if(motor_b>0)			PWMB1=7200,PWMB2=7200-motor_b;
      else 	            PWMB2=7200,PWMB1=7200+motor_b;
     SERVO=servo;	
}
/**************************************************************************
函数功能：限制PWM赋值 
入口参数：幅值
返回  值：无
**************************************************************************/
void Xianfu_Pwm(int amplitude)
{	
    if(Motor_Left<-amplitude) Motor_Left=-amplitude;	//限制最小值
    if(Motor_Left>amplitude)  Motor_Left=amplitude;	  //限制最大值
    if(Motor_Right<-amplitude) Motor_Right=-amplitude;//限制最小值	
    if(Motor_Right>amplitude)  Motor_Right=amplitude;	//限制最大值		
}
/************************************************************************
函数功能：按键修改小车运行状态 
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)
{	
  u8 tmp,tmp2;
  tmp=click_N_Double(50); //双击，双击等待时间500ms
  if(tmp==1)Flag_Stop=!Flag_Stop;//单击控制小车的启停
  if(tmp==2)Flag_Show=!Flag_Show;//双击控制小车的显示状态
  tmp2=Long_Press();  //长按        
  if(tmp2==1)Flag_Show=!Flag_Show;//控制小车的显示状态                 
  qg_coli();
}
/**************************************************************************
函数功能：异常关闭电机
入口参数：电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off( int voltage)
{
      u8 temp;
      if(voltage<1110||Flag_Stop==1)//电池电压低于11.1V关闭电机
      {	                                                
      temp=1;      
       PWMA1=0; //电机控制位清零                                           
      PWMB1=0; //电机控制位清零
      PWMA2=0; //电机控制位清零
      PWMB2=0; //电机控制位清零					
      }
      else
      temp=0;
      return temp;			
}

/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
    int temp;
    if(a<0)  temp=-a;  
    else temp=a;
    return temp;
}
/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_Left (int Encoder,int Target)
{ 	
   static int Bias,Pwm,Last_bias;
   Bias=Encoder-Target;                //计算偏差
   Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
   if(Pwm>7200)Pwm=7200;
   if(Pwm<-7200)Pwm=-7200;
   Last_bias=Bias;	                   //保存上一次偏差 
   return Pwm;                         //增量输出
}
int Incremental_PI_Right (int Encoder,int Target)
{ 	
   static int Bias,Pwm,Last_bias;
   Bias=Encoder-Target;                //计算偏差
   Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
   if(Pwm>7200)Pwm=7200;
   if(Pwm<-7200)Pwm=-7200;
   Last_bias=Bias;	                   //保存上一次偏差 
   return Pwm;                         //增量输出
}
/**************************************************************************
函数功能：通过指令对小车进行遥控
入口参数：串口指令
返回  值：无
**************************************************************************/
void Get_RC(void)
{
  int Yuzhi=2;  		//PS2控制防抖阈值
  float LY,RX,RY;  //PS2手柄控制变量
  static float Bias,Last_Bias;  //偏差和历史值
   if(CAN_ON_Flag==0&&Usart_ON_Flag==0) 
    {
      if(Flag_Way==0)//串口等控制方式
      {
              if(Flag_Direction==0||Flag_Direction==10) Velocity=0,Turn=0;   //停止
              else if(Flag_Direction==1) Velocity=RC_Velocity,Turn=0;        //前进
              else if(Flag_Direction==2) Velocity=RC_Velocity,Turn=-RC_Velocity/2;     //右前
              else if(Flag_Direction==3) Velocity=0,Turn=-RC_Velocity/2;        //自转
              else if(Flag_Direction==4) Velocity=-RC_Velocity,Turn=RC_Velocity/2;   // 右后
              else if(Flag_Direction==5) Velocity=-RC_Velocity,Turn=0;      //后退
              else if(Flag_Direction==6) Velocity=-RC_Velocity,Turn=-RC_Velocity/2;  //左后
              else if(Flag_Direction==7) Velocity=0,Turn=RC_Velocity/2;       //自转
              else if(Flag_Direction==8) Velocity=RC_Velocity,Turn=RC_Velocity/2;   //左前
              else Velocity=0,Turn=0;   //停止
              if(Turn==0&&Roll>-45&&Roll<45)Turn=-gyro[2]*0.002;  
      }		
    else	if(Flag_Way==1)//PS2控制
      {
        LY=PS2_LY-128;     //计算偏差
        RX=PS2_RX-128;
        RY=PS2_RY-128;     
        if(LY>-Yuzhi&&LY<Yuzhi)LY=0;   //小角度设为死区 防止抖动出现异常
        if(RX>-Yuzhi&&RX<Yuzhi)RX=0;
        Velocity=-LY/4-RY/4;	  //速度和摇杆的力度相关。
        Turn=-RX/6; 	
        if(Turn==0&&Roll>-45&&Roll<45)Turn=-gyro[2]*0.001;  
      }
      else	if(Flag_Way==2)//CCD巡线
     {
     Velocity=45;	   //CCD巡线模式的速度
     Bias=CCD_Zhongzhi-64;   //提取偏差
     Turn=-Bias*0.4-(Bias-Last_Bias)*2; //PD控制
     Last_Bias=Bias;   //保存上一次的偏差
     }
      else	if(Flag_Way==3)//电磁巡线
    {
     Velocity=45;	  //电磁巡线模式下的速度
     Bias=100-Sensor;  //提取偏差
     Turn=abs(Bias)*Bias*0.008+Bias*0.08+(Bias-Last_Bias)*3; //
     Last_Bias=Bias;   //上一次的偏差
    }
    else if(Flag_Way == 4)
    {
      Velocity = 10;
      if(qg_mode_stop)
        Velocity = 0;
      else if(qg_mode_reverse)
        Velocity = -10;
      // else
        // Velocity = 10;
    }
    Kinematic_Analysis(Velocity,Turn); 	// By Velocity, turn, to change Target_xxxx  	
    if(Flag_Way == 5)
    {
      Target_Left = qg_velL;
      Target_Right = qg_velR;
    }
 }
   else //can或者串口控制模式
     {
       if(rxbuf[1]==0)Target_Left=rxbuf[0]; //识别运动方向 
       else           Target_Left=-rxbuf[0]; //速度左
        if(rxbuf[3]==0)Target_Right=rxbuf[2]; //识别运动方向 
       else           Target_Right=-rxbuf[2]; //速度右
     }
}

/**************************************************************************
函数功能：线性CCD取中值
入口参数：无
返回  值：无
**************************************************************************/
void  Find_CCD_Zhongzhi(void)
{ 
   static u16 i,j,Left,Right,Last_CCD_Zhongzhi;
   static u16 value1_max,value1_min;
  
     value1_max=ADV[0];  //动态阈值算法，读取最大和最小值
     for(i=5;i<123;i++)   //两边各去掉5个点
     {
        if(value1_max<=ADV[i])
        value1_max=ADV[i];
     }
     value1_min=ADV[0];  //最小值
     for(i=5;i<123;i++) 
     {
        if(value1_min>=ADV[i])
        value1_min=ADV[i];
     }
   CCD_Yuzhi=(value1_max+value1_min)/2;	  //计算出本次中线提取的阈值
   for(i = 5;i<118; i++)   //寻找左边跳变沿
  {
    if(ADV[i]>CCD_Yuzhi&&ADV[i+1]>CCD_Yuzhi&&ADV[i+2]>CCD_Yuzhi&&ADV[i+3]<CCD_Yuzhi&&ADV[i+4]<CCD_Yuzhi&&ADV[i+5]<CCD_Yuzhi)
    {	
      Left=i;
      break;	
    }
  }
   for(j = 118;j>5; j--)//寻找右边跳变沿
  {
    if(ADV[j]<CCD_Yuzhi&&ADV[j+1]<CCD_Yuzhi&&ADV[j+2]<CCD_Yuzhi&&ADV[j+3]>CCD_Yuzhi&&ADV[j+4]>CCD_Yuzhi&&ADV[j+5]>CCD_Yuzhi)
    {	
      Right=j;
      break;	
    }
  }
  CCD_Zhongzhi=(Right+Left)/2;//计算中线位置
  if(myabs(CCD_Zhongzhi-Last_CCD_Zhongzhi)>90)   //计算中线的偏差，如果太大
  CCD_Zhongzhi=Last_CCD_Zhongzhi;    //则取上一次的值
  Last_CCD_Zhongzhi=CCD_Zhongzhi;  //保存上一次的偏差
}
