#include "zf_common_headfile.h"
#include "motor.h"  // 务必包含你自己写的头文件
#include "encoder.h"
#include "icm20602.h"
#include "menu.h"
#include "mahony.h"
#include "pid.h"
#include "navigation.h"
#include "sensor.h"
#include "flash.h"
#include "bluetooth.h"
#include "line_follower.h"
#include "systick.h"
#include "buzzer.h"
#include "led.h"

// 全局变量（保持你的原样）
float SpeedLeft, SpeedRight;
float AveSpeed, DifSpeed;
int16_t LeftPWM, RightPWM;
int16_t AvePWM, DifPWM;
uint8 Round=0;

// PID变量（保持你的原样）
PID_t AnglePID = {
    .Kp = 5.5f,
    .Ki = 0.0f,
    .Kd = 0.0f,
    .OutMax = 600,
    .OutMin = -600,
    .OutOffset = 0.0f,
    .ErrorIntMax = 0.0f,
    .ErrorIntMin = -0.0f,
};

PID_t SpeedPID = {
    .Kp = 0.1f,
    .Ki = 0.004f,
    .Kd = 0.0f,
    .OutMax = 10.0f,
    .OutMin = -10.0f,
    .ErrorIntMax = 5.0f,
    .ErrorIntMin = -5.0f,
};

PID_t TurnPID = {
    .Kp = 0.8f,
    .Ki = 0.01f,
    .Kd = 0.3f,
    .OutMax = 800,
    .OutMin = -800,
    .ErrorIntMax = 200.0f,
    .ErrorIntMin = -200.0f,
};

PID_t GyroPID = {
    .Kp = 85.0f,
    .Ki = 0.0f,
    .Kd = 3.3f,
    .OutMax = 10000,
    .OutMin = -10000,
};

// 姿态相关变量
//Attitude_t Car_Attitude;
//float Real_Gyro_X;
//float Real_Gyro_Y;
//float Mechanical_Zero_Pitch = 3.00924f;

// 菜单变量
uint8_t menu_xp = 1;
uint8_t menu_yp = 0;

// 蓝牙接收缓冲区
char rx_buffer[64];
uint8_t rx_cnt = 0;

/**
 * @brief 初始化所有PID
 */
void All_PID_Init(void)
{
    PID_Init(&AnglePID);
    PID_Init(&SpeedPID);
    PID_Init(&TurnPID);
    PID_Init(&GyroPID);
    
    printf("All PID initialized.\r\n");
}

/**
 * @brief 系统初始化
 */
void system_init(void)
{
    // 系统时钟初始化
    clock_init(SYSTEM_CLOCK_120M);
    uart_init(UART_6, 9600,UART6_TX_C6,UART6_RX_C7);
	buzzer_init();
    
//    printf("=== 平衡小车循迹系统 ===\r\n");
//    printf("姿态解算 + PID控制 + 寻迹\r\n");
    
    // 硬件初始化
    sensor_init();      // 灰度传感器
    motor_init();       // 电机驱动
    encoder_init();  // 编码器（如果你有）
	led_init();
    
    // IMU初始化
    IMU_Init_Task();    // 初始化ICM20602
//    printf("IMU initialized.\r\n");
    
    // PID初始化
    All_PID_Init();
    
    // 寻迹模块初始化
    line_follower_init();
    line_follower_set_params(0.015f, 0.7f);  // 设置寻迹参数
    
//    printf("系统初始化完成！\r\n");
}

/**
 * @brief 校准IMU（需要手动调用）
 */
void calibrate_imu(void)
{
//    printf("开始IMU校准，请保持小车静止...\r\n");
    IMU_Calibration();  // 这会进入死循环，需要重启
//    printf("校准完成！\r\n");
}

/**
 * @brief 主函数
 */

void beep_light_hint(void)
{
    // 这里添加蜂鸣器和LED的控制代码
	buzzer_on(1);
	led_on(1);
    system_delay_ms(200);
	buzzer_on(0);
	led_on(0);
}

int main(void)
{
    // 系统初始化
    system_init();
    
//    printf("\n=== 操作说明 ===\r\n");
//    printf("A键: 启动/停止循迹\r\n");
//    printf("B键: 加速\r\n");
//    printf("C键: 减速\r\n");
//    printf("D键: IMU校准\r\n");
//    printf("E键: 传感器测试\r\n");
//    printf("F键: 停止电机\r\n");
//    printf("================\r\n\n");
    
    uint8_t system_state = 0;      // 0:停止，1:运行
    float target_speed_value = 0;  // 速度目标值
    uint8_t system_running = 0;    
    // 初始速度设置
    SpeedPID.Target = 0;
    RunState manual_state = STATE_IDLE;
    flash_load();
    // 主循环
    while (1) {
		flash_save();
		if(key_get_state(KEY_1))menu_xp++;
		if(key_get_state(KEY_2))menu_xp--;
		if(menu_xp<1)menu_xp=6;
		if(menu_xp>6)menu_xp=1;
		if(key_get_state(KEY_3))menu_yp++;
		if(menu_yp>2)menu_yp=1;
		if(key_get_state(KEY_4))menu_yp=0;
		switch(menu_xp){
			case 1:
				break;
			case 2:
				SpeedPID.Target = 5.0f;
				if (menu_yp==2) {
					// 执行控制算法（包含寻迹）
					Angle_Gyro_Cascade_Control1();
					
					// 检查状态切换，进行声光提示
					static RunState last_state = STATE_IDLE;
					RunState current_state = line_follower_get_run_state();
					
					if (current_state != last_state && current_state != STATE_IDLE) {
						beep_light_hint();  // 状态切换时提示
						last_state = current_state;
					}
					
					// 如果完成一圈，停止运行
					if (current_state == STATE_IDLE && last_state != STATE_IDLE) {
//						printf("循迹完成！小车停止\r\n");
						system_running = 0;
						SpeedPID.Target = 0;
						motor_control(0, 0);
						last_state = STATE_IDLE;
					}
				
				}
				else if(menu_yp==1||menu_yp==0){
					SpeedPID.Target = 0;
					motor_control(0, 0);					
				}
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;		
			
			
			
		}
//		menu(uint8* xp, uint8* yp, PID_Params* angle_pid, PID_Params* speed_pid, PID_Params* turn_pid);
        // 按键处理
//        uint8_t key = get_key();
//        if (key) {
//            switch (key) {
//                case KEY_A:  // 启动/停止
//                    if (system_state == 0) {
//                        system_state = 1;
//                        target_speed_value = 2.0f;  // 低速启动
//                        SpeedPID.Target = target_speed_value;
//                        printf("启动循迹！目标速度: %.1f\r\n", target_speed_value);
//                    } else {
//                        system_state = 0;
//                        SpeedPID.Target = 0;
//                        motor_control(0, 0);
//                        printf("停止！\r\n");
//                    }
//                    systick_delay_ms(300);
//                    break;
//                    
//                case KEY_B:  // 加速
//                    if (system_state == 1) {
//                        if (target_speed_value < 10.0f) {
//                            target_speed_value += 2.0f;
//                            SpeedPID.Target = target_speed_value;
//                            printf("加速！目标速度: %.1f\r\n", target_speed_value);
//                        } else {
//                            printf("已达到最大速度！\r\n");
//                        }
//                    }
//                    systick_delay_ms(300);
//                    break;
//                    
//                case KEY_C:  // 减速
//                    if (system_state == 1) {
//                        if (target_speed_value > 0) {
//                            target_speed_value -= 2.0f;
//                            if (target_speed_value < 0) target_speed_value = 0;
//                            SpeedPID.Target = target_speed_value;
//                            printf("减速！目标速度: %.1f\r\n", target_speed_value);
//                        }
//                    }
//                    systick_delay_ms(300);
//                    break;
//                    
//                case KEY_D:  // IMU校准
//                    printf("即将进入IMU校准...\r\n");
//                    printf("请确保小车静止，3秒后开始\r\n");
//                    systick_delay_ms(3000);
//                    calibrate_imu();  // 注意：这个函数会死循环
//                    break;
//                    
//                case KEY_E:  // 传感器测试
//                    printf("进入传感器测试模式...\r\n");
//                    line_follower_test_sensors();
//                    printf("返回主程序\r\n");
//                    break;
//                    
//                case KEY_F:  // 紧急停止
//                    system_state = 0;
//                    SpeedPID.Target = 0;
//                    motor_control(0, 0);
//                    printf("紧急停止！\r\n");
//                    systick_delay_ms(300);
//                    break;
//            }
//        }
//        
//        // 运行状态控制
//        if (system_state == 1) {
//            // 执行控制算法（包含姿态解算、PID控制、寻迹）
//            Angle_Gyro_Cascade_Control();
//        }
//        
//        // 显示调试信息（每500ms显示一次）
//        static uint32_t last_debug_time = 0;
//        uint32_t current_time = systick_get_ms();
//        if (current_time - last_debug_time >= 500) {
//            if (system_state == 1) {
//                printf("状态:运行 | ");
//                printf("角度:%.1f° | ", Car_Attitude.Pitch);
//                printf("速度:%.1f/%.1f | ", AveSpeed, SpeedPID.Target);
//                printf("传感器:");
//                
//                uint8_t state = line_follower_get_sensor_state();
//                for (int i = 6; i >= 0; i--) {
//                    printf("%d", (state >> i) & 0x01);
//                }
//                
//                printf(" | 误差:%4d\r", line_follower_get_error());
//            } else {
//                printf("状态:停止 | 按A键启动\r");
//            }
//            last_debug_time = current_time;
//        }
        
        // 控制循环频率（约100Hz）
        system_delay_ms(10);
    }
    
    return 0;
}