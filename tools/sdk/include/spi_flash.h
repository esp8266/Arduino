/* 
 * copyright (c) Espressif System 2010
 * 
 */

#ifndef SPI_FLASH_H
#define SPI_FLASH_H

typedef enum {
    SPI_FLASH_RESULT_OK,
    SPI_FLASH_RESULT_ERR,
    SPI_FLASH_RESULT_TIMEOUT
} SpiFlashOpResult;

typedef struct{
	uint32	deviceId;
	uint32	chip_size;    // chip size in byte
	uint32	block_size;
	uint32  sector_size;
	uint32  page_size;
	uint32  status_mask;
} SpiFlashChip;

#define SPI_FLASH_SEC_SIZE      4096

extern SpiFlashChip * flashchip; // in ram ROM-BIOS

uint32 spi_flash_get_id(void);
SpiFlashOpResult spi_flash_erase_sector(uint16 sec);
SpiFlashOpResult spi_flash_write(uint32 des_addr, uint32 *src_addr, uint32 size);
SpiFlashOpResult spi_flash_read(uint32 src_addr, uint32 *des_addr, uint32 size);

typedef SpiFlashOpResult (* user_spi_flash_read)(
		SpiFlashChip *spi,
		uint32 src_addr,
		uint32 *des_addr,
        uint32 size);

void spi_flash_set_read_func(user_spi_flash_read read);

#endif
