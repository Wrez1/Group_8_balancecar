#include "zf_common_headfile.h"

#include "bluetooth.h"

static char rx_buffer[128];
static uint32_t rx_len = 0;

static fifo_struct     bluetooth_fifo;
static uint8           bluetooth_buffer[BLUETOOTH_BUFFER_SIZE];
static uint8           bluetooth_data = 0;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 发送数据
// 参数说明     data            8bit 数据
// 返回参数     uint32          实际发送字节数
// 使用示例     bluetooth_send_byte(0x5A);
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_send_byte (const uint8 data)
{
    uart_write_byte(BLUETOOTH_INDEX, data);
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 发送函数
// 参数说明     buff             需要发送的数据地址
// 返回参数     uint32           实际发送字节数
// 使用示例     bluetooth_send_buffer(buff, 16);
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_send_buffer (const uint8 *buff, uint32 len)
{
    zf_assert(NULL != buff);
    uart_write_buffer(BLUETOOTH_INDEX, buff, len);
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 发送字符串
// 参数说明     *str            要发送的字符串地址
// 返回参数     uint32          实际发送字节数
// 使用示例     bluetooth_send_string("Hello HC-04!");
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_send_string (const char *str)
{
    zf_assert(NULL != str);
    uint32 len = strlen(str);
    uart_write_buffer(BLUETOOTH_INDEX, (const uint8 *)str, len);
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 读取函数
// 参数说明     buff            存储的数据地址
// 参数说明     len             长度
// 返回参数     uint32          实际读取字节数
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_read_buffer (uint8 *buff, uint32 len)
{
    zf_assert(NULL != buff);
    uint32 data_len = len;
    fifo_read_buffer(&bluetooth_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 串口中断回调函数
// 参数说明     void
// 返回参数     void
// 使用示例     
// 备注信息     该函数在 ISR 文件的串口中断程序被调用
//              由串口中断服务函数调用 wireless_module_uart_handler() 函数
//              再由 wireless_module_uart_handler() 函数调用本函数
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_uart_callback (void)
{
    uart_query_byte(BLUETOOTH_INDEX, &bluetooth_data);            // 读取串口数据
    fifo_write_buffer(&bluetooth_fifo, &bluetooth_data, 1);       // 存入 FIFO
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 初始化
// 参数说明     void
// 返回参数     uint8           初始化状态 0-成功 1-失败
// 使用示例     bluetooth_init();
//-------------------------------------------------------------------------------------------------------------------
uint8 bluetooth_init (void)
{
    uint8 return_state = 0;

    uart_init(BLUETOOTH_INDEX, 9600, BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);
    uart_rx_interrupt(BLUETOOTH_INDEX, 1);
    
    return return_state;
}

void parse_bluetooth_data(void) {
    char temp_buffer[64];
    uint32_t temp_len;
    
    // 从FIFO读取数据到临时缓冲区
    while ((temp_len = bluetooth_read_buffer((uint8*)temp_buffer, sizeof(temp_buffer))) > 0) {
        // 将新数据追加到全局缓冲区
        if (rx_len + temp_len > sizeof(rx_buffer)) {
            rx_len = 0;  // 缓冲区溢出处理
            continue;
        }
        memcpy(rx_buffer + rx_len, temp_buffer, temp_len);
        rx_len += temp_len;
        
        // 解析数据包
        while (rx_len > 0) {
            // 查找起始符[
            char *start = strchr(rx_buffer, '[');  // 直接使用char*类型
            if (!start) {
                rx_len = 0;  // 未找到起始符，清空缓冲区
                break;
            }
            start++;  // 移动到数据起始位置
            
            // 查找结束符]
            char *end = strchr(start, ']');
            if (!end) {
                break;  // 未找到结束符，等待更多数据
            }
            
            // 提取有效数据
            int data_len = end - start;
            char data_str[data_len + 1];
            memcpy(data_str, start, data_len);
            data_str[data_len] = '\0';
            
            // 分割数值
            char *token = strtok(data_str, ",");
            int values[4] = {0};
            int index = 0;
            while (token && index < 4) {
                values[index++] = atoi(token);
                token = strtok(NULL, ",");
            };
                     
            // 移除已处理数据
            memmove(rx_buffer, end + 1, rx_len - (end - rx_buffer) - 1);
            rx_len -= (end - start + 1);
        }
    }
}
