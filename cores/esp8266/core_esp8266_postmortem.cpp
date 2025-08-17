/*
 postmortem.c - output of debug info on sketch crash
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"
#include "ets_sys.h"
#include "user_interface.h"
#include "esp8266_peri.h"
#include "cont.h"
#include "pgmspace.h"
#include "gdb_hooks.h"
#include "StackThunk.h"
#include "coredecls.h"
#include "umm_malloc/umm_malloc.h"

extern "C" {

// using numbers different from "REASON_" in user_interface.h (=0..6)
enum rst_reason_sw
{
    REASON_USER_STACK_OVERFLOW = 252,
    REASON_USER_STACK_SMASH = 253,
    REASON_USER_SWEXCEPTION_RST = 254
};

// Confirmed on 12/17/22: s_pm is in the .bss section and is in the
// _bss_start/end range to be zeroed by the SDK this happens after the SDK first
// calls to Cache_Read_Enable_New.
static struct PostmortemInfo {
    int user_reset_reason = REASON_DEFAULT_RST;

    // These will be pointers to PROGMEM const strings
    const char* panic_file = 0;
    int panic_line = 0;
    const char* panic_func = 0;
    const char* panic_what = 0;

    // Our wiring for abort() and C++ exceptions
    bool abort_called = false;
    const char* unhandled_exception = NULL;

    // Common way to notify about where the stack smashing happened
    // (but, **only** if caller uses our handler function)
    uint32_t stack_chk_addr = 0;
} s_pm;

// From UMM, the last caller of a malloc/realloc/calloc which failed:
extern struct umm_last_fail_alloc {
    const void *addr;
    size_t size;
#if defined(DEBUG_ESP_OOM)
    const char *file;
    int line;
#endif
} _umm_last_fail_alloc;


void abort() __attribute__((noreturn));
static void uart_write_char_d(char c);
static void uart0_write_char_d(char c);
static void uart1_write_char_d(char c);
static void print_stack(uint32_t start, uint32_t end);

static void raise_exception() __attribute__((noreturn));

extern void __custom_crash_callback( struct rst_info * rst_info, uint32_t stack, uint32_t stack_end ) {
    (void) rst_info;
    (void) stack;
    (void) stack_end;
}

extern void custom_crash_callback( struct rst_info * rst_info, uint32_t stack, uint32_t stack_end ) __attribute__ ((weak, alias("__custom_crash_callback")));


// Prints need to use our library function to allow for file and function
// to be safely accessed from flash. This function encapsulates snprintf()
// [which by definition will 0-terminate] and dumping to the UART
static void ets_printf_P(const char *str, ...) {
    char destStr[160];
    char *c = destStr;
    va_list argPtr;
    va_start(argPtr, str);
    vsnprintf(destStr, sizeof(destStr), str, argPtr);
    va_end(argPtr);
    while (*c) {
        ets_uart_putc1(*(c++));
    }
}

static void cut_here() {
    // https://tinyurl.com/8266dcdr => https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#exception
    ets_printf_P(PSTR("\nTo make this dump useful, DECODE IT - https://tinyurl.com/8266dcdr\n"));

    for (auto i = 0; i < 15; i++ ) {
        ets_putc('-');
    }
    ets_printf_P(PSTR(" CUT HERE FOR EXCEPTION DECODER "));
    for (auto i = 0; i < 15; i++ ) {
        ets_putc('-');
    }
    ets_putc('\n');
}

static inline bool is_pc_valid(uint32_t pc) {
    return pc >= XCHAL_INSTRAM0_VADDR && pc < (XCHAL_INSTROM0_VADDR + XCHAL_INSTROM0_SIZE);
}

/*
  Add some assembly to grab the stack pointer and pass it as an argument before
  it grows for the target function. Should stabilize the stack offsets, used to
  find the relevant stack content for dumping.
*/
extern "C" void __wrap_system_restart_local(void);
asm(
    ".section     .text.__wrap_system_restart_local,\"ax\",@progbits\n\t"
    ".literal_position\n\t"
    ".align       4\n\t"
    ".global      __wrap_system_restart_local\n\t"
    ".type        __wrap_system_restart_local, @function\n\t"
    "\n"
"__wrap_system_restart_local:\n\t"
    "mov          a2,     a1\n\t"
    "j.l          postmortem_report, a3\n\t"
    ".size __wrap_system_restart_local, .-__wrap_system_restart_local\n\t"
);

static void postmortem_report(uint32_t sp_dump) {
    struct rst_info rst_info;
    memset(&rst_info, 0, sizeof(rst_info));
    if (s_pm.user_reset_reason == REASON_DEFAULT_RST)
    {
        system_rtc_mem_read(0, &rst_info, sizeof(rst_info));
        if (rst_info.reason != REASON_SOFT_WDT_RST &&
            rst_info.reason != REASON_EXCEPTION_RST &&
            rst_info.reason != REASON_WDT_RST)
        {
            rst_info.reason = REASON_DEFAULT_RST;
        }
    }
    else
        rst_info.reason = s_pm.user_reset_reason;

    ets_install_putc1(&uart_write_char_d);

    cut_here();

    if (s_pm.panic_line) {
        ets_printf_P(PSTR("\nPanic %s:%d %s"), s_pm.panic_file, s_pm.panic_line, s_pm.panic_func);
        if (s_pm.panic_what) {
            ets_printf_P(PSTR(": Assertion '%s' failed."), s_pm.panic_what);
        }
        ets_putc('\n');
    }
    else if (s_pm.panic_file) {
        ets_printf_P(PSTR("\nPanic %s\n"), s_pm.panic_file);
    }
    else if (s_pm.unhandled_exception) {
        ets_printf_P(PSTR("\nUnhandled C++ exception: %s\n"), s_pm.unhandled_exception);
    }
    else if (s_pm.abort_called) {
        ets_printf_P(PSTR("\nAbort called\n"));
    }
    else if (rst_info.reason == REASON_EXCEPTION_RST) {
        // The GCC divide routine in ROM jumps to the address below and executes ILL (00 00 00) on div-by-zero
        // In that case, print the exception as (6) which is IntegerDivZero
        uint32_t epc1 = rst_info.epc1;
        uint32_t exccause = rst_info.exccause;
        bool div_zero = (exccause == 0) && (epc1 == 0x4000dce5u);
        if (div_zero) {
            exccause = 6;
            // In place of the detached 'ILL' instruction., redirect attention
            // back to the code that called the ROM divide function.
            __asm__ __volatile__("rsr.excsave1 %0\n\t" : "=r"(epc1) :: "memory");
        }
        ets_printf_P(PSTR("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"),
            exccause, epc1, rst_info.epc2, rst_info.epc3, rst_info.excvaddr, rst_info.depc);
    }
    else if (rst_info.reason == REASON_SOFT_WDT_RST) {
        ets_printf_P(PSTR("\nSoft WDT reset"));
        const uint8_t infinite_loop[] = { 0x06, 0xff, 0xff };  // loop: j loop
        if (is_pc_valid(rst_info.epc1) && 0 == memcmp_P(infinite_loop, (PGM_VOID_P)rst_info.epc1, 3u)) {
            // The SDK is riddled with these. They are usually preceded by an ets_printf.
            ets_printf_P(PSTR(" - deliberate infinite loop detected"));
        }
        ets_putc('\n');
        ets_printf_P(PSTR("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"),
            rst_info.exccause, /* Address executing at time of Soft WDT level-1 interrupt */ rst_info.epc1, 0, 0, 0, 0);
    }
    else if (rst_info.reason == REASON_USER_STACK_SMASH) {
        ets_printf_P(PSTR("\nStack smashing detected at 0x%08x\n"), s_pm.stack_chk_addr);
    }
    else if (rst_info.reason == REASON_USER_STACK_OVERFLOW) {
        ets_printf_P(PSTR("\nStack overflow detected\n"));
    }
    else {
        ets_printf_P(PSTR("\nGeneric Reset\n"));
    }

    uint32_t cont_stack_start;
    if (rst_info.reason == REASON_USER_STACK_SMASH) {
        cont_stack_start = s_pm.stack_chk_addr;
    } else {
        cont_stack_start = (uint32_t) (&g_pcont->stack[0]);
    }

    uint32_t cont_stack_end = cont_stack_start + CONT_STACKSIZE;

    // amount of stack taken by interrupt or exception handler
    // and everything up to __wrap_system_restart_local
    // ~(determined empirically, might break)~
    uint32_t offset = 0;
    if (rst_info.reason == REASON_SOFT_WDT_RST) {
        // Stack Tally
        // 256 User Exception vector handler reserves stack space
        //     directed to _xtos_l1int_handler function in Boot ROM
        //  48 wDev_ProcessFiq - its address appears in a vector table at 0x3FFFC27C
        //  16 ?unnamed? - index into a table, pull out pointer, and call if non-zero
        //     appears near near wDev_ProcessFiq
        //  32 pp_soft_wdt_feed_local - gather the specifics and call __wrap_system_restart_local
        offset = 32 + 16 + 48 + 256;
    }
    else if (rst_info.reason == REASON_EXCEPTION_RST) {
        // Stack Tally
        // 256 Exception vector reserves stack space
        //     filled in by "C" wrapper handler
        //  16 Handler level 1 - enable icache
        //  64 Handler level 2 - exception report
        offset = 64 + 16 + 256;
    }
    else if (rst_info.reason == REASON_WDT_RST) {
        offset = 16;
    }
    else if (rst_info.reason == REASON_USER_SWEXCEPTION_RST) {
        offset = 16;
    }

    ets_printf_P(PSTR("\n>>>stack>>>\n"));

    if (sp_dump > stack_thunk_get_stack_bot() && sp_dump <= stack_thunk_get_stack_top()) {
        // BearSSL we dump the BSSL second stack and then reset SP back to the main cont stack
        ets_printf_P(PSTR("\nctx: bearssl\nsp: %08x end: %08x offset: %04x\n"), sp_dump, stack_thunk_get_stack_top(), offset);
        print_stack(sp_dump + offset, stack_thunk_get_stack_top());
        offset = 0; // No offset needed anymore, the exception info was stored in the bssl stack
        sp_dump = stack_thunk_get_cont_sp();
    }

    uint32_t stack_end;

    // above and inside of cont, dump from the sp to the bottom of the stack
    if ((rst_info.reason == REASON_USER_STACK_OVERFLOW)
     || ((sp_dump > cont_stack_start) && (sp_dump < cont_stack_end)))
    {
        ets_printf_P(PSTR("\nctx: cont\n"));
        stack_end = cont_stack_end;
    }
    // in system, reposition to a known address
    // it's actually 0x3ffffff0, but the stuff below ets_run
    // is likely not really relevant to the crash
    else {
        ets_printf_P(PSTR("\nctx: sys\n"));
        stack_end = 0x3fffffb0;
    }

    ets_printf_P(PSTR("sp: %08x end: %08x offset: %04x\n"), sp_dump, stack_end, offset);

    print_stack(sp_dump + offset, stack_end);

    ets_printf_P(PSTR("<<<stack<<<\n"));

    // Use cap-X formatting to ensure the standard EspExceptionDecoder doesn't match the address
    if (_umm_last_fail_alloc.addr) {
#if defined(DEBUG_ESP_OOM)
        ets_printf_P(PSTR("\nlast failed alloc call: 0x%08X(%d), File: %s:%d\n"),
            (uint32_t)_umm_last_fail_alloc.addr,
            _umm_last_fail_alloc.size,
            (_umm_last_fail_alloc.file) ? _umm_last_fail_alloc.file : PSTR("??"),
            _umm_last_fail_alloc.line);
#else
        ets_printf_P(PSTR("\nlast failed alloc call: %08X(%d)\n"), (uint32_t)_umm_last_fail_alloc.addr, _umm_last_fail_alloc.size);
#endif
    }

    cut_here();

    if (s_pm.unhandled_exception && _umm_last_fail_alloc.addr) {
        // now outside from the "cut-here" zone, print correctly the `new` caller address,
        // idf-monitor.py will be able to decode this one and show exact location in sources
        ets_printf_P(PSTR("\nlast failed alloc caller: 0x%08x\n"), (uint32_t)_umm_last_fail_alloc.addr);
    }

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
    size_t oom_count = umm_get_oom_count();
    if (oom_count) {
        ets_printf_P(PSTR("\nOOM Count: %u\n"), oom_count);
    }
#endif

    custom_crash_callback( &rst_info, sp_dump + offset, stack_end );

    ets_delay_us(10000);
    __real_system_restart_local();
}


static void print_stack(uint32_t start, uint32_t end) {
    for (uint32_t pos = start; pos < end; pos += 0x10) {
        uint32_t* values = (uint32_t*)(pos);

        // avoid printing irrelevant data
        if ((values[0] == CONT_STACKGUARD)
         && (values[0] == values[1])
         && (values[1] == values[2])
         && (values[2] == values[3]))
        {
            continue;
        }

        // rough indicator: stack frames usually have SP saved as the second word
        const bool looksLikeStackFrame = (values[2] == pos + 0x10);

        ets_printf_P(PSTR("%08x:  %08x %08x %08x %08x %c\n"),
            pos, values[0], values[1], values[2], values[3], (looksLikeStackFrame) ? '<' : ' ');
    }
}

static void IRAM_ATTR uart_write_char_d(char c) {
    uart0_write_char_d(c);
    uart1_write_char_d(c);
}

static void IRAM_ATTR uart0_write_char_d(char c) {
    while (((USS(0) >> USTXC) & 0xff)) { }

    if (c == '\n') {
        USF(0) = '\r';
    }
    USF(0) = c;
}

static void IRAM_ATTR uart1_write_char_d(char c) {
    while (((USS(1) >> USTXC) & 0xff) >= 0x7e) { }

    if (c == '\n') {
        USF(1) = '\r';
    }
    USF(1) = c;
}

static void raise_exception() {
    if (gdb_present())
        __asm__ __volatile__ ("syscall"); // triggers GDB when enabled

    s_pm.user_reset_reason = REASON_USER_SWEXCEPTION_RST;
    ets_printf_P(PSTR("\nUser exception (panic/abort/assert)"));
    uint32_t sp;
    __asm__ __volatile__ ("mov %0, a1\n\t" : "=r"(sp) :: "memory");
    postmortem_report(sp);
    while (1); // never reached, needed to satisfy "noreturn" attribute
}

void abort() {
    s_pm.abort_called = true;
    raise_exception();
}

void __unhandled_exception(const char *str) {
    s_pm.unhandled_exception = str;
    raise_exception();
}

void __assert_func(const char *file, int line, const char *func, const char *what) {
    s_pm.panic_file = file;
    s_pm.panic_line = line;
    s_pm.panic_func = func;
    s_pm.panic_what = what;
    gdb_do_break();     /* if GDB is not present, this is a no-op */
    raise_exception();
}

void __panic_func(const char* file, int line, const char* func) {
    s_pm.panic_file = file;
    s_pm.panic_line = line;
    s_pm.panic_func = func;
    s_pm.panic_what = 0;
    gdb_do_break();     /* if GDB is not present, this is a no-op */
    raise_exception();
}

uintptr_t __stack_chk_guard = 0x08675309 ^ RANDOM_REG32;
void __stack_chk_fail(void) {
    s_pm.user_reset_reason = REASON_USER_STACK_SMASH;
    s_pm.stack_chk_addr = (uint32_t)__builtin_return_address(0);

    if (gdb_present())
        __asm__ __volatile__ ("syscall"); // triggers GDB when enabled

    uint32_t sp;
    __asm__ __volatile__ ("mov %0, a1\n\t" : "=r"(sp) :: "memory");
    postmortem_report(sp);

    __builtin_unreachable(); // never reached, needed to satisfy "noreturn" attribute
}

void __stack_overflow(cont_t* cont, uint32_t* sp) {
    s_pm.user_reset_reason = REASON_USER_STACK_OVERFLOW;
    s_pm.stack_chk_addr = (uint32_t)&cont->stack[0];

    if (gdb_present())
        __asm__ __volatile__ ("syscall"); // triggers GDB when enabled

    postmortem_report((uint32_t)sp);

    __builtin_unreachable(); // never reached, needed to satisfy "noreturn" attribute
}

} // extern "C"
