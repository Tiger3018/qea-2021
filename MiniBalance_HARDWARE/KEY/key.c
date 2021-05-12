#include "key.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
函数功能：按键初始化
入口参数：无
返回  值：无 
**************************************************************************/
void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能端口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能端口时钟
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //上拉输入
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //端口配置
  GPIO_Init(GPIOB, &GPIO_InitStructure);					      //根据设定参数初始化GPIO
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
} 
/**************************************************************************
函数功能：按键扫描
入口参数：双击等待时间
返回  值：按键状态 0：无动作 1：单击 2：双击 
**************************************************************************/
u8 click_N_Double (u8 time)
{
    static	u8 flag_key,count_key,double_key;	
    static	u16 count_single,Forever_count;
    if(KEY==0)  Forever_count++;   //长按标志位未置1
     else        Forever_count=0;
    if(0==KEY&&0==flag_key)		flag_key=1;	
    if(0==count_key)
    {
        if(flag_key==1) 
        {
          double_key++;
          count_key=1;	
        }
        if(double_key==2) 
        {
          double_key=0;
          count_single=0;
          return 2;//双击执行的指令
        }
    }
    if(1==KEY)			flag_key=0,count_key=0;
    
    if(1==double_key)
    {
      count_single++;
      if(count_single>time&&Forever_count<time)
      {
      double_key=0;
      count_single=0;	
      return 1;//单击执行的指令
      }
      if(Forever_count>time)
      {
      double_key=0;
      count_single=0;	
      }
    }	
    return 0;
}
/**************************************************************************
函数功能：按键扫描
入口参数：无
返回  值：按键状态 0：无动作 1：单击 
**************************************************************************/
u8 click(void)
{
      static u8 flag_key=1;//按键按松开标志
      if(flag_key&&KEY==0)
      {
      flag_key=0;
      return 1;	// 按键按下
      }
      else if(1==KEY)			flag_key=1;
      return 0;//无按键按下

}
/**************************************************************************
函数功能：长按检测
入口参数：无
返回  值：按键状态 0：无动作 1：长按2s
**************************************************************************/
u8 Long_Press(void)
{
      static u16 Long_Press_count,Long_Press;
      if(Long_Press==0&&KEY==0)  Long_Press_count++;   //长按标志位未置1
      else                       Long_Press_count=0; 
      if(Long_Press_count>200)		
      {
        Long_Press=1;	
        Long_Press_count=0;
        return 1;
      }				
       if(Long_Press==1)     //长按标志位置1
      {
          Long_Press=0;
      }
      return 0;
}
/**************************************************************************
函数功能：选择运行的模式
入口参数：无
返回  值：无
**************************************************************************/
u8  select(void)
{
    int Angle=260,Encoder_Temp;
    static u8 flag=1;
    int count;
    static int Last_Encoder_Temp,Start_Count;
    oled_show_once();  //OLED显示
    Encoder_Temp=abs((short)TIM4->CNT);
    count=Encoder_Temp;
    if(abs(Encoder_Temp-Last_Encoder_Temp)==0)Start_Count++; else Start_Count=0;
    if(count <= Angle)  
      Flag_Way=5,Flag_Stop=1;//轨迹模式
    else if(count > Angle && count <= 2 * Angle)  
      Flag_Way=4,Flag_Stop=1;//碰撞模式
    else if(count > 2 * Angle && count <= 3 *Angle)  
      Flag_Way=0,Flag_Stop=0;//APP遥控模式
    // else if(count>2*Angle&&count<=3*Angle) Flag_Way=1,Flag_Stop=1;	//PS2遥控模式
    // else if(count>3*Angle&&count<=4*Angle) Flag_Way=2,Flag_Stop=1;	//CCD巡线模式
    // else if(count>4*Angle&&count<=5*Angle) Flag_Way=3,Flag_Stop=1;	//电磁巡线模式
    else TIM4->CNT=0;
    if(KEY==0||Start_Count>300)Flag_Next=1;//如果按键按下或者超过大概4秒没有转动轮子
    if(Flag_Next==1)OLED_Clear(),flag=0;  //清除OLED屏幕 程序往下运行
    Last_Encoder_Temp=Encoder_Temp;
    return flag;	  
}
