/*
 cont_util.s - continuations support for Xtensa call0 ABI
 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

#include "cont.h"
#include <stddef.h>
#include "ets_sys.h"


#define CONT_STACKGUARD 0xfeefeffe

void ICACHE_RAM_ATTR cont_init(cont_t* cont) {
    cont->stack_guard1 = CONT_STACKGUARD;
    cont->stack_guard2 = CONT_STACKGUARD;
    cont->stack_end = cont->stack + (sizeof(cont->stack) / 4);
    cont->struct_start = (unsigned*) cont;
    
    // fill stack with magic values to check high water mark
    for(int pos = 0; pos < sizeof(cont->stack) / 4; pos++)
    {
        cont->stack[pos] = CONT_STACKGUARD;
    }
}

int ICACHE_RAM_ATTR cont_check(cont_t* cont) {
    if(cont->stack_guard1 != CONT_STACKGUARD || cont->stack_guard2 != CONT_STACKGUARD) return 1;

    return 0;
}

int ICACHE_RAM_ATTR cont_get_free_stack(cont_t* cont) {
    uint32_t *head = cont->stack;
    int freeWords = 0;

    while(*head == CONT_STACKGUARD)
    {
        head++;
        freeWords++;
    }
    
    return freeWords * 4;
}

bool ICACHE_RAM_ATTR cont_can_yield(cont_t* cont) {
    return !ETS_INTR_WITHINISR() &&
           cont->pc_ret != 0 && cont->pc_yield == 0;
}
