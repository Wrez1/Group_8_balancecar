#include "zf_common_headfile.h"

#include "bluetooth.h"

static  fifo_struct     bluetooth_hc04_fifo;
static  uint8           bluetooth_hc04_buffer[BLUETOOTH_HC04_BUFFER_SIZE];  // 数据存放数组
static  uint8           bluetooth_hc04_data = 0;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 发送数据
// 参数说明     data            8bit 数据
// 返回参数     uint32          实际发送字节数
// 使用示例     bluetooth_hc04_send_byte(0x5A);
// 备注信息     HC04无流控，直接发送
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_hc04_send_byte (const uint8 data)
{
    uart_write_byte(BLUETOOTH_HC04_INDEX, data);  // HC04无需流控检测，直接发送
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 发送函数
// 参数说明     buff             需要发送的数据地址
// 返回参数     uint32           实际发送字节数
// 使用示例     bluetooth_hc04_send_buffer(buff, 16);
// 备注信息     HC04无流控，直接发送
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_hc04_send_buffer (const uint8 *buff, uint32 len)
{
    zf_assert(NULL != buff);
    uart_write_buffer(BLUETOOTH_HC04_INDEX, buff, len);  // HC04无需分包处理
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 发送字符串
// 参数说明     *str            要发送的字符串地址
// 返回参数     uint32          实际发送字节数
// 使用示例     bluetooth_hc04_send_string("Hello HC-04!");
// 备注信息     HC04无流控，直接发送
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_hc04_send_string (const char *str)
{
    zf_assert(NULL != str);
    uint32 len = strlen(str);
    uart_write_buffer(BLUETOOTH_HC04_INDEX, (const uint8 *)str, len);
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 读取函数
// 参数说明     buff            存储的数据地址
// 参数说明     len             长度
// 返回参数     uint32          实际读取字节数
// 使用示例     bluetooth_hc04_read_buffer(buff, 16);
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_hc04_read_buffer (uint8 *buff, uint32 len)
{
    zf_assert(NULL != buff);
    uint32 data_len = len;
    fifo_read_buffer(&bluetooth_hc04_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
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
void bluetooth_hc04_uart_callback (void)
{
    uart_query_byte(BLUETOOTH_HC04_INDEX, &bluetooth_hc04_data);            // 读取串口数据
    fifo_write_buffer(&bluetooth_hc04_fifo, &bluetooth_hc04_data, 1);       // 存入 FIFO
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蓝牙转串口模块 初始化
// 参数说明     void
// 返回参数     uint8           初始化状态 0-成功 1-失败
// 使用示例     bluetooth_hc04_init();
// 备注信息     HC04默认波特率9600，需提前用AT指令配置
//-------------------------------------------------------------------------------------------------------------------
uint8 bluetooth_hc04_init (void)
{
    uint8 return_state = 0;
    
    // HC04无需流控引脚配置
    gpio_init(BLUETOOTH_HC04_TX_PIN, GPO, 1, GPI_PULL_UP);  // TX引脚初始化为推挽输出
    gpio_init(BLUETOOTH_HC04_RX_PIN, GPI, 0, GPI_PULL_UP);  // RX引脚初始化为输入上拉
    
    // 修改波特率为HC04默认的9600
    uart_init(BLUETOOTH_HC04_INDEX, 9600, BLUETOOTH_HC04_RX_PIN, BLUETOOTH_HC04_TX_PIN);
    uart_rx_interrupt(BLUETOOTH_HC04_INDEX, 1);
    
    return return_state;
}
