#include "zf_common_headfile.h"
#include <math.h>
#include <stdio.h>
#include "ICM.h"

// ICM专用变量定义
uint8 icm_gyro_ration = 16;
uint8 icm_acc_ration = 4;
float icm_filtering_angle = 0;
float icm_angle_roll_temp = 0.0f;
float icm_angle_yaw_temp = 0.0f;
float icm_yaw = 0.0f;

// 零偏误差
int16_t icm_gx_error = 0;
int16_t icm_gy_error = 0;
int16_t icm_gz_error = 0;

// 采样周期
float icm_call_cycle = 0.002f;

// 传感器原始数据
int16_t icm_gx, icm_gy, icm_gz;
int16_t icm_ax, icm_ay, icm_az;

// 中值角度
float icm_machine_mid = 0.0f;

// 初始化ICM20602姿态解算
void icm_attitude_init(void)
{
    while (1)
    {
        if (icm20602_init())
        {
            zf_log(0, "icm init error");
            break;
        }
        else
        {
            zf_log(0, "icm init success");
            break;
        }
    }
    
    // 设置比例因子
    switch(ICM20602_GYRO_SAMPLE_DEFAULT)
    {
        case ICM20602_GYRO_SAMPLE_SGN_250DPS:
            icm_gyro_ration = 131;
            break;
        case ICM20602_GYRO_SAMPLE_SGN_500DPS:
            icm_gyro_ration = 65.5;
            break;
        case ICM20602_GYRO_SAMPLE_SGN_1000DPS:
            icm_gyro_ration = 32.8;
            break;
        case ICM20602_GYRO_SAMPLE_SGN_2000DPS:
            icm_gyro_ration = 16.4;
            break;
        default:
            icm_gyro_ration = 16.4;
            break;
    }
    
    // 重置变量
    icm_angle_roll_temp = 0.0f;
    icm_angle_yaw_temp = 0.0f;
    icm_yaw = 0.0f;
    icm_filtering_angle = 0.0f;
}

// 校准ICM20602传感器
void icm_calibrate(void)
{
    int32_t gx_sum = 0, gy_sum = 0, gz_sum = 0;
    const uint16_t sample_count = 1000;
    
    for (uint16_t i = 0; i < sample_count; i++)
    {
        icm20602_get_gyro();
        gx_sum += icm20602_gyro_x;
        gy_sum += icm20602_gyro_y;
        gz_sum += icm20602_gyro_z;
        system_delay_ms(1);
    }
    
    icm_gx_error = -(gx_sum / sample_count);
    icm_gy_error = -(gy_sum / sample_count);
    icm_gz_error = -(gz_sum / sample_count);
    
    char log_buffer[100];
    snprintf(log_buffer, sizeof(log_buffer), 
             "ICM Calibration: gx_err=%d, gy_err=%d, gz_err=%d", 
             icm_gx_error, icm_gy_error, icm_gz_error);
    zf_log(0, log_buffer);
}

// ICM20602一阶互补滤波姿态解算
void icm_first_order_complementary_filtering(void)
{
    icm20602_get_acc();
    icm20602_get_gyro();
    
    // 零偏校正
    icm_gx = icm20602_gyro_x + icm_gx_error;
    icm_gy = icm20602_gyro_y + icm_gy_error;
    icm_gz = icm20602_gyro_z + icm_gz_error;
    icm_ax = icm20602_acc_x;
    icm_ay = icm20602_acc_y;
    icm_az = icm20602_acc_z;
    
    // 去除小噪声
    if (abs(icm_gx) < 5) icm_gx = 0;
    if (abs(icm_gy) < 5) icm_gy = 0;
    if (abs(icm_gz) < 5) icm_gz = 0;
    
    float gyro_temp;
    float acc_temp;
    
    // 滚转角解算
    gyro_temp = icm20602_gyro_transition(icm_gx);
    acc_temp = (icm_ay - icm_angle_roll_temp) * icm_acc_ration;
    icm_angle_roll_temp += ((gyro_temp + acc_temp) * icm_call_cycle);
    icm_filtering_angle = icm_angle_roll_temp + icm_machine_mid;
    
    // 偏航角解算
    gyro_temp = icm20602_gyro_transition(icm_gz);
    acc_temp = (icm_az - icm_angle_yaw_temp) * icm_acc_ration;
    icm_angle_yaw_temp += ((gyro_temp + acc_temp) * icm_call_cycle);
    icm_yaw = icm_angle_yaw_temp;
}

// 获取ICM姿态角度
void icm_get_attitude_angles(float *roll, float *pitch, float *yaw_angle)
{
    icm20602_get_acc();
    icm20602_get_gyro();
    
    icm_gx = icm20602_gyro_x + icm_gx_error;
    icm_gy = icm20602_gyro_y + icm_gy_error;
    icm_gz = icm20602_gyro_z + icm_gz_error;
    icm_ax = icm20602_acc_x;
    icm_ay = icm20602_acc_y;
    icm_az = icm20602_acc_z;
    
    // 基于加速度计的角度
    float acc_roll = atan2(icm_ay, icm_az) * 180.0f / 3.14159265f;
    float acc_pitch = atan2(-icm_ax, sqrt(icm_ay * icm_ay + icm_az * icm_az)) * 180.0f / 3.14159265f;
    
    static float gyro_roll = 0, gyro_pitch = 0;
    const float alpha = 0.98f;
    
    float gyro_x_rate = icm20602_gyro_transition(icm_gx) * icm_call_cycle;
    float gyro_y_rate = icm20602_gyro_transition(icm_gy) * icm_call_cycle;
    
    gyro_roll += gyro_x_rate;
    gyro_pitch += gyro_y_rate;
    
    *roll = alpha * gyro_roll + (1 - alpha) * acc_roll;
    *pitch = alpha * gyro_pitch + (1 - alpha) * acc_pitch;
    
    static float gyro_yaw = 0;
    float gyro_z_rate = icm20602_gyro_transition(icm_gz) * icm_call_cycle;
    gyro_yaw += gyro_z_rate;
    *yaw_angle = gyro_yaw;
}

// 设置中值角度
void icm_set_machine_mid(float mid_angle)
{
    icm_machine_mid = mid_angle;
}

// 设置采样周期
void icm_set_call_cycle(float cycle)
{
    if (cycle > 0)
    {
        icm_call_cycle = cycle;
    }
}

// 获取滤波后的角度
float icm_get_filtering_angle(void)
{
    return icm_filtering_angle;
}

// 获取偏航角
float icm_get_yaw(void)
{
    return icm_yaw;
}
