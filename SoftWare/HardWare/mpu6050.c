#include "zf_common_headfile.h"
#include <math.h>
#include "mpu6050.h"  // 新增MPU6050头文件

void imu_init(void)
{
    // 初始化MPU6050
    while (1)
    {
        if (mpu6050_init())  // 使用MPU6050初始化函数
        {
            ips200_show_string(10, 140, "imu init error"); 
        }
        else
        {
            ips200_show_string(10, 140, "imu init success");
            break;
        }
    }
}

/* 一阶互补滤波 */
uint8 gyro_ration = 16;     // 修改比例因子以匹配MPU6050量程（±2000dps对应16.4 LSB/(°/s)）
uint8 acc_ration = 4;      // 根据实际加速度计量程调整
float filtering_angle = 0; 
float angle_roll_temp;          
float angle_yaw_temp = 0.0f;
float yaw=0;

float call_cycle = 0.002f;

int16_t gx;
int16_t gy;
int16_t gz;
int16_t ax;
int16_t ay;
int16_t az;

void first_order_complementary_filtering(void)
{
    // 获取MPU6050数据
    mpu6050_get_acc();
    mpu6050_get_gyro();

    // 直接使用MPU6050转换后的数据（单位：°/s 和 g）
    gx = mpu6050_gyro_x + gx_error;
    gy = mpu6050_gyro_y + gy_error;
    gz = mpu6050_gyro_z + gz_error;
    ax = mpu6050_acc_x;
    ay = mpu6050_acc_y;
    az = mpu6050_acc_z;

    // 数据滤波处理
    if (abs(gx) < 5) gx = 0;
    if (abs(gy) < 5) gy = 0;
    if (abs(gz) < 5) gz = 0;

    float gyro_temp;
    float acc_temp;
    
    // 使用MPU6050内置转换函数
    gyro_temp = mpu6050_gyro_transition(gx);  // 自动处理量程转换
    acc_temp = (ay - angle_roll_temp) * acc_ration;
    
    angle_roll_temp += ((gyro_temp + acc_temp) * call_cycle);
    filtering_angle = angle_roll_temp + machine_mid;
    
    // Yaw轴处理（示例保留原始逻辑，需根据实际需求完善）
    gyro_temp = mpu6050_gyro_transition(gz);
    acc_temp = (az - angle_yaw_temp) * acc_ration;
    angle_yaw_temp += ((gyro_temp + acc_temp) * call_cycle);
    yaw = angle_yaw_temp;
}
