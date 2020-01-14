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
 * Timer has struck and a new boot cycle has begun. Since we have a late start,
 * some information may be lost due to DRAM usage by the Boot ROM and the
 * bootloader.
 *
 * We are using the method defined for `core_esp8266_app_entry_noextra4k.cpp` to
 * load an alternate `app_entry_redefinable()`. For details on this method, see
 * comments in `core_esp8266_main.cpp's app_entry()`.
 *
 * Using this alternate we can gain control before the SDK is started.
 * And dump what is left of the "sys" and "cont" stacks.
 *
 * By making some adjustments to start of the stack pointer, at the entry to
 * `app_entry_redefinable()`, and also to the stack pointer passed to the SDK,
 * we can preserve the stack during an HWDT event. At least thats the idea.
 *
 * To use, just place this file in the sketch directory and rebuild and upload
 * your sketch. That should be enough to get it included in your sketch. If
 * that does not work, then add a call to:
 *    `void enable_debug_hwdt_at_link_time (void);`
 * in `setup()`. Also, be sure to initialize the Serial interface for printing.
 * This tool relies on the Serial interface already being setup from the
 * previous boot cycle.
 *
 * When you get a stack dump, copy-paste it into the "ESP Exception Decoder".
 * Since we don't have a SP, we see a lot more stuff in the report. Start at the
 * bottom and work your way up. At this time I have not had a lot of practice
 * using this tool.  TODO: Update description with more details when available.
 *
 *
 *
 * Possible Issues/Thoughts/Improvements:
 *
 * I wonder if the eboot could be changed to use at stack pointer beginning at
 * 0x3fffeb30. This would allow for a value near 640 for ROM_STACK_SIZE.
 *
 * If a problem should arise with some data elements being corrupted during
 * reboot, would it be possible to move their DRAM location higher in memory.
 *
 */

#define DEBUG_HWDT

#ifdef DEBUG_HWDT

#include <c_types.h>
#include "cont.h"
#include "coredecls.h"
#include <core_esp8266_features.h>
#include <esp8266_undocumented.h>
#include <esp8266_peri.h>
#include <uart.h>

extern "C" {
#include <user_interface.h>
#include <uart_register.h>
extern void call_user_start();
extern uint32_t rtc_get_reset_reason(void);
}
// #define DEBUG_HWDT_DEBUG

/*
 * DEBUG_HWDT_NO4KEXTRA
 *
 * This option will leave more of the system stack available for the stack dump.
 * The problem with the "4K extra" option is that it pushes the system stack up
 * into the ROM's BSS area which gets zeroed at reboot by the Boot ROM.
 *
 * Using this option has the effect of taking 4K of DRAM away from the heap
 * which gets used for the "cont" stack. Leaving an extra 4K on the "sys" stack,
 * that is clear of the ROM's BSS area. This allows for a more complete "sys"
 * stack dump.
 *
 */
#define DEBUG_HWDT_NO4KEXTRA

/*
 * ROM_STACK_SIZE
 *
 * There are 4 sections of code that share the same stack space that starts at
 * around 0x40000000.
 *   1) The Boot ROM (uses around 640 bytes)
 *   2) The Bootloader, eboot.elf (uses around 720 bytes.)
 *   3) `app_entry_redefinable()` just before it starts the SDK.
 *   4) The NONOS SDK, optionally the Core when the extra 4K option is selected.
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
 * Gather some useful information on ROM and bootloader combined, sys, and cont
 * stack usage as well as other stuff that comes up.
 *
 */
 #define HWDT_INFO


/*
 * HWDT_OPTION_THE_OPTIMIST
 *
 * The optimist's choice. It can be used to reduce the IRAM footprint. It
 * assumes the strings in memory before the crash are still valid and can be
 * used for printing the report.
 *
 * EDIT: It appears I am mistaken. String constants appear to be loaded
 * at boot time and are available from the start before crt0 runs. I'll wait
 * for a reviewer confirmation before making this option a permanent selection.
 *
 */
 #define HWDT_OPTION_THE_OPTIMIST


/*
 * ROM_STACK_DUMP
 *
 * Dump the stack contents of the ROM Stack area. Good for getting a visual
 * on stack usage. Probably not of value beyond developing this tool.
 *
 #define ROM_STACK_DUMP
 */

/*
 * HWDT_IF_METHOD_RESET_REASON
 *
 * If statement vs switch method to implement the logic. Both can be made
 * smaller by removing confirmation checks.
 *
 * Checks are being performed when DEBUG_HWDT_DEBUG_RESET_REASON has been
 * defined.
 *
 #define DEBUG_HWDT_DEBUG_RESET_REASON
 */
 #define HWDT_IF_METHOD_RESET_REASON

/*
 * HWDT_PRINT_GREETING
 *
 * Prints a simple introduction to let you know this tool is active and in the
 * build. At power-on this may not be viewable on some devices. The crystal
 * has to be 40Mhz for this to work w/o using the HWDT_UART_SPEED option below.
 * May not be worth the cost in IRAM.
 *
 * EDIT: There is something different in the UART setup after a EXT_RST after
 * flash upload. I am unable to print using the same code that works for
 * Power-on and an EXT_RST at any other time. After the SDK has run a 2nd
 * EXT_RST will show the greeting message.
 *
 */
 #define HWDT_PRINT_GREETING

/*
 * HWDT_UART_SPEED
 *
 * UART serial speed to be used for stack dump. At boot/reboot the ESP8266 ROM
 * sets a speed of 115200 BPS. If you are using this default speed you can skip
 * this option and save the IRAM space.
 *
 */
 #define HWDT_UART_SPEED (115200)

/*
 * If you do not need the information in provided by HWDT info, you do not need
 * this include. If you do, uncomment the include line and copy-paste the
 * include block below into its respective filename.
 *
 */
// #include "hwdt_app_entry.h"


/*
 * This is only used to verify that the internal and external structure
 * definitions match.
 */
#ifdef HWDT_STACK_DUMP_H
#define HWDT_INFO_t LOCAL_HWDT_INFO_t
#define hwdt_info LOCAL_hwdt_info
#endif


/******** Start of copy-paste block to create "hwdt_app_entry.h" **************/
#if !defined(HWDT_STACK_DUMP_H) || defined(HWDT_INFO_t)
#define HWDT_STACK_DUMP_H

typedef struct HWDT_INFO {
    uint32_t rom;
    uint32_t sys;
    uint32_t cont;
    uint32_t rtc_sys_reason;
    uint32_t rom_api_reason;
    uint32_t cont_integrity;
    uint32_t reset_reason;
    bool g_pcont_valid;
} HWDT_INFO_t;

void enable_debug_hwdt_at_link_time(void);

extern uint32_t *g_rom_stack;
extern HWDT_INFO_t hwdt_info;

#endif
/******** End of copy-paste this block for creating "hwdt_app_entry.h" ********/


#ifdef HWDT_INFO_t
#undef HWDT_INFO_t
#undef hwdt_info
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
 * claimed by the SDK for stack space. This is a problem area for stack dump,
 * because the ROM BSS gets zeroed as part of ROM init on reboot. Any part of
 * the "sys" stack residing there is lost. On the other hand, it becomes a prime
 * candidate for DRAM address space to handle the needs of this stack dump
 * utility.
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

void enable_debug_hwdt_at_link_time(void)
{
    /*
     * This function does nothing; however, including a call to it in setup,
     * allows this module to override, at link time, the core_esp8266_main.cpp's
     * `app_entry()` with the one below. This will create a stack dump on
     * Hardware WDT resets.
     *
     * It appears just including this module in the sketch directory is enough.
     * However, just play it safe, call this function from setup.
     */
}


/* the following code is linked only if a call to the above function is made somewhere */

extern "C" {
#if 0 // Handy debug print during development
static void ICACHE_RAM_ATTR print_size(uintptr_t val) {
    uint32_t fmt_sz[4];
    fmt_sz[0]  = ('0' ) | ('x' <<8) | ('%' <<16) | ('0' <<24);
    fmt_sz[1]  = ('8' ) | ('X' <<8) | (',' <<16) | (' ' <<24);
    fmt_sz[2]  = (' ' ) | ('%' <<8) | ('5' <<16) | ('u' <<24);
    fmt_sz[3]  = ('\n') | ('\0'<<8) | ('\0'<<16) | ('\0'<<24);
    ets_printf((const char *)fmt_sz, val, val);
}
#endif


enum PRINT_STACK {
    CONT = 1,
    SYS = 2,
    ROM = 4
};

#if defined(HWDT_OPTION_THE_OPTIMIST)
static void ICACHE_RAM_ATTR print_stack(uintptr_t start, uintptr_t end, uint32_t chunk) {
    ets_printf("\n>>>stack>>>\n\nctx: ");

    if (chunk & PRINT_STACK::CONT) {
        ets_printf("cont");
    } else
    if (chunk & PRINT_STACK::SYS) {
        ets_printf("sys");
    } else
    if (chunk & PRINT_STACK::ROM) {
        ets_printf("ROM");
    }

    ets_printf("\nsp: %08x end: %08x offset: %04x\n", start, end, 0);

    size_t this_mutch = end - start;
    if (this_mutch >= 0x10) {
        for (size_t pos = 0; pos < this_mutch; pos += 0x10) {
            uint32_t *value = (uint32_t *)(start + pos);

            // rough indicator: stack frames usually have SP saved as the second word
            bool looksLikeStackFrame = (value[2] == (start + pos + 0x10));
            ets_printf("%08x:  %08x %08x %08x %08x %c\n", (uint32_t)&value[0],
                       value[0], value[1], value[2], value[3],
                       (looksLikeStackFrame)?'<':' ');
        }
    }

    ets_printf("<<<stack<<<\n");
}

#else
static void ICACHE_RAM_ATTR print_stack(uintptr_t start, uintptr_t end, uint32_t chunk) {

    uint32_t fmt_stk[6];
    fmt_stk[0] = ('\n') | ('>' <<8) | ('>' <<16) | ('>' <<24);
    fmt_stk[1] = ('s' ) | ('t' <<8) | ('a' <<16) | ('c' <<24);
    fmt_stk[2] = ('k' ) | ('>' <<8) | ('>' <<16) | ('>' <<24);
    fmt_stk[3] = ('\n') | ('\n'<<8) | ('c' <<16) | ('t' <<24);
    fmt_stk[4] = ('x' ) | (':' <<8) | (' ' <<16) | ('%' <<24);
    fmt_stk[5] = ('s' ) | ('\n'<<8) | ('\0'<<16) | ('\0'<<24);

    uint32_t fmt_sp[9];
    fmt_sp[0]  = ('s' ) | ('p' <<8) | (':' <<16) | (' ' <<24);
    fmt_sp[1]  = ('%' ) | ('0' <<8) | ('8' <<16) | ('x' <<24);
    fmt_sp[2]  = (' ' ) | ('e' <<8) | ('n' <<16) | ('d' <<24);
    fmt_sp[3]  = (':' ) | (' ' <<8) | ('%' <<16) | ('0' <<24);
    fmt_sp[4]  = ('8' ) | ('x' <<8) | (' ' <<16) | ('o' <<24);
    fmt_sp[5]  = ('f' ) | ('f' <<8) | ('s' <<16) | ('e' <<24);
    fmt_sp[6]  = ('t' ) | (':' <<8) | (' ' <<16) | ('%' <<24);
    fmt_sp[7]  = ('0' ) | ('4' <<8) | ('x' <<16) | ('\n'<<24);
    fmt_sp[8]  = ('\0') | ('\0'<<8) | ('\0'<<16) | ('\0'<<24);

    uint32_t fmt_rom[1];
    fmt_rom[0]  = ('R' ) | ('O' <<8) | ('M' <<16) | ('\0'<<24);

    uint32_t fmt_sys[1];
    fmt_sys[0]  = ('s' ) | ('y' <<8) | ('s' <<16) | ('\0'<<24);

    uint32_t fmt_cont[2];
    fmt_cont[0] = ('c' ) | ('o' <<8) | ('n' <<16) | ('t' <<24);
    fmt_cont[1] = ('\0') | ('\0'<<8) | ('\0'<<16) | ('\0'<<24);

    if (chunk & PRINT_STACK::CONT) {
        ets_printf((const char *)fmt_stk, (const char *)fmt_cont);
    } else
    if (chunk & PRINT_STACK::SYS) {
        ets_printf((const char *)fmt_stk, (const char *)fmt_sys);
    } else
    if (chunk & PRINT_STACK::ROM) {
        ets_printf((const char *)fmt_stk, (const char *)fmt_rom);
    }

    ets_printf((const char *)fmt_sp, start, end, 0);

    {
        uint32_t fmt_stk_dmp[8];
        fmt_stk_dmp[0] = ('%') | ('0' <<8) | ('8' <<16) | ('x' <<24);
        fmt_stk_dmp[1] = (':') | (' ' <<8) | (' ' <<16) | ('%' <<24);
        fmt_stk_dmp[2] = ('0') | ('8' <<8) | ('x' <<16) | (' ' <<24);
        fmt_stk_dmp[3] = ('%') | ('0' <<8) | ('8' <<16) | ('x' <<24);
        fmt_stk_dmp[4] = (' ') | ('%' <<8) | ('0' <<16) | ('8' <<24);
        fmt_stk_dmp[5] = ('x') | (' ' <<8) | ('%' <<16) | ('0' <<24);
        fmt_stk_dmp[6] = ('8') | ('x' <<8) | (' ' <<16) | ('%' <<24);
        fmt_stk_dmp[7] = ('c') | ('\n'<<8) | ('\0'<<16) | ('\0'<<24);

        size_t this_mutch = end - start;
        if (this_mutch >= 0x10) {
            for (size_t pos = 0; pos < this_mutch; pos += 0x10) {
                uint32_t *value = (uint32_t *)(start + pos);

                // rough indicator: stack frames usually have SP saved as the second word
                bool looksLikeStackFrame = (value[2] == (start + pos + 0x10));
                ets_printf((const char*)fmt_stk_dmp, (uint32_t)&value[0],
                           value[0], value[1], value[2], value[3],
                           (looksLikeStackFrame)?'<':' ');
            }
        }
    }
    {
        uint32_t fmt_stk_end[4];
        fmt_stk_end[0] = ('<' ) | ('<' <<8) | ('<' <<16) | ('s' <<24);
        fmt_stk_end[1] = ('t' ) | ('a' <<8) | ('c' <<16) | ('k' <<24);
        fmt_stk_end[2] = ('<' ) | ('<' <<8) | ('<' <<16) | ('\n'<<24);
        fmt_stk_end[3] = ('\n') | ('\0'<<8) | ('\0'<<16) | ('\0'<<24);
        ets_printf((const char *)fmt_stk_end);
    }
}
#endif

static const uint32_t * ICACHE_RAM_ATTR skip_stackguard(const uint32_t *start, const uint32_t *end, const uint32_t pattern) {
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

static void ICACHE_RAM_ATTR check_g_pcont_validity(void) {
    /*
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

static uint32_t ICACHE_RAM_ATTR get_reset_reason(bool* power_on, bool* hwdt_reset) {
    /*
     * Detecting a Hardware WDT (HWDT) reset is a little complicated at boot
     * before the SDK is started.
     *
     * While the ROM API will report an HWDT, it does not change the status
     * after a software restart. And the SDK has not been started so its API is
     * not available.
     *
     * There is a value in System RTC memory that appears to store, at boot,
     * the reset reason for the SDK. It appears to be set before the SDK
     * performs its restart. Of course, this value is invalid at power on
     * before the SDK runs.
     *
     * Case 1: At power-on boot the ROM API result is valid; however, the SDK
     * value in RTC Memory has not been set at this time.
     *
     * Case 2: A HWDT reset has occurred, which is later followed with a
     * restart by the SDK. At boot, the ROM API result still reports the HWDT
     * reason.
     *
     *
     *
     * I need to know if this is the 1st boot at power on. Combining these
     * indicators has been tricky, I think I now have it.
     *
     */

    uint32_t rtc_sys_reason = hwdt_info.rtc_sys_reason = RTC_SYS[0];
    hwdt_info.rom_api_reason = rtc_get_reset_reason();
    ROM_RST_REASON_t rom_api_reason = (ROM_RST_REASON_t)hwdt_info.rom_api_reason;

#ifdef HWDT_IF_METHOD_RESET_REASON
    *hwdt_reset = false;
    *power_on = false;
    /*
     * This logic takes the reason left in memory by the SDK as an initial
     * estimate and expands on it.
     */
    hwdt_info.reset_reason = rtc_sys_reason;
    if (REASON_WDT_RST == rtc_sys_reason) {
        if (OWDT_RESET == rom_api_reason) {
            *hwdt_reset = true;
        } else {
            hwdt_info.reset_reason = REASON_EXT_SYS_RST;
            if (!debug__confirm_rom_reason(EXT_RESET)) {
                hwdt_info.reset_reason = ~0;
            }
        }

    } else if (0 == rtc_sys_reason) {
        /*
         * 1) The 0 values shows up with multiple EXT_RSTs quickly.
         *    The 1 value, previous if, shows up if you wait a while before
         *    the EXT_RST.
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
          } else if (!debug__confirm_rom_reason(EXT_RESET)) {
              hwdt_info.reset_reason = ~0;
          } else {
              hwdt_info.reset_reason = REASON_EXT_SYS_RST;
          }
          break;
        /* These should be correct as is */
        case REASON_EXCEPTION_RST:
        case REASON_SOFT_WDT_RST:
        case REASON_SOFT_RESTART:
        case REASON_DEEP_SLEEP_AWAKE:    // TODO: Untested.
            hwdt_info.reset_reason = rtc_sys_reason;
            break;
        /*
         * REASON_EXT_SYS_RST is not expected at reboot, let it fall though to
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
            } else if (!debug__confirm_rom_reason(EXT_RESET)) {
                hwdt_info.reset_reason = ~0;
            } else {
                hwdt_info.reset_reason = REASON_EXT_SYS_RST;
            }
            break;
    }
#endif
    return hwdt_info.reset_reason;
}

#ifdef HWDT_UART_SPEED
/*
 * We need this because the SDK has an override on this already. If we call it
 * at this time we will crash in the unintialized SDK.
 */
#ifndef ROM_uart_div_modify
#define ROM_uart_div_modify         0x400039d8
#endif
typedef int (*fp_uart_div_modify_t)(uint32_t uart_no, uint32 DivLatchValue);
constexpr fp_uart_div_modify_t real_uart_div_modify = (fp_uart_div_modify_t)ROM_uart_div_modify;


static uint32_t ICACHE_RAM_ATTR set_uart_speed(uint32_t uart_no, uint32_t new_speed) {
    (void)uart_no;

    // #if F_CRYSTAL == 40000000
#ifdef F_CRYSTAL
    constexpr uint32_t crystal_freq = F_CRYSTAL;
#else
    constexpr uint32_t crystal_freq = 26000000;
#endif
    constexpr uint32_t UART_CLK_DIVISOR_M = (0x000FFFFF);
    constexpr uint32_t rom_uart_speed = 115200;
    uint32_t uart_divisor = ESP8266_REG(0x14) & UART_CLK_DIVISOR_M;
    uint32_t master_freq = crystal_freq * 2;

    if (REASON_DEFAULT_RST       == hwdt_info.reset_reason ||
        REASON_EXT_SYS_RST       == hwdt_info.reset_reason ||
        REASON_DEEP_SLEEP_AWAKE  == hwdt_info.reset_reason) {
        /*
         * At this time, with a power on boot or EXT_RSTs the CPU Frequency
         * calibration has not happended. Thus for a 26MHz Xtal, the CPU clock
         * is running at 52MHz. Tweak uart speed here, so printing works.
         * To avoid confusion at exit, put the divisor back as we found it.
         */
        master_freq = crystal_freq * 2;
    } else {
        /*
         * No adjustments needed on reboots, etc.
         * Avoid accumulating divide errors.
         */
        master_freq = ((uart_divisor * rom_uart_speed) < 100000000) ? 80000000 : 160000000;
    }
    uint32_t new_uart_divisor = master_freq / new_speed;

#if defined(DEBUG_HWDT_DEBUG)
    int rc = 1;
    if (new_uart_divisor != uart_divisor) {
        rc = real_uart_div_modify(0, new_uart_divisor);
        ets_delay_us(50); // Allow one 20K BPS character time to pass
    }
    ets_printf("\n\n%d = real_uart_div_modify(0, %u / %u);\n", rc, master_freq, new_speed);
    ets_printf("F_CRYSTAL = %u\n", crystal_freq);
    ets_printf("old uart_divisor = %u\n", uart_divisor);
    ets_printf("master_freq = %u\n", master_freq);
    return (rc) ? 0 : uart_divisor;
#else
    if (new_uart_divisor != uart_divisor) {
        return (real_uart_div_modify(0, new_uart_divisor)) ? 0 : uart_divisor;
    }
    return 0;
#endif
}
#endif
/*
 *
 *
 *
 *
 */
static void ICACHE_RAM_ATTR handle_hwdt(void) {

    ets_memset(&hwdt_info, 0, sizeof(hwdt_info));
    check_g_pcont_validity();

    bool power_on = false;
    bool hwdt_reset = false;
    get_reset_reason(&power_on, &hwdt_reset);

#ifdef HWDT_UART_SPEED
    uint32_t uart_divisor = set_uart_speed(0, HWDT_UART_SPEED);
#endif
#if defined(DEBUG_HWDT_DEBUG)
    ets_printf("Basic boot reason: %s\n", (power_on) ? "Power-on" : "Reboot");
    ets_printf("RTC_SYS Reset Reason = %u\n", hwdt_info.rtc_sys_reason);
    ets_printf("ROM API Reset Reason = %u\n", hwdt_info.rom_api_reason);
    ets_printf("HWDT Reset Reason = %u\n\n", hwdt_info.reset_reason);
#endif
#if defined(DEBUG_HWDT_DEBUG_RESET_REASON)
    if (REASON_EXT_SYS_RST < hwdt_info.reset_reason) {
        ets_printf("Reset reason confirmation failed!\n");
        ets_printf("  RTC_SYS Reset Reason = %u\n", hwdt_info.rtc_sys_reason);
        ets_printf("  ROM API Reset Reason = %u\n", hwdt_info.rom_api_reason);
    }
#endif
    // After a flash upload memory cannot be trusted.
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
            {
#if defined(HWDT_OPTION_THE_OPTIMIST)
              ets_printf("\nHardware WDT reset\n");
#else
              uint32_t fmt_hwdt[6];
              fmt_hwdt[0] = ('\n') | ('H' <<8) | ('a' <<16) | ('r' <<24);
              fmt_hwdt[1] = ('d' ) | ('w' <<8) | ('a' <<16) | ('r' <<24);
              fmt_hwdt[2] = ('e' ) | (' ' <<8) | ('W' <<16) | ('D' <<24);
              fmt_hwdt[3] = ('T' ) | (' ' <<8) | ('r' <<16) | ('e' <<24);
              fmt_hwdt[4] = ('s' ) | ('e' <<8) | ('t' <<16) | ('\n'<<24);
              fmt_hwdt[5] = 0;
              ets_printf((const char*)fmt_hwdt);
#endif
            }
            print_stack((uintptr_t)ctx_sys_ptr, (uintptr_t)rom_stack, PRINT_STACK::SYS);

#ifdef DEBUG_HWDT_NO4KEXTRA
            /* Print separate ctx: cont stack */
            print_stack((uintptr_t)ctx_cont_ptr, (uintptr_t)g_pcont->stack_end, PRINT_STACK::CONT);
#endif
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
    ets_printf("\n\nHardware WDT Stack Dump - enabled\n\n");
#endif

ets_delay_us(12000); /* Let UART FIFO clear. */
#ifdef HWDT_UART_SPEED
    if (uart_divisor) {
        real_uart_div_modify(0, uart_divisor); // Put it back the way we found it!
    }
#endif
}

void ICACHE_RAM_ATTR app_entry_start(void) {
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
    asm volatile("mov.n a1, %0\n"
                 "mov.n a3, %1\n"
                 "mov.n a0, %2\n"     // Should never return; however, set return to Boot ROM Breakpoint
                 "jx a3\n" : : "r" (sys_stack_first), "r" (call_user_start), "r" (0x4000044c) );

    __builtin_unreachable();
}


void ICACHE_RAM_ATTR app_entry_redefinable2(void) {
    handle_hwdt();
    app_entry_start();

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
     * Make the ROM BSS zeroed out memory the home for our temporary stack. That
     * way no additional information will be lost. That will remove this tool
     * from the list of possible concerns for stack overwrite. For now, At this
     * time it is set to 1024.
     *
     */
    asm volatile ("mov.n a1, %0\n"
                  "mov.n a3, %1\n"
                  "mov.n a0, %2\n"
                  "jx a3\n" : : "r" (0x3fffeb30), "r" (app_entry_redefinable2), "r" (0x4000044c) );
    /*
     * TODO: Look at the assembly, I had to split app_entry_redefinable like
     * this to stop some back indexing on the stack. Looks like some temporary
     * storage was set up and was being referenced and I moved it away by
     * changing the stack pointer. Splitting the `app_entry_redefinable()`
     * function into two parts seems to have taken care of that issue.
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
