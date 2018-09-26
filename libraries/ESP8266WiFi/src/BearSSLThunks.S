/*
  esp8266_thunks.S - Allow use second stack for BearSSL calls

  BearSSL uses a significant amount of stack space, much larger than
  the default Arduino core stack. These routines handle swapping
  between a secondary, user-allocated stack on the heap and the real
  stack.

  Copyright (c) 2017 Earle F. Philhower, III. All rights reserved.

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
  Modified 8 May 2015 by Hristo Gochkov (proper post and file upload handling)
*/

/* We need to keep track of the end of the thunking stack and the */
/* original stack pointer (A1). Keep them local here. */
.data
.align 4
_stackEnd: .word 0x00000000
_saveStack: .word 0x00000000



/* void SetThunkStackEnd(unsigned int *end);
 *
 * Stores the address of the user-allocated thunking stack (be sure to pass
 * in the *end* of the stack (highest value) since stacks move down on this
 * architecture.
 */
.text
.global SetThunkStackEnd
.type SetThunkStackEnd, @function
.align 4
SetThunkStackEnd:
  movi a15, _stackEnd
  s32i a2, a15, 0
  ret
  .size SetThunkStackEnd, . - SetThunkStackEnd


/*
 * Thunk code generator macro.  Only works for functions whose parameters
 * are passed in registers (up to 6 params).
 */

.macro thunk fcnName
  .text
  .global thunk_\fcnName
  .type thunk_\fcnName, @function
  .align 4
  thunk_\fcnName:
    addi a1, a1, -16      /* Allocate space for saved registers on stack */

    s32i a0, a1, 12       /* Store A0, trounced by calls */
    s32i a15, a1, 8       /* Store A15 (our temporary one) */

    movi a15, _saveStack  /* Store A1(SP) in temp space */
    s32i a1, a15, 0

    movi a15, _stackEnd   /* Load A1(SP) with thunk stack */
    l32i.n a1, a15, 0

    call0 \fcnName        /* Do the call */

    movi a15, _saveStack  /* Restore A1(SP) */
    l32i.n a1, a15, 0

    l32i.n a15, a1, 8     /* Restore the saved registers */
    l32i.n a0, a1, 12

    addi a1, a1, 16       /* Free up stack and return to caller */
    ret
  .size thunk_\fcnName, . - thunk_\fcnName
.endm

/* Actual thunks */

/* Only a few stack-hungry calls need to be thunked */
thunk br_ssl_engine_recvapp_ack
thunk br_ssl_engine_recvapp_buf
thunk br_ssl_engine_recvrec_ack
thunk br_ssl_engine_recvrec_buf
thunk br_ssl_engine_sendapp_ack
thunk br_ssl_engine_sendapp_buf
thunk br_ssl_engine_sendrec_ack
thunk br_ssl_engine_sendrec_buf
