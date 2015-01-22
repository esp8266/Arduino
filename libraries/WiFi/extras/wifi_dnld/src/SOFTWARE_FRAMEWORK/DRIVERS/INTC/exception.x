/* This file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Exception and interrupt vectors.
 *
 * This file maps all events supported by an AVR32.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with an INTC module can be used.
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

#if !__AVR32_UC__ && !__AVR32_AP__
  #error Implementation of the AVR32 architecture not supported by the INTC driver.
#endif


#include <avr32/io.h>


//! @{
//! \verbatim


  .section  .exception, "ax", @progbits


// Start of Exception Vector Table.

  // EVBA must be aligned with a power of two strictly greater than the EVBA-
  // relative offset of the last vector.
  .balign 0x200

  // Export symbol.
  .global _evba
  .type _evba, @function
_evba:

        .org  0x000
        // Unrecoverable Exception.
_handle_Unrecoverable_Exception:
        rjmp $

        .org  0x004
        // TLB Multiple Hit.
_handle_TLB_Multiple_Hit:
        rjmp $

        .org  0x008
        // Bus Error Data Fetch.
_handle_Bus_Error_Data_Fetch:
        rjmp $

        .org  0x00C
         // Bus Error Instruction Fetch.
_handle_Bus_Error_Instruction_Fetch:
        rjmp $

        .org  0x010
        // NMI.
_handle_NMI:
        rjmp $

        .org  0x014
        // Instruction Address.
_handle_Instruction_Address:
        rjmp $

        .org  0x018
        // ITLB Protection.
_handle_ITLB_Protection:
        rjmp $

        .org  0x01C
        // Breakpoint.
_handle_Breakpoint:
        rjmp $

        .org  0x020
        // Illegal Opcode.
_handle_Illegal_Opcode:
        rjmp $

        .org  0x024
        // Unimplemented Instruction.
_handle_Unimplemented_Instruction:
        rjmp $

        .org  0x028
        // Privilege Violation.
_handle_Privilege_Violation:
        rjmp $

        .org  0x02C
        // Floating-Point: UNUSED IN AVR32UC and AVR32AP.
_handle_Floating_Point:
        rjmp $

        .org  0x030
        // Coprocessor Absent: UNUSED IN AVR32UC.
_handle_Coprocessor_Absent:
        rjmp $

        .org  0x034
        // Data Address (Read).
_handle_Data_Address_Read:
        rjmp $

        .org  0x038
        // Data Address (Write).
_handle_Data_Address_Write:
        rjmp $

        .org  0x03C
        // DTLB Protection (Read).
_handle_DTLB_Protection_Read:
        rjmp $

        .org  0x040
        // DTLB Protection (Write).
_handle_DTLB_Protection_Write:
        rjmp $

        .org  0x044
        // DTLB Modified: UNUSED IN AVR32UC.
_handle_DTLB_Modified:
        rjmp $

        .org  0x050
        // ITLB Miss.
_handle_ITLB_Miss:
        rjmp $

        .org  0x060
        // DTLB Miss (Read).
_handle_DTLB_Miss_Read:
        rjmp $

        .org  0x070
        // DTLB Miss (Write).
_handle_DTLB_Miss_Write:
        rjmp $

        .org  0x100
        // Supervisor Call.
_handle_Supervisor_Call:
        rjmp $


// Interrupt support.
// The interrupt controller must provide the offset address relative to EVBA.
// Important note:
//   All interrupts call a C function named _get_interrupt_handler.
//   This function will read group and interrupt line number to then return in
//   R12 a pointer to a user-provided interrupt handler.

  .balign 4

  .irp    priority, 0, 1, 2, 3
_int\priority:
#if __AVR32_UC__
  // R8-R12, LR, PC and SR are automatically pushed onto the system stack by the
  // CPU upon interrupt entry. No other register is saved by hardware.
#elif __AVR32_AP__
  // PC and SR are automatically saved in respectively RAR_INTx and RSR_INTx by
  // the CPU upon interrupt entry. No other register is saved by hardware.
  pushm   r8-r12, lr
#endif
  mov     r12, \priority  // Pass the int_level parameter to the _get_interrupt_handler function.
  call    _get_interrupt_handler
  cp.w    r12, 0          // Get the pointer to the interrupt handler returned by the function.
#if __AVR32_UC__
  movne   pc, r12         // If this was not a spurious interrupt (R12 != NULL), jump to the handler.
#elif __AVR32_AP__
  breq    spint\priority  // If this was a spurious interrupt (R12 == NULL), branch.
  st.w    --sp, r12       // Push the pointer to the interrupt handler onto the system stack since no register may be altered.
  popm    r8-r12, lr, pc  // Restore registers and jump to the handler.
spint\priority:
  popm    r8-r12, lr
#endif
  rete                    // If this was a spurious interrupt (R12 == NULL), return from event handler.
  .endr


// Constant data area.

  .balign 4

  // Values to store in the interrupt priority registers for the various interrupt priority levels.
  // The interrupt priority registers contain the interrupt priority level and
  // the EVBA-relative interrupt vector offset.
  .global ipr_val
  .type ipr_val, @object
ipr_val:
  .word (AVR32_INTC_INT0 << AVR32_INTC_IPR_INTLEVEL_OFFSET) | (_int0 - _evba),\
        (AVR32_INTC_INT1 << AVR32_INTC_IPR_INTLEVEL_OFFSET) | (_int1 - _evba),\
        (AVR32_INTC_INT2 << AVR32_INTC_IPR_INTLEVEL_OFFSET) | (_int2 - _evba),\
        (AVR32_INTC_INT3 << AVR32_INTC_IPR_INTLEVEL_OFFSET) | (_int3 - _evba)


//! \endverbatim
//! @}
