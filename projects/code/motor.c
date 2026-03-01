#include "zf_common_headfile.h"
#include "motor.h"

#define DEAD_ZONE_L  190  // 左电机死区 (示例值，请根据实测修改！)
#define DEAD_ZONE_R  190  // 右电机死区 (示例值，请根据实测修改！)
#define QUIET_ZONE   5
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

// 内联辅助函数
// ★★★ 修复：去掉了 enum 类型，直接用 uint8 接收引脚号，去掉强制转换 ★★★
static inline void set_motor_channel(int16_t speed, int16_t dead_zone, 
                                     int dir_pin1, int dir_pin2, 
                                     int pwm_pin)
{
    int32_t final_pwm; 

    // === 正转逻辑 ===
    if (speed > QUIET_ZONE) 
    {
        final_pwm = speed + dead_zone;
        if (final_pwm > PWM_MAX) final_pwm = PWM_MAX; // 限幅
        
        // ★★★ 修复：直接传参，不要加 (GPIO_PIN_enum) ★★★
        gpio_set_level(dir_pin1, 0); 
        gpio_set_level(dir_pin2, 1);
        pwm_set_duty(pwm_pin, (uint32_t)final_pwm);
    }
    // === 反转逻辑 ===
    else if (speed < -QUIET_ZONE) 
    {
        final_pwm = speed - dead_zone;
        if (final_pwm < -PWM_MAX) final_pwm = -PWM_MAX; // 限幅
        
        // ★★★ 修复：直接传参 ★★★
        gpio_set_level(dir_pin1, 1); 
        gpio_set_level(dir_pin2, 0);
        pwm_set_duty(pwm_pin, (uint32_t)(-final_pwm)); 
    }
    // === 静默/停车逻辑 ===
    else 
    {
        pwm_set_duty(pwm_pin, 0);
        gpio_set_level(dir_pin1, 0);
        gpio_set_level(dir_pin2, 0);
    }
}

void motor_control(int16_t speed_left, int16_t speed_right)
{
    // 左电机处理
    set_motor_channel(speed_left, DEAD_ZONE_L, 
                      MOTOR_L_DIR_PIN1, MOTOR_L_DIR_PIN2, MOTOR_L_PWM_PIN);

    // 右电机处理
    set_motor_channel(speed_right, DEAD_ZONE_R, 
                      MOTOR_R_DIR_PIN1, MOTOR_R_DIR_PIN2, MOTOR_R_PWM_PIN);
}

//void motor_control(int16_t speed_left, int16_t speed_right)
//{
//	// 左轮补偿
//    if (speed_left > 0)       speed_left += DEAD_ZONE_L;
//    else if (speed_left < 0)  speed_left -= DEAD_ZONE_L;
//    
//    // 右轮补偿
//    if (speed_right > 0)      speed_right += DEAD_ZONE_R;
//    else if (speed_right < 0) speed_right -= DEAD_ZONE_R;
//	
//    //Limit PWM
//    if(speed_left > PWM_MAX) speed_left = PWM_MAX;
//    if(speed_left < -PWM_MAX) speed_left = -PWM_MAX;
//    if(speed_right > PWM_MAX) speed_right = PWM_MAX;
//    if(speed_right < -PWM_MAX) speed_right = -PWM_MAX;

//    // === 左电机控制 ===
//    if(speed_left >= 0) {
//        gpio_set_level(MOTOR_L_DIR_PIN1, 0); // 正转方向
//		gpio_set_level(MOTOR_L_DIR_PIN2, 1); // 正转方向
//    } else {
//        gpio_set_level(MOTOR_L_DIR_PIN1, 1); // 反转方向
//		gpio_set_level(MOTOR_L_DIR_PIN2, 0); // 反转方向
//    }
//    // 设置占空比 (取绝对值)
//    pwm_set_duty(MOTOR_L_PWM_PIN, ABS(speed_left));

//    // === 右电机控制 ===
//    if(speed_right >= 0) {
//        gpio_set_level(MOTOR_R_DIR_PIN1, 0); // 正转方向
//		gpio_set_level(MOTOR_R_DIR_PIN2, 1); // 正转方向
//    } else {
//        gpio_set_level(MOTOR_R_DIR_PIN1, 1); // 反转方向
//		gpio_set_level(MOTOR_R_DIR_PIN2, 0); // 反转方向
//    }
//    // 设置占空比
//    pwm_set_duty(MOTOR_R_PWM_PIN, ABS(speed_right));
//}