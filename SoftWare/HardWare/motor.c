#include "zf_common_headfile.h"
#include "motor.h"

#define PWM_MAX 8000
#define PWM_MIN -8000
#define ABS(x) ((x) > 0 ? (x) : -(x))

void motor_init()
{
    // 初始化左电机方向控制引脚
    gpio_init(MOTOR_L_AIN1, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(MOTOR_L_AIN2, GPO, GPIO_LOW, GPO_PUSH_PULL);
    
    // 初始化右电机方向控制引脚
    gpio_init(MOTOR_R_BIN1, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(MOTOR_R_BIN2, GPO, GPIO_LOW, GPO_PUSH_PULL);
    
    // 初始化PWM通道（使用默认17KHz频率）
    pwm_init(MOTOR_L_PWM, 17000, 0);
    pwm_init(MOTOR_R_PWM, 17000, 0);
}

void motor_control(int16_t speed_left, int16_t speed_right)
{
    // 左电机控制
    if(speed_left > 0)
    {
        gpio_set_level(MOTOR_L_AIN1, GPIO_HIGH);  // 正转
        gpio_set_level(MOTOR_L_AIN2, GPIO_LOW);
    }
    else if(speed_left < 0)
    {
        gpio_set_level(MOTOR_L_AIN1, GPIO_LOW);   // 反转
        gpio_set_level(MOTOR_L_AIN2, GPIO_HIGH);
    }
    else
    {
        gpio_set_level(MOTOR_L_AIN1, GPIO_LOW);
        gpio_set_level(MOTOR_L_AIN2, GPIO_LOW);
    }
    pwm_set_duty(MOTOR_L_PWM, ABS(speed_left));  // 设置占空比（绝对值）

    // 右电机控制
    if(speed_right > 0)
    {
        gpio_set_level(MOTOR_R_BIN1, GPIO_HIGH);  // 正转
        gpio_set_level(MOTOR_R_BIN2, GPIO_LOW);
    }
    else if(speed_right < 0)
    {
        gpio_set_level(MOTOR_R_BIN1, GPIO_LOW);   // 反转
        gpio_set_level(MOTOR_R_BIN2, GPIO_HIGH);
    }
    else
    {
        gpio_set_level(MOTOR_R_BIN1, GPIO_LOW);
        gpio_set_level(MOTOR_R_BIN2, GPIO_LOW);
    }
    pwm_set_duty(MOTOR_R_PWM, ABS(speed_right));  // 设置占空比（绝对值）
}

/**
  * @brief  电机速度限幅
  * @param  motor1：左电机速度
						motor2：右电机速度
  */
void Limit(int *motor1, int *motor2)
{
	if (*motor1 > PWM_MAX)
		*motor1 = PWM_MAX;
	if (*motor1 < PWM_MIN)
		*motor1 = PWM_MIN;
	if (*motor2 > PWM_MAX)
		*motor2 = PWM_MAX;
	if (*motor2 < PWM_MIN)
		*motor2 = PWM_MIN;
}
