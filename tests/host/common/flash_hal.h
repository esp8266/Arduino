#ifndef flash_hal_mock_h
#define flash_hal_mock_h

#include <stdint.h>

#define FLASH_HAL_OK          (0)
#define FLASH_HAL_READ_ERROR  (-1)
#define FLASH_HAL_WRITE_ERROR (-2)
#define FLASH_HAL_ERASE_ERROR (-3)

#define FS_start 0
#define FS_end 0

extern "C"
{

extern uint32_t s_phys_addr;
extern uint32_t s_phys_size;
extern uint32_t s_phys_page;
extern uint32_t s_phys_block;
extern uint8_t* s_phys_data;

extern int32_t flash_hal_read(uint32_t addr, uint32_t size, uint8_t *dst);
extern int32_t flash_hal_write(uint32_t addr, uint32_t size, const uint8_t *src);
extern int32_t flash_hal_erase(uint32_t addr, uint32_t size);

}

#endif
