#include "bluetooth.h"
#include "zf_device_bluetooth_ch9141.h"
#include "pid.h"   
#include "flash.h" 
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

// 接收缓冲区
static char rx_buffer[64];
static uint8_t rx_cnt = 0;

// -------------------------------------------------------------------------
// 内部函数：解析并执行指令
// -------------------------------------------------------------------------
static void Execute_Command(char *str)
{
    // 1. 处理保存命令 "SAVE"
    if(strncasecmp(str, "SAVE", 4) == 0)
    {
        flash_save(); // 保存 PID 到 Flash
        Bluetooth_Send("Save OK!\r\n");
        return;
    }
    
    // 2. 处理查看命令 "VIEW"
    if(strncasecmp(str, "VIEW", 4) == 0)
    {
        Bluetooth_Send("Ang P:%.2f I:%.2f D:%.2f\r\n", AnglePID.Kp, AnglePID.Ki, AnglePID.Kd);
        system_delay_ms(20);
        Bluetooth_Send("Gyr P:%.2f I:%.2f D:%.2f\r\n", GyroPID.Kp, GyroPID.Ki, GyroPID.Kd);
        system_delay_ms(20);
        Bluetooth_Send("Spd P:%.2f I:%.2f D:%.2f\r\n", SpeedPID.Kp, SpeedPID.Ki, SpeedPID.Kd);
        return;
    }

    // 3. 解析 PID 修改指令 (例如 "AP12.5")
    PID_t *target_pid = NULL;
    char *pid_name = "";

    switch(str[0])
    {
        case 'a': case 'A': target_pid = &AnglePID; pid_name = "Angle"; break;
        case 'g': case 'G': target_pid = &GyroPID;  pid_name = "Gyro";  break;
        case 's': case 'S': target_pid = &SpeedPID; pid_name = "Speed"; break;
        case 't': case 'T': target_pid = &TurnPID;  pid_name = "Turn";  break;
        default: return; // 未知指令，忽略
    }

    float val = atof(str + 2);

    if(target_pid != NULL)
    {
        char param_name = ' ';
        switch(str[1])
        {
            case 'p': case 'P': target_pid->Kp = val; param_name = 'P'; break;
            case 'i': case 'I': target_pid->Ki = val; param_name = 'I'; break;
            case 'd': case 'D': target_pid->Kd = val; param_name = 'D'; break;
            default: return;
        }
        Bluetooth_Send("Set %s-%c = %.3f\r\n", pid_name, param_name, val);
    }
}

// -------------------------------------------------------------------------
// 接口函数：初始化
// -------------------------------------------------------------------------
void Bluetooth_Init(void)
{
    // 确保 zf_device_bluetooth_ch9141.h 里波特率是 9600
    bluetooth_ch9141_init(); 
    Bluetooth_Send("Bluetooth Ready!\r\n");
}

// -------------------------------------------------------------------------
// 接口函数：主任务 (放入 while(1))
// -------------------------------------------------------------------------
void Bluetooth_Task(void)
{
    uint8_t dat;
    
    // ★★★ 修正点：使用 read_buffer 读取 1 个字节 ★★★
    // 只要能读到 1 个字节 (返回值为1)，循环就继续
    while(bluetooth_ch9141_read_buffer(&dat, 1))
    {
        if(dat == '\n' || dat == '\r')
        {
            if(rx_cnt > 0) 
            {
                rx_buffer[rx_cnt] = '\0'; 
                Execute_Command(rx_buffer); 
                rx_cnt = 0; 
            }
        }
        else
        {
            if(rx_cnt < 60)
            {
                rx_buffer[rx_cnt++] = dat;
            }
        }
    }
}

// -------------------------------------------------------------------------
// 接口函数：发送格式化数据 (printf 风格)
// -------------------------------------------------------------------------
void Bluetooth_Send(char *fmt, ...)
{
    char buf[128];
    va_list args;
    
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    
    bluetooth_ch9141_send_buffer((uint8_t*)buf, strlen(buf));
}