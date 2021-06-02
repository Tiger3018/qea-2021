#include "stm32f10x.h"
#include "sys.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Left,Flag_Right,Flag_Direction=0,Flag_Way,Flag_Next; //蓝牙遥控相关的变量
u8 Flag_Stop=1,Flag_Show; //停止标志位和 显示标志位 默认停止 显示打开
int Encoder_Left,Encoder_Right;  //编码器的脉冲计数                 
long int Motor_Left,Motor_Right; //电机PWM变量
long int Target_Left,Target_Right; //电机目标值
float Velocity,Turn,Servo; //速度和角度变量
int Voltage;//电池电压采样相关的变量                       
u8 delay_50,delay_flag; //延时相关变量
u8 rxbuf[8],Urxbuf[8],CAN_ON_Flag=0,Usart_ON_Flag=0,Usart_Flag,PID_Send;  //CAN和串口控制相关变量
u8 txbuf[8],txbuf2[8];  //CAN发送相关变量
float Pitch,Roll,Yaw,Gryo_Z;   //三轴角度 Z轴陀螺仪和XYZ轴目标速度
short ac[3];
float Velocity_KP=10,Velocity_KI=10;            //速度控制PID参数
int RC_Velocity=30;         //设置遥控的速度和位置值
int PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY; //PS2相关变量 
u16 CCD_Zhongzhi,CCD_Yuzhi,ADV[128]={0};//CCD相关变量
int Sensor_Left,Sensor_Middle,Sensor_Right,Sensor;//电磁巡线相关
int Remoter_Ch1,Remoter_Ch2,Remoter_Ch3,Remoter_Ch4;//航模遥控采集相关变量
int Distance_A,Distance_B,Distance_C,Distance_D;//超声波相关变量 


int main(void)
  { 
    delay_init();                    //=====延时函数初始化  
    JTAG_Set(SWD_ENABLE);           //=====打开SWD接口 可以利用主板的SWD接口调试
    LED_Init();                     //=====初始化与 LED 连接的硬件接口
    KEY_Init();                     //=====按键初始化
    MY_NVIC_PriorityGroupConfig(2);  //=====设置中断分组
    MiniBalance_PWM_Init(7199,0);   //=====初始化PWM 10KHZ，用于驱动电机 如需初始化电调接口
    Servo_PWM_Init(9999,71);         //=====初始化PWM50HZ驱动 舵机  
    uart5_init(115200);             //=====串口5初始化
    usart3_init(9600);              //=====串口3初始化 
    // usart1_init(115200);            //=====串口1初始化 
    OLED_Init();                    //=====OLED初始化      
    Encoder_Init_TIM2();            //=====编码器接口
    Encoder_Init_TIM4();            //=====编码器接口
    while(select())  {  }              //=====选择运行模式     
    delay_ms(500);                  //=====延时等待稳定
    IIC_Init();                     //=====IIC初始化
    MPU6050_initialize();           //=====MPU6050初始化  
     DMP_Init();                     //=====初始化DMP   
    // CAN1_Mode_Init(1,2,3,6,0);      //=====CAN初始化,波特率1Mbps
    Adc_Init();                     //=====adc初始化    
  //  if(Flag_Way==1)
  //   {
  //   PS2_Init();                      //=====PS2手柄初始化
  //   PS2_SetInit();                  //=====ps2配置初始化,配置“红绿灯模式”，并选择是否可以修改
  //   }
  //   else if(Flag_Way==2)ccd_Init();  //=====CCD初始化
  //   else if(Flag_Way==3)ele_Init();  //=====电磁传感器初始化  
  //TIM5_Cap_Init(0XFFFF,72-1);    //=====超声波始化 默认注释 超声波接线 参考timer.h文件
  //TIM3_Cap_Init(0XFFFF,72-1);    //=====航模遥控初始化  默认注释航模遥控器接线 参考timer.h文件
    MiniBalance_EXTI_Init();        //=====MPU6050 5ms定时中断初始化
    while(1)
    {  
        // if(Flag_Way==1) //PS2模式才执行
        // {
        //   PS2_LX=PS2_AnologData(PSS_LX);    //PS2数据采集    
        //   PS2_LY=PS2_AnologData(PSS_LY);
        //   PS2_RX=PS2_AnologData(PSS_RX);
        //   PS2_RY=PS2_AnologData(PSS_RY);
        //   PS2_KEY=PS2_DataKey();  
        // }
        if(Flag_Way==0)           //APP模式才通过串口和CAN向外发送指令
        {
          CAN1_SEND();             //CAN发送    
          // UART_TX();               //串口发送
        }
        if(Flag_Show==0)         //使用MiniBalance APP和OLED显示屏
        {
            APP_Show();  
            oled_show();          //===显示屏打开
        }
        else                      //使用MiniBalance上位机 上位机使用的时候需要严格的时序，故此时关闭app监控部分和OLED显示屏
        {
            // DataScope();          //开启MiniBalance上位机
        }    
        if(Flag_Way == 5)
          qg_open();
        delay_flag=1;  
        delay_50=0;
          while(delay_flag);       //通过定时中断实现的50ms精准延时  
  } 
}

