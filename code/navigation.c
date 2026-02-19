#include "navigation.h"
#include "flash.h"      // 引入 Flash 存取函数
#include <string.h>
#include "zf_common_headfile.h"
#include "buzzer.h"
#include "math.h"
#include "pid.h"
// 定义大数组 (放在 RAM 里)
int32_t Nav_Record_Buffer[MaxSize]; 
Nag N;

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
    // 累积里程
    N.Mileage_All += (float)(L_Mileage + R_Mileage) / 2.0f; 
    
    if(N.Mileage_All >= Nag_Set_mileage) 
    {
        // 满了自动停止
        if(N.Save_index >= MaxSize - 1) {
            N.End_f = 1; 
            return;
        }

        // 记录 Yaw (放大100倍存整数)
        Nav_Record_Buffer[N.Save_index] = (int32_t)(Nag_Yaw * 100.0f);
        N.Save_index++;

        // 清零里程累积
        N.Mileage_All = 0; 
    }
}

// 复现逻辑 (Repeat)
void Run_Nag_GPS(void)
{
    N.Mileage_All += (float)(L_Mileage + R_Mileage) / 2.0f;

    if(N.Mileage_All >= Nag_Set_mileage)
    {
		N.Run_index++;
        N.Mileage_All = 0;
		
        // 跑完了
        if(N.Run_index >= N.Save_index) {
            N.Nag_Stop_f = 1; 
            N.Final_Out = 0;
            return;
        }
        // =========================================================
        // ★ 优化二：动态前瞻 (Dynamic Look-ahead)
        // 公式：最终前瞻 = 基础前瞻 + (车速 × 前瞻系数)
        // =========================================================
        float base_look_ahead = 0.0f;  // 基础前瞻 (慢速时的最近距离)
        float k_look_ahead = 0.14f;     // 速度映射系数 (每增加1cm/s速度，多看多少个点)
        
        // 取目标速度的绝对值
        float current_speed = fabs(SpeedPID.Target); 
        
        // 计算动态前瞻索引
        uint16_t look_ahead = (uint16_t)(base_look_ahead + current_speed * k_look_ahead);
        
        // 限制最大前瞻距离，防止数组越界或看太远
        if(look_ahead > 80) look_ahead = 80; 
        
        uint16_t target_idx = N.Run_index + look_ahead;

        // 边界保护
        if(target_idx >= N.Save_index) target_idx = N.Save_index - 1;

        // 读取目标角度
        N.Angle_Run = (float)(Nav_Record_Buffer[target_idx]) / 100.0f;
    }
	
	// =========================================================
    // ★★★ 优化四：纵向速度规划 (平滑起步 + 提前减速) ★★★
    // 放在这里（每2ms执行一次），保证加减速响应极快且平滑
    // =========================================================
    if (N.Save_index > N.Run_index) // 确保有数据且没跑完
    {
        float cruise_speed = 100.0f;      // 【参数】最高巡航速度
        float min_speed = 15.0f;          // 【参数】冲线爬行速度 (防止太低导致卡住)
        uint16_t decel_distance = 25;    // 【参数】提前减速距离 (点数)。速度100较快，建议提前120个点(约1.2米)减速

        uint16_t remain_points = N.Save_index - N.Run_index;

        // 【阶段 1：减速阶段】快到终点了
        if (remain_points <= decel_distance) 
        {
            // 线性降速：按比例计算当前期望速度
            float target_v = ((float)remain_points / decel_distance) * (cruise_speed - min_speed) + min_speed;
            SpeedPID.Target = target_v; 
        } 
        // 【阶段 2：加速/巡航阶段】还在大马路上
        else 
        {
            if (SpeedPID.Target < cruise_speed) {
                SpeedPID.Target += 0.05f; // 每次增加 0.05 (1秒钟提速 25.0)
            } else if (SpeedPID.Target > cruise_speed) {
                SpeedPID.Target = cruise_speed; // 限制不超速
            }
        }
	}
}

// 主任务 (被 isr.c 调用)
void Nag_System(void)
{
    if(N.Nag_SystemRun_Index == 0 || N.Nag_Stop_f == 1) 
    {
        N.Final_Out = 0;
        return;
    }

    // 录制模式
    if(N.Nag_SystemRun_Index == 1)
    {
        if(N.End_f) {
            N.Nag_Stop_f = 1; 
            N.Nag_SystemRun_Index = 0; 
            N.End_f = 0;
			
        } else {
            Run_Nag_Save();
        }
    }
    // 复现模式
    else if(N.Nag_SystemRun_Index == 3)
    {
        Run_Nag_GPS(); 
        // 计算偏差：当前Yaw - 目标Yaw (加到转向环)
        N.Final_Out = N.Angle_Run - Nag_Yaw;
    }
}