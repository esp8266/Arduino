#include <Arduino.h>
#include <flash_utils.h>
#include <eboot_command.h>
#include <spi_flash.h>

/*

Issue: Sometimes when an ESP8266 is reflashed/upgraded the WiFi does not work.
Then a  serial flash with Erase Flash with WiFi setting is recommended. I have
seen this more often when changing SDK by OTA. We don't have an erase WiFi for
OTA.

This PR presents a proof of concept solution. Actually it describes three
different methods.

There are 3 cases to consider when the firmware is updated by OTA. The new
firmware:
1. has the same Flash Configuration as the old.
2. has a larger Flash Configuration than the old.
3. has a smaller Flash Configuration then the old.

In theory after an OTA and before a restart, the flash could be erased for
_case 1_. _Case 2_ is a problem because the size exceeds the size specified in
flashchip. That size is used by SPIEraseSector to validate the callers request
and fails when too large. We have to wait for a restart, after which the SDK
will have updated the values in `flashchip`. _Case 3_ is potentially unsafe
because we could be erasing the code that is running.

At app_entry() `flashchip` properties appear to be reset to a default 4MByte
flash. Even an ESP8285 reported 4MByte. The value of `flashchip->chip_size` was
changed at the 2nd SPIRead. The 1st read was to address 0, 4 bytes.

To erase the flash WiFi area for methods 1 and 2, I choose to wait until the SDK
has finished its adjustments to the `flashchip` structure. Then to begin erasing
WiFi sectors. Method 3 runs before the SDK starts. It temporarily updates
`flashchip->chip_size`. Then does the erase and puts everything back before
starting the SDK.

Summary of the three methods:

1. The first runs after the SDK calls `user_init()` and flash code execution is
available (No IRAM needed), but restarts to be sure the SDK  does not get
confused about its sectors being erased.

2. The 2nd method runs as early as possible requires IRAM. The sectors are
erased before the 2nd read is processed. This allows the SDK to start off
thinking the sectors were blank at boot.

3. Similar to method 2 runs as early as possible, turns on flash code execution
so more of the initialization code can be moved to flash. Directly modifies the
size element in `flashchip` structure in ROM data, dRAM. This allows the flash
erase to succeed.

The original flash size is restored before starting the SDK. With the assumption
that the SDK will handle the size change properly. Note that only changing the
size value in the `flashchip` structure, is equivalent to what the esptool.py is
doing.

I also added an example/test sketch for exercising the feature. It is
OTAEraseConfig. It also gathers some WiFi signal/connection statistics.

*/

#define ERASE_CONFIG_METHOD 3

#ifndef VAR_NAME_VALUE
#define VALUE(x) __STRINGIFY(x)
#define VAR_NAME_VALUE(var) #var " = "  VALUE(var)
#endif
#pragma message(VAR_NAME_VALUE(ERASE_CONFIG_METHOD))
#if (ERASE_CONFIG_METHOD == 0)
#undef ERASE_CONFIG_H
#endif

#ifdef DEBUG_ERASE_CONFIG
#define ETS_PRINTF(...) ets_uart_printf(__VA_ARGS__)
#define ETS_DELAY_US(a) ets_delay_us(a)
#define ETS_FLUSH(a) while((USS(a) >> USTXC) & 0xff){}
#define FIX_DIVIDER() fix_divider()

#else
#define ETS_PRINTF(...) do {} while(0)
#define ETS_DELAY_US(a) do {} while(0)
#define ETS_FLUSH(a) do {} while(false)
#define FIX_DIVIDER()  do {} while(false)
#endif

#ifdef ERASE_CONFIG_H
extern "C" {
#include "user_interface.h"

#if   (ERASE_CONFIG_METHOD == 1)
void __real_system_restart_local();
#define IRAM_MAYBE
#elif (ERASE_CONFIG_METHOD == 2)
#define IRAM_MAYBE ICACHE_RAM_ATTR
#elif (ERASE_CONFIG_METHOD == 3)
#define IRAM_MAYBE
#else
#pragma GCC error "Unsupported ERASE_CONFIG_METHOD"
#endif


void enable_erase_config_at_link_time(void) {
    /* This has to be called from somewhere for this module to get
       linked into the build. */
}

#if (ERASE_CONFIG_METHOD == 3)
extern "C" void Cache_Read_Enable_2(void);
extern "C" void Cache_Read_Disable_2(void);

/* Cannot be made static. The compiler will inline into erase_config and lose
   ICACHE_RAM_ATTR. Make name longer to be more unique */
int ICACHE_RAM_ATTR erase_config__erase_sector(const uint32_t sector) {
    /*
       Toggle Flash execution off and on, around ROM flash function calls.
       The SDK APIs would have normally handled this operation; however,
       it is too early for it to take calls.
     */
    int rc;
    Cache_Read_Disable_2();
    rc = SPIEraseSector(sector);
    Cache_Read_Enable_2();
    return rc;
}
#endif

bool IRAM_MAYBE erase_config(const uint32_t flash_erase_mask) {
    /* This is really the active configured size */
    uint32_t flash_size = flashchip->chip_size;
    uint32_t erase_mask = (flash_erase_mask & (uint32_t)ERASE_CONFIG_ALL_DATA);
    uint32_t sector = flash_size/SPI_FLASH_SEC_SIZE - 1U;

    for (; !!erase_mask; erase_mask >>= 1U, sector--) {
        if ((erase_mask & 1U)) {
#if (ERASE_CONFIG_METHOD == 1)
            if (0 != spi_flash_erase_sector(sector)) {
#elif (ERASE_CONFIG_METHOD == 2)
            if (0 != SPIEraseSector(sector)) {
#elif (ERASE_CONFIG_METHOD == 3)
            if (0 != erase_config__erase_sector(sector)) {
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
    /* This should work since each element of the structure is a word. */
    eboot_command volatile * ebcmd = (eboot_command volatile *)RTC_MEM;

    /*
       We want to run after an OTA has completed and the bin has been moved to
       its final resting place in flash. We want to catch the moment of the 1st
       boot of this new sketch. Then verify we have a valid erase option.
     */
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
            ETS_FLUSH(0);
            __real_system_restart_local();
            while(true){}
#elif (ERASE_CONFIG_METHOD == 2) || (ERASE_CONFIG_METHOD == 3)
            return erase_config(erase_flash_option);
#endif
        }
    } else {
        ETS_PRINTF("\nNo OTA erase flags\n");
    }
    return true;
}

#if defined(DEBUG_ERASE_CONFIG)
/******************************************************************************

                      Adjustments for Debug Printing

   Start of stuff to get serial printing to work just a little longer
   This block of code helps maintain the UART data rate of 74880 for debug
   printing. This is only needed when the CPU Crystal Frequency is not 40MHz.

   When a 26MHz Crystal is used, the UART is underclocked from 115200 to an
   effective data rate of 74880 bps. When the NONOS SDK initializes, the PLL
   circuit for the CPU Clock is corrected to work with the 26MHz Crystal. And
   the UART rate then shifts to being 115200 bps. It appears that the PLL
   adjustments are cleared after a EXT_RST, PD_EN, and power-on. On soft
   restarts, including exceptions and soft/hardware WDT reset, it appears the
   PLL adjustments are preserved, the UART date rate is 115200 bps.

   The solution impelmented here is to adjust the UART data rate to  an
   effective rate of 74880 based on the kind of reset. This minimizes the
   garbled printing.

   Assumes a data rate of 74880.
   miniterm.py does a good job of handling this rate on Linux.
 */

/* ROM reset reason values returned by `rtc_get_reset_reason` are different from
   the NONOS SDK reset reasons. */
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
   Here we use uart_div_modify in the Boot ROM. Note the Boot ROM version does
   not do any input validation.

   The SDK has an overide on uart_div_modify. We cannot use its replacement. It
   is not in IRAM and reauires SDK initialization.
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
       early callback(s) from the SDK like run_user_rf_pre_init(). The UART
       speed appears to always be based on the precalibrated crystal frequency.
     */
    uint32_t divider = UART_CLK_FREQ / 115200;
    if (is_cpu_freq_cal()) {
        divider = UART_CLK_FREQ / 74880;
    }
    real_uart_div_modify(0, divider);
    ETS_DELAY_US(150);
}

/*
  Something to see when we are in the SDK initialization.
*/
extern "C" void ICACHE_RAM_ATTR _Z22__run_user_rf_pre_initv(void) {
    real_uart_div_modify(0, UART_CLK_FREQ / 115200);
    ETS_DELAY_US(150);
    ETS_PRINTF("\n__run_user_rf_pre_init()\n");
    ETS_FLUSH(0);
}

#if 1
/*
  This helps keep the UART enabled at user_init() so we can get a few more
  messages printed.
*/
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
            /* Restore pin to TX after Power-on and EXT_RST */
            __pinMode(pin, FUNCTION_0);
        }
        in_initPins = false;
        return;
    }

    __pinMode( pin, mode );
}
#endif
/*
  End of stuff to help printing work.
 *****************************************************************************/
#endif


#if (ERASE_CONFIG_METHOD == 1)
extern struct rst_info resetInfo;

extern "C" void preinit (void) {
    /* do nothing On power up */
    if (0 != resetInfo.reason) {
        check_and_erase_config();
    }
}

#elif (ERASE_CONFIG_METHOD == 2)

#include "cont.h"

int eboot_two_shots __attribute__((section(".noinit")));
extern cont_t* g_pcont;
extern "C" void call_user_start();

extern "C" void ICACHE_RAM_ATTR app_entry_redefinable(void) {
    /* Allocate continuation context on this SYS stack,
       and save pointer to it. */
    cont_t s_cont __attribute__((aligned(16)));
    g_pcont = &s_cont;

    eboot_two_shots = 2;

    FIX_DIVIDER();
    ETS_PRINTF("\n\ncall_user_start()\n");
    ETS_FLUSH(0);

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
    /*
       The very 1st read that goes by is to get the config flash size from
       image header. The NONOS SDK will update flashchip->chip_size. Then, a
       verification read is performed. Before this read is passed on we erase
       config sectors.
     */
    if (eboot_two_shots) {
        eboot_two_shots--;
        if (0 == eboot_two_shots) {
            check_and_erase_config();
            ETS_FLUSH(0);
        }
    }

    int err = real_SPIRead(addr, dest, size);
    dbg_log_SPIRead(addr, dest, size, err);
    return err;
}

#elif (ERASE_CONFIG_METHOD == 3) // Newest option
extern "C" void Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v);
extern "C" void Cache_Read_Disable();
extern "C" uint32_t esp_c_magic_flash_chip_size(uint8_t byte);

#if defined(DEBUG_ERASE_CONFIG)
void print_flashchip() {
    ETS_PRINTF("\nflashchip->deviceId:      0x%08X, %8u\n", flashchip->deviceId,    flashchip->deviceId);
    ETS_PRINTF("flashchip->chip_size:     0x%08X, %8u\n", flashchip->chip_size,   flashchip->chip_size);
    ETS_PRINTF("flashchip->block_size:    0x%08X, %8u\n", flashchip->block_size,  flashchip->block_size);
    ETS_PRINTF("flashchip->sector_size:   0x%08X, %8u\n", flashchip->sector_size, flashchip->sector_size);
    ETS_PRINTF("flashchip->page_size:     0x%08X, %8u\n", flashchip->page_size,   flashchip->page_size);
    ETS_PRINTF("flashchip->status_mask:   0x%08X, %8u\n", flashchip->status_mask, flashchip->status_mask);
}
#define PRINT_FLASHCHIP() print_flashchip()
#else
#define PRINT_FLASHCHIP() do{}while(false)
#endif

/* Cannot be made static, compiler will inline into erase_config_method3 and use
   more IRAM. */
void set_flashchip_and_check_erase_config(void) {
    /*
       We patch and restore chip_size here. It should be noted that the ROM APIs
       use both the chip_size and sector_size to validate calling parameters.
       From what I see at least with esptool.py and core, there is a general
       assumption that sector_size is always 4K.

       I don't see a need to set and restore sector_size at this time.
     */
    PRINT_FLASHCHIP();
    /* Since Flash code has been mapped for execution, we can just address the
       flash image header located at the beginning of flash as iCACHE like memory. */
    const uint32_t imghdr_4bytes = *((uint32_t *)0x40200000);
    const image_header_t *partial_imghdr = (const image_header_t *)&imghdr_4bytes;
    uint32_t old_flash_size = flashchip->chip_size;
    flashchip->chip_size = esp_c_magic_flash_chip_size((partial_imghdr->flash_size_freq >> 4) & 0x0F);
    PRINT_FLASHCHIP();
    if (flashchip->chip_size) {
        check_and_erase_config();
    }
    flashchip->chip_size = old_flash_size;
}

void ICACHE_RAM_ATTR erase_config_method3(void) {
    Cache_Read_Enable(0, 0, 0); // 16K ICACHE
    set_flashchip_and_check_erase_config();
    Cache_Read_Disable();
}

#include "cont.h"

extern cont_t* g_pcont;
extern "C" void call_user_start();

extern "C" void ICACHE_RAM_ATTR app_entry_redefinable(void) {
    /* Allocate continuation context on this SYS stack,
       and save pointer to it. */
    cont_t s_cont __attribute__((aligned(16)));
    g_pcont = &s_cont;

    FIX_DIVIDER();
    erase_config_method3();

    ETS_PRINTF("\n\ncall_user_start()\n");
    ETS_FLUSH(0);

    /* Call the entry point of the SDK code. */
    call_user_start();
}

#endif

};
#endif // ERASE_CONFIG_H
