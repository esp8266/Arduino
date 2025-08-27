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


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "core_esp8266_postmortem.h"
#include "debug.h"

#include "coredecls.h" // __real_system_restart_local()
#include "StackThunk.h" // stack_thunk_get_...()
#include "gdb_hooks.h" // gdb_present(), gdb_do_break()
#include "esp8266_undocumented.h" // ets_delay_us()
#include "esp8266_peri.h" // UART peripheral macros

#include <ets_sys.h> // ets_install_putc1(), ets_putc()

// From UMM, the last caller of a malloc/realloc/calloc which failed
extern void *umm_last_fail_alloc_addr;
extern int umm_last_fail_alloc_size;
#if defined(DEBUG_ESP_OOM)
extern const char *umm_last_fail_alloc_file;
extern int umm_last_fail_alloc_line;
#else
#define umm_last_fail_alloc_file nullptr
#define umm_last_fail_alloc_line 0
#endif

extern "C" bool system_rtc_mem_read(uint32_t, struct rst_info*, uint16_t);

namespace {

static int s_level = 0;

// Explicit panic()
static const char *s_panic_file = nullptr;
static const char *s_panic_func = nullptr;
static int s_panic_line = 0;

// Reason text is for both panic() and unhandled exceptions what()
static const char *s_what = nullptr;

// Common way to notify about where the stack smashing happened
// (but, **only** if caller uses our handler function)
static uint32_t s_stack_chk_addr = 0;

// Whenever custom function (e.g. raise_exception()) is invoked
// Note that this is also used for internal inderect use the variables declared above
static int s_user_reset_reason = REASON_DEFAULT_RST;

// ROM ets_printf *does not* support PSTR / flash-strings.
// Make sure to set up char output with `ets_install_putc1`!
void ets_printf_P(const char *str, ...) {
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

inline bool is_pc_valid(uint32_t pc) {
    return (pc >= XCHAL_INSTRAM0_VADDR)
        && (pc < (XCHAL_INSTROM0_VADDR + XCHAL_INSTROM0_SIZE));
}

inline bool is_div_zero(rst_info *rst_info) {
    return (rst_info->reason == REASON_EXCEPTION_RST)
        && (rst_info->exccause == 0)
        && (rst_info->epc1 == 0x4000dce5u); // within ROM __divsi3
}

void IRAM_ATTR uart0_write_char_d(char c) {
    while (((USS(0) >> USTXC) & 0xff)) { }

    if (c == '\n') {
        USF(0) = '\r';
    }
    USF(0) = c;
}

void IRAM_ATTR uart1_write_char_d(char c) {
    while (((USS(1) >> USTXC) & 0xff) >= 0x7e) { }

    if (c == '\n') {
        USF(1) = '\r';
    }
    USF(1) = c;
}

void crash_system_prepare_rst_info(rst_info *rst_info, uint32_t user_reset_reason) noexcept {
    auto *rtc_sys = reinterpret_cast<uint32_t *>(0x60001100ul);
    memset(rst_info, 0, sizeof(*rst_info));

    // at this point, check SDK that may have a prepared reset struct
    if (user_reset_reason == REASON_DEFAULT_RST)
    {
        __asm__ __volatile__("memw" ::: "memory");
        memcpy(rst_info, rtc_sys, sizeof(*rst_info));
        switch (rst_info->reason) {
        case REASON_SOFT_WDT_RST:
        case REASON_EXCEPTION_RST:
        case REASON_WDT_RST:
            break;

        default:
            rst_info->reason = REASON_DEFAULT_RST;
            break;
        }
    }
    else
    {
        rst_info->reason = user_reset_reason;
    }

    // GCC divide routine in ROM jumps to the address below and executes ILL (00 00 00) on div-by-zero
    // In that case, store & print the exception as (6) which is IntegerDivZero
    if (is_div_zero(rst_info))
    {
        rst_info->exccause = 6;
        __asm__ __volatile__(
            "rsr.excsave1 %0\n\t"
            : "=a"(rst_info->epc1) :: "memory"
        );
    }

    // make sure our modifications persist on reboot
    memcpy(rtc_sys, rst_info,
        sizeof(rst_info->reason) + sizeof(rst_info->exccause));
    __asm__ __volatile__("memw" ::: "memory");
}

} // namespace

extern "C" {

[[noreturn, gnu::used]]
static void crash_system_restart_local(uint32_t) noexcept;
static void crash_system_restart_local(uint32_t sp) noexcept {
    // note: prefer a separate entity from core_esp8266_main.cpp `resetInfo`,
    // since it *may* be used within the callback to compare values
    rst_info rst_info;

    // Allow to capture crashes in postmortem callbacks, but don't loop until wdt happens
    if (s_level < 2) {
        ++s_level;
        crash_system_prepare_rst_info(&rst_info, s_user_reset_reason);
        custom_crash_postmortem_report(&rst_info, sp);
    }

    ets_delay_us(10000);
    __real_system_restart_local();
}

/*
  Note that this does not work without  `-Wl,-wrap,system_restart_local` in build flags.

  Add some assembly to grab the stack pointer and pass it as an argument before
  it grows for the target function. Should stabilize the stack offsets, used to
  find the relevant stack content for dumping.
*/
[[noreturn]]
void __wrap_system_restart_local(void);
asm (
    ".section     .text.__wrap_system_restart_local,\"ax\",@progbits\n\t"
    ".literal_position\n\t"
    ".align       4\n\t"
    ".global      __wrap_system_restart_local\n\t"
    ".type        __wrap_system_restart_local, @function\n\t"
    "\n"
"__wrap_system_restart_local:\n\t"
    "mov          a2,     a1\n\t"
    "j.l          crash_system_restart_local, a3\n\t"
    ".size __wrap_system_restart_local, .-__wrap_system_restart_local\n\t"
);

static void crash_callback(rst_info*, uint32_t, uint32_t) noexcept {
}

extern void custom_crash_callback(rst_info*, uint32_t, uint32_t) noexcept
    __attribute__((weak, alias("crash_callback")));

static void postmortem_report(rst_info *rst_info, uint32_t sp_dump) noexcept {
    ets_install_putc1(&custom_crash_putc1);
    custom_crash_postmortem_pre();

    switch (rst_info->reason) {
    case REASON_USER_PANIC:
    case REASON_USER_ASSERT:
        custom_crash_postmortem_panic(s_panic_file, s_panic_line, s_panic_func, s_what);
        break;

    case REASON_USER_EXCEPTION:
        custom_crash_postmortem_unhandled_exception(s_what);
        break;

    case REASON_USER_ABORT:
        custom_crash_postmortem_abort();
        break;

    default:
        custom_crash_postmortem_reason(rst_info);
        break;
    }

    // amount of stack taken by interrupt or exception handler
    // and everything up to __wrap_system_restart_local
    // ~(determined empirically, might break)~
    uint32_t offset;

    switch (rst_info->reason) {
    case REASON_SOFT_WDT_RST:
        // Stack Tally
        // 256 User Exception vector handler reserves stack space
        //     directed to _xtos_l1int_handler function in Boot ROM
        //  48 wDev_ProcessFiq - its address appears in a vector table at 0x3FFFC27C
        //  16 ?unnamed? - index into a table, pull out pointer, and call if non-zero
        //     appears near near wDev_ProcessFiq
        //  32 pp_soft_wdt_feed_local - gather the specifics and call __wrap_system_restart_local
        offset =  32 + 16 + 48 + 256;
        break;

    case REASON_EXCEPTION_RST:
        // Stack Tally
        // 256 Exception vector reserves stack space
        //     filled in by "C" wrapper handler
        //  16 Handler level 1 - enable icache
        //  64 Handler level 2 - exception report
        offset = 64 + 16 + 256;
        break;

    case REASON_WDT_RST:
        offset = 16;
        break;

    case REASON_USER_ABORT:
    case REASON_USER_ASSERT:
    case REASON_USER_PANIC:
        // Stack Tally
        //   16 raise_exception()
        offset = 16;
        break;

    case REASON_USER_EXCEPTION:
#if __cpp_exceptions
        //  96 GCC unwind for a generic exception
        offset = 16 + 96;
#else
        offset = 16;
#endif
        break;

    default:
        offset = 0;
        break;
    }

    // ensure CONT output does not cross guard boundaries
    uint32_t cont_stack_start;
    if (rst_info->reason == REASON_USER_STACK_SMASH) {
        cont_stack_start = s_stack_chk_addr;
    } else {
        cont_stack_start = (uint32_t) (&g_pcont->stack[0]);
    }

    uint32_t cont_stack_end = cont_stack_start + CONT_STACKSIZE;

    user_crash_stack_t user_crash_stack[2];
    size_t stacks = 1;

    // check & process bearssl stack before everything else
    uint32_t stack_thunk_bot = stack_thunk_get_stack_bot();
    uint32_t stack_thunk_top = stack_thunk_get_stack_top();

    if (sp_dump > stack_thunk_bot && sp_dump <= stack_thunk_top) {
        user_crash_stack[1] = {
            .ctx = PSTR("bearssl"),
            .stack_start = sp_dump,
            .stack_end = stack_thunk_top,
            .offset = offset,
        };

        offset = 0; // No offset needed anymore, the exception info was stored in the bssl stack
        sp_dump = stack_thunk_get_cont_sp();

        ++stacks;
    }

    user_crash_stack[0].stack_start = sp_dump;
    user_crash_stack[0].offset = offset;

    // above and inside of cont, dump from the sp to the bottom of the stack
    if ((rst_info->reason == REASON_USER_STACK_OVERFLOW)
     || ((sp_dump > cont_stack_start) && (sp_dump <= cont_stack_end)))
    {
        user_crash_stack[0].ctx = PSTR("cont");
        user_crash_stack[0].stack_end = cont_stack_end;
    }
    // in system, reposition to a known address
    // it's actually 0x3ffffff0, but the stuff below ets_run
    // is likely not really relevant to the crash
    else {
        user_crash_stack[0].ctx = PSTR("sys");
        user_crash_stack[0].stack_end = 0x3fffffb0;
    }

    custom_crash_postmortem_stack(&user_crash_stack[0], stacks);
    custom_crash_postmortem_alloc(
        umm_last_fail_alloc_addr,
        umm_last_fail_alloc_size,
        umm_last_fail_alloc_file,
        umm_last_fail_alloc_line,
        s_what);

    custom_crash_postmortem_post();

    // TODO stack only makes sense in this function and only at this point
    custom_crash_callback(
        rst_info,
        user_crash_stack[0].stack_start + user_crash_stack[0].offset,
        user_crash_stack[0].stack_end);
}

extern void custom_crash_postmortem_report(rst_info *, uint32_t) noexcept
    __attribute__((weak, alias("postmortem_report")));

static void IRAM_ATTR uart_write_char_d(char c) noexcept;
static void IRAM_ATTR uart_write_char_d(char c) noexcept {
    uart0_write_char_d(c);
    uart1_write_char_d(c);
}

extern void custom_crash_putc1(char) noexcept
    __attribute__((weak, alias("uart_write_char_d")));

static void postmortem_panic(const char *file, int line, const char *func, const char *what) noexcept {
    if (line) {
        ets_printf_P(PSTR("\nPanic %s:%d %s"), file, line, func);
        if (what) {
            ets_printf_P(PSTR(": Assertion '%s' failed."), what);
        }
        ets_putc('\n');
    }
    else if (file) {
        ets_printf_P(PSTR("\nPanic %s\n"), file);
    }
}

extern void custom_crash_postmortem_panic(const char *, int, const char *, const char *) noexcept
    __attribute__((weak, alias("postmortem_panic")));

static void postmortem_abort() {
    ets_printf_P(PSTR("\nAbort called\n"));
}

extern void custom_crash_postmortem_abort() noexcept
    __attribute__((weak, alias("postmortem_abort")));

static void postmortem_unhandled_exception(const char *what) {
    ets_printf_P(PSTR("\nUnhandled C++ exception: %s\n"), what);
}

extern void custom_crash_postmortem_unhandled_exception(const char *) noexcept
    __attribute__((weak, alias("postmortem_unhandled_exception")));

static void postmortem_reason(rst_info *rst_info) noexcept {
    if (rst_info->reason == REASON_EXCEPTION_RST) {
        ets_printf_P(PSTR("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"),
            rst_info->exccause, rst_info->epc1, rst_info->epc2, rst_info->epc3, rst_info->excvaddr, rst_info->depc);
    }
    else if (rst_info->reason == REASON_SOFT_WDT_RST) {
        ets_printf_P(PSTR("\nSoft WDT reset"));
        const uint8_t infinite_loop[] = { 0x06, 0xff, 0xff };  // loop: j loop
        if (is_pc_valid(rst_info->epc1) && 0 == memcmp_P(infinite_loop, (void*)rst_info->epc1, 3u)) {
            // The SDK is riddled with these. They are usually preceded by an ets_printf.
            ets_printf_P(PSTR(" - deliberate infinite loop detected"));
        }
        ets_putc('\n');
        ets_printf_P(PSTR("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"),
            rst_info->exccause, /* Address executing at time of Soft WDT level-1 interrupt */ rst_info->epc1, 0, 0, 0, 0);
    }
    else if (rst_info->reason == REASON_USER_STACK_SMASH) {
        ets_printf_P(PSTR("\nStack smashing detected at 0x%08x\n"), s_stack_chk_addr);
    }
    else if (rst_info->reason == REASON_USER_STACK_OVERFLOW) {
        ets_printf_P(PSTR("\nStack overflow detected\n"));
    }
    else {
        ets_printf_P(PSTR("\nGeneric Reset\n"));
    }
}

extern void custom_crash_postmortem_reason(rst_info *) noexcept
    __attribute__((weak, alias("postmortem_reason")));

static void print_stacks(user_crash_stack_t *stacks, size_t size) noexcept {
    ets_printf_P(PSTR("\n>>>stack>>>\n"));

    for (size_t n = 0; n < size; ++n) {
        auto *stack = &stacks[n];
        ets_printf_P(PSTR("\nctx: %s\nsp: %08x end: %08x offset: %04x\n"),
            stack->ctx,
            stack->stack_start,
            stack->stack_end,
            stack->offset);

        for (uint32_t pos = stack->stack_start + stack->offset; pos < stack->stack_end; pos += 0x10) {
            uint32_t *values = (uint32_t *)(pos);

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

    ets_printf_P(PSTR("\n<<<stack<<<\n\n"));
}

extern void custom_crash_postmortem_stack(user_crash_stack_t *, size_t) noexcept
    __attribute__((weak, alias("print_stacks")));

static void postmortem_alloc(const void *addr, size_t size, const char *file, int line, const char *what) noexcept {
    // Use cap-X formatting to ensure the standard EspExceptionDecoder doesn't match the address
    if (addr) {
#if defined(DEBUG_ESP_OOM)
        ets_printf_P(PSTR("\nlast failed alloc call: %08X(%d)@%s:%d\n"),
            (uint32_t)addr, size, file, line);
#else
        (void)file;
        (void)line;
        ets_printf_P(PSTR("\nlast failed alloc call: %08X(%d)\n"),
            (uint32_t)addr, size);
#endif
    }

    if (what && addr) {
        ets_printf_P(PSTR("\nlast failed alloc caller: 0x%08x\n"), (uint32_t)addr);
    }
}

extern void custom_crash_postmortem_alloc(const void*, size_t, const char*, int, const char *) noexcept
    __attribute__((weak, alias("postmortem_alloc")));

static void cut_here() noexcept {
    const char *line = PSTR("---------------");
    ets_printf_P(PSTR("%s CUT HERE FOR EXCEPTION DECODER %s\n"), line, line);
}

static void postmortem_pre() noexcept {
    // https://tinyurl.com/8266dcdr => https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#exception
    ets_printf_P(PSTR("\nTo make this dump useful, DECODE IT - https://tinyurl.com/8266dcdr\n"));
    cut_here();
}

extern void custom_crash_postmortem_post() noexcept
    __attribute__((weak, alias("cut_here")));

extern void custom_crash_postmortem_pre() noexcept
    __attribute__((weak, alias("postmortem_pre")));

[[noreturn]]
static void raise_exception() noexcept;
static void raise_exception() noexcept {
    if (gdb_present())
        __asm__ __volatile__ ("syscall"); // triggers GDB when enabled

    uint32_t sp;
    __asm__ __volatile__ ("mov %0, a1\n\t" : "=a"(sp) :: "memory");
    crash_system_restart_local(sp);
}

void abort() {
    s_user_reset_reason = REASON_USER_ABORT;
    raise_exception();
}

// TODO abi.cpp:new() throws w/ this regardless of -fexceptions / -fno-exceptions
void __unhandled_exception(const char *what) {
    s_user_reset_reason = REASON_USER_EXCEPTION;
    s_what = what;
    raise_exception();
}

[[noreturn]]
static inline void __panic_impl(const char *, const char *, int, const char *, const char *)
    __attribute__((always_inline));
static inline void __panic_impl(user_rst_reason reason, const char *file, int line, const char *func, const char *what) {
    s_user_reset_reason = reason;
    s_panic_file = file;
    s_panic_line = line;
    s_panic_func = func;
    s_what = what;
    gdb_do_break();     /* if GDB is not present, this is a no-op */
    raise_exception();
}

void __assert_func(const char *file, int line, const char *func, const char *what) {
    __panic_impl(
        REASON_USER_ASSERT,
        file, line, func, what);
}

void __panic_func(const char* file, int line, const char* func) {
    __panic_impl(
        REASON_USER_PANIC,
        file, line, func, nullptr);
}

uintptr_t __stack_chk_guard = 0x08675309 ^ RANDOM_REG32;
void __stack_chk_fail() {
    s_user_reset_reason = REASON_USER_STACK_SMASH;
    s_stack_chk_addr = reinterpret_cast<uint32_t>(__builtin_return_address(0));

    if (gdb_present())
        __asm__ __volatile__ ("syscall"); // triggers GDB when enabled

    uint32_t sp;
    __asm__ __volatile__ ("mov %0, a1\n\t" : "=a"(sp) :: "memory");

    crash_system_restart_local(sp);
}

void __stack_overflow(cont_t* cont, uint32_t* sp) {
    s_user_reset_reason = REASON_USER_STACK_OVERFLOW;
    s_stack_chk_addr = reinterpret_cast<uint32_t>(&cont->stack[0]);

    if (gdb_present())
        __asm__ __volatile__ ("syscall"); // triggers GDB when enabled

    crash_system_restart_local(reinterpret_cast<uint32_t>(sp));
}

} // extern "C"
