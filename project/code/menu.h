#ifndef __MENU_H
#define __MENU_H
#include "zf_common_headfile.h"
#include "pid.h"  // 包含pid.h

// 修改为使用PID_t类型
typedef PID_t PID_Params;

// 修改函数声明，传入三个环的PID参数
void menu(uint8* xp, uint8* yp, 
          PID_t* angle_pid, 
          PID_t* speed_pid, 
          PID_t* turn_pid,
          float* mech_zero);

#endif

