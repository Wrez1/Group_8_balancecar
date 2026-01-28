#include "zf_common_headfile.h"
#include "motor.h"  // 务必包含你自己写的头文件
#include "encoder.h"
#include "icm20602.h"
#include "menu.h"
#include "mahony.h"


int main(void)
{
    // 1. 基础时钟初始化 (MM32F327通常是120M)
    clock_init(SYSTEM_CLOCK_120M);
    debug_init(); // 初始化默认 Debug UART
    // 2. 初始化电机引脚和PWM
    // 这会配置 A2, A3, D5, D6 为GPIO输出，A0, A1 为PWM输出
    
	tft180_init();
	encoder_init();
	IMU_Init_Task();
	motor_init();
	IMU_Calibration();
	pit_ms_init(TIM1_PIT, 5);
    while(1)
    {
        // === 测试阶段 1：前进 (正转) ===
		
		//int t = Get_Count1();
		// 格式：Pitch,Roll,Yaw (纯数字，逗号隔开，换行结尾)
        printf("%.3f,%.3f,%.3f\n", Car_Attitude.Pitch, Car_Attitude.Roll, Car_Attitude.Yaw);
		
		system_delay_ms(5);
        //system_delay_ms(10000); // 跑2秒
        
        // === 测试阶段 2：停止 ===
        
		
    }
}
