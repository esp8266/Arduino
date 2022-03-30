#ifndef flash_hal_mock_h
#define flash_hal_mock_h

#include <stdint.h>
#include <../../cores/esp8266/flash_hal.h>

#undef FS_start
#undef FS_end
#define FS_start 0
#define FS_end 0

extern "C"
{
    extern uint32_t s_phys_addr;
    extern uint32_t s_phys_size;
    extern uint32_t s_phys_page;
    extern uint32_t s_phys_block;
    extern uint8_t* s_phys_data;

    extern int32_t flash_hal_read(uint32_t addr, uint32_t size, uint8_t* dst);
    extern int32_t flash_hal_write(uint32_t addr, uint32_t size, const uint8_t* src);
    extern int32_t flash_hal_erase(uint32_t addr, uint32_t size);
}

#endif
