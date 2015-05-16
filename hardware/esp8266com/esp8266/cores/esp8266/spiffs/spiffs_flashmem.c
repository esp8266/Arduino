#include "flashmem.h"
#include "esp8266_peri.h"
#include "Arduino.h"

// Based on NodeMCU platform_flash
// https://github.com/nodemcu/nodemcu-firmware

extern uint32_t _SPIFFS_start;

uint32_t flashmem_write( const void *from, uint32_t toaddr, uint32_t size )
{
  uint32_t temp, rest, ssize = size;
  unsigned i;
  char tmpdata[ INTERNAL_FLASH_WRITE_UNIT_SIZE ];
  const uint8_t *pfrom = ( const uint8_t* )from;
  const uint32_t blksize = INTERNAL_FLASH_WRITE_UNIT_SIZE;
  const uint32_t blkmask = INTERNAL_FLASH_WRITE_UNIT_SIZE - 1;

  // Align the start
  if( toaddr & blkmask )
  {
    rest = toaddr & blkmask;
    temp = toaddr & ~blkmask; // this is the actual aligned address
    // c_memcpy( tmpdata, ( const void* )temp, blksize );
    flashmem_read_internal( tmpdata, temp, blksize );
    for( i = rest; size && ( i < blksize ); i ++, size --, pfrom ++ )
      tmpdata[ i ] = *pfrom;
    flashmem_write_internal( tmpdata, temp, blksize );
    if( size == 0 )
      return ssize;
    toaddr = temp + blksize;
  }
  // The start address is now a multiple of blksize
  // Compute how many bytes we can write as multiples of blksize
  rest = size & blkmask;
  temp = size & ~blkmask;
  // Program the blocks now
  if( temp )
  {
	flashmem_write_internal( pfrom, toaddr, temp );
    toaddr += temp;
    pfrom += temp;
  }
  // And the final part of a block if needed
  if( rest )
  {
    // c_memcpy( tmpdata, ( const void* )toaddr, blksize );
	flashmem_read_internal( tmpdata, toaddr, blksize );
    for( i = 0; size && ( i < rest ); i ++, size --, pfrom ++ )
      tmpdata[ i ] = *pfrom;
    flashmem_write_internal( tmpdata, toaddr, blksize );
  }
  return ssize;
}

uint32_t flashmem_read( void *to, uint32_t fromaddr, uint32_t size )
{
  uint32_t temp, rest, ssize = size;
  unsigned i;
  char tmpdata[ INTERNAL_FLASH_READ_UNIT_SIZE ];
  uint8_t *pto = ( uint8_t* )to;
  const uint32_t blksize = INTERNAL_FLASH_READ_UNIT_SIZE;
  const uint32_t blkmask = INTERNAL_FLASH_READ_UNIT_SIZE - 1;

  // Align the start
  if( fromaddr & blkmask )
  {
    rest = fromaddr & blkmask;
    temp = fromaddr & ~blkmask; // this is the actual aligned address
    flashmem_read_internal( tmpdata, temp, blksize );
    for( i = rest; size && ( i < blksize ); i ++, size --, pto ++ )
      *pto = tmpdata[ i ];

    if( size == 0 )
      return ssize;
    fromaddr = temp + blksize;
  }
  // The start address is now a multiple of blksize
  // Compute how many bytes we can read as multiples of blksize
  rest = size & blkmask;
  temp = size & ~blkmask;
  // Program the blocks now
  if( temp )
  {
	flashmem_read_internal( pto, fromaddr, temp );
    fromaddr += temp;
    pto += temp;
  }
  // And the final part of a block if needed
  if( rest )
  {
	flashmem_read_internal( tmpdata, fromaddr, blksize );
    for( i = 0; size && ( i < rest ); i ++, size --, pto ++ )
      *pto = tmpdata[ i ];
  }
  return ssize;
}

SPIFlashInfo flashmem_get_info()
{
    volatile SPIFlashInfo spi_flash_info STORE_ATTR;
    spi_flash_info = *((SPIFlashInfo *)(INTERNAL_FLASH_START_ADDRESS));
    return spi_flash_info;
}

uint8_t flashmem_get_size_type()
{
    return flashmem_get_info().size;
}

uint32_t flashmem_get_size_bytes()
{
    uint32_t flash_size = 0;
    switch (flashmem_get_info().size)
    {
    case SIZE_2MBIT:
        // 2Mbit, 256kByte
        flash_size = 256 * 1024;
        break;
    case SIZE_4MBIT:
        // 4Mbit, 512kByte
        flash_size = 512 * 1024;
        break;
    case SIZE_8MBIT:
        // 8Mbit, 1MByte
        flash_size = 1 * 1024 * 1024;
        break;
    case SIZE_16MBIT:
        // 16Mbit, 2MByte
        flash_size = 2 * 1024 * 1024;
        break;
    case SIZE_32MBIT:
        // 32Mbit, 4MByte
        flash_size = 4 * 1024 * 1024;
        break;
    default:
        // Unknown flash size, fall back mode.
        flash_size = 512 * 1024;
        break;
    }
    return flash_size;
}

uint16_t flashmem_get_size_sectors()
{
    return flashmem_get_size_bytes() / SPI_FLASH_SEC_SIZE;
}

// Helper function: find the flash sector in which an address resides
// Return the sector number, as well as the start and end address of the sector
uint32_t flashmem_find_sector( uint32_t address, uint32_t *pstart, uint32_t *pend )
{
  address -= INTERNAL_FLASH_START_ADDRESS;
  // All the sectors in the flash have the same size, so just align the address
  uint32_t sect_id = address / INTERNAL_FLASH_SECTOR_SIZE;

  if( pstart )
    *pstart = sect_id * INTERNAL_FLASH_SECTOR_SIZE + INTERNAL_FLASH_START_ADDRESS;
  if( pend )
    *pend = ( sect_id + 1 ) * INTERNAL_FLASH_SECTOR_SIZE + INTERNAL_FLASH_START_ADDRESS - 1;
  return sect_id;
}

uint32_t flashmem_get_sector_of_address( uint32_t addr )
{
  return flashmem_find_sector( addr, NULL, NULL );
}

/////////////////////////////////////////////////////

bool flashmem_erase_sector( uint32_t sector_id )
{
  WDT_RESET();
  noInterrupts();
  bool erased = spi_flash_erase_sector( sector_id ) == SPI_FLASH_RESULT_OK;
  interrupts();
  return erased;
}

uint32_t flashmem_write_internal( const void *from, uint32_t toaddr, uint32_t size )
{
  toaddr -= INTERNAL_FLASH_START_ADDRESS;
  SpiFlashOpResult r;
  const uint32_t blkmask = INTERNAL_FLASH_WRITE_UNIT_SIZE - 1;
  uint32_t *apbuf = NULL;
  if( ((uint32_t)from) & blkmask ){
    apbuf = (uint32_t *)os_malloc(size);
    if(!apbuf)
      return 0;
    os_memcpy(apbuf, from, size);
  }
  WDT_RESET();
  noInterrupts();
  r = spi_flash_write(toaddr, apbuf?(uint32 *)apbuf:(uint32 *)from, size);
  interrupts();
  if(apbuf)
    os_free(apbuf);
  if(SPI_FLASH_RESULT_OK == r)
    return size;
  else{
	SYSTEM_ERROR( "ERROR in flash_write: r=%d at %08X\n", ( int )r, ( unsigned )toaddr+INTERNAL_FLASH_START_ADDRESS );
    return 0;
  }
}

uint32_t flashmem_read_internal( void *to, uint32_t fromaddr, uint32_t size )
{
  fromaddr -= INTERNAL_FLASH_START_ADDRESS;
  SpiFlashOpResult r;
  WDT_RESET();
  noInterrupts();
  r = spi_flash_read(fromaddr, (uint32 *)to, size);
  interrupts();
  if(SPI_FLASH_RESULT_OK == r)
    return size;
  else{
	  SYSTEM_ERROR( "ERROR in flash_read: r=%d at %08X\n", ( int )r, ( unsigned )fromaddr+INTERNAL_FLASH_START_ADDRESS );
    return 0;
  }
}

uint32_t flashmem_get_first_free_block_address(){
  if ((uint32_t)&_SPIFFS_start == 0){
	  return 0;
  }
	debugf("_SPIFFS_start:%08x\n", (uint32_t)&_SPIFFS_start);

  // Round the total used flash size to the closest flash block address
  uint32_t end;
  flashmem_find_sector( (uint32_t)&_SPIFFS_start - 1, NULL, &end);
  return end + 1;
}
