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

#ifndef DACC_H_INCLUDED
#define DACC_H_INCLUDED

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

//! DACC return codes
typedef enum dacc_rc {
	DACC_RC_OK = 0,        //!< Operation OK
	DACC_RC_INVALID_PARAM  //!< Invalid parameter
} dacc_rc_t;

#if SAM3N_SERIES
//! DACC resolution in number of data bits
# define DACC_RESOLUTION     10
#else
//! DACC resolution in number of data bits
# define DACC_RESOLUTION     12
#endif
//! DACC max data value
#define DACC_MAX_DATA       ((1 << DACC_RESOLUTION) - 1)

void dacc_reset(Dacc *p_dacc);
uint32_t dacc_set_trigger(Dacc *p_dacc, uint32_t ul_trigger);
void dacc_disable_trigger(Dacc *p_dacc);
uint32_t dacc_set_transfer_mode(Dacc *p_dacc, uint32_t ul_mode);
void dacc_enable_interrupt(Dacc *p_dacc, uint32_t ul_interrupt_mask);
void dacc_disable_interrupt(Dacc *p_dacc, uint32_t ul_interrupt_mask);
uint32_t dacc_get_interrupt_mask(Dacc *p_dacc);
uint32_t dacc_get_interrupt_status(Dacc *p_dacc);
void dacc_write_conversion_data(Dacc *p_dacc, uint32_t ul_data);
void dacc_set_writeprotect(Dacc *p_dacc, uint32_t ul_enable);
uint32_t dacc_get_writeprotect_status(Dacc *p_dacc);
Pdc *dacc_get_pdc_base(Dacc *p_dacc);

#if (SAM3N_SERIES) || defined(__DOXYGEN__)
void dacc_enable(Dacc *p_dacc);
void dacc_disable(Dacc *p_dacc);
uint32_t dacc_set_timing(Dacc *p_dacc, uint32_t ul_startup,
		uint32_t ul_clock_divider);
#endif /* (SAM3N_SERIES) */

#if (SAM3S_SERIES) || (SAM3XA_SERIES) || (SAM4S_SERIES) || defined(__DOXYGEN__)
uint32_t dacc_set_channel_selection(Dacc *p_dacc, uint32_t ul_channel);
void dacc_enable_flexible_selection(Dacc *p_dacc);

uint32_t dacc_set_power_save(Dacc *p_dacc, uint32_t ul_sleep_mode,
		uint32_t ul_fast_wakeup_mode);
uint32_t dacc_set_timing(Dacc *p_dacc, uint32_t ul_refresh, uint32_t ul_maxs,
		uint32_t ul_startup);
uint32_t dacc_enable_channel(Dacc *p_dacc, uint32_t ul_channel);
uint32_t dacc_disable_channel(Dacc *p_dacc, uint32_t ul_channel);
uint32_t dacc_get_channel_status(Dacc *p_dacc);
uint32_t dacc_set_analog_control(Dacc *p_dacc, uint32_t ul_analog_control);
uint32_t dacc_get_analog_control(Dacc *p_dacc);
#endif /* (SAM3S_SERIES) || (SAM3XA_SERIES) */

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* DACC_H_INCLUDED */
