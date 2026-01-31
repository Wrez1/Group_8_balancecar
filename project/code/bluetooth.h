#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include "zf_common_headfile.h"
#include "zf_driver_uart.h"

// 定义蓝牙使用的串口号和引脚
#define BLUETOOTH_UART UART_6
#define BLUETOOTH_TX_PIN UART6_TX_C6
#define BLUETOOTH_RX_PIN UART6_RX_C7
#define BLUETOOTH_UART_IRQn UART6_IRQn
// 接收缓冲区和状态标志
extern char Bluetooth_RxPacket[100];
extern uint8_t Bluetooth_RxFlag;

void Bluetooth_Init(void);
void Bluetooth_SendByte(uint8_t Byte);
void Bluetooth_SendArray(uint8_t *Array, uint16_t Length);
void Bluetooth_SendString(char *String);
void Bluetooth_SendNumber(uint32_t Number, uint8_t Length);
void Bluetooth_ProcessData(float *Speed,float *Turn);

#endif
