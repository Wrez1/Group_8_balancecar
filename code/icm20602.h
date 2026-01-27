#ifndef ICM20602_H
#define ICM20602_H

#include "zf_common_typedef.h"
#include "zf_device_icm20602.h" // 包含你上传的库

// 定义卡尔曼结构体
typedef struct {
    float angle;        // 输出：最优估计角度
    float gyro_bias;    // 输出：陀螺仪零漂
    float rate;         // 输出：去除零漂后的角速度
    float P[2][2];      // 误差协方差矩阵
    float Q_angle;      // 过程噪声：角度 (建议 0.001)
    float Q_gyro;       // 过程噪声：偏差 (建议 0.003)
    float R_angle;      // 测量噪声：加速度计 (建议 0.5)
    float K[2];         // 卡尔曼增益
} Kalman_t;

// 定义姿态数据结构体
typedef struct {
    float Pitch;        // 俯仰角 (用于平衡)
    float Roll;         // 横滚角
    float Yaw;          // 航向角 (纯陀螺仪积分，存在漂移)
    
    // 惯导用数据 (世界坐标系)
    float World_Acc_X;  // 去除重力后的世界坐标系加速度 X (cm/s^2)
    float World_Acc_Y;  // 去除重力后的世界坐标系加速度 Y (cm/s^2)
} Attitude_t;

extern Attitude_t Car_Attitude;

// 函数声明
void IMU_Init_Task(void);
void IMU_Get_Data_Task(float dt); // dt: 循环周期，单位秒 (例如 0.005)

#endif