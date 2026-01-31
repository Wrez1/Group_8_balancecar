#include "bluetooth.h"
#include <string.h>
#include <stdlib.h>
#include "zf_common_headfile.h"
#include "pid.h"

char Bluetooth_RxPacket[100] = {0};
uint8_t Bluetooth_RxFlag = 0;
// 定义一个超时计数器
uint32_t BT_Timeout_Count = 0;

// 初始化蓝牙通信
void Bluetooth_Init(void)
{
    // 初始化UART1
    uart_init(BLUETOOTH_UART, 9600, BLUETOOTH_TX_PIN, BLUETOOTH_RX_PIN);
    
    // 启用接收中断
    uart_rx_interrupt(BLUETOOTH_UART, 1);

}

// 发送单个字节
void Bluetooth_SendByte(uint8_t Byte)
{
    uart_write_byte(BLUETOOTH_UART, Byte);
}

// 发送数组
void Bluetooth_SendArray(uint8_t *Array, uint16_t Length)
{
    uart_write_buffer(BLUETOOTH_UART, Array, Length);
}

// 发送字符串
void Bluetooth_SendString(char *String)
{
    uart_write_string(BLUETOOTH_UART, String);
}

// 发送数字
void Bluetooth_SendNumber(uint32_t Number, uint8_t Length)
{
    char temp[20];
    sprintf(temp, "%0*d", Length, Number);
    Bluetooth_SendString(temp);
}

// 中断服务函数
void BLUETOOTH_UART_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	
    if(UART6->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        wireless_module_uart_handler();
		uint8_t RxData = (uint8_t)UART6->RDR;	
		if (RxState == 0)
		{
			if (RxData == '[' && Bluetooth_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (RxData == ']')
			{
				RxState = 0;
				Bluetooth_RxPacket[pRxPacket] = '\0';
				Bluetooth_RxFlag = 1;
			}
			else
			{
				Bluetooth_RxPacket[pRxPacket] = RxData;
				pRxPacket ++;
			}
		}
        UART6->ICR |= 0x00000002;                                               // 清除中断标志位
    }
	
}

// 数据处理函数
void Bluetooth_ProcessData(float *Speed,float *Turn)
{
    if (Bluetooth_RxFlag)
    {
        
        char *Tag = strtok(Bluetooth_RxPacket, ",");
//        if (strcmp(Tag, "key") == 0)
//        {
//            char *Name = strtok(NULL, ",");
//            char *Action = strtok(NULL, ",");
//            
//            if (strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
//            {
//                Bluetooth_SendString("key,1,up\r\n");
//            }
//            else if (strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0)
//            {
//                Bluetooth_SendString("key,2,down\r\n");
//            }
//        }
//        else if (strcmp(Tag, "slider") == 0)
//        {
//            char *Name = strtok(NULL, ",");
//            char *Value = strtok(NULL, ",");
//            
//            if (strcmp(Name, "1") == 0)
//            {
//                uint8_t IntValue = atoi(Value);
//                Bluetooth_SendString("slider,1,");
//                Bluetooth_SendNumber(IntValue, 3);
//                Bluetooth_SendString("\r\n");
//            }
//        }
//只需要蓝牙摇杆控值小车
        if (strcmp(Tag, "joystick") == 0)
        {
            // 摇杆协议通常是：joystick, LH, LV, RH, RV
            // LV: 左摇杆纵向 (速度)
            // RH: 右摇杆横向 (转向)
            // 假设 APP 发送范围是 -100 到 +100
            
            // 暂时没用的变量可以不读，或者读了不用
            strtok(NULL, ","); // 跳过 LH
            int8_t LV = atoi(strtok(NULL, ",")); // 读取 LV (速度)
            int8_t RH = atoi(strtok(NULL, ",")); // 读取 RH (转向)
            
            // ★★★ 1. 设置速度目标 (SpeedPID.Target) ★★★
            // 假设 LV 是 -100~100，你需要根据你的电机速度范围来缩放
            // 比如你的目标速度是 -50 到 50 (编码器脉冲/5ms)
            SpeedPID.Target = (float)LV * 0.5f; 
            
            // ★★★ 2. 设置转向目标 (Turn_Target) ★★★
            // RH 是 -100~100
            // 我们的目标是角速度 (度/秒)
            // 假设最大转速想要 200度/秒，那就乘以 2.0
            // 注意方向：如果摇杆往左是负，GyroZ 往左也是负，那就直接赋值
            Turn_Target = (float)RH * 2.0f; 
            
            // (可选) 加上死区：如果摇杆没动(0)，强制锁死
            if (RH > -5 && RH < 5) Turn_Target = 0.0f;
        }
        
        Bluetooth_RxFlag = 0;
    }
}