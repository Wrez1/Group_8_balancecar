#include "flash.h"
#include "pid.h"

#define FLASH_SECTION_INDEX       (127)
#define FLASH_PAGE_INDEX          (3)


extern PID_t AnglePID;
extern PID_t SpeedPID;
extern PID_t TurnPID;

void menu_save(void)
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

void menu_load(void)
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
