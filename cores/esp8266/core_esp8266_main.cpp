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

#define LOOP_TASK_PRIORITY 1
#define LOOP_QUEUE_SIZE    1
#define OPTIMISTIC_YIELD_TIME_US 16000

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
static uint32_t s_micros_at_task_start;

/* For ets_intr_lock_nest / ets_intr_unlock_nest
 * Max nesting seen by SDK so far is 2.
 */
#define ETS_INTR_LOCK_NEST_MAX 7
static uint16_t ets_intr_lock_stack[ETS_INTR_LOCK_NEST_MAX];
static byte     ets_intr_lock_stack_ptr=0;


extern "C" {
extern const uint32_t __attribute__((section(".ver_number"))) core_version = ARDUINO_ESP8266_GIT_VER;
const char* core_release =
#ifdef ARDUINO_ESP8266_RELEASE
    ARDUINO_ESP8266_RELEASE;
#else
    NULL;
#endif
} // extern "C"

void initVariant() __attribute__((weak));
void initVariant() {
}

void preloop_update_frequency() __attribute__((weak));
void preloop_update_frequency() {
#if defined(F_CPU) && (F_CPU == 160000000L)
    REG_SET_BIT(0x3ff00014, BIT(0));
    ets_update_cpu_frequency(160);
#endif
}

extern "C" bool can_yield() {
  return cont_can_yield(g_pcont);
}

static inline void esp_yield_within_cont() __attribute__((always_inline));
static void esp_yield_within_cont() {
        cont_yield(g_pcont);
        run_scheduled_recurrent_functions();
}

extern "C" void __esp_yield() {
    if (can_yield()) {
        esp_yield_within_cont();
    }
}

extern "C" void esp_yield() __attribute__ ((weak, alias("__esp_yield")));

extern "C" IRAM_ATTR void esp_schedule() {
    ets_post(LOOP_TASK_PRIORITY, 0, 0);
}

extern "C" void __yield() {
    if (can_yield()) {
        esp_schedule();
        esp_yield_within_cont();
    }
    else {
        panic();
    }
}

extern "C" void yield(void) __attribute__ ((weak, alias("__yield")));

extern "C" void optimistic_yield(uint32_t interval_us) {
    if (can_yield() &&
        (system_get_time() - s_micros_at_task_start) > interval_us)
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
  asm volatile ("rsr %0,ps":"=a" (saved));
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
    esp_schedule();
}

static void loop_task(os_event_t *events) {
    (void) events;
    s_micros_at_task_start = system_get_time();
    cont_run(g_pcont, &loop_wrapper);
    if (cont_check(g_pcont) != 0) {
        panic();
    }
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

   WPS beeing flawed by its poor security, or not beeing used by lots of
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
#if defined(DEBUG_ERASE_CONFIG)
extern "C" void fix_divider(void);
#define FIX_DIVIDER fix_divider
#define ETS_PRINTF(...) ets_uart_printf(__VA_ARGS__)
#define ETS_DELAY_US(a) ets_delay_us(a)
#else
#define FIX_DIVIDER() do {} while(0)
#define ETS_PRINTF(...) do {} while(0)
#define ETS_DELAY_US(a) do {} while(0)
#endif


extern "C" void app_entry_redefinable(void) __attribute__((weak));
extern "C" void app_entry_redefinable(void)
{
    /* Allocate continuation context on this SYS stack,
       and save pointer to it. */
    cont_t s_cont __attribute__((aligned(16)));
    g_pcont = &s_cont;

    FIX_DIVIDER();
    ETS_PRINTF("\n\ncall_user_start()\n");
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
    /* do nothing by default */
}

extern "C" void user_init(void) {
    struct rst_info *rtc_info_ptr = system_get_rst_info();
    memcpy((void *) &resetInfo, (void *) rtc_info_ptr, sizeof(resetInfo));

#if defined(DEBUG_ERASE_CONFIG)
    uart_div_modify(0, UART_CLK_FREQ / (74880));
    ETS_DELAY_US(150);
    ETS_PRINTF("\nuser_init()\n");
#else
    uart_div_modify(0, UART_CLK_FREQ / (115200));
#endif

    init(); // in core_esp8266_wiring.c, inits hw regs and sdk timer

    initVariant();

    cont_init(g_pcont);

    preinit(); // Prior to C++ Dynamic Init (not related to above init() ). Meant to be user redefinable.

    ets_task(loop_task,
        LOOP_TASK_PRIORITY, s_loop_queue,
        LOOP_QUEUE_SIZE);

    system_init_done_cb(&init_done);
}
