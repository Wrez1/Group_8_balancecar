#ifndef __PID_H
#define __PID_H
#include "zf_common_headfile.h"

typedef struct {
	float Target;
	float Actual;
	float Actual1;
	float Out;
	
	float Kp;
	float Ki;
	float Kd;
	
	float Error0;
	float Error1;
	float ErrorInt;
	
	float ErrorIntMax;
	float ErrorIntMin;
	
	float OutMax;
	float OutMin;
	
	float OutOffset;
} PID_t;


extern PID_t GyroPID;
// ★★★ 必须加上这一行！ ★★★
extern PID_t TurnPID;      // 转向环 PID 对象
extern PID_t SpeedPID;     // 建议把速度环也加上，以后可能会用到
extern PID_t AnglePID;     // 建议把直立环也加上

void Gyro_PIDControl(void); // 新增函数声明
void PID_Init(PID_t *p);
void PID_Update(PID_t *p);
void Angle_PIDControl(void);
void Speed_PIDControl(void);
void Turn_PIDControl(void);
extern float Real_Gyro_X;  // 真实角速度
extern float Target_Gyro;  // 目标角速度
extern PID_t GyroPID;      // 角速度环 PID 对象
void Angle_Gyro_Cascade_Control1(void);
void Angle_Gyro_Cascade_Control2(void);
#endif
