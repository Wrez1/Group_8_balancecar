#include "icm20602.h"
#include <math.h>
#include "zf_common_headfile.h"

// 全局姿态对象
Attitude_t Car_Attitude;

// 定义两个卡尔曼滤波器实例 (Pitch 和 Roll)
Kalman_t Kalman_Pitch;
Kalman_t Kalman_Roll;

// 常量定义
#define RAD_TO_DEG  57.2957795f
#define DEG_TO_RAD  0.01745329f

// ---------------------------------------------------------
// 卡尔曼滤波核心算法
// ---------------------------------------------------------
void Kalman_Filter_Calc(Kalman_t *k, float acc_angle, float gyro_rate, float dt)
{
    // 1. 预测 (Predict)
    // 更新角度：当前角度 = 上次角度 + (角速度 - 零漂) * dt
    k->angle += (gyro_rate - k->gyro_bias) * dt;

    // 更新协方差矩阵 P
    // P = F * P * F^T + Q
    k->P[0][0] += k->Q_angle - (k->P[0][1] + k->P[1][0]) * dt;
    k->P[0][1] -= k->P[1][1] * dt;
    k->P[1][0] -= k->P[1][1] * dt;
    k->P[1][1] += k->Q_gyro;

    // 2. 更新 (Update)
    // 计算卡尔曼增益 K
    // S = H * P * H^T + R
    float S = k->P[0][0] + k->R_angle;
    
    // K = P * H^T * S^-1
    k->K[0] = k->P[0][0] / S;
    k->K[1] = k->P[1][0] / S;

    // 计算观测误差 y = z - H * x
    float y = acc_angle - k->angle;

    // 更新最优估计 x = x + K * y
    k->angle      += k->K[0] * y;
    k->gyro_bias  += k->K[1] * y; // 自动估计并修正陀螺仪零漂
    k->rate        = gyro_rate - k->gyro_bias; // 输出修正后的角速度

    // 更新协方差矩阵 P = (I - K * H) * P
    float P00_temp = k->P[0][0];
    float P01_temp = k->P[0][1];

    k->P[0][0] -= k->K[0] * P00_temp;
    k->P[0][1] -= k->K[0] * P01_temp;
    k->P[1][0] -= k->K[1] * P00_temp;
    k->P[1][1] -= k->K[1] * P01_temp;
}

// ---------------------------------------------------------
// 卡尔曼参数初始化
// ---------------------------------------------------------
void Kalman_Init(Kalman_t *k)
{
    k->angle = 0.0f;
    k->gyro_bias = 0.0f;
    k->rate = 0.0f;
    
    k->P[0][0] = 1.0f; k->P[0][1] = 0.0f;
    k->P[1][0] = 0.0f; k->P[1][1] = 1.0f;
    
    // --- 参数调试指南 ---
    // 如果角度滞后严重（反应慢）：增大 Q_angle，减小 R_angle
    // 如果角度震荡严重（噪声大）：减小 Q_angle，增大 R_angle
    k->Q_angle = 0.001f;  
    k->Q_gyro  = 0.003f;
    k->R_angle = 80.0f;    
}

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
	
    
    // 初始化卡尔曼参数
    Kalman_Init(&Kalman_Pitch);
    Kalman_Init(&Kalman_Roll);
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
    // 1. 获取原始数据 (数据保存在 icm20602_acc_x 等全局变量中)
    icm20602_get_acc();
    icm20602_get_gyro();

    // 2. 物理量转换 (利用库中的宏)
    // 转换为 g
    float accX = icm20602_acc_transition(icm20602_acc_x);
    float accY = icm20602_acc_transition(icm20602_acc_y);
    float accZ = icm20602_acc_transition(icm20602_acc_z);
    
    // 转换为 deg/s
    float gyroX = icm20602_gyro_transition(icm20602_gyro_x);
    float gyroY = icm20602_gyro_transition(icm20602_gyro_y);
    float gyroZ = icm20602_gyro_transition(icm20602_gyro_z);

    // 3. 计算加速度计角度 (atan2 返回弧度，需转为度)
    // 假设芯片安装方向：X轴向前，Y轴向右，Z轴向下(或上)
    // Pitch 绕 Y 轴旋转，Roll 绕 X 轴旋转
    
    // 计算 Accelerometer Pitch (俯仰)
    // atan2(X, Z) 
    float acc_pitch_angle = atan2f(accX, accZ) * RAD_TO_DEG;
    
    // 计算 Accelerometer Roll (横滚)
    // atan2(Y, Z)
    float acc_roll_angle  = atan2f(accY, accZ) * RAD_TO_DEG;

    // 4. 卡尔曼滤波融合
    // 注意：陀螺仪轴向必须与加速度计计算出的角度变化方向一致
    // 如果 acc_pitch 增加时，gyroY 是负的，则这里需要写 -gyroY
    Kalman_Filter_Calc(&Kalman_Pitch, acc_pitch_angle, -gyroY, dt); // 绕Y轴转是Pitch
    Kalman_Filter_Calc(&Kalman_Roll,  acc_roll_angle,  gyroX, dt); // 绕X轴转是Roll (注意符号需实测)

    // 5. 更新全局结果
    Car_Attitude.Pitch = Kalman_Pitch.angle;
    Car_Attitude.Roll  = Kalman_Roll.angle;
    
    // Yaw 轴通常只能积分，没有磁力计无法融合修正漂移
    Car_Attitude.Yaw  += gyroZ * dt; 

    // 6. 惯性导航数据计算 (为速度环或位移估算提供数据)
    IMU_Inertial_Nav_Calc(accX, accY, accZ, Car_Attitude.Pitch, Car_Attitude.Roll);
}