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

#ifndef SSC_H_INCLUDED
#define SSC_H_INCLUDED

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

//! Receive stop selection.
#define SSC_RX_STOP_COMPARE_0        0
#define SSC_RX_STOP_COMPARE_0_1      1

//! Compare register ID.
#define COMPARE_ID0                  0
#define COMPARE_ID1                  1

//! SSC module default timeout. */
#define SSC_DEFAULT_TIMEOUT          10000

//! \brief SSC driver return codes.
enum ssc_return_code {
	SSC_RC_OK = 0,              //!< OK
	SSC_RC_YES = 0,             //!< Yes
	SSC_RC_NO = 1,              //!< No
	SSC_RC_ERROR = 1,           //!< General error
	SSC_RC_INVALID = 0xFFFFFFFF //!< Parameter invalid
};

//! Data frame structure.
typedef struct {
	//! Data bits length per transfer, should be 0 to 31.
	uint32_t ul_datlen;
	//! Bit sequence LSBF or MSBF.
	//! For receiver configuration, SSC_RFMR_MSBF or 0.
	//! For transmitter configuration, SSC_TFMR_MSBF or 0.
	uint32_t ul_msbf;
	//! Data number per frame, should be 0 to 15.
	uint32_t ul_datnb;
	//! Frame Sync. length should be 0 to 15.
	uint32_t ul_fslen;
	//! Frame Sync. length extension field, should be 0 to 15.
	uint32_t ul_fslen_ext;
	//! Frame Sync. output selection.
	//! For receiver configuration, one of SSC_RFMR_FSOS_NONE, SSC_RFMR_FSOS_NEGATIVE, SSC_RFMR_FSOS_POSITIVE,
	//! SSC_RFMR_FSOS_LOW, SSC_RFMR_FSOS_HIGH or SSC_RFMR_FSOS_TOGGLING.
	//! For transmitter configuration, one of SSC_TFMR_FSOS_NONE, SSC_TFMR_FSOS_NEGATIVE, SSC_TFMR_FSOS_POSITIVE
	//! SSC_TFMR_FSOS_LOW, SSC_TFMR_FSOS_HIGH, SSC_TFMR_FSOS_TOGGLING,
	uint32_t ul_fsos;
	//! Frame Sync. edge detection.
	//! For receiver configuration, SSC_RFMR_FSEDGE_POSITIVE or SSC_RFMR_FSEDGE_NEGATIVE.
	//! For transmitter configuration, SSC_TFMR_FSEDGE_POSITIVE or SSC_TFMR_FSEDGE_NEGATIVE.
	uint32_t ul_fsedge;
} data_frame_opt_t;

//! Clock mode structure.
typedef struct {
	//! Communication clock selection.
	//! For receiver configuration, one of SSC_RCMR_CKS_MCK, SSC_RCMR_CKS_TK or SSC_RCMR_CKS_RK.
	//! For transmitter configuration, one of SSC_TCMR_CKS_MCK, SSC_TCMR_CKS_TK or SSC_TCMR_CKS_RK.
	uint32_t ul_cks;
	//! Communication clock output mode selection.
	//! For receiver configuration, one of SSC_RCMR_CKO_NONE, SSC_RCMR_CKO_CONTINUOUS or SSC_RCMR_CKO_TRANSFER.
	//! For transmitter configuration, one of SSC_TCMR_CKO_NONE, SSC_TCMR_CKO_CONTINUOUS or SSC_TCMR_CKO_TRANSFER.
	uint32_t ul_cko;
	//! Communication clock inversion.
	//! For receiver configuration, SSC_RCMR_CKI or 0.
	//! For transmitter configuration, SSC_TCMR_CKI or 0.
	uint32_t ul_cki;
	//! Communication clock gating selection.
	//! For receiver configuration, one of SSC_RCMR_CKG_NONE, SSC_RCMR_CKG_CONTINUOUS and SSC_RCMR_CKG_TRANSFER.
	//! For transmitter configuration, one of SSC_TCMR_CKG_NONE, SSC_TCMR_CKG_CONTINUOUS and SSC_TCMR_CKG_TRANSFER.
	uint32_t ul_ckg;
	//! Period divider selection, should be 0 to 255.
	uint32_t ul_period;
	//! Communication start delay, should be 0 to 255.
	uint32_t ul_sttdly;
	//! Communication start selection.
	//! For receiver configuration, one of SSC_RCMR_START_CONTINUOUS, SSC_RCMR_START_TRANSMIT, SSC_RCMR_START_RF_LOW,
	//! SSC_RCMR_START_RF_HIGH, SSC_RCMR_START_RF_FALLING, SSC_RCMR_START_RF_RISING, SSC_RCMR_START_RF_LEVEL,
	//! SSC_RCMR_START_RF_EDGE or SSC_RCMR_START_CMP_0.
	//! For transmitter configuration, one of SSC_TCMR_START_CONTINUOUS, SSC_TCMR_START_TRANSMIT, SSC_TCMR_START_RF_LOW,
	//! SSC_TCMR_START_RF_HIGH, SSC_TCMR_START_RF_FALLING, SSC_TCMR_START_RF_RISING, SSC_TCMR_START_RF_LEVEL,
	//! SSC_TCMR_START_RF_EDGE or SSC_TCMR_START_CMP_0.
	uint32_t ul_start_sel;
} clock_opt_t;

//! SSC working role in I2S mode.
#define SSC_I2S_MASTER_OUT    (1 << 0) //! Working mode for transmitter as master.
#define SSC_I2S_MASTER_IN     (1 << 1) //! Working mode for receiver as master.
#define SSC_I2S_SLAVE_OUT     (1 << 2) //! Working mode for transmitter as slave.
#define SSC_I2S_SLAVE_IN      (1 << 3) //! Working mode for receiver as slave.

//! Bit for SSC Audio channel left.
#define SSC_AUDIO_CH_LEFT     (1 << 0)
//! Bit for SSC Audio channel right.
#define SSC_AUDIO_CH_RIGHT    (1 << 1)
//! SSC Audio Channel modes.
enum {
	//! Mono, left channel enabled.
	SSC_AUDIO_MONO_LEFT = (SSC_AUDIO_CH_LEFT),
	//! Mono, right channel enabled.
	SSC_AUDIO_MONO_RIGHT = (SSC_AUDIO_CH_RIGHT),
	//! Stereo, two channels.
	SSC_AUDIO_STERO = (SSC_AUDIO_CH_LEFT | SSC_AUDIO_CH_RIGHT)
};

uint32_t ssc_set_clock_divider(Ssc *p_ssc, uint32_t ul_bitclock, uint32_t ul_mck);
void ssc_i2s_set_transmitter(Ssc *p_ssc, uint32_t ul_mode,
		uint32_t ul_cks, uint32_t ul_ch_mode, uint32_t ul_datlen);
void ssc_i2s_set_receiver(Ssc *p_ssc, uint32_t ul_mode,
		uint32_t ul_cks, uint32_t ul_ch_mode, uint32_t ul_datlen);
void ssc_reset(Ssc *p_ssc);
void ssc_enable_rx(Ssc *p_ssc);
void ssc_disable_rx(Ssc *p_ssc);
void ssc_enable_tx(Ssc *p_ssc);
void ssc_disable_tx(Ssc *p_ssc);
void ssc_set_normal_mode(Ssc *p_ssc);
void ssc_set_loop_mode(Ssc *p_ssc);
void ssc_set_rx_stop_selection(Ssc *p_ssc, uint32_t ul_sel);
void ssc_set_td_default_level(Ssc *p_ssc, uint32_t ul_level);
void ssc_enable_tx_frame_sync_data(Ssc *p_ssc);
void ssc_disable_tx_frame_sync_data(Ssc *p_ssc);
void ssc_set_receiver(Ssc *p_ssc, clock_opt_t *p_rx_clk_opt, data_frame_opt_t *p_rx_data_frame);
void ssc_set_transmitter(Ssc *p_ssc, clock_opt_t *p_tx_clk_opt, data_frame_opt_t *p_tx_data_frame);
void ssc_set_rx_compare(Ssc *p_ssc, uint32_t ul_id, uint32_t ul_value);
uint32_t ssc_get_rx_compare(Ssc *p_ssc, uint32_t ul_id);
void ssc_enable_interrupt(Ssc *p_ssc, uint32_t ul_sources);
void ssc_disable_interrupt(Ssc *p_ssc, uint32_t ul_sources);
uint32_t ssc_get_interrupt_mask(Ssc *p_ssc);
uint32_t ssc_get_status(Ssc *p_ssc);
uint32_t ssc_is_tx_ready(Ssc *p_ssc);
uint32_t ssc_is_tx_empty(Ssc *p_ssc);
uint32_t ssc_is_rx_ready(Ssc *p_ssc);
uint32_t ssc_is_tx_enabled(Ssc *p_ssc);
uint32_t ssc_is_rx_enabled(Ssc *p_ssc);
#if (defined _SAM3S_) || (defined _SAM4S_)
uint32_t ssc_is_rx_buf_end(Ssc *p_ssc);
uint32_t ssc_is_tx_buf_end(Ssc *p_ssc);
uint32_t ssc_is_rx_buf_full(Ssc *p_ssc);
uint32_t ssc_is_tx_buf_empty(Ssc *p_ssc);
Pdc *ssc_get_pdc_base(Ssc *p_ssc);
#endif
uint32_t ssc_write(Ssc *p_ssc, uint32_t ul_frame);
uint32_t ssc_read(Ssc *p_ssc, uint32_t *ul_data);
void ssc_write_sync_data(Ssc *p_ssc, uint32_t ul_frame);
uint32_t ssc_read_sync_data(Ssc *p_ssc);
#if ((defined _SAM3XA_) || (defined _SAM3U_))
void *ssc_get_tx_access(Ssc *p_ssc);
void *ssc_get_rx_access(Ssc *p_ssc);
#endif
void ssc_set_writeprotect(Ssc *p_ssc, uint32_t ul_enable);
uint32_t ssc_get_writeprotect_status(Ssc *p_ssc);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* SSC_H_INCLUDED */
