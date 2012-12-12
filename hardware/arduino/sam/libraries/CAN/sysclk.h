/**
 * \file
 *
 * \brief Chip-specific system clock management functions.
 *
 * Copyright (c) 2011 - 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef CHIP_SYSCLK_H_INCLUDED
#define CHIP_SYSCLK_H_INCLUDED

#include <osc.h>
#include <pll.h>

/**
 * \page sysclk_quickstart Quick Start Guide for the System Clock Management service (SAM3A)
 *
 * This is the quick start guide for the \ref sysclk_group "System Clock Management"
 * service, with step-by-step instructions on how to configure and use the service for
 * specific use cases.
 *
 * \section sysclk_quickstart_usecases System Clock Management use cases
 * - \ref sysclk_quickstart_basic
 *
 * \section sysclk_quickstart_basic Basic usage of the System Clock Management service
 * This section will present a basic use case for the System Clock Management service.
 * This use case will configure the main system clock to 84MHz, using an internal PLL
 * module to multiply the frequency of a crystal attached to the microcontroller.
 *
 * \subsection sysclk_quickstart_use_case_1_prereq Prerequisites
 *  - None
 *
 * \subsection sysclk_quickstart_use_case_1_setup_steps Initialization code
 * Add to the application initialization code:
 * \code
 *    sysclk_init();
 * \endcode
 *
 * \subsection sysclk_quickstart_use_case_1_setup_steps_workflow Workflow
 * -# Configure the system clocks according to the settings in conf_clock.h:
 *    \code sysclk_init(); \endcode
 *
 * \subsection sysclk_quickstart_use_case_1_example_code Example code
 *   Add or uncomment the following in your conf_clock.h header file, commenting out all other
 *   definitions of the same symbol(s):
 *   \code
 *   #define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_PLLACK
 *
 *   // Fpll0 = (Fclk * PLL_mul) / PLL_div
 *   #define CONFIG_PLL0_SOURCE          PLL_SRC_MAINCK_XTAL
 *   #define CONFIG_PLL0_MUL             (84000000UL / BOARD_FREQ_MAINCK_XTAL)
 *   #define CONFIG_PLL0_DIV             1
 *
 *   // Fbus = Fsys / BUS_div
 *   #define CONFIG_SYSCLK_PRES          SYSCLK_PRES_1
 *   \endcode
 *
 * \subsection sysclk_quickstart_use_case_1_example_workflow Workflow
 *  -# Configure the main system clock to use the output of the PLL module as its source:
 *   \code #define CONFIG_SYSCLK_SOURCE          SYSCLK_SRC_PLLACK \endcode
 *  -# Configure the PLL module to use the fast external fast crystal oscillator as its source:
 *   \code #define CONFIG_PLL0_SOURCE            PLL_SRC_MAINCK_XTAL \endcode
 *  -# Configure the PLL module to multiply the external fast crystal oscillator frequency up to 84MHz:
 *   \code
 *   #define CONFIG_PLL0_MUL             (84000000UL / BOARD_FREQ_MAINCK_XTAL)
 *   #define CONFIG_PLL0_DIV             1 
 *   \endcode
 *   \note For user boards, \c BOARD_FREQ_MAINCK_XTAL should be defined in the board \c conf_board.h configuration
 *         file as the frequency of the fast crystal attached to the microcontroller.
 *  -# Configure the main clock to run at the full 84MHz, disable scaling of the main system clock speed:
 *    \code
 *    #define CONFIG_SYSCLK_PRES         SYSCLK_PRES_1
 *    \endcode
 *    \note Some dividers are powers of two, while others are integer division factors. Refer to the
 *          formulas in the conf_clock.h template commented above each division define.
 */

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \weakgroup sysclk_group
 * @{
 */
 
//! \name Configuration Symbols
//@{
/**
 * \def CONFIG_SYSCLK_SOURCE
 * \brief Initial/static main system clock source
 *
 * The main system clock will be configured to use this clock during
 * initialization.
 */
#ifndef CONFIG_SYSCLK_SOURCE
# define CONFIG_SYSCLK_SOURCE   SYSCLK_SRC_MAINCK_4M_RC
#endif
/**
 * \def CONFIG_SYSCLK_PRES
 * \brief Initial CPU clock divider (mck)
 *
 * The MCK will run at
 * \f[
 *   f_{MCK} = \frac{f_{sys}}{\mathrm{CONFIG\_SYSCLK\_PRES}}\,\mbox{Hz}
 * \f]
 * after initialization.
 */
#ifndef CONFIG_SYSCLK_PRES
# define CONFIG_SYSCLK_PRES  0
#endif

//@}

//! \name Master Clock Sources (MCK)
//@{
#define SYSCLK_SRC_SLCK_RC			0	//!< Internal 32kHz RC oscillator as master source clock
#define SYSCLK_SRC_SLCK_XTAL		1	//!< External 32kHz crystal oscillator as master source clock
#define SYSCLK_SRC_SLCK_BYPASS		2	//!< External 32kHz bypass oscillator as master source clock
#define SYSCLK_SRC_MAINCK_4M_RC		3	//!< Internal 4MHz RC oscillator as master source clock
#define SYSCLK_SRC_MAINCK_8M_RC		4	//!< Internal 8MHz RC oscillator as master source clock
#define SYSCLK_SRC_MAINCK_12M_RC	5	//!< Internal 12MHz RC oscillator as master source clock
#define SYSCLK_SRC_MAINCK_XTAL		6	//!< External crystal oscillator as master source clock
#define SYSCLK_SRC_MAINCK_BYPASS	7	//!< External bypass oscillator as master source clock
#define SYSCLK_SRC_PLLACK			8	//!< Use PLLACK as master source clock
#define SYSCLK_SRC_UPLLCK			9	//!< Use UPLLCK as master source clock
//@}

//! \name Master Clock Prescalers (MCK)
//@{
#define SYSCLK_PRES_1			PMC_MCKR_PRES_CLK_1		//!< Set master clock prescaler to 1
#define SYSCLK_PRES_2			PMC_MCKR_PRES_CLK_2		//!< Set master clock prescaler to 2
#define SYSCLK_PRES_4			PMC_MCKR_PRES_CLK_4		//!< Set master clock prescaler to 4
#define SYSCLK_PRES_8			PMC_MCKR_PRES_CLK_8		//!< Set master clock prescaler to 8
#define SYSCLK_PRES_16			PMC_MCKR_PRES_CLK_16	//!< Set master clock prescaler to 16
#define SYSCLK_PRES_32			PMC_MCKR_PRES_CLK_32	//!< Set master clock prescaler to 32
#define SYSCLK_PRES_64			PMC_MCKR_PRES_CLK_64	//!< Set master clock prescaler to 64
#define SYSCLK_PRES_3			PMC_MCKR_PRES_CLK_3		//!< Set master clock prescaler to 3
//@}

//! \name USB Clock Sources
//@{
#define USBCLK_SRC_PLL0       0     //!< Use PLLA
#define USBCLK_SRC_UPLL       1     //!< Use UPLL
//@}

/**
 * \def CONFIG_USBCLK_SOURCE
 * \brief Configuration symbol for the USB generic clock source
 *
 * Sets the clock source to use for the USB. The source must also be properly
 * configured.
 *
 * Define this to one of the \c USBCLK_SRC_xxx settings. Leave it undefined if
 * USB is not required.
 */
#ifdef __DOXYGEN__
# define CONFIG_USBCLK_SOURCE
#endif

/**
 * \def CONFIG_USBCLK_DIV
 * \brief Configuration symbol for the USB generic clock divider setting
 *
 * Sets the clock division for the USB generic clock. If a USB clock source is
 * selected with CONFIG_USBCLK_SOURCE, this configuration symbol must also be
 * defined.
 */
#ifdef __DOXYGEN__
# define CONFIG_USBCLK_DIV
#endif

/**
 * \name Querying the system clock
 *
 * The following functions may be used to query the current frequency of
 * the system clock and the CPU and bus clocks derived from it.
 * sysclk_get_main_hz() and sysclk_get_cpu_hz() can be assumed to be
 * available on all platforms, although some platforms may define
 * additional accessors for various chip-internal bus clocks. These are
 * usually not intended to be queried directly by generic code.
 */
//@{

/**
 * \brief Return the current rate in Hz of the main system clock
 *
 * \todo This function assumes that the main clock source never changes
 * once it's been set up, and that PLL0 always runs at the compile-time
 * configured default rate. While this is probably the most common
 * configuration, which we want to support as a special case for
 * performance reasons, we will at some point need to support more
 * dynamic setups as well.
 */
#if (defined CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
extern uint32_t sysclk_initialized;
#endif
static inline uint32_t sysclk_get_main_hz(void)
{
#if (defined CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
	if (!sysclk_initialized ) {
		return OSC_MAINCK_4M_RC_HZ;
	}
#endif

	/* Config system clock setting */
	switch (CONFIG_SYSCLK_SOURCE) {
	case SYSCLK_SRC_SLCK_RC:
		return OSC_SLCK_32K_RC_HZ;
	
	case SYSCLK_SRC_SLCK_XTAL:
		return OSC_SLCK_32K_XTAL_HZ;
		
	case SYSCLK_SRC_SLCK_BYPASS:
		return OSC_SLCK_32K_BYPASS_HZ;


    case SYSCLK_SRC_MAINCK_4M_RC:
		return OSC_MAINCK_4M_RC_HZ;

    case SYSCLK_SRC_MAINCK_8M_RC:
		return OSC_MAINCK_8M_RC_HZ;

    case SYSCLK_SRC_MAINCK_12M_RC:
		return OSC_MAINCK_12M_RC_HZ;

    case SYSCLK_SRC_MAINCK_XTAL:
		return OSC_MAINCK_XTAL_HZ;

    case SYSCLK_SRC_MAINCK_BYPASS:
		return OSC_MAINCK_BYPASS_HZ;

#ifdef CONFIG_PLL0_SOURCE
	case SYSCLK_SRC_PLLACK:
		return pll_get_default_rate(0);	
#endif

#ifdef CONFIG_PLL1_SOURCE
	case SYSCLK_SRC_UPLLCK:
		return PLL_UPLL_HZ;	
#endif
	
	default:
		/* unhandled_case(CONFIG_SYSCLK_SOURCE); */
		return 0;
	}
}

/**
 * \brief Return the current rate in Hz of the CPU clock
 *
 * \todo This function assumes that the CPU always runs at the system
 * clock frequency. We want to support at least two more scenarios:
 * Fixed CPU/bus clock dividers (config symbols) and dynamic CPU/bus
 * clock dividers (which may change at run time). Ditto for all the bus
 * clocks.
 *
 * \return Frequency of the CPU clock, in Hz.
 */
static inline uint32_t sysclk_get_cpu_hz(void)
{
	/* CONFIG_SYSCLK_PRES is the register value for setting the expected */
	/* prescaler, not an immediate value. */
	return sysclk_get_main_hz() /
		((CONFIG_SYSCLK_PRES == SYSCLK_PRES_3) ? 3 :
			(1 << (CONFIG_SYSCLK_PRES >> PMC_MCKR_PRES_Pos)));
}

/**
 * \brief Retrieves the current rate in Hz of the peripheral clocks.
 *
 * \return Frequency of the peripheral clocks, in Hz.
 */
static inline uint32_t sysclk_get_peripheral_hz(void)
{
	/* CONFIG_SYSCLK_PRES is the register value for setting the expected */
	/* prescaler, not an immediate value. */
	return sysclk_get_main_hz() /
		((CONFIG_SYSCLK_PRES == SYSCLK_PRES_3) ? 3 :
			(1 << (CONFIG_SYSCLK_PRES >> PMC_MCKR_PRES_Pos)));
}

//@}

//! \name Enabling and disabling synchronous clocks
//@{

/**
 * \brief Enable a peripheral's clock.
 *
 * \param ul_id Id (number) of the peripheral clock.
 */
static inline void sysclk_enable_peripheral_clock(uint32_t ul_id)
{
	pmc_enable_periph_clk(ul_id);
}

/**
 * \brief Disable a peripheral's clock.
 *
 * \param ul_id Id (number) of the peripheral clock.
 */
static inline void sysclk_disable_peripheral_clock(uint32_t ul_id)
{
	pmc_disable_periph_clk(ul_id);
}

//@}

//! \name System Clock Source and Prescaler configuration
//@{

extern void sysclk_set_prescalers(uint32_t ul_pres);
extern void sysclk_set_source(uint32_t ul_src);

//@}

extern void sysclk_enable_usb(void);
extern void sysclk_disable_usb(void);

extern void sysclk_init(void);

//! @}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* CHIP_SYSCLK_H_INCLUDED */
