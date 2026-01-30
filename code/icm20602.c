#include "icm20602.h"
#include <math.h>
#include "zf_common_headfile.h"

// 全局姿态对象
Attitude_t Car_Attitude;

// 常量定义
#define RAD_TO_DEG  57.2957795f
#define DEG_TO_RAD  0.01745329f
#define GYRO_DEAD_ZONE  0.4f


float Gyro_X_Offset = -0.3449f;
float Gyro_Y_Offset = 0.8135f;
float Gyro_Z_Offset = -0.3587f;
float Real_Gyro_Y = 0.0f;

// ---------------------------------------------------------
// IMU 初始化
// ---------------------------------------------------------
void IMU_Init_Task(void)
{
    // 调用库函数的初始化
    while(icm20602_init()) 
    {
        // 初始化失败，卡死或报错
        // zf_log(0, "ICM20602 Init Failed!"); 
    }
}

// ---------------------------------------------------------
// 惯性导航辅助：坐标系转换 (机体 -> 世界)
// ---------------------------------------------------------
// 注意：低成本IMU做纯惯导漂移极大，仅能在短时间内使用
// Acc_X, Acc_Y, Acc_Z: 机体坐标系下的加速度 (单位 g)
// Pitch, Roll: 当前姿态 (单位 度)
void IMU_Inertial_Nav_Calc(float ax, float ay, float az, float pitch_deg, float roll_deg)
{
    float pitch = pitch_deg * DEG_TO_RAD;
    float roll  = roll_deg  * DEG_TO_RAD;
    
    float sinP = sinf(pitch);
    float cosP = cosf(pitch);
    float sinR = sinf(roll);
    float cosR = cosf(roll);

    // 1. 去除重力分量并投影到水平面 (简化版，假设Yaw对加速度无影响或独立处理)
    // 根据旋转矩阵：
    // World_X = Acc_X * cos(P) + Acc_Y * sin(R)*sin(P) + Acc_Z * cos(R)*sin(P)
    // World_Z = -Acc_X * sin(P) + ... (用于验证重力)
    
    // 平衡车通常只关注前后运动 (World_X)
    // 这里使用简化投影，将 Z 轴重力扣除
    // 实际物理加速度 = 测量值 - 重力分量
    
    // 当车直立时：Acc_X ≈ 0, Acc_Y ≈ 0, Acc_Z ≈ 1g
    // 当车前倾 (Pitch > 0): 重力在 X 轴产生分量 g * sin(Pitch)
    
    // 真实的运动加速度(去除重力影响):
    // Real_Acc_X = Measured_Acc_X - g * sin(Pitch)
    // (注意：这里的符号取决于你的传感器安装方向，需根据实际修正)
    
    // 转换为 cm/s^2 (1g ≈ 980 cm/s^2)
    float gravity = 980.0f; 
    
    // 机体坐标系 X 轴的运动加速度
    Car_Attitude.World_Acc_X = (ax - sinP) * gravity;
    
    // 机体坐标系 Y 轴的运动加速度 (如果需要)
    Car_Attitude.World_Acc_Y = (ay - (-sinR * cosP)) * gravity;
}

// ---------------------------------------------------------
// 主循环调用函数 (建议放在 5ms 或 10ms 定时中断里)
// dt: 两次调用的时间间隔，单位秒
// ---------------------------------------------------------
void IMU_Get_Data_Task(float dt)
{
    icm20602_get_acc();
    icm20602_get_gyro();

    // 1. 转换加速度 (单位 g)
    float ax = icm20602_acc_transition(icm20602_acc_x);
    float ay = icm20602_acc_transition(icm20602_acc_y);
    float az = icm20602_acc_transition(icm20602_acc_z);
    
	/*
	if (fabsf(Speed_L) < 2.0f && fabsf(Speed_R) < 2.0f) 
    {
        // 2. 判断是否“手扶稳定”
        // 当前陀螺仪读数波动不能太大，防止你手扶着车在晃，导致错误的校准
        // 假设当前读数和之前的 Offset 相差在 2度/秒以内，说明是温漂，而不是运动
        if (fabsf(raw_gz - Gyro_Z_Offset) < 2.0f) 
        {
            // 3. 核心：指数加权移动平均 (EMA)
            // 让 Offset 极其缓慢地“爬”向当前的读数
            // 0.001 是更新速度，越小越慢越稳。意味着每次修正 0.1% 的误差
            Gyro_Z_Offset = Gyro_Z_Offset * 0.999f + raw_gz * 0.001f;
        }
        
        // 4. 既然停着，直接锁死输出为 0 (强力死区)
        gz_deg = 0.0f;
    }
    else 
    {
        // 车动的时候，使用动态更新后的 Offset
        gz_deg = raw_gz - Gyro_Z_Offset;
    }
	*/
	
    // 2. 转换陀螺仪 (先减零漂，再转弧度！)
    // 死区逻辑保留！这对 Mahony 同样有效
    float gx_deg = icm20602_gyro_transition(icm20602_gyro_x) - Gyro_X_Offset;
    float gy_deg = icm20602_gyro_transition(icm20602_gyro_y) - Gyro_Y_Offset;
    float gz_deg = icm20602_gyro_transition(icm20602_gyro_z) - Gyro_Z_Offset;
	
	Real_Gyro_Y = gy_deg;

    if (gz_deg > -GYRO_DEAD_ZONE && gz_deg < GYRO_DEAD_ZONE) gz_deg = 0.0f;	// 死区
	if (gx_deg > -GYRO_DEAD_ZONE && gx_deg < GYRO_DEAD_ZONE) gz_deg = 0.0f;
	if (gy_deg > -GYRO_DEAD_ZONE && gy_deg < GYRO_DEAD_ZONE) gz_deg = 0.0f;

    float gx = gx_deg * 0.0174533f; // 转弧度
    float gy = gy_deg * 0.0174533f;
    float gz = gz_deg * 0.0174533f;

    // 3. ★调用 Mahony 算法★
    // 注意：如果是平衡车，Pitch/Roll 颠倒的话，交换 ax/ay 或 gx/gy
    MahonyAHRSupdateIMU(gy, gx, gz, -ay, -ax, az);

    // 4. 计算角度
    Mahony_Get_Angles();
}

// === 校准函数定义 ===
// 放在 main 函数前面，或者单独的文件里都可以
void IMU_Calibration(void)
{
    float sum_x = 0, sum_y = 0, sum_z = 0;
    
    // 1. 提示开始
    tft180_clear();
    tft180_show_string(0, 0, "Calibrating...");
    tft180_show_string(0, 20, "Don't Move!");
    
    // 2. 循环读取 1000 次 (让它测久一点，更准)
    for(int i = 0; i < 1000; i++)
    {
        icm20602_get_gyro();
        
        // 累加物理值 (deg/s)
        sum_x += icm20602_gyro_transition(icm20602_gyro_x);
        sum_y += icm20602_gyro_transition(icm20602_gyro_y);
        sum_z += icm20602_gyro_transition(icm20602_gyro_z);
        
        system_delay_ms(2); // 采样间隔
    }
    
    // 3. 算出平均值 (这就是零漂)
    Gyro_X_Offset = sum_x / 1000.0f;
    Gyro_Y_Offset = sum_y / 1000.0f;
    Gyro_Z_Offset = sum_z / 1000.0f;
    
    // 4. 显示在屏幕上 (保留4位小数)
    tft180_clear();
    tft180_show_string(0, 0, "Gyro Offset:");
    
    tft180_show_string(0, 30, "X:");
    tft180_show_float(40, 30, Gyro_X_Offset, 2, 4);
    
    tft180_show_string(0, 50, "Y:");
    tft180_show_float(40, 50, Gyro_Y_Offset, 2, 4);
    
    tft180_show_string(0, 70, "Z:");
    tft180_show_float(40, 70, Gyro_Z_Offset, 2, 4);
    
    // 5. 死循环卡住 (防止程序跑飞，给你时间拍照/抄写)
    while(1);
}