#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "zf_device_key.h"
#include "menu.h"
#include "flash.h"
#include "motor.h" 
//非编辑模式（PID设置界面）：
// KEY_1: 切换到下一个环（角度环→速度环→转向环）
// KEY_2: 切换到上一个环（转向环→速度环→角度环）
// KEY_3: 进入编辑模式
// KEY_4: 返回上级菜单（主菜单）
//编辑模式（调节参数）：
// KEY_1: 增加当前参数值
// KEY_2: 减少当前参数值
// KEY_3: 切换参数（KP→KI→KD→KP）
// KEY_4: 退出编辑模式，回到非编辑模式

extern uint8_t balance_mode_active;
extern uint8_t blue_mode_active;
// 主菜单显示函数
void showpalce0(uint8 x){
    switch(x){
        case 1:
            tft180_show_string(0,10, "    MAIN MENU          ");
            tft180_show_string(0,30, "----------------");
            tft180_show_string(0,50," >MODE1: balance    ");
            tft180_show_string(0,60,"  MODE2: run circle");
            tft180_show_string(0,70,"  MODE3: run 8");
            tft180_show_string(0,80,"  MODE4: path memory");
            tft180_show_string(0,90,"  MODE5: remote control");
            tft180_show_string(0,100,"  PID setting");
            tft180_show_string(0,110,"  Mechanical zero         ");  // ★★★ 新增项 ★★★
            break;
        case 2:
            tft180_show_string(0,10, "    MAIN MENU          ");
            tft180_show_string(0,30, "----------------");
            tft180_show_string(0,50,"  MODE1: balance    ");
            tft180_show_string(0,60," >MODE2: run circle");
            tft180_show_string(0,70,"  MODE3: run 8");
            tft180_show_string(0,80,"  MODE4: path memory");
            tft180_show_string(0,90,"  MODE5: remote control");
            tft180_show_string(0,100,"  PID setting");
            tft180_show_string(0,110,"  Mechanical zero        ");
            break;
        case 3:
            tft180_show_string(0,10, "    MAIN MENU          ");
            tft180_show_string(0,30, "----------------");
            tft180_show_string(0,50,"  MODE1: balance    ");
            tft180_show_string(0,60,"  MODE2: run circle");
            tft180_show_string(0,70," >MODE3: run 8");
            tft180_show_string(0,80,"  MODE4: path memory");
            tft180_show_string(0,90,"  MODE5: remote control");
            tft180_show_string(0,100,"  PID setting");
            tft180_show_string(0,110,"  Mechanical zero        ");
            break;
        case 4:
            tft180_show_string(0,10, "    MAIN MENU          ");
            tft180_show_string(0,30, "----------------");
            tft180_show_string(0,50,"  MODE1: balance    ");
            tft180_show_string(0,60,"  MODE2: run circle");
            tft180_show_string(0,70,"  MODE3: run 8");
            tft180_show_string(0,80," >MODE4: path memory");
            tft180_show_string(0,90,"  MODE5: remote control");
            tft180_show_string(0,100,"  PID setting");
            tft180_show_string(0,110,"  Mechanical zero        ");
            break;
        case 5:
            tft180_show_string(0,10, "    MAIN MENU          ");
            tft180_show_string(0,30, "----------------");
            tft180_show_string(0,50,"  MODE1: balance    ");
            tft180_show_string(0,60,"  MODE2: run circle");
            tft180_show_string(0,70,"  MODE3: run 8");
            tft180_show_string(0,80,"  MODE4: path memory");
            tft180_show_string(0,90," >MODE5: remote control");
            tft180_show_string(0,100,"  PID setting");
            tft180_show_string(0,110,"  Mechanical zero        ");
            break;
        case 6:
            tft180_show_string(0,10, "    MAIN MENU           ");
            tft180_show_string(0,30, "----------------");
            tft180_show_string(0,50,"  MODE1: balance       ");
            tft180_show_string(0,60,"  MODE2: run circle");
            tft180_show_string(0,70,"  MODE3: run 8");
            tft180_show_string(0,80,"  MODE4: path memory");
            tft180_show_string(0,90,"  MODE5: remote control");
            tft180_show_string(0,100," >PID setting");
            tft180_show_string(0,110,"  Mechanical zero        ");
            break;
        case 7:  // ★★★ 新增机械中值设置项 ★★★
            tft180_show_string(0,10, "    MAIN MENU          ");
            tft180_show_string(0,30, "----------------");
            tft180_show_string(0,50,"  MODE1: balance"    );
            tft180_show_string(0,60,"  MODE2: run circle");
            tft180_show_string(0,70,"  MODE3: run 8");
            tft180_show_string(0,80,"  MODE4: path memory");
            tft180_show_string(0,90,"  MODE5: remote control");
            tft180_show_string(0,100,"  PID setting");
            tft180_show_string(0,110," >Mechanical zero        ");
            break;
    }
}

void showplace1(uint8 x){

	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 1 balance     ");
			tft180_show_string(0, 50, "    >READY             ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
            tft180_show_string(0,110,"                        ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 1 balance     ");
			tft180_show_string(0, 50, "    GO!GO!GO!          ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
            tft180_show_string(0,110,"                        ");
			break;
	}
}

void showplace2(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 2: run circle ");
			tft180_show_string(0, 50, "    >READY             ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
		    tft180_show_string(0,110,"                        ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 2: run circle ");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
		    tft180_show_string(0,110,"                        ");
			break;
	}
}

void showplace3(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 3: run 8      ");
			tft180_show_string(0, 50, "   >READY              ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
            tft180_show_string(0,110,"                        ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 3: run 8      ");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
		    tft180_show_string(0,110,"                        ");
			break;
	}
}

void showplace4(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 4: path memory");
			tft180_show_string(0, 50, "  >READY               ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
		    tft180_show_string(0,110,"                        ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 4: path memory");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
		    tft180_show_string(0,110,"                        ");
			break;
	}
}

void showplace5(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 5: remote     ");
			tft180_show_string(0, 50, "   >READY              ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
		    tft180_show_string(0,110,"                        ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 5: remote     ");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
		    tft180_show_string(0,110,"                        ");
			break;
	}
}

// 修改后的PID设置显示函数
void showplace6(uint8 ring_sel, uint8 param_sel, uint8 che, 
                PID_Params* angle_pid, 
                PID_Params* speed_pid, 
                PID_Params* turn_pid) {
    
    // 根据选择的环显示不同的标题
    switch(ring_sel) {
        case 1:  // 角度环
            tft180_show_string(0, 10, "  ANGLE PID SETTING  ");
            tft180_show_string(0, 30, "-------------------");
            
            // 根据参数选择显示光标
            if(param_sel == 1) {
                tft180_show_string(0, 50, " >KP:                ");
                tft180_show_string(0, 70, "  KI:                    ");
                tft180_show_string(0, 90, "  KD:                    ");
            } else if(param_sel == 2) {
                tft180_show_string(0, 50, "  KP:                ");
                tft180_show_string(0, 70, " >KI:                    ");
                tft180_show_string(0, 90, "  KD:                    ");
            } else if(param_sel == 3) {
                tft180_show_string(0, 50, "  KP:                ");
                tft180_show_string(0, 70, "  KI:                    ");
                tft180_show_string(0, 90, " >KD:                    ");
            }
            
            // 显示角度环PID值
            tft180_show_float(40, 50, angle_pid->Kp, 2, 4);
            tft180_show_float(40, 70, angle_pid->Ki, 2, 4);
            tft180_show_float(40, 90, angle_pid->Kd, 2, 4);
            break;
            
        case 2:  // 速度环
            tft180_show_string(0, 10, "  SPEED PID SETTING  ");
            tft180_show_string(0, 30, "-------------------");
            
            // 根据参数选择显示光标
            if(param_sel == 1) {
                tft180_show_string(0, 50, " >KP:                ");
                tft180_show_string(0, 70, "  KI:                    ");
                tft180_show_string(0, 90, "  KD:                    ");
            } else if(param_sel == 2) {
                tft180_show_string(0, 50, "  KP:                ");
                tft180_show_string(0, 70, " >KI:                    ");
                tft180_show_string(0, 90, "  KD:                    ");
            } else if(param_sel == 3) {
                tft180_show_string(0, 50, "  KP:                ");
                tft180_show_string(0, 70, "  KI:                    ");
                tft180_show_string(0, 90, " >KD:                    ");
            }
            
            // 显示速度环PID值
            tft180_show_float(40, 50, speed_pid->Kp, 2, 4);
            tft180_show_float(40, 70, speed_pid->Ki, 2, 4);
            tft180_show_float(40, 90, speed_pid->Kd, 2, 4);
            break;
            
        case 3:  // 转向环
            tft180_show_string(0, 10, "  TURN PID SETTING   ");
            tft180_show_string(0, 30, "-------------------");
            
            // 根据参数选择显示光标
            if(param_sel == 1) {
                tft180_show_string(0, 50, " >KP:                ");
                tft180_show_string(0, 70, "  KI:                    ");
                tft180_show_string(0, 90, "  KD:                    ");
            } else if(param_sel == 2) {
                tft180_show_string(0, 50, "  KP:                ");
                tft180_show_string(0, 70, " >KI:                    ");
                tft180_show_string(0, 90, "  KD:                    ");
            } else if(param_sel == 3) {
                tft180_show_string(0, 50, "  KP:                ");
                tft180_show_string(0, 70, "  KI:                    ");
                tft180_show_string(0, 90, " >KD:                    ");
            }
            
            // 显示转向环PID值
            tft180_show_float(40, 50, turn_pid->Kp, 2, 4);
            tft180_show_float(40, 70, turn_pid->Ki, 2, 4);
            tft180_show_float(40, 90, turn_pid->Kd, 2, 4);
            break;
    }
    
    // 清空底部区域
    tft180_show_string(0, 60, "                       ");
    tft180_show_string(0, 80, "                       ");
    tft180_show_string(0, 100, "                       ");
    
    // 显示当前选择的环
    tft180_show_string(0, 110, "Ring:  ");
    switch(ring_sel) {
        case 1: tft180_show_string(40, 110, "Angle      "); break;
        case 2: tft180_show_string(40, 110, "Speed      "); break;
        case 3: tft180_show_string(40, 110, "Turn       "); break;
    }
}

void showplace7(uint8 che, float mech_zero) {
    // 显示标题
    tft180_show_string(0, 10, " MECHANICAL ZERO SET");
    tft180_show_string(0, 30, "-------------------");
    
    // 显示当前机械中值
    tft180_show_string(0, 50, "Current Value:");
    tft180_show_float(80, 50, mech_zero, 5, 3);  // 显示5位，3位小数
    tft180_show_string(0, 60, "                       ");
    // 显示提示信息
    tft180_show_string(0, 70, "Press KEY1/KEY2 to adjust");
    
    if(che == 0) {
		tft180_show_string(0, 80, "                       ");
        tft180_show_string(0, 90, "Press KEY3 to edit");
		tft180_show_string(0, 100, "                       ");
        tft180_show_string(0, 110, "Press KEY4 to save & exit");
    } else {
		tft180_show_string(0, 80, "                       ");
        tft180_show_string(0, 90, "Editing...              ");
		tft180_show_string(0, 100, "                       ");
        tft180_show_string(0, 110, "Press KEY4 to save");
    }
}

// 全局变量：编辑模式标志
uint8 che = 0;

// 修改后的菜单函数
void menu(uint8* xp, uint8* yp, 
          PID_Params* angle_pid, 
          PID_Params* speed_pid, 
          PID_Params* turn_pid,
          float* mech_zero) {
    
    static uint8 ring_sel = 1;  // 当前选择的环：1-角度环，2-速度环，3-转向环
    static uint8 param_sel = 1; // 当前选择的参数：1-KP，2-KI，3-KD
    static uint8 mech_zero_edit = 0;
	
	if (balance_mode_active) {
            balance_mode_active = 0;
            motor_control(0, 0);
        }  
    if(*yp == 0) {  // 主菜单
        showpalce0(*xp);
        
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS) {
            *xp += 1;
            key_clear_state(KEY_1);
        }
        if(key_get_state(KEY_2) == KEY_SHORT_PRESS) {
            *xp -= 1;
            key_clear_state(KEY_2);
        }
        if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
            *yp += 1;
            key_clear_state(KEY_3);
        }
        
        if(*xp > 7) *xp = 1;
        if(*xp < 1) *xp = 7;
    }
    else if(*yp != 0) {  // 子菜单
        if(*xp == 1) {  // MODE1
            showplace1(*yp);
            if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                *yp += 1;
                key_clear_state(KEY_3);
                // 如果进入GO状态，开启平衡模式
                if (*yp == 2) {
                    balance_mode_active = 1;
                }
            }
            if(*yp > 2) *yp = 1;
            
            if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                *yp = 0;  // 返回主菜单
                key_clear_state(KEY_4);
            }
        }
        else if(*xp == 2) {  // MODE2
            showplace2(*yp);
            if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                *yp += 1;
                key_clear_state(KEY_3);
            }
            if(*yp > 2) *yp = 1;
            if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                *yp = 0;
                key_clear_state(KEY_4);
            }
        }
        else if(*xp == 3) {  // MODE3
            showplace3(*yp);
            if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                *yp += 1;
                key_clear_state(KEY_3);
            }
            if(*yp > 2) *yp = 1;
            if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                *yp = 0;
                key_clear_state(KEY_4);
            }
        }
        else if(*xp == 4) {  // MODE4
            showplace4(*yp);
            if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                *yp += 1;
                key_clear_state(KEY_3);
            }
            if(*yp > 2) *yp = 1;
            if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                *yp = 0;
                key_clear_state(KEY_4);
            }
        }
        else if(*xp == 5) {  // MODE5
            showplace5(*yp);
            if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                *yp += 1;
                key_clear_state(KEY_3);
                if (*yp == 2) {
                    blue_mode_active = 1;
                }
            }
            if(*yp > 2) *yp = 1;
            if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                *yp = 0;
                key_clear_state(KEY_4);
            }
        }
        else if(*xp == 6) {  // PID设置
            if(che == 0) {  // 非编辑模式
                // KEY1/KEY2: 切换环
                if(key_get_state(KEY_1) == KEY_SHORT_PRESS) {
                    ring_sel += 1;
                    if(ring_sel > 3) ring_sel = 1;
                    key_clear_state(KEY_1);
                }
                if(key_get_state(KEY_2) == KEY_SHORT_PRESS) {
                    ring_sel -= 1;
                    if(ring_sel < 1) ring_sel = 3;
                    key_clear_state(KEY_2);
                }
                
                // KEY3: 选中进入编辑模式
                if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                    che = 1;
                    key_clear_state(KEY_3);
                }
                
                // KEY4: 返回上级菜单（主菜单）
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                    *yp = 0;
                    key_clear_state(KEY_4);
                }
            }
            else if(che != 0) {  // 编辑模式
                PID_Params* current_pid;
                
                // 获取当前选中的PID环
                switch(ring_sel) {
                    case 1: current_pid = angle_pid; break;
                    case 2: current_pid = speed_pid; break;
                    case 3: current_pid = turn_pid; break;
                    default: current_pid = angle_pid; break;
                }
                
                // KEY1/KEY2: 增减当前参数
                switch(param_sel) {
                    case 1:  // 调节KP
                        if(key_get_state(KEY_1) == KEY_SHORT_PRESS) {
                            current_pid->Kp += 0.05;
                            key_clear_state(KEY_1);
                        }
                        if(key_get_state(KEY_2) == KEY_SHORT_PRESS) {
                            current_pid->Kp -= 0.05;
                            key_clear_state(KEY_2);
                        }

                        break;
                        
                    case 2:  // 调节KI
                        if(key_get_state(KEY_1) == KEY_SHORT_PRESS) {
                            current_pid->Ki += 0.05;
                            key_clear_state(KEY_1);
                        }
                        if(key_get_state(KEY_2) == KEY_SHORT_PRESS) {
                            current_pid->Ki -= 0.05;
                            key_clear_state(KEY_2);
                        }

                        break;
                        
                    case 3:  // 调节KD
                        if(key_get_state(KEY_1) == KEY_SHORT_PRESS) {
                            current_pid->Kd += 0.05;
                            key_clear_state(KEY_1);
                        }
                        if(key_get_state(KEY_2) == KEY_SHORT_PRESS) {
                            current_pid->Kd -= 0.05;
                            key_clear_state(KEY_2);
                        }

                        break;
                }
                
                // KEY3: 切换参数（在编辑模式下）
                if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                    param_sel += 1;
                    if(param_sel > 3) param_sel = 1;
                    key_clear_state(KEY_3);
                }
                
                // KEY4: 退出编辑模式
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                    che = 0;
                    key_clear_state(KEY_4);
                }
            }
            
            // 显示PID设置界面
            showplace6(ring_sel, param_sel, che, angle_pid, speed_pid, turn_pid);
        }
        else if(*xp == 7) {  // ★★★ 新增：机械中值设置 ★★★
            if(mech_zero_edit == 0) {  // 非编辑模式
                // 显示机械中值设置界面
                showplace7(mech_zero_edit, *mech_zero);  // ★★★ 正确使用mech_zero ★★★
                // KEY3: 进入编辑模式
                if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                    mech_zero_edit = 1;
                    key_clear_state(KEY_3);
                }
                // KEY4: 保存并返回主菜单
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                    flash_save_mech_zero();  // 保存到Flash
                    *yp = 0;  // 返回主菜单
                    key_clear_state(KEY_4);
                }
            }
            else {  // 编辑模式
                // 显示编辑界面
                showplace7(mech_zero_edit, *mech_zero);  // ★★★ 正确使用mech_zero ★★★
                
                // KEY1: 增加机械中值
                if(key_get_state(KEY_1) == KEY_SHORT_PRESS) {
                    *mech_zero += 0.01f;  // 每次增加0.01
                    key_clear_state(KEY_1);
                }
                // KEY2: 减少机械中值
                if(key_get_state(KEY_2) == KEY_SHORT_PRESS) {
                    *mech_zero -= 0.01f;  // 每次减少0.01
                    key_clear_state(KEY_2);
                }
                // KEY4: 保存并退出编辑模式
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                    flash_save_mech_zero();  // 保存到Flash
                    mech_zero_edit = 0;  // 退出编辑模式
                    key_clear_state(KEY_4);
                }
            }
        }
    }
}
