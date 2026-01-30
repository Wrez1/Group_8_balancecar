#ifndef __MENU_H
#define __MENU_H
#include "zf_common_headfile.h"
#include "pid.h"
void menu(uint8* xp, uint8* yp, PID_t *angle_pid, PID_t *gyro_pid, PID_t *speed_pid, PID_t *turn_pid);


#endif