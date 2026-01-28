#include "zf_common_headfile.h"
#include "buzzer.h"

void buzzer_init()
{
	gpio_init(C11, GPO, GPIO_LOW, GPO_PUSH_PULL);
}

/**
 * @brief  蜂鸣器警报
 * @param  tim（1开启，0关闭）
 */
void buzzer_on(int16 tim)
{
	if (tim == 1)
	{
		gpio_set_level(C11, GPIO_HIGH);
		return;
	}
	if (tim == 0)
	{
		gpio_set_level(C11, GPIO_LOW);
		return;
	}

}
