#include "navigation.h"
#include "flash.h"      // 引入 Flash 存取函数
#include <string.h>

// 定义大数组 (放在 RAM 里)
int32_t Nav_Record_Buffer[MaxSize]; 
Nag N;

// 初始化
void Init_Nag(void)
{
    memset(&N, 0, sizeof(N));
    memset(Nav_Record_Buffer, 0, sizeof(Nav_Record_Buffer));
    
    // 上电尝试从 Flash 加载上次录制的路径
    if(flash_load_nag() == 1)
    {
        // 加载成功 (可选: 蜂鸣器响一声提示)
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
        // 跑完了
        if(N.Run_index >= N.Save_index) {
            N.Nag_Stop_f = 1; 
            N.Final_Out = 0;
            return;
        }

        // 读取目标角度
        N.Angle_Run = (float)(Nav_Record_Buffer[N.Run_index]) / 100.0f;
        N.Run_index++;

        N.Mileage_All = 0;
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
            // 停止录制时，自动保存到 Flash
            if(N.Save_index > 10) flash_save_nag();
        } else {
            Run_Nag_Save();
        }
    }
    // 复现模式
    else if(N.Nag_SystemRun_Index == 3)
    {
        Run_Nag_GPS(); 
        // 计算偏差：当前Yaw - 目标Yaw (加到转向环)
        N.Final_Out = Nag_Yaw - N.Angle_Run;
    }
}