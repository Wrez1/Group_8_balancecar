#include "flash.h"
#include "pid.h"
#include "navigation.h" // ★ 必须引入，为了访问 N 和 Nav_Record_Buffer
#include "buzzer.h"
// === PID 参数存储区 (原配置) ===
#define FLASH_SECTION_INDEX       (127)
#define FLASH_PAGE_INDEX          (3)

// === 机械中值存储区 ===
#define MECH_ZERO_SECTION_INDEX   (127)
#define MECH_ZERO_PAGE_INDEX      (2)    // 使用第2页存储机械中值

// === 惯导数据存储区 (新配置) ===
// 使用 Sector 125 和 126 (共8KB空间)
#define NAG_FLASH_SECTOR_START    (125) 
#define NAG_MAGIC_WORD            (0xA5A5A5A5) // 用于标记"这里有有效数据"

// 定义一个 PID 专用的魔术字 (随便写一个由十六进制组成的词)
#define PID_MAGIC_WORD  0xDEADBEEF
extern PID_t AnglePID;
extern PID_t SpeedPID;
extern PID_t TurnPID;
extern float Mechanical_Zero_Pitch;

// 带校验的 PID 保存函数
void flash_save(void)
{
    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX)) 
    {
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX); 
    }
    flash_buffer_clear(); 

    // === 1. 写入魔术字 (占用第0个位置) ===
    flash_union_buffer[0].uint32_type = PID_MAGIC_WORD;

    // 计算 PID 结构体占用的 float 数量
    int pid_size = sizeof(PID_t) / sizeof(float);
    int offset = 1; // 从第1个位置开始存数据 (第0个是魔术字)

    // === 2. 保存 AnglePID ===
    for (int i = 0; i < pid_size; i++)
    {
        flash_union_buffer[offset + i].float_type = ((float*)&AnglePID)[i];
    }
    offset += pid_size;
    
    // === 3. 保存 SpeedPID ===
    for (int i = 0; i < pid_size; i++)
    {
        flash_union_buffer[offset + i].float_type = ((float*)&SpeedPID)[i];
    }
    offset += pid_size;
    
    // === 4. 保存 TurnPID ===
    for (int i = 0; i < pid_size; i++)
    {
        flash_union_buffer[offset + i].float_type = ((float*)&TurnPID)[i];
    }

    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);
}

// 带校验的读取函数 (返回值: 1=成功, 0=失败/为空)
uint8_t flash_load(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);

    // === 1. 检查魔术字 ===
    // 如果第0个位置不是我们存的 0xDEADBEEF，说明 Flash 是空的或者乱的
    if (flash_union_buffer[0].uint32_type != PID_MAGIC_WORD)
    {
        return 0; // 返回失败，告诉 main 函数不要用这些垃圾数据
    }

    int pid_size = sizeof(PID_t) / sizeof(float);
    int offset = 1; // 从第1个位置开始读

    // === 2. 加载 AnglePID ===
    for (int i = 0; i < pid_size; i++)
    {
        ((float*)&AnglePID)[i] = flash_union_buffer[offset + i].float_type;
    }
    offset += pid_size;
    
    // === 3. 加载 SpeedPID ===
    for (int i = 0; i < pid_size; i++)
    {
        ((float*)&SpeedPID)[i] = flash_union_buffer[offset + i].float_type;
    }
    offset += pid_size;
    
    // === 4. 加载 TurnPID ===
    for (int i = 0; i < pid_size; i++)
    {
        ((float*)&TurnPID)[i] = flash_union_buffer[offset + i].float_type;
    }

    flash_buffer_clear();
    return 1; // 成功
}

// 保存机械中值
void flash_save_mech_zero(void)
{
    if(flash_check(MECH_ZERO_SECTION_INDEX, MECH_ZERO_PAGE_INDEX)) 
    {
        flash_erase_page(MECH_ZERO_SECTION_INDEX, MECH_ZERO_PAGE_INDEX);
    }
    
    flash_buffer_clear();
    
    // 计算需要存储的float数量
    // 我们存储一个float，但为了对齐和未来扩展，我们还是按float数组处理
    int float_count = 1;  // 只存储机械中值一个float
    
    // 存储机械中值
    flash_union_buffer[0].float_type = Mechanical_Zero_Pitch;
    
    // 添加校验标记（仿照PID存储方式，虽然PID没校验，但我们可以加一个）
    // 使用0x5A5A5A5A作为校验标记
    flash_union_buffer[1].uint32_type = 0x5A5A5A5A;
    
    // 可以存储一个版本号，方便以后扩展
    flash_union_buffer[2].uint32_type = 0x00000001;  // 版本1
    
    flash_write_page_from_buffer(MECH_ZERO_SECTION_INDEX, MECH_ZERO_PAGE_INDEX);
}

// 加载机械中值
void flash_load_mech_zero(void)
{
    // 默认值
    float default_value = 0.0f;
    
    // 读取Flash页面
    flash_read_page_to_buffer(MECH_ZERO_SECTION_INDEX, MECH_ZERO_PAGE_INDEX);
    
    // 检查校验标记
    uint32_t check_mark = flash_union_buffer[1].uint32_type;
    uint32_t version = flash_union_buffer[2].uint32_type;
    
    // 如果校验标记正确，版本为1，则加载数据
    if(check_mark == 0x5A5A5A5A && version == 0x00000001)
    {
        // 加载机械中值
        Mechanical_Zero_Pitch = flash_union_buffer[0].float_type;
        // 打印调试信息（可选）
        // printf("Loaded mech zero from flash: %f\n", Mechanical_Zero_Pitch);
    }
    else
    {
        // 校验失败，使用默认值
        Mechanical_Zero_Pitch = default_value;
        // 可选：保存默认值到Flash
        // flash_save_mech_zero();
    }
    
    flash_buffer_clear();
}

// ----------------------------------------------------
// ★★★ 新增：惯导大数组 跨页存取逻辑 ★★★
// ----------------------------------------------------

// 保存路径到 Flash (跨页写入)
void flash_save_nag(void)
{			
	for(int s = 0; s < 2; s++) {
        for(int p = 0; p < 4; p++) {
            flash_erase_page(NAG_FLASH_SECTOR_START + s, p);
        }
    }
	
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
	
	// ==========================================================
    // 阶段三：立即自检 (Verify)
    // 存完马上读回来检查，看看是不是真的存进去了
    // ==========================================================
    flash_read_page_to_buffer(NAG_FLASH_SECTOR_START, 0);
    
    // 检查魔术字
    if(flash_union_buffer[0].uint32_type == NAG_MAGIC_WORD) {
        // 校验成功！响两声短音
        buzzer_on(1); system_delay_ms(100); buzzer_on(0);
        system_delay_ms(100);
        buzzer_on(1); system_delay_ms(100); buzzer_on(0);
    } else {
        // 校验失败！(说明Flash坏了或者写保护)，响三声长音报警
        for(int k=0; k<3; k++){
            buzzer_on(1); system_delay_ms(500); buzzer_on(0); system_delay_ms(200);
        }
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
