#include "zf_common_headfile.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"

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
		if (p->ErrorInt < p->ErrorIntMin) {p->ErrorInt = p->ErrorIntMin;}
	}
	else
	{
		p->ErrorInt = 0;
	}
	
	p->Out = p->Kp * p->Error0
		   + p->Ki * p->ErrorInt
//		   + p->Kd * (p->Error0 - p->Error1);
		   - p->Kd * (p->Actual - p->Actual1);
	
	if (p->Out > 0) {p->Out += p->OutOffset;}
	if (p->Out < 0) {p->Out -= p->OutOffset;}
	
	if (p->Out > p->OutMax) {p->Out = p->OutMax;}
	if (p->Out < p->OutMin) {p->Out = p->OutMin;}
	
	p->Actual1 = p->Actual;
}

//角度环
extern float yaw, pitch, roll;
extern PID_t AnglePID;
extern int16_t LeftPWM,RightPWM;
extern int16_t AvePWM,DifPWM;
void Angle_PIDControl(void)
{
	if (pitch > 50 || pitch < -50)	//角度过大保护	
	{
		motor_control(0, 0);
		return;
	}
	AnglePID.Actual = pitch;
	PID_Update(&AnglePID);
	
	AvePWM = AnglePID.Out;
	LeftPWM = AvePWM + DifPWM / 2;
	RightPWM = AvePWM - DifPWM / 2;
	
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
		SpeedLeft=Get_Encoder2();
		SpeedRight=Get_Encoder1();
		pit_encoder_handler();
		AveSpeed=(SpeedLeft+SpeedRight)/2.f;
		DifSpeed=SpeedLeft-SpeedRight;
		
		SpeedPID.Actual=AveSpeed;
		PID_Update(&SpeedPID);
		AnglePID.Target=SpeedPID.Out;
}

//转向环
extern PID_t TurnPID;
void Turn_PIDControl(void)
{
		SpeedLeft=Get_Encoder2();
		SpeedRight=Get_Encoder1();
		pit_encoder_handler();
		AveSpeed=(SpeedLeft+SpeedRight)/2.f;
		DifSpeed=SpeedLeft-SpeedRight;
		
		TurnPID.Actual=DifSpeed;
		PID_Update(&TurnPID);
		DifPWM=TurnPID.Out;
}