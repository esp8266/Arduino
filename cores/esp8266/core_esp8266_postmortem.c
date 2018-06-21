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
#include "debug.h"
#include "ets_sys.h"
#include "user_interface.h"
#include "esp8266_peri.h"
#include "cont.h"
#include "pgmspace.h"
#include "gdb_hooks.h"

extern void __real_system_restart_local();

extern cont_t* g_pcont;

// These will be pointers to PROGMEM const strings
static const char* s_panic_file = 0;
static int s_panic_line = 0;
static const char* s_panic_func = 0;
static const char* s_panic_what = 0;

static bool s_abort_called = false;

void abort() __attribute__((noreturn));
static void uart_write_char_d(char c);
static void uart0_write_char_d(char c);
static void uart1_write_char_d(char c);
static void print_stack(uint32_t start, uint32_t end);

// From UMM, the last caller of a malloc/realloc/calloc which failed:
extern void *umm_last_fail_alloc_addr;
extern int umm_last_fail_alloc_size;

static void raise_exception() __attribute__((noreturn));

extern void __custom_crash_callback( struct rst_info * rst_info, uint32_t stack, uint32_t stack_end ) {
    (void) rst_info;
    (void) stack;
    (void) stack_end;
}

extern void custom_crash_callback( struct rst_info * rst_info, uint32_t stack, uint32_t stack_end ) __attribute__ ((weak, alias("__custom_crash_callback")));

// Single, non-inlined copy of pgm_read_byte to save IRAM space (as this is not timing critical)
static char ICACHE_RAM_ATTR iram_read_byte (const char *addr) {
    return pgm_read_byte(addr);
}

// Required to output the s_panic_file, it's stored in PMEM
#define ets_puts_P(pstr) \
{ \
    char c; \
    do { \
        c = iram_read_byte(pstr++); \
        if (c) ets_putc(c); \
    } while (c); \
}

// Place these strings in .text because the SPI interface may be in bad shape during an exception.
#define ets_printf_P(str, ...) \
{ \
    static const char istr[] ICACHE_RAM_ATTR = (str); \
    char mstr[sizeof(str)]; \
    for (size_t i=0; i < sizeof(str); i++) mstr[i] = iram_read_byte(&istr[i]); \
    ets_printf(mstr, ##__VA_ARGS__); \
}

void __wrap_system_restart_local() {
    register uint32_t sp asm("a1");

    if (gdb_present()) {
        /* When GDBStub is present, exceptions are handled by GDBStub,
           but Soft WDT will still call this function.
           Trigger an exception to break into GDB.
           TODO: check why gdb_do_break() or asm("break.n 0") do not
           break into GDB here. */
        raise_exception();
    }

    struct rst_info rst_info = {0};
    system_rtc_mem_read(0, &rst_info, sizeof(rst_info));
    if (rst_info.reason != REASON_SOFT_WDT_RST &&
        rst_info.reason != REASON_EXCEPTION_RST &&
        rst_info.reason != REASON_WDT_RST)
    {
        return;
    }

    ets_install_putc1(&uart_write_char_d);

    if (s_panic_line) {
        ets_printf_P("\nPanic ");
        ets_puts_P(s_panic_file); // This is in PROGMEM, need special output because ets_printf can't handle ROM parameters
        ets_printf_P(":%d %s", s_panic_line, s_panic_func);
        if (s_panic_what) {
            ets_printf_P(": Assertion '");
            ets_puts_P(s_panic_what); // This is also in PMEM
            ets_printf_P("' failed.");
        }
        ets_putc('\n');
    }
    else if (s_abort_called) {
        ets_printf_P("\nAbort called\n");
    }
    else if (rst_info.reason == REASON_EXCEPTION_RST) {
        ets_printf_P("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n",
            rst_info.exccause, rst_info.epc1, rst_info.epc2, rst_info.epc3, rst_info.excvaddr, rst_info.depc);
    }
    else if (rst_info.reason == REASON_SOFT_WDT_RST) {
        ets_printf_P("\nSoft WDT reset\n");
    }

    uint32_t cont_stack_start = (uint32_t) &(g_pcont->stack);
    uint32_t cont_stack_end = (uint32_t) g_pcont->stack_end;
    uint32_t stack_end;

    // amount of stack taken by interrupt or exception handler
    // and everything up to __wrap_system_restart_local
    // (determined empirically, might break)
    uint32_t offset = 0;
    if (rst_info.reason == REASON_SOFT_WDT_RST) {
        offset = 0x1b0;
    }
    else if (rst_info.reason == REASON_EXCEPTION_RST) {
        offset = 0x1a0;
    }
    else if (rst_info.reason == REASON_WDT_RST) {
        offset = 0x10;
    }

    if (sp > cont_stack_start && sp < cont_stack_end) {
        ets_printf_P("\nctx: cont \n");
        stack_end = cont_stack_end;
    }
    else {
        ets_printf_P("\nctx: sys \n");
        stack_end = 0x3fffffb0;
        // it's actually 0x3ffffff0, but the stuff below ets_run
        // is likely not really relevant to the crash
    }

    ets_printf_P("sp: %08x end: %08x offset: %04x\n", sp, stack_end, offset);

    print_stack(sp + offset, stack_end);

    // Use cap-X formatting to ensure the standard EspExceptionDecoder doesn't match the address
    if (umm_last_fail_alloc_addr) {
      ets_printf("\nlast failed alloc call: %08X(%d)\n", (uint32_t)umm_last_fail_alloc_addr, umm_last_fail_alloc_size);
    }

    custom_crash_callback( &rst_info, sp + offset, stack_end );

    delayMicroseconds(10000);
    __real_system_restart_local();
}


static void ICACHE_RAM_ATTR print_stack(uint32_t start, uint32_t end) {
    ets_printf_P("\n>>>stack>>>\n");
    for (uint32_t pos = start; pos < end; pos += 0x10) {
        uint32_t* values = (uint32_t*)(pos);

        // rough indicator: stack frames usually have SP saved as the second word
        bool looksLikeStackFrame = (values[2] == pos + 0x10);

        ets_printf_P("%08x:  %08x %08x %08x %08x %c\n",
            pos, values[0], values[1], values[2], values[3], (looksLikeStackFrame)?'<':' ');
    }
    ets_printf_P("<<<stack<<<\n");
}

static void uart_write_char_d(char c) {
    uart0_write_char_d(c);
    uart1_write_char_d(c);
}

static void uart0_write_char_d(char c) {
    while (((USS(0) >> USTXC) & 0xff)) { }

    if (c == '\n') {
        USF(0) = '\r';
    }
    USF(0) = c;
}

static void uart1_write_char_d(char c) {
    while (((USS(1) >> USTXC) & 0xff) >= 0x7e) { }

    if (c == '\n') {
        USF(1) = '\r';
    }
    USF(1) = c;
}

static void raise_exception() {
    __asm__ __volatile__ ("syscall");
    while (1); // never reached, needed to satisfy "noreturn" attribute
}

void abort() {
    s_abort_called = true;
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

