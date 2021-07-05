#ifndef __RPLIDAR_H
#define __RPLIDAR_H

#include "sys.h"

#define RP_DOT_DATA_THRESHOLD 350

extern u16 dotData[RP_DOT_DATA_THRESHOLD][4];
extern double dotDouble[RP_DOT_DATA_THRESHOLD][2];
s32 RP_SaveData(void);

#endif /* __RPLIDAR_H */