#include "zf_common_headfile.h"
#include <string.h>
#include <stdlib.h>
#include "bluetooth.h"

char Bluetooth_RxPacket[100];
uint8_t Bluetooth_RxFlag;

// 蓝牙串口串口初始化
void Bluetooth_Init(void)
{
	uart_init(UART_6, 9600, UART6_TX_C6, UART6_RX_C7);
	uart_rx_interrupt (UART_6, 1);
	Bluetooth_RxFlag = 0;  // 初始化标志位
}

// 函数简介     串口接收中断函数
void UART6_IRQHandler (void)
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


void Bluetooth_Control(float *SpeedTarget,float *TurnTarget)
{
	
	if (Bluetooth_RxFlag == 1)
	{           	
		char *Tag = strtok(Bluetooth_RxPacket, ",");
	   
		if (strcmp(Tag, "joystick") == 0)
		{
		    int8_t LH = atoi(strtok(NULL, ","));
		    int8_t LV = atoi(strtok(NULL, ","));
		    int8_t RH = atoi(strtok(NULL, ","));
		    int8_t RV = atoi(strtok(NULL, ","));
			
			*SpeedTarget=LV/8;
		    *TurnTarget=-RH*1.2;
		}
		
		Bluetooth_RxFlag = 0;//记得标志位归零
		
		
    }	
}
