#include "zf_common_headfile.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"
#include "icm20602.h"
#include "math.h"
#include "menu.h"
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

float Mechanical_Zero_Pitch = -0.3f;
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
float AveSpeed,DifSpeed,AveSpeed_mode2;
// 定义一个静态变量来保存上一次的速度
static float Last_AveSpeed = 0.0f;
static float Last_AveSpeed_mode2 = 0.0f;
void Speed_PIDControl(void)
{
		SpeedLeft=(int16_t)Get_Count1();
		SpeedRight=(int16_t)Get_Count2();
		pit_encoder_handler();
		float Raw_Speed=(SpeedLeft+SpeedRight)/2.f;
		
		if(CarMode==2){
			AveSpeed_mode2 = Raw_Speed * 0.66f + Last_AveSpeed_mode2 * 0.34f;
			Last_AveSpeed_mode2 = AveSpeed_mode2; // 更新历史值
			SpeedPID.Actual=AveSpeed_mode2;
		}
		else{
			AveSpeed = Raw_Speed * 0.66f + Last_AveSpeed * 0.34f;
			Last_AveSpeed = AveSpeed; // 更新历史值
			SpeedPID.Actual=AveSpeed;
		}
		DifSpeed=SpeedLeft-SpeedRight;
		PID_Update(&SpeedPID);
		//AnglePID.Target=SpeedPID.Out;
	
		//TurnPID.Actual = DifSpeed;
		//PID_Update(&TurnPID);
		//DifPWM = TurnPID.Out;
	
	
}

//转向环
extern PID_t TurnPID;
// 定义全局变量
float Turn_Target = 0.0;
float Line_Turn_Target=0.0;
void Turn_PIDControl(void)
{
	if(CarMode==5){
    // 目标：来自蓝牙 (Turn_Target)
		TurnPID.Target = Turn_Target;
    }
	else if(CarMode==2){
		TurnPID.Target=Line_Turn_Target;
	}
    // 反馈：来自陀螺仪 Z 轴 (Real_Gyro_Z)
    // 注意：Real_Gyro_Z 必须是“去零漂”后的值 (在 icm20602.c 里处理)
    TurnPID.Actual = Real_Gyro_Z; 
    
    // 计算
    PID_Update(&TurnPID);
    
    // 输出：DifPWM 留给 Cascade 函数去叠加
    DifPWM = TurnPID.Out;
}

// 1. 定义全局变量
PID_t PositionPID;       // 位置环结构体
float Target_Location = 0.0f; // 目标位置
volatile uint8 Control_Mode = 0;// 0:蓝牙遥控(开环), 1:位置模式(闭环/惯导)
extern float Location;

// 2. 初始化函数 (在 All_PID_Init 中调用)
// 参数建议：位置环不需要太快，Kp 给小一点，限幅限制最大巡航速度
void Position_PID_Init(void)
{
    PID_Init(&PositionPID);
    PositionPID.Kp = 0.05f;      // 试探值：误差1000脉冲 -> 目标速度 50
    PositionPID.Ki = 0.0f;       // 位置环不需要积分
    PositionPID.Kd = 0.0f;       // 位置环不需要微分 (速度环已经是微分了)
    
    PositionPID.OutMax = 20.0f;  // ★最大巡航速度限制 (防止飞车)
    PositionPID.OutMin = -20.0f;
}

// 3. 位置环控制函数
void Position_PIDControl(void)
{
    PositionPID.Target = Target_Location; // 你想去哪
    PositionPID.Actual = Location;        // 你在哪
    
    PID_Update(&PositionPID);
    
    // ★★★ 核心连接：位置环的输出 = 速度环的目标 ★★★
    // 只有在 Control_Mode == 1 时，这个值才会被写入 SpeedPID.Target
    // 具体的写入操作，我们放在 isr.c 里做，比较安全
	// 如果误差小于 50 个脉冲 (大概几毫米)，就认为到了，不发力
    if (fabsf(PositionPID.Target - PositionPID.Actual) < 50.0f)
    {
        PositionPID.Out = 0.0f;
    }
}


// 引用 encoder.c 里的全局位置变量
extern float Location;


extern Attitude_t Car_Attitude; // 你的姿态结构体
extern PID_t GyroPID;           // 新增
//extern int16_t icm20602_gyro_y; // 或者是转换后的 float gyro_y (角速度原始值)
float Target_Gyro = 0.0f;

void Angle_Gyro_Cascade_Control(void)
{
    float AvePWM;
    
    // ==========================================
    // 1. 安全保护 (炸机保护)
    // ==========================================
    // 角度过大（倒地），强制关闭电机
    if (Car_Attitude.Pitch > 55.0f || Car_Attitude.Pitch < -55.0f) 
    {
        motor_control(0, 0);
		// 2. ★★★ 关键：清空所有 PID 的积分和历史误差 ★★★
        // 否则你把车扶正的瞬间，积分项会让车子猛冲一下
        PID_Init(&AnglePID);
        PID_Init(&GyroPID);
        PID_Init(&SpeedPID);
        PID_Init(&TurnPID); // 转向环也要清空！
        
        // 3. 目标全部归零
        Turn_Target = 0;
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
	
	float Safe_Angle = 14.0f;
    // 差速控制：左加右减（或反之）
    if (fabsf(Car_Attitude.Pitch - Mechanical_Zero_Pitch) < Safe_Angle)
    {
        // 【情况 A：车子站稳了】
        // 允许叠加转向环输出，开始干活！
        LeftPWM  = (int16_t)(AvePWM - TurnPID.Out);
        RightPWM = (int16_t)(AvePWM + TurnPID.Out);
    }
    else
    {
        // 【情况 B：车子还在挣扎（起步或快倒了）】
        // 1. 强制切断转向力，全力维持平衡！
        LeftPWM  = (int16_t)AvePWM;
        RightPWM = (int16_t)AvePWM;
        
        // 2. ★关键★：清空转向环的积分和输出
        // 为什么要清空？
        // 如果不清空，PID 会在后台一直憋大招（积分累积）。
        // 等车子刚一站稳（进入 ±8 度），这股憋着的力量会突然释放，
        // 导致车子猛地甩一下尾，甚至直接甩倒。
        TurnPID.ErrorInt = 0;
        TurnPID.Out = 0;
    }
    
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
	
    motor_control(-LeftPWM, RightPWM);
}
