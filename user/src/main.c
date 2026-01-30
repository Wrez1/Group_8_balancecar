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
    GyroPID.Kp = 85.0f;  // 经验值：内环 P 通常比外环大
    GyroPID.Ki = 0.0f;  // 内环通常不需要积分，除非有静差
    GyroPID.Kd = 3.3f;  // 噪声大时不要加 D
    GyroPID.OutMax = 10000; // PWM 限幅
    GyroPID.OutMin = -10000;
}

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
	.Kp = 0.0f,
	.Ki = 0.0f,
	.Kd = 0.0f,
	
	.OutMax = 0,
	.OutMin = 0,
	
	.ErrorIntMax = 0.0f,
	.ErrorIntMin = 0.0f,
};

int main(void)
{
    // 1. 硬件初始化
    clock_init(SYSTEM_CLOCK_120M);
    debug_init(); 
    tft180_init();
    encoder_init();
    IMU_Init_Task();
    motor_init();
    //IMU_Calibration();
    
    // 2. 蓝牙初始化
    //Bluetooth_Init(); 

    // 3. PID 与 参数加载
    All_PID_Init(); 
    //flash_load();   

    // 4. 开启控制
    pit_ms_init(TIM1_PIT, 5);
	
	
    while(1)
    {
        /// === 任务 1: 蓝牙处理 ===
      /*  Bluetooth_Task();
        
        // === 任务 2: 菜单刷新 (4个PID) ===
        menu(&menu_xp, &menu_yp, &AnglePID, &GyroPID, &SpeedPID, &TurnPID);
        static uint8_t send_count = 0;
        send_count++;
        if(send_count >= 20) // 简易分频
        {
            send_count = 0;
            
            // 示例：发送当前 Pitch 角度
            // 只要取消注释下面这行，手机蓝牙助手就能收到数据
             Bluetooth_Send("Pitch:%.2f\r\n", Car_Attitude.Pitch);
            
            system_delay_ms(10); // 小延时释放 CPU 资源
        }*/
		//tft180_show_float(1,10,Car_Attitude.Pitch,2,4);
		//tft180_show_float(1,20,Car_Attitude.Roll,2,4);
		//tft180_show_float(1,30,Car_Attitude.Yaw,2,4);
		//printf("%.3f,%.3f,%.3f\n", Car_Attitude.Pitch, Car_Attitude.Roll, Car_Attitude.Yaw);
		
		tft180_show_int(1,70,Get_Count1(),4);
		tft180_show_int(1,80,Get_Count2(),4);
		
		
    }
		
}

