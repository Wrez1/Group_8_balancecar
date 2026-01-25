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
    tft180_show_string(10, 8, "SmartCar Menu");
    tft180_show_string(0, 20, "----------------");

    // 根据当前索引绘制菜单项
    tft180_show_string(0, 30,  (g_current_mode_index == MODE_1_BALANCE) ? "> 1.Balance"    : "  1.Balance");
	tft180_show_string(0, 40,  (g_current_mode_index == MODE_2_ROUTE_1) ? "> 2.Route A-D"  : "  2.Route A-D");
    tft180_show_string(0, 50,  (g_current_mode_index == MODE_3_ROUTE_2) ? "> 3.Race 4Laps" : "  3.Race 4Laps");
    tft180_show_string(0, 60,  (g_current_mode_index == MODE_4_TEACH)   ? "> 4.Teach Mode" : "  4.Teach Mode");
    tft180_show_string(0, 70, (g_current_mode_index == MODE_5_REMOTE)  ? "> 5.Remote"     : "  5.Remote");
    
    tft180_show_string(0, 80, "K1:Next K2:Ok");
    
    g_refresh_needed = 0; // 清除刷新标志
}

// 绘制模式详情界面
static void _draw_mode_page(void) {
    if(g_refresh_needed == 1) return;
    
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
    
    tft180_show_string(0, 100, "Hold K4 to Exit");
    
    g_refresh_needed = 0;
}

// ================= 外部接口实现 =================

// 初始化状态变量
void menu_init(void) {
    g_sys_state = SYS_MENU_SELECT;
    g_current_mode_index = MODE_1_BALANCE;
    g_refresh_needed = 1;
	
	timer_init(TIM_6, TIMER_MS); 
    timer_start(TIM_6);
	
	key_init(5);
}

// 菜单主循环操作函数
void menu_operation(void) {
    static uint16_t last_scan_time = 0;
    uint16_t current_time = timer_get(TIM_6);

    // 【核心】每 5ms 才允许运行一次按键逻辑
    if ((uint16_t)(current_time - last_scan_time) >= 5) 
    {
        last_scan_time = current_time;

        // --- 按键处理区域 ---
        key_scanner(); // 库函数扫描
        
        // 获取状态
        key_state_enum k1 = key_get_state(KEY_1);
        key_state_enum k2 = key_get_state(KEY_2);
        key_state_enum k4 = key_get_state(KEY_4);
        
        // 状态机
        switch (g_sys_state) {
            case SYS_MENU_SELECT:
                _draw_main_menu();
                // 只有消抖后，这里才不会连发
                if (k1 == KEY_SHORT_PRESS) {
                    g_current_mode_index++;
                    if (g_current_mode_index >= MODE_COUNT) g_current_mode_index = 0;
                    g_refresh_needed = 1; 
                }
                // 确认进入
                if (k2 == KEY_SHORT_PRESS) {
                    g_sys_state = SYS_MODE_DISPLAY;
                    g_refresh_needed = 1; 
                }
                break;
                
            case SYS_MODE_DISPLAY:
                _draw_mode_page();
                if (k4 == KEY_LONG_PRESS) {
                    g_sys_state = SYS_MENU_SELECT;
                    g_refresh_needed = 1; 
                }
                break;
        }
        key_clear_all_state(); 
    }
}