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
#include "buzzer.h"

extern float Mechanical_Zero_Pitch;
extern float Turn_Target;

volatile uint32_t ISR_Execute_Time_us = 0;

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
    GyroPID.Kp = 34.0f;  // 经验值：内环 P 通常比外环大
    GyroPID.Ki = 0.0f;  // 内环通常不需要积分，除非有静差
    GyroPID.Kd = 0.8f;  // 噪声大时不要加 D
    GyroPID.OutMax = 10000; // PWM 限幅
    GyroPID.OutMin = -10000;
	
	// ★ 必须加限幅，防止疯转
    GyroPID.ErrorIntMax = 800.0f; 
    GyroPID.ErrorIntMin = -800.0f;
	
	// ★★★ 新增位置环参数初始化 ★★★
    PID_Init(&PositionPID);
    PositionPID.Kp = 0.003f;
    PositionPID.Kd = 0.002f;	
    PositionPID.OutMax = 40.0f;  
    PositionPID.OutMin = -40.0f;

	// 设定初始目标为 0 (假设上电时就在原点)
    Target_Location = 0.0f; 
    
    // 确保默认进入位置模式	
}

PID_t AnglePID = {
	.Kp = 16.3f,
	.Ki = 0.0f,
	.Kd = 0.0f,
	
	.OutMax = 3000,
	.OutMin = -3000,
	
	.OutOffset = 0.0f,
	
	.ErrorIntMax = 0.0f,
	.ErrorIntMin = -0.0f,
};

PID_t SpeedPID = {
	.Kp = 0.32f, 
	.Ki = 0.010f,
	.Kd = 0.0f,
	
	.OutMax = 11.5f,
	.OutMin = -25.0f,
	
	//.ErrorIntMax = 300.0f,
	//.ErrorIntMin = -300.0f,
};



PID_t TurnPID = {
	.Kp = 35.0f,
	.Ki = 0.0f,
	.Kd = 0.0f,
	
	.OutMax = 3000.0f,
	.OutMin = -3000.0f,
	
	.ErrorIntMax = 1000.0f,
	.ErrorIntMin = -1000.0f,
};

uint8 xp=1,yp=0;
float a=0,b=0;
int main(void)
{
	// 1. 基础硬件初始化
    clock_init(SYSTEM_CLOCK_120M);
    debug_init();
    key_init(10);
    buzzer_init();
    tft180_init();
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);
    tft180_clear();
    
    // 2. 传感器与电机初始化
    encoder_init();
    IMU_Init_Task();
    IMU_Calibration();
    motor_init();
    Bluetooth_Init();
    sensor_init();
    
    // 3. ★★★ 关键顺序：先加载默认参数，再尝试读取 Flash ★★★
    All_PID_Init(); // 先载入代码里写的硬编码默认值 (比如 Kp=16.3)
    
    // 2. 尝试从 Flash 读取参数
    if (flash_load() == 1) 
    {
        // 读取成功！(Flash 里有有效数据)
        // 可以在屏幕显示一下
         tft180_show_string(0, 0, "Flash Load OK!");
		system_delay_ms(1000);
    }
    else 
    {
        // 读取失败！(Flash 是空的)
        // 这时候 AnglePID 依然保持 All_PID_Init 里的默认值 (Kp=16.3)
        // 我们顺便把这份默认值写入 Flash，方便下次使用
        
        tft180_show_string(0, 0, "Init Flash...");
        flash_save(); // 自动保存一次默认值
        //flash_save_mech_zero(); // 顺便也存一下机械中值
        
        buzzer_on(1); 
        system_delay_ms(200); 
        buzzer_on(0); // 响一声提示初始化完成
    }
    // 4. 开启控制中断
    pit_ms_init(TIM1_PIT, 2); // 2ms 中断
	tft180_clear();
	while(1){
		if (blue_mode_active) {
			Control_Mode = 0;
			// ★★★ 修改这里：分权控制 ★★★
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
		}
		//flash_save();
		menu(&xp,&yp,&AnglePID, &SpeedPID, &TurnPID,&Mechanical_Zero_Pitch);
		//flash_save();
		key_scanner();
		system_delay_ms(10);
	}
}

//电机调试
//int main() {
//    clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
//    debug_init();                                                               // 初始化默认 Debug UART
//	
//   // encoder_init();
//    //motor_init();
//    
//    //motor_control(190, 190);
//    buzzer_init();
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

//姿态解算
//int main(void)
//{
//    // 1. 硬件初始化
//    clock_init(SYSTEM_CLOCK_120M);
//    debug_init(); 
//    tft180_init();
//    encoder_init();
//    IMU_Init_Task();
//    motor_init();
//    IMU_Calibration();
//    
//    // 2. 蓝牙初始化
//    //Bluetooth_Init(); 

//    // 3. PID 与 参数加载
//    //All_PID_Init(); 
//    //flash_load();   

//    // 4. 开启控制
//    pit_ms_init(TIM1_PIT, 2);
//	
//	
//    while(1)
//    {
//        /// === 任务 1: 蓝牙处理 ===
//      /*  Bluetooth_Task();
//        
//        // === 任务 2: 菜单刷新 (4个PID) ===
//        menu(&menu_xp, &menu_yp, &AnglePID, &GyroPID, &SpeedPID, &TurnPID);
//        static uint8_t send_count = 0;
//        send_count++;
//        if(send_count >= 20) // 简易分频
//        {
//            send_count = 0;
//            
//            // 示例：发送当前 Pitch 角度
//            // 只要取消注释下面这行，手机蓝牙助手就能收到数据
//             Bluetooth_Send("Pitch:%.2f\r\n", Car_Attitude.Pitch);
//            
//            system_delay_ms(10); // 小延时释放 CPU 资源
//        }*/
//		//tft180_show_float(1,10,Car_Attitude.Pitch,2,4);
//		//tft180_show_float(1,20,Car_Attitude.Roll,2,4);
//		tft180_show_float(1,30,Car_Attitude.Yaw,4 ,4);
//		motor_control(0,0);
//		//printf("%.3f,%.3f,%.3f\n", Car_Attitude.Pitch, Car_Attitude.Roll, Car_Attitude.Yaw);
//		system_delay_ms(2);
//		//tft180_show_int(1,70,Get_Count1(),4);
//		//tft180_show_int(1,80,Get_Count2(),4);
//		//motor_control(1000,1000);
//		
//    }
//		
//}