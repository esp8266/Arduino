/*
 main.cpp - platform initialization and context switching
 emulation

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

//This may be used to change user task stack size:
//#define CONT_STACKSIZE 4096

#include <numeric>

#include <Arduino.h>
#include "Schedule.h"
extern "C" {
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "cont.h"
}
#include <core_version.h>
#include "gdb_hooks.h"
#include "flash_quirks.h"
#include "hwdt_app_entry.h"
#include <umm_malloc/umm_malloc.h>
#include <core_esp8266_non32xfer.h>
#include "core_esp8266_vm.h"

#define LOOP_TASK_PRIORITY 1
#define LOOP_QUEUE_SIZE    1

extern "C" void call_user_start();
extern void loop();
extern void setup();
extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

/* Not static, used in Esp.cpp */
struct rst_info resetInfo;

/* Not static, used in core_esp8266_postmortem.c and other places.
 * Placed into noinit section because we assign value to this variable
 * before .bss is zero-filled, and need to preserve the value.
 */
cont_t* g_pcont __attribute__((section(".noinit")));

/* Event queue used by the main (arduino) task */
static os_event_t s_loop_queue[LOOP_QUEUE_SIZE];

/* Used to implement optimistic_yield */
static uint32_t s_cycles_at_resume;

/* For ets_intr_lock_nest / ets_intr_unlock_nest
 * Max nesting seen by SDK so far is 2.
 */
#define ETS_INTR_LOCK_NEST_MAX 7
static uint16_t ets_intr_lock_stack[ETS_INTR_LOCK_NEST_MAX];
static uint8_t  ets_intr_lock_stack_ptr=0;


extern "C" {
extern const uint32_t __attribute__((section(".ver_number"))) core_version = ARDUINO_ESP8266_GIT_VER;
const char* core_release =
#ifdef ARDUINO_ESP8266_RELEASE
    ARDUINO_ESP8266_RELEASE;
#else
    NULL;
#endif

static os_timer_t delay_timer;
#define ONCE 0
#define REPEAT 1
} // extern "C"

void initVariant() __attribute__((weak));
void initVariant() {
}

extern "C" void __preloop_update_frequency() {
#if defined(F_CPU) && (F_CPU == 160000000L)
    ets_update_cpu_frequency(160);
    CPU2X |= 1UL;
#elif defined(F_CPU)
    ets_update_cpu_frequency(80);
    CPU2X &= ~1UL;
#elif !defined(F_CPU)
    if (system_get_cpu_freq() == 160) {
        CPU2X |= 1UL;
    }
    else {
        CPU2X &= ~1UL;
    }
#endif
}

extern "C" void preloop_update_frequency() __attribute__((weak, alias("__preloop_update_frequency")));

extern "C" bool can_yield() {
  return cont_can_suspend(g_pcont);
}

static inline void esp_suspend_within_cont() __attribute__((always_inline));
static void esp_suspend_within_cont() {
        cont_suspend(g_pcont);
        s_cycles_at_resume = ESP.getCycleCount();
        run_scheduled_recurrent_functions();
}

extern "C" void __esp_suspend() {
    if (cont_can_suspend(g_pcont)) {
        esp_suspend_within_cont();
    }
}

extern "C" void esp_suspend() __attribute__ ((weak, alias("__esp_suspend")));

extern "C" IRAM_ATTR void esp_schedule() {
    ets_post(LOOP_TASK_PRIORITY, 0, 0);
}

// Replacement for delay(0). In CONT, same as yield(). Whereas yield() panics
// in SYS, esp_yield() is safe to call and only schedules CONT. Use yield()
// whereever only called from CONT, use esp_yield() if code is called from SYS
// or both CONT and SYS.
extern "C" void esp_yield() {
    esp_schedule();
    esp_suspend();
}

void delay_end(void* arg) {
    (void)arg;
    esp_schedule();
}

extern "C" void __esp_delay(unsigned long ms) {
    if (ms) {
        os_timer_setfn(&delay_timer, (os_timer_func_t*)&delay_end, 0);
        os_timer_arm(&delay_timer, ms, ONCE);
    }
    else {
        esp_schedule();
    }
    esp_suspend();
    if (ms) {
        os_timer_disarm(&delay_timer);
    }
}

extern "C" void esp_delay(unsigned long ms) __attribute__((weak, alias("__esp_delay")));

bool esp_try_delay(const uint32_t start_ms, const uint32_t timeout_ms, const uint32_t intvl_ms) {
    if (!timeout_ms) {
        esp_yield();
        return true;
    }

    uint32_t expired = millis() - start_ms;
    if (expired >= timeout_ms) {
        return true; // expired
    }

    // compute greatest chunked delay with respect to scheduled recurrent functions
    uint32_t grain_ms = std::gcd(intvl_ms, compute_scheduled_recurrent_grain());

    // recurrent scheduled functions will be called from esp_delay()->esp_suspend()
    esp_delay(grain_ms > 0 ?
        std::min((timeout_ms - expired), grain_ms):
        (timeout_ms - expired));

    return false; // expiration must be checked again
}

extern "C" void __yield() {
    if (cont_can_suspend(g_pcont)) {
        esp_schedule();
        esp_suspend_within_cont();
    }
    else {
        panic();
    }
}

extern "C" void yield(void) __attribute__ ((weak, alias("__yield")));

// In CONT, actually performs yield() only once the given time interval
// has elapsed since the last time yield() occured. Whereas yield() panics
// in SYS, optimistic_yield() additionally is safe to call and does nothing.
extern "C" void optimistic_yield(uint32_t interval_us) {
    const uint32_t intvl_cycles = interval_us *
#if defined(F_CPU)
        clockCyclesPerMicrosecond();
#else
        ESP.getCpuFreqMHz();
#endif
    if ((ESP.getCycleCount() - s_cycles_at_resume) > intvl_cycles &&
        can_yield())
    {
        yield();
    }
}

// Replace ets_intr_(un)lock with nestable versions
extern "C" void IRAM_ATTR ets_intr_lock() {
  if (ets_intr_lock_stack_ptr < ETS_INTR_LOCK_NEST_MAX)
     ets_intr_lock_stack[ets_intr_lock_stack_ptr++] = xt_rsil(3);
  else
     xt_rsil(3);
}

extern "C" void IRAM_ATTR ets_intr_unlock() {
  if (ets_intr_lock_stack_ptr > 0)
     xt_wsr_ps(ets_intr_lock_stack[--ets_intr_lock_stack_ptr]);
  else
     xt_rsil(0);
}


// Save / Restore the PS state across the rom ets_post call as the rom code
// does not implement this correctly.
extern "C" bool ets_post_rom(uint8 prio, ETSSignal sig, ETSParam par);

extern "C" bool IRAM_ATTR ets_post(uint8 prio, ETSSignal sig, ETSParam par) {
  uint32_t saved;
  __asm__ __volatile__ ("rsr %0,ps":"=a" (saved));
  bool rc=ets_post_rom(prio, sig, par);
  xt_wsr_ps(saved);
  return rc;
}

extern "C" void __loop_end (void)
{
    run_scheduled_functions();
    run_scheduled_recurrent_functions();
}

extern "C" void loop_end (void) __attribute__ ((weak, alias("__loop_end")));

static void loop_wrapper() {
    static bool setup_done = false;
    preloop_update_frequency();
    if(!setup_done) {
        setup();
        setup_done = true;
    }
    loop();
    loop_end();
    cont_check_guard(g_pcont);
    if (serialEventRun) {
        serialEventRun();
    }
    esp_schedule();
}

extern "C" void __stack_chk_fail(void);

static void loop_task(os_event_t *events) {
    (void) events;
    s_cycles_at_resume = ESP.getCycleCount();
    ESP.resetHeap();
    cont_run(g_pcont, &loop_wrapper);
    ESP.setDramHeap();
}

extern "C" {
struct object { long placeholder[ 10 ]; };
void __register_frame_info (const void *begin, struct object *ob);
extern char __eh_frame[];
}

static void do_global_ctors(void) {
    static struct object ob;
    __register_frame_info( __eh_frame, &ob );

    void (**p)(void) = &__init_array_end;
    while (p != &__init_array_start)
        (*--p)();
}

extern "C" {
extern void __unhandled_exception(const char *str);

static void  __unhandled_exception_cpp()
{
#ifndef __EXCEPTIONS
    abort();
#else
    static bool terminating;
    if (terminating)
        abort();
    terminating = true;
    /* Use a trick from vterminate.cc to get any std::exception what() */
    try {
        __throw_exception_again;
    } catch (const std::exception& e) {
        __unhandled_exception( e.what() );
    } catch (...) {
        __unhandled_exception( "" );
    }
#endif
}
}

void init_done() {
    system_set_os_print(1);
    gdb_init();
    std::set_terminate(__unhandled_exception_cpp);
    do_global_ctors();
    esp_schedule();
    ESP.setDramHeap();
}

/* This is the entry point of the application.
 * It gets called on the default stack, which grows down from the top
 * of DRAM area.
 * .bss has not been zeroed out yet, but .data and .rodata are in place.
 * Cache is not enabled, so only ROM and IRAM functions can be called.
 * Peripherals (except for SPI0 and UART0) are not initialized.
 * This function does not return.
 */
/*
   A bit of explanation for this entry point:

   SYS is the SDK task/context used by the upperlying system to run its
   administrative tasks (at least WLAN and lwip's receive callbacks and
   Ticker).  NONOS-SDK is designed to run user's non-threaded code in
   another specific task/context with its own stack in BSS.

   Some clever fellows found that the SYS stack was a large and quite unused
   piece of ram that we could use for the user's stack instead of using user's
   main memory, thus saving around 4KB on ram/heap.

   A problem arose later, which is that this stack can heavily be used by
   the SDK for some features.  One of these features is WPS.  We still don't
   know if other features are using this, or if this memory is going to be
   used in future SDK releases.

   WPS being flawed by its poor security, or not being used by lots of
   users, it has been decided that we are still going to use that memory for
   user's stack and disable the use of WPS.

   app_entry() jumps to app_entry_custom() defined as "weakref" calling
   itself a weak customizable function, allowing to use another one when
   this is required (see core_esp8266_app_entry_noextra4k.cpp, used by WPS).

   (note: setting app_entry() itself as "weak" is not sufficient and always
    ends up with the other "noextra4k" one linked, maybe because it has a
    default ENTRY(app_entry) value in linker scripts).

   References:
   https://github.com/esp8266/Arduino/pull/4553
   https://github.com/esp8266/Arduino/pull/4622
   https://github.com/esp8266/Arduino/issues/4779
   https://github.com/esp8266/Arduino/pull/4889

*/

extern "C" void app_entry_redefinable(void) __attribute__((weak));
extern "C" void app_entry_redefinable(void)
{
    /* Allocate continuation context on this SYS stack,
       and save pointer to it. */
    cont_t s_cont __attribute__((aligned(16)));
    g_pcont = &s_cont;

    /* Doing umm_init just once before starting the SDK, allowed us to remove
       test and init calls at each malloc API entry point, saving IRAM. */
#ifdef UMM_INIT_USE_IRAM
    umm_init();
#else
    // umm_init() is in IROM
    mmu_wrap_irom_fn(umm_init);
#endif
    /* Call the entry point of the SDK code. */
    call_user_start();
}
static void app_entry_custom (void) __attribute__((weakref("app_entry_redefinable")));

extern "C" void app_entry (void)
{
    return app_entry_custom();
}

extern "C" void preinit (void) __attribute__((weak));
extern "C" void preinit (void)
{
    /* does nothing, kept for backward compatibility */
}

extern "C" void __disableWiFiAtBootTime (void) __attribute__((weak));
extern "C" void __disableWiFiAtBootTime (void)
{
    // Starting from arduino core v3: wifi is disabled at boot time
    // WiFi.begin() or WiFi.softAP() will wake WiFi up
    wifi_set_opmode_current(0/*WIFI_OFF*/);
    wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
    wifi_fpm_open();
    wifi_fpm_do_sleep(0xFFFFFFF);
}

#if FLASH_MAP_SUPPORT
#include "flash_hal.h"
extern "C" const char *flashinit (void);
uint32_t __flashindex;
#endif

#if (NONOSDK >= (0x30000))
#undef ETS_PRINTF
#define ETS_PRINTF(...) ets_uart_printf(__VA_ARGS__)
extern "C" uint8_t uart_rx_one_char_block();

#if ! FLASH_MAP_SUPPORT
#include "flash_hal.h"
#endif

extern "C" void user_rf_pre_init();

extern "C" void ICACHE_FLASH_ATTR user_pre_init(void)
{
    const char *flash_map_str = NULL;
    const char *chip_sz_str = NULL;
    const char *table_regist_str = NULL;
    [[maybe_unused]] uint32_t ld_config_chip_size = 0;
    uint32_t flash_size = 0;
    uint32_t phy_data = 0;
    uint32_t rf_cal = 0;
    uint32_t system_parameter = 0;
    [[maybe_unused]] const partition_item_t *_at_partition_table = NULL;
    size_t _at_partition_table_sz = 0;

    do {
        #if FLASH_MAP_SUPPORT
        flash_map_str = flashinit();
        if (flash_map_str) {
            continue;
        }
        #endif

        // For SDKs 3.0.0 and later, place phy_data readonly overlay on top of
        // the EEPROM address. For older SDKs without a system partition, RF_CAL
        // and PHY_DATA shared the same flash segment.
        //
        // For the Arduino ESP8266 core, the sectors for "EEPROM = size -
        // 0x5000", "RF_CAL =  size - 0x4000", and "SYSTEM_PARAMETER = size -
        // 0x3000" are positioned in the last five sectors of flash memory.
        // PHY_INIT_DATA is special. It is a one time read of 128 bytes of data
        // that is provided by a spoofed flash read.
        #if FLASH_MAP_SUPPORT
        flash_size = __flashdesc[__flashindex].flash_size_kb * 1024u;
        #else
        // flashchip->chip_size is updated by the SDK. The size is based on the
        // value patched into the .bin header by esptool.
        // system_get_flash_size_map() returns that patched value.
        flash_size = flashchip->chip_size;
        #endif

        // For all configurations, place RF_CAL and system_parameter in the
        // last 4 sectors of the flash chip.
        rf_cal = flash_size - 0x4000u;
        system_parameter = flash_size - 0x3000u;

        // The system_partition_table_regist will not allow partitions to
        // overlap. EEPROM_start is a good choice for phy_data overlay. The SDK
        // does not need to know about EEPROM_start. So we can omit it from the
        // table. The real EEPROM access is after user_init() begins long after
        // the PHY_DATA read. So it should be safe from conflicts.
        phy_data = EEPROM_start - 0x40200000u;

        // For SDKs 3.0 builds, "sdk3_begin_phy_data_spoof and
        // user_rf_cal_sector_set" starts and stops the spoofing logic in
        // `core_esp8266_phy.cpp`.
        extern void sdk3_begin_phy_data_spoof();
        sdk3_begin_phy_data_spoof();

        ld_config_chip_size = phy_data + 4096 * 5;

        // -DALLOW_SMALL_FLASH_SIZE=1
        // Allows for small flash-size builds targeted for multiple devices,
        // commonly IoT, of varying flash sizes.
        #if !defined(FLASH_MAP_SUPPORT) && !defined(ALLOW_SMALL_FLASH_SIZE)
        // Note, system_partition_table_regist will only catch when the build
        // flash size value set by the Arduino IDE Tools menu is larger than
        // the firmware image value detected and updated on the fly by esptool.
        if (flashchip->chip_size != ld_config_chip_size) {
            // Stop to avoid possible stored flash data corruption. This
            // mismatch will not occur with flash size selection "Mapping
            // defined by Hardware and Sketch".
            chip_sz_str = PSTR("Flash size mismatch, check that the build setting matches the device.\n");
            continue;
        }
        #elif defined(ALLOW_SMALL_FLASH_SIZE) && !defined(FLASH_MAP_SUPPORT)
        // Note, while EEPROM is confined to a smaller flash size, we are still
        // placing RF_CAL and SYSTEM_PARAMETER at the end of flash. To prevent
        // this, esptool or its equal needs to not update the flash size in the
        // .bin image.
        #endif

        #if FLASH_MAP_SUPPORT && defined(DEBUG_ESP_PORT)
        // I don't think this will ever fail. Everything traces back to the results of spi_flash_get_id()
        if (flash_size != flashchip->chip_size) {
            chip_sz_str = PSTR("Flash size mismatch, check that the build setting matches the device.\n");
            continue;
        }
        #endif

        // All the examples I find, show the partition table in the global address space.
        static const partition_item_t at_partition_table[] =
        {
            { SYSTEM_PARTITION_PHY_DATA,         phy_data,         0x1000 }, // type 5
            { SYSTEM_PARTITION_RF_CAL,           rf_cal,           0x1000 }, // type 4
            { SYSTEM_PARTITION_SYSTEM_PARAMETER, system_parameter, 0x3000 }, // type 6
        };
        _at_partition_table = at_partition_table;
        _at_partition_table_sz = std::size(at_partition_table);
        // SDK 3.0's `system_partition_table_regist` is FOTA-centric. It will report
        // on BOOT, OTA1, and OTA2 being missing. We are Non-FOTA. I don't see
        // anything we can do about this. Other than maybe turning off os_print.
        if (!system_partition_table_regist(at_partition_table, _at_partition_table_sz, system_get_flash_size_map())) {
            table_regist_str = PSTR("System partition table registration failed!\n");
            continue;
        }
    } while(false);

    if (chip_sz_str || flash_map_str || table_regist_str) {
        // user_pre_init() is called very early in the SDK startup. When called,
        // the PLL CPU clock calibration hasn't not run. Since we are failing, the
        // calibration will never complete. And the process will repeat over and
        // over. The effective data rate will always be 74880 bps. If we had a
        // successful boot, the effective data rate would be 115200 on a restart
        // or HWDT. This hack relies on the CPU clock calibration never having
        // completed. This assumes we are starting from a hard reset.

        // A possible exception would be a soft reset after flashing. In which
        // case the message will not be readable until after a hard reset or
        // power cycle.

        // After flashing, the Arduino Serial Monitor needs a moment to
        // reconnect. This also allows time for the FIFO to clear and the host
        // serial port to clear any framing errors.
        ets_delay_us(200u * 1000u); // For an uncalibrated CPU Clock, this is close enough.

        #if !defined(F_CRYSTAL)
        #define F_CRYSTAL 26000000
        #endif
        // For print messages to be readable, the UART clock rate is based on the
        // precalibration rate.
        if (F_CRYSTAL != 40000000) {
            uart_div_modify(0, F_CRYSTAL * 2 / 115200);
            ets_delay_us(150);
        }
        do {
            ETS_PRINTF("\n\n");
            // Because SDK v3.0.x always has a non-32-bit wide exception handler
            // installed, we can use PROGMEM strings with Boot ROM print functions.
#if defined(DEBUG_ESP_CORE) || defined(DEBUG_ESP_PORT) // DEBUG_ESP_CORE => verbose
            #if FLASH_MAP_SUPPORT
            if (flash_map_str) {
                ETS_PRINTF(flash_map_str);
                #if defined(DEBUG_ESP_CORE)
                size_t num = __flashindex; // On failure __flashindex is the size of __flashdesc[]; :/
                ETS_PRINTF(PSTR("Table of __flashdesc[%u].flash_size_kb entries converted to bytes:\n"), num);
                for (size_t i = 0; i < num; i++) {
                    uint32_t size = __flashdesc[i].flash_size_kb << 10;
                    ETS_PRINTF(PSTR("  [%02u] 0x%08X %8u\n"), i, size, size);
                }
                #endif
                ETS_PRINTF(PSTR("Reference info:\n"));
                uint32_t flash_chip_size = 1 << ((spi_flash_get_id() >> 16) & 0xff);
                ETS_PRINTF(PSTR("  %-24s 0x%08X %8u\n"), PSTR("fn(spi_flash_get_id())"), flash_chip_size, flash_chip_size);
                ETS_PRINTF(PSTR("  %-24s 0x%08X %8u\n"), PSTR("bin_chip_size"), flashchip->chip_size, flashchip->chip_size);
            } else
            #endif
            if (chip_sz_str) {
                ETS_PRINTF(chip_sz_str);
            } else
            if (table_regist_str) {
                ETS_PRINTF(table_regist_str);
                // (printing now works) repeat ...regist error messages
                system_partition_table_regist(_at_partition_table, _at_partition_table_sz, system_get_flash_size_map());
            }
            if (chip_sz_str || table_regist_str) {
                ETS_PRINTF(PSTR("Reference info:\n"));
                #if FLASH_MAP_SUPPORT
                ETS_PRINTF(PSTR("  %-24s 0x%08X %8u\n"), PSTR("fn(...ex].flash_size_kb)"), flash_size, flash_size);
                uint32_t flash_chip_size = 1 << ((spi_flash_get_id() >> 16) & 0xff);
                ETS_PRINTF(PSTR("  %-24s 0x%08X %8u\n"), PSTR("fn(spi_flash_get_id())"), flash_chip_size, flash_chip_size);
                ETS_PRINTF(PSTR("  %-24s 0x%08X %8u\n"), PSTR("bin_chip_size"), flashchip->chip_size, flashchip->chip_size);
                #else
                ETS_PRINTF(PSTR("  %-24s 0x%08X %8u\n"), PSTR("config_flash_size"), ld_config_chip_size, ld_config_chip_size);
                ETS_PRINTF(PSTR("  %-24s 0x%08X %8u\n"), PSTR("bin_chip_size"), flashchip->chip_size, flashchip->chip_size);
                #endif
                #if defined(DEBUG_ESP_CORE)
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("PHY_DATA"), phy_data);
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("RF_CAL"), rf_cal);
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("SYSTEM_PARAMETER"), system_parameter);
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("EEPROM_start"), EEPROM_start);
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("FS_start"), FS_start);
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("FS_end"), FS_end);
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("FS_page"), FS_page);
                ETS_PRINTF(PSTR("  %-24s 0x%08X\n"), PSTR("FS_block"), FS_block);
                #endif
            }
#else
            if (flash_map_str) {
                ETS_PRINTF(flash_map_str);
            } else
            if (chip_sz_str) {
                ETS_PRINTF(chip_sz_str);
            } else
            if (table_regist_str) {
                ETS_PRINTF(table_regist_str);
            }
#endif
            uart_rx_one_char_block(); // Someone said hello - repeat message
        } while(true);
    }
    /*
      The function user_rf_pre_init() is no longer called from SDK 3.0 and up.
      The SDK manual and release notes skipped this detail. The 2023 ESP-FAQ
      hints at the change with "* Call system_phy_set_powerup_option(3) in
      function user_pre_init or user_rf_pre_init"
      https://docs.espressif.com/_/downloads/espressif-esp-faq/en/latest/pdf/#page=14

      Add call to user_rf_pre_init(), so we can still perform early calls like
      system_phy_set_rfoption(rf_mode), system_phy_set_powerup_option(2), etc.

      Placement, should this be at the beginning or end of user_pre_init()?
      By the end, we have registered the PHY_DATA partition; however, PHY_DATA
      read occurs after return and before user_init() is called.
    */
    user_rf_pre_init();
}
#endif // #if (NONOSDK >= (0x30000))

extern "C" void user_init(void) {

    struct rst_info *rtc_info_ptr = system_get_rst_info();
    memcpy((void *) &resetInfo, (void *) rtc_info_ptr, sizeof(resetInfo));

    uart_div_modify(0, UART_CLK_FREQ / (115200));

#if FLASH_MAP_SUPPORT && (NONOSDK < (0x30000))
    const char *err_msg = flashinit();
    if (err_msg) __panic_func(err_msg, 0, NULL);
#endif

    init(); // in core_esp8266_wiring.c, inits hw regs and sdk timer

    initVariant();

    experimental::initFlashQuirks(); // Chip specific flash init.

    cont_init(g_pcont);

#if defined(DEBUG_ESP_HWDT) || defined(DEBUG_ESP_HWDT_NOEXTRA4K)
    debug_hwdt_init();
#endif

#if defined(UMM_HEAP_EXTERNAL)
    install_vm_exception_handler();
#endif

#if defined(NON32XFER_HANDLER) || (defined(MMU_IRAM_HEAP) && (NONOSDK < (0x30000)))
    install_non32xfer_exception_handler();
#endif

#if defined(MMU_IRAM_HEAP)
    umm_init_iram();
#endif
    preinit(); // Prior to C++ Dynamic Init (not related to above init() ). Meant to be user redefinable.
    __disableWiFiAtBootTime(); // default weak function disables WiFi

    ets_task(loop_task,
        LOOP_TASK_PRIORITY, s_loop_queue,
        LOOP_QUEUE_SIZE);

    system_init_done_cb(&init_done);
}
