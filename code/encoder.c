#include "zf_common_headfile.h"
#include "encoder.h"

float SpeedLeft = 0.0f;
float SpeedRight = 0.0f;

// === 2. 定义内部静态变量 (替代原来的 Temp) ===
static float SpeedLeft_Prev = 0.0f;
static float SpeedRight_Prev = 0.0f;

float Speed_Smooth=0.2; 

float Location=0.0f;


void encoder_init()
{
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // 初始化编码器模块与引脚 正交解码编码器模式
	encoder_clear_count(ENCODER_1);
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // 初始化编码器模块与引脚 正交解码编码器模式
	encoder_clear_count(ENCODER_2);
}

// ---------------------------------------------------------
// ★★★ 核心函数：读取 -> 清零 -> 纠正方向 -> 滤波 ★★★
// ---------------------------------------------------------
void encoder_Get_Speed(void)
{
    // 1. 读取原始脉冲 (int16)
    // 只有几十，绝对不会溢出
    int16_t raw_L = encoder_get_count(ENCODER_1);
    int16_t raw_R = encoder_get_count(ENCODER_2);
    
    // 2. 立刻清零 (增量模式)
    encoder_clear_count(ENCODER_1); 
    encoder_clear_count(ENCODER_2); 
    
    // 3. 修正方向 (根据你的屏幕显示经验)
    // 左轮：Get_Count1 是 -encoder，所以这里加负号
    float current_L = -(float)raw_L; 
    
    // 右轮：Get_Count2 是 +encoder，所以这里保持正号
    float current_R =  (float)raw_R;  
    
    // 4. 低通滤波 (Int 转 Float 并平滑)
    SpeedLeft  = current_L * Speed_Smooth + SpeedLeft_Prev  * (1.0f - Speed_Smooth);
    SpeedRight = current_R * Speed_Smooth + SpeedRight_Prev * (1.0f - Speed_Smooth);
    
    // 更新历史值
    SpeedLeft_Prev = SpeedLeft;
    SpeedRight_Prev = SpeedRight;
    
    // 5. 累加位置
    Location += (SpeedLeft + SpeedRight) * 0.5f; 
}

void clear_location(void) 
{
    Location = 0.0f;  // 直接重置为0
}

