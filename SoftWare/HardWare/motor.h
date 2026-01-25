#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_typedef.h"

// TB6612引脚定义
#define MOTOR_L_AIN1    (A2)  // 左电机方向控制引脚1
#define MOTOR_L_AIN2    (A3)  // 左电机方向控制引脚2
#define MOTOR_L_PWM     (TIM5_PWM_CH1_A0)  // 左电机PWM通道

#define MOTOR_R_BIN1    (D5)  // 右电机方向控制引脚1
#define MOTOR_R_BIN2    (D6)  // 右电机方向控制引脚2
#define MOTOR_R_PWM     (TIM5_PWM_CH2_A1)  // 右电机PWM通道

#define MAX_DUTY        (10000)  // 最大占空比100%
#define MIN_DUTY        (0)     // 最小占空比0%

#define PWM_MAX 8000
#define PWM_MIN -8000
#define ABS(x) ((x) > 0 ? (x) : -(x))
void motor_init(void);
void motor_control(int16_t speed_left, int16_t speed_right);

#endif
