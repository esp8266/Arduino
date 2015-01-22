/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Power Manager driver.
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

#include "compiler.h"
#include "pm.h"


/*! \name PM Writable Bit-Field Registers
 */
//! @{

typedef union
{
  unsigned long                 mcctrl;
  avr32_pm_mcctrl_t             MCCTRL;
} u_avr32_pm_mcctrl_t;

typedef union
{
  unsigned long                 cksel;
  avr32_pm_cksel_t              CKSEL;
} u_avr32_pm_cksel_t;

typedef union
{
  unsigned long                 pll;
  avr32_pm_pll_t                PLL;
} u_avr32_pm_pll_t;

typedef union
{
  unsigned long                 oscctrl0;
  avr32_pm_oscctrl0_t           OSCCTRL0;
} u_avr32_pm_oscctrl0_t;

typedef union
{
  unsigned long                 oscctrl1;
  avr32_pm_oscctrl1_t           OSCCTRL1;
} u_avr32_pm_oscctrl1_t;

typedef union
{
  unsigned long                 oscctrl32;
  avr32_pm_oscctrl32_t          OSCCTRL32;
} u_avr32_pm_oscctrl32_t;

typedef union
{
  unsigned long                 ier;
  avr32_pm_ier_t                IER;
} u_avr32_pm_ier_t;

typedef union
{
  unsigned long                 idr;
  avr32_pm_idr_t                IDR;
} u_avr32_pm_idr_t;

typedef union
{
  unsigned long                 icr;
  avr32_pm_icr_t                ICR;
} u_avr32_pm_icr_t;

typedef union
{
  unsigned long                 gcctrl;
  avr32_pm_gcctrl_t             GCCTRL;
} u_avr32_pm_gcctrl_t;

typedef union
{
  unsigned long                 rccr;
  avr32_pm_rccr_t               RCCR;
} u_avr32_pm_rccr_t;

typedef union
{
  unsigned long                 bgcr;
  avr32_pm_bgcr_t               BGCR;
} u_avr32_pm_bgcr_t;

typedef union
{
  unsigned long                 vregcr;
  avr32_pm_vregcr_t             VREGCR;
} u_avr32_pm_vregcr_t;

typedef union
{
  unsigned long                 bod;
  avr32_pm_bod_t                BOD;
} u_avr32_pm_bod_t;

//! @}


/*! \brief Sets the mode of the oscillator 0.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 * \param mode Oscillator 0 mode (i.e. AVR32_PM_OSCCTRL0_MODE_x).
 */
static void pm_set_osc0_mode(volatile avr32_pm_t *pm, unsigned int mode)
{
  // Read
  u_avr32_pm_oscctrl0_t u_avr32_pm_oscctrl0 = {pm->oscctrl0};
  // Modify
  u_avr32_pm_oscctrl0.OSCCTRL0.mode = mode;
  // Write
  pm->oscctrl0 = u_avr32_pm_oscctrl0.oscctrl0;
}


void pm_enable_osc0_ext_clock(volatile avr32_pm_t *pm)
{
  pm_set_osc0_mode(pm, AVR32_PM_OSCCTRL0_MODE_EXT_CLOCK);
}


void pm_enable_osc0_crystal(volatile avr32_pm_t *pm, unsigned int fosc0)
{
  pm_set_osc0_mode(pm, (fosc0 <  900000) ? AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G0 :
                       (fosc0 < 3000000) ? AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G1 :
                       (fosc0 < 8000000) ? AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G2 :
                                           AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G3);
}


void pm_enable_clk0(volatile avr32_pm_t *pm, unsigned int startup)
{
  pm_enable_clk0_no_wait(pm, startup);
  pm_wait_for_clk0_ready(pm);
}


void pm_disable_clk0(volatile avr32_pm_t *pm)
{
  pm->mcctrl &= ~AVR32_PM_MCCTRL_OSC0EN_MASK;
}


void pm_enable_clk0_no_wait(volatile avr32_pm_t *pm, unsigned int startup)
{
  // Read register
  u_avr32_pm_oscctrl0_t u_avr32_pm_oscctrl0 = {pm->oscctrl0};
  // Modify
  u_avr32_pm_oscctrl0.OSCCTRL0.startup = startup;
  // Write back
  pm->oscctrl0 = u_avr32_pm_oscctrl0.oscctrl0;

  pm->mcctrl |= AVR32_PM_MCCTRL_OSC0EN_MASK;
}


void pm_wait_for_clk0_ready(volatile avr32_pm_t *pm)
{
  while (!(pm->poscsr & AVR32_PM_POSCSR_OSC0RDY_MASK));
}


/*! \brief Sets the mode of the oscillator 1.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 * \param mode Oscillator 1 mode (i.e. AVR32_PM_OSCCTRL1_MODE_x).
 */
static void pm_set_osc1_mode(volatile avr32_pm_t *pm, unsigned int mode)
{
  // Read
  u_avr32_pm_oscctrl1_t u_avr32_pm_oscctrl1 = {pm->oscctrl1};
  // Modify
  u_avr32_pm_oscctrl1.OSCCTRL1.mode = mode;
  // Write
  pm->oscctrl1 = u_avr32_pm_oscctrl1.oscctrl1;
}


void pm_enable_osc1_ext_clock(volatile avr32_pm_t *pm)
{
  pm_set_osc1_mode(pm, AVR32_PM_OSCCTRL1_MODE_EXT_CLOCK);
}


void pm_enable_osc1_crystal(volatile avr32_pm_t *pm, unsigned int fosc1)
{
  pm_set_osc1_mode(pm, (fosc1 <  900000) ? AVR32_PM_OSCCTRL1_MODE_CRYSTAL_G0 :
                       (fosc1 < 3000000) ? AVR32_PM_OSCCTRL1_MODE_CRYSTAL_G1 :
                       (fosc1 < 8000000) ? AVR32_PM_OSCCTRL1_MODE_CRYSTAL_G2 :
                                           AVR32_PM_OSCCTRL1_MODE_CRYSTAL_G3);
}


void pm_enable_clk1(volatile avr32_pm_t *pm, unsigned int startup)
{
  pm_enable_clk1_no_wait(pm, startup);
  pm_wait_for_clk1_ready(pm);
}


void pm_disable_clk1(volatile avr32_pm_t *pm)
{
  pm->mcctrl &= ~AVR32_PM_MCCTRL_OSC1EN_MASK;
}


void pm_enable_clk1_no_wait(volatile avr32_pm_t *pm, unsigned int startup)
{
  // Read register
  u_avr32_pm_oscctrl1_t u_avr32_pm_oscctrl1 = {pm->oscctrl1};
  // Modify
  u_avr32_pm_oscctrl1.OSCCTRL1.startup = startup;
  // Write back
  pm->oscctrl1 = u_avr32_pm_oscctrl1.oscctrl1;

  pm->mcctrl |= AVR32_PM_MCCTRL_OSC1EN_MASK;
}


void pm_wait_for_clk1_ready(volatile avr32_pm_t *pm)
{
  while (!(pm->poscsr & AVR32_PM_POSCSR_OSC1RDY_MASK));
}


/*! \brief Sets the mode of the 32-kHz oscillator.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 * \param mode 32-kHz oscillator mode (i.e. AVR32_PM_OSCCTRL32_MODE_x).
 */
static void pm_set_osc32_mode(volatile avr32_pm_t *pm, unsigned int mode)
{
  // Read
  u_avr32_pm_oscctrl32_t u_avr32_pm_oscctrl32 = {pm->oscctrl32};
  // Modify
  u_avr32_pm_oscctrl32.OSCCTRL32.mode = mode;
  // Write
  pm->oscctrl32 = u_avr32_pm_oscctrl32.oscctrl32;
}


void pm_enable_osc32_ext_clock(volatile avr32_pm_t *pm)
{
  pm_set_osc32_mode(pm, AVR32_PM_OSCCTRL32_MODE_EXT_CLOCK);
}


void pm_enable_osc32_crystal(volatile avr32_pm_t *pm)
{
  pm_set_osc32_mode(pm, AVR32_PM_OSCCTRL32_MODE_CRYSTAL);
}


void pm_enable_clk32(volatile avr32_pm_t *pm, unsigned int startup)
{
  pm_enable_clk32_no_wait(pm, startup);
  pm_wait_for_clk32_ready(pm);
}


void pm_disable_clk32(volatile avr32_pm_t *pm)
{
  pm->oscctrl32 &= ~AVR32_PM_OSCCTRL32_OSC32EN_MASK;
}


void pm_enable_clk32_no_wait(volatile avr32_pm_t *pm, unsigned int startup)
{
  // Read register
  u_avr32_pm_oscctrl32_t u_avr32_pm_oscctrl32 = {pm->oscctrl32};
  // Modify
  u_avr32_pm_oscctrl32.OSCCTRL32.osc32en = 1;
  u_avr32_pm_oscctrl32.OSCCTRL32.startup = startup;
  // Write back
  pm->oscctrl32 = u_avr32_pm_oscctrl32.oscctrl32;
}


void pm_wait_for_clk32_ready(volatile avr32_pm_t *pm)
{
  while (!(pm->poscsr & AVR32_PM_POSCSR_OSC32RDY_MASK));
}


void pm_cksel(volatile avr32_pm_t *pm,
              unsigned int pbadiv,
              unsigned int pbasel,
              unsigned int pbbdiv,
              unsigned int pbbsel,
              unsigned int hsbdiv,
              unsigned int hsbsel)
{
  u_avr32_pm_cksel_t u_avr32_pm_cksel = {0};

  u_avr32_pm_cksel.CKSEL.cpusel = hsbsel;
  u_avr32_pm_cksel.CKSEL.cpudiv = hsbdiv;
  u_avr32_pm_cksel.CKSEL.hsbsel = hsbsel;
  u_avr32_pm_cksel.CKSEL.hsbdiv = hsbdiv;
  u_avr32_pm_cksel.CKSEL.pbasel = pbasel;
  u_avr32_pm_cksel.CKSEL.pbadiv = pbadiv;
  u_avr32_pm_cksel.CKSEL.pbbsel = pbbsel;
  u_avr32_pm_cksel.CKSEL.pbbdiv = pbbdiv;

  pm->cksel = u_avr32_pm_cksel.cksel;

  // Wait for ckrdy bit and then clear it
  while (!(pm->poscsr & AVR32_PM_POSCSR_CKRDY_MASK));
}


void pm_gc_setup(volatile avr32_pm_t *pm,
                  unsigned int gc,
                  unsigned int osc_or_pll, // Use Osc (=0) or PLL (=1)
                  unsigned int pll_osc, // Sel Osc0/PLL0 or Osc1/PLL1
                  unsigned int diven,
                  unsigned int div)
{
  u_avr32_pm_gcctrl_t u_avr32_pm_gcctrl = {0};

  u_avr32_pm_gcctrl.GCCTRL.oscsel = pll_osc;
  u_avr32_pm_gcctrl.GCCTRL.pllsel = osc_or_pll;
  u_avr32_pm_gcctrl.GCCTRL.diven  = diven;
  u_avr32_pm_gcctrl.GCCTRL.div    = div;

  pm->gcctrl[gc] = u_avr32_pm_gcctrl.gcctrl;
}


void pm_gc_enable(volatile avr32_pm_t *pm,
                  unsigned int gc)
{
  pm->gcctrl[gc] |= AVR32_PM_GCCTRL_CEN_MASK;
}


void pm_gc_disable(volatile avr32_pm_t *pm,
                   unsigned int gc)
{
  pm->gcctrl[gc] &= ~AVR32_PM_GCCTRL_CEN_MASK;
}


void pm_pll_setup(volatile avr32_pm_t *pm,
                  unsigned int pll,
                  unsigned int mul,
                  unsigned int div,
                  unsigned int osc,
                  unsigned int lockcount)
{
  u_avr32_pm_pll_t u_avr32_pm_pll = {0};

  u_avr32_pm_pll.PLL.pllosc   = osc;
  u_avr32_pm_pll.PLL.plldiv   = div;
  u_avr32_pm_pll.PLL.pllmul   = mul;
  u_avr32_pm_pll.PLL.pllcount = lockcount;

  pm->pll[pll] = u_avr32_pm_pll.pll;
}


void pm_pll_set_option(volatile avr32_pm_t *pm,
                       unsigned int pll,
                       unsigned int pll_freq,
                       unsigned int pll_div2,
                       unsigned int pll_wbwdisable)
{
  u_avr32_pm_pll_t u_avr32_pm_pll = {pm->pll[pll]};
  u_avr32_pm_pll.PLL.pllopt = pll_freq | (pll_div2 << 1) | (pll_wbwdisable << 2);
  pm->pll[pll] = u_avr32_pm_pll.pll;
}


unsigned int pm_pll_get_option(volatile avr32_pm_t *pm,
                               unsigned int pll)
{
  return (pm->pll[pll] & AVR32_PM_PLLOPT_MASK) >> AVR32_PM_PLLOPT_OFFSET;
}


void pm_pll_enable(volatile avr32_pm_t *pm,
                  unsigned int pll)
{
  pm->pll[pll] |= AVR32_PM_PLLEN_MASK;
}


void pm_pll_disable(volatile avr32_pm_t *pm,
                  unsigned int pll)
{
  pm->pll[pll] &= ~AVR32_PM_PLLEN_MASK;
}


void pm_wait_for_pll0_locked(volatile avr32_pm_t *pm)
{
  while (!(pm->poscsr & AVR32_PM_POSCSR_LOCK0_MASK));
}


void pm_wait_for_pll1_locked(volatile avr32_pm_t *pm)
{
  while (!(pm->poscsr & AVR32_PM_POSCSR_LOCK1_MASK));
}


void pm_switch_to_clock(volatile avr32_pm_t *pm, unsigned long clock)
{
  // Read
  u_avr32_pm_mcctrl_t u_avr32_pm_mcctrl = {pm->mcctrl};
  // Modify
  u_avr32_pm_mcctrl.MCCTRL.mcsel = clock;
  // Write back
  pm->mcctrl = u_avr32_pm_mcctrl.mcctrl;
}


void pm_switch_to_osc0(volatile avr32_pm_t *pm, unsigned int fosc0, unsigned int startup)
{
  pm_enable_osc0_crystal(pm, fosc0);            // Enable the Osc0 in crystal mode
  pm_enable_clk0(pm, startup);                  // Crystal startup time - This parameter is critical and depends on the characteristics of the crystal
  pm_switch_to_clock(pm, AVR32_PM_MCSEL_OSC0);  // Then switch main clock to Osc0
}


void pm_bod_enable_irq(volatile avr32_pm_t *pm)
{
  pm->ier = AVR32_PM_IER_BODDET_MASK;
}


void pm_bod_disable_irq(volatile avr32_pm_t *pm)
{
  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  pm->idr = AVR32_PM_IDR_BODDET_MASK;
  pm->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();
}


void pm_bod_clear_irq(volatile avr32_pm_t *pm)
{
  pm->icr = AVR32_PM_ICR_BODDET_MASK;
}


unsigned long pm_bod_get_irq_status(volatile avr32_pm_t *pm)
{
  return ((pm->isr & AVR32_PM_ISR_BODDET_MASK) != 0);
}


unsigned long pm_bod_get_irq_enable_bit(volatile avr32_pm_t *pm)
{
  return ((pm->imr & AVR32_PM_IMR_BODDET_MASK) != 0);
}


unsigned long pm_bod_get_level(volatile avr32_pm_t *pm)
{
  return (pm->bod & AVR32_PM_BOD_LEVEL_MASK) >> AVR32_PM_BOD_LEVEL_OFFSET;
}


unsigned long pm_read_gplp(volatile avr32_pm_t *pm, unsigned long gplp)
{
  return pm->gplp[gplp];
}


void pm_write_gplp(volatile avr32_pm_t *pm, unsigned long gplp, unsigned long value)
{
  pm->gplp[gplp] = value;
}


long pm_enable_module(volatile avr32_pm_t *pm, unsigned long module)
{
  unsigned long domain = module>>5;
  unsigned long *regptr = (unsigned long*)(&(pm->cpumask) + domain);

  // Implementation-specific shortcut: the ckMASK registers are contiguous and
  // memory-mapped in that order: CPUMASK, HSBMASK, PBAMASK, PBBMASK.

  *regptr |= (1<<(module%32));

  return PASS;
}

long pm_disable_module(volatile avr32_pm_t *pm, unsigned long module)
{
  unsigned long domain = module>>5;
  unsigned long *regptr = (unsigned long*)(&(pm->cpumask) + domain);

  // Implementation-specific shortcut: the ckMASK registers are contiguous and
  // memory-mapped in that order: CPUMASK, HSBMASK, PBAMASK, PBBMASK.

  *regptr &= ~(1<<(module%32));

  return PASS;
}
