/////////////////////////////////////////////////////////////////////////////
//
//  cont.h: continuations support for Xtensa call0 ABI
//  Copyright (c) Ivan Grokhotkov 2014
//
//  This file is licensed under MIT license
//
//  Permission is hereby granted, free of charge, to any person obtaining
//  a copy of this software and associated documentation files (the
//  "Software"), to deal in the Software without restriction, including
//  without limitation the rights to use, copy, modify, merge, publish,
//  distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so, subject to
//  the following conditions:
//
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef CONT_H_
#define CONT_H_

#ifndef CONT_STACKSIZE
#define CONT_STACKSIZE 4096
#endif

typedef struct cont_
{
	void (*pc_ret)(void);
	unsigned* sp_ret;

	void (*pc_yield)(void);
	unsigned* sp_yield;

	unsigned* stack_end;
	unsigned  stack_guard1;

	unsigned  stack[CONT_STACKSIZE / 4];

	unsigned  stack_guard2;
	unsigned* struct_start;
} cont_t;

// Initialize the cont_t structure before calling cont_run
void cont_init (cont_t*);

// Run function pfn in a separate stack, or continue execution
// at the point where cont_yield was called
void cont_run(cont_t*, void(*pfn)(void));

// Return to the point where cont_run was called, saving the
// execution state (registers and stack)
void cont_yield(cont_t*);

// Check guard bytes around the stack. Return 0 in case everything is ok,
// return 1 if guard bytes were overwritten.
int  cont_check(cont_t* cont);

#endif /* CONT_H_ */
