/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief NEWLIB_ADDONS interrupts include file for AVR32.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef __AVR32_NEWLIB_ADDONS_INTERRUPTS_H__
#define __AVR32_NEWLIB_ADDONS_INTERRUPTS_H__

#include <_ansi.h>

_BEGIN_STD_C

#define INT_GRPS          64
#define INT_LINES         32
#define INTPR_BASE        (__intc_base__ + 0x0000)
#define INTREQ_BASE       (__intc_base__ + 64*4)
#define INTCAUSE_BASE     (__intc_base__ + 2*64*4)

//Register offsets
#define INTLEVEL          30
#define AUTOVECTOR        0
#define AUTOVECTOR_BITS   14

//Priorities
#define INT0              0
#define INT1              1
#define INT2              2
#define INT3              3


typedef void (*__newlib_int_handler)(int /* int_grp*/, void */*user_handle*/);

__newlib_int_handler register_interrupt(__newlib_int_handler handler, int int_grp, int line, int priority,
                                 .../* void *user_handle*/);
void init_interrupts();
void set_interrupts_base(void *base);

_END_STD_C

#endif
