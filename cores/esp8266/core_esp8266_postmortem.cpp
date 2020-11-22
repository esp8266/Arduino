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
#include "esp8266_undocumented.h"
#include "core_esp8266_features.h"

extern "C" {

/*
  The Boot ROM `__divsi3` function handles a divide by 0 by branching to the
  `ill` instruction at address 0x4000dce5. By looking for this address in epc1
  we can separate the divide by zero event from other `ill` instruction events.
*/
constexpr uint32_t divide_by_0_exception = 0x4000dce5u;

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_EXCEPTIONS)
#define ALIGN_UP(a, s) ((decltype(a))((((uintptr_t)(a)) + (s-1)) & ~(s-1)))

// Famous locations in Boot ROM
constexpr uint32_t _xtos_unhandled_exception__bp_address = 0x4000DC4Bu;
constexpr uint32_t _xtos_unhandled_interrupt__bp_address = 0x4000DC3Cu;

/*
  For exceptions that are directed to one of the general exception vectors
  (UserExceptionVector, KernelExceptionVector, or DoubleExceptionVector) there
  are 64 possible causes. The exception cause register EXCCAUSE has the value.
  The value is in the lower 6 bits (5 ... 0). Other bits in the register are
  reserved and may need to be truncated before use.
*/
constexpr size_t max_num_exccause_values = 64u;

/*
  Xtensa (R) Instruction Set Architecture (ISA) Reference Manual, p90:
  EXCCAUSE value 20, InstFetchProhibitedCause - An instruction fetch referenced
  a page mapped with an attribute that does not  permit instruction fetch.
*/
constexpr uint32_t instFetchProhibitedCause = 20u;

extern void _DebugExceptionVector(void);
extern void _KernelExceptionVector(void);
extern void _DoubleExceptionVector(void);

void postmortem_debug_exception_vector(void);
void postmortem_kernel_exception_handler(void);
void postmortem_double_exception_handler(void);

/*
  I cannot find official documentation for the size and offset addresse for
  the exception vectors. These values were inferred from one of the Boot ROM
  listings out on the Internet.
*/
constexpr size_t max_debug_exc_vector_size = 16u;
constexpr size_t max_kernel_exc_vector_size = 32u;
constexpr size_t max_double_exc_vector_size = 16u;

// These function entry points are for computation ONLY they are NOT aligned as functions
extern void *postmortem_debug_exception_vector_last;
extern void *postmortem_kernel_exception_handler_last;
extern void *postmortem_double_exception_handler_last;
extern void *postmortem_double_exception_handler_ill;

// Calculate the location address of where the `ill` instruction that is
// used/leveraged to flag exceptions: Debug, Kerne, and Double Exceptions.
// See "Handle breakpoints via postmortem ..." below for more details.
const uint32_t bp_ill_address = (uintptr_t)_DoubleExceptionVector + ((uintptr_t)&postmortem_double_exception_handler_ill - (uintptr_t)postmortem_double_exception_handler);

// Context structure used by Debug, Double, Kernel Exception and unhandled
// exception stubs to build a stack frame with potentially useful addresses for
// EXP Exception Decoder's use. Also used to save some special registers
// that were overwritten by using the `ill` instruction.
struct EXC_CONTEXT{
    uint32_t ps;          // +0
    uint32_t pad1;        // +4
    uint32_t pad2;        // +8
    uint32_t excsave2;    // +12
    uint32_t exccause;    // +16
    uint32_t epc1;        // +20
    uint32_t a1;          // +24
    uint32_t excsave1;    // +28, a0 at the time of the event
};

#endif

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

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_EXCEPTIONS)
    if (!gdb_present()) {
        // Clear breakpoint registers so ESP doesn't get a HWDT crash at reboot.
        uint32_t tmp;  // Let the compiler select the optimum scratch register
        asm volatile(
            "movi.n           %0,   0\n\t"
            "wsr.dbreakc0     %0\n\t"
            "wsr.ibreakenable %0\n\t"
            "wsr.icount       %0\n\t"
            :"=r"(tmp) ::
        );
    }
#endif

    /*
      Check for Exceptions mapped into User Exception.
    */
    if (rst_info.reason == REASON_EXCEPTION_RST && rst_info.exccause == 0) {
#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_EXCEPTIONS)
        if (rst_info.epc1 == bp_ill_address) {
            struct EXC_CONTEXT *exc_context;
            asm volatile("rsr.excsave2 %0\n\t" : "=a" (exc_context) :: "memory");
            // Use Special Register values from before the `ill` instruction.
            // Note, for a double exception, the exccause is that of the second exception.
            // epc1 is still the address of the 1st exception, its cause is now unknown.
            rst_info.exccause = exc_context->exccause;
            rst_info.epc1 = exc_context->epc1;
            // Double Exception
            if (rst_info.depc) {
                ets_printf_P(PSTR("\nDouble Exception"));
                // The current "ESP Exception Decoder" does not process the value in depc
                // Set a fake epc1 so the decoder identifies the line of the Double Exception
                rst_info.epc1 = rst_info.depc;
                // The "ESP Exception Decoder" is also helped by updating the
                // context saved on the stack.
                exc_context->epc1 = rst_info.depc;
            }
            // BP - Debug Exception
            else if (rst_info.epc2) {
                if (rst_info.epc2 == _xtos_unhandled_exception__bp_address) {
                    // Unhandled Exceptions land here. 'break 1, 1' in _xtos_unhandled_exception
                    if (instFetchProhibitedCause == exc_context->exccause) {
                        // This could be the result from a jump or call; a call
                        // is more likely. eg. calling a null callback function
                        // For the call case, a0 is likely the return address.
                        rst_info.epc1 = exc_context->excsave1;
                    }
                    ets_printf_P(PSTR("\nXTOS Unhandled exception - BP"));
                }
                else if (rst_info.epc2 == _xtos_unhandled_interrupt__bp_address) {
                    // Unhandled interrupts land here. 'break 1, 15' in _xtos_unhandled_interrupt
                    ets_printf_P(PSTR("\nXTOS Unhandled interrupt - BP"));
                    // Don't know what should be done here if anything.
                }
                else {
                    ets_printf_P(PSTR("\nDebug Exception"));
                    // The current Exception Decoder does not process the value in epc2
                    // Set a fake epc1 so the Exception Decoder identifies the correct BP line
                    rst_info.epc1 = rst_info.epc2;
                }
            }
            // Kernel Exception
            else {
                ets_printf_P(PSTR("\nKernel Exception"));
            }
            ets_printf_P(PSTR(" - excsave1=0x%08x excsave2=0x%08x epc1=0x%08x\n"),
                exc_context->excsave1, exc_context->excsave2, exc_context->epc1 );
        }
#endif
        // The GCC divide routine in ROM jumps to the address below and executes ILL (00 00 00) on div-by-zero
        // In that case, print the exception as (6) which is IntegerDivZero
        if (rst_info.epc1 == divide_by_0_exception) {
            rst_info.exccause = 6;
        }
    }
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
        ets_printf_P(PSTR("\nException (%d):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"),
            rst_info.exccause, rst_info.epc1, rst_info.epc2, rst_info.epc3, rst_info.excvaddr, rst_info.depc);
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

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_EXCEPTIONS)
/*
        Handle breakpoints via postmortem when GDB is not running.

    Objective: Add support for Debug, Kernel, and Double Exception reporting
    with minimum impact on IRAM usage. Additionally, improve reporting of
    Unhandled Exception causes for the disabled interrupt case.

    Overview: In the absence of GDB, install a small breakpoint handler that
    clears EXCM and executes an illegal instruction. This leverages the existing
    exception handling code in the SDK for the `ill` instruction. At postmortem
    processing, identify the event and report. This allows for alerting of
    embedded breakpoints. In the stack trace, EPC1 is updated with the value of
    EPC2. This helps "Exception Decoder" identify the line where the BP
    occurred. By using the SDK's handling for `ill`, we get support for handling
    the process of getting out of the Cache_Read_Disable state free, without
    consuming IRAM.

    For breakpoints (BP), the SDK's default behavior is to loop, waiting for an
    interrupt >2 until the HWDT kicks in. The current "C++" compiler will embed
    breakpoints when it detects a divide by zero at compile time. Are there
    other situations where the compiler does this?

    Expand technique to preserve more of the current context and to also report
    Kernel and Double exceptions through postmortem. As well as enhanced
    reporting for XTOS Unhandled Exceptions.

    To avoid having to deal with literals and to minimize the use of IRAM, some
    jumping around is done to use pockets of available IRAM in each vector
    handler's allotted space. Regrettably, the relative jumps needed to ties
    these pockets of memory together were computed by hand.

    To assist the "ESP Exception Decoder", create a stack frame to store
    various special registers for inspection. The expectation is that some of
    these values may point to code. This can give insight into what was running
    at the time of the crash.
*/

//
//  Stage Exception Vector Stubs to be copied into respective vector address
//  space in ICACHE/PROGMEM address space.
//
asm(  // 16 bytes MAX, using 16
    ".section     .text.postmortem_debug_exception_vector,\"ax\",@progbits\n\t"
    ".align       4\n\t"
    ".type        postmortem_debug_exception_vector, @function\n\t"
    "\n"
"postmortem_debug_exception_vector:\n\t"  // Copy destination _DebugExceptionVector
    "wsr.excsave2 a0\n\t"
    "j            . + 32\n\t"             // _KernelExceptionVector + 3

    // continue here from _KernelExceptionVector
"postmortem_debug_exception_vector_part2:\n\t"
    "s32i         a0,     a1,     16\n\t"
    "rsr.ps       a0\n\t"
    "s32i         a0,     a1,     0\n\t"
    "j            . + 86\n\t"             // finish with postmortem_double_exception_handler_part2
"postmortem_debug_exception_vector_last:\n\t"
    ".size postmortem_debug_exception_vector, .-postmortem_debug_exception_vector\n\t"
);

asm(  // 32 bytes MAX, using 31
    ".section     .text.postmortem_kernel_exception_handler,\"ax\",@progbits\n\t"
    ".align       4\n\t"
    ".type        postmortem_kernel_exception_handler, @function\n\t"
    "\n"
"postmortem_kernel_exception_handler:\n\t" // Copy destination _KernelExceptionVector
    "wsr.excsave1 a0\n\t"

"postmortem_kernel_exception_handler_p3:\n\t"
    "addi         a0,     a1,     -32\n\t"
    "s32i         a1,     a0,     24\n\t"
    "mov.n        a1,     a0\n\t"
    "xsr.excsave2 a0\n\t"                 // stash pointer to stack data in excsave2
    "s32i         a0,     a1,     12\n\t" // and save previous
    "rsr.excsave1 a0\n\t"
    "s32i         a0,     a1,     28\n\t"
    "rsr.epc1     a0\n\t"
    "s32i         a0,     a1,     20\n\t"
    // save current exccause
    "rsr.exccause a0\n\t"
    "j            . - 54\n\t"   // continue at postmortem_debug_exception_vector_part2
"postmortem_kernel_exception_handler_last:\n\t"
    ".size postmortem_kernel_exception_handler, .-postmortem_kernel_exception_handler\n\t"
);

asm(  // 16 byte MAX, using 14
    ".section     .text.postmortem_double_exception_handler,\"ax\",@progbits\n\t"
    ".align       4\n\t"
    ".type        postmortem_double_exception_handler, @function\n\t"
    "\n"
"postmortem_double_exception_handler:\n\t" // Copy destination _DoubleExceptionVector
    // Common code at _KernelExceptionVector, which is -64 bytes from
    // _DoubleExceptionVector.
    "j             .  - 64\n\t" // continue at _KernelExceptionVector

    // finish here from _DebugExceptionVector part 2
    // _DoubleExceptionVector part 2
"postmortem_double_exception_handler_part2:\n\t"
    // User mode, Clear EXCM, and block all interrupts
    "movi.n       a0,    0x02F\n\t"
    "wsr.ps       a0\n\t"
    "rsync\n\t"
    // Let _UserExceptionVector finish processing the exception as an
    // exception(0). Adjust results at __wrap_system_restart_local so that `ESP
    // Exception Decoder` reports the state before this `ill` instruction.
"postmortem_double_exception_handler_ill:\n\t"
    "ill\n\t"
"postmortem_double_exception_handler_last:\n\t"
    ".size postmortem_double_exception_handler, .-postmortem_double_exception_handler\n\t"
);

/*
  Of the Exception Cause table's 64 entries, most are set to
  `_xtos_unhandled_exception`. And I assume most of these never occur; however,
  at lesat one does exccause 20. This one occurs on a call to a NULL pointer. This
  can happen when a function prototype is given the weak attribute but never
  defined, or forgetting to test a call back function pointer for NULL before
  calling, etc.

  A problem with the `_xtos_unhandled_exception` function is that it handles the
  event with a Debug Exception BP. If you don't have GDB running you see a HWDT
  reset. If interrupts are at INTLEVEL 2 or higher (a BP instruction becomes a
  NOP), you still see a HWDT reset regardless of running GDB.
*/

void postmortem_xtos_unhandled_exception(...);
asm(
    ".section     .iram.text.postmortem_xtos_unhandled_exception,\"ax\",@progbits\n\t"
    ".literal_position\n\t"
    ".align       4\n\t"
    ".type        postmortem_xtos_unhandled_exception, @function\n\t"
    "\n"
"postmortem_xtos_unhandled_exception:\n\t"
    // Rewind Boot ROM User Exception prologue and continue to
    // _KernelExceptionVector such that we look like a Debug BP Exception
    // sitting at 'break 1, 1' in the _xtos_unhandled_exception handler.
    "movi         a2,     0x4000dc4bu\n\t"  // _xtos_unhandled_exception__bp_address
    "wsr.epc2     a2\n\t"
    "l32i         a2,     a1,     20\n\t"
    "l32i         a3,     a1,     24\n\t"
    "addmi        a1,     a1,     0x100\n\t"
    "j            _KernelExceptionVector\n\t"
    ".size postmortem_xtos_unhandled_exception, .-postmortem_xtos_unhandled_exception\n\t"
);


static void replace_exception_handler_on_match(
                uint32_t cause,
                _xtos_handler match,
                _xtos_handler replacement) {

    _xtos_handler old_wrapper = _xtos_exc_handler_table[cause];
    if (old_wrapper == match || NULL == match) {
        _xtos_exc_handler_table[cause] = replacement;
    }
}

// While _xtos_unhandled_exception is in the linker .ld file, it may have been
// overridden. We require the original Boot ROM function address to limit our
// override to those old values in the table.
const _xtos_handler ROM_xtos_unhandled_exception = (reinterpret_cast<_xtos_handler>(0x4000dc44));

static void install_unhandled_exception_handler(void) {
    // Only replace Exception Table entries still using the orignal Boot ROM
    // _xtos_unhandled_exception handler.
    for (size_t i = 0; i < max_num_exccause_values; i++) {
        replace_exception_handler_on_match(
            i,
            ROM_xtos_unhandled_exception,
            postmortem_xtos_unhandled_exception);
    }
}

void postmortem_init(void) {
    if (!gdb_present()) {
        // Install all three (interdependent) exception handler stubs.
        // Length of copy must be rounded up to copy memory in aligned 4 byte
        // increments to comply with IRAM memory access requirements.
        const size_t debug_vector_sz  = ALIGN_UP((uintptr_t)&postmortem_debug_exception_vector_last   - (uintptr_t)postmortem_debug_exception_vector, 4);
        const size_t kernel_vector_sz = ALIGN_UP((uintptr_t)&postmortem_kernel_exception_handler_last - (uintptr_t)postmortem_kernel_exception_handler, 4);
        const size_t double_vector_sz = ALIGN_UP((uintptr_t)&postmortem_double_exception_handler_last - (uintptr_t)postmortem_double_exception_handler, 4);
#ifdef DEBUG_POSTMORTEM_EXCEPTION_VERIFY_SIZES
        if (max_debug_exc_vector_size < debug_vector_sz ||
            max_kernel_exc_vector_size < kernel_vector_sz ||
            max_double_exc_vector_size < double_vector_sz)
        {
            ets_printf_P(PSTR("\npostmortem_init: Internal Error: exception vector size(s) too big\n"));
            ets_printf_P(PSTR("  debug_vector_sz(%u), kernel_vector_sz(%u), double_vector_sz(%u)\n"),
                debug_vector_sz, kernel_vector_sz, double_vector_sz);
        } else
#endif
        {
            uint32_t save_ps = xt_rsil(15);
            ets_memcpy((void*)_DebugExceptionVector, (void*)postmortem_debug_exception_vector, debug_vector_sz);
            ets_memcpy((void*)_KernelExceptionVector, (void*)postmortem_kernel_exception_handler, kernel_vector_sz);
            ets_memcpy((void*)_DoubleExceptionVector, (void*)postmortem_double_exception_handler, double_vector_sz);
            // A little asm optimization. No need to zero exccause, epc1 and
            // excsave1 these will be quickly set on the 1st timer tick for the
            // Soft WDT. Set the rest to zero.
            uint32_t tmp;
            asm volatile(
                "movi.n       %0,   0\n\t"
                "wsr.epc2     %0\n\t"
                "wsr.epc3     %0\n\t"
                "wsr.excsave2 %0\n\t"
                "wsr.depc     %0\n\t"
                :"=r"(tmp) ::
            );
            install_unhandled_exception_handler();
            xt_wsr_ps(save_ps);
        }
    }
}

#endif

};
