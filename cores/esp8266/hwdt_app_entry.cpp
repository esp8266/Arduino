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
 * To use this tool, select HWDT or HWDT_NOEXTRA4K from the Arduino IDE menu
 * "Tools->Debug Level" before building your sketch. Note, 'Tools->Debug port'
 * selection is not needed or referenced for printing the HWDT stack dump.
 *
 * To enable in other build environments, add DEBUG_ESP_HWDT_NOEXTRA4K or
 * DEBUG_ESP_HWDT global defines to your build.
 *
 * This tool prints to the serial port at the default serial port speed set by
 * the Boot ROM. On a Hardware WDT reset that port speed is 115200 bps. If your
 * needs differ, see the DEBUG_ESP_HWDT_UART_SPEED option below.
 *
 * More on crystal differences and data rates:
 *     When the ESP8266 restarts because of a Hardware WDT reset, the port speed
 *     defaults to 115200. This will be the speed, even if you have a 26MHz or
 *     40MHz Crystal. If you would like to use a different data rate, use the
 *     option DEBUG_ESP_HWDT_UART_SPEED described below.
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
 * SYS Stack Issue with Extra 4K Heap option:
 *     During WiFi Connect, Reconnect, and about every hour a block of memory
 *     0x3FFFEA80 - 0x3FFFEB30 (176 bytes) is zeroed by the Boot ROM function
 *     aes_decrypt_init. All other painted memory in the area was untouched
 *     after starting WiFi. See `core/esp8266/aes_unwrap.cpp` for more details.
 *
 *
 * Possible Issues/Thoughts/Improvements:
 *
 * On reboot after an OTA download, eboot requires a lot of stack and DRAM
 * space. On the other hand, for routine loads from flash, the stack and DRAM
 * usage is small, leaving us valid data to print a stack dump.
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
 * Maybe an in/out ref count would be nice for bearssl and cont stacks.
 */

/*____________________________________________________________________________*/
/*                                                                            */
/*                          Configuration Options                             */
/*____________________________________________________________________________*/


/*
 * DEBUG_ESP_HWDT
 *
 * Enables this debug tool for printing a Hardware WDT stack dump at restart.
 *
 * This option is now managed from the Arduino IDE menu 'Tools->Debug Level'
 #define DEBUG_ESP_HWDT
 */


/*
 * DEBUG_ESP_HWDT_NOEXTRA4K
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
 * This option is now managed from the Arduinoo IDE menu 'Tools->Debug Level'
 #define DEBUG_ESP_HWDT_NOEXTRA4K
 */


/*
 * DEBUG_ESP_HWDT_UART_SPEED
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
 // #define DEBUG_ESP_HWDT_UART_SPEED (19200)
 // #define DEBUG_ESP_HWDT_UART_SPEED (74880)
 // #define DEBUG_ESP_HWDT_UART_SPEED (115200)
 // #define DEBUG_ESP_HWDT_UART_SPEED (230400)


/*
 * DEBUG_ESP_HWDT_PRINT_GREETING
 *
 * Prints a simple introduction to let you know this tool is active and in the
 * build. At power-on, this may not be viewable on some devices. The crystal has
 * to be 40Mhz for this to work w/o using the DEBUG_ESP_HWDT_UART_SPEED option
 * above. May not be worth the cost in IRAM.
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
 * EDIT3: The delay before the uart_div_modify() has been replaced with a wait
 * till FIFO empty loop. I now believe the lost greeting message after an
 * esptool firmware update, has to do with the transition period between the
 * tool performing hardware reset and exiting, then the serial monitor
 * re-engaging. This is not an issue that needs to be addressed here.
 */
 #define DEBUG_ESP_HWDT_PRINT_GREETING


/*
 * DEBUG_ESP_HWDT_ROM_STACK_SIZE
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
 * For the "NO extra 4K Heap" case, we use a ROM stack size of 1024. However,
 * without `aes_unwrap.cpp`, 1024 is not safe for the "extra 4K of heap" case.
 * Bad crashes happen with the 1024 and the "extra 4K of Heap". For now, leave
 * this case with 720 bytes for ROM Stack since it also gives more SYS stack for
 * dumping. See comment in aes_unwrap.cpp for AES buffer clash with SYS stack
 * space description.
 *
 * If or when eboot.elf uses more than 720 there will be a little over-writing
 * of the cont stack that we report. Then 720 can be increased as long as the
 * replacement aes_unwrap is used.
 *
 * If possible, use the no-extra 4K heap option. This is the optimum choice for
 * debugging HWDT crashes. It has the content of SYS stack fully exposed.
 *
 */
#ifndef DEBUG_ESP_HWDT_ROM_STACK_SIZE
  #ifdef DEBUG_ESP_HWDT_NOEXTRA4K
    #define DEBUG_ESP_HWDT_ROM_STACK_SIZE (1024UL)
  #else
    #define DEBUG_ESP_HWDT_ROM_STACK_SIZE (720UL)
  #endif
#endif


/*
 * DEBUG_ESP_HWDT_INFO
 *
 * Gather some information on ROM and bootloader combined, sys, and cont stack
 * usage. If you are missing the include file for this structure, you can
 * copy-paste from the embedded version of the .h below.
 *
 */
 // #define DEBUG_ESP_HWDT_INFO


/*
 * ROM_STACK_DUMP
 *
 * Dump the stack contents of the ROM Stack area. This gives us a visual of the
 * stack usage. Probably not of value, beyond developing this tool.
 *
 * To see printing, you may need to use this option with DEBUG_ESP_HWDT_UART_SPEED.
 */
 // #define ROM_STACK_DUMP


/*
 * HWDT_IF_METHOD_RESET_REASON
 *
 * "If" statement or "switch" method to implement, the reset reason logic. Both
 * can be made smaller by removing confirmation checks.
 *
 * Checks are performed when DEBUG_ESP_HWDT_DEV_DEBUG_RESET_REASON has been defined.
 *
 * EDIT: I should settle on one or the other; however, new issues continue to
 * pop up on determining reset reason. I'll wait until later and pick one.
 *
 #define DEBUG_ESP_HWDT_DEV_DEBUG_RESET_REASON
 */
 #define HWDT_IF_METHOD_RESET_REASON

/*____________________________________________________________________________*/
/*                                                                            */
/*                     End of Configuration Options                           */
/*____________________________________________________________________________*/

#if defined(DEBUG_ESP_HWDT) || defined(DEBUG_ESP_HWDT_NOEXTRA4K)

#include <c_types.h>
#include "cont.h"
#include "coredecls.h"
#include <core_esp8266_features.h>
#include <esp8266_undocumented.h>
#include <esp8266_peri.h>
#include <uart.h>
#include <pgmspace.h>
#include "mmu_iram.h"

extern "C" {
#include <user_interface.h>
extern void call_user_start();
extern uint32_t rtc_get_reset_reason(void);

uint32_t __zero_return() {
  return 0;
}
// extern void stack_thunk_dump_stack();
extern uint32_t stack_thunk_get_refcnt() __attribute__((weak, alias("__zero_return")));
extern uint32_t stack_thunk_get_stack_top() __attribute__((weak, alias("__zero_return")));
extern uint32_t stack_thunk_get_stack_bot() __attribute__((weak, alias("__zero_return")));

}

// #define DEBUG_ESP_HWDT_DEV_DEBUG
// #define USE_IRAM

//C To the maintainers: Since the Cache_Read_Enable logic seems to work, do you
//C see a need or would you like to keep the IRAM build path?
//C If you see no need for it the USE_IRAM build path could be deleted.
#ifdef USE_IRAM
#undef USE_IRAM
#define USE_IRAM 1
#define IRAM_MAYBE IRAM_ATTR

#else
#undef USE_IRAM
#define IRAM_MAYBE
#endif

#define STATIC static __attribute__((noinline))
// #define STATIC

#ifdef DEBUG_ESP_HWDT_DEV_DEBUG
/*
 * We have two copies of hwdt_info_t. Verify internal and external structures
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
#define hwdt_info_t LOCAL_HWDT_INFO_T
#define hwdt_info_ LOCAL_HWDT_INFO_
#define hwdt_info LOCAL_HWDT_INFO
#define HWDT_VERIFY_HWDT_INFO
#endif

/*
 * If you are using the hwdt_info_t structure, and are missing the include file.
 * Copy-paste the include block below into its respective filename.
 */

/*____________________________________________________________________________*/
/*                                                                            */
/*        Start of copy-paste block to create "hwdt_app_entry.h"              */
/*____________________________________________________________________________*/
#if !defined(HWDT_STACK_DUMP_H) || defined(HWDT_VERIFY_HWDT_INFO)
#define HWDT_STACK_DUMP_H

typedef struct hwdt_info_ {
    uint32_t rom;
    uint32_t sys;
    uint32_t cont;
    uint32_t bearssl;
    uint32_t rom_api_reason;
    uint32_t rtc_sys_reason;
    uint32_t reset_reason;
    uint32_t cont_integrity;
    bool g_pcont_valid;
} hwdt_info_t;

extern "C" void debug_hwdt_init(void);

extern uint32_t *g_rom_stack;
extern hwdt_info_t hwdt_info;

#endif
/*____________________________________________________________________________*/
/*                                                                            */
/*        End of copy-paste block for creating "hwdt_app_entry.h"             */
/*____________________________________________________________________________*/


#ifdef HWDT_VERIFY_HWDT_INFO
#undef hwdt_info_t
#undef hwdt_info_
#undef hwdt_info
#undef HWDT_VERIFY_HWDT_INFO
static_assert(sizeof(hwdt_info_t) == sizeof(LOCAL_HWDT_INFO_T), "Local and include version of hwdt_info_t do not match.");
#endif


#define MK_ALIGN16_SZ(a) (((a) + 0x0FUL) & ~0x0FUL)
#define ALIGN_UP(a, s) ((decltype(a))((((uintptr_t)(a)) + (s-1)) & ~(s-1)))
#define ALIGN_DOWN(a, s) ((decltype(a))(((uintptr_t)(a)) & ~(s-1)))

#ifndef CONT_STACKGUARD
#define CONT_STACKGUARD 0xfeefeffe
#endif
#define RTC_SYS         ((volatile uint32_t*)0x60001100UL)

/*
 *
 #define DRAM_START      ((uint32_t *)0x3FFE8000UL)
 #define DRAM_END        ((uint32_t *)0x40000000UL)
 *
 * The space between 0x3fffe000 up to 0x3fffeb30 is a ROM BSS area that is later
 * claimed by the SDK for stack space. This is a problem area for this tool,
 * because the ROM BSS gets zeroed as part of ROM init on reboot. Any part of
 * the "sys" stack residing there is lost. On the other hand, it becomes a prime
 * candidate for DRAM address space to handle the needs of this tool.
 *
 #define SYS_STACK_E000     ((uint32_t *)0x3fffe000UL)
 */

#define ROM_STACK_FIRST ((uint32_t *)0x40000000UL)
#define SYS_STACK       ((uint32_t *)0x3fffeb30UL)

// Map out who will live where.
#define ROM_STACK_A16_SZ   (MK_ALIGN16_SZ(DEBUG_ESP_HWDT_ROM_STACK_SIZE))
#define CONT_STACK_A16_SZ  (MK_ALIGN16_SZ(sizeof(cont_t)))
/*
 * For WPS support, cont stack comes out of the user's heap address space.
 * The NONOS-SDK stack address is initialized before the reserved ROM stack
 * space. In this configuration there is no extra 4K in the heap.
 * Memory map: 0x3FFE8000, ..., (CONT_STACK), ..., (SYS), (ROM_STACK), 0x4000000
 *
 * sys_stack_first <= ROM_STACK
 */
#define ROM_STACK ((uint32_t *) ((uintptr_t)ROM_STACK_FIRST - ROM_STACK_A16_SZ))


#define CONT_STACK_FIRST ROM_STACK // only for computation
/*
 * For extra 4K of heap space, the continuation stack (user's stack) is created
 * in the SYS stack address space. The NONOS-SDK stack starts before the cont
 * stack.
 * Memory map: 0x3FFE8000, ..., (SYS), (CONT_STACK), (ROM_STACK), 0x4000000
 *
 * sys_stack_first <= CONT_STACK
 */
#define CONT_STACK ((cont_t *)((uintptr_t)CONT_STACK_FIRST - CONT_STACK_A16_SZ))


uint32_t *g_rom_stack  __attribute__((section(".noinit")));
uint32_t *sys_stack_first  __attribute__((section(".noinit")));
size_t g_rom_stack_A16_sz  __attribute__((section(".noinit")));
hwdt_info_t hwdt_info __attribute__((section(".noinit")));

extern "C" {

extern cont_t * get_noextra4k_g_pcont(void);

cont_t * IRAM_ATTR get_noextra4k_g_pcont(void) __attribute__((weak));
cont_t * IRAM_ATTR get_noextra4k_g_pcont(void) {
    return NULL;
}

static void IRAM_MAYBE set__sys_stack_first(void) {
  if (get_noextra4k_g_pcont()) {
    sys_stack_first = ROM_STACK;
  } else {
    sys_stack_first = (uint32_t *)CONT_STACK;
  }
}

#if USE_IRAM
#define ETS_PRINTF ets_uart_printf

#else
/*
 * This function is already in umm_malloc for some debug options.
 * Define here in case they are not enabled.
 */
int ICACHE_FLASH_ATTR umm_info_safe_printf_P(const char *fmt, ...) __attribute__((weak));
int ICACHE_FLASH_ATTR umm_info_safe_printf_P(const char *fmt, ...) {
    char ram_buf[strlen_P(fmt) + 1];
    strcpy_P(ram_buf, fmt);
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
    ROM = 4,
    BEARSSL = 8
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
    } else
    if (chunk & PRINT_STACK::BEARSSL) {
        ETS_PRINTF("bearssl");
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

bool IRAM_MAYBE hwdt_check_g_pcont_validity(void) {
    /*
     * DRAM appears to remain valid after most resets. There is more on this in
     * handle_hwdt().
     *
     * Testing of vital pointers for validity could also aid as a partial
     * indicator of power-on. Not needed for that purpose at this time.
     */
    cont_t *noextra4k_g_pcont = get_noextra4k_g_pcont();
    if (g_rom_stack == ROM_STACK &&
        g_rom_stack_A16_sz == ROM_STACK_A16_SZ &&
        g_pcont == ((noextra4k_g_pcont) ? noextra4k_g_pcont : CONT_STACK)
        ) {
            hwdt_info.g_pcont_valid = true;
    } else {
        hwdt_info.g_pcont_valid = false;
        g_rom_stack = ROM_STACK;
        g_rom_stack_A16_sz = ROM_STACK_A16_SZ;
    }
    return hwdt_info.g_pcont_valid;
}

#if defined(DEBUG_ESP_HWDT_DEV_DEBUG) || defined(DEBUG_ESP_HWDT_DEV_DEBUG_RESET_REASON)
#ifndef DEBUG_ESP_HWDT_DEV_DEBUG_RESET_REASON
#define DEBUG_ESP_HWDT_DEV_DEBUG_RESET_REASON
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
     * Maybe a TODO: This logic is for the general case of detecting the true
     * reset cause before the SDK is started. It may be possible to reduce to
     * a specific HWDT test. Leaving for now.
     */

    hwdt_info.rtc_sys_reason = RTC_SYS[0];
    const uint32_t rtc_sys_reason = hwdt_info.rtc_sys_reason;
    hwdt_info.rom_api_reason = rtc_get_reset_reason();
    const ROM_RST_REASON_t rom_api_reason = (ROM_RST_REASON_t)hwdt_info.rom_api_reason;

//C To maintainers: Cleanup: Of these two methods is there one you prefer?
//C The other could be deleted.
//C I did it two ways looking to see if one was shorter than the other. I think
//C they ended out being about the same. Also coding it twice, helped check my
//C logic.
#ifdef HWDT_IF_METHOD_RESET_REASON
    *hwdt_reset = false;
    *power_on = false;
    /*
     * This logic takes the reason left in memory by the SDK as an initial
     * estimate and improves on it.
     */
    hwdt_info.reset_reason = rtc_sys_reason;
    if (REASON_DEFAULT_RST == rtc_sys_reason ||
        REASON_WDT_RST     == rtc_sys_reason) {
        /*
         * 1) The 0 value (REASON_DEFAULT_RST) shows up with multiple EXT_RSTs
         *    quickly. The 1 value (REASON_WDT_RST), previous if, shows up if
         *    you wait a while before the EXT_RST.
         * 2) The 0 value also shows up if a HWDT reset occurs too quickly after
         *    the system starts. Note the current SDK will get this one wrong
         *    and indicate power-on reset.
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
         * REASON_EXT_SYS_RST is not expected at reboot, let it fall through
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

#ifdef DEBUG_ESP_HWDT_UART_SPEED
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
// const fp_uart_div_modify_t real_uart_div_modify = (fp_uart_div_modify_t)ROM_uart_div_modify;
#define real_uart_div_modify ((fp_uart_div_modify_t)ROM_uart_div_modify)

#define UART_CLKDIV_MASK 0x000FFFFFUL

void adjust_uart_speed(uint32_t uart_divisor) {
    /*
     * Take care to let serial data in the FIFO finish before changing UART
     * speed. Then after changing speeds, let the uart clock generator
     * stablilize, and let the remote receiver come to an idle state before
     * continuing.
     *
     * Load a Rubout character for the final character shifting out to stop
     * the last character from getting crunched during the speed change.
     *
     * The thinking is if the speed changes while shifting out, as long as the
     * start bit gets out before the change. The change will not be noticed
     * since all ones will look like the serial line idle state. And for text
     * displaying, received rubout characters should be discarded. At least that
     * was true 40 years ago.
     *
     * EDIT: In today's world, it appears the rubout character is no longer
     * discarded. However, I think we are better off using it since it allows
     * for creating a deterministic last character that will not clear the
     * screen or something worse. And it is simpler than the other option of
     * calculating and waiting for a character time to pass.
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
#if (1 < F_CRYSTAL)
    constexpr uint32_t crystal_freq = F_CRYSTAL;
#else
    constexpr uint32_t crystal_freq = 26000000;
#endif
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
        master_freq = crystal_freq * 2;
    }

    uint32_t new_uart_divisor = master_freq / new_speed;

    if (UART_CLKDIV_MASK < new_uart_divisor ||
        2 > new_uart_divisor ||
        new_uart_divisor == uart_divisor) {
        uart_divisor = 0;   // used to indicate no change

    } else {
        adjust_uart_speed(new_uart_divisor);
    }

#if defined(DEBUG_ESP_HWDT_DEV_DEBUG)
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
 * When g_pcont is valid, we expect these checks to be valid. I am not sure
 * what to do when they are not. An error that could lead to a crash is
 * corrected. We currently continue and print the stack dump. This assumes
 * something is better than nothing.
 *
 * Make global so postmortem can take advange of this check.
 */
uint32_t IRAM_MAYBE hwdt_cont_integrity_check() {
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
    if (g_pcont->struct_start != (unsigned*)g_pcont) {
      cont_integrity |= 0x4000;
      g_pcont->struct_start = (unsigned*)g_pcont;
    }
    hwdt_info.cont_integrity = cont_integrity;
    return cont_integrity;
}
/*
 * Determine if we have a HWDT reboot and dump stack traces if so.
 */
STATIC void IRAM_MAYBE handle_hwdt(void)  __attribute__((used));
STATIC void IRAM_MAYBE handle_hwdt(void) {
#ifdef DEBUG_ESP_HWDT_NOEXTRA4K
    disable_extra4k_at_link_time();
#endif
    set__sys_stack_first();

    ets_memset(&hwdt_info, 0, sizeof(hwdt_info));
    hwdt_check_g_pcont_validity();

    bool power_on = false;
    bool hwdt_reset = false;
    get_reset_reason(&power_on, &hwdt_reset);

#ifdef DEBUG_ESP_HWDT_UART_SPEED
    const uint32_t uart_divisor = set_uart_speed(0, DEBUG_ESP_HWDT_UART_SPEED);
#endif
#if defined(DEBUG_ESP_HWDT_DEV_DEBUG)
    ETS_PRINTF("Basic boot reason: %s\n", (power_on) ? "Power-on" : "Reboot");
    ETS_PRINTF("RTC_SYS Reset Reason = %u\n", hwdt_info.rtc_sys_reason);
    ETS_PRINTF("ROM API Reset Reason = %u\n", hwdt_info.rom_api_reason);
    ETS_PRINTF("HWDT Reset Reason = %u\n\n", hwdt_info.reset_reason);
#endif
#if defined(DEBUG_ESP_HWDT_DEV_DEBUG_RESET_REASON)
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
     */
    if (!power_on && hwdt_info.g_pcont_valid) {
        // Checks and fixes incorrect cont_t structure values that might
        // otherwise cause us to crash.
        hwdt_cont_integrity_check();

        const uint32_t *ctx_cont_ptr = NULL;
#if !defined(DEBUG_ESP_HWDT_INFO)
        if (get_noextra4k_g_pcont())
#endif
        {
            ctx_cont_ptr = skip_stackguard(g_pcont->stack, g_pcont->stack_end, CONT_STACKGUARD);
            hwdt_info.cont = (uintptr_t)g_pcont->stack_end - (uintptr_t)ctx_cont_ptr;
        }

        const uint32_t *ctx_sys_ptr = skip_stackguard(SYS_STACK, ROM_STACK, CONT_STACKGUARD);
        hwdt_info.sys = (uintptr_t)ROM_STACK - (uintptr_t)ctx_sys_ptr;

#ifndef USE_IRAM
        const uint32_t *bearssl_stack_top = NULL;
        const uint32_t *ctx_bearssl_ptr = NULL;
        if (stack_thunk_get_refcnt()) {
            bearssl_stack_top = (const uint32_t *)stack_thunk_get_stack_top();
            ctx_bearssl_ptr = skip_stackguard((const uint32_t *)stack_thunk_get_stack_bot(), bearssl_stack_top, 0xdeadbeef);
            hwdt_info.bearssl = (uintptr_t)bearssl_stack_top - (uintptr_t)ctx_bearssl_ptr;
        }
#endif

        if (hwdt_reset) {
            ETS_PRINTF("\n\nHardware WDT reset\n");
#ifndef USE_IRAM
            if (bearssl_stack_top) {
                /* Print context bearssl */
                print_stack((uintptr_t)ctx_bearssl_ptr, (uintptr_t)bearssl_stack_top, PRINT_STACK::BEARSSL);
            }
#endif
            /* Print context SYS */
            print_stack((uintptr_t)ctx_sys_ptr, (uintptr_t)ROM_STACK, PRINT_STACK::SYS);
            if (get_noextra4k_g_pcont()) {
                /* Print separate ctx: cont stack */

                /* Check if cont stack is yielding to SYS */
                if (0 == hwdt_info.cont_integrity && 0 != g_pcont->pc_suspend) {
                    ctx_cont_ptr = (const uint32_t *)((uintptr_t)g_pcont->sp_suspend - 8u);
                }
                print_stack((uintptr_t)ctx_cont_ptr, (uintptr_t)g_pcont->stack_end, PRINT_STACK::CONT);
            } else {
                if (0 == hwdt_info.cont_integrity && 0 != g_pcont->pc_suspend) {
                    ETS_PRINTF("\nCont stack is yielding. Active stack starts at 0x%08X.\n", (uint32_t)g_pcont->sp_suspend - 8u);
                }
            }

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
        size_t this_mutch = (uintptr_t)ROM_STACK - (uintptr_t)SYS_STACK;
        this_mutch /= sizeof(uint32_t);
        for (size_t i = 0; i < this_mutch; i++) {
            SYS_STACK[i] = CONT_STACKGUARD;
        }
    }

#if defined(DEBUG_ESP_HWDT_INFO) || defined(ROM_STACK_DUMP)
    /*
     *  Reports on ROM_STACK usage by ROM and eboot.
     *  Used to confirm DEBUG_ESP_HWDT_ROM_STACK_SIZE is large enough.
     */
    {
        const uint32_t *ctx_rom_ptr = skip_stackguard(ROM_STACK, ROM_STACK_FIRST, CONT_STACKGUARD);
        hwdt_info.rom = (uintptr_t)ROM_STACK_FIRST - (uintptr_t)ctx_rom_ptr;
#if defined(ROM_STACK_DUMP)
        print_stack((uintptr_t)ctx_rom_ptr, (uintptr_t)ROM_STACK_FIRST, PRINT_STACK::ROM);
#endif
    }
#endif

#if defined(DEBUG_ESP_HWDT_PRINT_GREETING)
    ETS_PRINTF("\n\nHardware WDT Stack Dump - enabled\n\n");
#else
    ETS_PRINTF("\n\n");
#endif

#ifdef DEBUG_ESP_HWDT_UART_SPEED
    if (uart_divisor) {
        adjust_uart_speed(uart_divisor);
    }
#endif
}

#if defined(DEBUG_ESP_HWDT_DEV_DEBUG) && !defined(USE_IRAM)
static void printSanityCheck() {
  ETS_PRINTF("\n\nsys_stack_first:         %p\n", sys_stack_first);
  ETS_PRINTF(    "CONT_STACK:              %p\n", CONT_STACK);
  ETS_PRINTF(    "g_pcont:                 %p\n", g_pcont);
  ETS_PRINTF(    "ROM_STACK:               %p\n", ROM_STACK);
  ETS_PRINTF(    "get_noextra4k_g_pcont(): %p\n", get_noextra4k_g_pcont());
  ETS_PRINTF(    "g_rom_stack:             %p\n", g_rom_stack);
  ETS_PRINTF(    "g_rom_stack_A16_sz:      0x%08X\n\n", g_rom_stack_A16_sz);
}
#endif //DEBUG_ESP_HWDT_DEV_DEBUG

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
 *
 * The logic to use Cache_Read_Enable and Cache_Read_Disable has been
 * generalized into a wrapper function, mmu_wrap_irom_fn, and moved to
 * mmu_iram.cpp.
 */

/*
   hwdt_pre_sdk_init() is the result of a hook for development diagnotics which
   evolved and was generlized to run any optional diagnostic code supplied at
   link time.

   Summary of the hwdt_pre_sdk_init() runtime environment:
    * The code can run from flash and use PROGMEM strings.
    * All functions must be extern "C" type
    * C/C++ runtime has not started. Structures have not been initialized and
      should have the values prior to reboot. With the exception of hwdt_info,
      which was updated before this call.
    * You can reference hwdt_info.reset_reason to control the action of the diagnostic.
    * The stack is on the SYS stack. You have about 3K available before you
      overwrite ROM Data area.
    * Printing will work best with ets_uart_printf and umm_info_safe_printf_P.
 */
void hwdt_pre_sdk_init(void) __attribute__((weak));
void hwdt_pre_sdk_init(void) {
#if defined(DEBUG_ESP_HWDT_DEV_DEBUG) && !defined(USE_IRAM)
  printSanityCheck();
#endif
}

static void __attribute__((noinline)) hwdt_pre_sdk_init_icache(void) __attribute__((used));
void hwdt_pre_sdk_init_icache(void) {
#ifdef DEBUG_ESP_HWDT_UART_SPEED
  const uint32_t uart_divisor = set_uart_speed(0, DEBUG_ESP_HWDT_UART_SPEED);
#endif

  hwdt_pre_sdk_init();

#ifdef DEBUG_ESP_HWDT_UART_SPEED
  if (uart_divisor) {
      adjust_uart_speed(uart_divisor);
  }
#endif
}

/*
  For app_entry_redefinable, use Basic ASM instead of "C" with Extended ASM. The
  (inline) Extended ASM approach required constant inspection to verify that the
  compiler's optimizer did not clobber needed registers or do something weird
  after minor changes in code or compiler updates. Also, I think Basic ASM is
  the safer route when changing the stack pointer multiple times.
*/
cont_t *hwdt_app_entry__cont_stack __attribute__((used)) = CONT_STACK;

asm  (
    ".section        .iram.text.hwdt_app_entry.cpp,\"ax\",@progbits\n\t"
    ".literal_position\n\t"
    ".literal .g_pcont, g_pcont\n\t"
    ".literal .pcont_stack, hwdt_app_entry__cont_stack\n\t"
    ".literal .sys_stack_first, sys_stack_first\n\t"
    ".literal .umm_init, umm_init\n\t"
    ".literal .call_user_start, call_user_start\n\t"
    ".literal .get_noextra4k_g_pcont, get_noextra4k_g_pcont\n\t"
    ".literal .mmu_wrap_irom_fn, mmu_wrap_irom_fn\n\t"
    ".align  4\n\t"
    ".global app_entry_redefinable\n\t"
    ".type   app_entry_redefinable, @function\n\t"
    "\n"
"app_entry_redefinable:\n\t"
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
    "movi    a1, 0x3fffeb30\n\t"
#ifdef USE_IRAM
    "call0   handle_hwdt\n\t"
#else
    "l32r    a0, .mmu_wrap_irom_fn\n\t"
    "movi    a2, handle_hwdt\n\t"
    "callx0  a0\n\t"
#endif
    /*
     *  Use new calculated SYS stack from top.
     *  Call the entry point of the SDK code.
     */
    "l32r    a2, .sys_stack_first\n\t"
    /*
     * Stack cases:
     *
     *  1) Continuation context is in BSS. (noextra4k)
     *     g_pcont = get_noextra4k_g_pcont(); was &g_cont;
     *
     *  2) The continuation context is on the stack just after the reserved
     *     space for the ROM/eboot stack and before the SYS stack begins.
     *     All computations were done at top, save pointer to it now.
     *     g_pcont = CONT_STACK;
     */
    "l32r    a13, .pcont_stack\n\t"
    "l32r     a0, .get_noextra4k_g_pcont\n\t"
    "l32r    a14, .g_pcont\n\t"
    // We now switch to the SYS stack the SDK will use
    "l32i.n   a1,  a2, 0\n\t"          // delayed load for pipeline
    "l32i.n  a13, a13, 0\n\t"
    "callx0   a0\n\t"
    "moveqz   a2, a13, a2\n\t"
    "s32i.n   a2, a14, 0\n\t"

    /*
     * Allow for running additional diagnotics supplied at link time.
     */
    "l32r     a0, .mmu_wrap_irom_fn\n\t"
    "movi     a2, hwdt_pre_sdk_init_icache\n\t"
    "callx0   a0\n\t"

    // In case somebody cares, leave things as we found them
    // - Restore ROM BSS zeros.
    "movi     a2, 0x3FFFE000\n\t" // ROM BSS Area
    "movi     a3, 0x0b30\n\t"     // ROM BSS Size
    "call0    ets_bzero\n\t"

    /*
     * Up until this call, the heap at crash time has been available for
     * analysis. This is needed for dumping the bearssl stack. Also, future
     * improvements could possibly use hwdt_pre_sdk_init() to run other early
     * diagnostic tools.
     */
#ifdef UMM_INIT_USE_ICACHE
    "l32r     a0, .mmu_wrap_irom_fn\n\t"
    "l32r     a2, .umm_init\n\t"
#else
    "l32r     a0, .umm_init\n\t"
#endif
    "callx0   a0\n\t"

    "l32r     a3, .call_user_start\n\t"
    "movi     a0, 0x4000044c\n\t"
    "jx       a3\n\t"
    ".size app_entry_redefinable, .-app_entry_redefinable\n\t"
);

#if defined(DEBUG_ESP_HWDT_INFO) || defined(ROM_STACK_DUMP)
void debug_hwdt_init(void) {
    /*
     * Fill the ROM_STACK while it is not actively being used.
     *
     * I am thinking that during the time the sketch is running this block of
     * memory could be used for a scratch buffer.
     */
    for (size_t i = 0; i < g_rom_stack_A16_sz/sizeof(uint32_t); i++) {
        g_rom_stack[i] = CONT_STACKGUARD;
    }
}

#else
void debug_hwdt_init(void) {
}
#endif

};

#endif // end of #if defined(DEBUG_ESP_HWDT) || defined(DEBUG_ESP_HWDT_NOEXTRA4K)
