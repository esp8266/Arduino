/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief High-level library abstracting features such as oscillators/pll/dfll
 *        configuration, clock configuration, System-sensible parameters
 *        configuration, buses clocks configuration, sleep mode, reset.
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
#include "power_clocks_lib.h"


//! Device-specific data
#if UC3L
static long int pcl_configure_clocks_uc3l(pcl_freq_param_t *param); // FORWARD declaration
#endif

#if UC3C
static long int pcl_configure_clocks_uc3c(pcl_freq_param_t *param); // FORWARD declaration
#endif

long int pcl_configure_clocks(pcl_freq_param_t *param)
{
#ifndef AVR32_PM_VERSION_RESETVALUE
  // Implementation for UC3A, UC3A3, UC3B parts.
  return(pm_configure_clocks(param));
#else
  #ifdef AVR32_PM_410_H_INCLUDED
    // Implementation for UC3C parts.
    return(pcl_configure_clocks_uc3c(param));
  #else
    // Implementation for UC3L parts.
    return(pcl_configure_clocks_uc3l(param));
  #endif
#endif
}


//! Device-specific implementation
#if UC3L
// FORWARD declaration
static long int pcl_configure_synchronous_clocks( pm_clk_src_t main_clk_src,
                                                  unsigned long main_clock_freq_hz,
                                                  pcl_freq_param_t *param);

long int pcl_configure_clocks_rcsys(pcl_freq_param_t *param)
{
  // Supported main clock sources: PCL_MC_RCSYS

  // Supported synchronous clocks frequencies if RCSYS is the main clock source:
  // 115200Hz, 57600Hz, 28800Hz, 14400Hz, 7200Hz, 3600Hz, 1800Hz, 900Hz, 450Hz.

  // NOTE: by default, this implementation doesn't perform thorough checks on the
  // input parameters. To enable the checks, define AVR32SFW_INPUT_CHECK.

#ifdef AVR32SFW_INPUT_CHECK
  // Verify that fCPU >= fPBx
  if((param->cpu_f < param->pba_f) || (param->cpu_f < param->pbb_f))
    return(-1);
#endif

#ifdef AVR32SFW_INPUT_CHECK
    // Verify that the target frequencies are reachable.
    if((param->cpu_f > SCIF_SLOWCLOCK_FREQ_HZ) || (param->pba_f > SCIF_SLOWCLOCK_FREQ_HZ)
      || (param->pbb_f > SCIF_SLOWCLOCK_FREQ_HZ))
      return(-1);
#endif

  return(pcl_configure_synchronous_clocks(PM_CLK_SRC_SLOW, SCIF_SLOWCLOCK_FREQ_HZ, param));
}


long int pcl_configure_clocks_rc120m(pcl_freq_param_t *param)
{
  // Supported main clock sources: PCL_MC_RC120M

  // Supported synchronous clocks frequencies if RC120M is the main clock source:
  // 30MHz, 15MHz, 7.5MHz, 3.75MHz, 1.875MHz, 937.5kHz, 468.75kHz.

  // NOTE: by default, this implementation doesn't perform thorough checks on the
  // input parameters. To enable the checks, define AVR32SFW_INPUT_CHECK.

#ifdef AVR32SFW_INPUT_CHECK
  // Verify that fCPU >= fPBx
  if((param->cpu_f < param->pba_f) || (param->cpu_f < param->pbb_f))
    return(-1);
#endif

#ifdef AVR32SFW_INPUT_CHECK
  // Verify that the target frequencies are reachable.
  if((param->cpu_f > SCIF_RC120M_FREQ_HZ) || (param->pba_f > SCIF_RC120M_FREQ_HZ)
    || (param->pbb_f > SCIF_RC120M_FREQ_HZ))
    return(-1);
#endif

  // Start the 120MHz internal RCosc (RC120M) clock
  scif_start_rc120M();

  return(pcl_configure_synchronous_clocks(PM_CLK_SRC_RC120M, SCIF_RC120M_FREQ_HZ, param));
}


long int pcl_configure_clocks_osc0(pcl_freq_param_t *param)
{
  // Supported main clock sources: PCL_MC_OSC0

  // Supported synchronous clocks frequencies if OSC0 is the main clock source:
  // (these obviously depend on the OSC0 frequency; we'll take 16MHz as an example)
  // 16MHz, 8MHz, 4MHz, 2MHz, 1MHz, 500kHz, 250kHz, 125kHz, 62.5kHz.

  // NOTE: by default, this implementation doesn't perform thorough checks on the
  // input parameters. To enable the checks, define AVR32SFW_INPUT_CHECK.

  unsigned long               main_clock_freq;


#ifdef AVR32SFW_INPUT_CHECK
  // Verify that fCPU >= fPBx
  if((param->cpu_f < param->pba_f) || (param->cpu_f < param->pbb_f))
    return(-1);
#endif

  main_clock_freq = param->osc0_f;
#ifdef AVR32SFW_INPUT_CHECK
  // Verify that the target frequencies are reachable.
  if((param->cpu_f > main_clock_freq) || (param->pba_f > main_clock_freq)
    || (param->pbb_f > main_clock_freq))
    return(-1);
#endif
  // Configure OSC0 in crystal mode, external crystal with a fcrystal Hz frequency.
  scif_configure_osc_crystalmode(SCIF_OSC0, main_clock_freq);
  // Enable the OSC0
  scif_enable_osc(SCIF_OSC0, param->osc0_startup, true);

  return(pcl_configure_synchronous_clocks(PM_CLK_SRC_OSC0, main_clock_freq, param));
}


long int pcl_configure_clocks_dfll0(pcl_freq_param_t *param)
{
  // Supported main clock sources: PCL_MC_DFLL

  // Supported synchronous clocks frequencies if DFLL is the main clock source:
  // (these obviously depend on the DFLL target frequency; we'll take 100MHz as an example)
  // 50MHz, 25MHz, 12.5MHz, 6.25MHz, 3.125MHz, 1562.5kHz, 781.25kHz, 390.625kHz.

  // NOTE: by default, this implementation doesn't perform thorough checks on the
  // input parameters. To enable the checks, define AVR32SFW_INPUT_CHECK.

  unsigned long   main_clock_freq;
  scif_gclk_opt_t *pgc_dfllif_ref_opt;


#ifdef AVR32SFW_INPUT_CHECK
  // Verify that fCPU >= fPBx
  if((param->cpu_f < param->pba_f) || (param->cpu_f < param->pbb_f))
    return(-1);
#endif

  main_clock_freq = param->dfll_f;
#ifdef AVR32SFW_INPUT_CHECK
  // Verify that the target DFLL output frequency is in the correct range.
  if((main_clock_freq > SCIF_DFLL_MAXFREQ_HZ) || (main_clock_freq < SCIF_DFLL_MINFREQ_HZ))
    return(-1);
  // Verify that the target frequencies are reachable.
  if((param->cpu_f > main_clock_freq) || (param->pba_f > main_clock_freq)
    || (param->pbb_f > main_clock_freq))
    return(-1);
#endif
  pgc_dfllif_ref_opt = (scif_gclk_opt_t *)param->pextra_params;
  // Implementation note: this implementation configures the DFLL in closed-loop
  // mode (because it gives the best accuracy) which enables the generic clock CLK_DFLLIF_REF
  // as a reference (RCSYS being used as the generic clock source, undivided).
  scif_dfll0_closedloop_configure_and_start(pgc_dfllif_ref_opt, main_clock_freq, TRUE);

  return(pcl_configure_synchronous_clocks(PM_CLK_SRC_DFLL0, main_clock_freq, param));
}


static long int pcl_configure_clocks_uc3l(pcl_freq_param_t *param)
{
  // Supported main clock sources: PCL_MC_RCSYS, PCL_MC_OSC0, PCL_MC_DFLL0, PCL_MC_RC120M

  // Supported synchronous clocks frequencies if RCSYS is the main clock source:
  // 115200Hz, 57600Hz, 28800Hz, 14400Hz, 7200Hz, 3600Hz, 1800Hz, 900Hz, 450Hz.

  // Supported synchronous clocks frequencies if RC120M is the main clock source:
  // 30MHz, 15MHz, 7.5MHz, 3.75MHz, 1.875MHz, 937.5kHz, 468.75kHz.

  // Supported synchronous clocks frequencies if OSC0 is the main clock source:
  // (these obviously depend on the OSC0 frequency; we'll take 16MHz as an example)
  // 16MHz, 8MHz, 4MHz, 2MHz, 1MHz, 500kHz, 250kHz, 125kHz, 62.5kHz.

  // Supported synchronous clocks frequencies if DFLL is the main clock source:
  // (these obviously depend on the DFLL target frequency; we'll take 100MHz as an example)
  // 50MHz, 25MHz, 12.5MHz, 6.25MHz, 3.125MHz, 1562.5kHz, 781.25kHz, 390.625kHz.

  // NOTE: by default, this implementation doesn't perform thorough checks on the
  // input parameters. To enable the checks, define AVR32SFW_INPUT_CHECK.


#ifdef AVR32SFW_INPUT_CHECK
  // Verify that fCPU >= fPBx
  if((param->cpu_f < param->pba_f) || (param->cpu_f < param->pbb_f))
    return(-1);
#endif

  if(PCL_MC_RCSYS == param->main_clk_src)
  {
    return(pcl_configure_clocks_rcsys(param));
  }
  else if(PCL_MC_RC120M == param->main_clk_src)
  {
    return(pcl_configure_clocks_rc120m(param));
  }
  else if(PCL_MC_OSC0 == param->main_clk_src)
  {
    return(pcl_configure_clocks_osc0(param));
  }
  else // PCL_MC_DFLL0 == param->main_clk_src
  {
    return(pcl_configure_clocks_dfll0(param));
  }
}

static long int pcl_configure_synchronous_clocks(pm_clk_src_t main_clk_src, unsigned long main_clock_freq_hz, pcl_freq_param_t *param)
{
  //#
  //# Set the Synchronous clock division ratio for each clock domain
  //#
  pm_set_all_cksel(main_clock_freq_hz, param->cpu_f, param->pba_f, param->pbb_f);

  //#
  //# Set the Flash wait state and the speed read mode (depending on the target CPU frequency).
  //#
#if UC3L
    flashcdw_set_flash_waitstate_and_readmode(param->cpu_f);
#elif UC3C
    flashc_set_flash_waitstate_and_readmode(param->cpu_f);
#endif


  //#
  //# Switch the main clock source to the selected clock.
  //#
  pm_set_mclk_source(main_clk_src);

  return PASS;
}

#endif // UC3L device-specific implementation

//! UC3C Device-specific implementation
#if UC3C
static long int pcl_configure_clocks_uc3c(pcl_freq_param_t *param)
{
  #define PM_MAX_MUL                         ((1 << AVR32_SCIF_PLLMUL_SIZE) - 1)
  #define AVR32_PM_PBA_MAX_FREQ              66000000
  #define AVR32_PM_PLL_VCO_RANGE0_MAX_FREQ   240000000
  #define AVR32_PM_PLL_VCO_RANGE0_MIN_FREQ   160000000

    // Implementation for  UC3C parts.
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

        // Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
        scif_configure_osc_crystalmode(SCIF_OSC0, in_osc0_f);
        // Enable the OSC0
        scif_enable_osc(SCIF_OSC0, param->osc0_startup, true);
        // Set the main clock source as being OSC0.
        pm_set_mclk_source(PM_CLK_SRC_OSC0);

        // Start with CPU freq config
        if (in_cpu_f == in_osc0_f)
        {
          param->cpu_f = in_osc0_f;
          param->pba_f = in_osc0_f;
          return PASS;
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
          return FAIL;

        mul = (in_cpu_f * div) / in_osc0_f;

        if (mul > PM_MAX_MUL)
          return FAIL;

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

        scif_pll_opt_t opt;

        opt.osc = SCIF_OSC0,     // Sel Osc0 or Osc1
        opt.lockcount = 16,      // lockcount in main clock for the PLL wait lock
        opt.div = div,             // DIV=1 in the formula
        opt.mul = mul,             // MUL=7 in the formula
        opt.pll_div2 = div2_en,        // pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
        opt.pll_wbwdisable = 0,  //pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
        opt.pll_freq = (pll_freq < AVR32_PM_PLL_VCO_RANGE0_MIN_FREQ) ? 1 : 0,        // Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.


        scif_pll_setup(SCIF_PLL0, opt); // lockcount in main clock for the PLL wait lock

        /* Enable PLL0 */
        scif_pll_enable(SCIF_PLL0);

        /* Wait for PLL0 locked */
        scif_wait_for_pll_locked(SCIF_PLL0) ;

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

        if (b_div2_cpu == TRUE )
        {
          pm_set_clk_domain_div(PM_CLK_DOMAIN_0, (pm_divratio_t) div2_cpu); // CPU
          pm_set_clk_domain_div(PM_CLK_DOMAIN_1, (pm_divratio_t) div2_cpu); // HSB
          pm_set_clk_domain_div(PM_CLK_DOMAIN_3, (pm_divratio_t) div2_cpu); // PBB
        }
        if (b_div2_pba == TRUE )
        {
          pm_set_clk_domain_div(PM_CLK_DOMAIN_2, (pm_divratio_t) div2_pba); // PBA
          pm_set_clk_domain_div(PM_CLK_DOMAIN_4, (pm_divratio_t) div2_pba); // PBC
        }

        // Set Flashc Wait State
        flashc_set_flash_waitstate_and_readmode(param->cpu_f);

        // Set the main clock source as being PLL0.
        pm_set_mclk_source(PM_CLK_SRC_PLL0);

        return PASS;
}
#endif // UC3C device-specific implementation

long int pcl_switch_to_osc(pcl_osc_t osc, unsigned int fcrystal, unsigned int startup)
{
#ifndef AVR32_PM_VERSION_RESETVALUE
// Implementation for UC3A, UC3A3, UC3B parts.
  if(PCL_OSC0 == osc)
  {
    // Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency,
    // enable the OSC0, set the main clock source as being OSC0.
    pm_switch_to_osc0(&AVR32_PM, fcrystal, startup);
  }
  else
  {
    return PCL_NOT_SUPPORTED;
  }
#else
// Implementation for UC3C, UC3L parts.
  #if AVR32_PM_VERSION_RESETVALUE < 0x400
    return PCL_NOT_SUPPORTED;
  #else
  if(PCL_OSC0 == osc)
  {
    // Configure OSC0 in crystal mode, external crystal with a fcrystal Hz frequency.
    scif_configure_osc_crystalmode(SCIF_OSC0, fcrystal);
    // Enable the OSC0
    scif_enable_osc(SCIF_OSC0, startup, true);
    // Set the Flash wait state and the speed read mode (depending on the target CPU frequency).
#if UC3L
    flashcdw_set_flash_waitstate_and_readmode(fcrystal);
#elif UC3C
    flashc_set_flash_waitstate_and_readmode(fcrystal);
#endif
    // Set the main clock source as being OSC0.
    pm_set_mclk_source(PM_CLK_SRC_OSC0);
  }
  else
  {
    return PCL_NOT_SUPPORTED;
  }
  #endif
#endif
  return PASS;
}

long int pcl_configure_usb_clock(void)
{
#ifndef AVR32_PM_VERSION_RESETVALUE
// Implementation for UC3A, UC3A3, UC3B parts.
  pm_configure_usb_clock();
  return PASS;
#else
  #ifdef AVR32_PM_410_H_INCLUDED
    const scif_pll_opt_t opt = {
              .osc = SCIF_OSC0,     // Sel Osc0 or Osc1
              .lockcount = 16,      // lockcount in main clock for the PLL wait lock
              .div = 1,             // DIV=1 in the formula
              .mul = 5,             // MUL=7 in the formula
              .pll_div2 = 1,        // pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
              .pll_wbwdisable = 0,  //pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
              .pll_freq = 1,        // Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.
    };

    /* Setup PLL1 on Osc0, mul=7 ,no divisor, lockcount=16, ie. 16Mhzx6 = 96MHz output */
    scif_pll_setup(SCIF_PLL1, opt); // lockcount in main clock for the PLL wait lock

    /* Enable PLL1 */
    scif_pll_enable(SCIF_PLL1);

    /* Wait for PLL1 locked */
    scif_wait_for_pll_locked(SCIF_PLL1) ;

  // Implementation for UC3C parts.
    // Setup the generic clock for USB
    scif_gc_setup(AVR32_SCIF_GCLK_USB,
                  SCIF_GCCTRL_PLL1,
                  AVR32_SCIF_GC_NO_DIV_CLOCK,
                  0);
    // Now enable the generic clock
    scif_gc_enable(AVR32_SCIF_GCLK_USB);
    return PASS;
  #else
      return PCL_NOT_SUPPORTED;
  #endif
#endif
}


#if UC3L
#else
void pcl_write_gplp(unsigned long gplp, unsigned long value)
{
#ifndef AVR32_PM_VERSION_RESETVALUE
// Implementation for UC3A, UC3A3, UC3B parts.
  pm_write_gplp(&AVR32_PM,gplp,value);
#else
  scif_write_gplp(gplp,value);
#endif
}

unsigned long pcl_read_gplp(unsigned long gplp)
{
#ifndef AVR32_PM_VERSION_RESETVALUE
// Implementation for UC3A, UC3A3, UC3B parts.
  return pm_read_gplp(&AVR32_PM,gplp);
#else
  return scif_read_gplp(gplp);
#endif
}
#endif
