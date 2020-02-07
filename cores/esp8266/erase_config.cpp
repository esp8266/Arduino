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

#define ERASE_CONFIG_METHOD 2


#ifndef VAR_NAME_VALUE
#define VALUE(x) __STRINGIFY(x)
#define VAR_NAME_VALUE(var) #var " = "  VALUE(var)
#endif
#pragma message(VAR_NAME_VALUE(ERASE_CONFIG_METHOD))
#if (ERASE_CONFIG_METHOD == 0)
#undef ERASE_CONFIG_H
#endif

#if 1
#define ETS_PRINTF(...) ets_uart_printf(__VA_ARGS__)
#define ETS_DELAY_US(a) ets_delay_us(a)
#else
#define ETS_PRINTF(...) do {} while(0)
#define ETS_DELAY_US(a) do {} while(0)
#endif

#ifdef ERASE_CONFIG_H
extern "C" {
#include "user_interface.h"
void __real_system_restart_local();

#if   (ERASE_CONFIG_METHOD == 1)
#define IRAM_MAYBE
#elif (ERASE_CONFIG_METHOD == 2)
#define IRAM_MAYBE ICACHE_RAM_ATTR
#else
#pragma GCC error "Unsupported ERASE_CONFIG_METHOD"
#endif


void enable_erase_config_at_link_time(void) {
    /*
      This has to be called from somewhere for this module to get
      linked into the build.
    */
}

bool IRAM_MAYBE erase_config(const uint32_t flash_erase_mask) {
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
                ETS_PRINTF("Erase sector 0x%04X failed!\n", sector);
                return false;
            } else {
                ETS_PRINTF("Erased sector 0x%04X\n", sector);
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
            ETS_PRINTF("\nerase_config(0x%03X)\n", erase_flash_option);
#if (ERASE_CONFIG_METHOD == 1)
            erase_config(erase_flash_option);
            ETS_PRINTF("\n__real_system_restart_local\n\n");
            ETS_DELAY_US(12000);
            __real_system_restart_local();
            while(true){}
#elif (ERASE_CONFIG_METHOD == 2)
            return erase_config(erase_flash_option);
#endif
        }
    } else {
        ETS_PRINTF("\nNo OTA erase flags\n");
    }
    return true;
}




typedef enum ROM_RST_REASON { /* Comments on the right are from RTOS SDK */
  NO_MEAN                =  0,    /* Undefined */
  POWERON_RESET          =  1,    /* Power on boot      *//**<1, Vbat power on reset */
  EXT_RESET              =  2,    /* External reset or wake-up from Deep-sleep */
                                                          /**<2, external system reset */
  SW_RESET               =  3,    /*                    *//**<3, Software reset digital core */
  OWDT_RESET             =  4,    /* Hardware WDT reset *//**<4, Legacy watch dog reset digital core */
  DEEPSLEEP_RESET        =  5,    /*                    *//**<5, Deep Sleep reset digital core */
  SDIO_RESET             =  6,    /*                    *//**<6, Reset by SLC module, reset digital core*/
} ROM_RST_REASON_t;

constexpr volatile uint32_t *RTC_SYS = (volatile uint32_t*)0x60001100;
extern uint32_t rtc_get_reset_reason(void);

bool ICACHE_RAM_ATTR is_cpu_freq_cal(void) {
    const uint32_t rtc_sys_reason = RTC_SYS[0];
    const uint32_t rom_api_reason = rtc_get_reset_reason();
    if (1 >= rtc_sys_reason && OWDT_RESET != rom_api_reason) {
      return false;  // REASON_EXT_SYS_RST
    }
    if (REASON_EXT_SYS_RST < rtc_sys_reason) {
      return false;  // REASON_DEFAULT_RST
    }
    return true;
}

/*
 * Here we use uart_div_modify in the Boot ROM. Note the Boot ROM version does
 * not do any input validation.
 *
 * We cannot use the SDKs replacement. It is not in IRAM.
 */
#ifndef ROM_uart_div_modify
#define ROM_uart_div_modify         0x400039d8
#endif
typedef void (*fp_uart_div_modify_t)(uint32_t uart_no, uint32 DivLatchValue);
constexpr fp_uart_div_modify_t real_uart_div_modify = (fp_uart_div_modify_t)ROM_uart_div_modify;

void ICACHE_RAM_ATTR fix_divider(void) {
    /*
      When reset cause is not power-on or EXT_RST the CPU crystal calibration
      has been done and there is no need to correct. With the exception of some
      early callback(s) from the SDK like run_user_rf_pre_init(). The UART speed
      appears to always be based on the precalibrated crystal frequency.
    */
    uint32_t divider = UART_CLK_FREQ / 115200;
    if (is_cpu_freq_cal()) {
        divider = UART_CLK_FREQ / 74880;
    }
    real_uart_div_modify(0, divider);
    ETS_DELAY_US(150);
}





#if (ERASE_CONFIG_METHOD == 1)
extern struct rst_info resetInfo;

extern "C" void preinit (void) {
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

extern "C" void ICACHE_RAM_ATTR app_entry_redefinable(void)
{
    /* Allocate continuation context on this SYS stack,
       and save pointer to it. */
    cont_t s_cont __attribute__((aligned(16)));
    g_pcont = &s_cont;

    eboot_two_shots = 2;

    fix_divider();
    ETS_PRINTF("\n\ncall_user_start()\n");
    ETS_DELAY_US(12000);

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
        if (0 == eboot_two_shots) {
            fix_divider();
            check_and_erase_config();
            ETS_DELAY_US(12000);
        }
    }

    int err = real_SPIRead(addr, dest, size);
    dbg_log_SPIRead(addr, dest, size, err);
    return err;
}


#endif

};
#endif // ERASE_CONFIG_H


extern "C" void ICACHE_RAM_ATTR _Z22__run_user_rf_pre_initv(void) {
    // fix_divider();
    real_uart_div_modify(0, UART_CLK_FREQ / 115200);
    ETS_DELAY_US(150);
    ets_uart_printf("\n__run_user_rf_pre_init()\n");
    ETS_DELAY_US(12000);
}

/*
  This helps keep the UART enabled longer so we can get a few more messages
  printed.
*/
#if 1
extern struct rst_info resetInfo;
extern "C" void __pinMode( uint8_t pin, uint8_t mode );

inline bool is_gpio_persistent(void) {
  return REASON_EXCEPTION_RST <= resetInfo.reason &&
         REASON_SOFT_RESTART  >= resetInfo.reason;
}

extern "C" void pinMode( uint8_t pin, uint8_t mode ) {
  static bool in_initPins = true;
  if (in_initPins && (1 == pin)) {
    if (!is_gpio_persistent()) {
      // Restore pin to TX after Power-on and EXT_RST
      __pinMode(pin, FUNCTION_0);
    }
    in_initPins = false;
    return;
  }

  __pinMode( pin, mode );
}
#endif
