#include "track.h"
#include "sensor.h" 
#include "led.h"
#include "buzzer.h"

// ==========================================================
// 1. 4路红外寻迹偏差计算 (作为眼睛，只返回数据，不乱动PID)
// 返回值：给转向环的 Turn_Target。如果没有压线，返回 999.0f (标志位)
// ==========================================================
float Get_IR_Turn_Out(uint8 curve_dir) 
{
    // 读取传感器 (1 = 黑线，0 = 白底)
    // 假设 D1最左, D2偏左, D3偏右, D4最右
    uint8_t L2 = D1; 
    uint8_t L1 = D2; 
    uint8_t R1 = D3; 
    uint8_t R2 = D4; 
    
    float turn_out = 0.0f;
    uint8_t line_detect = L2 | L1 | R1 | R2;
    
	// ★ 增加静态变量，记住小车上一瞬间的转弯趋势
    static float last_turn_out = 0.0f;
    // 如果全白 (四路都没踩到黑线)
    if (line_detect == 0) 
    {
        return 999.0f; // 999 代表丢线了，交给 H 题状态机去处理(盲走)
    }

    // 寻迹逻辑 (状态穷举法，比队友的加权平均法在转急弯时更凶悍、更稳定)
    // 注意正负号：如果偏左导致右转，符号根据你的车轮实际情况调整
    float Kp_Track = 85.0f; // 【调参点】常规修正力度
    float Kp_Sharp = 117.0f; // 【调参点】急弯修正力度

    // ===============================================
    // ★ 核心机制：弯道方向优先级 (防甩尾屏蔽逻辑)
    // ===============================================
    if (curve_dir == 1) // 【当前大脑知道我们在过右半圆 (B->C)】
    {
        // 右弯时，黑线理应在车身右侧。如果左侧亮了，99%是车速太快甩尾导致的假象！
        if      (R2) turn_out = -Kp_Sharp;          // 压到最右边，全力向右拽回！
        else if (R1) turn_out = -Kp_Track;          // 偏右，正常向右微调
        // --- 下面是防甩尾核心 ---
        else if (L1) turn_out = Kp_Track * 0.6f;    // ★ 甩尾假象！削弱 80% 的反向拉扯力
        else if (L2) turn_out = Kp_Sharp * 0.5f;    // ★ 严重甩尾！削弱 80% 的反向拉扯力
        else         turn_out = last_turn_out;
    }
    else if (curve_dir == 2) // 【当前大脑知道我们在过左半圆 (D->A)】
    {
        if      (L2) turn_out = Kp_Sharp;           // 压到最左边，全力向左拽回！
        else if (L1) turn_out = Kp_Track;           // 偏左，正常向左微调
        // --- 下面是防甩尾核心 ---
        else if (R1) turn_out = -Kp_Track * 0.6f;   // ★ 甩尾假象！削弱 80% 的反向拉扯力
        else if (R2) turn_out = -Kp_Sharp * 0.5f;   // ★ 严重甩尾！削弱 80% 的反向拉扯力
        else         turn_out = last_turn_out;
    }
    else // 【未知情况或直道情况 (curve_dir == 0)】- 维持你的原始逻辑
    {
        if      (L1 && R1)            turn_out = 0.0f;      
        else if (L1 && !R1 && !L2)    turn_out = Kp_Track;  
        else if (R1 && !L1 && !R2)    turn_out = -Kp_Track; 
        else if (L2)                  turn_out = Kp_Sharp;  
        else if (R2)                  turn_out = -Kp_Sharp; 
        else                          turn_out = last_turn_out;      
    }
    
    last_turn_out = turn_out; // 更新记忆
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