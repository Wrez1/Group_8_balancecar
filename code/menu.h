#ifndef _MENU_H_
#define _MENU_H_

#include "zf_common_headfile.h" // 包含常用头文件

// 初始化菜单变量（可选，用于复位状态）
void menu_init(void);

// 核心菜单运行函数，需要在主循环中循环调用
void menu_operation(void);

#endif