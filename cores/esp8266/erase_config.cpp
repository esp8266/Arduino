#include <Arduino.h>
#include <flash_utils.h>
#include <eboot_command.h>
#include <spi_flash.h>

/*

Issue: Sometimes when an ESP8266 is reflashed/upgraded the WiFi does not work.
Then a  serial flash with Erase Flash with WiFi setting is recommended. I have
seen this more often when changing SDK by OTA. We don't have an erase WiFi for
OTA.

The PR tries to present a proof of concept solution. Actually it describes two
different methods.

There are 3 cases to consider when the firmware is updated by OTA. The new
firmware:
1. has the same Flash Configuration as the old.
2. has a larger Flash Configuration than the old.
3. has a smaller Flash Configuration then the old.

In theory after an OTA and before a restart, the flash could be erased for
_case 1_. _Case 2_ is a problem because the size exceeds what the SPIEraseSector
expects and fails. We have to wait for a restart, after which the SDK will have
updated the values in `flashchip`. _Case 3_ is potentially unsafe because we
could be erasing the code that is running.

At app_entry() `flashchip` properties appear to be reset to a default 4MByte
flash. Even an ESP8285 reported 4MByte. The value of `flashchip->chip_size` was
changed at the 2nd SPIRead. The 1st read was to address 0, 4 bytes.

To erase the flash WiFi area, I choose to wait until the SDK has finished its
adjustments to the `flashchip` structure. Then to begin erasing WiFi sectors.

I implemented two methods:
1. The first runs after the SDK calls `user_init()` and flash code execution is
available (No IRAM needed), but restarts to be sure the SDK  does not get
confused about its sectors being erased.
2. The 2nd method runs as early as possible required IRAM. The sectors are
erased before that 2nd read is processed. The SDK allowing the SDK to start off
thinking the sectors were blank at boot.

I also added an example/test sketch for exercising the feature. It is
OTAEraseConfig.


*/

#define ERASE_CONFIG_METHOD 1


#ifndef VAR_NAME_VALUE
#define VALUE(x) __STRINGIFY(x)
#define VAR_NAME_VALUE(var) #var " = "  VALUE(var)
#endif
#pragma message(VAR_NAME_VALUE(ERASE_CONFIG_METHOD))
#if (ERASE_CONFIG_METHOD == 0)
#undef ERASE_CONFIG_H
#endif


#ifdef ERASE_CONFIG_H
extern "C" {
#include "user_interface.h"

#if   (ERASE_CONFIG_METHOD == 1)
#define IRAM_MAYBE
#elif (ERASE_CONFIG_METHOD == 2)
#define IRAM_MAYBE ICACHE_RAM_ATTR
#else
#pragma GCC error "Unsupported ERASE_CONFIG_METHOD"
#endif

bool IRAM_MAYBE erase_config2(const uint32_t flash_erase_mask) {
    // This is really the active configured size
    uint32_t flash_size = flashchip->chip_size;
    uint32_t erase_mask = (flash_erase_mask & (uint32_t)ERASE_CONFIG_ALL_DATA);
    uint32_t sector = flash_size/SPI_FLASH_SEC_SIZE - 1U;

    for (; !!erase_mask; erase_mask >>= 1U, sector--) {
        if ((erase_mask & 1U)) {
#if (ERASE_CONFIG_METHOD == 1)
            if (0 != spi_flash_erase_sector(sector)) {
#elif (ERASE_CONFIG_METHOD == 2)
            if (0 != SPIEraseSector(sector)) {
#endif
                return false;
            }
        }
    }

    return true;
}

bool IRAM_MAYBE check_and_erase_config(void) {
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
#if (ERASE_CONFIG_METHOD == 1)
            erase_config2(erase_flash_option);
            system_restart();
            while(true){}
#elif (ERASE_CONFIG_METHOD == 2)
            return erase_config2(erase_flash_option);
#endif
        }
    }
    return true;
}


#if (ERASE_CONFIG_METHOD == 1)
extern struct rst_info resetInfo;

extern "C" void preinit (void)
{
    /* do nothing On power up */
    if (0 != resetInfo.reason) {
        check_and_erase_config();
    }
}



#elif (ERASE_CONFIG_METHOD == 2) //Another option
#include "cont.h"

int eboot_two_shots __attribute__((section(".noinit")));
extern cont_t* g_pcont;
extern "C" void call_user_start();

extern "C" void app_entry_redefinable(void)
{
    /* Allocate continuation context on this SYS stack,
       and save pointer to it. */
    cont_t s_cont __attribute__((aligned(16)));
    g_pcont = &s_cont;

    eboot_two_shots = 2;

    /* Call the entry point of the SDK code. */
    call_user_start();
}


void ICACHE_RAM_ATTR dbg_log_SPIRead(uint32_t addr, void *dest, size_t size, int err) __attribute__((weak));
void ICACHE_RAM_ATTR dbg_log_SPIRead(uint32_t addr, void *dest, size_t size, int err) {
  (void)addr;
  (void)dest;
  (void)size;
  (void)err;
}

#ifndef ROM_SPIRead
#define ROM_SPIRead         0x40004b1cU
#endif
typedef int (*fp_SPIRead_t)(uint32_t addr, void *dest, size_t size);
constexpr fp_SPIRead_t real_SPIRead = (fp_SPIRead_t)ROM_SPIRead;

int ICACHE_RAM_ATTR SPIRead(uint32_t addr, void *dest, size_t size) {
    // The very 1st read that goes by is to get the config flash size from
    // image header. The NONOS SDK will update flashchip->chip_size. Then, a
    // verification read is performed. Before this read is passed on we erase
    // config sectors.
    if (eboot_two_shots) {
        eboot_two_shots--;
        if (0 == eboot_two_shots)
            check_and_erase_config();
    }

    int err = real_SPIRead(addr, dest, size);
    dbg_log_SPIRead(addr, dest, size, err);
    return err;
}
#endif

};
#endif // ERASE_CONFIG_H
