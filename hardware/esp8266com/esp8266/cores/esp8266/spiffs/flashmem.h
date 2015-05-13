// Based on NodeMCU platform_flash
// https://github.com/nodemcu/nodemcu-firmware

#ifndef SYSTEM_FLASHMEM_H_
#define SYSTEM_FLASHMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "spiffs.h"
#include "spi_flash.h"

#define INTERNAL_FLASH_WRITE_UNIT_SIZE  4
#define INTERNAL_FLASH_READ_UNIT_SIZE	4

#define FLASH_TOTAL_SEC_COUNT 	(flashmem_get_size_sectors())

#define SYS_PARAM_SEC_COUNT 4
#define FLASH_WORK_SEC_COUNT (FLASH_TOTAL_SEC_COUNT - SYS_PARAM_SEC_COUNT)

#define INTERNAL_FLASH_SECTOR_SIZE      SPI_FLASH_SEC_SIZE
#define INTERNAL_FLASH_SIZE             ( (FLASH_WORK_SEC_COUNT) * INTERNAL_FLASH_SECTOR_SIZE )
#define INTERNAL_FLASH_START_ADDRESS    0x40200000

typedef struct
{
    uint8_t unknown0;
    uint8_t unknown1;
    enum
    {
        MODE_QIO = 0,
        MODE_QOUT = 1,
        MODE_DIO = 2,
        MODE_DOUT = 15,
    } mode : 8;
    enum
    {
        SPEED_40MHZ = 0,
        SPEED_26MHZ = 1,
        SPEED_20MHZ = 2,
        SPEED_80MHZ = 15,
    } speed : 4;
    enum
    {
        SIZE_4MBIT = 0,
        SIZE_2MBIT = 1,
        SIZE_8MBIT = 2,
        SIZE_16MBIT = 3,
        SIZE_32MBIT = 4,
    } size : 4;
} STORE_TYPEDEF_ATTR SPIFlashInfo;

extern uint32_t flashmem_write( const void *from, uint32_t toaddr, uint32_t size );
extern uint32_t flashmem_read( void *to, uint32_t fromaddr, uint32_t size );
extern bool flashmem_erase_sector( uint32_t sector_id );

extern SPIFlashInfo flashmem_get_info();
extern uint8_t flashmem_get_size_type();
extern uint32_t flashmem_get_size_bytes();
extern uint16_t flashmem_get_size_sectors();
uint32_t flashmem_find_sector( uint32_t address, uint32_t *pstart, uint32_t *pend );
uint32_t flashmem_get_sector_of_address( uint32_t addr );

extern uint32_t flashmem_write_internal( const void *from, uint32_t toaddr, uint32_t size );
extern uint32_t flashmem_read_internal( void *to, uint32_t fromaddr, uint32_t size );
extern uint32_t flashmem_get_first_free_block_address();

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_FLASHMEM_H_ */
