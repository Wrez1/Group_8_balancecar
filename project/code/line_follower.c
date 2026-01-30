#include "line_follower.h"
#include "sensor.h"
#include "systick.h"

// 全局实例
static LineFollower follower;

// 传感器加权值（中间为0，左负右正）
static const int16_t sensor_weights[7] = {
    -30,   // D7（最左）
    -20,   // D6
    -10,   // D5
    0,     // D4（中心）
    10,    // D3
    20,    // D2
    30     // D1（最右）
};

/**
 * @brief 初始化寻迹模块
 */
void line_follower_init(void)
{
    follower.sensor_state = 0;
    follower.line_error = 0;
    follower.sensor_count = 0;
    
    // 赛道状态
    follower.track_type = TRACK_STRAIGHT;
    follower.is_on_arc = 0;
    follower.run_state = STATE_IDLE;
    
    // 控制参数（默认值）
    follower.error_gain = 0.015f;
    follower.arc_speed_factor = 0.7f;
    
    // 统计信息
    follower.straight_timer = 0;
    follower.arc_timer = 0;
    follower.lap_count = 0;
    
//printf("Line follower initialized for H-track.\r\n");
}

/**
 * @brief 读取传感器数据（黑线输出1）
 */
static uint8_t read_sensors(void)
{
    uint8_t state = 0;
    
    // 黑线输出1
    if (D1 == 1) state |= 0x01;  // 最右边
    if (D2 == 1) state |= 0x02;
    if (D3 == 1) state |= 0x04;
    if (D4 == 1) state |= 0x08;  // 中心
    if (D5 == 1) state |= 0x10;
    if (D6 == 1) state |= 0x20;
    if (D7 == 1) state |= 0x40;  // 最左边
    
    return state;
}

/**
 * @brief 计算线路误差
 */
static int16_t calculate_error(uint8_t sensor_state)
{
    int32_t weighted_sum = 0;
    uint8_t count = 0;
    
    for (int i = 0; i < 7; i++) {
        if (sensor_state & (1 << i)) {
            weighted_sum += sensor_weights[i];
            count++;
        }
    }
    
    follower.sensor_count = count;
    
    if (count == 0) {
        return 0;  // 没有检测到黑线
    }
    
    // 计算平均误差并放大
    int16_t error = (int16_t)((weighted_sum / (float)count) * 100.0f);
    
    // 限幅
    if (error > 3000) error = 3000;
    if (error < -3000) error = -3000;
    
    return error;
}

/**
 * @brief 更新赛道状态
 */
static void update_track_status(void)
{
    // 读取传感器
    follower.sensor_state = read_sensors();
    
    // 计算误差
    follower.line_error = calculate_error(follower.sensor_state);
    
    // 判断赛道类型
    if (follower.sensor_count > 0) {
        // 检测到黑线 → 在圆弧上
        follower.track_type = TRACK_ARC;
        follower.is_on_arc = 1;
        follower.straight_timer = 0;
        follower.arc_timer++;
    } else {
        // 没有检测到黑线 → 在直线上
        follower.track_type = TRACK_STRAIGHT;
        follower.is_on_arc = 0;
        follower.straight_timer++;
        follower.arc_timer = 0;
    }
}

/**
 * @brief 更新运行状态（自动状态机）
 */
static void update_run_state(void)
{
    static uint32_t state_timer = 0;
    
    // 只有在运行状态下才更新状态机
    if (follower.run_state == STATE_IDLE) {
        return;
    }
    
    state_timer++;
    
    switch (follower.run_state) {
        case STATE_A_TO_B:
            // A到B：直线100cm
            if (follower.is_on_arc) {
                // 不应该在直线上检测到圆弧，可能提前进入
                follower.run_state = STATE_B_TO_C;
                state_timer = 0;
//                printf("进入B->C段（右半圆弧）\r\n");
            } else if (state_timer > 500) {  // 约5秒走完100cm
                follower.run_state = STATE_B_TO_C;
                state_timer = 0;
//                printf("进入B->C段（右半圆弧）\r\n");
            }
            break;
            
        case STATE_B_TO_C:
            // B到C：右半圆弧（半径40cm）
            if (!follower.is_on_arc) {
                // 圆弧结束
                follower.run_state = STATE_C_TO_D;
                state_timer = 0;
//                printf("进入C->D段（直线）\r\n");
            } else if (state_timer > 800) {  // 约8秒走完圆弧
                follower.run_state = STATE_C_TO_D;
                state_timer = 0;
//                printf("进入C->D段（直线）\r\n");
            }
            break;
            
        case STATE_C_TO_D:
            // C到D：直线100cm
            if (follower.is_on_arc) {
                // 不应该在直线上检测到圆弧
                follower.run_state = STATE_D_TO_A;
                state_timer = 0;
//                printf("进入D->A段（左半圆弧）\r\n");
            } else if (state_timer > 500) {  // 约5秒走完100cm
                follower.run_state = STATE_D_TO_A;
                state_timer = 0;
//                printf("进入D->A段（左半圆弧）\r\n");
            }
            break;
            
        case STATE_D_TO_A:
            // D到A：左半圆弧（半径40cm）
            if (!follower.is_on_arc) {
                // 圆弧结束，完成一圈
                follower.run_state = STATE_IDLE;
                follower.lap_count++;
//                printf("完成第%d圈！\r\n", follower.lap_count);
                state_timer = 0;
            } else if (state_timer > 800) {  // 约8秒走完圆弧
                follower.run_state = STATE_IDLE;
                follower.lap_count++;
//                printf("完成第%d圈！\r\n", follower.lap_count);
                state_timer = 0;
            }
            break;
            
        default:
            break;
    }
}

/**
 * @brief 更新寻迹控制（核心函数）
 * 说明：这个函数会设置TurnPID.Target
 */
void line_follower_update(void)
{
    // 1. 更新传感器状态和误差
    update_track_status();
    
    // 2. 更新运行状态（自动状态机）
    update_run_state();
    
    // 3. 设置转向目标
    if (follower.is_on_arc) {
        // 在圆弧上：使用误差进行转向控制
        TurnPID.Target = follower.line_error * follower.error_gain;
    } else {
        // 在直线上：直行（转向目标为0）
        TurnPID.Target = 0;
    }
}

/**
 * @brief 检查是否在圆弧上
 */
uint8_t line_follower_is_on_arc(void)
{
    return follower.is_on_arc;
}

/**
 * @brief 获取当前误差
 */
int16_t line_follower_get_error(void)
{
    return follower.line_error;
}

/**
 * @brief 获取传感器状态
 */
uint8_t line_follower_get_sensor_state(void)
{
    return follower.sensor_state;
}

/**
 * @brief 获取运行状态
 */
RunState line_follower_get_run_state(void)
{
    return follower.run_state;
}

/**
 * @brief 设置运行状态
 */
void line_follower_set_run_state(RunState state)
{
    follower.run_state = state;
//    printf("运行状态设置为：%d\r\n", state);
}

/**
 * @brief 设置控制参数
 */
void line_follower_set_params(float error_gain, float arc_speed_factor)
{
    follower.error_gain = error_gain;
    follower.arc_speed_factor = arc_speed_factor;
//    printf("寻迹参数：误差增益=%.3f，圆弧速度系数=%.2f\r\n", error_gain, arc_speed_factor);
}

/**
 * @brief 测试传感器
 */
//void line_follower_test_sensors(void)
//{
////    printf("=== 传感器测试模式 ===\r\n");
////    printf("黑线输出1，白区输出0\r\n");
////    printf("按任意键退出测试\r\n\n");
//    
//    while (1) {
//        uint8_t state = read_sensors();
//        int16_t error = calculate_error(state);
//        uint8_t count = 0;
//        
//        for (int i = 0; i < 7; i++) {
//            if (state & (1 << i)) count++;
//        }
//        
//        printf("传感器：");
//        for (int i = 6; i >= 0; i--) {
////            printf("%d", (state >> i) & 0x01);
//        }
////        printf("  检测到%d个黑线  误差：%5d\r", count, error);
//        
//        systick_delay_ms(200);
//        
//        if (get_key()) {
//            printf("\n测试结束\r\n");
//            break;
//        }
//    }
//}

/**
 * @brief 重置寻迹模块
 */
void line_follower_reset(void)
{
    follower.run_state = STATE_IDLE;
    follower.straight_timer = 0;
    follower.arc_timer = 0;
//    printf("寻迹模块已重置\r\n");
}
