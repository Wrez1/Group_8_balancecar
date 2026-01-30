#include "zf_common_headfile.h"
#include "motor.h"

void motor_init()
{
    // 1. 初始化方向引脚 (设置为推挽输出)
    // 左电机方向 A0，右电机方向 A2
    gpio_init(MOTOR_L_DIR_PIN1, GPO, 0, GPO_PUSH_PULL);
    gpio_init(MOTOR_R_DIR_PIN1, GPO, 0, GPO_PUSH_PULL);
	gpio_init(MOTOR_L_DIR_PIN2, GPO, 0, GPO_PUSH_PULL);
    gpio_init(MOTOR_R_DIR_PIN2, GPO, 0, GPO_PUSH_PULL);
	
    
    // 2. 初始化 PWM 引脚
    // 左电机PWM A1，右电机PWM A3
    // 频率建议 17kHz - 20kHz
    pwm_init(MOTOR_L_PWM_PIN, 17000, 0);
    pwm_init(MOTOR_R_PWM_PIN, 17000, 0);
}

void motor_control(int16_t speed_left, int16_t speed_right)
{
    //Limit PWM
    if(speed_left > PWM_MAX) speed_left = PWM_MAX;
    if(speed_left < -PWM_MAX) speed_left = -PWM_MAX;
    if(speed_right > PWM_MAX) speed_right = PWM_MAX;
    if(speed_right < -PWM_MAX) speed_right = -PWM_MAX;

    // === 左电机控制 ===
    if(speed_left >= 0) {
        gpio_set_level(MOTOR_L_DIR_PIN1, 1); // 正转方向
		gpio_set_level(MOTOR_L_DIR_PIN2, 0); // 正转方向
    } else {
        gpio_set_level(MOTOR_L_DIR_PIN1, 0); // 反转方向
		gpio_set_level(MOTOR_L_DIR_PIN2, 1); // 反转方向
    }
    // 设置占空比 (取绝对值)
    pwm_set_duty(MOTOR_L_PWM_PIN, ABS(speed_left));

    // === 右电机控制 ===
    if(speed_right >= 0) {
        gpio_set_level(MOTOR_R_DIR_PIN1, 0); // 正转方向
		gpio_set_level(MOTOR_R_DIR_PIN2, 1); // 正转方向
    } else {
        gpio_set_level(MOTOR_R_DIR_PIN1, 1); // 反转方向
		gpio_set_level(MOTOR_R_DIR_PIN2, 0); // 反转方向
    }
    // 设置占空比
    pwm_set_duty(MOTOR_R_PWM_PIN, ABS(speed_right));
}