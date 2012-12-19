/**
 * \file
 *
 * \brief Synchronous Serial Controller (SSC) driver for SAM.
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

#include <string.h>
#include "ssc.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_ssc_group Synchronous Serial Controller (SSC)
 *
 * The Synchronous Serial Controller (SSC) provides a synchronous communication
 * link with external devices. It supports many serial synchronous communication
 * protocols generally used in audio and telecom applications such as I2S,
 * Short Frame Sync, Long Frame Sync, etc.
 * This is a driver for configuration and use of the SSC peripheral.
 *
 * @{
 */

#define SSC_WPKEY SSC_WPMR_WPKEY(0x535343)

/**
 * \brief Set up clock.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_bitrate Desired bit clock.
 * \param ul_mck MCK clock.
 *
 * \retval SSC_RC_YES Success.
 * \retval SSC_RC_NO Invalid input value.
 */
uint32_t ssc_set_clock_divider(Ssc *p_ssc, uint32_t ul_bitrate,
		uint32_t ul_mck)
{
	if (ul_mck && ul_bitrate) {
		p_ssc->SSC_CMR = SSC_CMR_DIV(((ul_mck + ul_bitrate) / ul_bitrate) >> 1);
		return SSC_RC_YES;
	} else {
		return SSC_RC_NO;
	}
}

/**
 * \brief Setup for I2S transmitter.
 *
 * \note If working in master mode, the divided clock needs to be configured before
 * calling this function according to the sample rate and ul_datlen field.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_mode Working mode, SSC_I2S_MASTER_OUT or SSC_I2S_SLAVE_OUT.
 * \param ul_cks Source clock selection while working in SSC_I2S_SLAVE_OUT mode.
 * \param ul_ch_mode Channel mode, stereo or mono.
 * \param ul_datlen Data length for one channel.
 */
void ssc_i2s_set_transmitter(Ssc *p_ssc, uint32_t ul_mode,
		uint32_t ul_cks, uint32_t ul_ch_mode, uint32_t ul_datlen)
{
	clock_opt_t tx_clk_option;
	data_frame_opt_t tx_data_frame_option;

	/* Initialize the local variable. */
	memset((uint8_t *)&tx_clk_option, 0, sizeof(clock_opt_t));
	memset((uint8_t *)&tx_data_frame_option, 0, sizeof(data_frame_opt_t));

	/* Data start: MonoLeft-Falling, MonoRight-Rising, Stero-Edge. */
	switch (ul_ch_mode) {
	case SSC_AUDIO_MONO_RIGHT:
		tx_clk_option.ul_start_sel = SSC_TCMR_START_RF_RISING;
		break;
	case SSC_AUDIO_MONO_LEFT:
		tx_clk_option.ul_start_sel = SSC_TCMR_START_RF_FALLING;
		break;
	case SSC_AUDIO_STERO:
		tx_clk_option.ul_start_sel = SSC_TCMR_START_RF_EDGE;
		break;
	}
	if (ul_mode & SSC_I2S_MASTER_OUT) {
		/* Stereo has 2 data words, and mono has only one data word. */
		if (SSC_AUDIO_STERO == ul_ch_mode) {
			tx_data_frame_option.ul_datnb = 1;
		} else {
			tx_data_frame_option.ul_datnb = 0;
		}

		/* Configure TCMR Settings. */
		tx_clk_option.ul_cks = SSC_TCMR_CKS_MCK;
		tx_clk_option.ul_cko = SSC_TCMR_CKO_CONTINUOUS;
		tx_clk_option.ul_cki = 0;
		tx_clk_option.ul_ckg = SSC_RCMR_CKG_NONE;
		/* The delay is defined by I2S protocol. */
		tx_clk_option.ul_sttdly = 1;
		tx_clk_option.ul_period = ul_datlen - 1;

		/* Configure TFMR Settings. */
		tx_data_frame_option.ul_datlen = ul_datlen - 1;
		tx_data_frame_option.ul_msbf = SSC_TFMR_MSBF;
		tx_data_frame_option.ul_fslen = ul_datlen - 1;
		tx_data_frame_option.ul_fsos = SSC_TFMR_FSOS_NEGATIVE;
		tx_data_frame_option.ul_fsedge = SSC_TFMR_FSEDGE_POSITIVE;
	} else if (ul_mode & SSC_I2S_SLAVE_OUT) {
		/* Configure TCMR Settings. */
		tx_clk_option.ul_cks = ul_cks;
		tx_clk_option.ul_cko = SSC_TCMR_CKO_NONE;
		tx_clk_option.ul_cki = 0;
		tx_clk_option.ul_ckg = SSC_RCMR_CKG_NONE;
		tx_clk_option.ul_sttdly = 1;
		tx_clk_option.ul_period = 0;

		/* Configure TFMR Settings. */
		tx_data_frame_option.ul_datlen = ul_datlen - 1;
		tx_data_frame_option.ul_msbf = SSC_TFMR_MSBF;
		tx_data_frame_option.ul_fslen = 0;
		tx_data_frame_option.ul_fsos = SSC_TFMR_FSOS_NONE;
		tx_data_frame_option.ul_fsedge = SSC_TFMR_FSEDGE_POSITIVE;
	}
	/* Configure the default level on TD pin. */
	ssc_set_td_default_level(p_ssc, 0);

	/* Configure the SSC transmitter. */
	ssc_set_transmitter(p_ssc, &tx_clk_option, &tx_data_frame_option);
}

/**
 * \brief Setup for I2S receiver.
 *
 * \note If working in master mode, the divided clock needs to be configured before
 * calling this function according to the sample rate and ul_datlen field.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_mode Working mode, SSC_I2S_MASTER_IN or SSC_I2S_SLAVE_IN.
 * \param ul_cks Source clock selection while working in SSC_I2S_SLAVE_IN mode.
 * \param ul_ch_mode Channel mode, stereo or mono.
 * \param ul_datlen Data length for one channel.
 */
void ssc_i2s_set_receiver(Ssc *p_ssc, uint32_t ul_mode,
		uint32_t ul_cks, uint32_t ul_ch_mode, uint32_t ul_datlen)
{
	clock_opt_t rx_clk_option;
	data_frame_opt_t rx_data_frame_option;

	/* Initialize the local variable. */
	memset((uint8_t *)&rx_clk_option, 0, sizeof(clock_opt_t));
	memset((uint8_t *)&rx_data_frame_option, 0, sizeof(data_frame_opt_t));

	/* Data start: MonoLeft-Falling, MonoRight-Rising, Stero-Edge. */
	switch (ul_ch_mode) {
	case SSC_AUDIO_MONO_RIGHT:
		rx_clk_option.ul_start_sel = SSC_RCMR_START_RF_RISING;
		break;
	case SSC_AUDIO_MONO_LEFT:
		rx_clk_option.ul_start_sel = SSC_RCMR_START_RF_FALLING;
		break;
	case SSC_AUDIO_STERO:
		rx_clk_option.ul_start_sel = SSC_RCMR_START_RF_EDGE;
		break;
	}
	if (ul_mode & SSC_I2S_MASTER_IN) {
		/* Stereo has 2 data words, and mono has only one data word. */
		if (SSC_AUDIO_STERO == ul_ch_mode) {
			rx_data_frame_option.ul_datnb = 1;
		} else {
			rx_data_frame_option.ul_datnb = 0;
		}

		/* Configure RCMR Settings. */
		rx_clk_option.ul_cks = SSC_TCMR_CKS_MCK;
		rx_clk_option.ul_cko = SSC_TCMR_CKO_CONTINUOUS;
		rx_clk_option.ul_cki = 0;
		rx_clk_option.ul_ckg = SSC_RCMR_CKG_NONE;
		rx_clk_option.ul_sttdly = 1;
		rx_clk_option.ul_period = ul_datlen - 1;

		/* Configure RFMR Settings. */
		rx_data_frame_option.ul_datlen = ul_datlen - 1;
		rx_data_frame_option.ul_msbf = SSC_TFMR_MSBF;
		rx_data_frame_option.ul_fslen = ul_datlen - 1;
		rx_data_frame_option.ul_fsos = SSC_TFMR_FSOS_NEGATIVE;
		rx_data_frame_option.ul_fsedge = SSC_TFMR_FSEDGE_POSITIVE;
	} else if (ul_mode & SSC_I2S_SLAVE_IN) {
		/* Configure TCMR Settings. */
		rx_clk_option.ul_cks = ul_cks;
		rx_clk_option.ul_cko = SSC_TCMR_CKO_NONE;
		rx_clk_option.ul_cki = 0;
		rx_clk_option.ul_ckg = SSC_RCMR_CKG_NONE;
		rx_clk_option.ul_sttdly = 1;
		rx_clk_option.ul_period = 0;

		/* Configure TFMR Settings. */
		rx_data_frame_option.ul_datlen = ul_datlen - 1;
		rx_data_frame_option.ul_msbf = SSC_TFMR_MSBF;
		rx_data_frame_option.ul_fslen = 0;
		rx_data_frame_option.ul_fsos = SSC_TFMR_FSOS_NONE;
		rx_data_frame_option.ul_fsedge = SSC_TFMR_FSEDGE_POSITIVE;
	}

	/* Configure the SSC receiver. */
	ssc_set_receiver(p_ssc, &rx_clk_option, &rx_data_frame_option);
}

/**
 * \brief Reset SSC module.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_reset(Ssc *p_ssc)
{
	p_ssc->SSC_CR = SSC_CR_SWRST;
	p_ssc->SSC_CMR = 0;
	p_ssc->SSC_RCMR = 0;
	p_ssc->SSC_RFMR = 0;
	p_ssc->SSC_TCMR = 0;
	p_ssc->SSC_TFMR = 0;
}

/**
 * \brief Enable SSC receiver.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_enable_rx(Ssc *p_ssc)
{
	p_ssc->SSC_CR = SSC_CR_RXEN;
}

/**
 * \brief Disable SSC receiver.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_disable_rx(Ssc *p_ssc)
{
	p_ssc->SSC_CR = SSC_CR_RXDIS;
}

/**
 * \brief Enable SSC Transmitter.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_enable_tx(Ssc *p_ssc)
{
	p_ssc->SSC_CR = SSC_CR_TXEN;
}

/**
 * \brief Disable SSC Transmitter.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_disable_tx(Ssc *p_ssc)
{
	p_ssc->SSC_CR = SSC_CR_TXDIS;
}

/**
 * \brief Configure SSC to work in normal mode.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_set_normal_mode(Ssc *p_ssc)
{
	p_ssc->SSC_RFMR &= ~SSC_RFMR_LOOP;
}

/**
 * \brief Configure SSC to work in loop mode.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_set_loop_mode(Ssc *p_ssc)
{
	p_ssc->SSC_RFMR |= SSC_RFMR_LOOP;
}

/**
 * \brief Configure SSC receive stop selection.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_sel Compare 0 used or Compare both 0 & 1 used.
 */
void ssc_set_rx_stop_selection(Ssc *p_ssc, uint32_t ul_sel)
{
	if (SSC_RX_STOP_COMPARE_0_1 == ul_sel) {
		p_ssc->SSC_RCMR |= SSC_RCMR_STOP;
	} else if (SSC_RX_STOP_COMPARE_0 == ul_sel) {
		p_ssc->SSC_RCMR &= ~SSC_RCMR_STOP;
	}
}

/**
 * \brief Configure SSC default level driven on the TD pin while
 * out of transmission.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_level The default driven level of TD pin.
 */
void ssc_set_td_default_level(Ssc *p_ssc, uint32_t ul_level)
{
	if (ul_level) {
		p_ssc->SSC_TFMR |= SSC_TFMR_DATDEF;
	} else {
		p_ssc->SSC_TFMR &= ~SSC_TFMR_DATDEF;
	}
}

/**
 * \brief The TD line is driven with the SSC_TSHR register value
 * during the transmission of the Transmit Frame Sync Signal.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_enable_tx_frame_sync_data(Ssc *p_ssc)
{
	p_ssc->SSC_TFMR |= SSC_TFMR_FSDEN;
}

/**
 * \brief The TD line is driven with the default value during the Transmit
 * Frame Sync signal.
 *
 * \param p_ssc Pointer to an SSC instance.
 */
void ssc_disable_tx_frame_sync_data(Ssc *p_ssc)
{
	p_ssc->SSC_TFMR &= ~SSC_TFMR_FSDEN;
}

/**
 * \brief Configure SSC receiver clock mode and date frame configuration.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param p_rx_clk_opt Pointer to the receiver clock configuration structure.
 * \param p_rx_data_frame Pointer to the receiver data frame configuration structure.
 */
void ssc_set_receiver(Ssc *p_ssc, clock_opt_t *p_rx_clk_opt,
		data_frame_opt_t *p_rx_data_frame)
{
	if (p_rx_clk_opt == NULL) {
		p_ssc->SSC_RCMR = 0;
	} else {
		p_ssc->SSC_RCMR |= p_rx_clk_opt->ul_cks |
				p_rx_clk_opt->ul_cko | p_rx_clk_opt->ul_cki |
				p_rx_clk_opt->ul_ckg |
				p_rx_clk_opt->ul_start_sel |
				SSC_RCMR_PERIOD(p_rx_clk_opt->ul_period) |
				SSC_RCMR_STTDLY(p_rx_clk_opt->ul_sttdly);
	}

	if (p_rx_data_frame == NULL) {
		p_ssc->SSC_RFMR = 0;
	} else {
		p_ssc->SSC_RFMR |= SSC_RFMR_DATLEN(p_rx_data_frame->ul_datlen) |
				p_rx_data_frame->ul_msbf |
				SSC_RFMR_DATNB(p_rx_data_frame->ul_datnb) |
				SSC_RFMR_FSLEN(p_rx_data_frame->ul_fslen) |
				SSC_RFMR_FSLEN_EXT(p_rx_data_frame->ul_fslen_ext) |
				p_rx_data_frame->ul_fsos |
				p_rx_data_frame->ul_fsedge;
	}
}

/**
 * \brief Configure SSC transmitter clock mode and date frame configuration.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param p_tx_clk_opt Pointer to the transmitter clock configuration structure.
 * \param p_tx_data_frame Pointer to the transmitter data frame configuration structure.
 */
void ssc_set_transmitter(Ssc *p_ssc, clock_opt_t *p_tx_clk_opt,
		data_frame_opt_t *p_tx_data_frame)
{
	if (p_tx_clk_opt == NULL) {
		p_ssc->SSC_TCMR = 0;
	} else {
		p_ssc->SSC_TCMR |= p_tx_clk_opt->ul_cks |
				p_tx_clk_opt->ul_cko | p_tx_clk_opt->ul_cki |
				p_tx_clk_opt->ul_ckg |
				p_tx_clk_opt->ul_start_sel |
				SSC_RCMR_PERIOD(p_tx_clk_opt->ul_period) |
				SSC_RCMR_STTDLY(p_tx_clk_opt->ul_sttdly);
	}

	if (p_tx_data_frame == NULL) {
		p_ssc->SSC_TFMR = 0;
	} else {
		p_ssc->SSC_TFMR |= SSC_RFMR_DATLEN(p_tx_data_frame->ul_datlen) |
				p_tx_data_frame->ul_msbf |
				SSC_RFMR_DATNB(p_tx_data_frame->ul_datnb) |
				SSC_RFMR_FSLEN(p_tx_data_frame->ul_fslen) |
				SSC_RFMR_FSLEN_EXT(p_tx_data_frame->ul_fslen_ext) |
				p_tx_data_frame->ul_fsos |
				p_tx_data_frame->ul_fsedge;
	}
}

/**
 * \brief Configure SSC Receive Compare Register.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_id Compare register ID.
 * \param ul_value Value to configure.
 */
void ssc_set_rx_compare(Ssc *p_ssc, uint32_t ul_id, uint32_t ul_value)
{
	switch (ul_id) {
	case COMPARE_ID0:
		p_ssc->SSC_RC0R = ul_value;
		break;
	case COMPARE_ID1:
		p_ssc->SSC_RC1R = ul_value;
		break;
	}
}

/**
 * \brief Get SSC Receive Compare Register.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_id Compare register ID.
 *
 * \return Receive Compare Register value for the specified ul_id, otherwise SSC_RC_INVALID.
 */
uint32_t ssc_get_rx_compare(Ssc *p_ssc, uint32_t ul_id)
{
	switch (ul_id) {
	case COMPARE_ID0:
		return p_ssc->SSC_RC0R;
	case COMPARE_ID1:
		return p_ssc->SSC_RC1R;
	default:
		return SSC_RC_INVALID;
	}
}

/**
 * \brief Enable SSC interrupts.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_sources Interrupts to be enabled.
 */
void ssc_enable_interrupt(Ssc *p_ssc, uint32_t ul_sources)
{
	p_ssc->SSC_IER = ul_sources;
}

/**
 * \brief Disable SSC interrupts.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_sources Interrupts to be enabled.
 */
void ssc_disable_interrupt(Ssc *p_ssc, uint32_t ul_sources)
{
	p_ssc->SSC_IDR = ul_sources;
}

/**
 * \brief Read SSC interrupt mask.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \return The interrupt mask value.
 */
uint32_t ssc_get_interrupt_mask(Ssc *p_ssc)
{
	return p_ssc->SSC_IMR;
}

/**
 * \brief Read SSC status.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \return The SSC status value.
 */
uint32_t ssc_get_status(Ssc *p_ssc)
{
	return p_ssc->SSC_SR;
}

/**
 * \brief Check if data has been loaded in SSC_THR and is waiting to be loaded
 * in the Transmit Shift Register (TSR).
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES There is no data in the SSC_THR.
 * \retval SSC_RC_NO There is one data in the SSC_THR.
 */
uint32_t ssc_is_tx_ready(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_TXRDY) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Check if the last data written in SSC_THR has been loaded in TSR
 * and the last data loaded in TSR has been transmitted.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES Both of the two registers are empty.
 * \retval SSC_RC_NO At least one of the two registers is not empty.
 */
uint32_t ssc_is_tx_empty(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_TXEMPTY) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Check if data has been received and loaded in SSC_RHR.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES There is one data in the SSC_RHR.
 * \retval SSC_RC_NO There is no data in the SSC_RHR.
 */
uint32_t ssc_is_rx_ready(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_RXRDY) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Check if transmitter is enabled.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES The transmitter is enabled.
 * \retval SSC_RC_NO The transmitter is disabled.
 */
uint32_t ssc_is_tx_enabled(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_TXEN) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Check if receiver is enabled.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES The receiver is enabled.
 * \retval SSC_RC_NO The receiver is disabled.
 */
uint32_t ssc_is_rx_enabled(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_RXEN) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

#if (SAM3S_SERIES) || (SAM4S_SERIES)
/**
 * \brief Check if one receive buffer is filled.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES Receive Counter has reached zero.
 * \retval SSC_RC_NO Data is written on the Receive Counter Register or
 * Receive Next Counter Register.
 */
uint32_t ssc_is_rx_buf_end(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_ENDRX) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Check if the register SSC_TCR has reached 0.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES The register SSC_TCR has reached 0.
 * \retval SSC_RC_NO The register SSC_TCR hasn't reached 0.
 */
uint32_t ssc_is_tx_buf_end(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_ENDTX) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Check if both receive buffers are full.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES Both of the two receive buffers have reached 0.
 * \retval SSC_RC_NO One of the two receive buffers hasn't reached 0.
 */
uint32_t ssc_is_rx_buf_full(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_RXBUFF) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Check if both transmit buffers are empty.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \retval SSC_RC_YES Both of the two transmit buffers have reached 0.
 * \retval SSC_RC_NO One of the two transmit buffers hasn't reached 0.
 */
uint32_t ssc_is_tx_buf_empty(Ssc *p_ssc)
{
	if (p_ssc->SSC_SR & SSC_SR_TXBUFE) {
		return SSC_RC_YES;
	}
	return SSC_RC_NO;
}

/**
 * \brief Get SSC PDC registers base address.
 *
 * \param p_ssc Pointer to SSC registers set instance.
 *
 * \return SSC PDC registers base address for PDC driver to access.
 */
Pdc *ssc_get_pdc_base(Ssc *p_ssc)
{
	return (Pdc *)&(p_ssc->SSC_RPR);
}
#endif // (SAM3S_SERIES) || (SAM4S_SERIES)

/**
 * \brief Write to SSC Transmit Holding Register.
 * Send data through SSC Data frame.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_frame Frame data to be transmitted.
 *
 * \retval SSC_RC_ERROR Time-out.
 * \retval SSC_RC_OK Success.
 *
 */
uint32_t ssc_write(Ssc *p_ssc, uint32_t ul_frame)
{
	uint32_t ul_timeout = SSC_DEFAULT_TIMEOUT;

	while (!(p_ssc->SSC_SR & SSC_SR_TXEMPTY)) {
		if (!ul_timeout--) {
			return SSC_RC_ERROR;
		}
	}

	p_ssc->SSC_THR = ul_frame;
	return SSC_RC_OK;
}

/**
 * \brief Read from SSC Receive Holding Register.
 * Read data that is received in SSC Data frame.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_data Pointer to the location where to store the received data.
 *
 * \retval SSC_RC_ERROR Time-out.
 * \retval SSC_RC_OK Success.
 */
uint32_t ssc_read(Ssc *p_ssc, uint32_t *ul_data)
{
	uint32_t ul_timeout = SSC_DEFAULT_TIMEOUT;

	while (!(p_ssc->SSC_SR & SSC_SR_RXRDY)) {
		if (!ul_timeout--) {
			return SSC_RC_ERROR;
		}
	}

	*ul_data = p_ssc->SSC_RHR;
	return SSC_RC_OK;
}

/**
 * \brief Write to SSC Transmit Synchronization Holding Register.
 * Send data through SSC Synchronization frame. If there is sync data that needs to be
 * transmitted, call this function first to send out the sync data, and then call the
 * ssc_write() function to send out application data.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_frame Frame Synchronization data.
 */
void ssc_write_sync_data(Ssc *p_ssc, uint32_t ul_frame)
{
	p_ssc->SSC_TSHR = ul_frame;
}

/**
 * \brief Read from SSC Receive Synchronization Holding Register.
 * Read data that is received in SSC Synchronization frame. When the sync data is actually
 * used, after successfully reading the application data by calling ssc_read(), call
 * this function, and the return sync data is useful.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \return Current RSHR value.
 */
uint32_t ssc_read_sync_data(Ssc *p_ssc)
{
	return p_ssc->SSC_RSHR;
}

#if (SAM3XA_SERIES || SAM3U_SERIES)
/**
 * \brief Get Transmit address for DMA operation.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \return Transmitting address for DMA access.
 */
void *ssc_get_tx_access(Ssc *p_ssc)
{
	return (void *)&(p_ssc->SSC_THR);
}

/**
 * \brief Get Receive address for DMA operation.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \return Transmitting address for DMA access.
 */
void *ssc_get_rx_access(Ssc *p_ssc)
{
	return (void *)&(p_ssc->SSC_RHR);
}
#endif // (SAM3XA_SERIES || SAM3U_SERIES)

/**
 * \brief Enable or disable write protection of SSC registers.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_enable 1 to enable, 0 to disable.
 */
void ssc_set_writeprotect(Ssc *p_ssc, uint32_t ul_enable)
{
	if (ul_enable) {
		p_ssc->SSC_WPMR = SSC_WPKEY | SSC_WPMR_WPEN;
	} else {
		p_ssc->SSC_WPMR = SSC_WPKEY;
	}
}

/**
 * \brief Indicate write protect status.
 *
 * \param p_ssc Pointer to an SSC instance.
 *
 * \return 0 if the peripheral is not protected. Write Protect Violation Status otherwise.
 */
uint32_t ssc_get_writeprotect_status(Ssc *p_ssc)
{
	uint32_t ul_reg_val;

	ul_reg_val = p_ssc->SSC_WPMR;
	if (ul_reg_val & SSC_WPMR_WPEN) {
		return (ul_reg_val & SSC_WPSR_WPVSRC_Msk) >> SSC_WPSR_WPVSRC_Pos;
	} else {
		return 0;
	}
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
