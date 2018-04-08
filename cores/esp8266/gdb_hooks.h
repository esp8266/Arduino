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
 * this function is overriden and does necessary initialization of that library.
 * Called early at startup.
 */
void gdb_init(void);

/**
 * @brief Break into GDB, if present
 * 
 * By default, this function is a no-op. When GDBStub library is linked,
 * this function is overriden and triggers entry into the debugger, which
 * looks like a breakpoint hit.
 */
void gdb_do_break(void);

/**
 * @brief Check if GDB stub is present.
 * 
 * By default, this function returns false. When GDBStub library is linked,
 * this function is overriden and returns true. Can be used to check whether
 * GDB is used.
 * 
 * @return true if GDB stub is present
 */
bool gdb_present(void);

#ifdef __cplusplus
}
#endif
