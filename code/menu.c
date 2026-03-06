#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "zf_device_key.h"
#include "menu.h"
#include "flash.h"
#include "motor.h" 
#include "encoder.h" // ★★★ 必须添加，否则读不到 Location
#include "pid.h"     // 确保能访问 Control_Mode 和 Target_Location
#include "icm20602.h"
#include "navigation.h"
#include "isr.h"
//#include "buzzer.h"
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

extern float SpeedLeft, SpeedRight;

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
			tft180_show_float(0,60 , TurnPID.Actual, 3 , 3);
			tft180_show_float(0,70 , Car_Attitude.Pitch, 3, 3);
			tft180_show_float(0,80 , SpeedLeft,4,3);
			tft180_show_float(0,90 , SpeedRight,4,3);
			tft180_show_string(0,100,"                        ");
            tft180_show_string(0,110,"                        ");
			break;
	}
}

extern volatile float Auto_Drive_Distance;
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
			tft180_show_float(0,70 , SpeedLeft,4,3);
			tft180_show_float(0,80 , SpeedRight,4,3);
			tft180_show_float(0,90 , Auto_Drive_Distance,4,3);
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

// 增加 sub_mode 参数：0=录制，1=复现
void showplace4(uint8 x, uint8 sub_mode){
    switch(x){
        case 1: // === 准备界面 (Ready) ===
            tft180_show_string(0, 10, "    MODE 4: Inertial   ");
            tft180_show_string(0, 30, "--------------------");
            
            // 根据 sub_mode 显示光标 >
            if(sub_mode == 0) {
                tft180_show_string(0, 50, " > [TEACH] RECORD      ");
                tft180_show_string(0, 70, "   [RUN]   REPLAY      ");
            } else {
                tft180_show_string(0, 50, "   [TEACH] RECORD      ");
                tft180_show_string(0, 70, " > [RUN]   REPLAY      ");
            }
            
            // ★★★ 核心修复：擦除主菜单的残影 (Y=60, 80, 90) ★★★
            tft180_show_string(0, 60, "                       "); 
            tft180_show_string(0, 80, "                       ");
            tft180_show_string(0, 90, "                       ");
            
            tft180_show_string(0, 100," KEY1/2: Select        ");
            tft180_show_string(0, 110," KEY3: GO!             ");
            break;
            
        case 2: // === 运行界面 (Running) ===
		{	
		    static uint8_t refresh_cnt = 0;
            refresh_cnt++;
            if(refresh_cnt < 10) break; // 如果不到 10 次，直接跳出，不去画屏幕！
            refresh_cnt = 0;            // 到了 10 次，清零并往下执行画屏
		
            tft180_show_string(0, 10, "    MODE 4: RUNNING    ");
            tft180_show_string(0, 30, "--------------------");
            
            if(sub_mode == 0) 
                 tft180_show_string(0, 50, "   Status: RECORDING   ");
            else 
                 tft180_show_string(0, 50, "   Status: REPLAYING   ");
            
            // ★★★ 升级 1：显示实时的 X 和 Y 坐标 ★★★
            // 假设横向坐标较短，我们把 X 和 Y 挤在同一行
            tft180_show_string(0, 60, "X:        Y:           ");
            tft180_show_float(15, 60, N.Current_X, 4, 1); // 预留4位整数，1位小数
            tft180_show_float(85, 60, N.Current_Y, 4, 1);
            
            // 显示当前 Yaw
            tft180_show_string(0, 70, "Yaw:                   ");
            tft180_show_float(35, 70, Car_Attitude.Yaw, 3, 1);
            
            // ★★★ 升级 2：智能显示进度 ★★★
            if(sub_mode == 0) {
                // 录制模式：只显示存了多少个点
                tft180_show_string(0, 80, "Saved Pts:             ");
                tft180_show_int(85, 80, N.Save_index, 4);
                tft180_show_string(0, 90, "                       "); // 清空下一行
            } else {
                // 复现模式：显示进度 (Run_index / Save_index)
                tft180_show_string(0, 80, "Run/Total:             ");
                tft180_show_int(85, 80, N.Run_index, 4);
                tft180_show_string(115, 80, "/");
                tft180_show_int(125, 80, N.Save_index, 4);
                
                // (可选)复现模式还可以顺便显示当前的纯跟踪计算曲率输出
                tft180_show_string(0, 90, "Curv Out:              ");
                tft180_show_float(80, 90, N.Final_Out, 3, 2);
            }
                 
            // ★★★ 擦除准备界面的残影 (Line 100 是 KEY1/2 Select) ★★★
            tft180_show_string(0, 100,"                       ");
            
            tft180_show_string(0, 110," KEY4: STOP/SAVE       ");
            break;
		}	
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
			tft180_show_float(0,70 , Car_Attitude.Pitch, 3, 3);
			tft180_show_float(0,80 , SpeedLeft,4,3);
			tft180_show_float(0,90 , SpeedRight,4,3);
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

// 将此函数直接放进 menu.c 中
void showplace_nav_params(uint8_t cursor)
{
	tft180_show_string(9,0,"max:");
	tft180_show_string(9,10,"cor:");
	tft180_show_string(9,20,"fin:");
	tft180_show_float(35,0, max_straight_speed,3,1);
	tft180_show_float(35,10, min_corner_speed,3,1);
	tft180_show_float(35,20, finish_min_speed,3,1);

	tft180_show_string(9,30,"SPEED:");
	tft180_show_string(9,40,"ma0:");
	tft180_show_string(9,50,"st0:");
	tft180_show_string(9,60,"en0:");
	tft180_show_string(9,70,"sp1:");
	tft180_show_string(9,80,"ma2:");
	tft180_show_string(9,90,"st2:");
	tft180_show_string(9,100,"en2:");
	tft180_show_string(9,110,"sp3:");
	tft180_show_float(35,40, m3_Speed_case0_max,3,1);
	tft180_show_float(35,50, m3_Speed_case0_start,3,1);
	tft180_show_float(35,60, m3_Speed_case0_end,3,1);
	tft180_show_float(35,70, m3_Speed_case1,3,1);
	tft180_show_float(35,80, m3_Speed_case2_max,3,1);
	tft180_show_float(35,90, m3_Speed_case2_start,3,1);
	tft180_show_float(35,100, m3_Speed_case2_end,3,1);
	tft180_show_float(35,110, m3_Speed_case3,3,1);
	
	tft180_show_string(80,10,"ANGLE:");
	tft180_show_string(80,20,"ca0_1:");
	tft180_show_string(80,30,"ca0_2:");
	tft180_show_string(80,40,"ca0_3:");
	tft180_show_string(80,50,"ca2_1:");
	tft180_show_string(80,60,"ca2_2:");
	tft180_show_string(80,70,"ca2_3:");
	tft180_show_float(120,20, m3_angle_case0_1,3,1);
	tft180_show_float(120,30, m3_angle_case0_2,3,1);
	tft180_show_float(120,40, m3_angle_case0_3,3,1);
	tft180_show_float(120,50, m3_angle_case2_1,3,1);
	tft180_show_float(120,60, m3_angle_case2_2,3,1);
	tft180_show_float(120,70, m3_angle_case2_3,3,1);
	
	tft180_show_string(0, 0,  (cursor==1) ?">":" "); tft180_show_string(0, 10, (cursor==2) ?">":" ");
    tft180_show_string(0, 20, (cursor==3) ?">":" "); tft180_show_string(0, 40, (cursor==4) ?">":" ");
    tft180_show_string(0, 50, (cursor==5) ?">":" "); tft180_show_string(0, 60, (cursor==6) ?">":" ");
    tft180_show_string(0, 70, (cursor==7) ?">":" "); tft180_show_string(0, 80, (cursor==8) ?">":" ");
    tft180_show_string(0, 90, (cursor==9) ?">":" "); tft180_show_string(0, 100,(cursor==10)?">":" ");
    tft180_show_string(0, 110,(cursor==11)?">":" ");
    tft180_show_string(71, 20, (cursor==12)?">":" "); tft180_show_string(71, 30, (cursor==13)?">":" ");
    tft180_show_string(71, 40, (cursor==14)?">":" "); tft180_show_string(71, 50, (cursor==15)?">":" ");
    tft180_show_string(71, 60, (cursor==16)?">":" "); tft180_show_string(71, 70, (cursor==17)?">":" ");
}

// 全局变量：编辑模式标志
uint8 che = 0;
// 在 menu 函数外部定义静态变量，记住上次的选择
static uint8_t nag_sub_mode = 0; // 0=录制, 1=复现
// 修改后的菜单函数
void menu(uint8* xp, uint8* yp, 
          PID_Params* angle_pid, 
          PID_Params* speed_pid, 
          PID_Params* turn_pid,
          float* mech_zero) {
    
    static uint8 ring_sel = 1;  // 当前选择的环：1-角度环，2-速度环，3-转向环
    static uint8 param_sel = 1; // 当前选择的参数：1-KP，2-KI，3-KD
    static uint8_t nav_cursor = 0; // 0代表非编辑模式，1~17代表正在编辑对应的参数
			  
    if(*yp == 0) {  // 主菜单
		// ==========================================================
        // ★★★ 核心安全机制：只要退回主菜单，全系统强制进入“瘫痪”待机状态 ★★★
        // ==========================================================
        balance_mode_active = 0;   // 1. 关掉底层推力，不再维持平衡
        blue_mode_active = 0;      // 2. 关掉特殊中断权限
        Control_Mode = 0;          // 3. 关掉所有外环(速度/位置/自动驾驶)
        
        SpeedPID.Target = 0.0f;    // 4. 目标速度清零
        Turn_Target = 0.0f;        // 5. 转向目标清零
        
        N.Nag_SystemRun_Index = 0; // 6. 惯导状态机强制切回空闲
        N.Nag_Stop_f = 0;          
        
        motor_control(0, 0);       // 7. 物理级断电，彻底瘫软在地
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
			tft180_clear();
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
					IMU_Instant_Init();
					// 1. 设置为位置模式 (启用位置环->速度环->直立环串级)
                    Control_Mode = 1;
					
					// 2. ★关键★：锁死当前位置！
                    // 如果不写这句，Target_Location 默认为 0，
                    // 而你当前 Location 可能是 5000，车子会疯了一样倒车。
                    // ★★★ 2. 核心：位置清零 ★★★
                    // 告诉 PID：“我现在就在原点，别乱跑”
                    extern float Location;
                    Location = 0.0f; 
                    Target_Location = 0.0f; 	
                    
                    // 3. 重置PID (清除历史积分，防止跳变)
                    //PID_Init(&PositionPID); 
					PID_Init(&SpeedPID);
                    PID_Init(&AnglePID);
                    PID_Init(&TurnPID);
	
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
				// 进入 GO!GO!GO!
                if (*yp == 2) {
                    balance_mode_active = 1; // 1. 开启直立平衡
                    Control_Mode = 2;        // 2. 告诉系统进入模式 2 的状态机
                    
                    // 3. ★★★ 核心：彻底清空状态机历史进度 ★★★
                    extern uint8_t Auto_Drive_State;
                    Auto_Drive_State = 0;       // 回到第0阶段(直道盲走)
                    Auto_Drive_Distance = 0.0f; // 里程清零
                    
                    // 4. ★★★ 核心：重置车头航向角 ★★★
                    // 这相当于告诉小车：“你现在的朝向就是绝对 0 度赛道方向”
                    IMU_Instant_Init();
                    
                    // 5. 清空 PID 防止起步瞬间突变抽搐
                    SpeedPID.Target = 0;
                    Turn_Target = 0;
                    PID_Init(&SpeedPID);
                    PID_Init(&AnglePID);
                    PID_Init(&TurnPID);
                }
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
				
				// === 瞬间进入 GO!GO!GO! 状态 ===
                if (*yp == 2) {
                    balance_mode_active = 1; // 1. 开启底层直立平衡推力
                    Control_Mode = 3;        // 2. 告诉中枢大脑，进入模式3状态机
                    
                    // 3. ★★★ 核心：彻底清空状态机、里程与【跑圈计数】 ★★★
                    extern uint8_t Auto_Drive_State;
                    extern volatile uint8_t Loop_Count;
                    extern volatile float Auto_Drive_Distance;
                    
                    Auto_Drive_State = 0;       // 切回阶段 0：对角线盲走
                    Auto_Drive_Distance = 0.0f; // 里程池子强行抽干
                    Loop_Count = 0;             // 圈数清零，重新算 4 圈
                    
                    // 4. ★★★ 核心：重置车头航向参考系 ★★★
                    // 车头依然要正对 B 点，程序会自动把它定为 0 度，然后自己往 38.6 度歪头
                    IMU_Instant_Init();
                    
                    // 5. 清空残余的油门和方向盘，防止起步瞬间暴走
                    SpeedPID.Target = 0.0f;
                    extern float Turn_Target;
                    Turn_Target = 0.0f;
                    
                    PID_Init(&SpeedPID);
                    PID_Init(&AnglePID);
                    PID_Init(&TurnPID);
                }
            }
            if(*yp > 2) *yp = 1;
            if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
				if(*yp == 2) { // 如果车子正处于狂飙状态
                    balance_mode_active = 0; // 瞬间拔掉底层电机电源
                    Control_Mode = 0;        // 拔掉大脑控制权
                    motor_control(0, 0);     // 瘫软在地
                }
                *yp = 0;
                key_clear_state(KEY_4);
            }
        }
        else if(*xp == 4) {  // MODE4
            showplace4(*yp, nag_sub_mode);
            // === 状态 1: 准备界面 (选择功能) ===
            if(*yp == 1) {
                // KEY1 / KEY2: 切换子模式 (录制/复现)
                if(key_get_state(KEY_1) == KEY_SHORT_PRESS || key_get_state(KEY_2) == KEY_SHORT_PRESS) {
                    nag_sub_mode = !nag_sub_mode; // 0变1，1变0
                    key_clear_state(KEY_1);
                    key_clear_state(KEY_2);
                }
                
                // KEY3: 确认执行 (GO!)
                if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                    *yp = 2; // 进入运行状态
                    key_clear_state(KEY_3);
                    
                    // 重置姿态 (把当前车头设为 0 度)
                    IMU_Instant_Init();
                    
                    if(nag_sub_mode == 0) {
                        // =============== [录制模式] ===============
                        balance_mode_active = 0; // 关平衡
                        blue_mode_active = 1;    // ★开中断权限 (借用蓝牙标志位让ISR运行)
                        Control_Mode = 0;        // 开环
                        
                        // 惯导配置
                        N.Nag_SystemRun_Index = 1; // 1 = 录制状态
                        N.Save_index = 0;          // 从头录
                        N.Mileage_All = 0;
                        N.End_f = 0;
                        
						N.Nag_Stop_f = 0; 
                    
                        // ★★★ 核心修复 2：彻底清空内存中残留的老轨迹，保证新录入干净 ★★★
                        memset(Nav_Record_Buffer, 0, sizeof(Nav_Record_Buffer));
						
						// ★★★ 新增：重置世界原点 ★★★
                        N.Current_X = 0;
                        N.Current_Y = 0;
						
                        // 确保电机无力 (手推模式)
                        SpeedPID.Target = 0;
                        Turn_Target = 0;
                        //motor_control(0,0);
                    } 
                    else {
                        // =============== [复现模式] ===============
                        balance_mode_active = 1; // ★开平衡！
                        blue_mode_active = 0;
                        Control_Mode = 0;        // 1=位置/惯导模式 (ISR里会调用Position_PIDControl)
                        
                        // 惯导配置
                        N.Nag_SystemRun_Index = 3; // 3 = 复现状态
                        N.Run_index = 0;
                        N.Mileage_All = 0;
                        N.Nag_Stop_f = 0;
                        
						// ★★★ 新增：重置世界原点 ★★★
                        N.Current_X = 0;
                        N.Current_Y = 0;
						
                        // PID 重置
                        PID_Init(&SpeedPID);
                        PID_Init(&AnglePID);
                        PID_Init(&TurnPID);
						PID_Init(&PositionPID);
                        
						// 锁死当前位置，防止倒车
						extern float Location;
						Location = 0;
						Target_Location = 0;
						
                        // ★★★ 设定巡航速度 ★★★
                        // 这里通过 PositionPID 间接控制，或者直接给 SpeedPID.Target
                        // 建议先给一个较小的固定速度测试
                        SpeedPID.Target = 15.0f; // 约 30cm/s ? 根据你的编码器换算
                    }
                }
            }
            
            // === 状态 2: 运行中 ===
            if(*yp == 2) {
                // KEY4: 停止/保存
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                    
                    if(nag_sub_mode == 0) {
                        // 如果是录制模式，触发保存
                        N.End_f = 1; 
                        // 在 navigation.c 里会检测 End_f 并执行 flash_save_nag
                        // 这里稍微延时等待一下保存 (简易处理)
						tft180_show_string(0, 50, "   SAVING...           ");
						
						if(N.Save_index > 0) {
                            flash_save_nag(); 
                        }
						
//                        buzzer_on(1);
//						// 这里的延时既给了Flash写入时间，也充当了蜂鸣器的响声时长
//                        system_delay_ms(500); // 响 0.5 秒
//                        
//                        // ★★★ 新增：关闭蜂鸣器 ★★★
//                        buzzer_on(0);
						system_delay_ms(300);
                    }
                    
                    // 统一停车逻辑
                    balance_mode_active = 0;
                    blue_mode_active = 0;
                    N.Nag_SystemRun_Index = 0; // 退出惯导状态机
                    motor_control(0,0);
                    
                    *yp = 1; // 回到准备界面
                    key_clear_state(KEY_4);
                }
            }
			
			// 在准备界面按 KEY4 返回主菜单
            if(*yp == 1 && key_get_state(KEY_4) == KEY_SHORT_PRESS) {
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
					Control_Mode = 0;
					balance_mode_active = 1;
					
					
					// 清空摇杆历史残留指令
                    SpeedPID.Target = 0.0f;
                    extern float Turn_Target;
                    Turn_Target = 0.0f;
					
					PID_Init(&SpeedPID);
                    PID_Init(&AnglePID);
                    PID_Init(&TurnPID);
					
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
            if(nav_cursor == 0) {  // 【非编辑模式】
                // 显示设置界面（传0代表隐藏光标）
                showplace_nav_params(0);  
                
                // KEY3: 进入编辑模式
                if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                    nav_cursor = 1; // 激活光标，默认指向第1个参数
                    key_clear_state(KEY_3);
                }
                // KEY4: 保存并返回主菜单
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
                    flash_save_nav_params();  // 退出前保存一次（以防万一）
                    *yp = 0;  // 返回主菜单
					tft180_clear();
                    key_clear_state(KEY_4);
                }
            }
            else {  // 【编辑模式】
                // 显示编辑界面，带上当前动态光标
                showplace_nav_params(nav_cursor);  
                
                // KEY1: 增加当前选中的参数 (+0.5)
                if(key_get_state(KEY_1) == KEY_SHORT_PRESS) {
                    switch(nav_cursor) {
                        case 1:  max_straight_speed   += 0.5f; break;
                        case 2:  min_corner_speed     += 0.5f; break;
                        case 3:  finish_min_speed     += 0.5f; break;
                        case 4:  m3_Speed_case0_max   += 0.5f; break;
                        case 5:  m3_Speed_case0_start += 0.5f; break;
                        case 6:  m3_Speed_case0_end   += 0.5f; break;
                        case 7:  m3_Speed_case1       += 0.5f; break;
                        case 8:  m3_Speed_case2_max   += 0.5f; break;
                        case 9:  m3_Speed_case2_start += 0.5f; break;
                        case 10: m3_Speed_case2_end   += 0.5f; break;
                        case 11: m3_Speed_case3       += 0.5f; break;
                        case 12: m3_angle_case0_1     += 0.5f; break;
                        case 13: m3_angle_case0_2     += 0.5f; break;
                        case 14: m3_angle_case0_3     += 0.5f; break;
                        case 15: m3_angle_case2_1     += 0.5f; break;
                        case 16: m3_angle_case2_2     += 0.5f; break;
                        case 17: m3_angle_case2_3     += 0.5f; break;
                    }
                    key_clear_state(KEY_1);
                }
                
                // KEY2: 减少当前选中的参数 (-0.5)
                if(key_get_state(KEY_2) == KEY_SHORT_PRESS) {
                    switch(nav_cursor) {
                        case 1:  max_straight_speed   -= 0.5f; break;
                        case 2:  min_corner_speed     -= 0.5f; break;
                        case 3:  finish_min_speed     -= 0.5f; break;
                        case 4:  m3_Speed_case0_max   -= 0.5f; break;
                        case 5:  m3_Speed_case0_start -= 0.5f; break;
                        case 6:  m3_Speed_case0_end   -= 0.5f; break;
                        case 7:  m3_Speed_case1       -= 0.5f; break;
                        case 8:  m3_Speed_case2_max   -= 0.5f; break;
                        case 9:  m3_Speed_case2_start -= 0.5f; break;
                        case 10: m3_Speed_case2_end   -= 0.5f; break;
                        case 11: m3_Speed_case3       -= 0.5f; break;
                        case 12: m3_angle_case0_1     -= 0.5f; break;
                        case 13: m3_angle_case0_2     -= 0.5f; break;
                        case 14: m3_angle_case0_3     -= 0.5f; break;
                        case 15: m3_angle_case2_1     -= 0.5f; break;
                        case 16: m3_angle_case2_2     -= 0.5f; break;
                        case 17: m3_angle_case2_3     -= 0.5f; break;
                    }
                    key_clear_state(KEY_2);
                }
                
                // KEY3: 切换参数 (1~17 循环)
                if(key_get_state(KEY_3) == KEY_SHORT_PRESS) {
                    nav_cursor++;
                    if(nav_cursor > 17) nav_cursor = 1; 
                    key_clear_state(KEY_3);
                }
                
                // KEY4: 保存并退出编辑模式
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS) {
					flash_save_nav_params();
                    nav_cursor = 0;					// 回到非编辑模式
                    key_clear_state(KEY_4);
                }
            }
        }
    }
}
