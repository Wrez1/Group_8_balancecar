#include "zf_common_headfile.h"

#define SystemCoreClock 72000000
void SysTick_Init_NoIT(void)
{
    SysTick->LOAD = SystemCoreClock / 1000 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}
// 封装毫秒获取函数
uint32_t systick_get_ms()
{
    static uint32_t last_val = 0, ms_cnt = 0;
    uint32_t curr_val = SysTick->VAL;
    if(curr_val < last_val) ms_cnt += (last_val - curr_val)/(SystemCoreClock/1000);
    last_val = curr_val;
    return ms_cnt;
}
