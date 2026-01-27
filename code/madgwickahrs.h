//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef madgwickahrs_h
#define madgwickahrs_h

// 举例：如果静止时串口打印 gyroX=1.2, 则填 1.2
#define GYRO_X_OFFSET  0.0f 
#define GYRO_Y_OFFSET  0.0f
#define GYRO_Z_OFFSET  0.0f

// 角度转弧度系数 (PI / 180)
#define DEG2RAD  0.01745329f

//----------------------------------------------------------------------------------------------------
// Variable declaration

extern volatile float beta;				// algorithm gain
extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void Madgwick_Get_Angles(void);
#endif
//=====================================================================================================
// End of file
//=====================================================================================================
