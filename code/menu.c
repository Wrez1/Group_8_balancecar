#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "zf_device_key.h"
#include <stdio.h> // 用于 sprintf
#include "menu.h"

void showpalce0(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0,10, "    MAIN MANU          ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50," >MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 2:
			tft180_show_string(0,10, "    MAIN MANU          ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60," >MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 3:
			tft180_show_string(0,10, "    MAIN MANU          ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70," >MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 4:
			tft180_show_string(0,10, "    MAIN MANU          ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80," >MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 5:
			tft180_show_string(0,10, "    MAIN MANU          ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90," >MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 6:
			tft180_show_string(0,10, "    MAIN MANU          ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
			tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control ");
			tft180_show_string(0,100," >PID setting");
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

		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 1 balance     ");
			tft180_show_string(0, 50, "    GO!GO!GO!          ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");

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
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 2: run circle ");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
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

		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 3: run 8      ");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
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
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 4: path memory");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
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
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 5: remote     ");
			tft180_show_string(0, 50, "   GO!GO!GO!           ");
			tft180_show_string(0,60 ,"                        ");
			tft180_show_string(0,70 ,"                        ");
			tft180_show_string(0,80 ,"                        ");
			tft180_show_string(0,90 ,"                        ");
			tft180_show_string(0,100,"                        ");
			break;
	}
}

// ★★★ 新增：XP=6 PID 选择页面 (选择调哪个环) ★★★
void showpalce6(uint8 yp)
{
    tft180_show_string(0, 10, "   SELECT PID   ");
    tft180_show_string(0, 30, "----------------");
    
    // 显示光标 >
    tft180_show_string(0, 40, (yp==1)?"> Angle (Out)":"  Angle (Out)");
    tft180_show_string(0, 60, (yp==2)?"> Gyro  (In) ":"  Gyro  (In) ");
    tft180_show_string(0, 80, (yp==3)?"> Speed PID  ":"  Speed PID  ");
    tft180_show_string(0,100, (yp==4)?"> Turn  PID  ":"  Turn  PID  ");
}

// ★★★ 改造：XP=7 PID 数值修改页面 (实时显示蓝牙改动) ★★★
// 参数 current_pid 是当前选中的那个环
void showpalce7(uint8 yp, PID_t *current_pid, char *title)
{
    char txt[30];
    
    tft180_show_string(0, 10, title); // 显示标题，如 "ANGLE PID"
    tft180_show_string(0, 30, "----------------");
    
    // P值
    if(yp == 1) tft180_show_string(0, 50, ">>"); // 光标
    else        tft180_show_string(0, 50, "  ");
    // 使用 %-6.2f 格式化，直接读取结构体里的最新值
    sprintf(txt, "KP: %-6.3f", current_pid->Kp); 
    tft180_show_string(20, 50, txt);
    
    // I值
    if(yp == 2) tft180_show_string(0, 70, ">>");
    else        tft180_show_string(0, 70, "  ");
    sprintf(txt, "KI: %-6.3f", current_pid->Ki);
    tft180_show_string(20, 70, txt);
    
    // D值
    if(yp == 3) tft180_show_string(0, 90, ">>");
    else        tft180_show_string(0, 90, "  ");
    sprintf(txt, "KD: %-6.3f", current_pid->Kd);
    tft180_show_string(20, 90, txt);
}

uint8 che=0;
// ★★★ 主菜单函数逻辑修改 ★★★
// 增加静态变量来记录当前选了哪个 PID
static PID_t *target_pid = NULL; 
static char *target_title = "";

void menu(uint8* xp, uint8* yp, PID_t *angle_pid, PID_t *gyro_pid, PID_t *speed_pid, PID_t *turn_pid)
{
    // === 主菜单逻辑 (完全保留原样) ===
    if(*yp==0){
        showpalce0(*xp);
        if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
            *xp+=1;
            key_clear_state(KEY_1);
        }
        if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
            *xp-=1;
            key_clear_state(KEY_2);
        }
        if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
            *yp+=1; // 进入子菜单
            key_clear_state(KEY_3);
        }
        if(*xp>6)*xp=1;
        if(*xp<1)*xp=6;
    }
    // === 子菜单逻辑 ===
    else if(*yp!=0){
        // 模式1 (保留原样)
        if(*xp==1){
            showplace1(*yp);
            if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                *yp+=1;
                key_clear_state(KEY_3);
            }
            if(*yp>2)*yp=1;
            if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
                *yp=0;
                key_clear_state(KEY_4);
            }           
        }
        // 模式2 (保留原样)
        else if(*xp==2){
            showplace2(*yp);
            if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                *yp+=1;
                key_clear_state(KEY_3);
            }
            if(*yp>2)*yp=1;
            if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
                *yp=0;
                key_clear_state(KEY_4);
            }           
        }
        // 模式3 (保留原样)
        else if(*xp==3){
            showplace3(*yp);
            if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                *yp+=1;
                key_clear_state(KEY_3);
            }
            if(*yp>2)*yp=1;
            if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
                *yp=0;
                key_clear_state(KEY_4);
            }   
        }
        // 模式4 (保留原样)
        else if(*xp==4){
            showplace4(*yp);
            if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                *yp+=1;
                key_clear_state(KEY_3);
            }
            if(*yp>2)*yp=1;
            if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
                *yp=0;
                key_clear_state(KEY_4);
            }   
        }
        // 模式5 (保留原样)
        else if(*xp==5){
            showplace5(*yp);
            if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                *yp+=1;
                key_clear_state(KEY_3);
            }
            if(*yp>2)*yp=1;
            if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
                *yp=0;
                key_clear_state(KEY_4);
            }
        }
        
        // ★★★ XP=6: PID 环选择页面 (新逻辑) ★★★
        else if(*xp==6){
            // 调用选择页显示函数 (Angle/Gyro/Speed/Turn)
            showpalce6(*yp); 
            
            // KEY_1/KEY_2 移动光标选择环 (1-4)
            if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
                *yp+=1;
                key_clear_state(KEY_1);
            }
            if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
                *yp-=1;
                key_clear_state(KEY_2);
            }
            if(*yp>4)*yp=1; // 限制在4个选项内
            if(*yp<1)*yp=4;
            
            // KEY_4 返回主菜单
            if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
                *yp=0;
                key_clear_state(KEY_4);
            }
            
            // KEY_3 确认选择，进入修改页(XP=7)
            if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                // 根据光标位置锁定要修改的 PID
                if(*yp == 1) { target_pid = angle_pid; target_title = "ANGLE PID"; }
                if(*yp == 2) { target_pid = gyro_pid;  target_title = "GYRO  PID"; }
                if(*yp == 3) { target_pid = speed_pid; target_title = "SPEED PID"; }
                if(*yp == 4) { target_pid = turn_pid;  target_title = "TURN  PID"; }
                
                *xp = 7; // 跳转到隐藏的 XP=7 修改状态
                *yp = 1; // 默认选中 P项
                che = 0; // 默认非编辑状态
                key_clear_state(KEY_3);
                tft180_clear(); // 刷屏
            }
        }
        
        // ★★★ XP=7: PID 数值修改页面 (复用你原本的XP=6逻辑) ★★★
        else if(*xp==7 && target_pid != NULL){
            
            // che=0: 移动光标选择 P/I/D
            if(che==0){
                if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
                    *yp+=1;
                    key_clear_state(KEY_1);
                }
                if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
                    *yp-=1;
                    key_clear_state(KEY_2);
                }
                if(*yp>3)*yp=1;
                if(*yp<1)*yp=3;
                
                // KEY_4 返回上一级 (PID选择页)
                if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
                    *xp = 6; // 回到选择页
                    *yp = 1;
                    che = 0;
                    key_clear_state(KEY_4);
                    tft180_clear();
                }
                // KEY_3 切换编辑状态
                if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                    che=!che;
                    key_clear_state(KEY_3);
                }
            }
            // che!=0: 修改数值 (直接操作 target_pid 指向的内存)
            else if(che!=0){
                // 确定步进值 (角速度环 GyroPID 通常较小，可以微调)
                float step_s = 0.1f;
                float step_l = 0.5f;
                if(target_pid == gyro_pid) { step_s = 0.05f; step_l = 0.2f; }
                
                // 修改 KP
                if(*yp==1){
                    if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
                        target_pid->Kp += step_s;
                        key_clear_state(KEY_1);
                    }
                    if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
                        target_pid->Kp -= step_s;
                        key_clear_state(KEY_2);
                    }
                    if(key_get_state(KEY_1)==KEY_LONG_PRESS){
                        target_pid->Kp += step_l;
                        key_clear_state(KEY_1);
                    }
                    if(key_get_state(KEY_2)==KEY_LONG_PRESS){
                        target_pid->Kp -= step_l;
                        key_clear_state(KEY_2);
                    }
                }
                // 修改 KI
                else if(*yp==2){
                    if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
                        target_pid->Ki += step_s;
                        key_clear_state(KEY_1);
                    }
                    if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
                        target_pid->Ki -= step_s;
                        key_clear_state(KEY_2);
                    }
                    if(key_get_state(KEY_1)==KEY_LONG_PRESS){
                        target_pid->Ki += step_l;
                        key_clear_state(KEY_1);
                    }
                    if(key_get_state(KEY_2)==KEY_LONG_PRESS){
                        target_pid->Ki -= step_l;
                        key_clear_state(KEY_2);
                    }
                }
                // 修改 KD
                else if(*yp==3){
                    if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
                        target_pid->Kd += step_s;
                        key_clear_state(KEY_1);
                    }
                    if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
                        target_pid->Kd -= step_s;
                        key_clear_state(KEY_2);
                    }
                    if(key_get_state(KEY_1)==KEY_LONG_PRESS){
                        target_pid->Kd += step_l;
                        key_clear_state(KEY_1);
                    }
                    if(key_get_state(KEY_2)==KEY_LONG_PRESS){
                        target_pid->Kd -= step_l;
                        key_clear_state(KEY_2);
                    }
                }
                // KEY_3 退出编辑
                if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
                    che=!che;
                    key_clear_state(KEY_3);
                }
            }
            
            // 显示修改页面 (需配合之前给的 showpalce7 使用)
            showpalce7(*yp, target_pid, target_title);
        }
    }
}