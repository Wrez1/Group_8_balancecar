#ifndef __TRACK_H
#define __TRACK_H


// 传感器权值数组（7路传感器）
extern const int8_t Track_Weight[4];

// 上一次的循迹输出（记忆用）
extern float Track_Last_Output;

// 是否检测到线
extern uint8_t Track_Has_Line;

// 计算误差（根据传感器的输出）
float Track_Calc_Error(void);

// 控制任务（将误差传递给PID控制）
void Track_Control_Task(void);

void led_buzzer(void);

void Line_Edge_Check_Task(void);

#endif // __TRACK_H
