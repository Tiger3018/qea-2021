#include "qg.h"
u8 qg_mode_stop = 0, qg_mode_reverse = 0;

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
