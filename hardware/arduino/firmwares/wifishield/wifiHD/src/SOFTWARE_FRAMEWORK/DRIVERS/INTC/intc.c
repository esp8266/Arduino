/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief INTC driver for AVR32 UC3.
 *
 * AVR32 Interrupt Controller driver module.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
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

#include <avr32/io.h>
#include "compiler.h"
#include "preprocessor.h"
#include "intc.h"

// define _evba from exception.S
extern void _evba;

//! Values to store in the interrupt priority registers for the various interrupt priority levels.
extern const unsigned int ipr_val[AVR32_INTC_NUM_INT_LEVELS];

//! Creates a table of interrupt line handlers per interrupt group in order to optimize RAM space.
//! Each line handler table contains a set of pointers to interrupt handlers.
#if (defined __GNUC__)
#define DECL_INT_LINE_HANDLER_TABLE(GRP, unused) \
static volatile __int_handler _int_line_handler_table_##GRP[Max(AVR32_INTC_NUM_IRQS_PER_GRP##GRP, 1)];
#elif (defined __ICCAVR32__)
#define DECL_INT_LINE_HANDLER_TABLE(GRP, unused) \
static volatile __no_init __int_handler _int_line_handler_table_##GRP[Max(AVR32_INTC_NUM_IRQS_PER_GRP##GRP, 1)];
#endif
MREPEAT(AVR32_INTC_NUM_INT_GRPS, DECL_INT_LINE_HANDLER_TABLE, ~);
#undef DECL_INT_LINE_HANDLER_TABLE

//! Table containing for each interrupt group the number of interrupt request
//! lines and a pointer to the table of interrupt line handlers.
static const struct
{
  unsigned int num_irqs;
  volatile __int_handler *_int_line_handler_table;
} _int_handler_table[AVR32_INTC_NUM_INT_GRPS] =
{
#define INSERT_INT_LINE_HANDLER_TABLE(GRP, unused) \
  {AVR32_INTC_NUM_IRQS_PER_GRP##GRP, _int_line_handler_table_##GRP},
  MREPEAT(AVR32_INTC_NUM_INT_GRPS, INSERT_INT_LINE_HANDLER_TABLE, ~)
#undef INSERT_INT_LINE_HANDLER_TABLE
};


/*! \brief Default interrupt handler.
 *
 * \note Taken and adapted from Newlib.
 */
#if (defined __GNUC__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void _unhandled_interrupt(void)
{
  // Catch unregistered interrupts.
  while (TRUE);
}


/*! \brief Gets the interrupt handler of the current event at the \a int_level
 *         interrupt priority level (called from exception.S).
 *
 * \param int_level Interrupt priority level to handle.
 *
 * \return Interrupt handler to execute.
 *
 * \note Taken and adapted from Newlib.
 */
__int_handler _get_interrupt_handler(unsigned int int_level)
{
  // ICR3 is mapped first, ICR0 last.
  // Code in exception.S puts int_level in R12 which is used by AVR32-GCC to
  // pass a single argument to a function.
  unsigned int int_grp = AVR32_INTC.icr[AVR32_INTC_INT3 - int_level];
  unsigned int int_req = AVR32_INTC.irr[int_grp];

  // As an interrupt may disappear while it is being fetched by the CPU
  // (spurious interrupt caused by a delayed response from an MCU peripheral to
  // an interrupt flag clear or interrupt disable instruction), check if there
  // are remaining interrupt lines to process.
  // If a spurious interrupt occurs, the status register (SR) contains an
  // execution mode and interrupt level masks corresponding to a level 0
  // interrupt, whatever the interrupt priority level causing the spurious
  // event. This behavior has been chosen because a spurious interrupt has not
  // to be a priority one and because it may not cause any trouble to other
  // interrupts.
  // However, these spurious interrupts place the hardware in an unstable state
  // and could give problems in other/future versions of the CPU, so the
  // software has to be written so that they never occur. The only safe way of
  // achieving this is to always clear or disable peripheral interrupts with the
  // following sequence:
  // 1: Mask the interrupt in the CPU by setting GM (or IxM) in SR.
  // 2: Perform the bus access to the peripheral register that clears or
  //    disables the interrupt.
  // 3: Wait until the interrupt has actually been cleared or disabled by the
  //    peripheral. This is usually performed by reading from a register in the
  //    same peripheral (it DOES NOT have to be the same register that was
  //    accessed in step 2, but it MUST be in the same peripheral), what takes
  //    bus system latencies into account, but peripheral internal latencies
  //    (generally 0 cycle) also have to be considered.
  // 4: Unmask the interrupt in the CPU by clearing GM (or IxM) in SR.
  // Note that steps 1 and 4 are useless inside interrupt handlers as the
  // corresponding interrupt level is automatically masked by IxM (unless IxM is
  // explicitly cleared by the software).
  //
  // Get the right IRQ handler.
  //
  // If several interrupt lines are active in the group, the interrupt line with
  // the highest number is selected. This is to be coherent with the
  // prioritization of interrupt groups performed by the hardware interrupt
  // controller.
  //
  // If no handler has been registered for the pending interrupt,
  // _unhandled_interrupt will be selected thanks to the initialization of
  // _int_line_handler_table_x by INTC_init_interrupts.
  //
  // exception.S will provide the interrupt handler with a clean interrupt stack
  // frame, with nothing more pushed onto the stack. The interrupt handler must
  // manage the `rete' instruction, what can be done thanks to pure assembly,
  // inline assembly or the `__attribute__((__interrupt__))' C function
  // attribute.
  return (int_req) ? _int_handler_table[int_grp]._int_line_handler_table[32 - clz(int_req) - 1] : NULL;
}

//! Init EVBA address. This sequence might also be done in the UTILS/STARTUP/GCC/crt0.S
static __inline__ void INTC_init_evba(void)
{
  Set_system_register(AVR32_EVBA, (int)&_evba );
}

void INTC_init_interrupts(void)
{
  unsigned int int_grp, int_req;

  INTC_init_evba();

  // For all interrupt groups,
  for (int_grp = 0; int_grp < AVR32_INTC_NUM_INT_GRPS; int_grp++)
  {
    // For all interrupt request lines of each group,
    for (int_req = 0; int_req < _int_handler_table[int_grp].num_irqs; int_req++)
    {
      // Assign _unhandled_interrupt as default interrupt handler.
      _int_handler_table[int_grp]._int_line_handler_table[int_req] = &_unhandled_interrupt;
    }

    // Set the interrupt group priority register to its default value.
    // By default, all interrupt groups are linked to the interrupt priority
    // level 0 and to the interrupt vector _int0.
    AVR32_INTC.ipr[int_grp] = ipr_val[AVR32_INTC_INT0];
  }
}


void INTC_register_interrupt(__int_handler handler, unsigned int irq, unsigned int int_level)
{
  // Determine the group of the IRQ.
  unsigned int int_grp = irq / AVR32_INTC_MAX_NUM_IRQS_PER_GRP;

  // Store in _int_line_handler_table_x the pointer to the interrupt handler, so
  // that _get_interrupt_handler can retrieve it when the interrupt is vectored.
  _int_handler_table[int_grp]._int_line_handler_table[irq % AVR32_INTC_MAX_NUM_IRQS_PER_GRP] = handler;

  // Program the corresponding IPRX register to set the interrupt priority level
  // and the interrupt vector offset that will be fetched by the core interrupt
  // system.
  // NOTE: The _intx functions are intermediate assembly functions between the
  // core interrupt system and the user interrupt handler.
  AVR32_INTC.ipr[int_grp] = ipr_val[int_level & (AVR32_INTC_IPR_INTLEVEL_MASK >> AVR32_INTC_IPR_INTLEVEL_OFFSET)];
}
