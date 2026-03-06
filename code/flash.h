#ifndef _FLASH_H
#define _FLASH_H

#include "zf_common_headfile.h"

// 原有的 PID 保存函数
void flash_save(void);
uint8_t flash_load(void);

// ★★★ 新增：惯导路径保存与读取函数 ★★★
// 返回值 1 表示成功，0 表示失败（比如Flash为空）
void flash_save_nag(void);
uint8_t flash_load_nag(void);

// ★★★ 保存与读取17个导航参数 ★★★
void flash_save_nav_params(void);
void flash_load_nav_params(void);

#endif
