#include <Arduino.h>
#include <flash_utils.h>
#include <eboot_command.h>
#include <spi_flash.h>

#ifdef ERASE_CONFIG_H

extern "C" {
#include "user_interface.h"

bool erase_config(const uint32_t flash_erase_mask) {
    // This is really the active configured size
    uint32_t flash_size = flashchip->chip_size;
    uint32_t erase_mask = (flash_erase_mask & (uint32_t)ERASE_CONFIG_ALL_DATA);
    uint32_t sector = flash_size/SPI_FLASH_SEC_SIZE - 1U;

    for (; !!erase_mask; erase_mask >>= 1U, sector--) {
        if ((erase_mask & 1U)) {
            if (0 != spi_flash_erase_sector(sector)) {
                return false;
            }
        }
    }

    return true;
}

bool check_and_erase_config(void) {
  // This should work since each element of the structure is a word.
  eboot_command volatile * ebcmd = (eboot_command volatile *)RTC_MEM;

  // We want to run after an OTA has completed and the bin has been moved to its
  // final resting place in flash. We want to catch the moment of the 1st boot
  // of this new sketch. Then verify we have a valid erase option.
  if (0U == ebcmd->magic &&
      0U == ebcmd->crc32 &&
      ACTION_COPY_RAW == ebcmd->action &&
      ebcmd->args[4] ==  ebcmd->args[6] &&
      ebcmd->args[5] ==  ebcmd->args[7] &&
      ebcmd->args[4] == ~ebcmd->args[5] &&
      0U == (ebcmd->args[4] & ~ERASE_CONFIG_ALL_DATA)) {

    uint32_t erase_flash_option = ebcmd->args[4];

    // Make sure we don't repeat
    for (size_t i=4; i<=7; i++)
      ebcmd->args[i] = 0U;

    if (erase_flash_option) {
      erase_config(erase_flash_option);
      system_restart();
      while(true){}
    }
  }
  return true;
}

};

#endif // ERASE_CONFIG_H
