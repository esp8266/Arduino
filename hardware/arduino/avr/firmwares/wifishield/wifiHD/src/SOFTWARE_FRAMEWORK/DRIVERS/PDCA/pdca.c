/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief PDCA driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PDCA interface on AVR32
 * devices.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a PDCA module.
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

#include "compiler.h"
#include "pdca.h"


volatile avr32_pdca_channel_t *pdca_get_handler(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = &AVR32_PDCA.channel[pdca_ch_number];

  if (pdca_ch_number >= AVR32_PDCA_CHANNEL_LENGTH)
    return (volatile avr32_pdca_channel_t *)PDCA_INVALID_ARGUMENT;

  return pdca_channel;
}


int pdca_init_channel(unsigned int pdca_ch_number, const pdca_channel_options_t *opt)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_disable_interrupt_transfer_complete(pdca_ch_number); // disable channel interrupt
  pdca_disable_interrupt_reload_counter_zero(pdca_ch_number); // disable channel interrupt

  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  pdca_channel->mar = (unsigned long)opt->addr;
  pdca_channel->tcr = opt->size;
  pdca_channel->psr = opt->pid;
  pdca_channel->marr = (unsigned long)opt->r_addr;
  pdca_channel->tcrr = opt->r_size;
  pdca_channel->mr =
#if (defined AVR32_PDCA_120_H_INCLUDED ) || (defined AVR32_PDCA_121_H_INCLUDED ) || (defined AVR32_PDCA_122_H_INCLUDED )
opt->etrig << AVR32_PDCA_ETRIG_OFFSET |
#endif // #ifdef AVR32_PDCA_120_H_INCLUDED
                     opt->transfer_size << AVR32_PDCA_SIZE_OFFSET;
  pdca_channel->cr = AVR32_PDCA_ECLR_MASK;
  pdca_channel->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();

  return PDCA_SUCCESS;
}


unsigned int pdca_get_channel_status(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  return (pdca_channel->sr & AVR32_PDCA_TEN_MASK) != 0;
}


void pdca_disable(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  // Disable transfer
  pdca_channel->cr = AVR32_PDCA_TDIS_MASK;

}


void pdca_enable(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  // Enable transfer
  pdca_channel->cr = AVR32_PDCA_TEN_MASK;
}


unsigned int pdca_get_load_size(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  return pdca_channel->tcr;
}


void pdca_load_channel(unsigned int pdca_ch_number, volatile void *addr, unsigned int size)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  pdca_channel->mar = (unsigned long)addr;
  pdca_channel->tcr = size;
  pdca_channel->cr = AVR32_PDCA_ECLR_MASK;
  pdca_channel->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();
}


unsigned int pdca_get_reload_size(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  return pdca_channel->tcrr;
}


void pdca_reload_channel(unsigned int pdca_ch_number, volatile void *addr, unsigned int size)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  // set up next memory address
  pdca_channel->marr = (unsigned long)addr;
  // set up next memory size
  pdca_channel->tcrr = size;
  pdca_channel->cr = AVR32_PDCA_ECLR_MASK;
  pdca_channel->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();
}


void pdca_set_peripheral_select(unsigned int pdca_ch_number, unsigned int pid)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_channel->psr = pid;
}


void pdca_set_transfer_size(unsigned int pdca_ch_number, unsigned int transfer_size)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_channel->mr = (pdca_channel->mr & ~AVR32_PDCA_SIZE_MASK) |
                     transfer_size << AVR32_PDCA_SIZE_OFFSET;
}


#if (defined AVR32_PDCA_120_H_INCLUDED ) || (defined AVR32_PDCA_121_H_INCLUDED ) || (defined AVR32_PDCA_122_H_INCLUDED )


void pdca_disable_event_trigger(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_channel->mr &= ~AVR32_PDCA_ETRIG_MASK;
}


void pdca_enable_event_trigger(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_channel->mr |= AVR32_PDCA_ETRIG_MASK;
}


#endif // #ifdef AVR32_PDCA_120_H_INCLUDED


void pdca_disable_interrupt_transfer_error(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  pdca_channel->idr = AVR32_PDCA_TERR_MASK;
  pdca_channel->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();
}


void pdca_enable_interrupt_transfer_error(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_channel->ier = AVR32_PDCA_TERR_MASK;
}


void pdca_disable_interrupt_transfer_complete(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  pdca_channel->idr = AVR32_PDCA_TRC_MASK;
  pdca_channel->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();
}


void pdca_enable_interrupt_transfer_complete(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_channel->ier = AVR32_PDCA_TRC_MASK;
}


void pdca_disable_interrupt_reload_counter_zero(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  pdca_channel->idr = AVR32_PDCA_RCZ_MASK;
  pdca_channel->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();
}


void pdca_enable_interrupt_reload_counter_zero(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  pdca_channel->ier = AVR32_PDCA_RCZ_MASK;
}


unsigned long pdca_get_transfer_status(unsigned int pdca_ch_number)
{
  // get the correct channel pointer
  volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(pdca_ch_number);

  return pdca_channel->isr;
}
