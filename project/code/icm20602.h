#ifndef ICM20602_H
#define ICM20602_H

#include "zf_common_typedef.h"
#include "zf_device_icm20602.h" // 包含你上传的库
#include "mahony.h"

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
extern float Gyro_X_Offset, Gyro_Y_Offset, Gyro_Z_Offset; 
extern float Real_Gyro_X;
extern float Real_Gyro_Y;
extern float Real_Gyro_Z;
// 函数声明
void IMU_Init_Task(void);
void IMU_Get_Data_Task(float dt); // dt: 循环周期，单位秒 (例如 0.005)
void IMU_Calibration(void);
#endif
