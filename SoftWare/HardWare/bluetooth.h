#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "zf_common_typedef.h"

#define BLUETOOTH_INDEX                  ( UART_6 )      // 使用的串口号（需与硬件连接一致）
#define BLUETOOTH_BUAD_RATE              ( 9600 )        // HC04默认波特率
#define BLUETOOTH_TX_PIN                 ( UART6_RX_C7 ) // HC04 TX接MCU RX（推挽输出模式）
#define BLUETOOTH_RX_PIN                 ( UART6_TX_C6 ) // HC04 RX接MCU TX（输入上拉）
#define BLUETOOTH_BUFFER_SIZE            ( 64 )          // 接收缓冲区大小
#define BLUETOOTH_TIMEOUT_COUNT          ( 500 )         // 超时计数阈值

// 函数声明
uint32 bluetooth_send_byte          (const uint8 data);
uint32 bluetooth_send_buffer        (const uint8 *buff, uint32 len);
uint32 bluetooth_send_string        (const char *str);
void   bluetooth_send_image         (const uint8 *image_addr, uint32 image_size);
uint32 bluetooth_read_buffer        (uint8 *buff, uint32 len);
void   bluetooth_uart_callback      (void);
uint8  bluetooth_init               (void);

#endif //
