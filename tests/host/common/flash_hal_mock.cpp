/* Emulate the flash read/write HAL */

#include <stdint.h>
#include <string.h>

extern "C"
{
    uint32_t s_phys_addr  = 0;
    uint32_t s_phys_size  = 0;
    uint32_t s_phys_page  = 0;
    uint32_t s_phys_block = 0;
    uint8_t* s_phys_data  = nullptr;
}

int32_t flash_hal_read(uint32_t addr, uint32_t size, uint8_t* dst)
{
    memcpy(dst, s_phys_data + addr, size);
    return 0;
}

int32_t flash_hal_write(uint32_t addr, uint32_t size, const uint8_t* src)
{
    memcpy(s_phys_data + addr, src, size);
    return 0;
}

int32_t flash_hal_erase(uint32_t addr, uint32_t size)
{
    if ((size & (FLASH_SECTOR_SIZE - 1)) != 0 || (addr & (FLASH_SECTOR_SIZE - 1)) != 0)
    {
        abort();
    }
    const uint32_t sector      = addr / FLASH_SECTOR_SIZE;
    const uint32_t sectorCount = size / FLASH_SECTOR_SIZE;
    for (uint32_t i = 0; i < sectorCount; ++i)
    {
        memset(s_phys_data + (sector + i) * FLASH_SECTOR_SIZE, 0xff, FLASH_SECTOR_SIZE);
    }
    return 0;
}
