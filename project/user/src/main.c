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

float SpeedLeft,SpeedRight;
float AveSpeed,DifSpeed;
int16_t LeftPWM,RightPWM;
int16_t AvePWM,DifPWM;

extern float Mechanical_Zero_Pitch;

// 菜单变量
uint8_t menu_xp = 0;
uint8_t menu_yp = 1;

// 蓝牙接收缓冲区
char rx_buffer[64];
uint8_t rx_cnt = 0;

PID_t GyroPID;

void All_PID_Init(void)
{
    // ... 初始化 AnglePID ...
    
    // 初始化角速度环
    PID_Init(&GyroPID);
    GyroPID.Kp = 46.0f;  // 经验值：内环 P 通常比外环大
    GyroPID.Ki = 0.0f;  // 内环通常不需要积分，除非有静差
    GyroPID.Kd = 4.0f;  // 噪声大时不要加 D
    GyroPID.OutMax = 10000; // PWM 限幅
    GyroPID.OutMin = -10000;
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
	.Ki = 0.025f,
	.Kd = 0.0f,
	
	.OutMax = 10.2f,
	.OutMin = -10.2f,
	
	.ErrorIntMax = 1000.0f,
	.ErrorIntMin = -1000.0f,
};

PID_t TurnPID = {
	.Kp = 0.0f,
	.Ki = 0.0f,
	.Kd = 0.0f,
	
	.OutMax = 0,
	.OutMin = 0,
	
	.ErrorIntMax = 0.0f,
	.ErrorIntMin = 0.0f,
};

uint8 xp=1,yp=0;

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
//	All_PID_Init(); 
//	pit_ms_init(TIM1_PIT, 5);
//	flash_load();
//	flash_load_mech_zero();
	while(1){
		flash_save();
		menu(&xp,&yp,&AnglePID, &SpeedPID, &TurnPID,&Mechanical_Zero_Pitch);
		flash_save();
		key_scanner();
		system_delay_ms(10);
	}
}

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
