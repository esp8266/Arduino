/*
  cont.S - continuations support for Xtensa call0 ABI
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

    .section .irom0.text
    .align    4
    .literal_position
    .global   cont_suspend
    .type     cont_suspend, @function
cont_suspend:
    /* a1: sp */
    /* a2: void* cont_ctx */
    /* adjust stack and save registers */
    addi    a1,  a1, -24
    s32i    a12, a1, 0
    s32i    a13, a1, 4
    s32i    a14, a1, 8
    s32i    a15, a1, 12
    s32i    a0,  a1, 16
    s32i    a2,  a1, 20

    /* &cont_continue -> cont_ctx.pc_suspend */
    movi    a3, cont_continue
    s32i    a3, a2, 8
    /* sp -> cont_ctx.sp_suspend */
    s32i    a1, a2, 12

    /* a0 <- cont_ctx.pc_ret */
    l32i    a0, a2, 0
    /* sp <- cont_ctx.sp_ret */
    l32i    a1, a2, 4
    jx      a0

cont_continue:
    l32i    a12, a1, 0
    l32i    a13, a1, 4
    l32i    a14, a1, 8
    l32i    a15, a1, 12
    l32i    a0,  a1, 16
    l32i    a2,  a1, 20
    addi    a1,  a1, 24
    ret
    .size    cont_suspend, . - cont_suspend

////////////////////////////////////////////////////

/*
  The purpose of cont_wrapper is to signal to xtensa-gdb
  that we want to treat this function as the outermost one.

  From: binutils-gdb-xtensa/gdb/xtensa-tdep.c:2677 <https://git.io/vA8Ps>
    "Special case for terminating backtrace at a function that wants to
    be seen as the outermost one.  Such a function will clear it's RA (A0)
    register to 0 in the prologue instead of saving its original value."
*/

    .text
    .align   4
    .literal_position
    .global  cont_wrapper
    .type    cont_wrapper, @function
cont_wrapper:
    movi    a0, 0
    callx0  a3
    movi    a2, cont_norm
    jx      a2
    .size   cont_wrapper, . - cont_wrapper

////////////////////////////////////////////////////

    .section .irom0.text
    .align   4
    .literal_position
    .global  cont_run
    .type    cont_run, @function
cont_run:
    /* a1: sp */
    /* a2: void* cont_ctx */
    /* a3: void (*pfn) */

    /* adjust stack and save registers */
    addi    a1,  a1, -20
    s32i    a12, a1, 0
    s32i    a13, a1, 4
    s32i    a14, a1, 8
    s32i    a15, a1, 12
    s32i    a0,  a1, 16

    /* cont_ret -> a4 -> cont_ctx.pc_ret*/
    movi    a4, cont_ret
    s32i    a4, a2, 0
    /* sp -> cont_ctx.sp_ret */
    s32i    a1, a2, 4

    /* if cont_ctx.pc_suspend != 0, goto cont_resume */
    l32i    a4, a2, 8
    bnez    a4, cont_resume
    /* else */
    /* set new stack*/
    l32i    a1, a2, 16;
    /* goto pfn */
    movi    a2, cont_wrapper
    jx      a2

cont_resume:
    /* a1 <- cont_ctx.sp_suspend */
    l32i    a1, a2, 12
    /* reset yield flag, 0 -> cont_ctx.pc_suspend */
    movi    a3, 0
    s32i    a3, a2, 8
    /* jump to saved cont_ctx.pc_suspend */
    movi    a0, cont_ret
    jx       a4

cont_norm:
    /* calculate pointer to cont_ctx.struct_start from sp */
    l32i    a2,    a1, 4
    /* sp <- cont_ctx.sp_ret */
    l32i    a1, a2, 4
    /* 0 -> cont_ctx.pc_ret */
    movi    a4, 0
    s32i    a4, a2, 0

cont_ret:
    /* restore registers */
    l32i    a12, a1, 0
    l32i    a13, a1, 4
    l32i    a14, a1, 8
    l32i    a15, a1, 12
    l32i    a0,  a1, 16
    /* adjust stack and return */
    addi    a1,  a1, 20
    ret
    .size   cont_run, . - cont_run
