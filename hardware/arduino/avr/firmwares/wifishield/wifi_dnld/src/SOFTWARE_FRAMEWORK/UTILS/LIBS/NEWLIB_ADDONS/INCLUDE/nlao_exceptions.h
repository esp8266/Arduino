/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief NEWLIB_ADDONS exceptions include file for AVR32.
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

#ifndef __AVR32_NEWLIB_ADDONS_EXCEPTIONS_H__
#define __AVR32_NEWLIB_ADDONS_EXCEPTIONS_H__

#include <_ansi.h>

_BEGIN_STD_C

/*
 Exception vector offsets
*/
#define EVBA_UNRECOVERABLE    0x000
#define EVBA_TLB_MULTIPLE     0x004
#define EVBA_BUS_ERROR_DATA   0x008
#define EVBA_BUS_ERROR_INSTR  0x00C
#define EVBA_NMI              0x010
#define EVBA_INSTR_ADDR       0x014
#define EVBA_ITLB_MISS        0x050
#define EVBA_ITLB_PROT        0x018
#define EVBA_BREAKPOINT       0x01C
#define EVBA_ILLEGAL_OPCODE   0x020
#define EVBA_UNIMPLEMENTED    0x024
#define EVBA_PRIVILEGE_VIOL   0x028
#define EVBA_FLOATING_POINT   0x02C
#define EVBA_COP_ABSENT       0x030
#define EVBA_SCALL            0x100
#define EVBA_DATA_ADDR_R      0x034
#define EVBA_DATA_ADDR_W      0x038
#define EVBA_DTLB_MISS_R      0x060
#define EVBA_DTLB_MISS_W      0x070
#define EVBA_DTLB_PROT_R      0x03C
#define EVBA_DTLB_PROT_W      0x040
#define EVBA_DTLB_MODIFIED    0x044


/*
   Define the form of the function used when registering exceptions.
   The function should return the address which the exception should
   return to after the exception processing.
*/

typedef unsigned int (*__exception_handler)(int /*evba_offset*/, int /*return address*/);

/*
   Define the form of the function used when registering a scall handler.
*/

typedef void (*__scall_handler)(int /*code*/, int /*p1*/, int /*p2*/
                                         , int /*p3*/, int /*p4*/);

/*
   Function for registering an exception handler for the exception with
   offset given by evba_offset.
*/
void _register_exception_handler(__exception_handler handler, int evba_offset);

/*
   Function for registering a scall handler which can be a arbirary
   function which uses r8-r12 for parameters.
*/
void _register_scall_handler(__scall_handler handler);

/*
   Initialize exceptions. Must be called before registering exception handlers
   and needed to enable exceptions. 'evba' is the pointer to the exception
   vector. 'handler_table' is a pointer to an array where the pointers to
   the exception handlers are stored. This array must be at least 0x104 bytes
   and word aligned.
*/
void init_exceptions(void *evba, void *handler_table);

_END_STD_C

#endif
