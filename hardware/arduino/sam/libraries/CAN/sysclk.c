/**
 * \file
 *
 * \brief Chip-specific system clock management functions.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
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

#include <sysclk.h>

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

#if defined(CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
/**
 * \brief boolean signalling that the sysclk_init is done.
 */
uint32_t sysclk_initialized = 0;
#endif

/**
 * \brief Set system clock prescaler configuration
 *
 * This function will change the system clock prescaler configuration to
 * match the parameters.
 *
 * \note The parameters to this function are device-specific.
 *
 * \param cpu_shift The CPU clock will be divided by \f$2^{mck\_pres}\f$
 */
void sysclk_set_prescalers(uint32_t ul_pres)
{
	pmc_mck_set_prescaler(ul_pres);
	SystemCoreClockUpdate();
}

/**
 * \brief Change the source of the main system clock.
 *
 * \param src The new system clock source. Must be one of the constants
 * from the <em>System Clock Sources</em> section.
 */
void sysclk_set_source(uint32_t ul_src)
{
	switch (ul_src) {
	case SYSCLK_SRC_SLCK_RC:
	case SYSCLK_SRC_SLCK_XTAL:
	case SYSCLK_SRC_SLCK_BYPASS:
		pmc_mck_set_source(PMC_MCKR_CSS_SLOW_CLK);
		break;

	case SYSCLK_SRC_MAINCK_4M_RC:
	case SYSCLK_SRC_MAINCK_8M_RC:
	case SYSCLK_SRC_MAINCK_12M_RC:
	case SYSCLK_SRC_MAINCK_XTAL:
	case SYSCLK_SRC_MAINCK_BYPASS:
		pmc_mck_set_source(PMC_MCKR_CSS_MAIN_CLK);
		break;

	case SYSCLK_SRC_PLLACK:
		pmc_mck_set_source(PMC_MCKR_CSS_PLLA_CLK);
		break;

	case SYSCLK_SRC_UPLLCK:
		pmc_mck_set_source(PMC_MCKR_CSS_UPLL_CLK);
		break;
	}

	SystemCoreClockUpdate();
}

#if defined(CONFIG_USBCLK_SOURCE) || defined(__DOXYGEN__)
/**
 * \brief Enable full speed USB clock.
 *
 * \note The SAM3X UDP hardware interprets div as div+1. For readability the hardware div+1
 * is hidden in this implementation. Use div as div effective value.
 *
 * \param pll_id Source of the USB clock.
 * \param div Actual clock divisor. Must be superior to 0.
 */
void sysclk_enable_usb(void)
{
	Assert(CONFIG_USBCLK_DIV > 0);

	switch (CONFIG_USBCLK_SOURCE) {
#ifdef CONFIG_PLL0_SOURCE
	case USBCLK_SRC_PLL0: {
		struct pll_config pllcfg;

		pll_enable_source(CONFIG_PLL0_SOURCE);
		pll_config_defaults(&pllcfg, 0);
		pll_enable(&pllcfg, 0);
		pll_wait_for_lock(0);
		pmc_switch_udpck_to_pllack(CONFIG_USBCLK_DIV - 1);
		pmc_enable_udpck();
		break;
	}
#endif

	case USBCLK_SRC_UPLL: {

		pmc_enable_upll_clock();
		pmc_switch_udpck_to_upllck(CONFIG_USBCLK_DIV - 1);
		pmc_enable_udpck();
		break;
	}

	}
}

/**
 * \brief Disable full speed USB clock.
 *
 * \note This implementation does not switch off the PLL, it just turns off the USB clock.
 */
void sysclk_disable_usb(void)
{
	pll_disable(1);
}
#endif // CONFIG_USBCLK_SOURCE

void sysclk_init(void)
{
	struct pll_config pllcfg;

	/* Set a flash wait state depending on the new cpu frequency */
	system_init_flash(sysclk_get_cpu_hz());

	/* Config system clock setting */
	switch (CONFIG_SYSCLK_SOURCE) {
	case SYSCLK_SRC_SLCK_RC:
		osc_enable(OSC_SLCK_32K_RC);
		osc_wait_ready(OSC_SLCK_32K_RC);
		pmc_switch_mck_to_sclk(CONFIG_SYSCLK_PRES);
		break;

	case SYSCLK_SRC_SLCK_XTAL:
		osc_enable(OSC_SLCK_32K_XTAL);
		osc_wait_ready(OSC_SLCK_32K_XTAL);
		pmc_switch_mck_to_sclk(CONFIG_SYSCLK_PRES);
		break;

	case SYSCLK_SRC_SLCK_BYPASS:
		osc_enable(OSC_SLCK_32K_BYPASS);
		osc_wait_ready(OSC_SLCK_32K_BYPASS);
		pmc_switch_mck_to_sclk(CONFIG_SYSCLK_PRES);
		break;

	case SYSCLK_SRC_MAINCK_4M_RC:
		/* Already running from SYSCLK_SRC_MAINCK_4M_RC */
		break;

	case SYSCLK_SRC_MAINCK_8M_RC:
		osc_enable(OSC_MAINCK_8M_RC);
		osc_wait_ready(OSC_MAINCK_8M_RC);
		pmc_switch_mck_to_mainck(CONFIG_SYSCLK_PRES);
		break;

	case SYSCLK_SRC_MAINCK_12M_RC:
		osc_enable(OSC_MAINCK_12M_RC);
		osc_wait_ready(OSC_MAINCK_12M_RC);
		pmc_switch_mck_to_mainck(CONFIG_SYSCLK_PRES);
		break;


	case SYSCLK_SRC_MAINCK_XTAL:
		osc_enable(OSC_MAINCK_XTAL);
		osc_wait_ready(OSC_MAINCK_XTAL);
		pmc_switch_mck_to_mainck(CONFIG_SYSCLK_PRES);
		break;

	case SYSCLK_SRC_MAINCK_BYPASS:
		osc_enable(OSC_MAINCK_BYPASS);
		osc_wait_ready(OSC_MAINCK_BYPASS);
		pmc_switch_mck_to_mainck(CONFIG_SYSCLK_PRES);
		break;

#ifdef CONFIG_PLL0_SOURCE
	case SYSCLK_SRC_PLLACK:
		pll_enable_source(CONFIG_PLL0_SOURCE);
		pll_config_defaults(&pllcfg, 0);
		pll_enable(&pllcfg, 0);
		pll_wait_for_lock(0);
		pmc_switch_mck_to_pllack(CONFIG_SYSCLK_PRES);
		break;
#endif

	case SYSCLK_SRC_UPLLCK:
		pll_enable_source(CONFIG_PLL1_SOURCE);
		pll_config_defaults(&pllcfg, 1);
		pll_enable(&pllcfg, 1);
		pll_wait_for_lock(1);
		pmc_switch_mck_to_upllck(CONFIG_SYSCLK_PRES);
		break;
	}

	/* Update the SystemFrequency variable */
	SystemCoreClockUpdate();

#if (defined CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
	/* Signal that the internal frequencies are setup */
	sysclk_initialized = 1;
#endif
}

//! @}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
