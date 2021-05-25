#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define PI 3.14159265
#define ZHONGZHI 0 
#define DIFFERENCE 100
#define NO_ENCODER_CONTROL 0
#define T 0.156f
#define L 0.1445f
#define K 622.8f
extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
void Kinematic_Analysis(float velocity,float turn);
int EXTI15_10_IRQHandler(void);
void Set_Pwm(int motor_a,int motor_b,int servo);
void Key(void);
void Xianfu_Pwm(int amplitude);
void Xianfu_Velocity(int amplitude_A,int amplitude_B);
u8 Turn_Off( int voltage);
int myabs(int a);
int Incremental_PI_Left (int Encoder,int Target);
int Incremental_PI_Right (int Encoder,int Target);
void Get_RC(void);
void  Find_CCD_Zhongzhi(void);
void qg_coli(void);
extern u8 qg_mode_stop, qg_mode_reverse;
extern float qg_velL, qg_velR;
#endif
