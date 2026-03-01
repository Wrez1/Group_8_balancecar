#include "track.h"
#include "sensor.h" 
#include "led.h"
#include "buzzer.h"
#include <math.h>   // 使用 fabsf

// ==========================================================
// 1. 8路红外寻迹偏差计算 (作为眼睛，只返回数据，不乱动PID)
// 返回值：给转向环的 Turn_Target。如果没有压线，返回 999.0f (标志位)
// ==========================================================
float Get_IR_Turn_Out(uint8 curve_dir) 
{
    // 读取传感器 (假设 1 = 黑线，0 = 白底)
    // 从左到右依次为 D1~D8
    uint8_t sensors[8] = {D1, D2, D3, D4, D5, D6, D7, D8};
    const float weight[8] = {-7.0f, -5.0f, -3.0f, -1.0f, 1.0f, 3.0f, 5.0f, 7.0f};
    
    float sum = 0.0f;
    int count = 0;
    uint8_t line_detect = 0;
    
    // 计算加权和以及黑线数量
    for (int i = 0; i < 8; i++) {
        if (sensors[i] == 1) { // 检测到黑线
            sum += weight[i];
            count++;
        }
    }
    
    
    // 如果全白 (八路都没踩到黑线)
    if (count == 0 && !D8 && !D7) 
    {
        return 999.0f; // 999 代表丢线了，交给 H 题状态机去处理(盲走)
    }

    // 计算黑线中心位置（加权平均）
    float center = sum / count; // 范围 -7 ~ 7
    float abs_center = fabsf(center);

    // 寻迹逻辑：比例控制，结合弯道方向削弱防甩尾
    // 定义比例系数：常规修正力度，急弯修正力度（根据偏移大小自动切换）
    float Kp_Track = 80.0f;  // 【调参点】常规修正力度（偏移较小时）
    float Kp_Sharp = 110.0f;  // 【调参点】急弯修正力度（偏移较大时）

    float turn_out = 0.0f;
    // 根据中心偏移大小选择比例系数
    float kp = (abs_center > 4.0f) ? Kp_Sharp : Kp_Track;

    // ===============================================
    // ★ 核心机制：弯道方向优先级 (防甩尾屏蔽逻辑)
    // ===============================================
    if (curve_dir == 1) // 【当前大脑知道我们在过右半圆】
    {
        // 右弯时，黑线理应在右侧（center > 0）
        if (center > 0) {
            // 黑线偏左，可能是甩尾，削弱转向力度
            turn_out = -center * kp * 0.7f;  // 削弱系数0.5可调
        } else {
            turn_out = -center * kp;
        }
    }
    else if (curve_dir == 2) // 【当前大脑知道我们在过左半圆】
    {
        // 左弯时，黑线理应在左侧（center < 0）
        if (center < 0) {
            // 黑线偏右，可能是甩尾，削弱转向力度
            turn_out = -center * kp * 0.7f;
        } else {
            turn_out = -center * kp;
        }
    }
    else // 【未知情况或直道情况】
    {
        turn_out = center * kp;
    }

    // 限幅，防止输出过大（可根据实际调整）
    if (turn_out > 3000.0f) turn_out = 3000.0f;
    if (turn_out < -3000.0f) turn_out = -3000.0f;

    return turn_out;
}

// ==========================================================
// 2. 非阻塞式声光提示 (告别 system_delay_ms)
// ==========================================================
static uint16_t beep_timer = 0; // 倒计时变量

// 任何地方想要响蜂鸣器，调用这个函数即可 (瞬间返回，绝对不卡)
void Trigger_Beep(void)
{
    beep_timer = 50; // 50 * 2ms = 100ms
    led_on(1);
    buzzer_on(1);
}

// 把这个函数放到 2ms 定时器中断 (TIM1_UP_IRQHandler) 里不断循环
void Beep_Flash_Task(void)
{
    if (beep_timer > 0)
    {
        beep_timer--;
        if (beep_timer == 0) // 时间到了，自动关掉
        {
            led_on(0);
            buzzer_on(0);
        }
    }
}
