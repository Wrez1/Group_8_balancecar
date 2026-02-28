#include "navigation.h"
#include "flash.h"      // 引入 Flash 存取函数
#include <string.h>
#include "zf_common_headfile.h"
#include "buzzer.h"
#include "math.h"
#include "pid.h"
// 定义大数组 (放在 RAM 里)
PathPoint Nav_Record_Buffer[MaxSize]; 
Nag N;
extern PID_t SpeedPID;

// 初始化
void Init_Nag(void)
{
    memset(&N, 0, sizeof(N));
    memset(Nav_Record_Buffer, 0, sizeof(Nav_Record_Buffer));
    
    // 尝试加载
    uint8_t status = flash_load_nag();
    
    if(status == 1)
    {
        // 加载成功：一声长滴 (代表有数据)
        buzzer_on(1);
        system_delay_ms(500);
        buzzer_on(0);
    }
    else
    {
        // 加载失败 (Flash为空或坏了)：三声急促短滴
        // 这样你一上电就知道有没有读到数据
        for(int i=0; i<3; i++) {
            buzzer_on(1); system_delay_ms(50); buzzer_on(0); system_delay_ms(50);
        }
    }
}

// 录制逻辑 (Teach)
void Run_Nag_Save(void)
{
    // 1. 获取这 2ms 内的真实物理微小位移
    float step_pulses = (L_Mileage + R_Mileage) / 2.0f;
    float current_step = step_pulses / (float)Nag_Set_mileage; 
    float yaw_rad = Nag_Yaw * PI / 180.0f;
    
    // 2. ★ 核心修复：每次 2ms 都进行平滑的坐标推算
    N.Current_X += current_step * (-sinf(yaw_rad));
    N.Current_Y += current_step * cosf(yaw_rad);

    // 3. 累积里程用于存点判断 (维持原有逻辑，保证点距均匀)
    N.Mileage_All += step_pulses; 
    
    if(N.Mileage_All >= Nag_Set_mileage) 
    {
        if(N.Save_index >= MaxSize - 1) {
            N.End_f = 1; 
            return;
        }
        // 将当前的平滑坐标存入 Flash 数组
        Nav_Record_Buffer[N.Save_index].x = N.Current_X;
        Nav_Record_Buffer[N.Save_index].y = N.Current_Y;
        N.Save_index++;

        N.Mileage_All -= Nag_Set_mileage; 
    }
}


// ========================================================
// ★ 复现逻辑 (Repeat) - 纯跟踪算法 (2ms 高频丝滑版)
// ========================================================
void Run_Nag_GPS(void)
{
    // === 1. 实时位置积分 (每2ms执行，保持最高精度) ===
    float step_pulses = (L_Mileage + R_Mileage) / 2.0f;
    float current_step = step_pulses / (float)Nag_Set_mileage; // 1 step = 1 cm
    float yaw_rad = Nag_Yaw * PI / 180.0f;
    
    N.Current_X += current_step * (-sinf(yaw_rad));
    N.Current_Y += current_step * cosf(yaw_rad);

    // === 2. 终点停车判定 ===
    if(N.Save_index <= 10 || N.Run_index >= N.Save_index - 10) {
        N.Nag_Stop_f = 1; 
        N.Final_Out = 0;
        SpeedPID.Target = 0.0f; 
        return;
    }

    // ★ 已经彻底删除了 calc_div 的 10ms 限制，现在每 2ms 都会往下执行！

    // === 3. 动态预瞄距离 L ===
    float current_speed = fabs(SpeedPID.Target); 
    
    // ★ 核心修复 1：既然 1个点=1cm，预瞄距离必须拉长！
    // 设基础预瞄为 25cm，速度越快看得越远
    float L_distance = 3.0f + current_speed * 0.13f; 
    if(L_distance > 80.0f) L_distance = 80.0f; // 最多看前方 80cm
    
    // 算力优化：提前算好平方，下面比对时就不需要开根号了
    float L_dist_sq = L_distance * L_distance; 

    // === 4. 寻找定位点 (最近点) ===
    float min_dist_sq = 99999999.0f;
    int closest_index = N.Run_index;
    
    int search_start = N.Run_index - 40;
    if(search_start < 0) search_start = 0;
    int search_end = N.Run_index + 40;
    if(search_end >= N.Save_index) search_end = N.Save_index - 1;
    
    for(int i = search_start; i <= search_end; i++) {
        float dx = Nav_Record_Buffer[i].x - N.Current_X;
        float dy = Nav_Record_Buffer[i].y - N.Current_Y;
        
        // 算力优化：直接用平方值对比
        float dist_sq = dx*dx + dy*dy; 
        if(dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            closest_index = i;
        }
    }
    N.Run_index = closest_index; 

    // === 5. 寻找预瞄点 (目标点) ===
    int target_idx = closest_index;
    
    // 搜索窗口必须大于最大预瞄距离 (100)，否则找不到点
    int max_search_idx = closest_index + 100;
    if(max_search_idx > N.Save_index) max_search_idx = N.Save_index;

    for(int i = closest_index; i < max_search_idx; i++) {
        float dx = Nav_Record_Buffer[i].x - N.Current_X;
        float dy = Nav_Record_Buffer[i].y - N.Current_Y;
        float dist_sq = dx*dx + dy*dy;
        
        if(dist_sq >= L_dist_sq) { 
            target_idx = i;
            break;
        }
        target_idx = i;
    }

    // === 6. 纯跟踪曲率解算 ===
    float dx = Nav_Record_Buffer[target_idx].x - N.Current_X;
    float dy = Nav_Record_Buffer[target_idx].y - N.Current_Y;
    
    float e_lat = -dx * cosf(yaw_rad) - dy * sinf(yaw_rad);
    
    // 整个中断 2ms 只在这里开 1 次根号，对 CPU 毫无压力
    float L_actual = sqrtf(dx*dx + dy*dy);
    if(L_actual < 10.0f) L_actual = 10.0f; // 防除0

    float curvature = 2.0f * e_lat / (L_actual * L_actual);
    
    // ★ 核心修复 2：因为 L 放大了，L的平方放大了几十倍，这里的系数也要相应增加，保证转向力
    N.Final_Out = curvature * 1500.0f; 

    float max_turn = 50.0f; 
    if(N.Final_Out > max_turn)  N.Final_Out = max_turn;
    if(N.Final_Out < -max_turn) N.Final_Out = -max_turn;
    
    // === 7. 纵向速度规划 ===
    if (N.Save_index > N.Run_index) 
    {
        // 1. 核心参数设定
        float max_straight_speed = 100.0f; // 【参数】直道极限速度 (大胆往上提)
        float min_corner_speed   = 80.0f; // 【参数】最急的弯允许的最低速度
        float finish_min_speed   = 20.0f; // 【参数】终点冲线速度
        
        // 2. 弯道动态限速 (根据曲率计算)
        // 曲率越大(弯越急)，减速越多。50000.0f 是灵敏度系数，需根据实际曲率大小微调
        float abs_curv = fabsf(curvature);
        float corner_target_v = max_straight_speed - abs_curv * 50000.0f; 
        
        // 限制弯道最低速度，防止在弯心停下
        if(corner_target_v < min_corner_speed) {
            corner_target_v = min_corner_speed;
        }

        // 默认期望速度等于弯道限速
        float expected_v = corner_target_v;

        // 3. 终点提前减速逻辑 (优先级最高)
        uint16_t decel_distance = 35;     // 离终点还剩多少个点开始刹车
        uint16_t remain_points = N.Save_index - N.Run_index;
        
        if (remain_points <= decel_distance) {
            float finish_v = ((float)remain_points / decel_distance) * (max_straight_speed - finish_min_speed) + finish_min_speed;
            
            // 如果终点要求的速度比弯道要求的还低，听终点的
            if(finish_v < expected_v) {
                expected_v = finish_v;
            }
        } 

        // 4. 执行速度平滑过渡 (斜率限制)
        float accel_step = 0.06f; // 【参数】直道加速的猛烈程度 (推背感)
        float decel_step = 0.20f; // 【参数】入弯刹车的猛烈程度 (刹车一定要比加速快，防止撞墙)

        if (SpeedPID.Target < expected_v) {
            SpeedPID.Target += accel_step; 
            if(SpeedPID.Target > expected_v) SpeedPID.Target = expected_v;
        } 
        else if (SpeedPID.Target > expected_v) {
            SpeedPID.Target -= decel_step; 
            if(SpeedPID.Target < expected_v) SpeedPID.Target = expected_v;
        }
    }
}

// 主任务 (被 isr.c 调用)
void Nag_System(void)
{
    if(N.Nag_SystemRun_Index == 0 || N.Nag_Stop_f == 1) {
        N.Final_Out = 0;
        return;
    }

    if(N.Nag_SystemRun_Index == 1) {
        if(N.End_f) {
            N.Nag_Stop_f = 1; 
            N.Nag_SystemRun_Index = 0; 
            N.End_f = 0;
        } else {
            Run_Nag_Save();
        }
    }
    else if(N.Nag_SystemRun_Index == 3) {
        // 在这里，复现模式不用再做 Angle_Run - Nag_Yaw 的差值了
        // 因为 N.Final_Out 已经在 Run_Nag_GPS 的纯跟踪里算出来了
        Run_Nag_GPS(); 
    }
}

// 计算最短转角误差 (返回值范围: -180 到 180)
float Get_Minor_Arc(float target_angle, float current_angle)
{
    float error = target_angle - current_angle;
    while (error > 180.0f)  error -= 360.0f;
    while (error < -180.0f) error += 360.0f;
    return error;
}

// ==========================================================
// H题 模式2 & 模式3 核心状态机 (放在主循环或TIM2中运行)
// ==========================================================
volatile uint8_t Auto_Drive_State = 0;     
volatile float Auto_Drive_Distance = 0.0f; 
volatile uint8_t Loop_Count = 0;           

void Run_Auto_Drive_Logic(uint8_t mode) 
{
	uint8_t dir = 0;
	
    if (mode == 2) dir = 1;      // 阶段1是右半圆
    else if(mode == 3)// 模式3
	{
		if (Auto_Drive_State == 1) dir = 2;      // 模式3：C->B 逆时针，是左弯
        else if (Auto_Drive_State == 3) dir = 1; // 模式3：D->A 顺时针，是右弯
	}
	
	
    float ir_out = Get_IR_Turn_Out(dir);
    uint8_t is_on_line = (ir_out != 999.0f); 

    // ======================================
    // 模式 2：跑 O 型 (A -> B -> C -> D -> A)
    // ======================================
	//现状：基本成功
    if (mode == 2) 
    {
        switch (Auto_Drive_State)
        {
            case 0: 
                // 分段调速：前 60cm 飙高速，60cm 后减速准备入弯
                if (Auto_Drive_Distance > 15 && Auto_Drive_Distance < 80.0f) {
                    SpeedPID.Target = 90.0f; // 【调参点】满血冲刺速度
                }
				else if(Auto_Drive_Distance <= 15)
				{
					SpeedPID.Target = 80.0f;
				}
				else {
                    SpeedPID.Target = 80.0f; // 【调参点】入弯安全速度
                }
                
                Turn_Target = Get_Minor_Arc(0.0f, Car_Attitude.Yaw) * 2.0f;
                
                if ((Auto_Drive_Distance > 95.0f && is_on_line) || Auto_Drive_Distance > 130.0f) {
                    Trigger_Beep(); 
                    Auto_Drive_State = 1; 
                    Auto_Drive_Distance = 0; 
                }
                break;
                
            case 1: 
                SpeedPID.Target = 75.0f; 
                if(is_on_line) Turn_Target = ir_out; 
                else {
                    // 【调参点5：防丢线强制引导】
                    // 这是一个右半圆，如果速度太快瞬间丢线，绝不能直走，必须强行往右打死方向！
                    // 负数代表右转，正数代表左转（请根据你的实际转向环方向修改正负号）
                    Turn_Target = -90.0f; 
                }
                if (Auto_Drive_Distance > 140.0f && !is_on_line) {
                    Trigger_Beep();
                    Auto_Drive_State = 2; 
                    Auto_Drive_Distance = 0; 
                }
                break;
                
            case 2: 
                // 分段调速：前 60cm 飙高速，60cm 后减速准备入弯
                // 分段调速：前 60cm 飙高速，60cm 后减速准备入弯
                if (Auto_Drive_Distance > 15 && Auto_Drive_Distance < 80.0f) {
                    SpeedPID.Target = 90.0f; // 【调参点】满血冲刺速度
                }
				else if(Auto_Drive_Distance <= 15)
				{
					SpeedPID.Target = 80.0f;
				}
				else {
                    SpeedPID.Target = 80.0f; // 【调参点】入弯安全速度
                }
                
                Turn_Target = Get_Minor_Arc(180.0f, Car_Attitude.Yaw) * 2.0f; 
                
                if ((Auto_Drive_Distance > 95.0f && is_on_line) || Auto_Drive_Distance > 130.0f) {
                    Trigger_Beep(); 
                    Auto_Drive_State = 3; 
                    Auto_Drive_Distance = 0; 
                }
				break;
                
            case 3: 
                SpeedPID.Target = 75.0f; 
                if(is_on_line) Turn_Target = ir_out; 
                else {
                    // 这是左半圆，丢线时强行往左打死方向
                    Turn_Target = -90.0f; 
                }
                if (Auto_Drive_Distance > 140.0f && !is_on_line) {
                    Trigger_Beep();
                    SpeedPID.Target = 0.0f;
                    Turn_Target = 0.0f;
                    Control_Mode = 0; 
                }
                break;
        }
    }
    // ======================================
    // 模式 3：跑 8 字 (A -> C -> B -> D -> A)
    // ======================================
	//现状：参数正负正确，但大小没调好，具体表现为入弯能检测到但转不过来
	//！！！注意：以下数值都要比实际大，或者可以自己测量1cm的的编码器计数来修改Nag_Set_mileage
    else if (mode == 3) 
    {
        switch (Auto_Drive_State)
        {
            case 0: // 阶段 0：A -> C 对角线盲走
                // 【调参点1：对角线分段变速】对角线总长约 128cm
                if (Auto_Drive_Distance > 15 && Auto_Drive_Distance < 90.0f) {
                    SpeedPID.Target = 90.0f; // 【调参点】满血冲刺速度
                }
				else if(Auto_Drive_Distance <= 15)
				{
					SpeedPID.Target = 80.0f;
				}
				else {
                    SpeedPID.Target = 50.0f;
                }
                // 锁定 38.6 度直走 (加上防死亡打转的 Get_Minor_Arc)
                Turn_Target = Get_Minor_Arc(-39.5f, Car_Attitude.Yaw) * 4.0f; 
                
                // 【调参点2：寻迹眼睁开时机】对角线较长，90cm睁眼比较安全
                if ((Auto_Drive_Distance > 110.0f && is_on_line) || Auto_Drive_Distance > 145.0f) {
                    Trigger_Beep();
                    Auto_Drive_State = 1; Auto_Drive_Distance = 0; 
                }
                break;
                
            case 1: // 阶段 1：C -> B 右半圆寻迹
                if (Auto_Drive_Distance > 20 && Auto_Drive_Distance < 80.0f) {
                    SpeedPID.Target = 70.0f; // 【调参点】满血冲刺速度
                }
				else if(Auto_Drive_Distance <= 20)
				{
					SpeedPID.Target = 50.0f;
				}
				else {
                    SpeedPID.Target = 50.0f;
                } // 弯道一定要稳！
                
                if(is_on_line) {
                    Turn_Target = ir_out; 
                } else {
                    // 【调参点3：极其关键的防丢线保护】
                    // 从对角线切入 C 点由于角度刁钻极易瞬间丢线。如果全白了，
                    // 绝不能直走，必须强行给出一个极其猛烈的【右转】指令咬住黑线！
                    Turn_Target = 70.0f; 
                }
                
                if (Auto_Drive_Distance > 160.0f && !is_on_line) {
                    Trigger_Beep();
                    Auto_Drive_State = 2; Auto_Drive_Distance = 0; 
                }
                break;
                
            case 2: // 阶段 2：B -> D 对角线盲走
                // 【调参点1：对角线分段变速】对角线总长约 128cm
                if (Auto_Drive_Distance > 15 && Auto_Drive_Distance < 90.0f) {
                    SpeedPID.Target = 90.0f; // 【调参点】满血冲刺速度
                }
				else if(Auto_Drive_Distance <= 15)
				{
					SpeedPID.Target = 80.0f;
				}
				else {
                    SpeedPID.Target = 50.0f;
                }
                // 从 B 到 D 的绝对航向角是 141.4 度
                Turn_Target = Get_Minor_Arc(219.0f, Car_Attitude.Yaw) * 4.0f; 
                
                if ((Auto_Drive_Distance > 110.0f && is_on_line) || Auto_Drive_Distance > 145.0f) {
                    Trigger_Beep();
                    Auto_Drive_State = 3; Auto_Drive_Distance = 0; 
                }
                break;
                
            case 3: // 阶段 3：D -> A 左半圆寻迹
               if (Auto_Drive_Distance > 20 && Auto_Drive_Distance < 80.0f) {
                    SpeedPID.Target = 70.0f; // 【调参点】满血冲刺速度
                }
				else if(Auto_Drive_Distance <= 20)
				{
					SpeedPID.Target = 50.0f;
				}
				else {
                    SpeedPID.Target = 50.0f;
                } // 弯道一定要稳！ 
                
                if(is_on_line) {
                    Turn_Target = ir_out; 
                } else {
                    // D 到 A 是左弯，丢线时强行给【左转】指令捕捉黑线
                    Turn_Target = -70.0f; 
                }
                
                if (Auto_Drive_Distance > 160.0f && !is_on_line) {
                    Trigger_Beep();
                    Auto_Drive_State = 0; Auto_Drive_Distance = 0; 
                    
                    Loop_Count++;
                    if(Loop_Count >= 4) { // 跑完 4 圈
                        SpeedPID.Target = 0.0f;
                        Turn_Target = 0.0f;
                        Control_Mode = 0; // 停车断电
                    }
                }
                break;
        }
    }
}

//// 复现逻辑 (Repeat)
//void Run_Nag_GPS(void)
//{
//    N.Mileage_All += (float)(L_Mileage + R_Mileage) / 2.0f;

//    if(N.Mileage_All >= Nag_Set_mileage)
//    {
//		N.Run_index++;
//        N.Mileage_All = 0;
//		
//        // 跑完了
//        if(N.Run_index >= N.Save_index) {
//            N.Nag_Stop_f = 1; 
//            N.Final_Out = 0;
//            return;
//        }
//        // =========================================================
//        // ★ 优化二：动态前瞻 (Dynamic Look-ahead)
//        // 公式：最终前瞻 = 基础前瞻 + (车速 × 前瞻系数)
//        // =========================================================
//        float base_look_ahead = 0.0f;  // 基础前瞻 (慢速时的最近距离)
//        float k_look_ahead = 0.14f;     // 速度映射系数 (每增加1cm/s速度，多看多少个点)
//        
//        // 取目标速度的绝对值
//        float current_speed = fabs(SpeedPID.Target); 
//        
//        // 计算动态前瞻索引
//        uint16_t look_ahead = (uint16_t)(base_look_ahead + current_speed * k_look_ahead);
//        
//        // 限制最大前瞻距离，防止数组越界或看太远
//        if(look_ahead > 80) look_ahead = 80; 
//        
//        uint16_t target_idx = N.Run_index + look_ahead;

//        // 边界保护
//        if(target_idx >= N.Save_index) target_idx = N.Save_index - 1;

//        // 读取目标角度
//        N.Angle_Run = (float)(Nav_Record_Buffer[target_idx]) / 100.0f;
//    }
//	
//	// =========================================================
//    // ★★★ 优化四：纵向速度规划 (平滑起步 + 提前减速) ★★★
//    // 放在这里（每2ms执行一次），保证加减速响应极快且平滑
//    // =========================================================
//    if (N.Save_index > N.Run_index) // 确保有数据且没跑完
//    {
//        float cruise_speed = 100.0f;      // 【参数】最高巡航速度
//        float min_speed = 15.0f;          // 【参数】冲线爬行速度 (防止太低导致卡住)
//        uint16_t decel_distance = 25;    // 【参数】提前减速距离 (点数)。速度100较快，建议提前120个点(约1.2米)减速

//        uint16_t remain_points = N.Save_index - N.Run_index;

//        // 【阶段 1：减速阶段】快到终点了
//        if (remain_points <= decel_distance) 
//        {
//            // 线性降速：按比例计算当前期望速度
//            float target_v = ((float)remain_points / decel_distance) * (cruise_speed - min_speed) + min_speed;
//            SpeedPID.Target = target_v; 
//        } 
//        // 【阶段 2：加速/巡航阶段】还在大马路上
//        else 
//        {
//            if (SpeedPID.Target < cruise_speed) {
//                SpeedPID.Target += 0.05f; // 每次增加 0.05 (1秒钟提速 25.0)
//            } else if (SpeedPID.Target > cruise_speed) {
//                SpeedPID.Target = cruise_speed; // 限制不超速
//            }
//        }
//	}
//}

//// 主任务 (被 isr.c 调用)
//void Nag_System(void)
//{
//    if(N.Nag_SystemRun_Index == 0 || N.Nag_Stop_f == 1) 
//    {
//        N.Final_Out = 0;
//        return;
//    }

//    // 录制模式
//    if(N.Nag_SystemRun_Index == 1)
//    {
//        if(N.End_f) {
//            N.Nag_Stop_f = 1; 
//            N.Nag_SystemRun_Index = 0; 
//            N.End_f = 0;
//			
//        } else {
//            Run_Nag_Save();
//        }
//    }
//    // 复现模式
//    else if(N.Nag_SystemRun_Index == 3)
//    {
//        Run_Nag_GPS(); 
//        // 计算偏差：当前Yaw - 目标Yaw (加到转向环)
//        N.Final_Out = N.Angle_Run - Nag_Yaw;
//    }
//}