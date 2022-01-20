/*
   Demonstrate CRC check passing and failing by simulating a bit flip in flash.
   WARNING!!!  You would never want to actually do this in a real application!

   Released to the Public Domain by Earle F. Philhower, III <earlephilhower@yahoo.com>
*/

extern "C" {
#include "spi_flash.h"
}
// Artificially create a space in PROGMEM that fills multiple sectors so
// we can corrupt one without crashing the system
const int corruptme[SPI_FLASH_SEC_SIZE * 4] PROGMEM = { 0 };

void setup() {
  Serial.begin(115200);
  Serial.printf("Starting\n");
  Serial.printf("CRC check: %s\n", ESP.checkFlashCRC() ? "OK" : "ERROR");
  Serial.printf("...Corrupting a portion of flash in the array...\n");

  uint32_t ptr = (uint32_t)corruptme;
  // Find a page aligned spot inside the array
  ptr += 2 * SPI_FLASH_SEC_SIZE;
  ptr &= ~(SPI_FLASH_SEC_SIZE - 1);  // Sectoralign
  uint32_t sector = ((((uint32_t)ptr - 0x40200000) / SPI_FLASH_SEC_SIZE));

  // Create a sector with 1 bit set (i.e. fake corruption)
  uint32_t* space = (uint32_t*)calloc(SPI_FLASH_SEC_SIZE, 1);
  space[42]       = 64;

  // Write it into flash at the spot in question
  spi_flash_erase_sector(sector);
  spi_flash_write(sector * SPI_FLASH_SEC_SIZE, (uint32_t*)space, SPI_FLASH_SEC_SIZE);
  Serial.printf("CRC check: %s\n", ESP.checkFlashCRC() ? "OK" : "ERROR");

  Serial.printf("...Correcting the flash...\n");
  memset(space, 0, SPI_FLASH_SEC_SIZE);
  spi_flash_erase_sector(sector);
  spi_flash_write(sector * SPI_FLASH_SEC_SIZE, (uint32_t*)space, SPI_FLASH_SEC_SIZE);
  Serial.printf("CRC check: %s\n", ESP.checkFlashCRC() ? "OK" : "ERROR");
}

void loop() { }
