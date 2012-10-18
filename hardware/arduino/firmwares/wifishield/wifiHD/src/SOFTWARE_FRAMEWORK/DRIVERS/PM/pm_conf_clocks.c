/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Power Manager clocks configuration helper.
 *
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

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

#include <string.h>
#include "compiler.h"
#include "pm.h"

extern void flashc_set_wait_state(unsigned int wait_state);
#if (defined AVR32_FLASHC_210_H_INCLUDED)
extern void flashc_issue_command(unsigned int command, int page_number);
#endif


#define PM_MAX_MUL                ((1 << AVR32_PM_PLL0_PLLMUL_SIZE) - 1)


int pm_configure_clocks(pm_freq_param_t *param)
{
  // Supported frequencies:
  // Fosc0 mul div PLL div2_en cpu_f pba_f   Comment
  //  12   15   1  192     1     12    12
  //  12    9   3   40     1     20    20    PLL out of spec
  //  12   15   1  192     1     24    12
  //  12    9   1  120     1     30    15
  //  12    9   3   40     0     40    20    PLL out of spec
  //  12   15   1  192     1     48    12
  //  12   15   1  192     1     48    24
  //  12    8   1  108     1     54    27
  //  12    9   1  120     1     60    15
  //  12    9   1  120     1     60    30
  //  12   10   1  132     1     66    16.5
  //
  unsigned long in_cpu_f  = param->cpu_f;
  unsigned long in_osc0_f = param->osc0_f;
  unsigned long mul, div, div2_en = 0, div2_cpu = 0, div2_pba = 0;
  unsigned long pll_freq, rest;
  Bool b_div2_pba, b_div2_cpu;

  // Switch to external Oscillator 0
  pm_switch_to_osc0(&AVR32_PM, in_osc0_f, param->osc0_startup);

  // Start with CPU freq config
  if (in_cpu_f == in_osc0_f)
  {
    param->cpu_f = in_osc0_f;
    param->pba_f = in_osc0_f;
    return PM_FREQ_STATUS_OK;
  }
  else if (in_cpu_f < in_osc0_f)
  {
    // TBD
  }

  rest = in_cpu_f % in_osc0_f;

  for (div = 1; div < 32; div++)
  {
    if ((div * rest) % in_osc0_f == 0)
      break;
  }
  if (div == 32)
    return PM_FREQ_STATUS_FAIL;

  mul = (in_cpu_f * div) / in_osc0_f;

  if (mul > PM_MAX_MUL)
    return PM_FREQ_STATUS_FAIL;

  // export 2power from PLL div to div2_cpu
  while (!(div % 2))
  {
    div /= 2;
    div2_cpu++;
  }

  // Here we know the mul and div parameter of the PLL config.
  // . Check out if the PLL has a valid in_cpu_f.
  // . Try to have for the PLL frequency (VCO output) the highest possible value
  //   to reduce jitter.
  while (in_osc0_f * 2 * mul / div < AVR32_PM_PLL_VCO_RANGE0_MAX_FREQ)
  {
    if (2 * mul > PM_MAX_MUL)
      break;
    mul *= 2;
    div2_cpu++;
  }

  if (div2_cpu != 0)
  {
    div2_cpu--;
    div2_en = 1;
  }

  pll_freq = in_osc0_f * mul / (div * (1 << div2_en));

  // Update real CPU Frequency
  param->cpu_f = pll_freq / (1 << div2_cpu);
  mul--;

  pm_pll_setup(&AVR32_PM
  , 0   // pll
  , mul // mul
  , div // div
  , 0   // osc
  , 16  // lockcount
  );

  pm_pll_set_option(&AVR32_PM
  , 0 // pll
  // PLL clock is lower than 160MHz: need to set pllopt.
  , (pll_freq < AVR32_PM_PLL_VCO_RANGE0_MIN_FREQ) ? 1 : 0 // pll_freq
  , div2_en // pll_div2
  , 0 // pll_wbwdisable
  );

  rest = pll_freq;
  while (rest > AVR32_PM_PBA_MAX_FREQ ||
         rest != param->pba_f)
  {
    div2_pba++;
    rest = pll_freq / (1 << div2_pba);
    if (rest < param->pba_f)
      break;
  }

  // Update real PBA Frequency
  param->pba_f = pll_freq / (1 << div2_pba);

  // Enable PLL0
  pm_pll_enable(&AVR32_PM, 0);

  // Wait for PLL0 locked
  pm_wait_for_pll0_locked(&AVR32_PM);

  if (div2_cpu)
  {
    b_div2_cpu = TRUE;
    div2_cpu--;
  }
  else
    b_div2_cpu = FALSE;

  if (div2_pba)
  {
    b_div2_pba = TRUE;
    div2_pba--;
  }
  else
    b_div2_pba = FALSE;

  pm_cksel(&AVR32_PM
  , b_div2_pba, div2_pba // PBA
  , b_div2_cpu, div2_cpu // PBB
  , b_div2_cpu, div2_cpu // HSB
  );

  if (param->cpu_f > AVR32_FLASHC_FWS_0_MAX_FREQ)
  {
    flashc_set_wait_state(1);
#if (defined AVR32_FLASHC_210_H_INCLUDED)
    if (param->cpu_f > AVR32_FLASHC_HSEN_FWS_1_MAX_FREQ)
      flashc_issue_command(AVR32_FLASHC_FCMD_CMD_HSEN, -1);
    else
      flashc_issue_command(AVR32_FLASHC_FCMD_CMD_HSDIS, -1);
#endif
  }
  else
  {
    flashc_set_wait_state(0);
#if (defined AVR32_FLASHC_210_H_INCLUDED)
    if (param->cpu_f > AVR32_FLASHC_HSEN_FWS_0_MAX_FREQ)
      flashc_issue_command(AVR32_FLASHC_FCMD_CMD_HSEN, -1);
    else
      flashc_issue_command(AVR32_FLASHC_FCMD_CMD_HSDIS, -1);
#endif
  }

  pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

  return PM_FREQ_STATUS_OK;
}


void pm_configure_usb_clock(void)
{
#if UC3A3

  // Setup USB GCLK.
  pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_USBB, // gc
                  0,                  // osc_or_pll: use Osc (if 0) or PLL (if 1)
                  0,                  // pll_osc: select Osc0/PLL0 or Osc1/PLL1
                  0,                  // diven
                  0);                 // div

  // Enable USB GCLK.
  pm_gc_enable(&AVR32_PM, AVR32_PM_GCLK_USBB);
#else
  // Use 12MHz from OSC0 and generate 96 MHz
  pm_pll_setup(&AVR32_PM, 1,  // pll.
	  7,   // mul.
	  1,   // div.
	  0,   // osc.
	  16); // lockcount.

  pm_pll_set_option(&AVR32_PM, 1, // pll.
	  1,  // pll_freq: choose the range 80-180MHz.
	  1,  // pll_div2.
	  0); // pll_wbwdisable.

  // start PLL1 and wait forl lock
  pm_pll_enable(&AVR32_PM, 1);

  // Wait for PLL1 locked.
  pm_wait_for_pll1_locked(&AVR32_PM);

  pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_USBB,  // gc.
            1,  // osc_or_pll: use Osc (if 0) or PLL (if 1).
            1,  // pll_osc: select Osc0/PLL0 or Osc1/PLL1.
            0,  // diven.
            0); // div.
  pm_gc_enable(&AVR32_PM, AVR32_PM_GCLK_USBB);
#endif
}
