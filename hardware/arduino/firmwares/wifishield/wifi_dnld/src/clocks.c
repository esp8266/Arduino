/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <avr32/io.h>
#include "board.h"
#include "clocks.h"
#include "pm.h"
#include "flashc.h"
#include "gpio.h"

/**
 * Initializes the MCU system clocks.
 */
void
init_sys_clocks(void)
{
        /* switch to OSC0 to speed up the booting */
        pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
    
#ifndef USE_PLL
        return;
#endif
        
        /* For audio, ee have to use OSC1 on to generate the correct clockrate
         * for the SSC
         */
#if OSC == 1
        /* start oscillator1 */
        pm_enable_osc1_crystal(&AVR32_PM, FOSC1);
        pm_enable_clk1(&AVR32_PM, OSC1_STARTUP);
#endif

        /* configure pll multipliers */
        pm_pll_setup(&AVR32_PM,
                     0,          /* pll */
                     PLL_MUL,    /* mul */
                     1,          /* div */
                     OSC,        /* osc */
                     16);        /* lockcount */

        /* set PLL operating range and divider (fpll = fvco/2)
         * this gives PLL output = 66 MHz (62.0928 MHz for EVK1105/OSC1)
         */
        pm_pll_set_option(&AVR32_PM,
                          0,  /* pll */
                          1,  /* pll_freq */
                          1,  /* pll_div2 */
                          0); /* pll_wbwdisable. */
  

        /* start PLL0 and wait for the lock */
        pm_pll_enable(&AVR32_PM, 0);
        pm_wait_for_pll0_locked(&AVR32_PM);
        
        /* Set all peripheral clocks torun at master clock rate */
        pm_cksel(&AVR32_PM,
                 0,   /* pbadiv */
                 0,   /* pbasel */
                 0,   /* pbbdiv */
                 0,   /* pbbsel */
                 0,   /* hsbdiv */
                 0);  /* hsbsel */ 
        
        /* Set one waitstate for the flash */
        flashc_set_wait_state(1);
        
        /* Switch to PLL0 as the master clock */
        pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

#if OSC == 1
        pm_configure_usb_clock();
#endif
}
