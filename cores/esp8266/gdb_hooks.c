/*
 gdb_hooks.c - Default (no-op) hooks for GDB Stub library
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

#include "ets_sys.h"
#include "gdb_hooks.h"


/* gdb_init and gdb_do_break do not return anything, but since the return
   value is in register, it doesn't hurt to return a bool, so that the
   same stub can be used for gdb_present. */

bool ICACHE_RAM_ATTR __gdb_no_op()
{
    return false;
}

extern void gdb_init(void) __attribute__ ((weak, alias("__gdb_no_op")));
extern void gdb_do_break(void) __attribute__ ((weak, alias("__gdb_no_op")));
extern bool gdb_present(void) __attribute__ ((weak, alias("__gdb_no_op")));

