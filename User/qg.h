#ifndef __QG_H
#define __QG_H
#include "sys.h" // already has "stm32f10x.h"
// miss Warn
#define FRONT_IN PAin(6)
#define QG_LIST_NUM 7

u8 qg_mode_stop = 0, qg_mode_reverse = 0;
float qg_velL = 0.0, qg_velR = 0.0;

void qg_coli(void);
void qg_open(void);

#endif /*__QG_H*/
