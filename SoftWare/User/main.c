#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "zf_device_key.h"
#include "menu.h"

uint8 xp=1,yp=0;
float kp=0.1,ki=0.2,kd=0.3;

int main(void)
{
	debug_init();
	clock_init(SYSTEM_CLOCK_120M);
	key_init(10);
	tft180_init();
	tft180_set_color(RGB565_BLACK, RGB565_WHITE);
	tft180_clear();
	while(1){
		menu(&xp,&yp,&kp,&ki,&kd);
		key_scanner();
		system_delay_ms(10);
	}
}
