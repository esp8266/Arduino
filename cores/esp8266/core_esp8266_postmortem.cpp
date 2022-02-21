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

extern "C" {

// These will be pointers to PROGMEM const strings
static const char* s_panic_file = 0;
static int s_panic_line = 0;
static const char* s_panic_func = 0;
static const char* s_panic_what = 0;

static bool s_abort_called = false;
static const char* s_unhandled_exception = NULL;

static uint32_t s_stacksmash_addr = 0;

void abort() __attribute__((noreturn));
static void uart_write_char_d(char c);
static void uart0_write_char_d(char c);
static void uart1_write_char_d(char c);
static void print_stack(uint32_t start, uint32_t end);

// using numbers different from "REASON_" in user_interface.h (=0..6)
enum rst_reason_sw
{
    REASON_USER_STACK_SMASH = 253,
    REASON_USER_SWEXCEPTION_RST = 254
};
static int s_user_reset_reason = REASON_DEFAULT_RST;

// From UMM, the last caller of a malloc/realloc/calloc which failed:
extern void *umm_last_fail_alloc_addr;
extern int umm_last_fail_alloc_size;
#if defined(DEBUG_ESP_OOM)
extern const char *umm_last_fail_alloc_file;
extern int umm_last_fail_alloc_line;
#endif

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
    ets_putc('\n');
    for (auto i = 0; i < 15; i++ ) {
        ets_putc('-');
    }
    ets_printf_P(PSTR(" CUT HERE FOR EXCEPTION DECODER "));
    for (auto i = 0; i < 15; i++ ) {
        ets_putc('-');
    }
    ets_putc('\n');
}

void __wrap_system_restart_local() {
    register uint32_t sp asm("a1");
    uint32_t sp_dump = sp;

    struct rst_info rst_info;
    memset(&rst_info, 0, sizeof(rst_info));
    if (s_user_reset_reason == REASON_DEFAULT_RST)
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
        rst_info.reason = s_user_reset_reason;

    ets_install_putc1(&uart_write_char_d);

    cut_here();

    if (s_panic_line) {
        ets_printf_P(PSTR("\nPanic %S:%d %S"), s_panic_file, s_panic_line, s_panic_func);
        if (s_panic_what) {
            ets_printf_P(PSTR(": Assertion '%S' failed."), s_panic_what);
        }
        ets_putc('\n');
    }
    else if (s_unhandled_exception) {
        ets_printf_P(PSTR("\nUnhandled C++ exception: %S\n"), s_unhandled_exception);
    }
    else if (s_abort_called) {
        ets_printf_P(PSTR("\nAbort called\n"));
    }
    else if (rst_info.reason == REASON_EXCEPTION_RST) {
        // The GCC divide routine in ROM jumps to the address below and executes ILL (00 00 00) on div-by-zero
        // In that case, print the exception as (6) which is IntegerDivZero
        bool div_zero = (rst_info.exccause == 0) && (rst_info.epc1 == 0x4000dce5);
        ets_printf_P(PSTR("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"),
            div_zero ? 6 : rst_info.exccause, rst_info.epc1, rst_info.epc2, rst_info.epc3, rst_info.excvaddr, rst_info.depc);
    }
    else if (rst_info.reason == REASON_SOFT_WDT_RST) {
        ets_printf_P(PSTR("\nSoft WDT reset\n"));
    }
    else if (rst_info.reason == REASON_USER_STACK_SMASH) {
        ets_printf_P(PSTR("\nStack overflow detected.\n"));
        ets_printf_P(PSTR("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"),
            5 /* Alloca exception, closest thing to stack fault*/, s_stacksmash_addr, 0, 0, 0, 0);
   }
    else {
        ets_printf_P(PSTR("\nGeneric Reset\n"));
    }

    uint32_t cont_stack_start = (uint32_t) &(g_pcont->stack);
    uint32_t cont_stack_end = (uint32_t) g_pcont->stack_end;
    uint32_t stack_end;

    // amount of stack taken by interrupt or exception handler
    // and everything up to __wrap_system_restart_local
    // (determined empirically, might break)
    uint32_t offset = 0;
    if (rst_info.reason == REASON_SOFT_WDT_RST) {
        offset = 0x1a0;
    }
    else if (rst_info.reason == REASON_EXCEPTION_RST) {
        offset = 0x190;
    }
    else if (rst_info.reason == REASON_WDT_RST) {
        offset = 0x10;
    }

    ets_printf_P(PSTR("\n>>>stack>>>\n"));

    if (sp_dump > stack_thunk_get_stack_bot() && sp_dump <= stack_thunk_get_stack_top()) {
        // BearSSL we dump the BSSL second stack and then reset SP back to the main cont stack
        ets_printf_P(PSTR("\nctx: bearssl\nsp: %08x end: %08x offset: %04x\n"), sp_dump, stack_thunk_get_stack_top(), offset);
        print_stack(sp_dump + offset, stack_thunk_get_stack_top());
        offset = 0; // No offset needed anymore, the exception info was stored in the bssl stack
        sp_dump = stack_thunk_get_cont_sp();
    }

    if (sp_dump > cont_stack_start && sp_dump < cont_stack_end) {
        ets_printf_P(PSTR("\nctx: cont\n"));
        stack_end = cont_stack_end;
    }
    else {
        ets_printf_P(PSTR("\nctx: sys\n"));
        stack_end = 0x3fffffb0;
        // it's actually 0x3ffffff0, but the stuff below ets_run
        // is likely not really relevant to the crash
    }

    ets_printf_P(PSTR("sp: %08x end: %08x offset: %04x\n"), sp_dump, stack_end, offset);

    print_stack(sp_dump + offset, stack_end);

    ets_printf_P(PSTR("<<<stack<<<\n"));

    // Use cap-X formatting to ensure the standard EspExceptionDecoder doesn't match the address
    if (umm_last_fail_alloc_addr) {
#if defined(DEBUG_ESP_OOM)
        ets_printf_P(PSTR("\nlast failed alloc call: %08X(%d)@%S:%d\n"),
            (uint32_t)umm_last_fail_alloc_addr, umm_last_fail_alloc_size,
            umm_last_fail_alloc_file, umm_last_fail_alloc_line);
#else
        ets_printf_P(PSTR("\nlast failed alloc call: %08X(%d)\n"), (uint32_t)umm_last_fail_alloc_addr, umm_last_fail_alloc_size);
#endif
    }

    cut_here();

    if (s_unhandled_exception && umm_last_fail_alloc_addr) {
        // now outside from the "cut-here" zone, print correctly the `new` caller address,
        // idf-monitor.py will be able to decode this one and show exact location in sources
        ets_printf_P(PSTR("\nlast failed alloc caller: 0x%08x\n"), (uint32_t)umm_last_fail_alloc_addr);
    }

    custom_crash_callback( &rst_info, sp_dump + offset, stack_end );

    ets_delay_us(10000);
    __real_system_restart_local();
}


static void print_stack(uint32_t start, uint32_t end) {
    for (uint32_t pos = start; pos < end; pos += 0x10) {
        uint32_t* values = (uint32_t*)(pos);

        // rough indicator: stack frames usually have SP saved as the second word
        bool looksLikeStackFrame = (values[2] == pos + 0x10);

        ets_printf_P(PSTR("%08x:  %08x %08x %08x %08x %c\n"),
            pos, values[0], values[1], values[2], values[3], (looksLikeStackFrame)?'<':' ');
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

    s_user_reset_reason = REASON_USER_SWEXCEPTION_RST;
    ets_printf_P(PSTR("\nUser exception (panic/abort/assert)"));
    __wrap_system_restart_local();

    while (1); // never reached, needed to satisfy "noreturn" attribute
}

void abort() {
    s_abort_called = true;
    raise_exception();
}

void __unhandled_exception(const char *str) {
    s_unhandled_exception = str;
    raise_exception();
}

void __assert_func(const char *file, int line, const char *func, const char *what) {
    s_panic_file = file;
    s_panic_line = line;
    s_panic_func = func;
    s_panic_what = what;
    gdb_do_break();     /* if GDB is not present, this is a no-op */
    raise_exception();
}

void __panic_func(const char* file, int line, const char* func) {
    s_panic_file = file;
    s_panic_line = line;
    s_panic_func = func;
    s_panic_what = 0;
    gdb_do_break();     /* if GDB is not present, this is a no-op */
    raise_exception();
}

uintptr_t __stack_chk_guard = 0x08675309 ^ RANDOM_REG32;
void __stack_chk_fail(void) {
    s_user_reset_reason = REASON_USER_STACK_SMASH;
    ets_printf_P(PSTR("\nPANIC: Stack overrun"));

    s_stacksmash_addr = (uint32_t)__builtin_return_address(0);

    if (gdb_present())
        __asm__ __volatile__ ("syscall"); // triggers GDB when enabled

    __wrap_system_restart_local();

    while (1); // never reached, needed to satisfy "noreturn" attribute
}


};
