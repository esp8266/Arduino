/*
    cont.h - continuations support for Xtensa call0 ABI
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

#ifndef CONT_H_
#define CONT_H_

#include <stdbool.h>

#ifndef CONT_STACKSIZE
#define CONT_STACKSIZE 4096
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cont_
{
    void (*pc_ret)(void);
    unsigned* sp_ret;

    void (*pc_yield)(void);
    unsigned* sp_yield;

    unsigned* stack_end;
    unsigned unused1;
    unsigned unused2;
    unsigned stack_guard1;

    unsigned stack[CONT_STACKSIZE / 4];

    unsigned stack_guard2;
    unsigned* struct_start;
} cont_t;

extern cont_t* g_pcont;

// Initialize the cont_t structure before calling cont_run
void cont_init(cont_t*);

// Run function pfn in a separate stack, or continue execution
// at the point where cont_yield was called
void cont_run(cont_t*, void (*pfn)(void));

// Return to the point where cont_run was called, saving the
// execution state (registers and stack)
void cont_yield(cont_t*);

// Check guard bytes around the stack. Return 0 in case everything is ok,
// return 1 if guard bytes were overwritten.
int cont_check(cont_t* cont);

// Go through stack and check how many bytes are most probably still unchanged
// and thus weren't used by the user code. i.e. that stack space is free. (high water mark)
int cont_get_free_stack(cont_t* cont);

// Check if yield() may be called. Returns true if we are running inside
// continuation stack
bool cont_can_yield(cont_t* cont);

// Repaint the stack from the current SP to the end, to allow individual
// routines' stack usages to be calculated by re-painting, checking current
// free, running the routine, then checking the max free
void cont_repaint_stack(cont_t *cont);


#ifdef __cplusplus
}
#endif

#endif /* CONT_H_ */
