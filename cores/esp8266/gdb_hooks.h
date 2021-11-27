/*
 gdb_hooks.h - Hooks for GDB Stub library
 Copyright (c) 2018 Ivan Grokhotkov.  All right reserved.

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize GDB stub, if present
 * 
 * By default, this function is a no-op. When GDBStub library is linked,
 * this function is overridden and does necessary initialization of that library.
 * Called early at startup.
 */
void gdb_init(void);

/**
 * @brief Break into GDB, if present
 * 
 * By default, this function is a no-op. When GDBStub library is linked,
 * this function is overridden and triggers entry into the debugger, which
 * looks like a breakpoint hit.
 */
void gdb_do_break(void);

/**
 * @brief Check if GDB stub is present.
 * 
 * By default, this function returns false. When GDBStub library is linked,
 * this function is overridden and returns true. Can be used to check whether
 * GDB is used.
 * 
 * @return true if GDB stub is present
 */
bool gdb_present(void);

// If gdbstub has these set true, then we will disable our own
// usage of them, but use gdbstub's callbacks for them instead
/**
 * @brief Check if GDB is installing a putc1 callback.
 * 
 * By default, this function returns false. When GDBStub library is linked,
 * this function is overridden and returns true.
 * 
 * @return true if GDB is installing a putc1 callback
 */
bool gdbstub_has_putc1_control(void);

/**
 * @brief Register a putc1 callback with GDB.
 * @param func function GDB will proxy putc1 data to
 * 
 * By default, this function is a no-op. When GDBStub library is linked,
 * this function is overridden and sets GDB stub's secondary putc1 callback to
 * func. When GDB stub is linked, but a GDB session is not current attached,
 * then GDB stub will pass putc1 chars directly to this function.
 */
void gdbstub_set_putc1_callback(void (*func)(char));

/**
 * @brief Check if GDB is installing a uart0 isr callback.
 * 
 * By default, this function returns false. When GDBStub library is linked,
 * this function is overridden and returns true.
 * 
 * @return true if GDB is installing a uart0 isr callback
 */
bool gdbstub_has_uart_isr_control(void);

/**
 * @brief Register a uart0 isr callback with GDB.
 * @param func function GDB will proxy uart0 isr data to
 * 
 * By default, this function is a no-op. When GDBStub library is linked,
 * this function is overridden and sets GDB stub's secondary uart0 isr callback
 * to func. When GDB stub is linked, but a GDB session is not current attached,
 * then GDB stub will pass uart0 isr data back to this function.
 */
void gdbstub_set_uart_isr_callback(void (*func)(void*, uint8_t), void* arg);

/**
 * @brief Write a character for output to a GDB session on uart0.
 * @param c character to write
 * 
 * By default, this function is a no-op. When GDBStub library is linked,
 * this function is overridden and writes a char to either the GDB session on
 * uart0 or directly to uart0 if not GDB session is attached.
 */
void gdbstub_write_char(char c);

/**
 * @brief Write a char buffer for output to a GDB session on uart0.
 * @param buf buffer of data to write
 * @param size length of buffer
 * 
 * By default, this function is a no-op. When GDBStub library is linked,
 * this function is overridden and writes a buffer to either the GDB session on
 * uart0 or directly to uart0 if not GDB session is attached.
 */
void gdbstub_write(const char* buf, size_t size);

#ifdef __cplusplus
}
#endif
