/*
  BearSSLThunks.c - Allow use second stack for BearSSL calls

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

#include <stdint.h>
#include <stdlib.h>

static uint32_t *_stackPtr = NULL;
static uint32_t *_stackTop = NULL;
static uint32_t _refcnt = 0;

#define _stackSize (4500/4)
#define _stackPaint 0xdeadbeef

/* Add a reference, and allocate the stack if necessary */
void br_thunk_add_ref()
{
  _refcnt++;
  if (_refcnt == 1) {
    _stackPtr = (uint32_t *)malloc(_stackSize * sizeof(uint32_t));
    _stackTop = _stackPtr + _stackSize - 1;
    for (int i=0; i < _stackSize; i++) {
      _stackPtr[i] = _stackPaint;
    }
  }
}

/* Drop a reference, and free stack if no more in use */
void br_thunk_del_ref()
{
  if (_refcnt == 0) {
    /* Error! */
    return;
  }
  _refcnt--;
  if (!_refcnt) {
    free(_stackPtr);
    _stackPtr = NULL;
    _stackTop = NULL;
  }
}

/* Return the number of bytes ever used since the stack was created */
uint32_t br_thunk_get_max_usage()
{
  uint32_t cnt = 0;

  /* No stack == no usage by definition! */
  if (!_stackPtr) {
    return 0;
  }

  for (cnt=0; (cnt < _stackSize) && (_stackPtr[cnt] == _stackPaint); cnt++) {
    /* Noop, all work done in for() */
  }
  return 4 * (_stackSize - cnt);
}

__asm("\n\
.data\n\
.align 4\n\
_saveStack: .word 0x00000000\n\
\n\
.text\n\
.literal_position\n\
\n\
.macro thunk fcnName\n\
  .text\n\
  .global thunk_\\fcnName\n\
  .type thunk_\\fcnName, @function\n\
  .align 4\n\
  thunk_\\fcnName:\n\
    addi a1, a1, -16      /* Allocate space for saved registers on stack */\n\
    s32i a0, a1, 12       /* Store A0, trounced by calls */\n\
    s32i a15, a1, 8       /* Store A15 (our temporary one) */\n\
    movi a15, _saveStack  /* Store A1(SP) in temp space */\n\
    s32i a1, a15, 0\n\
    movi a15, _stackTop   /* Load A1(SP) with thunk stack */\n\
    l32i.n a1, a15, 0\n\
    call0 \\fcnName        /* Do the call */\n\
    movi a15, _saveStack  /* Restore A1(SP) */\n\
    l32i.n a1, a15, 0\n\
    l32i.n a15, a1, 8     /* Restore the saved registers */\n\
    l32i.n a0, a1, 12\n\
    addi a1, a1, 16       /* Free up stack and return to caller */\n\
    ret\n\
  .size thunk_\\fcnName, . - thunk_\\fcnName\n\
.endm\n\
\n\
thunk br_ssl_engine_recvapp_ack\n\
thunk br_ssl_engine_recvapp_buf\n\
thunk br_ssl_engine_recvrec_ack\n\
thunk br_ssl_engine_recvrec_buf\n\
thunk br_ssl_engine_sendapp_ack\n\
thunk br_ssl_engine_sendapp_buf\n\
thunk br_ssl_engine_sendrec_ack\n\
thunk br_ssl_engine_sendrec_buf");
