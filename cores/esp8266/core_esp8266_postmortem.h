/*
 Part of the postmortem.c - output of debug info on sketch crash
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

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <rst_info.h> // NONOS SDK `struct rst_info`, ref. user_interafce.h

#ifdef __cplusplus
extern "C" {
#endif

// using numbers different from "REASON_" in user_interface.h (=0..6)
enum user_rst_reason
{
    REASON_USER_ABORT = 249,
    REASON_USER_ASSERT = 250,
    REASON_USER_PANIC = 251,
    REASON_USER_STACK_OVERFLOW = 252,
    REASON_USER_STACK_SMASH = 253,
    REASON_USER_EXCEPTION = 254,
};

// Following functions could be overridden, simply declare them in your own code with a custom functionality
// Ensure that overrides are either declared with `extern "C"` *or* located in .c file(s)
// Comments below describe the default function.

#ifdef __cplusplus
#define ESP8266_POSTMORTEM_NOEXCEPT noexcept
#else
#define ESP8266_POSTMORTEM_NOEXCEPT
#endif

// Postmortem report uses both UART0 & UART1 for output by installing a custom SDK putc1 function
void custom_crash_putc1(char)
    ESP8266_POSTMORTEM_NOEXCEPT;

// Executed before and after output. By default, displays CUT HERE line
void custom_crash_postmortem_pre(void)
    ESP8266_POSTMORTEM_NOEXCEPT;
void custom_crash_postmortem_post(void)
    ESP8266_POSTMORTEM_NOEXCEPT;

// Before processing stack, handle system or user reset reason
void custom_crash_postmortem_reason(struct rst_info *)
    ESP8266_POSTMORTEM_NOEXCEPT;

// Whenever panic() is called explicitly or via assert(), displays its arguments
void custom_crash_postmortem_panic(const char *file, int line, const char *func, const char *what)
    ESP8266_POSTMORTEM_NOEXCEPT;

// Whenever abort() called explicitly, adds a notice
void custom_crash_postmortem_abort(void)
    ESP8266_POSTMORTEM_NOEXCEPT;

// Displays C++ unhandled exception what()
void custom_crash_postmortem_unhandled_exception(const char *what)
    ESP8266_POSTMORTEM_NOEXCEPT;

struct user_crash_stack_t {
    const char *ctx; // name of the stack context
    uint32_t stack_start; // stack address range
    uint32_t stack_end;
    uint32_t offset; // offset to actually useful data on the stack (determined empirically, might break)
};

// Nicely formatted stack(s) contents
void custom_crash_postmortem_stack(struct user_crash_stack_t *, size_t)
    ESP8266_POSTMORTEM_NOEXCEPT;

// Notifies about the last allocation failure, sometimes w/ a caller location
void custom_crash_postmortem_alloc(const void *addr, size_t size, const char *file, int line, const char *what)
    ESP8266_POSTMORTEM_NOEXCEPT;

// No-op by default. Stack iseEither CONT or SYS, stack start offset is already applied
void custom_crash_callback(struct rst_info *, uint32_t stack, uint32_t stack_end)
    ESP8266_POSTMORTEM_NOEXCEPT;

// Or, it is also possible to override the postmortem function itself and remove all output and custom callback
// > extern "C" custom_crash_postmortem_report(struct rst_info *, uint32_t) {
// > }
void custom_crash_postmortem_report(struct rst_info *, uint32_t)
    ESP8266_POSTMORTEM_NOEXCEPT;

#undef ESP8266_POSTMORTEM_NOEXCEPT

#ifdef __cplusplus
} // extern "C"
#endif
