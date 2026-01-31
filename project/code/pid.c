#include "zf_common_headfile.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"
#include "icm20602.h"
extern uint8_t balance_mode_active;
void PID_Init(PID_t *p)
{
	p->Target = 0;
	p->Actual = 0;
	p->Actual1 = 0;
	p->Out = 0;
	p->Error0 = 0;
	p->Error1 = 0;
	p->ErrorInt = 0;
}

void PID_Update(PID_t *p)
{
	p->Error1 = p->Error0;
	p->Error0 = p->Target - p->Actual;
	
	if (p->Ki != 0)
	{
		p->ErrorInt += p->Error0;
		
		if (p->ErrorInt > p->ErrorIntMax) {p->ErrorInt = p->ErrorIntMax;}
		if (p->ErrorInt < p->ErrorIntMin) {p->ErrorInt = p->ErrorIntMin;}//积分限幅
	}
	else
	{
		p->ErrorInt = 0;
	}
	
	p->Out = p->Kp * p->Error0
		   + p->Ki * p->ErrorInt
//		   + p->Kd * (p->Error0 - p->Error1);
		   - p->Kd * (p->Actual - p->Actual1);//微分先行
	
	if (p->Out > 0) {p->Out += p->OutOffset;}
	if (p->Out < 0) {p->Out -= p->OutOffset;}//输出偏移
	
	if (p->Out > p->OutMax) {p->Out = p->OutMax;}
	if (p->Out < p->OutMin) {p->Out = p->OutMin;}
	
	p->Actual1 = p->Actual;
}

//角度环
extern PID_t AnglePID;
extern int16_t LeftPWM,RightPWM;
extern int16_t AvePWM,DifPWM;
extern PID_t SpeedPID;
extern PID_t TurnPID;
extern float SpeedLeft,SpeedRight;
extern float AveSpeed,DifSpeed;

float Mechanical_Zero_Pitch = 1.80f;
void Angle_PIDControl(void)
{
	if (Car_Attitude.Pitch > 70 || Car_Attitude.Pitch < -70)	//角度过大保护	
	{
		motor_control(0, 0);
		return;
	}
	AnglePID.Actual = Car_Attitude.Pitch;
	PID_Update(&AnglePID);
	AvePWM = -AnglePID.Out;
	
	LeftPWM = AvePWM + DifPWM / 2.0f;
	RightPWM = AvePWM - DifPWM / 2.0f;
	
	if (LeftPWM > 10000) LeftPWM = 10000;else if (LeftPWM < -10000) LeftPWM = -10000;
	if (RightPWM > 10000) RightPWM = 10000;else if (RightPWM < -10000) RightPWM = -10000;
	
	motor_control(LeftPWM, RightPWM);
}

//速度环
extern PID_t SpeedPID;
extern float SpeedLeft,SpeedRight;
extern float AveSpeed,DifSpeed;
void Speed_PIDControl(void)
{
		SpeedLeft=(int16_t)Get_Count2();
		SpeedRight=(int16_t)Get_Count1();
		pit_encoder_handler();
		AveSpeed=(SpeedLeft+SpeedRight)/2.f;
		DifSpeed=SpeedLeft-SpeedRight;
		
		SpeedPID.Actual=AveSpeed;
		PID_Update(&SpeedPID);
		AnglePID.Target=SpeedPID.Out;
	
		TurnPID.Actual = DifSpeed;
		PID_Update(&TurnPID);
		DifPWM = TurnPID.Out;
}

//转向环
extern PID_t TurnPID;
void Turn_PIDControl(void)
{
		SpeedLeft=(int16_t)Get_Count2();
		SpeedRight=(int16_t)Get_Count1();
		pit_encoder_handler();
		AveSpeed=(SpeedLeft+SpeedRight)/2.f;
		DifSpeed=SpeedLeft-SpeedRight;
		
		TurnPID.Actual=DifSpeed;
		PID_Update(&TurnPID);
		DifPWM=TurnPID.Out;
}

extern Attitude_t Car_Attitude; // 你的姿态结构体
extern PID_t GyroPID;           // 新增
//extern int16_t icm20602_gyro_y; // 或者是转换后的 float gyro_y (角速度原始值)
float Target_Gyro = 0.0f;

void Angle_Gyro_Cascade_Control(void)
{ 
//    if (!balance_mode_active) {
//        motor_control(0, 0);
//	}
    float AvePWM;
    
    // ==========================================
    // 1. 安全保护 (炸机保护)
    // ==========================================
    // 角度过大（倒地），强制关闭电机
    if (Car_Attitude.Pitch > 45.0f || Car_Attitude.Pitch < -45.0f) 
    {
        motor_control(0, 0);
        return;
    }

    // ==========================================
    // 2. 速度环叠加 (改变平衡角度)
    // ==========================================
    // 速度环的输出叠加到角度目标上
    // 原理：我想加速(SpeedPID输出正)，我就让目标角度变大(前倾)，车就会去追
    float Target_Angle = Mechanical_Zero_Pitch + SpeedPID.Out;

    // ==========================================
    // 3. 外环：直立角度环 (P控制)
    // ==========================================
    // 输入：角度误差
    // 输出：目标角速度
    AnglePID.Target = Target_Angle;
    AnglePID.Actual = Car_Attitude.Pitch;
    
    // 计算 PID (通常直立外环只有 P，Ki=0, Kd=0)
    PID_Update(&AnglePID);
    
    // 外环输出 = 内环目标
    float Target_Gyro = -AnglePID.Out; 
    
    // ==========================================
    // 4. 内环：角速度环 (PD控制)
    // ==========================================
    // 输入：角速度误差
    // 输出：PWM
    GyroPID.Target = Target_Gyro;
    
    // ★★★ 修正：使用 X 轴角速度 (因为你的Mahony第2个参数是gx) ★★★
	GyroPID.Actual = Real_Gyro_X;
    
    // 计算 PID (通常内环只有 P 和 D，Ki=0)
    PID_Update(&GyroPID);
    
    // 获取直立控制的基础 PWM
    // 注意符号：如果车往前倒(Pitch变大)，GyroPID.Out 通常为正，
    // 我们需要电机向前转(正PWM)来平衡，所以这里可能不需要负号，或者需要负号
    // 这取决于你的 motor_control 逻辑和传感器安装方向。
    // 如果发现车子加速倒下，请把这里的符号取反！
    AvePWM = GyroPID.Out; 

    // ==========================================
    // 5. 转向环叠加 & 电机输出
    // ==========================================
    // TurnPID.Out 在 isr.c 或 navigation.c 里计算
    // 差速控制：左加右减（或反之）
    LeftPWM  = (int16_t)(AvePWM - TurnPID.Out);
    RightPWM = (int16_t)(AvePWM + TurnPID.Out);
    
    // ==========================================
    // 6. 限幅与执行
    // ==========================================
    if (LeftPWM  > 9500) LeftPWM  = 9500;  else if (LeftPWM  < -9500) LeftPWM  = -9500;
    if (RightPWM > 9500) RightPWM = 9500;  else if (RightPWM < -9500) RightPWM = -9500;
    
	//tft180_show_float(0, 0, AnglePID.Kp, 2, 1); 
    // 显示 实际角度，看看是不是 0？
    //tft180_show_float(0, 20, Car_Attitude.Pitch, 3, 2);
    // 显示 目标角速度，看看是不是 0？
    //tft180_show_float(0, 40, AnglePID.Out, 3, 2);
    // 显示 最终PWM，看看是不是 0？
    //tft180_show_int(0, 60, AvePWM, 4);
	
    motor_control(LeftPWM, RightPWM);
}
