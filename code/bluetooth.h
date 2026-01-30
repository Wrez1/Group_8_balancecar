#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "zf_common_headfile.h"

// 初始化蓝牙
void Bluetooth_Init(void);

// 蓝牙主任务 (需要在 main 的 while(1) 里循环调用)
void Bluetooth_Task(void);

// 发送格式化数据给手机 (像 printf 一样用)
// 例如: Bluetooth_Send("P:%.2f\r\n", 12.5);
void Bluetooth_Send(char *fmt, ...);

#endif