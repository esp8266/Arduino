#ifndef SYSTEM_FLASHMEM_H_
#define SYSTEM_FLASHMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "spiffs.h"

/*******************
The W25Q32BV array is organized into 16,384 programmable pages of 256-bytes each. Up to 256 bytes can be programmed at a time.
Pages can be erased in groups of 16 (4KB sector erase), groups of 128 (32KB block erase), groups of 256 (64KB block erase) or
the entire chip (chip erase). The W25Q32BV has 1,024 erasable sectors and 64 erasable blocks respectively.
The small 4KB sectors allow for greater flexibility in applications that require data and parameter storage.
********************/

#define SPIFFS_API_DBG_V(fmt, ...) //os_printf(fmt, ##__VA_ARGS__)
#define SPIFFS_API_DBG_E(fmt, ...) //os_printf("ERROR: " fmt , ##__VA_ARGS__)

#define INTERNAL_FLASH_PAGE_SIZE        256
#define INTERNAL_FLASH_SECTOR_SIZE      4096
#define INTERNAL_FLASH_START_ADDRESS    0x40200000

#define INTERNAL_FLASH_WRITE_UNIT_SIZE  4
#define INTERNAL_FLASH_READ_UNIT_SIZE   4

extern uint32_t flashmem_write( const void *from, uint32_t toaddr, uint32_t size );
extern uint32_t flashmem_read( void *to, uint32_t fromaddr, uint32_t size );
extern bool flashmem_erase_sector( uint32_t sector_id );
uint32_t flashmem_find_sector( uint32_t address, uint32_t *pstart, uint32_t *pend );
uint32_t flashmem_get_sector_of_address( uint32_t addr );

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_FLASHMEM_H_ */
