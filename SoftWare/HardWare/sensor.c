#include "sensor.h"


/**
 * @brief 初始化八路灰度/红外传感器
 * 配置为：GPIO输入模式 + 上拉 (Pull-Up)
 */
void sensor_init(void) {
    // 参数说明：gpio_init(引脚, 模式, 默认电平, 配置);
    // 模式: GPI (通用输入)
   
    gpio_init(E8,  GPI, 0, GPI_PULL_UP);
    gpio_init(E9,  GPI, 0, GPI_PULL_UP);
    gpio_init(E10, GPI, 0, GPI_PULL_UP);
    gpio_init(E11, GPI, 0, GPI_PULL_UP);
    gpio_init(E12, GPI, 0, GPI_PULL_UP);
    gpio_init(E13, GPI, 0, GPI_PULL_UP);
    gpio_init(E14, GPI, 0, GPI_PULL_UP);
    gpio_init(E15, GPI, 0, GPI_PULL_UP);
}