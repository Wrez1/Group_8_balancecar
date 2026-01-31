#include "zf_common_headfile.h"

#include "encoder.h"

int16 Encoder_Left = 0;
int16 Encoder_Right = 0;

int Speed_L,Speed_R,Speed_L_Temp,Speed_R_Temp;
float Speed_Smooth=0.2; 
float Location=0.0f;


void encoder_init()
{
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // 初始化编码器模块与引脚 正交解码编码器模式
	encoder_clear_count(ENCODER_1);
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // 初始化编码器模块与引脚 正交解码编码器模式
	encoder_clear_count(ENCODER_2);
}

int16_t Get_Count1(void)															
{
	return  -encoder_get_count(ENCODER_1);	
}

int16_t Get_Count2(void)															
{
	return  encoder_get_count(ENCODER_2);	
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     PIT 的中断处理函数 这个函数将在 PIT 对应的定时器中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     pit_handler();
//-------------------------------------------------------------------------------------------------------------------
void pit_encoder_handler(void)
{
    Encoder_Left = encoder_get_count(ENCODER_1); // 获取编码器计数
    encoder_clear_count(ENCODER_1);              // 清空编码器计数

    Encoder_Right = -encoder_get_count(ENCODER_2); // 获取编码器计数
    encoder_clear_count(ENCODER_2);               // 清空编码器计数	
}

void encoder_Read()
{		
				Speed_L=Encoder_Left;
				Speed_R=Encoder_Right;   
			
				Speed_L=Speed_L*Speed_Smooth+Speed_L_Temp*(1-Speed_Smooth);
				Speed_R=Speed_R*Speed_Smooth+Speed_R_Temp*(1-Speed_Smooth);
	
				Location+=(Speed_L+Speed_R);
}

void clear_location(void) 
{
    Location = 0.0f;  // 直接重置为0
}
