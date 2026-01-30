#include "flash.h"
#include "pid.h"
#include "navigation.h" // ★ 必须引入，为了访问 N 和 Nav_Record_Buffer

// === PID 参数存储区 (原配置) ===
#define FLASH_SECTION_INDEX       (127)
#define FLASH_PAGE_INDEX          (3)

// === 惯导数据存储区 (新配置) ===
// 使用 Sector 125 和 126 (共8KB空间)
#define NAG_FLASH_SECTOR_START    (125) 
#define NAG_MAGIC_WORD            (0xA5A5A5A5) // 用于标记"这里有有效数据"

extern PID_t AnglePID;
extern PID_t SpeedPID;
extern PID_t TurnPID;

// ----------------------------------------------------
// 原有的 PID 保存/读取函数 (保持不变)
// ----------------------------------------------------
void flash_save(void)
{
    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX)) 
    {
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX); 
    }
    flash_buffer_clear(); 

    // 保存AnglePID参数
    for (int i = 0; i < sizeof(PID_t)/sizeof(float); i++)
    {
        flash_union_buffer[i].float_type = ((float*)&AnglePID)[i];
    }
    
    // 保存SpeedPID参数
    for (int i = 0; i < sizeof(PID_t)/sizeof(float); i++)
    {
        flash_union_buffer[sizeof(PID_t)/sizeof(float) + i].float_type = ((float*)&SpeedPID)[i];
    }
    
    // 保存TurnPID参数
    for (int i = 0; i < sizeof(PID_t)/sizeof(float); i++)
    {
        flash_union_buffer[2*sizeof(PID_t)/sizeof(float) + i].float_type = ((float*)&TurnPID)[i];
    }

    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);
}

void flash_load(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);

    // 加载AnglePID参数
    for (int i = 0; i < sizeof(PID_t)/sizeof(float); i++)
    {
        ((float*)&AnglePID)[i] = flash_union_buffer[i].float_type;
    }
    
    // 加载SpeedPID参数
    for (int i = 0; i < sizeof(PID_t)/sizeof(float); i++)
    {
        ((float*)&SpeedPID)[i] = flash_union_buffer[sizeof(PID_t)/sizeof(float) + i].float_type;
    }
    
    // 加载TurnPID参数
    for (int i = 0; i < sizeof(PID_t)/sizeof(float); i++)
    {
        ((float*)&TurnPID)[i] = flash_union_buffer[2*sizeof(PID_t)/sizeof(float) + i].float_type;
    }

    flash_buffer_clear();
}

// ----------------------------------------------------
// ★★★ 新增：惯导大数组 跨页存取逻辑 ★★★
// ----------------------------------------------------

// 保存路径到 Flash (跨页写入)
void flash_save_nag(void)
{
    uint32_t sector = NAG_FLASH_SECTOR_START;
    uint32_t page = 0;
    
    // === 1. 准备第一页 (包含文件头) ===
    flash_buffer_clear();
    
    // 写入文件头
    flash_union_buffer[0].uint32_type = NAG_MAGIC_WORD; // 标记:有效数据
    flash_union_buffer[1].uint32_type = (uint32_t)N.Save_index; // 标记:有多少个点
    
    // 写入第一批数据
    int count = 0;
    int items_per_page = FLASH_DATA_BUFFER_SIZE; // 256个uint32
    // 第一页前面占用了2个位置，所以还能存 254 个数据
    int first_page_capacity = items_per_page - 2;
    
    for(int i=0; i < first_page_capacity && i < N.Save_index; i++)
    {
        flash_union_buffer[2+i].int32_type = Nav_Record_Buffer[i];
        count++;
    }
    
    // 写入第一页 (注意: flash_write_page_from_buffer 内部会自动擦除)
    flash_write_page_from_buffer(sector, page);
    page++; // 准备写下一页
    
    // === 2. 循环写入剩余数据 ===
    while(count < N.Save_index)
    {
        // 处理页码/扇区递增 (MM32F327 每个扇区4页)
        if(page > 3) {
            sector++;
            page = 0;
        }
        
        flash_buffer_clear();
        
        // 填满这一页缓冲
        for(int i=0; i < items_per_page && count < N.Save_index; i++)
        {
            flash_union_buffer[i].int32_type = Nav_Record_Buffer[count];
            count++;
        }
        
        // 写入 Flash
        flash_write_page_from_buffer(sector, page);
        page++;
    }
}

// 从 Flash 读取路径 (跨页读取)
uint8_t flash_load_nag(void)
{
    uint32_t sector = NAG_FLASH_SECTOR_START;
    uint32_t page = 0;
    
    // === 1. 读取第一页 (检查文件头) ===
    flash_read_page_to_buffer(sector, page);
    
    // 检查魔术字 (如果不是 A5A5A5A5，说明还没存过数据)
    if(flash_union_buffer[0].uint32_type != NAG_MAGIC_WORD) {
        return 0; // 失败
    }
    
    // 恢复点数
    N.Save_index = (uint16_t)flash_union_buffer[1].uint32_type;
    
    // 安全保护：防止读出来的长度撑爆 RAM
    if(N.Save_index > MaxSize) N.Save_index = MaxSize;
    
    // 读取第一批数据
    int count = 0;
    int items_per_page = FLASH_DATA_BUFFER_SIZE;
    int first_page_capacity = items_per_page - 2;
    
    for(int i=0; i < first_page_capacity && count < N.Save_index; i++)
    {
        Nav_Record_Buffer[count] = flash_union_buffer[2+i].int32_type;
        count++;
    }
    
    page++;
    
    // === 2. 循环读取剩余数据 ===
    while(count < N.Save_index)
    {
        if(page > 3) {
            sector++;
            page = 0;
        }
        
        flash_read_page_to_buffer(sector, page);
        
        for(int i=0; i < items_per_page && count < N.Save_index; i++)
        {
            Nav_Record_Buffer[count] = flash_union_buffer[i].int32_type;
            count++;
        }
        page++;
    }
    
    return 1; // 成功
}
