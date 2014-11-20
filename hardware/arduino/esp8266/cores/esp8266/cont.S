/////////////////////////////////////////////////////////////////////////////
//
//  cont.S: continuations support for Xtensa call0 ABI
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


	.text
	.align	4
	.literal_position
	.global	cont_yield
	.type	cont_yield, @function
cont_yield:
	/* a1: sp */
	/* a2: void* cont_ctx */
	/* adjust stack and save registers */
	addi	a1,	a1, -24
	s32i	a12, a1, 0
	s32i	a13, a1, 4
	s32i	a14, a1, 8
	s32i	a15, a1, 12
	s32i	a0,  a1, 16
	s32i	a2,	 a1, 20

	/* &cont_continue -> cont_ctx.pc_yield */
	movi	a3, cont_continue
	s32i	a3, a2, 8
	/* sp -> cont_ctx.sp_yield */
	s32i	a1, a2, 12

	/* a0 <- cont_ctx.pc_ret */
	l32i	a0, a2, 0
	/* sp <- cont_ctx.sp_ret */
	l32i	a1, a2, 4
	jx  	a0

cont_continue:
	l32i	a12, a1, 0
	l32i	a13, a1, 4
	l32i	a14, a1, 8
	l32i	a15, a1, 12
	l32i	a0,  a1, 16
	l32i	a2,	 a1, 20
	addi	a1,	 a1, 24
	ret
	.size	cont_yield, . - cont_yield

////////////////////////////////////////////////////

	.text
	.align	4
	.literal_position
	.global	cont_run
	.type	cont_run, @function
cont_run:
	/* a1: sp */
	/* a2: void* cont_ctx */
	/* a3: void (*pfn) */

	/* adjust stack and save registers */
	addi	a1,  a1, -20
	s32i	a12, a1, 0
	s32i	a13, a1, 4
	s32i	a14, a1, 8
	s32i	a15, a1, 12
	s32i	a0,  a1, 16

	/* cont_ret -> a4 -> cont_ctx.pc_ret*/
	movi	a4, cont_ret
	s32i	a4, a2, 0
	/* sp -> cont_ctx.sp_ret */
	s32i	a1, a2, 4

	/* if cont_ctx.pc_yield != 0, goto cont_resume */
	l32i	a4, a2, 8
	bnez	a4, cont_resume
	/* else */
	/* set new stack*/
	l32i	a1, a2, 16;
	/* goto pfn */
	movi	a0, cont_norm
	jx   	a3

cont_resume:
	/* a1 <- cont_ctx.sp_yield */
	l32i	a1, a2, 12
	/* reset yield flag, 0 -> cont_ctx.pc_yield */
	movi	a3, 0
	s32i	a3, a2, 8
	/* jump to saved cont_ctx.pc_yield */
	movi	a0, cont_ret
	jx   	a4

cont_norm:
	/* calculate pointer to cont_ctx.struct_start from sp */
	l32i	a2,	a1, 8
	/* sp <- cont_ctx.sp_ret */
	l32i	a1, a2, 4

cont_ret:
	/* restore registers */
	l32i	a12, a1, 0
	l32i	a13, a1, 4
	l32i	a14, a1, 8
	l32i	a15, a1, 12
	l32i	a0,  a1, 16
	/* adjust stack and return */
	addi	a1,  a1, 20
	ret
	.size	cont_run, . - cont_run
