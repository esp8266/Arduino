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
    uint32_t expired = millis() - start_ms;
    if (expired >= timeout_ms) {
        return true;
    }
    esp_delay(std::min((timeout_ms - expired), intvl_ms));
    return false;
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
    cont_check(g_pcont);
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

extern "C" void ICACHE_FLASH_ATTR user_pre_init(void)
{
    uint32_t rf_cal = 0;
    uint32_t phy_data = 0;
    uint32_t system_parameter = 0;

    switch (system_get_flash_size_map())
    {
    case FLASH_SIZE_2M:
        rf_cal = 0x3b000;
        phy_data = 0x3c000;
        system_parameter = 0x3d000;
        break;
    case FLASH_SIZE_4M_MAP_256_256:
        rf_cal = 0x7b000;
        phy_data = 0x7c000;
        system_parameter = 0x7d000;
        break;
    case FLASH_SIZE_8M_MAP_512_512:
        rf_cal = 0xfb000;
        phy_data = 0xfc000;
        system_parameter = 0xfd000;
        break;
    case FLASH_SIZE_16M_MAP_512_512:
    case FLASH_SIZE_16M_MAP_1024_1024:
        rf_cal = 0x1fb000;
        phy_data = 0x1fc000;
        system_parameter = 0x1fd000;
        break;
    case FLASH_SIZE_32M_MAP_512_512:
    case FLASH_SIZE_32M_MAP_1024_1024:
    case FLASH_SIZE_32M_MAP_2048_2048:
        rf_cal = 0x3fb000;
        phy_data = 0x3fc000;
        system_parameter = 0x3fd000;
        break;
    case FLASH_SIZE_64M_MAP_1024_1024:
        rf_cal = 0x7fb000;
        phy_data = 0x7fc000;
        system_parameter = 0x7fd000;
        break;
    case FLASH_SIZE_128M_MAP_1024_1024:
        rf_cal = 0xffb000;
        phy_data = 0xffc000;
        system_parameter = 0xffd000;
        break;
    }

    extern uint32_t user_rf_cal_sector_set(void);
    user_rf_cal_sector_set();

    const partition_item_t at_partition_table[] =
    {
        { SYSTEM_PARTITION_RF_CAL,           rf_cal,           0x1000 },
        { SYSTEM_PARTITION_PHY_DATA,         phy_data,         0x1000 },
        { SYSTEM_PARTITION_SYSTEM_PARAMETER, system_parameter, 0x3000 },
    };
    system_partition_table_regist(at_partition_table, sizeof(at_partition_table) / sizeof(at_partition_table[0]), system_get_flash_size_map());
}

#endif

extern "C" void user_init(void) {

#if (NONOSDK >= (0x30000))
    extern void user_rf_pre_init();
    user_rf_pre_init();
#endif

    struct rst_info *rtc_info_ptr = system_get_rst_info();
    memcpy((void *) &resetInfo, (void *) rtc_info_ptr, sizeof(resetInfo));

    uart_div_modify(0, UART_CLK_FREQ / (115200));

#if FLASH_MAP_SUPPORT
    const char *err_msg = flashinit();
    if (err_msg) __unhandled_exception(err_msg);
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

#if defined(NON32XFER_HANDLER) || defined(MMU_IRAM_HEAP)
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
