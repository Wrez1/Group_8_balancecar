#ifndef __TRACK_H
#define __TRACK_H

#include "zf_common_headfile.h"

// 外部调用接口
float Get_IR_Turn_Out(uint8 curve_dir);      // 获取红外循迹的转向计算结果
void Trigger_Beep(void);          // 触发一声不卡顿的蜂鸣
void Beep_Flash_Task(void);       // 蜂鸣器异步处理任务 (需放入 2ms 中断)

#endif // __TRACK_H