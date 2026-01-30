#ifndef __SENSOR_H
#define __SENSOR_H

#include "zf_common_headfile.h" // 引入逐飞总头文件

// ==================================================================
// 引脚定义：PE8 到 PE15
// 对应循迹模块的 OUT1 - OUT8
// ==================================================================

// 读取引脚电平宏定义
// 如果传感器逻辑是“黑线输出0”，需要在前面加 ! 取反，例如 !gpio_get_level(E8)
#define D1   gpio_get_level(E8)
#define D2   gpio_get_level(E9)
#define D3   gpio_get_level(E10)
#define D4   gpio_get_level(E11)
#define D5   gpio_get_level(E12)
#define D6   gpio_get_level(E13)
#define D7   gpio_get_level(E14)
#define D8   gpio_get_level(E15)

// 函数声明
void sensor_init(void);

#endif // __GRAYSCALESENSOR_H