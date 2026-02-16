#include "zf_common_headfile.h"
#include "track.h"
#include "sensor.h"  // 假设你有一个包含传感器读取的头文件
#include "pid.h"
#include "led.h"
#include "buzzer.h"
#include "math.h"

// 引入全局变量
extern float Turn_Target;

// 传感器权值数组（4路传感器）
const int8_t Track_Weight[4] = {-3,-1,1,3};  // 根据你传感器的排列和精度调整权值

// 上一次的循迹输出（记忆用）
float Track_Last_Output = 0.0f;

// 是否检测到线
uint8_t Track_Has_Line = 0;

// 计算误差
float Track_Calc_Error(void)
{
    int sum = 0;
    int count = 0;

    // 读取传感器值（1 表示检测到黑线，0 表示没有）
    uint8_t sensor[4] = {D1,D2,D3,D4};  // 假设你有一个 D1-D7 定义传感器状态

    for (int i = 0; i < 4; i++)
    {
        if (sensor[i] == 1)   // 黑线
        {
            sum += Track_Weight[i];
            count++;
        }
    }

    if (count == 0)
    {
        Track_Has_Line = 0;
        return Track_Last_Output;   // 没线时返回上一次值，保持直行
    }
    else
    {
        Track_Has_Line = 1;
        Track_Last_Output = (float)sum / count;
        return Track_Last_Output;   // 计算偏差并记住当前状态
    }
}

// 控制任务（将误差传递给PID控制）
void Track_Control_Task(void)
{
    // 1. 计算偏差
    float track_error = Track_Calc_Error();

    // 2. 设定速度 (比如 3.0 或 5.0，太快容易脱轨)
    SpeedPID.Target = 5.0f; 

    // 3. ★关键对接★：把循迹误差转化为转向目标
    // 系数 3.0f 是“转向灵敏度”，如果转弯转不过来，把这个数改大 (比如 5.0)
    // 如果车子在直线上疯狂摆头，把这个数改小 (比如 1.0)
    Turn_Target = track_error * 3.0f;


}

void led_buzzer(void){
	led_on(1);
	buzzer_on(1);
	system_delay_ms(100);
	led_on(0);
	buzzer_on(0);
}


typedef enum
{
    LINE_NONE = 0,   // 未检测到黑线
    LINE_DETECTED    // 检测到黑线
} line_state_t;

static line_state_t line_state_last = LINE_NONE;
static line_state_t line_state_now  = LINE_NONE;

line_state_t Line_Detect(void)
{
    // 示例：假设 1 = 黑线，0 = 非黑线
    if (Track_Has_Line == 1)
        return LINE_DETECTED;
    else
        return LINE_NONE;
}

void Line_Edge_Check_Task(void)
{
    line_state_now = Line_Detect();

    // ? 关键：状态发生变化
    if (line_state_now != line_state_last)
    {
        led_buzzer();   // 只在变化瞬间调用一次
        line_state_last = line_state_now;
    }
}
