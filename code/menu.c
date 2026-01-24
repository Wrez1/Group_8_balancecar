#include "zf_common_headfile.h"
// ================= 内部定义 =================
// 菜单系统的状态
typedef enum {
    SYS_MENU_SELECT,    // 主菜单选择界面
    SYS_MODE_DISPLAY    // 模式详情/运行界面
} SystemState_e;

// 5种发车模式
typedef enum {
    MODE_1_BALANCE = 0,
    MODE_2_ROUTE_1,
    MODE_3_ROUTE_2,
    MODE_4_TEACH,
    MODE_5_REMOTE,
    MODE_COUNT          // 计数用
} RunMode_e;

// 内部全局变量
static SystemState_e g_sys_state = SYS_MENU_SELECT;
static RunMode_e     g_current_mode_index = MODE_1_BALANCE;
static uint8_t       g_refresh_needed = 1; // 1 = 需要刷新屏幕

// ================= 内部辅助函数 (界面绘制) =================

// 绘制主菜单界面
static void _draw_main_menu(void) {
    if(g_refresh_needed == 0) return;

    tft180_clear(); //
    tft180_show_string(10, 5, "SmartCar Menu");
    tft180_show_string(0, 25, "----------------");

    // 根据当前索引绘制菜单项
    tft180_show_string(0, 40,  (g_current_mode_index == MODE_1_BALANCE) ? "> 1.Balance"    : "  1.Balance");
    tft180_show_string(0, 56,  (g_current_mode_index == MODE_2_ROUTE_1) ? "> 2.Route A-D"  : "  2.Route A-D");
    tft180_show_string(0, 72,  (g_current_mode_index == MODE_3_ROUTE_2) ? "> 3.Race 4Laps" : "  3.Race 4Laps");
    tft180_show_string(0, 88,  (g_current_mode_index == MODE_4_TEACH)   ? "> 4.Teach Mode" : "  4.Teach Mode");
    tft180_show_string(0, 104, (g_current_mode_index == MODE_5_REMOTE)  ? "> 5.Remote"     : "  5.Remote");
    
    tft180_show_string(0, 130, "K1:Next K2:Ok");
    
    g_refresh_needed = 0; // 清除刷新标志
}

// 绘制模式详情界面
static void _draw_mode_page(void) {
    if(g_refresh_needed == 0) return;
    
    tft180_clear();
    tft180_show_string(10, 10, "Mode Running...");
    tft180_show_string(0, 25, "----------------");
    
    switch(g_current_mode_index) {
        case MODE_1_BALANCE:
            tft180_show_string(10, 50, "[Mode 1]");
            tft180_show_string(10, 70, "Balance Loop");
            break;
        case MODE_2_ROUTE_1:
            tft180_show_string(10, 50, "[Mode 2]");
            tft180_show_string(10, 70, "Route Task");
            break;
        case MODE_3_ROUTE_2:
            tft180_show_string(10, 50, "[Mode 3]");
            tft180_show_string(10, 70, "Speed Race");
            break;
        case MODE_4_TEACH:
            tft180_show_string(10, 50, "[Mode 4]");
            tft180_show_string(10, 70, "Teach/Replay");
            break;
        case MODE_5_REMOTE:
            tft180_show_string(10, 50, "[Mode 5]");
            tft180_show_string(10, 70, "Remote Ctrl");
        
		default:
            // 捕获 MODE_COUNT 或其他非法值，什么都不做，或者显示出错
            break;
    }
    
    tft180_show_string(0, 140, "Hold K4 to Exit");
    
    g_refresh_needed = 0;
}

// ================= 外部接口实现 =================

// 初始化状态变量
void menu_init(void) {
    g_sys_state = SYS_MENU_SELECT;
    g_current_mode_index = MODE_1_BALANCE;
    g_refresh_needed = 1;
}

// 菜单主循环操作函数
void menu_operation(void) {
    // 1. 扫描按键 (核心库函数)
    key_scanner(); //
    
    // 获取按键状态
    // 请根据实际情况修改 KEY_1, KEY_2 等引脚宏定义
    key_state_enum k1 = key_get_state(KEY_1); // 切换
    key_state_enum k2 = key_get_state(KEY_2); // 确认
    key_state_enum k4 = key_get_state(KEY_4); // 退出
    
    // 2. 状态机逻辑
    switch (g_sys_state) {
        
        // --- 状态：主菜单选择 ---
        case SYS_MENU_SELECT:
            // 绘制菜单
            _draw_main_menu();
            
            // 逻辑：切换选项
            if (k1 == KEY_SHORT_PRESS) {
                g_current_mode_index++;
                if (g_current_mode_index >= MODE_COUNT) g_current_mode_index = 0;
                g_refresh_needed = 1; // 触发刷新
            }
            
            // 逻辑：确认进入
            if (k2 == KEY_SHORT_PRESS) {
                g_sys_state = SYS_MODE_DISPLAY;
                g_refresh_needed = 1; // 触发刷新
            }
            break;
            
        // --- 状态：模式运行/显示 ---
        case SYS_MODE_DISPLAY:
            // 绘制详情页
            _draw_mode_page();
            
            // 可以在这里添加具体的模式运行逻辑调用
            // 例如：if (g_current_mode_index == MODE_1_BALANCE) run_balance_logic();
            
            // 逻辑：长按退出回菜单
            if (k4 == KEY_LONG_PRESS || k4 == KEY_SHORT_PRESS) {
                g_sys_state = SYS_MENU_SELECT;
                g_refresh_needed = 1; // 触发刷新
            }
            break;
    }
    
    // 3. 清除按键状态 (必须调用，否则按键会一直触发)
    key_clear_all_state(); //
}