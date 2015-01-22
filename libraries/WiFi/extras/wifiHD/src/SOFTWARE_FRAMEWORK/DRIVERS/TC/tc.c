/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief TC driver for AVR32 UC3.
 *
 * AVR32 Timer/Counter driver module.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a TC module can be used.
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
#include "tc.h"


int tc_get_interrupt_settings(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  return tc->channel[channel].imr;
}


int tc_configure_interrupts(volatile avr32_tc_t *tc, unsigned int channel, const tc_interrupt_t *bitfield)
{
  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // Enable the appropriate interrupts.
  tc->channel[channel].ier = bitfield->etrgs << AVR32_TC_ETRGS_OFFSET |
                             bitfield->ldrbs << AVR32_TC_LDRBS_OFFSET |
                             bitfield->ldras << AVR32_TC_LDRAS_OFFSET |
                             bitfield->cpcs << AVR32_TC_CPCS_OFFSET |
                             bitfield->cpbs << AVR32_TC_CPBS_OFFSET |
                             bitfield->cpas << AVR32_TC_CPAS_OFFSET |
                             bitfield->lovrs << AVR32_TC_LOVRS_OFFSET |
                             bitfield->covfs << AVR32_TC_COVFS_OFFSET;

  // Disable the appropriate interrupts.
  if (global_interrupt_enabled) Disable_global_interrupt();
  tc->channel[channel].idr = (~bitfield->etrgs & 1) << AVR32_TC_ETRGS_OFFSET |
                             (~bitfield->ldrbs & 1) << AVR32_TC_LDRBS_OFFSET |
                             (~bitfield->ldras & 1) << AVR32_TC_LDRAS_OFFSET |
                             (~bitfield->cpcs & 1) << AVR32_TC_CPCS_OFFSET |
                             (~bitfield->cpbs & 1) << AVR32_TC_CPBS_OFFSET |
                             (~bitfield->cpas & 1) << AVR32_TC_CPAS_OFFSET |
                             (~bitfield->lovrs & 1) << AVR32_TC_LOVRS_OFFSET |
                             (~bitfield->covfs & 1) << AVR32_TC_COVFS_OFFSET;
  tc->channel[channel].sr;
  if (global_interrupt_enabled) Enable_global_interrupt();

  return 0;
}


int tc_select_external_clock(volatile avr32_tc_t *tc, unsigned int channel, unsigned int ext_clk_sig_src)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS || ext_clk_sig_src >= 1 << AVR32_TC_BMR_TC0XC0S_SIZE)
    return TC_INVALID_ARGUMENT;

  // Clear bit-field and set the correct behavior.
  tc->bmr = (tc->bmr & ~(AVR32_TC_BMR_TC0XC0S_MASK << (channel * AVR32_TC_BMR_TC0XC0S_SIZE))) |
            (ext_clk_sig_src << (channel * AVR32_TC_BMR_TC0XC0S_SIZE));

  return 0;
}


int tc_init_capture(volatile avr32_tc_t *tc, const tc_capture_opt_t *opt)
{
  // Check for valid input.
  if (opt->channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // MEASURE SIGNALS: Capture operating mode.
  tc->channel[opt->channel].cmr = opt->ldrb << AVR32_TC_LDRB_OFFSET |
                                  opt->ldra << AVR32_TC_LDRA_OFFSET |
                                  0 << AVR32_TC_WAVE_OFFSET |
                                  opt->cpctrg << AVR32_TC_CPCTRG_OFFSET |
                                  opt->abetrg << AVR32_TC_ABETRG_OFFSET |
                                  opt->etrgedg << AVR32_TC_ETRGEDG_OFFSET|
                                  opt->ldbdis << AVR32_TC_LDBDIS_OFFSET |
                                  opt->ldbstop << AVR32_TC_LDBSTOP_OFFSET |
                                  opt->burst << AVR32_TC_BURST_OFFSET |
                                  opt->clki << AVR32_TC_CLKI_OFFSET |
                                  opt->tcclks << AVR32_TC_TCCLKS_OFFSET;

  return 0;
}


int tc_init_waveform(volatile avr32_tc_t *tc, const tc_waveform_opt_t *opt)
{
  // Check for valid input.
  if (opt->channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // GENERATE SIGNALS: Waveform operating mode.
  tc->channel[opt->channel].cmr = opt->bswtrg << AVR32_TC_BSWTRG_OFFSET |
                                  opt->beevt << AVR32_TC_BEEVT_OFFSET |
                                  opt->bcpc << AVR32_TC_BCPC_OFFSET |
                                  opt->bcpb << AVR32_TC_BCPB_OFFSET |
                                  opt->aswtrg << AVR32_TC_ASWTRG_OFFSET |
                                  opt->aeevt << AVR32_TC_AEEVT_OFFSET |
                                  opt->acpc << AVR32_TC_ACPC_OFFSET |
                                  opt->acpa << AVR32_TC_ACPA_OFFSET |
                                  1 << AVR32_TC_WAVE_OFFSET |
                                  opt->wavsel << AVR32_TC_WAVSEL_OFFSET |
                                  opt->enetrg << AVR32_TC_ENETRG_OFFSET |
                                  opt->eevt << AVR32_TC_EEVT_OFFSET |
                                  opt->eevtedg << AVR32_TC_EEVTEDG_OFFSET |
                                  opt->cpcdis << AVR32_TC_CPCDIS_OFFSET |
                                  opt->cpcstop << AVR32_TC_CPCSTOP_OFFSET |
                                  opt->burst << AVR32_TC_BURST_OFFSET |
                                  opt->clki << AVR32_TC_CLKI_OFFSET |
                                  opt->tcclks << AVR32_TC_TCCLKS_OFFSET;

  return 0;
}


int tc_start(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // Enable, reset and start the selected timer/counter channel.
  tc->channel[channel].ccr = AVR32_TC_SWTRG_MASK | AVR32_TC_CLKEN_MASK;

  return 0;
}


int tc_stop(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // Disable the selected timer/counter channel.
  tc->channel[channel].ccr = AVR32_TC_CLKDIS_MASK;

  return 0;
}


int tc_software_trigger(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // Reset the selected timer/counter channel.
  tc->channel[channel].ccr = AVR32_TC_SWTRG_MASK;

  return 0;
}


void tc_sync_trigger(volatile avr32_tc_t *tc)
{
  // Reset all channels of the selected timer/counter.
  tc->bcr = AVR32_TC_BCR_SYNC_MASK;
}


void tc_sync_start(volatile avr32_tc_t *tc)
{
  unsigned int i;
  // Enable the clock for each channel.
  for(i=0; i<TC_NUMBER_OF_CHANNELS;i++)
    tc->channel[i].ccr = AVR32_TC_CLKEN_MASK;
    
  // Reset all channels of the selected timer/counter.
  tc->bcr = AVR32_TC_BCR_SYNC_MASK;
}


int tc_read_sr(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  return tc->channel[channel].sr;
}


int tc_read_tc(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  return Rd_bitfield(tc->channel[channel].cv, AVR32_TC_CV_MASK);
}


int tc_read_ra(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  return Rd_bitfield(tc->channel[channel].ra, AVR32_TC_RA_MASK);
}


int tc_read_rb(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  return Rd_bitfield(tc->channel[channel].rb, AVR32_TC_RB_MASK);
}


int tc_read_rc(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  return Rd_bitfield(tc->channel[channel].rc, AVR32_TC_RC_MASK);
}


int tc_write_ra(volatile avr32_tc_t *tc, unsigned int channel, unsigned short value)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // This function is only available in WAVEFORM mode.
  if (Tst_bits(tc->channel[channel].cmr, AVR32_TC_WAVE_MASK))
    Wr_bitfield(tc->channel[channel].ra, AVR32_TC_RA_MASK, value);

  return value;
}


int tc_write_rb(volatile avr32_tc_t *tc, unsigned int channel, unsigned short value)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // This function is only available in WAVEFORM mode.
  if (Tst_bits(tc->channel[channel].cmr, AVR32_TC_WAVE_MASK))
    Wr_bitfield(tc->channel[channel].rb, AVR32_TC_RB_MASK, value);

  return value;
}


int tc_write_rc(volatile avr32_tc_t *tc, unsigned int channel, unsigned short value)
{
  // Check for valid input.
  if (channel >= TC_NUMBER_OF_CHANNELS)
    return TC_INVALID_ARGUMENT;

  // This function is only available in WAVEFORM mode.
  if (Tst_bits(tc->channel[channel].cmr, AVR32_TC_WAVE_MASK))
    Wr_bitfield(tc->channel[channel].rc, AVR32_TC_RC_MASK, value);

  return value;
}
