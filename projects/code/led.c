#include "zf_common_headfile.h"
#include "led.h"

void led_init()
{
	gpio_init(C0, GPO, GPIO_LOW, GPO_PUSH_PULL);
}

/**
 * @brief  LED发光
 * @param  tim（1开启，0关闭）
 */
void led_on(int16 tim)
{
	if (tim == 1)
	{
		gpio_set_level(C0, GPIO_HIGH);
		return;
	}
	if (tim == 0)
	{
		gpio_set_level(C0, GPIO_LOW);
		return;
	}

}
