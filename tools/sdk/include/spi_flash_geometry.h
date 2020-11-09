#ifndef SPI_FLASH_GEOMETRY_H
#define SPI_FLASH_GEOMETRY_H

/* The flash geometry is meant to be unified here. This header file should be included wherever needed.
 * Beware: this file is needed by eboot as well as the Arduino core.
 */

#define FLASH_SECTOR_SIZE 0x1000
#define FLASH_BLOCK_SIZE 0x10000
#define FLASH_PAGE_SIZE 0x100
#define APP_START_OFFSET 0x1000

//pulled this define from spi_flash.h for reuse in the Arduino core without pulling in a bunch of other stuff
#define SPI_FLASH_SEC_SIZE      FLASH_SECTOR_SIZE

#endif
