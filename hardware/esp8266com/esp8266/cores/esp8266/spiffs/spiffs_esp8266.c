#include "spiffs_esp8266.h"
#include "spi_flash.h"
#include "esp8266_peri.h"
#include "Arduino.h"

/*
  FLASH ACCESS FUNCTIONS
*/

//lowest level sector erase method
bool flashmem_erase_sector( uint32_t sector_id ){
  WDT_RESET();
  noInterrupts();
  uint32_t chip_size = flashchip->chip_size;
  flashchip->chip_size = 0x01000000;
  bool erased = spi_flash_erase_sector( sector_id ) == SPI_FLASH_RESULT_OK;
  flashchip->chip_size = chip_size;
  interrupts();
  return erased;
}

//lowest level data write method
uint32_t flashmem_write_internal( const void *from, uint32_t toaddr, uint32_t size ){
  toaddr -= INTERNAL_FLASH_START_ADDRESS;
  SpiFlashOpResult r;
  const uint32_t blkmask = INTERNAL_FLASH_WRITE_UNIT_SIZE - 1;
  uint32_t *apbuf = NULL;
  if(((uint32_t)from) & blkmask){
    apbuf = (uint32_t *)os_malloc(size);
    if(!apbuf)
      return 0;
    os_memcpy(apbuf, from, size);
  }
  WDT_RESET();
  noInterrupts();
  uint32_t chip_size = flashchip->chip_size;
  flashchip->chip_size = 0x01000000;
  r = spi_flash_write(toaddr, apbuf?(uint32 *)apbuf:(uint32 *)from, size);
  flashchip->chip_size = chip_size;
  interrupts();
  if(apbuf)
    os_free(apbuf);
  if(SPI_FLASH_RESULT_OK == r)
    return size;
  else{
    SPIFFS_API_DBG_E( "ERROR in flash_write: r=%d at %08X\n", ( int )r, ( unsigned )toaddr+INTERNAL_FLASH_START_ADDRESS );
    return 0;
  }
}

//lowest level data read method
uint32_t flashmem_read_internal( void *to, uint32_t fromaddr, uint32_t size ){
  fromaddr -= INTERNAL_FLASH_START_ADDRESS;
  SpiFlashOpResult r;
  WDT_RESET();
  noInterrupts();
  uint32_t chip_size = flashchip->chip_size;
  flashchip->chip_size = 0x01000000;
  r = spi_flash_read(fromaddr, (uint32 *)to, size);
  flashchip->chip_size = chip_size;
  interrupts();
  if(SPI_FLASH_RESULT_OK == r)
    return size;
  else{
	  SPIFFS_API_DBG_E( "ERROR in flash_read: r=%d at %08X\n", ( int )r, ( unsigned )fromaddr+INTERNAL_FLASH_START_ADDRESS );
    return 0;
  }
}

//mid level data write method
uint32_t flashmem_write( const void *from, uint32_t toaddr, uint32_t size ){
  uint32_t temp, rest, ssize = size;
  unsigned i;
  char tmpdata[ INTERNAL_FLASH_WRITE_UNIT_SIZE ];
  const uint8_t *pfrom = ( const uint8_t* )from;
  const uint32_t blksize = INTERNAL_FLASH_WRITE_UNIT_SIZE;
  const uint32_t blkmask = INTERNAL_FLASH_WRITE_UNIT_SIZE - 1;

  // Align the start
  if(toaddr & blkmask){
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
  if(temp){
    flashmem_write_internal( pfrom, toaddr, temp );
    toaddr += temp;
    pfrom += temp;
  }
  // And the final part of a block if needed
  if(rest){
    // c_memcpy( tmpdata, ( const void* )toaddr, blksize );
    flashmem_read_internal( tmpdata, toaddr, blksize );
    for( i = 0; size && ( i < rest ); i ++, size --, pfrom ++ )
      tmpdata[ i ] = *pfrom;
    flashmem_write_internal( tmpdata, toaddr, blksize );
  }
  return ssize;
}

//mid level data write method
uint32_t flashmem_read( void *to, uint32_t fromaddr, uint32_t size ){
  uint32_t temp, rest, ssize = size;
  unsigned i;
  char tmpdata[ INTERNAL_FLASH_READ_UNIT_SIZE ];
  uint8_t *pto = ( uint8_t* )to;
  const uint32_t blksize = INTERNAL_FLASH_READ_UNIT_SIZE;
  const uint32_t blkmask = INTERNAL_FLASH_READ_UNIT_SIZE - 1;

  // Align the start
  if(fromaddr & blkmask){
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
  if(temp){
    flashmem_read_internal( pto, fromaddr, temp );
    fromaddr += temp;
    pto += temp;
  }
  // And the final part of a block if needed
  if(rest){
    flashmem_read_internal( tmpdata, fromaddr, blksize );
    for( i = 0; size && ( i < rest ); i ++, size --, pto ++ )
      *pto = tmpdata[ i ];
  }
  return ssize;
}

//shorthand when start and end addresses of the sector are not needed
uint32_t flashmem_get_sector_of_address( uint32_t addr ){
  return (addr - INTERNAL_FLASH_START_ADDRESS) / INTERNAL_FLASH_SECTOR_SIZE;;
}

