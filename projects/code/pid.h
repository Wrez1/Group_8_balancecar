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
	
	// ★★★ 新增：增量式专用变量 ★★★
    float Last_Integral; // 用于存储学长代码里的 last_i (上一次积分增量)
	
	float OutMax;
	float OutMin;
	
	float OutOffset;
} PID_t;


extern PID_t GyroPID;
// ★★★ 必须加上这一行！ ★★★
extern PID_t TurnPID;      // 转向环 PID 对象
extern PID_t SpeedPID;     // 建议把速度环也加上，以后可能会用到
extern PID_t AnglePID;     // 建议把直立环也加上
extern PID_t PositionPID; // ★★★ 新增：位置环 PID 对象 ★★★

extern float Target_Location; // 目标位置 (想要去哪里)
extern volatile uint8_t Control_Mode;  // 控制模式开关

void Position_PIDControl(void); // 新增函数声明
void Gyro_PIDControl(void); // 新增函数声明
void PID_Init(PID_t *p);
void PID_Update(PID_t *p);
void Angle_PIDControl(void);
void Speed_PIDControl(void);
void Turn_PIDControl(void);
extern float Real_Gyro_X;  // 真实角速度
extern float Target_Gyro;  // 目标角速度
extern float Mechanical_Zero_Pitch;


 //新增：声明 Z 轴角速度 和 转向目标变量
extern float Turn_Target;  // 转向目标 (由遥控器或循迹算法赋值)

extern PID_t GyroPID;      // 角速度环 PID 对象
void Angle_Gyro_Cascade_Control(void);
#endif
