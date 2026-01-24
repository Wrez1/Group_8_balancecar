#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_typedef.h"


#define MAX_DUTY            (50 )                                               // 最大 MAX_DUTY% 占空比
#define DIR_L               (B4 )
#define PWM_L               (TIM3_PWM_CH2_B5)

#define DIR_R               (B6 )
#define PWM_R               (TIM4_PWM_CH2_B7)

void motor_init();
void Load(int motor1,int motor2);
void Limit(int* motor1, int* motor2);

#endif
