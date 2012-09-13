/* This file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief AVR32UC C runtime startup file.
 *
 * This file has been built from the Newlib crt0.S.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32UC devices can be used.
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

#include <avr32/io.h>


//! @{
//! \verbatim


  // This must be linked @ 0x80000000 if it is to be run upon reset.
  .section  .reset, "ax", @progbits


  .global _start
  .type _start, @function
_start:
  // Jump to the C runtime startup routine.
  lda.w   pc, _stext


  // _stext is placed outside the .reset section so that the program entry point
  // can be changed without affecting the C runtime startup.
  .section  .text._stext, "ax", @progbits


  .global _stext
  .type _stext, @function
_stext:
  // Set initial stack pointer.
  lda.w   sp, _estack

  // Set up EVBA so interrupts can be enabled.
  lda.w   r0, _evba
  mtsr    AVR32_EVBA, r0

  // Enable the exception processing.
  csrf    AVR32_SR_EM_OFFSET

  // Load initialized data having a global lifetime from the data LMA.
  lda.w   r0, _data
  lda.w   r1, _edata
  cp      r0, r1
  brhs    idata_load_loop_end
  lda.w   r2, _data_lma
idata_load_loop:
  ld.d    r4, r2++
  st.d    r0++, r4
  cp      r0, r1
  brlo    idata_load_loop
idata_load_loop_end:

  // Clear uninitialized data having a global lifetime in the blank static storage section.
  lda.w   r0, __bss_start
  lda.w   r1, _end
  cp      r0, r1
  brhs    udata_clear_loop_end
  mov     r2, 0
  mov     r3, 0
udata_clear_loop:
  st.d    r0++, r2
  cp      r0, r1
  brlo    udata_clear_loop
udata_clear_loop_end:

#ifdef CONFIG_FRAME_POINTER
  // Safety: Set the default "return" @ to the exit routine address.
  lda.w   lr, exit
#endif

  // Start the show.
  lda.w   pc, main


//! \endverbatim
//! @}
