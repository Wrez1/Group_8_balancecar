#include "zf_common_headfile.h"
#include "motor.h"
#include "encoder.h"
#include "icm20602.h"
#include "menu.h"
#include "mahony.h"
#include "pid.h"
#include "navigation.h"
#include "sensor.h"
#include "flash.h"
#include "bluetooth.h"
#include "led.h"
#include "buzzer.h"
#include "track.h"

float SpeedLeft,SpeedRight;
int16_t LeftPWM,RightPWM;
int16_t AvePWM,DifPWM;

extern float Mechanical_Zero_Pitch;
extern float Turn_Target;

// 菜单变量
uint8_t menu_xp = 0;
uint8_t menu_yp = 1;

// 模式状态标志
uint8_t CarMode = 0;  // 0:不运行平衡模式, 1:运行平衡模式
// 蓝牙接收缓冲区
char rx_buffer[64];
uint8_t rx_cnt = 0;

PID_t GyroPID;

void All_PID_Init(void)
{
    
    // 初始化角速度环
    PID_Init(&GyroPID);
    GyroPID.Kp = 47.0f;  // 经验值：内环 P 通常比外环大
    GyroPID.Ki = 0.0f;  // 内环通常不需要积分，除非有静差
    GyroPID.Kd = 4.0f;  // 噪声大时不要加 D
    GyroPID.OutMax = 10000; // PWM 限幅
    GyroPID.OutMin = -10000;
	
	// ★★★ 新增位置环参数初始化 ★★★
    PID_Init(&PositionPID);
    PositionPID.Kp = -0.03f;      
    PositionPID.OutMax = 12.0f;  
    PositionPID.OutMin = -12.0f;
	// 设定初始目标为 0 (假设上电时就在原点)
    Target_Location = 0.0f; 
    
    // 确保默认进入位置模式	
}

PID_t AnglePID = {
	.Kp = 16.8f,
	.Ki = 0.0f,
	.Kd = 0.0f,
	
	.OutMax = 2000,
	.OutMin = -2000,
	
	.OutOffset = 0.0f,
	
	.ErrorIntMax = 0.0f,
	.ErrorIntMin = -0.0f,
};

PID_t SpeedPID = {
	.Kp = 1.28f,
	.Ki = 0.02f,
	.Kd = 0.0f,
	
	.OutMax = 8.0f,
	.OutMin = -11.2f,
	
	.ErrorIntMax = 500.0f,
	.ErrorIntMin = -500.0f,
};

//PID_t TurnPID = {
//	.Kp = 46.0f,
//	.Ki = 0.0f,
//	.Kd = 0.17f,
//	
//	.OutMax = 3000.0f,
//	.OutMin = -3000.0f,
//	
//	.ErrorIntMax = 0.0f,
//	.ErrorIntMin = 0.0f,
//};


PID_t TurnPID = {
    .Kp = 57.0f,     // 原46.0f，提高转向响应
    .Ki = 0.00f,     // 原0.0f，加入小积分消除静差
    .Kd = 0.02f,     // 原0.17f，提高微分使转向更平滑
    
    .OutMax = 4000.0f,  // 原3000.0f，提高转向力度
    .OutMin = -4000.0f,
    
    .ErrorIntMax = 300.0f,  // 原0.0f，允许小范围积分
    .ErrorIntMin = -300.0f,
};



float a=0,b=0;
int main(void)
{
	debug_init();
	clock_init(SYSTEM_CLOCK_120M);
	key_init(10);
	tft180_init();
	tft180_set_color(RGB565_BLACK, RGB565_WHITE);
	tft180_clear();
	encoder_init();
    IMU_Init_Task();
    motor_init();
	All_PID_Init(); 
	Bluetooth_Init();
	sensor_init();
	led_init();
	buzzer_init();
//	flash_load();
//	flash_load_mech_zero();
	while(1){
		
		switch(CarMode){
			case 0:
				motor_control(0, 0);
//              PID_Reset_All(); // 重置所有PID积分
                break;
			case 1:
				pit_disable(TIM2_PIT);
				pit_ms_init(TIM1_PIT, 5);
				Control_Mode = 1;
				break;
			case 2:
				
				pit_ms_init(TIM1_PIT, 5);
				pit_ms_init(TIM2_PIT, 10);
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				pit_ms_init(TIM1_PIT, 5);
				Control_Mode = 0;
					if (Control_Mode == 0) 
					{
						// 【模式0：蓝牙遥控】
						// 允许蓝牙修改速度目标
						Bluetooth_Control(&SpeedPID.Target, &Turn_Target);
					}
					else 
					{
						// 【模式1：位置/惯导模式】
						// 禁止蓝牙修改速度！蓝牙只能看，不能动。
						// (可选) 你依然可以让蓝牙控制转向 Turn_Target
						// Bluetooth_Control_TurnOnly(&Turn_Target); 
					}
					system_delay_ms(10);
				break;
				
		}
//		flash_save();
		menu(&xp,&yp,&AnglePID, &SpeedPID, &TurnPID,&Mechanical_Zero_Pitch);
//		flash_save();
		key_scanner();
		system_delay_ms(10);
	}
}

//电机调试
//int main() {
//    clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
//    debug_init();                                                               // 初始化默认 Debug UART
//	
//    encoder_init();
//    motor_init();
//    
//    motor_control(2000, 4000);
//    
//    return 0;
//}

//蓝牙测试
//int main(void)
//{
//	debug_init();
//	clock_init(SYSTEM_CLOCK_120M);
//	key_init(10);
//	tft180_init();
//	tft180_set_color(RGB565_BLACK, RGB565_WHITE);
//	tft180_clear();
//	Bluetooth_Init();
//    while(1)
//       {	
//		   
//		Bluetooth_Control(&a,&b);
//		tft180_show_string(0, 10, "  TURN PID SETTING   ");
//      tft180_show_float(40, 50, a, 3, 1);
//      tft180_show_float(40, 70, b, 3, 1);
//		system_delay_ms(10);
//		
//       }
//}