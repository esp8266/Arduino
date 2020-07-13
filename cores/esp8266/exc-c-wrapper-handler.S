// exc-c-wrapper-handler.S, this is a reduced version of the original file at
// https://github.com/qca/open-ath9k-htc-firmware/blob/master/sboot/magpie_1_1/sboot/athos/src/xtos/exc-c-wrapper-handler.S#L62-L67
//

// exc-c-wrapper-handler.S - General Exception Handler that Dispatches C Handlers

// Copyright (c) 2002-2004, 2006-2007, 2010 Tensilica Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/specreg.h>
// #include "xtos-internal.h"
// #ifdef SIMULATOR
// #include <xtensa/simcall.h>
// #endif

#include "xtruntime-frames.h"

/*
 *  This is the general exception assembly-level handler that dispatches C handlers.
 */
        .section .iram.text
        .align  4
        .literal_position
        .global  _xtos_c_wrapper_handler
_xtos_c_wrapper_handler:

        //  HERE:  a2, a3, a4 have been saved to exception stack frame allocated with a1 (sp).
        //  a2 contains EXCCAUSE.
        s32i  a5, a1, UEXC_a5   // a5 will get clobbered by ENTRY after the pseudo-CALL4
                                //   (a4..a15 spilled as needed; save if modified)

        //NOTA:  Possible future improvement:
        //  keep interrupts disabled until we get into the handler, such that
        //  we don't have to save other critical state such as EXCVADDR here.
// @mhightower83 - This promise was broken by an "rsil a13, 0" below.
        //rsr  a3, EXCVADDR
        s32i  a2, a1, UEXC_exccause
        //s32i  a3, a1, UEXC_excvaddr

        //  Set PS fields:
        //  EXCM     = 0
        //  WOE      = __XTENSA_CALL0_ABI__ ? 0 : 1
        //  UM       = 1
        //  INTLEVEL = EXCM_LEVEL = 1
        //  CALLINC  = __XTENSA_CALL0_ABI__ ? 0 : 1
        //  OWB      = 0 (really, a dont care if !__XTENSA_CALL0_ABI__)

//        movi   a2, 0x23 // 0x21, PS_UM|PS_INTLEVEL(XCHAL_EXCM_LEVEL)
// @mhightower83 - testing to see if we need INTLEVEL 15 instead of 3
        movi   a2, 0x2F // 0x21, PS_UM|PS_INTLEVEL(XCHAL_EXCM_LEVEL)
        rsr    a3, EPC_1
// @mhightower83 - I assume PS.EXCM was set and now is being cleared, thus
// allowing new exceptions and interrupts within PS_INTLEVEL to be possible.
        xsr    a2, PS

        //  HERE:  window overflows enabled, but NOT SAFE because we're not quite
        //  in a valid windowed context (haven't restored a1 yet...);
        //  so don't cause any (keep to a0..a3) until we've saved critical state and restored a1:

        //  NOTE:  MUST SAVE EPC1 before causing any overflows, because overflows corrupt EPC1.
        s32i   a3, a1, UEXC_pc
        s32i   a2, a1, UEXC_ps
        s32i   a0, a1, UEXC_a0    // save the rest of the registers
        s32i   a6, a1, UEXC_a6
        s32i   a7, a1, UEXC_a7
        s32i   a8, a1, UEXC_a8
        s32i   a9, a1, UEXC_a9
        s32i  a10, a1, UEXC_a10
        s32i  a11, a1, UEXC_a11
        s32i  a12, a1, UEXC_a12
        s32i  a13, a1, UEXC_a13
        s32i  a14, a1, UEXC_a14
        s32i  a15, a1, UEXC_a15
        rsync        // wait for WSR to PS to complete
        rsr     a12,  SAR

// @mhightower83 - I think after the next instruction we have the potential of
// loosing UEXC_excvaddr Which the earlier comment said we need to preserve for
// the exception handler.
//
//        rsil    a13,  0

        movi    a13,  _xtos_c_handler_table   // &table
        l32i    a15,  a1, UEXC_exccause       // arg2: exccause
        s32i    a12,  a1, UEXC_sar
        addx4   a12, a15, a13  // a12 = table[exccause]
        l32i    a12, a12, 0    // ...
        mov      a2,  a1       // arg1: exception parameters
        mov      a3, a15       // arg2: exccause
        beqz    a12,  1f       // null handler => skip call
        callx0  a12            // call C exception handler for this exception
1:
        //  Now exit the handler.

        // Restore special registers
        l32i    a14,  a1, UEXC_sar

        /*
         *  Disable interrupts while returning from the pseudo-CALL setup above,
         *  for the same reason they were disabled while doing the pseudo-CALL:
         *  this sequence restores SP such that it doesn't reflect the allocation
         *  of the exception stack frame, which we still need to return from
         *  the exception.
         */
//        rsil  a12, 1 // XCHAL_EXCM_LEVEL
        wsr   a14, SAR
        movi   a0, _xtos_return_from_exc
        jx     a0

        /* FIXME: what about _GeneralException ? */
        .size  _xtos_c_wrapper_handler, . - _xtos_c_wrapper_handler
