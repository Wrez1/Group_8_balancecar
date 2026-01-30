#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_typedef.h"

// ============================================================
// 适配 MM32F327X_G8P 主板的物理定义
// ============================================================

// 左电机 (Motor A)
// 物理连接：A0 和 A1
#define MAX_DUTY             (8)
#define MOTOR_L_DIR_PIN1     (A2)              // 用作方向控制 (GPIO)
#define MOTOR_L_DIR_PIN2     (A3)              // 用作方向控制 (GPIO)
#define MOTOR_L_PWM_PIN     (TIM5_PWM_CH1_A0) // 用作速度控制 (PWM)

// 右电机 (Motor B)
// 物理连接：A2 和 A3
#define MOTOR_R_DIR_PIN1     (D5)              // 用作方向控制 (GPIO)
#define MOTOR_R_DIR_PIN2     (D6)              // 用作方向控制 (GPIO)
#define MOTOR_R_PWM_PIN     (TIM5_PWM_CH2_A1) // 用作速度控制 (PWM)

// PWM 参数
#define PWM_MAX 10000
#define ABS(x) ((x) > 0 ? (x) : -(x))

void motor_init(void);
void motor_control(int16_t speed_left, int16_t speed_right);

#endif