#ifndef _LINE_FOLLOWER_H
#define _LINE_FOLLOWER_H

#include "zf_common_headfile.h"
#include "pid.h"

// 赛道状态
typedef enum {
    TRACK_STRAIGHT = 0,     // 直线（无黑线）
    TRACK_ARC = 1           // 圆弧（有黑线）
} TrackType;

// 运行状态
typedef enum {
    STATE_IDLE = 0,         // 空闲
    STATE_A_TO_B = 1,       // A到B段（直线）
    STATE_B_TO_C = 2,       // B到C段（右半圆弧）
    STATE_C_TO_D = 3,       // C到D段（直线）
    STATE_D_TO_A = 4        // D到A段（左半圆弧）
} RunState;

// 寻迹控制结构体
typedef struct {
    // 传感器状态
    uint8_t sensor_state;      // 传感器原始状态
    int16_t line_error;        // 线路误差
    uint8_t sensor_count;      // 检测到黑线的传感器数量
    
    // 赛道信息
    TrackType track_type;      // 当前赛道类型
    uint8_t is_on_arc;         // 是否在圆弧上
    RunState run_state;        // 运行状态
    
    // 控制参数
    float error_gain;          // 误差增益
    float arc_speed_factor;    // 圆弧速度系数
    
    // 统计信息
    uint32_t straight_timer;   // 直线运行计时
    uint32_t arc_timer;        // 圆弧运行计时
    uint16_t lap_count;        // 圈数计数
} LineFollower;

// 函数声明
void line_follower_init(void);
void line_follower_update(void);
uint8_t line_follower_is_on_arc(void);
int16_t line_follower_get_error(void);
uint8_t line_follower_get_sensor_state(void);
RunState line_follower_get_run_state(void);
void line_follower_set_run_state(RunState state);
void line_follower_set_params(float error_gain, float arc_speed_factor);
void line_follower_test_sensors(void);
void line_follower_reset(void);

#endif
