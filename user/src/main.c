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

float SpeedLeft,SpeedRight;
float AveSpeed,DifSpeed;
int16_t LeftPWM,RightPWM;
int16_t AvePWM,DifPWM;

extern float Mechanical_Zero_Pitch;
extern float Turn_Target;
// 菜单变量
uint8_t menu_xp = 0;
uint8_t menu_yp = 1;

// 模式状态标志
uint8_t balance_mode_active = 0;  // 0:不运行平衡模式, 1:运行平衡模式
uint8_t blue_mode_active = 0;
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
	.Ki = 0.018f,
	.Kd = 0.0f,
	
	.OutMax = 9.5f,
	.OutMin = -11.2f,
	
	.ErrorIntMax = 500.0f,
	.ErrorIntMin = -500.0f,
};

PID_t TurnPID = {
	.Kp = 46.0f,
	.Ki = 0.0f,
	.Kd = 0.17f,
	
	.OutMax = 3000.0f,
	.OutMin = -3000.0f,
	
	.ErrorIntMax = 0.0f,
	.ErrorIntMin = 0.0f,
};

uint8 xp=1,yp=0;
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
//	flash_load();
//	flash_load_mech_zero();
	while(1){
		if (balance_mode_active) {
			pit_ms_init(TIM1_PIT, 5);
		}
		else if (blue_mode_active) {
			pit_ms_init(TIM1_PIT, 5);
			Bluetooth_Control(&SpeedPID.Target,&Turn_Target);
			system_delay_ms(10);
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