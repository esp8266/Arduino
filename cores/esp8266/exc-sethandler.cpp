/*
 * Adaptation of _xtos_set_exception_handler for Arduino ESP8266 core
 *
 * This replacement for the Boot ROM `_xtos_set_exception_handler` is used to
 * install our replacement `_xtos_c_wrapper_handler`. This change protects the
 * value of `excvaddr` from corruption.
 *
 *
 *                             Details
 *
 * The issue, the Boot ROM "C" wrapper for exception handlers,
 * `_xtos_c_wrapper_handler`, turns interrupts back on. This leaves `excvaddr`
 * exposed to possible overwrite before it is read. For example, if an interrupt
 * is taken during the exception handler processing and the ISR handler
 * generates a new exception, the original value of `excvaddr` is lost.  To
 * address this issue we have a replacement `_xtos_c_wrapper_handler` in file
 * `exc-c-wrapper-handler.S`.
 *
 * An overview, of an exception at entry: New interrupts are blocked by EXCM
 * being set. Once cleared, interrupts above the current INTLEVEL and exceptions
 * (w/o creating a DoubleException) can occur.
 *
 * Using our replacement for `_xtos_c_wrapper_handler`, INTLEVEL is raised to 15
 * with EXCM cleared.
 *
 * The original Boot ROM `_xtos_c_wrapper_handler` at entry would set INTLEVEL
 * to 3 with EXCM cleared, save registers, then do a `rsil 0` (interrupts fully
 * enabled!) just before calling the registered "C" Exception handler. Our
 * replacement keeps INTLEVEL at 15. This is needed to support the Arduino model
 * of interrupts disabled while an ISR runs.
 *
 * And we also need it for umm_malloc to work safely with an IRAM heap from an
 * ISR call. While malloc() will supply DRAM for all allocation from an ISR, we
 * want free() to safely operate from an ISR to avoid a leak potential.
 *
 * If an exception handler needs interrupts enabled, it would be done after it
 * has consumed the value of `excvaddr`. Whether such action is safe is left to
 * the exception handler writer to determine. However, with our current
 * architecture, I am not convinced it can be done safely.
 *
*/

#if defined(NON32XFER_HANDLER) || defined(MMU_IRAM_HEAP) || \
    defined(NEW_EXC_C_WRAPPER) || defined(MMU_EXTERNAL_HEAP) || (NONOSDK >= (0x30000 - 1))

/*
 * The original module source code came from:
 *   https://github.com/qca/open-ath9k-htc-firmware/blob/master/sboot/magpie_1_1/sboot/athos/src/xtos/exc-sethandler.c
 *
 * It has been revised to use Arduino ESP8266 core includes, types, and
 * formatting.
*/

/* exc-sethandler.c - register an exception handler in XTOS */

/*
 * Copyright (c) 1999-2006 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Arduino.h>
#include "esp8266_undocumented.h"

extern "C" {

/*
 *  Register a C handler for the specified general exception
 *  (specified EXCCAUSE value).
 */
fn_c_exception_handler_t _xtos_set_exception_handler(int cause, fn_c_exception_handler_t fn)
{
    fn_c_exception_handler_t ret;

    if( (unsigned) cause >= XCHAL_EXCCAUSE_NUM )
        return 0;

    if( fn == 0 )
        fn = &_xtos_p_none;

    ret = _xtos_c_handler_table[cause];

    _xtos_exc_handler_table[cause] = ( (fn == &_xtos_p_none)
				 ? &_xtos_unhandled_exception
				 : &_xtos_c_wrapper_handler );

    _xtos_c_handler_table[cause] = fn;

    if( ret == &_xtos_p_none )
        ret = 0;

    return ret;
}

};

#endif
