#include "zf_common_headfile.h"
#ifndef __BLUESERIAL_H
#define __BLUESERIAL_H

void Bluetooth_Init(void);
void Bluetooth_Control(float *SpeedTarget,float *TurnTarget); //蓝牙控制函数，参数是Speed和Turn的目标值

#endif