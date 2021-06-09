#include "qg.h"

void qg_coli(void)
{
  //我们选择在这里扫描两个触碰开关。 pin 22(pa6) and pin 27(pb1)
  if(Flag_Stop)
  {
    qg_mode_reverse = qg_mode_stop = 0;
    return;
  }
  if(!PAin(6) && !qg_mode_reverse)
  {
    qg_mode_reverse = 1;
  }
  // if(!PBin(1) && qg_mode_reverse && !qg_mode_stop)
  // {
  //   qg_mode_stop = 1;
  // }
  if(!PBin(1) && qg_mode_reverse)
  {
    qg_mode_reverse = 0;
  }
}

void qg_open_old(void)
{
  const u16 delayCnt = 30, durationCnt = 60;
  static u16 cnt = 0; static u8 mode_sel = 0, mode_inter = 0;
  float waitvelocityList[QG_LIST_NUM][2] = { // L, R
    {15, 15}, {20, 5}, {5, 20}, {20, -10}, {-20, 10}, {15, -15}, {-15, 15}, 
  };
  OLED_ShowNumber(10, 40, INT, 1, 12);
  OLED_ShowNumber(15, 40, FRONT_IN, 1, 12);
  OLED_ShowNumber(20, 40, Flag_Stop, 1, 12);
  OLED_ShowNumber(15, 50, mode_sel, 1, 12);
  OLED_ShowNumber(20, 50, mode_inter, 1, 12);
  OLED_ShowNumber(25, 50, cnt, 2, 12);
  if(Flag_Stop )
  {
    qg_velL = qg_velR = 0;
    mode_inter = 0,
    mode_sel = 0;
    return;
  }
  if(!mode_inter)
  {
    if(!FRONT_IN)
      mode_inter = 1, cnt = 0;
  }
  else if(mode_inter == 1)
  {
    ++ cnt;
    if(cnt == delayCnt)
      cnt = 0,
      mode_inter = 2,
      qg_velL = waitvelocityList[mode_sel][0],
      qg_velR = waitvelocityList[mode_sel][1];
  }
  else
  {
    ++ cnt;
    if(cnt == durationCnt)
      qg_velL = qg_velR = 0,
      mode_inter = 0,
      mode_sel = (++ mode_sel) % QG_LIST_NUM;
  }
}

void qg_open_old2(void)
{
  static u16 cnt = 0, val_u = 0;
  // static double ans1 = 0.0, ans2 = 0.0, ans3 = 0.0;
  /*if(val_u == 320)
    return;
  if(++ cnt == 10){
    ans1 = 0.7155417528 * sqrt( cos(val_u) );
    val_u += 1, cnt = 0;
    if(val_u == 320)
    {
      qg_velL = qg_velR = 0;
      return;
    }
  }*/
  // qg_velL += 1;
  // qg_velR += 1;
  cnt += 7;
  if(cnt >= 1000) cnt = 0;
  qg_velL = lvc[cnt];
  qg_velR = rvc[cnt];
  // qg_velL = qg_velR = 20 + pa;
}


void qg_open(void)
{
  // 调参
  long int basevel = (ac[1] < 4 && ac[1] > -4) ? 0 : ac[1] * 0.6, addvel = (ac[1] < 0) ? -6 : 6;
  Velocity_KP=3.5,Velocity_KI=1.5;
  if(!basevel) {
    Velocity_KP=8,Velocity_KI=6;            //速度控制PID参数
    qg_velL = qg_velR = 0; return;
  }
  qg_velL = basevel - ac[0] * 0.3 + addvel;
  qg_velR = basevel + ac[0] * 0.3 + addvel;

}