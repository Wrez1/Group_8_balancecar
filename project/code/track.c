#include "zf_common_headfile.h"
#include "track.h"
#include "sensor.h"  // 假设你有一个包含传感器读取的头文件
#include "pid.h"
#include "led.h"
#include "buzzer.h"
#include "math.h"
#include "menu.h"
#include "motor.h"

#define TRACK_TASK_PERIOD_MS   10
#define LOST_LINE_TIME_MS      40
#define LOST_LINE_COUNT        (LOST_LINE_TIME_MS / TRACK_TASK_PERIOD_MS)

#define LINE_TASK_PERIOD_MS    10
#define LINE_CONFIRM_TIME_MS  200
#define LINE_CONFIRM_CNT      (LINE_CONFIRM_TIME_MS / LINE_TASK_PERIOD_MS)


static uint16_t lost_line_cnt = 0;




// 传感器权值数组（7路传感器）
const int8_t Track_Weight[4] = {40,8,-8,-40};  // 根据你传感器的排列和精度调整权值

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

		if (lost_line_cnt < LOST_LINE_COUNT)
		{
			lost_line_cnt++;
			return Track_Last_Output;   // 0~0.5s：继续沿用
		}
		else
		{
			return 0.0f;                // ≥0.5s：强制直行
		}
	}
	else
	{
		Track_Has_Line = 1;
		lost_line_cnt = 0;              // ? 一旦找回线，清零
		Track_Last_Output = (float)sum / count;
		return Track_Last_Output;
	}

}

// 控制任务（将误差传递给PID控制）
void Track_Control_Task(void)
{
    float track_error = Track_Calc_Error();
//	float base_speed = 3.0f;
//	float slow_down  = fabsf(track_error) * 0.8f;
//	SpeedPID.Target = base_speed - slow_down;

	if (SpeedPID.Target < 2.0f)
		SpeedPID.Target = 2.0f;

    // 1. 转向控制
    Line_Turn_Target = track_error * 5.0f;

    static uint32_t counter = 0;
    counter++;
	if (counter < 150){
		SpeedPID.Target = 3.0f;
		
	}
	else{
		SpeedPID.Target = 9.0f;
		
	}   // ★给一个前进速度（单位是你的速度环单位）


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
    LINE_RAW_NONE = 0,
    LINE_RAW_DETECTED
} line_raw_state_t;

typedef enum
{
    LINE_CONF_NONE = 0,
    LINE_CONF_DETECTED
} line_confirm_state_t;

static line_raw_state_t     line_raw_now;
static line_confirm_state_t line_conf_state = LINE_CONF_NONE;

static uint16_t line_stable_cnt = 0;

static line_raw_state_t Line_Raw_Detect(void)
{
    return (Track_Has_Line) ? LINE_RAW_DETECTED : LINE_RAW_NONE;
}

uint8 point_round=0;

void Line_Edge_Check_Task(void)
{
    line_raw_now = Line_Raw_Detect();

    // 原始状态和确认状态不同，开始计时
    if ((line_raw_now == LINE_RAW_DETECTED && line_conf_state == LINE_CONF_NONE) ||
        (line_raw_now == LINE_RAW_NONE     && line_conf_state == LINE_CONF_DETECTED))
    {
        line_stable_cnt++;

        if (line_stable_cnt >= LINE_CONFIRM_CNT)
        {
            // ? 确认状态翻转
            line_conf_state = (line_raw_now == LINE_RAW_DETECTED) ?
                               LINE_CONF_DETECTED : LINE_CONF_NONE;
			
            led_buzzer();           // ? 只在“确认翻转”时响一次
            line_stable_cnt = 0;// 清计数
			if(CarMode==2){
				point_round++;
			}
        }
    }
    else
    {
        // 状态一致，清计数
        line_stable_cnt = 0;
    }
	if(CarMode == 2 && point_round >= 4){
		xp=1;
		yp=2;
		SpeedPID.Target = 0.0f;
		Line_Turn_Target = 0.0f;
		point_round=0;
		
	}
}

