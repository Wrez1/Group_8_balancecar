/*
 * navigation.h
 * 适配方案：RAM录制 + 停车一次性写入Flash
 */

#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

#include "zf_common_headfile.h"
#include "icm20602.h" // 引用姿态，为了获取 Yaw

// ================= 用户设置区域 =================

// 1. 存储空间定义
// 定义 RAM 数组大小 (2000个点 * 4字节 = 8KB)
// 刚好能填满 2 个 Flash 扇区 (Sector 125, 126)
#define MaxSize 2000    

// 2. 里程计阈值
// MG513电机 (30:1减速) -> 2cm 约 153 脉冲
#define Nag_Set_mileage 153 

// 3. 传感器绑定
#define Nag_Yaw Car_Attitude.Yaw // 使用你调好的无漂移 Yaw

// 4. 编码器绑定
// 绑定到 isr.c 里定义的全局累积量
extern int64_t Total_Encoder_L;
extern int64_t Total_Encoder_R;
#define L_Mileage Total_Encoder_L
#define R_Mileage Total_Encoder_R

// ===============================================

// 核心结构体 (删除了所有无关的 Flash 翻页变量)
typedef struct{
       float Final_Out;     // [核心] 输出给转向环的偏差值
       float Mileage_All;   // 当前累积里程 (用于判断是否走过 2cm)
       float Angle_Run;     // 当前复现的目标角度
       uint8 Nag_Stop_f;    // 结束标志 (1=跑完了)
       
       uint16 Run_index;    // 复现进度 (当前跑到第几个点)
       uint16 Save_index;   // 录制总数 (一共录了多少个点)
       uint8 End_f;         // 停止录制命令 (按键触发)
       
       uint8 Nag_SystemRun_Index; // 状态机: 0=空闲, 1=录制, 3=复现
} Nag;

extern Nag N;

// 核心大数组 (所有路径数据都在这)
extern int32_t Nav_Record_Buffer[MaxSize];

// 函数声明
// 只保留这就够了，其他的 Run_Nag_Save 等函数只在 .c 内部使用，不需要暴露
void Init_Nag(void);
void Nag_System(void);

#endif /* _NAVIGATION_H_ */