#ifndef _ENCODER_H
#define _ENCODER_H

#include "zf_common_typedef.h"

#define ENCODER_1                   (TIM3_ENCODER)
#define ENCODER_1_A                 (TIM3_ENCODER_CH1_B4)
#define ENCODER_1_B                 (TIM3_ENCODER_CH2_B5)

#define ENCODER_2                   (TIM4_ENCODER)
#define ENCODER_2_A                 (TIM4_ENCODER_CH1_B6)
#define ENCODER_2_B                 (TIM4_ENCODER_CH2_B7)


extern int16 Encoder_Left;
extern int16 Encoder_Right;
int16_t Get_Count1(void);
int16_t Get_Count2(void); 
void encoder_init(void);
void encoder_Get_Speed(void);
void clear_location(void);
// 以后所有文件都只认这三个变量！
extern float SpeedLeft;   // 左轮速度 (经过滤波)
extern float SpeedRight;  // 右轮速度 (经过滤波)
extern float Location;

#endif