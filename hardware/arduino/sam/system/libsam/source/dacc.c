/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#include "dacc.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_dacc_group Digital-to-Analog Converter Controller (DACC)
 *
 * \par Purpose
 *
 * Driver for the Digital-to-Analog Converter Controller. It provides access to the main
 * features of the DAC controller.
 *
 * \par Usage
 *
 * -# DACC clock should be enabled before using it.
 *    - \ref pmc_enable_periph_clk() can be used to enable the clock.
 * -# Reset DACC with \ref dacc_reset().
 * -# If DACC can be enabled/disabled, uses \ref dacc_enable() and
 *    \ref dacc_disable().
 * -# Initialize DACC timing with \ref dacc_set_timing() (different DAC
 *    peripheral may require different parameters).
 * -# Write conversion data with \ref dacc_write_conversion_data().
 * -# Configure trigger with \ref dacc_set_trigger()
 *    and \ref dacc_disable_trigger().
 * -# Configure FIFO transfer mode with \ref dacc_set_transfer_mode().
 * -# Control interrupts with \ref dacc_enable_interrupt(),
 *    \ref dacc_disable_interrupt(), \ref dacc_get_interrupt_mask() and
 *    \ref dacc_get_interrupt_status().
 * -# DACC registers support write protect with \ref dacc_set_writeprotect()
 *    and \ref dacc_get_writeprotect_status().
 * -# If the DACC can work with PDC, use \ref dacc_get_pdc_base() to get
 *    PDC register base for the DAC controller.
 * -# If the DACC has several channels to process, the following functions can
 *    be used:
 *    - Enable/Disable TAG and select output channel selection by
 *      \ref dacc_set_channel_selection(),
 *      \ref dacc_enable_flexible_channel_selection().
 *    - Enable/disable channel by \ref dacc_enable_channel() /
 *      \ref dacc_disable_channel(), get channel status by
 *      \ref dacc_get_channel_status().
 *
 * \section dependencies Dependencies
 * This driver does not depend on other modules.
 *
 * @{
 */

//! Max channel number
#if (SAM3N_SERIES)
# define MAX_CH_NB        0
#else
# define MAX_CH_NB        1
#endif

//! DACC Write Protect Key "DAC" in ASCII
#define DACC_WP_KEY     (0x444143)

/**
 * \brief Reset DACC.
 *
 * \param p_dacc Pointer to a DACC instance.
 */
void dacc_reset(Dacc *p_dacc)
{
	p_dacc->DACC_CR = DACC_CR_SWRST;
}

/**
 * \brief Enable trigger and set the trigger source.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_trigger Trigger source number.
 *
 * \return \ref DACC_RC_OK for OK.
 */
uint32_t dacc_set_trigger(Dacc *p_dacc, uint32_t ul_trigger)
{
	uint32_t mr = p_dacc->DACC_MR & (~(DACC_MR_TRGSEL_Msk));
#if (SAM3N_SERIES)
	p_dacc->DACC_MR = mr
		| DACC_MR_TRGEN
		| ((ul_trigger << DACC_MR_TRGSEL_Pos) & DACC_MR_TRGSEL_Msk);
#else
	p_dacc->DACC_MR = mr | DACC_MR_TRGEN_EN | DACC_MR_TRGSEL(ul_trigger);
#endif
	return DACC_RC_OK;
}

/**
 * \brief Disable trigger (free run mode).
 *
 * \param p_dacc Pointer to a DACC instance.
 */
void dacc_disable_trigger(Dacc *p_dacc)
{
	p_dacc->DACC_MR &= ~DACC_MR_TRGEN;
}

/**
 * \brief Set the transfer mode.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_mode Transfer mode configuration.
 *
 * \return \ref DACC_RC_OK for OK.
 */
uint32_t dacc_set_transfer_mode(Dacc *p_dacc, uint32_t ul_mode)
{
	if (ul_mode) {
#if (SAM3N_SERIES)
		p_dacc->DACC_MR |= DACC_MR_WORD;
#else
		p_dacc->DACC_MR |= DACC_MR_WORD_WORD;
#endif
	} else {
#if (SAM3N_SERIES)
		p_dacc->DACC_MR &= (~DACC_MR_WORD);
#else
		p_dacc->DACC_MR &= (~DACC_MR_WORD_WORD);
#endif
	}
	return DACC_RC_OK;
}

/**
 * \brief Enable DACC interrupts.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_interrupt_mask The interrupt mask.
 */
void dacc_enable_interrupt(Dacc *p_dacc, uint32_t ul_interrupt_mask)
{
	p_dacc->DACC_IER = ul_interrupt_mask;
}

/**
 * \brief Disable DACC interrupts.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_interrupt_mask The interrupt mask.
 */
void dacc_disable_interrupt(Dacc *p_dacc, uint32_t ul_interrupt_mask)
{
	p_dacc->DACC_IDR = ul_interrupt_mask;
}

/**
 * \brief Get the interrupt mask.
 *
 * \param p_dacc Pointer to a DACC instance.
 *
 * \return The interrupt mask.
 */
uint32_t dacc_get_interrupt_mask(Dacc *p_dacc)
{
	return p_dacc->DACC_IMR;
}

/**
 * \brief Get the interrupt status.
 *
 * \param p_dacc Pointer to a DACC instance.
 *
 * \return The interrupt status.
 */
uint32_t dacc_get_interrupt_status(Dacc *p_dacc)
{
	return p_dacc->DACC_ISR;
}

/**
 * \brief Write data to conversion register.
 *
 * \note The \a ul_data could be output data or data with channel TAG when
 * flexible mode is used.
 *
 * In flexible mode the 2 bits, DACC_CDR[13:12] which are otherwise unused,
 * are employed to select the channel in the same way as with the USER_SEL
 * field. Finally, if the WORD field is set, the 2 bits, DACC_CDR[13:12] are
 * used for channel selection of the first data and the 2 bits,
 * DACC_CDR[29:28] for channel selection of the second data.
 *
 * \see dacc_enable_flexible_selection()
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_data The data to be transferred to analog value.
 */
void dacc_write_conversion_data(Dacc *p_dacc, uint32_t ul_data)
{
	p_dacc->DACC_CDR = ul_data;
}

/**
 * \brief Enable or disable write protect of DACC registers.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_enable 1 to enable, 0 to disable.
 */
void dacc_set_writeprotect(Dacc *p_dacc, uint32_t ul_enable)
{
	if (ul_enable) {
		p_dacc->DACC_WPMR = DACC_WPMR_WPKEY(DACC_WP_KEY)
			          | DACC_WPMR_WPEN;
	} else {
		p_dacc->DACC_WPMR = DACC_WPMR_WPKEY(DACC_WP_KEY);
	}
}

/**
 * \brief Get the write protect status.
 *
 * \param p_dacc Pointer to a DACC instance.
 *
 * \return Write protect status.
 */
uint32_t dacc_get_writeprotect_status(Dacc *p_dacc)
{
	return p_dacc->DACC_WPSR;
}

/**
 * \brief Get PDC registers base address.
 *
 * \param p_dacc Pointer to a DACC instance.
 *
 * \return DACC PDC register base address.
 */
Pdc *dacc_get_pdc_base(Dacc *p_dacc)
{
	p_dacc = p_dacc;
	return PDC_DACC;
}

#if (SAM3N_SERIES) || defined(__DOXYGEN__)
/**
 * \brief Enable DACC.
 *
 * \param p_dacc Pointer to a DACC instance.
 */
void dacc_enable(Dacc *p_dacc)
{
	p_dacc->DACC_MR |= DACC_MR_DACEN;
}

/**
 * \brief Disable DACC.
 *
 * \param p_dacc Pointer to a DACC instance.
 *
 * \return \ref DACC_RC_OK for OK.
 */
void dacc_disable(Dacc *p_dacc)
{
	p_dacc->DACC_MR &= (~DACC_MR_DACEN);
}

/**
 * \brief Set the DACC timing.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_startup Startup time selection.
 * \param ul_clock_divider Clock divider for internal trigger.
 *
 * \return \ref DACC_RC_OK for OK.
 */
uint32_t dacc_set_timing(Dacc *p_dacc, uint32_t ul_startup,
		uint32_t ul_clock_divider)
{
	uint32_t mr = p_dacc->DACC_MR
		& ~(DACC_MR_STARTUP_Msk | DACC_MR_CLKDIV_Msk);
	p_dacc->DACC_MR = mr | DACC_MR_STARTUP(ul_startup)
		| DACC_MR_CLKDIV(ul_clock_divider);
	return DACC_RC_OK;
}
#endif /* #if (SAM3N_SERIES) */

#if (SAM3S_SERIES) || (SAM3XA_SERIES) || (SAM4S_SERIES) || defined(__DOXYGEN__)
/**
 * \brief Disable flexible (TAG) mode and select a channel for DAC outputs.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_channel Channel to select.
 *
 * \return \ref DACC_RC_OK if successful.
 */
uint32_t dacc_set_channel_selection(Dacc *p_dacc, uint32_t ul_channel)
{
	uint32_t mr = p_dacc->DACC_MR & (~DACC_MR_USER_SEL_Msk);
	if (ul_channel > MAX_CH_NB) {
		return DACC_RC_INVALID_PARAM;
	}
	mr &= ~(DACC_MR_TAG);
	mr |= ul_channel << DACC_MR_USER_SEL_Pos;
	p_dacc->DACC_MR = mr;
	return DACC_RC_OK;
}

/**
 * \brief Enable the flexible channel selection mode (TAG).
 *
 * In this mode the 2 bits, DACC_CDR[13:12] which are otherwise unused, are
 * employed to select the channel in the same way as with the USER_SEL field.
 * Finally, if the WORD field is set, the 2 bits, DACC_CDR[13:12] are used
 * for channel selection of the first data and the 2 bits, DACC_CDR[29:28]
 * for channel selection of the second data.
 *
 * \param p_dacc Pointer to a DACC instance.
 */
void dacc_enable_flexible_selection(Dacc *p_dacc)
{
	p_dacc->DACC_MR |= DACC_MR_TAG;
}

/**
 * \brief Set the power save mode.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_sleep_mode Sleep mode configuration.
 * \param ul_fast_wakeup_mode Fast wakeup mode configuration.
 *
 * \return \ref DACC_RC_OK if successful.
 */
uint32_t dacc_set_power_save(Dacc *p_dacc,
		uint32_t ul_sleep_mode, uint32_t ul_fast_wakeup_mode)
{
	if (ul_sleep_mode) {
		p_dacc->DACC_MR |= DACC_MR_SLEEP;
	} else {
		p_dacc->DACC_MR &= (~DACC_MR_SLEEP);
	}
	if (ul_fast_wakeup_mode) {
		p_dacc->DACC_MR |= DACC_MR_FASTWKUP;
	} else {
		p_dacc->DACC_MR &= (~DACC_MR_FASTWKUP);
	}
	return DACC_RC_OK;
}

/**
 * \brief Set DACC timings.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_refresh Refresh period setting value.
 * \param ul_maxs Max speed mode configuration.
 * \param ul_startup Startup time selection.
 *
 * \return \ref DACC_RC_OK for OK.
 */
uint32_t dacc_set_timing(Dacc *p_dacc,
		uint32_t ul_refresh, uint32_t ul_maxs, uint32_t ul_startup)
{
	uint32_t mr = p_dacc->DACC_MR
		& (~(DACC_MR_REFRESH_Msk | DACC_MR_STARTUP_Msk));
	mr |= DACC_MR_REFRESH(ul_refresh);
	if (ul_maxs) {
		mr |= DACC_MR_MAXS;
	} else {
		mr &= ~DACC_MR_MAXS;
	}
	mr |= (DACC_MR_STARTUP_Msk & ((ul_startup) << DACC_MR_STARTUP_Pos));
	p_dacc->DACC_MR = mr;
	return DACC_RC_OK;
}

/**
 * \brief Enable DACC channel.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_channel The output channel to enable.
 *
 * \return \ref DACC_RC_OK for OK.
 */
uint32_t dacc_enable_channel(Dacc *p_dacc, uint32_t ul_channel)
{
	if (ul_channel > MAX_CH_NB)
		return DACC_RC_INVALID_PARAM;

	p_dacc->DACC_CHER = DACC_CHER_CH0 << ul_channel;
	return DACC_RC_OK;
}

/**
 * \brief Disable DACC channel.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_channel The output channel to disable.
 *
 * \return \ref DACC_RC_OK for OK.
 */
uint32_t dacc_disable_channel(Dacc *p_dacc, uint32_t ul_channel)
{
	if (ul_channel > MAX_CH_NB) {
		return DACC_RC_INVALID_PARAM;
	}
	p_dacc->DACC_CHDR = DACC_CHDR_CH0 << ul_channel;
	return DACC_RC_OK;
}

/**
 * \brief Get the channel status.
 *
 * \param p_dacc Pointer to a DACC instance.
 *
 * \return DACC channel status.
 */
uint32_t dacc_get_channel_status(Dacc *p_dacc)
{
	return p_dacc->DACC_CHSR;
}

/**
 * \brief Set the analog control value.
 *
 * \param p_dacc Pointer to a DACC instance.
 * \param ul_analog_control Analog control configuration.
 *
 * \return \ref DACC_RC_OK for OK.
 */
uint32_t dacc_set_analog_control(Dacc *p_dacc, uint32_t ul_analog_control)
{
	p_dacc->DACC_ACR = ul_analog_control;
	return DACC_RC_OK;
}

/**
 * \brief Get the analog control value.
 *
 * \param p_dacc Pointer to a DACC instance.
 *
 * \return Current setting of analog control.
 */
uint32_t dacc_get_analog_control(Dacc *p_dacc)
{
	return p_dacc->DACC_ACR;
}
#endif /* (SAM3S_SERIES) || (SAM3XA_SERIES) */

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
