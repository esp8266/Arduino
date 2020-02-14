/*
 *   Copyright 2020 Michael Hightower
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

/*
 * As far as I know, there is no way to get called for a Hardware WDT. I assume
 * it generates a form of reset that occurs at a low level that cannot be
 * trapped. Debugging an HWDT can be quite challenging.
 *
 * This module writes a stack dump to the serial port after a Hardware Watchdog
 * Timer has struck, and a new boot cycle has begun. By making adjustments to the
 * stack, we can avoid crash stack data being overwritten by this tool,
 * the Boot ROM, and the bootloader.
 *
 * We are using the method defined for `core_esp8266_app_entry_noextra4k.cpp` to
 * load an alternate `app_entry_redefinable()`. For details on this method, see
 * comments in `core_esp8266_main.cpp's app_entry()`.
 *
 * Using this tool's alternate `app_entry_redefinable()`, we can gain control
 * before the SDK is started. And dump the contents of the "sys" and "cont"
 * stacks.
 *
 * By making some adjustments to start of the stack pointer, at the entry to
 * `app_entry_redefinable()`, and also to the stack pointer passed to the SDK,
 * we can preserve the stack during an HWDT event.
 *
 * To use, just place this file in the sketch directory before opening the
 * project and build and upload your sketch. That should be enough to get it
 * included in your sketch. If that does not work, then add a call to:
 *    `void enable_debug_hwdt_at_link_time (void);`
 * in `setup()`. This tool prints to the serial port at the default serial port
 * speed set by the Boot ROM. On a Hardware WDT reset that port speed is
 * 115200 bps. If your needs differ, see the HWDT_UART_SPEED option below.
 *
 * More on crystal differences and data rates:
 *     When the ESP8266 restarts because of a Hardware WDT reset, the port speed
 *     defaults to 115200. This will be the speed, even if you have a 26MHz or
 *     40MHz Crystal. If you would like to use a different data rate, use the
 *     option HWDT_UART_SPEED described below.
 *
 *     The Boot ROM initially sets the UART clock divisor to support a data rate
 *     of 115200 bps with the assumption that it has a 40MHz crystal. When a
 *     26MHz crystal is used instead, the resulting error gives us a real rate
 *     of 74880 bps and printing is garbled at first, until the CPU clock's PLL
 *     is adjusted by the NONOS SDK. While CH_PD and EST_RST bring the CPU back
 *     to this state of underclocking with a 26MHz crystal, the Hardware WDT
 *     reset does not appear to do the same. The UART continues to be clocked at
 *     a rate to support a device at 115200 bps. Thus, allowing us to see the
 *     old cryptic WDT message along with our new stack dump.
 *
 *
 * When you get a stack dump, copy-paste it into the "ESP Exception Decoder".
 * Since we don't have an SP, we see a lot more stuff in the report, compared to
 * what you would see with a postmortem report. Start at the bottom and work
 * your way up. At this time, I have not had a lot of practice using this tool.
 * TODO: Update description with more details when available.
 *
 *
 *
 * Possible Issues/Thoughts/Improvements:
 *
 * On reboot after an OTA download, eboot has a large demand for stack and DRAM
 * space. For routine loads from flash, its stack and DRAM usage is light and
 * should leave us a good stack to dump for a HWDT.
 *
 * If a problem should arise with some data elements being corrupted during
 * reboot, would it be possible to move their DRAM location higher in memory?
 *
 * Also, DRAM being valid across reset events other than power-on and deep
 * sleep, suggest that a variable in the .noinit section could be used instead
 * of the more limited RTC Memory for sketches that don't do deep sleep.
 * However, DRAM should be considered invalid after an upload serial or OTA.
 * These operations use a lot of DRAM.
 *
 * With this module active, postmortem stack dumps will be a little longer than
 * they need to be. The sys stack now ends at 0x3FFFFC00 instead of 0x3FFFFB0.
 *
 * Uh, there is a third stack ?? ctx: bearssl - I have not coded for this.
 * I'll leave this for later, as a possible future enhancement.
 * There are three possible stacks to hang in!?
 * Notes for later:
 *   * Stack is allocted with malloc - must be recovered before SDK init
 *     (umm_init() called on 1ST heap free call early in SDK init)
 *   * Function calls for addresses are pageable. Need to access pointers
 *     directly.
 *   * stackpaint is 0xdeadbeef
 *   * stack_thunk_save has reg a1.
 *   * stack_thunk_ptr malloc-ed address
 *   * use stack_thunk_refcnt to determine if present
 *   * malloc addresses are aligned 8 not 16
 *   * stack_thunk_top beginning of stack. Defined as stack_thunk_ptr + size -1
 *
 * Maybe an in/out ref count would be nice for bearssl and cont stacks.
 */

/*____________________________________________________________________________*/
/*                                                                            */
/*                          Configuration Options                             */
/*____________________________________________________________________________*/


/*
 * DEBUG_HWDT
 *
 * Enables this debug tool for printing a Hardware WDT stack dump at restart.
 *
 */
 #define DEBUG_HWDT


/*
 * DEBUG_HWDT_NO4KEXTRA
 *
 * This option will leave more of the system stack available for the stack dump.
 * A problem we have with the "4K extra" option, is it pushes the system stack
 * up into the ROM's BSS area which gets zeroed at reboot by the Boot ROM.
 *
 * Using this option has the effect of taking 4K of DRAM away from the heap,
 * which gets used for the "cont" stack. Leaving an extra 4K on the "sys" stack,
 * that is clear of the ROM's BSS area. This allows for a more complete "sys"
 * stack dump. The choice here can depend on where you are crashing.
 *
 * Because we don't know where the crash occurs, this option prints two stack
 * dumps, one for "cont" (user stack) and one for "sys" (NONOS SDK).
 *
 * In contrast, if the hang is happening on the "cont" stack, you don't need a
 * complete stack dump of the "sys" stack. You can omit this define and have an
 * extra 4K in the heap.
 *
 */
#define DEBUG_HWDT_NO4KEXTRA


/*
 * HWDT_UART_SPEED
 *
 * This option alters the UART serial speed used for printing the Hardware WDT
 * reset stack dump. Without this option on an HWDT reset, the existing default
 * speed of 115200 bps will be used. If you are using this default speed, you
 * can skip this option and save on the IRAM space. Note this option only
 * changes the speed while this module is printing.
 *
 * For more confusion on the serial port speed, see "More on crystal differences
 * and data rates" in the comments at the top.
 *
 */
 // #define HWDT_UART_SPEED (19200)
 // #define HWDT_UART_SPEED (74880)
 // #define HWDT_UART_SPEED (115200)
 // #define HWDT_UART_SPEED (230400)


/*
 * HWDT_PRINT_GREETING
 *
 * Prints a simple introduction to let you know this tool is active and in the
 * build. At power-on, this may not be viewable on some devices. The crystal
 * has to be 40Mhz for this to work w/o using the HWDT_UART_SPEED option above.
 * May not be worth the cost in IRAM.
 *
 * EDIT: There is something different in the UART setup after a flash upload. I
 * am unable to print using the same code that works for Power-on and an EXT_RST
 * at any other time. After the SDK has run a 2nd EXT_RST will show the greeting
 * message.
 *
 * EDIT2: Seems to work better now. Placed delays around calls to
 * uart_div_modify(). Leave these comments until I have more experience with
 * this change.
 *
 */
 // #define HWDT_PRINT_GREETING


/*
 * ROM_STACK_SIZE
 *
 * There are four sections of code starting just before 0x40000000, that share
 * the same stack space.
 *   1) The Boot ROM (uses around 640 bytes)
 *   2) The Bootloader, eboot.elf (uses around 720 bytes.)
 *   3) `app_entry_redefinable()` just before it starts the SDK.
 *   4) The NONOS SDK and optionally the Core when the extra 4K option is selected.
 *
 * To preserve the sketch stack data for a stack dump, I define three separate
 * stacks:
 *   1) Boot ROM and eboot
 *   2) this stack dump code
 *   3) SDK, Core, and Sketch
 *
 * With this, we can recover a complete stack trace of our failed sketch. To be
 * safe, I am leaving this at 1024; however, I think there is room to lower it
 * without loss of information.
 */
#ifndef ROM_STACK_SIZE
#define ROM_STACK_SIZE (1024)
#endif


/*
 * HWDT_INFO
 *
 * Gather some information on ROM and bootloader combined, sys, and cont stack
 * usage. If you are missing the include file for this structure, you can
 * copy-paste from the embedded version of the .h below.
 *
 */
 #define HWDT_INFO


/*
 * ROM_STACK_DUMP
 *
 * Dump the stack contents of the ROM Stack area. This gives us a visual of the
 * stack usage. Probably not of value, beyond developing this tool.
 *
 * To see printing, you may need to use this option with HWDT_UART_SPEED.
 */
 // #define ROM_STACK_DUMP


/*
 * HWDT_IF_METHOD_RESET_REASON
 *
 * "If" statement or "switch" method to implement, the reset reason logic. Both
 * can be made smaller by removing confirmation checks.
 *
 * Checks are performed when DEBUG_HWDT_DEBUG_RESET_REASON has been defined.
 *
 * EDIT: I should settle on one or the other; however, new issues continue to
 * pop up on determining reset reason. I'll wait until later and pick one.
 *
 #define DEBUG_HWDT_DEBUG_RESET_REASON
 */
 #define HWDT_IF_METHOD_RESET_REASON

/*____________________________________________________________________________*/
/*                                                                            */
/*                     End of Configuration Options                           */
/*____________________________________________________________________________*/


#ifdef DEBUG_HWDT

#include <c_types.h>
#include "cont.h"
#include "coredecls.h"
#include <core_esp8266_features.h>
#include <esp8266_undocumented.h>
#include <esp8266_peri.h>
#include <uart.h>
#include <pgmspace.h>

extern "C" {
#include <user_interface.h>
extern void call_user_start();
extern uint32_t rtc_get_reset_reason(void);
}

// #define DEBUG_HWDT_DEBUG
// #define USE_IRAM

#ifdef USE_IRAM
#undef USE_IRAM
#define USE_IRAM 1
#define IRAM_MAYBE ICACHE_RAM_ATTR

#else
#undef USE_IRAM
#define IRAM_MAYBE
#endif

#define STATIC static __attribute__((noinline))
// #define STATIC

#ifdef DEBUG_HWDT_DEBUG
/*
 * We have two copies of HWDT_INFO_t. Verify internal and external structures
 * match.
 *
 * This duplication is done so that in most cases, a simple/quick add one file
 * to a sketch folder is enough to debug.
 *
 * Only if additional internal information is needed, would this include be
 * added. Since we have two copies, a static_assert is used to verify that at
 * least the the size of the two structures are the same.
 */
#include "hwdt_app_entry.h"
#endif

/*
 * Verify that the internal and external structure definitions match.
 */
#ifdef HWDT_STACK_DUMP_H
#define HWDT_INFO_t LOCAL_HWDT_INFO_t
#define hwdt_info LOCAL_hwdt_info
#define HWDT_VERIFY_HWDT_INFO
#endif

/*
 * If you are using the HWDT_INFO_t structure, and are missing the include file.
 * Copy-paste the include block below into its respective filename.
 */

/*____________________________________________________________________________*/
/*                                                                            */
/*        Start of copy-paste block to create "hwdt_app_entry.h"              */
/*____________________________________________________________________________*/
#if !defined(HWDT_STACK_DUMP_H) || defined(HWDT_VERIFY_HWDT_INFO)
#define HWDT_STACK_DUMP_H

typedef struct HWDT_INFO {
    uint32_t rom;
    uint32_t sys;
    uint32_t cont;
    uint32_t rom_api_reason;
    uint32_t rtc_sys_reason;
    uint32_t reset_reason;
    uint32_t cont_integrity;
    bool g_pcont_valid;
} HWDT_INFO_t;

void enable_debug_hwdt_at_link_time(void);

extern uint32_t *g_rom_stack;
extern HWDT_INFO_t hwdt_info;

#endif
/*____________________________________________________________________________*/
/*                                                                            */
/*        End of copy-paste block for creating "hwdt_app_entry.h"             */
/*____________________________________________________________________________*/


#ifdef HWDT_VERIFY_HWDT_INFO
#undef HWDT_INFO_t
#undef hwdt_info
#undef HWDT_VERIFY_HWDT_INFO
static_assert(sizeof(HWDT_INFO_t) == sizeof(LOCAL_HWDT_INFO_t), "Local and include verison of HWDT_INFO_t do not match.");
#endif


#define MK_ALIGN16_SZ(a) (((a) + 0x0F) & ~0x0F)
#define ALIGN_UP(a, s) ((decltype(a))((((uintptr_t)(a)) + (s-1)) & ~(s-1)))
#define ALIGN_DOWN(a, s) ((decltype(a))(((uintptr_t)(a)) & ~(s-1)))

#ifndef CONT_STACKGUARD
#define CONT_STACKGUARD 0xfeefeffe
#endif
constexpr volatile uint32_t *RTC_SYS = (volatile uint32_t*)0x60001100;


constexpr uint32_t *dram_start      = (uint32_t *)0x3FFE8000;
constexpr uint32_t *dram_end        = (uint32_t *)0x40000000;

constexpr uint32_t *rom_stack_first = (uint32_t *)0x40000000;
constexpr uint32_t *sys_stack       = (uint32_t *)0x3fffeb30;
/*
 * The space between 0x3fffe000 up to 0x3fffeb30 is a ROM BSS area that is later
 * claimed by the SDK for stack space. This is a problem area for this tool,
 * because the ROM BSS gets zeroed as part of ROM init on reboot. Any part of
 * the "sys" stack residing there is lost. On the other hand, it becomes a prime
 * candidate for DRAM address space to handle the needs of this tool.
 */
constexpr uint32_t *sys_stack_e000  = (uint32_t *)0x3fffe000;

// Map out who will live where.
constexpr size_t rom_stack_A16_sz = MK_ALIGN16_SZ(ROM_STACK_SIZE);
constexpr size_t cont_stack_A16_sz = MK_ALIGN16_SZ(sizeof(cont_t));
constexpr uint32_t *rom_stack = (uint32_t *)((uintptr_t)rom_stack_first - rom_stack_A16_sz);


#ifdef DEBUG_HWDT_NO4KEXTRA
/* This is the default NONOS-SDK user's heap location for NO4KEXTRA */
static cont_t g_cont __attribute__ ((aligned (16)));
constexpr uint32_t *sys_stack_first = (uint32_t *)((uintptr_t)rom_stack);

#else
constexpr uint32_t *cont_stack_first = (uint32_t *)((uintptr_t)rom_stack); // only for computation
constexpr cont_t *cont_stack = (cont_t *)((uintptr_t)cont_stack_first - cont_stack_A16_sz);
constexpr uint32_t *sys_stack_first = (uint32_t *)((uintptr_t)cont_stack);
#endif

uint32_t *g_rom_stack  __attribute__((section(".noinit")));
size_t g_rom_stack_A16_sz  __attribute__((section(".noinit")));
HWDT_INFO_t hwdt_info __attribute__((section(".noinit")));

void enable_debug_hwdt_at_link_time(void) {
    /*
     * This function does nothing; however, including a call to it in setup,
     * allows this module to override, at link time, the core_esp8266_main.cpp's
     * `app_entry()` with the one below. This will create a stack dump on
     * Hardware WDT resets.
     *
     * It appears just including this module in the sketch directory before
     * opening the project is enough. However, just play it safe, call this
     * function from setup.
     */
}


/* the following code is linked only if a call to the above function is made somewhere */

extern "C" {

#if USE_IRAM
#define ETS_PRINTF ets_uart_printf

#else
/*
 This function is already in umm_malloc for some debug options.
 Define here in case they are not enabled.
*/
int ICACHE_FLASH_ATTR umm_info_safe_printf_P(const char *fmt, ...) __attribute__((weak));
int ICACHE_FLASH_ATTR umm_info_safe_printf_P(const char *fmt, ...) {
    /*
      To use ets_strlen() and ets_strcpy() safely with PROGMEM, flash storage,
      the PROGMEM address must be word (4 bytes) aligned. The destination
      address for ets_memcpy must also be word-aligned.
    */
    char ram_buf[ets_strlen(fmt)] __attribute__ ((aligned(4)));
    ets_strcpy(ram_buf, fmt);
    va_list argPtr;
    va_start(argPtr, fmt);
    int result = ets_vprintf(ets_uart_putc1, ram_buf, argPtr);
    va_end(argPtr);
    return result;
}

#define ETS_PRINTF(fmt, ...) umm_info_safe_printf_P(PSTR(fmt), ##__VA_ARGS__)
#endif

#define ETS_FLUSH(uart_no) while((USS(uart_no) >> USTXC) & 0xff) {}

enum PRINT_STACK {
    CONT = 1,
    SYS = 2,
    ROM = 4
};


STATIC void IRAM_MAYBE print_stack(const uintptr_t start, const uintptr_t end, const uint32_t chunk) {
    ETS_PRINTF("\n\n>>>stack>>>\n\nctx: ");

    if (chunk & PRINT_STACK::CONT) {
        ETS_PRINTF("cont");
    } else
    if (chunk & PRINT_STACK::SYS) {
        ETS_PRINTF("sys");
    } else
    if (chunk & PRINT_STACK::ROM) {
        ETS_PRINTF("ROM");
    }

    ETS_PRINTF("\nsp: %08x end: %08x offset: %04x\n", start, end, 0);

    const size_t this_mutch = end - start;
    if (this_mutch >= 0x10) {
        for (size_t pos = 0; pos < this_mutch; pos += 0x10) {
            const uint32_t *value = (uint32_t *)(start + pos);

            /* rough indicator: stack frames usually have SP saved as the second word */
            bool looksLikeStackFrame = (value[2] == (start + pos + 0x10));
            ETS_PRINTF("%08x:  %08x %08x %08x %08x %c\n", (uint32_t)&value[0],
                       value[0], value[1], value[2], value[3],
                       (looksLikeStackFrame)?'<':' ');
        }
    }

    ETS_PRINTF("<<<stack<<<\n");
}


STATIC const uint32_t * IRAM_MAYBE skip_stackguard(const uint32_t *start, const uint32_t *end, const uint32_t pattern) {
    // Find the end of SYS stack activity
    const uint32_t *uptr = start;

    size_t this_mutch = (uintptr_t)end - (uintptr_t)start;
    this_mutch /= sizeof(uint32_t);
    size_t i = 0;
    for (; i < this_mutch; i++) {
        if (pattern != uptr[i]) {
            i &= ~3U;
            uptr = &uptr[i];
            break;
        }
    }
    if (i == this_mutch) {
        uptr = &uptr[i];
    }

    return uptr;
}

STATIC void IRAM_MAYBE check_g_pcont_validity(void) {
    /*
     * DRAM appears to remain valid after most resets. There is more on this in
     * handle_hwdt().
     *
     * Testing of vital pointers for validity could also aid as a partial
     * indicator of power-on. Not needed for that purpose at this time.
     */
    if (g_rom_stack == rom_stack &&
        g_rom_stack_A16_sz == rom_stack_A16_sz &&
#ifdef DEBUG_HWDT_NO4KEXTRA
        g_pcont == &g_cont
#else
        g_pcont == cont_stack
#endif
        ) {
            hwdt_info.g_pcont_valid = true;
    } else {
        hwdt_info.g_pcont_valid = false;
        g_rom_stack = rom_stack;
        g_rom_stack_A16_sz = rom_stack_A16_sz;
    }
}

#if defined(DEBUG_HWDT_DEBUG) || defined(DEBUG_HWDT_DEBUG_RESET_REASON)
#ifndef DEBUG_HWDT_DEBUG_RESET_REASON
#define DEBUG_HWDT_DEBUG_RESET_REASON
#endif
#define debug__confirm_rom_reason(a) ((a) == rom_api_reason)
#else
#define debug__confirm_rom_reason(a) (true)
#endif

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

STATIC uint32_t IRAM_MAYBE get_reset_reason(bool* power_on, bool* hwdt_reset) {
    /*
     * Detecting a Hardware WDT (HWDT) reset is a little complicated at boot
     * before the SDK is started.
     *
     * While the ROM API will report an HWDT, it does not change the status
     * after a software restart. And the SDK has not been started so its API is
     * not available.
     *
     * There is a value in System RTC memory that appears to store, at restart,
     * the reset reason for the SDK. I assume it is set by SDK before the
     * restart. Of course, this value is invalid at power on before the SDK
     * runs. It is set to 0 by the SDK after init and later is changed to a 1
     * and remains 1 during the operation of the sketch. So during normal
     * execution the reason is preset to indicate a Hardware WDT reset.
     *
     * These cases represent some examples. The list is not meant to be
     * comprehensive.
     *
     * Case 1: At power-on boot the ROM API result is valid; however, the SDK
     * value in RTC Memory has not been set at this time.
     *
     * Case 2: A HWDT reset has occurred, which is later followed with a
     * restart by the SDK. At boot, the ROM API result still reports the HWDT
     * reason.
     *
     * Case 3: It is pssible to see a value of 0 or 1 (power-on or HWDT) for the
     * SDK reset reason for both EXT_RST and Hardware WDT resets. For either of
     * these values, the ROM API does hold the valid reset reason.
     *
     * I need to know if this is the 1st boot at power on. Combining these
     * indicators has been tricky, I think I now have it.
     *
     */

    hwdt_info.rtc_sys_reason = RTC_SYS[0];
    const uint32_t rtc_sys_reason = hwdt_info.rtc_sys_reason;
    hwdt_info.rom_api_reason = rtc_get_reset_reason();
    const ROM_RST_REASON_t rom_api_reason = (ROM_RST_REASON_t)hwdt_info.rom_api_reason;

#ifdef HWDT_IF_METHOD_RESET_REASON
    *hwdt_reset = false;
    *power_on = false;
    /*
     * This logic takes the reason left in memory by the SDK as an initial
     * estimate and expands on it.
     */
    hwdt_info.reset_reason = rtc_sys_reason;
    if (REASON_DEFAULT_RST == rtc_sys_reason ||
        REASON_WDT_RST     == rtc_sys_reason) {
        /*
         * 1) The 0 value (REASON_DEFAULT_RST) shows up with multiple EXT_RSTs
         *    quickly. The 1 value (REASON_WDT_RST), previous if, shows up if
         *    you wait a while before the EXT_RST.
         * 2) The 0 value also shows up if a HWDT reset occurs too quickly after
         *    the system starts. Note even the SDK gets this one right.
         */
        if (OWDT_RESET == rom_api_reason) {
             *hwdt_reset = true;
             hwdt_info.reset_reason = REASON_WDT_RST;
        } else {
             hwdt_info.reset_reason = REASON_EXT_SYS_RST;
             if (!debug__confirm_rom_reason(EXT_RESET)) {
                hwdt_info.reset_reason = ~0;
             }
        }

    } else if (REASON_EXT_SYS_RST < rtc_sys_reason) {
        /*
         * We only want to indicate power-on, if the ROM API reason confirms it.
         * A reliable power-on indicator is need for set_uart_speed() to work
         * properly.
         */
        *power_on = true;
        hwdt_info.reset_reason = REASON_DEFAULT_RST;
        if (!debug__confirm_rom_reason(POWERON_RESET)) {
            hwdt_info.reset_reason = ~0;
            *power_on = false;
        }
    } else {
        /*
         * REASON_EXT_SYS_RST is not expected at reboot, let it fall through to
         * for confirmation in debug option.
         */
        if (REASON_EXT_SYS_RST == rtc_sys_reason) {
            if (!debug__confirm_rom_reason(EXT_RESET)) {
                hwdt_info.reset_reason = ~0;
            }
        }
    }
#else
    // New reset reason logic test
    *hwdt_reset = false;
    *power_on = false;
    switch(rtc_sys_reason) {
        case REASON_DEFAULT_RST:
          /*
           * This can be present for REASON_EXT_SYS_RST or REASON_WDT_RST
           * The rtc_sys_reason starts off at 0 and is set to 1 later,
           * if crash occurs before set, then it is still 0.
           */
        case REASON_WDT_RST:
          /*
           * This may be present for REASON_EXT_SYS_RST or REASON_WDT_RST,
           * use rom_api_reason to confirm.
           */
          if (OWDT_RESET == rom_api_reason) {
              hwdt_info.reset_reason = REASON_WDT_RST;
              *hwdt_reset = true;
          } else {
              hwdt_info.reset_reason = REASON_EXT_SYS_RST;
              if (!debug__confirm_rom_reason(EXT_RESET)) {
                  hwdt_info.reset_reason = ~0;
              }
          }
          break;
        /* These should be correct as is */
        case REASON_EXCEPTION_RST:
        case REASON_SOFT_WDT_RST:
        case REASON_SOFT_RESTART:
        case REASON_DEEP_SLEEP_AWAKE:
            hwdt_info.reset_reason = rtc_sys_reason;
            break;
        /*
         * REASON_EXT_SYS_RST is not expected at reboot, let it fall through to
         * default for confirmation.
         */
        case REASON_EXT_SYS_RST:
        default:
            /*
             * Out of range value, this could be a REASON_DEFAULT_RST,
             * use rom_api_reason to confirm.
             */
            if (POWERON_RESET == rom_api_reason) {
                hwdt_info.reset_reason = REASON_DEFAULT_RST;
                *power_on = true;
            } else {
                hwdt_info.reset_reason = REASON_EXT_SYS_RST;
                if (!debug__confirm_rom_reason(EXT_RESET)) {
                    hwdt_info.reset_reason = ~0;
                }
            }
            break;
    }
#endif
    return hwdt_info.reset_reason;
}

#ifdef HWDT_UART_SPEED
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

#define UART_CLKDIV_MASK 0x000FFFFFUL

void adjust_uart_speed(uint32_t uart_divisor) {
    /*
     * Take care to let serial data in the FIFO finish before changing UART
     * speed. Then after changing speeds, let the uart clock generator
     * stablilize, and let the remote receiver come to an idle state before
     * continuing.
     *
     * Load a Rubout character for the final charcter shifting out to stop
     * the last charcter from getting crunched during the speed change.
     *
     * The thinking is if the speed changes while shifting out, as long as the
     * start bit gets out before the change. The change will not be noticed
     * since all ones will look like the serial line idle state. And for text
     * displaying, received rubout characters should be discarded. At least that
     * was true 40 years ago.
     *
     * These adjustments appear to resolve the lost data problem that occurs
     * when printing after a flash upload using esptool.
     */
    ets_putc('\xFF');
    ETS_FLUSH(0);
    real_uart_div_modify(0, uart_divisor);
    ets_delay_us(150);
}

STATIC uint32_t IRAM_MAYBE set_uart_speed(const uint32_t uart_no, const uint32_t new_speed) {

    uint32_t uart_divisor = USD(uart_no) & UART_CLKDIV_MASK;
    /*
     * No adjustments are needed on most reboots, etc.
     *
     * The UART clock is independent of the CPU Speed. (ie. 80MHz, 160MHz)
     * UART_CLK_FREQ is used in user_init, and ESP8266_CLOCK is used in
     * uart.h. Both are defined to be 80000000.
     */
    uint32_t master_freq = UART_CLK_FREQ;
    if (REASON_DEFAULT_RST       == hwdt_info.reset_reason ||
        REASON_EXT_SYS_RST       == hwdt_info.reset_reason ||
        REASON_DEEP_SLEEP_AWAKE  == hwdt_info.reset_reason) {
        /*
         * At this time, with power on boot or EXT_RST, the CPU Frequency
         * calibration has not happened. Thus for a 26MHz Xtal, the CPU clock is
         * running at 52MHz. Tweak UART speed here, so printing works. To avoid
         * confusion on exit, we later restore the divisor.
         */
#if (1 < F_CRYSTAL)
        constexpr uint32_t crystal_freq = F_CRYSTAL;
#else
        constexpr uint32_t crystal_freq = 26000000;
#endif
        master_freq = crystal_freq * 2;
    }

    uint32_t new_uart_divisor = master_freq / new_speed;

    if (UART_CLKDIV_MASK < new_uart_divisor ||
        2 > new_uart_divisor ||
        new_uart_divisor == uart_divisor) {
        uart_divisor = 0;

    } else {
        adjust_uart_speed(new_uart_divisor);
    }

#if defined(DEBUG_HWDT_DEBUG)
    ETS_PRINTF("\n\nreal_uart_div_modify(0, %u / %u);\n", master_freq, new_speed);
    ETS_PRINTF("F_CRYSTAL = %u\n", crystal_freq);
    ETS_PRINTF("old uart_divisor = %u\n", uart_divisor);
    ETS_PRINTF("new uart_divisor = %u\n", new_uart_divisor);
    ETS_PRINTF("master_freq = %u\n", master_freq);
#endif

    return uart_divisor;
}
#endif

/*
 *
 *
 *
 *
 */
STATIC void IRAM_MAYBE handle_hwdt(void) {

    ets_memset(&hwdt_info, 0, sizeof(hwdt_info));
    check_g_pcont_validity();

    bool power_on = false;
    bool hwdt_reset = false;
    get_reset_reason(&power_on, &hwdt_reset);

#ifdef HWDT_UART_SPEED
    const uint32_t uart_divisor = set_uart_speed(0, HWDT_UART_SPEED);
#endif
#if defined(DEBUG_HWDT_DEBUG)
    ETS_PRINTF("Basic boot reason: %s\n", (power_on) ? "Power-on" : "Reboot");
    ETS_PRINTF("RTC_SYS Reset Reason = %u\n", hwdt_info.rtc_sys_reason);
    ETS_PRINTF("ROM API Reset Reason = %u\n", hwdt_info.rom_api_reason);
    ETS_PRINTF("HWDT Reset Reason = %u\n\n", hwdt_info.reset_reason);
#endif
#if defined(DEBUG_HWDT_DEBUG_RESET_REASON)
    if (REASON_EXT_SYS_RST < hwdt_info.reset_reason) {
        ETS_PRINTF("Reset reason confirmation failed!\n");
        ETS_PRINTF("  RTC_SYS Reset Reason = %u\n", hwdt_info.rtc_sys_reason);
        ETS_PRINTF("  ROM API Reset Reason = %u\n", hwdt_info.rom_api_reason);
    }
#endif
    /*
     * With a few exceptions, DRAM data remains valid after a reset.
     *
     * Check for "cont" stack consistency.
     * The contents of DRAM are not expected to be valid after a:
     *   1) flash update (OTA or serial)
     *   2) power-on
     *   3) deep sleep
     * Additionally, g_pcont is expected to be invalid after these events.
     *
     * When g_pcont is valid, we expect these checks to be valid. I am not sure
     * what to do when they are not. An error that could lead to a crash is
     * corrected. We currently continue and print the stack dump. This assumes
     * something is better than nothing.
     */
    if (!power_on && hwdt_info.g_pcont_valid) {
        uint32_t cont_integrity = 0;
        if (g_pcont->stack_guard1 != CONT_STACKGUARD) {
          cont_integrity |= 0x0001;
        }
        if (g_pcont->stack_guard2 != CONT_STACKGUARD) {
          cont_integrity |= 0x0020;
        }
        if (g_pcont->stack_end != (g_pcont->stack + (sizeof(g_pcont->stack) / 4))) {
          cont_integrity |= 0x0300;
          // Fix ending so we don't crash
          g_pcont->stack_end = (g_pcont->stack + (sizeof(g_pcont->stack) / 4));
        }
        if (g_pcont->struct_start != (unsigned*) g_pcont) {
          cont_integrity |= 0x4000;
        }
        hwdt_info.cont_integrity = cont_integrity;

#if defined(DEBUG_HWDT_NO4KEXTRA) || defined(HWDT_INFO)
        const uint32_t *ctx_cont_ptr = skip_stackguard(g_pcont->stack, g_pcont->stack_end, CONT_STACKGUARD);
        hwdt_info.cont = (uintptr_t)g_pcont->stack_end - (uintptr_t)ctx_cont_ptr;
#endif

        const uint32_t *ctx_sys_ptr = skip_stackguard(sys_stack, rom_stack, CONT_STACKGUARD);
        hwdt_info.sys = (uintptr_t)rom_stack - (uintptr_t)ctx_sys_ptr;

        /* Print context SYS */
        if (hwdt_reset) {
            ETS_PRINTF("\n\nHardware WDT reset\n");
            print_stack((uintptr_t)ctx_sys_ptr, (uintptr_t)rom_stack, PRINT_STACK::SYS);

#ifdef DEBUG_HWDT_NO4KEXTRA
            /* Print separate ctx: cont stack */
            print_stack((uintptr_t)ctx_cont_ptr, (uintptr_t)g_pcont->stack_end, PRINT_STACK::CONT);
#endif
            if (hwdt_info.cont_integrity) {
                ETS_PRINTF("\nCaution, the stack is possibly corrupt integrity checks did not pass.\n\n");
            }
        }
    }

    /*
     *  Fill the SDK stack area with CONT_STACKGUARD so we can detect and
     *  skip the unused section of the stack when printing a Stack Dump.
     */
    {
        size_t this_mutch = (uintptr_t)rom_stack - (uintptr_t)sys_stack;
        this_mutch /= sizeof(uint32_t);
        for (size_t i = 0; i < this_mutch; i++) {
            sys_stack[i] = CONT_STACKGUARD;
        }
    }

#if defined(HWDT_INFO) || defined(ROM_STACK_DUMP)
    /*
     *  Reports on rom_stack usage by ROM and eboot.
     *  Used to confirm ROM_STACK_SIZE is large enough.
     */
    {
        const uint32_t *ctx_rom_ptr = skip_stackguard(rom_stack, rom_stack_first, CONT_STACKGUARD);
        hwdt_info.rom = (uintptr_t)rom_stack_first - (uintptr_t)ctx_rom_ptr;
#if defined(ROM_STACK_DUMP)
        print_stack((uintptr_t)ctx_rom_ptr, (uintptr_t)rom_stack_first, PRINT_STACK::ROM);
#endif
    }
#endif

#if defined(HWDT_PRINT_GREETING)
    ETS_PRINTF("\n\nHardware WDT Stack Dump - enabled\n\n");
#else
    ETS_PRINTF("\n\n");
#endif

#ifdef HWDT_UART_SPEED
    if (uart_divisor) {
        adjust_uart_speed(uart_divisor);
    }
#endif
}

/*
 * Using Cache_Read_Enable/Cache_Read_Disable to reduce IRAM usage. Moved
 * strings and most functions to flash. At this phase of the startup, "C++" has
 * not initialized. So, we needed a local "C" function to handle printing from
 * flash. For this, I grabbed a copy of umm_info_safe_printf_P.
 *
 * This reduced IRAM usage by ~1K and DRAM ~200 bytes.
 *
 * Inspiration for using Cache_Read_Enable came from reviewing rboot, zboot, and
 * https://richard.burtons.org/2015/06/12/esp8266-cache_read_enable/.
 * Additional insight can be gleemed from reviewing the ESP8266_RTOS_SDK.
 * (eg. ../components/bootloader_support/src/bootloader_utility.c)
 */
#define ICACHE_SIZE_32 1
#define ICACHE_SIZE_16 0

extern "C" void Cache_Read_Disable(void);
extern "C" void Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v);

void ICACHE_RAM_ATTR app_entry_start(void) {

    handle_hwdt();

#ifdef DEBUG_HWDT_NO4KEXTRA
    /*
     *  Continuation context is in BSS.
     */
    g_pcont = &g_cont;
#else
    /*
     *  The continuation context is on the stack just after the reserved space
     *  for the ROM/eboot stack and before the SYS stack begins.
     *  All computations were done at top, save pointer to it now.
     */
    g_pcont = cont_stack;
#endif
    /*
     *  Use new calculated SYS stack from top.
     *  Call the entry point of the SDK code.
     */
    asm volatile("" ::: "memory");
    asm volatile ("mov.n a1, %0\n" :: "r" (sys_stack_first));

#ifndef USE_IRAM
    asm volatile ("call0 Cache_Read_Disable\n" ::);
#endif
    asm volatile("movi a0, 0x4000044c\n"     /* Should never return; however, set return to Boot ROM Breakpoint */
                 "j call_user_start\n" ::);

    __builtin_unreachable();
}

void ICACHE_RAM_ATTR app_entry_redefinable(void) {
    /*
     * There are 4 sections of code that share the stack starting near
     * 0x40000000.
     *   1) The Boot ROM (uses around 640 bytes)
     *   2) The Bootloader, eboot.elf (last seen using 720 bytes.)
     *   3) `app_entry_redefinable()` just before it starts the SDK.
     *   4) The NONOS SDK, optionally the Core when the extra 4K option is
     *      selected.
     *
     * Use the ROM BSS zeroed out memory as the home for our temporary stack.
     * This way no additional information will be lost. That will remove this
     * tool from the list of possible concerns for stack overwrite.
     *
     */
    asm volatile ("movi a1, 0x3fffeb30\n" ::);

#ifndef USE_IRAM
    // Enable cache over flash
    asm volatile ("movi.n a2, 0\n"
                  "mov.n  a3, a2\n"
                  "movi.n a4, 0\n"    // ICACHE_SIZE_16
                  "call0 Cache_Read_Enable\n" ::);
#endif

    asm volatile ("j app_entry_start" ::);

    /*
     * Keep this function with just asm seems to help avoid a stack frame being
     * created for this function and things getting really confused.
     */

    __builtin_unreachable();
}


#if defined(HWDT_INFO)
void preinit(void) {
    /*
     * Fill the rom_stack while it is not actively being used.
     *
     * I am thinking that during the time the sketch is running this block of
     * memory could be used for a scratch buffer.
     */
    for (size_t i = 0; i < g_rom_stack_A16_sz/sizeof(uint32_t); i++) {
        g_rom_stack[i] = CONT_STACKGUARD;
    }
}
#endif

};

#else
void enable_debug_hwdt_at_link_time (void){
}
#endif // end of #ifdef DEBUG_HWDT
