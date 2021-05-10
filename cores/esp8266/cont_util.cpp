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
#include <string.h>
#include "ets_sys.h"

extern "C" {

#define CONT_STACKGUARD 0xfeefeffe

void cont_init(cont_t* cont) {
    memset(cont, 0, sizeof(cont_t));
    
    cont->stack_guard1 = CONT_STACKGUARD;
    cont->stack_guard2 = CONT_STACKGUARD;
    cont->stack_end = cont->stack + (sizeof(cont->stack) / 4);
    cont->struct_start = (unsigned*) cont;
    
    // fill stack with magic values to check high water mark
    for(int pos = 0; pos < (int)(sizeof(cont->stack) / 4); pos++)
    {
        cont->stack[pos] = CONT_STACKGUARD;
    }
}

int IRAM_ATTR cont_check(cont_t* cont) {
    if(cont->stack_guard1 != CONT_STACKGUARD || cont->stack_guard2 != CONT_STACKGUARD) return 1;

    return 0;
}

// No need for this to be in IRAM, not expected to be IRQ called
int cont_get_free_stack(cont_t* cont) {
    uint32_t *head = cont->stack;
    int freeWords = 0;

    while(*head == CONT_STACKGUARD)
    {
        head++;
        freeWords++;
    }
    
    return freeWords * 4;
}

bool IRAM_ATTR cont_can_yield(cont_t* cont) {
    return !ETS_INTR_WITHINISR() &&
           cont->pc_ret != 0 && cont->pc_yield == 0;
}

// No need for this to be in IRAM, not expected to be IRQ called
void cont_repaint_stack(cont_t *cont)
{
    register uint32_t *sp asm("a1");
    // Ensure 64 bytes adjacent to the current SP don't get touched to endure
    // we don't accidentally trounce over locals or IRQ temps.
    // Fill stack with magic values
    for ( uint32_t *pos = sp - 16; pos >= &cont->stack[0]; pos-- )
    {
       *pos = CONT_STACKGUARD;
    }
}

};
