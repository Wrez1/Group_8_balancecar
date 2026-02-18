#include "icm20602.h"
#include <math.h>
#include "zf_common_headfile.h"

// 全局姿态对象
Attitude_t Car_Attitude;

// 常量定义
#define RAD_TO_DEG  57.2957795f
#define DEG_TO_RAD  0.01745329f
#define GYRO_DEAD_ZONE  0.3f


//float Gyro_X_Offset = -0.5524f;
//float Gyro_Y_Offset = 0.7862f;
//float Gyro_Z_Offset = -0.3352f;
float Gyro_X_Offset = 0.0f; 
float Gyro_Y_Offset = 0.0f;
float Gyro_Z_Offset = 0.0f;
float Real_Gyro_X = 0.0f; 
float Real_Gyro_Y = 0.0f;
float Real_Gyro_Z = 0.0f;
// 滤波历史变量
static float Last_Gyro_X = 0.0f;
//static float Last_Gyro_Z = 0.0f;

// 定义两个历史变量，分别用于不同的滤波通道
static float Last_Gyro_Z_Nav = 0.0f;  // 导航用 (轻滤波)
static float Last_Gyro_Z_Ctrl = 0.0f; // 控制用 (重滤波)

// 定义全局变量 (记得去 .h 声明一下)
float Gyro_Z_For_Nav = 0.0f;  // ★ 专门给积分 Yaw 用的
float Gyro_Z_For_Ctrl = 0.0f; // ★ 专门给 PID 用的
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

// ★★★ 新增：封装好的瞬时初始化函数 ★★★
// 供外部（如菜单）调用，实现一键重置姿态
void IMU_Instant_Init(void)
{
    // 1. 获取当前最新的加速度原始数据
    icm20602_get_acc();
    
    // 2. 转换为物理值 (单位 g)
    float ax = icm20602_acc_transition(icm20602_acc_x);
    float ay = icm20602_acc_transition(icm20602_acc_y);
    float az = icm20602_acc_transition(icm20602_acc_z);
    
    // 3. 调用 Mahony 的重置接口
    // ★ 关键：在这里统一处理坐标系映射，防止外部调用搞错方向
    // 你的 Update 函数用的是: -ay, -ax, az
    // 所以这里保持一致
    Mahony_Init(-ay, -ax, az); 
    
    // 4. 立即计算一次角度，确保全局变量 Car_Attitude 更新
    Mahony_Get_Angles();
	Car_Attitude.Yaw = 0.0f;
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
// 主循环调用函数 (建议放在 5ms 定时中断里)
// dt: 两次调用的时间间隔，单位秒
// ---------------------------------------------------------

// 引入电机速度用于判断静止
extern float SpeedLeft, SpeedRight;

void IMU_Get_Data_Task(float dt)
{
    icm20602_get_acc();
    icm20602_get_gyro();

    // 1. 转换加速度 (单位 g)
    float ax = icm20602_acc_transition(icm20602_acc_x);
    float ay = icm20602_acc_transition(icm20602_acc_y);
    float az = icm20602_acc_transition(icm20602_acc_z);
    
	// 获取原始数据（不减Offset，为了计算动态Offset）
    float raw_gx = icm20602_gyro_transition(icm20602_gyro_x);
    float raw_gy = icm20602_gyro_transition(icm20602_gyro_y);
    float raw_gz = icm20602_gyro_transition(icm20602_gyro_z);
	
	// ================== 核心优化：动态零偏校准 ==================
    // 判断条件：左右轮速极低（近似静止）
    if (fabsf(SpeedLeft) < 5.0f && fabsf(SpeedRight) < 5.0f)
    {
        // 计算当前读数与已知零偏的偏差
        float diff = fabsf(raw_gz - Gyro_Z_Offset);
        
        // 只有偏差很小时，才认为是静止噪声，予以消除
        // 如果偏差很大（比如被撞了），说明车子在动，不能强制归零
        if (diff < 5.0f) 
        {
            // ★★★ 修改点 1：不要动态更新 Offset，风险太大 ★★★
            // Gyro_Z_Offset = Gyro_Z_Offset * 0.995f + raw_gz * 0.005f; 
            
            // ★★★ 修改点 2：直接暴力把输出锁死为 0 ★★★
            // 既然认为是静止，那就让物理值直接为 0
            // (注意：这里改 raw_gz 是为了欺骗后面的 gz_deg 计算公式)
            raw_gz = Gyro_Z_Offset; 
        }
    }
    // ==========================================================
	
	
    // 2. 转换陀螺仪 (先减零漂，再转弧度！)
    // 2. 减去动态更新后的 Offset
    float gx_deg = raw_gx - Gyro_X_Offset;
    float gy_deg = raw_gy - Gyro_Y_Offset;
    float gz_deg = raw_gz - Gyro_Z_Offset;
	
    if (fabsf(gx_deg) < GYRO_DEAD_ZONE) gx_deg = 0.0f;
    if (fabsf(gy_deg) < GYRO_DEAD_ZONE) gy_deg = 0.0f;
    if (fabsf(gz_deg) < 0.0f) gz_deg = 0.0f;
	
	//6. X轴/Y轴 滤波 (直立平衡用，中等滤波)
    float Alpha = 0.6f; 
    gx_deg = gx_deg * Alpha + Last_Gyro_X * (1.0f - Alpha);
    Last_Gyro_X = gx_deg;
    
    // ==============================================================
    // ★★★ 7. Z轴 双轨滤波 (核心修改) ★★★
    // ==============================================================
    
    // 通道A：导航专用 (轻滤波，保真实，低延迟)
    // Alpha = 0.9 表示 90% 信新数据，几乎无延迟
    float Alpha_Nav = 0.9f;
    Gyro_Z_For_Nav = gz_deg * Alpha_Nav + Last_Gyro_Z_Nav * (1.0f - Alpha_Nav);
    Last_Gyro_Z_Nav = Gyro_Z_For_Nav;
    
    // 通道B：控制专用 (重滤波，去噪声，高Kp)
    // Alpha = 0.2 表示只信 20% 新数据，极度平滑，适合 PID
    float Alpha_Ctrl = 0.2f;
    Gyro_Z_For_Ctrl = gz_deg * Alpha_Ctrl + Last_Gyro_Z_Ctrl * (1.0f - Alpha_Ctrl);
    Last_Gyro_Z_Ctrl = Gyro_Z_For_Ctrl;
    
    // 更新全局变量 (Real_Gyro_Z 一般用于显示，建议显示 Nav 的)
    Real_Gyro_X = gx_deg;
    Real_Gyro_Y = gy_deg;
    Real_Gyro_Z = Gyro_Z_For_Nav;
	
	
//	// ★★★ 核心修改：加入黄金比例滤波器 (既快又滑) ★★★
//    // ==============================================================
//    // 系数 0.6f 意味着：60% 相信最新的数据，40% 相信历史数据
//    // 相比之前的 0.3f，现在的响应速度快了一倍，但依然能滤掉高频毛刺
//    // 如果觉得还不够快，可以改到 0.7f ~ 0.8f
//    float Alpha = 0.6f; 
//    
//    gx_deg = gx_deg * Alpha + Last_Gyro_X * (1.0f - Alpha);
//    Last_Gyro_X = gx_deg; // 更新历史值
//	
//	gz_deg = gz_deg * Alpha + Last_Gyro_Z * (1.0f - Alpha);
//    Last_Gyro_Z = gz_deg;
//	
//	Real_Gyro_X = gx_deg;
//    Real_Gyro_Y = gy_deg;
    //Real_Gyro_Z = gz_deg;
	
	// ★★★ 4. 纯积分计算 Yaw (必须用滤过波的 gz_deg) ★★★
    // ==============================================================
    // dt = 0.002s (由 isr.c 传入)
    // 符号说明：如果发现转左 Yaw 变小，就改成 -=
    Car_Attitude.Yaw += Gyro_Z_For_Nav * dt;
	
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
	
	system_delay_ms(3000);
    
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
    
    system_delay_ms(1000); 
    tft180_clear();
}