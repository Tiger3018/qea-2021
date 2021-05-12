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

void qg_open(void)
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

