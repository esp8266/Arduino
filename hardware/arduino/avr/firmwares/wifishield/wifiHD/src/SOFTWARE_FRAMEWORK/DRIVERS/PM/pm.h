/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

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

#ifndef _PM_H_
#define _PM_H_

#include <avr32/io.h>
#include "compiler.h"
#include "preprocessor.h"


/*! \brief Sets the MCU in the specified sleep mode.
 *
 * \param mode Sleep mode:
 *   \arg \c AVR32_PM_SMODE_IDLE: Idle;
 *   \arg \c AVR32_PM_SMODE_FROZEN: Frozen;
 *   \arg \c AVR32_PM_SMODE_STANDBY: Standby;
 *   \arg \c AVR32_PM_SMODE_STOP: Stop;
 *   \arg \c AVR32_PM_SMODE_DEEP_STOP: DeepStop;
 *   \arg \c AVR32_PM_SMODE_STATIC: Static.
 */
#define SLEEP(mode)   {__asm__ __volatile__ ("sleep "STRINGZ(mode));}


//! Input and output parameters when initializing PM clocks using pm_configure_clocks().
typedef struct
{
  //! CPU frequency (input/output argument).
  unsigned long cpu_f;

  //! PBA frequency (input/output argument).
  unsigned long pba_f;

  //! Oscillator 0's external crystal(or external clock) frequency (board dependant) (input argument).
  unsigned long osc0_f;

  //! Oscillator 0's external crystal(or external clock) startup time: AVR32_PM_OSCCTRL0_STARTUP_x_RCOSC (input argument).
  unsigned long osc0_startup;
} pm_freq_param_t;

#define PM_FREQ_STATUS_FAIL   (-1)
#define PM_FREQ_STATUS_OK     (0)


/*! \brief Gets the MCU reset cause.
 *
 * \param pm Base address of the Power Manager instance (i.e. &AVR32_PM).
 *
 * \return The MCU reset cause which can be masked with the
 *         \c AVR32_PM_RCAUSE_x_MASK bit-masks to isolate specific causes.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ unsigned int pm_get_reset_cause(volatile avr32_pm_t *pm)
{
  return pm->rcause;
}


/*!
 * \brief This function will enable the external clock mode of the oscillator 0.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_enable_osc0_ext_clock(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the crystal mode of the oscillator 0.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param fosc0 Oscillator 0 crystal frequency (Hz)
 */
extern void pm_enable_osc0_crystal(volatile avr32_pm_t *pm, unsigned int fosc0);


/*!
 * \brief This function will enable the oscillator 0 to be used with a startup time.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param startup Clock 0 startup time. AVR32_PM_OSCCTRL0_STARTUP_x_RCOSC.
 */
extern void pm_enable_clk0(volatile avr32_pm_t *pm, unsigned int startup);


/*!
 * \brief This function will disable the oscillator 0.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_disable_clk0(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the oscillator 0 to be used with no startup time.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param startup Clock 0 startup time, for which the function does not wait. AVR32_PM_OSCCTRL0_STARTUP_x_RCOSC.
 */
extern void pm_enable_clk0_no_wait(volatile avr32_pm_t *pm, unsigned int startup);


/*!
 * \brief This function will wait until the Osc0 clock is ready.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_wait_for_clk0_ready(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the external clock mode of the oscillator 1.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_enable_osc1_ext_clock(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the crystal mode of the oscillator 1.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param fosc1 Oscillator 1 crystal frequency (Hz)
 */
extern void pm_enable_osc1_crystal(volatile avr32_pm_t *pm, unsigned int fosc1);


/*!
 * \brief This function will enable the oscillator 1 to be used with a startup time.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param startup Clock 1 startup time. AVR32_PM_OSCCTRL1_STARTUP_x_RCOSC.
 */
extern void pm_enable_clk1(volatile avr32_pm_t *pm, unsigned int startup);


/*!
 * \brief This function will disable the oscillator 1.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_disable_clk1(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the oscillator 1 to be used with no startup time.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param startup Clock 1 startup time, for which the function does not wait. AVR32_PM_OSCCTRL1_STARTUP_x_RCOSC.
 */
extern void pm_enable_clk1_no_wait(volatile avr32_pm_t *pm, unsigned int startup);


/*!
 * \brief This function will wait until the Osc1 clock is ready.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_wait_for_clk1_ready(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the external clock mode of the 32-kHz oscillator.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_enable_osc32_ext_clock(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the crystal mode of the 32-kHz oscillator.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_enable_osc32_crystal(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the oscillator 32 to be used with a startup time.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param startup Clock 32 kHz startup time. AVR32_PM_OSCCTRL32_STARTUP_x_RCOSC.
 */
extern void pm_enable_clk32(volatile avr32_pm_t *pm, unsigned int startup);


/*!
 * \brief This function will disable the oscillator 32.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_disable_clk32(volatile avr32_pm_t *pm);


/*!
 * \brief This function will enable the oscillator 32 to be used with no startup time.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param startup Clock 32 kHz startup time, for which the function does not wait. AVR32_PM_OSCCTRL32_STARTUP_x_RCOSC.
 */
extern void pm_enable_clk32_no_wait(volatile avr32_pm_t *pm, unsigned int startup);


/*!
 * \brief This function will wait until the osc32 clock is ready.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_wait_for_clk32_ready(volatile avr32_pm_t *pm);


/*!
 * \brief This function will select all the power manager clocks.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param pbadiv Peripheral Bus A clock divisor enable
 * \param pbasel Peripheral Bus A select
 * \param pbbdiv Peripheral Bus B clock divisor enable
 * \param pbbsel Peripheral Bus B select
 * \param hsbdiv High Speed Bus clock divisor enable (CPU clock = HSB clock)
 * \param hsbsel High Speed Bus select (CPU clock = HSB clock )
 */
extern void pm_cksel(volatile avr32_pm_t *pm, unsigned int pbadiv, unsigned int pbasel, unsigned int pbbdiv, unsigned int pbbsel, unsigned int hsbdiv, unsigned int hsbsel);


/*!
 * \brief This function will setup a generic clock.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param gc generic clock number (0 for gc0...)
 * \param osc_or_pll Use OSC (=0) or PLL (=1)
 * \param pll_osc Select Osc0/PLL0 or Osc1/PLL1
 * \param diven Generic clock divisor enable
 * \param div Generic clock divisor
 */
extern void pm_gc_setup(volatile avr32_pm_t *pm, unsigned int gc, unsigned int osc_or_pll, unsigned int pll_osc, unsigned int diven, unsigned int div);


/*!
 * \brief This function will enable a generic clock.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param gc generic clock number (0 for gc0...)
 */
extern void pm_gc_enable(volatile avr32_pm_t *pm, unsigned int gc);


/*!
 * \brief This function will disable a generic clock.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param gc generic clock number (0 for gc0...)
 */
extern void pm_gc_disable(volatile avr32_pm_t *pm, unsigned int gc);


/*!
 * \brief This function will setup a PLL.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param pll PLL number(0 for PLL0, 1 for PLL1)
 * \param mul PLL MUL in the PLL formula
 * \param div PLL DIV in the PLL formula
 * \param osc OSC number (0 for osc0, 1 for osc1)
 * \param lockcount PLL lockount
 */
extern void pm_pll_setup(volatile avr32_pm_t *pm, unsigned int pll, unsigned int mul, unsigned int div, unsigned int osc, unsigned int lockcount);


/*!
 * \brief This function will set a PLL option.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param pll PLL number(0 for PLL0, 1 for PLL1)
 * \param pll_freq Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.
 * \param pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
 * \param pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
 */
extern void pm_pll_set_option(volatile avr32_pm_t *pm, unsigned int pll, unsigned int  pll_freq, unsigned int  pll_div2, unsigned int  pll_wbwdisable);


/*!
 * \brief This function will get a PLL option.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param pll PLL number(0 for PLL0, 1 for PLL1)
 * \return       Option
 */
extern unsigned int pm_pll_get_option(volatile avr32_pm_t *pm, unsigned int pll);


/*!
 * \brief This function will enable a PLL.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param pll PLL number(0 for PLL0, 1 for PLL1)
 */
extern void pm_pll_enable(volatile avr32_pm_t *pm, unsigned int pll);


/*!
 * \brief This function will disable a PLL.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param pll PLL number(0 for PLL0, 1 for PLL1)
 */
extern void pm_pll_disable(volatile avr32_pm_t *pm, unsigned int pll);


/*!
 * \brief This function will wait for PLL0 locked
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_wait_for_pll0_locked(volatile avr32_pm_t *pm);


/*!
 * \brief This function will wait for PLL1 locked
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 */
extern void pm_wait_for_pll1_locked(volatile avr32_pm_t *pm);


/*!
 * \brief This function will switch the power manager main clock.
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param clock Clock to be switched on. AVR32_PM_MCSEL_SLOW for RCOsc, AVR32_PM_MCSEL_OSC0 for Osc0, AVR32_PM_MCSEL_PLL0 for PLL0.
 */
extern void pm_switch_to_clock(volatile avr32_pm_t *pm, unsigned long clock);


/*!
 * \brief Switch main clock to clock Osc0 (crystal mode)
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param fosc0 Oscillator 0 crystal frequency (Hz)
 * \param startup Crystal 0 startup time. AVR32_PM_OSCCTRL0_STARTUP_x_RCOSC.
 */
extern void pm_switch_to_osc0(volatile avr32_pm_t *pm, unsigned int fosc0, unsigned int startup);


/*! \brief Enables the Brown-Out Detector interrupt.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 */
extern void pm_bod_enable_irq(volatile avr32_pm_t *pm);


/*! \brief Disables the Brown-Out Detector interrupt.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 */
extern void pm_bod_disable_irq(volatile avr32_pm_t *pm);


/*! \brief Clears the Brown-Out Detector interrupt flag.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 */
extern void pm_bod_clear_irq(volatile avr32_pm_t *pm);


/*! \brief Gets the Brown-Out Detector interrupt flag.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 *
 * \retval 0 No BOD interrupt.
 * \retval 1 BOD interrupt pending.
 */
extern unsigned long pm_bod_get_irq_status(volatile avr32_pm_t *pm);


/*! \brief Gets the Brown-Out Detector interrupt enable status.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 *
 * \retval 0 BOD interrupt disabled.
 * \retval 1 BOD interrupt enabled.
 */
extern unsigned long pm_bod_get_irq_enable_bit(volatile avr32_pm_t *pm);


/*! \brief Gets the triggering threshold of the Brown-Out Detector.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM).
 *
 * \return Triggering threshold of the BOD. See the electrical characteristics
 *         in the part datasheet for actual voltage levels.
 */
extern unsigned long pm_bod_get_level(volatile avr32_pm_t *pm);


/*!
 * \brief Read the content of the PM GPLP registers
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param gplp GPLP register index (0,1,... depending on the number of GPLP registers for a given part)
 *
 * \return The content of the chosen GPLP register.
 */
extern unsigned long pm_read_gplp(volatile avr32_pm_t *pm, unsigned long gplp);


/*!
 * \brief Write into the PM GPLP registers
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param gplp GPLP register index (0,1,... depending on the number of GPLP registers for a given part)
 * \param value Value to write
 */
extern void pm_write_gplp(volatile avr32_pm_t *pm, unsigned long gplp, unsigned long value);


/*! \brief Enable the clock of a module.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param module The module to clock (use one of the defines in the part-specific
 * header file under "toolchain folder"/avr32/inc(lude)/avr32/; depending on the
 * clock domain, look for the sections "CPU clocks", "HSB clocks", "PBx clocks")
 *
 * \return Status.
 *   \retval 0  Success.
 *   \retval <0 An error occured.
 */
extern long pm_enable_module(volatile avr32_pm_t *pm, unsigned long module);

/*! \brief Disable the clock of a module.
 *
 * \param pm Base address of the Power Manager (i.e. &AVR32_PM)
 * \param module The module to shut down (use one of the defines in the part-specific
 * header file under "toolchain folder"/avr32/inc(lude)/avr32/; depending on the
 * clock domain, look for the sections "CPU clocks", "HSB clocks", "PBx clocks")
 *
 * \return Status.
 *   \retval 0  Success.
 *   \retval <0 An error occured.
 */
extern long pm_disable_module(volatile avr32_pm_t *pm, unsigned long module);



/*! \brief Automatically configure the CPU, PBA, PBB, and HSB clocks
 *         according to the user wishes.
 *
 * This function needs some parameters stored in a pm_freq_param_t structure:
 *  - cpu_f and pba_f are the wanted frequencies,
 *  - osc0_f is the oscillator 0 on-board frequency (e.g. FOSC0),
 *  - osc0_startup is the oscillator 0 startup time (e.g. OSC0_STARTUP).
 *
 * The function will then configure the clocks using the following rules:
 *  - It first try to find a valid PLL frequency (the highest possible value to avoid jitter) in order
 *    to satisfy the CPU frequency,
 *  - It optimizes the configuration depending the various divide stages,
 *  - Then, the PBA frequency is configured from the CPU freq.
 *  - Note that HSB and PBB are configured with the same frequency as CPU.
 *  - Note also that the number of wait states of the flash read accesses is automatically set-up depending
 *    the CPU frequency. As a consequence, the application needs the FLASHC driver to compile.
 *
 * The CPU, HSB and PBA frequencies programmed after configuration are stored back into cpu_f and pba_f.
 *
 * \param param    pointer on the configuration structure.
 *
 * \retval PM_FREQ_STATUS_OK    Mode successfully initialized.
 * \retval PM_FREQ_STATUS_FAIL  The configuration can not be done.
 */
extern int pm_configure_clocks(pm_freq_param_t *param);


/*! \brief Automatically configure the USB clock.
 *
 * USB clock is configured to 48MHz, using the PLL1 from the Oscillator0, assuming
 * a 12 MHz crystal is connected to it.
 */
extern void pm_configure_usb_clock(void);


#endif  // _PM_H_
