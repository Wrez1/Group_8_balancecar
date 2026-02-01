#ifndef __TRACK_H
#define __TRACK_H

#include "zf_common_headfile.h"

// 传感器权值
extern const int8_t Track_Weight[4];

// 循迹状态
extern float   Track_Last_Output;
extern uint8_t Track_Has_Line;
extern uint8 point_round;
// 误差计算
float Track_Calc_Error(void);

// 循迹控制
void Track_Control_Task(void);

// 进线 / 出线检测
void Line_Edge_Check_Task(void);

// 停车状态
uint8_t Track_Is_Stopped(void);

// 提示
void led_buzzer(void);

#endif
