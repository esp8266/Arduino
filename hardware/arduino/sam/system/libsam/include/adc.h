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

#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/* The max adc sample freq definition*/
#define ADC_FREQ_MAX   20000000
/* The min adc sample freq definition*/
#define ADC_FREQ_MIN    1000000
/* The normal adc startup time*/
#define ADC_STARTUP_NORM     40
/* The fast adc startup time*/
#define ADC_STARTUP_FAST     12

/* Definitions for ADC resolution */
#if SAM3S_SERIES || SAM4S_SERIES || SAM3XA_SERIES
enum adc_resolution_t {
	ADC_10_BITS = ADC_MR_LOWRES_BITS_10,    /* ADC 10-bit resolution */
	ADC_12_BITS = ADC_MR_LOWRES_BITS_12     /* ADC 12-bit resolution */
};
#elif SAM3N_SERIES
enum adc_resolution_t {
	ADC_8_BITS = ADC_MR_LOWRES_BITS_8,      /* ADC 8-bit resolution */
	ADC_10_BITS = ADC_MR_LOWRES_BITS_10     /* ADC 10-bit resolution */
} ;
#elif SAM3U_SERIES
enum adc_resolution_t {
	ADC_8_BITS = ADC_MR_LOWRES_BITS_8,      /* ADC 8-bit resolution */
	ADC_10_BITS = ADC12B_MR_LOWRES_BITS_10, /* ADC 10-bit resolution */
 	ADC_12_BITS = ADC12B_MR_LOWRES_BITS_12  /* ADC 12-bit resolution */
} ;
#endif

/* Definitions for ADC trigger */
enum adc_trigger_t {
	ADC_TRIG_SW = ADC_MR_TRGEN_DIS,  /* Starting a conversion is only possible by software. */
	ADC_TRIG_EXT = ((ADC_MR_TRGSEL_ADC_TRIG0 << ADC_MR_TRGSEL_Pos) &
									ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* External trigger */
	ADC_TRIG_TIO_CH_0 = (ADC_MR_TRGSEL_ADC_TRIG1  & ADC_MR_TRGSEL_Msk) |
											ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 0 */
	ADC_TRIG_TIO_CH_1 = (ADC_MR_TRGSEL_ADC_TRIG2  & ADC_MR_TRGSEL_Msk) |
											ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 1 */
	ADC_TRIG_TIO_CH_2 = (ADC_MR_TRGSEL_ADC_TRIG3  & ADC_MR_TRGSEL_Msk) |
											ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 2 */
#if SAM3S_SERIES || SAM4S_SERIES || SAM3XA_SERIES || SAM3U_SERIES
	ADC_TRIG_PWM_EVENT_LINE_0 = (ADC_MR_TRGSEL_ADC_TRIG4  & ADC_MR_TRGSEL_Msk) |
															ADC_MR_TRGEN, /* PWM Event Line 0 */
	ADC_TRIG_PWM_EVENT_LINE_1 = (ADC_MR_TRGSEL_ADC_TRIG5  & ADC_MR_TRGSEL_Msk) |
															ADC_MR_TRGEN  /* PWM Event Line 1 */
#endif
} ;

#if SAM3S_SERIES || SAM4S_SERIES || SAM3N_SERIES || SAM3XA_SERIES
/* Definitions for ADC channel number */
enum adc_channel_num_t {
	ADC_CHANNEL_0  = 0,
	ADC_CHANNEL_1  = 1,
	ADC_CHANNEL_2  = 2,
	ADC_CHANNEL_3  = 3,
	ADC_CHANNEL_4  = 4,
	ADC_CHANNEL_5  = 5,
	ADC_CHANNEL_6  = 6,
	ADC_CHANNEL_7  = 7,
	ADC_CHANNEL_8  = 8,
	ADC_CHANNEL_9  = 9,
	ADC_CHANNEL_10 = 10,
	ADC_CHANNEL_11 = 11,
	ADC_CHANNEL_12 = 12,
	ADC_CHANNEL_13 = 13,
	ADC_CHANNEL_14 = 14,
	ADC_TEMPERATURE_SENSOR = 15,
} ;
#elif SAM3U_SERIES
/* Definitions for ADC channel number */
enum adc_channel_num_t {
	ADC_CHANNEL_0  = 0,
	ADC_CHANNEL_1  = 1,
	ADC_CHANNEL_2  = 2,
	ADC_CHANNEL_3  = 3,
	ADC_CHANNEL_4  = 4,
	ADC_CHANNEL_5  = 5,
	ADC_CHANNEL_6  = 6,
	ADC_CHANNEL_7  = 7,
} ;
#endif
/* Definitions for ADC gain value */
enum adc_gainvalue_t{
	ADC_GAINVALUE_0 = 0,
	ADC_GAINVALUE_1 = 1,
	ADC_GAINVALUE_2 = 2,
	ADC_GAINVALUE_3 = 3
};
/* Definitions for ADC analog settling time */
#if SAM3S_SERIES || SAM4S_SERIES || SAM3XA_SERIES
enum adc_settling_time_t{
	ADC_SETTLING_TIME_0 = ADC_MR_SETTLING_AST3,
	ADC_SETTLING_TIME_1 = ADC_MR_SETTLING_AST5,
	ADC_SETTLING_TIME_2 = ADC_MR_SETTLING_AST9,
	ADC_SETTLING_TIME_3 = ADC_MR_SETTLING_AST17
};
#endif

#if SAM3S_SERIES || SAM4S_SERIES || SAM3N_SERIES || SAM3XA_SERIES
uint32_t adc_init(Adc *p_adc, const uint32_t ul_mck,
                            const uint32_t ul_adc_clock, const uint8_t uc_startup);
void adc_configure_trigger(Adc *p_adc, const enum adc_trigger_t trigger,
				const uint8_t uc_freerun);
void adc_configure_power_save(Adc *p_adc, const uint8_t uc_sleep, const uint8_t uc_fwup);
void adc_configure_sequence(Adc *p_adc, const enum adc_channel_num_t ch_list[],
                                                               const uint8_t uc_num);
void adc_enable_tag(Adc *p_adc);
void adc_disable_tag(Adc *p_adc);
enum adc_channel_num_t adc_get_tag(const Adc *p_adc);
void adc_start_sequencer(Adc *p_adc);
void adc_stop_sequencer(Adc *p_adc);
void adc_set_comparison_mode(Adc *p_adc, const uint8_t uc_mode);
uint32_t adc_get_comparison_mode(const Adc *p_adc);
void adc_set_comparison_window(Adc *p_adc, const uint16_t us_low_threshold,
                                                                        const uint16_t us_high_threshold);
void adc_set_comparison_channel(Adc *p_adc, const enum adc_channel_num_t channel);
void adc_set_writeprotect(Adc *p_adc, const uint32_t ul_enable);
uint32_t adc_get_writeprotect_status(const Adc *p_adc);
void adc_check(Adc* p_adc, const uint32_t ul_mck);
uint32_t adc_get_overrun_status(const Adc *p_adc);
#elif SAM3U_SERIES
uint32_t adc_init(Adc * p_adc, const uint32_t ul_mck, const uint32_t ul_adc_clock,
		const uint32_t ul_startuptime);
void adc_configure_trigger(Adc *p_adc, const enum adc_trigger_t trigger);
void adc_configure_power_save(Adc *p_adc, const uint8_t uc_sleep);
#endif

void adc_set_resolution(Adc *p_adc, const enum adc_resolution_t resolution);
void adc_start(Adc *p_adc);
void adc_stop(Adc *p_adc);
void adc_enable_channel(Adc *p_adc, const enum adc_channel_num_t adc_ch);
void adc_disable_channel(Adc *p_adc, const enum adc_channel_num_t adc_ch);
void adc_enable_all_channel(Adc *p_adc);
void adc_disable_all_channel(Adc *p_adc);
uint32_t adc_get_channel_status(const Adc *p_adc, const enum adc_channel_num_t adc_ch);
uint32_t adc_get_channel_value(const Adc *p_adc,const  enum adc_channel_num_t adc_ch);
uint32_t adc_get_latest_value(const Adc *p_adc);
uint32_t adc_get_actual_adc_clock(const Adc *p_adc, const uint32_t ul_mck);
void adc_enable_interrupt(Adc *p_adc, const uint32_t ul_source);
void adc_disable_interrupt(Adc *p_adc, const uint32_t ul_source);
uint32_t adc_get_status(const Adc *p_adc);
uint32_t adc_get_interrupt_mask(const Adc *p_adc);
Pdc *adc_get_pdc_base(const Adc *p_adc);

#if SAM3S_SERIES || SAM4S_SERIES || SAM3XA_SERIES
void adc_configure_timing(Adc *p_adc, const uint8_t uc_tracking,
                                         const enum adc_settling_time_t settling, const uint8_t uc_transfer);
void adc_enable_anch( Adc *p_adc );
void adc_disable_anch( Adc *p_adc );
void adc_enable_channel_differential_input(Adc *p_adc, const enum adc_channel_num_t channel);
void adc_disable_channel_differential_input(Adc *p_adc, const enum adc_channel_num_t channel);
void adc_enable_channel_input_offset(Adc *p_adc, const enum adc_channel_num_t channel);
void adc_disable_channel_input_offset(Adc *p_adc, const enum adc_channel_num_t channel);
void adc_set_channel_input_gain(Adc *p_adc, const enum adc_channel_num_t channel,
                                                       const enum adc_gainvalue_t uc_gain);
void adc_set_bias_current(Adc *p_adc, const uint8_t uc_ibctl);
void adc_enable_ts(Adc *p_adc);
void adc_disable_ts(Adc *p_adc);
#elif SAM3N_SERIES
void adc_configure_timing(Adc *p_adc, const uint8_t uc_tracking);
#elif SAM3U_SERIES
void adc_configure_timing(Adc *p_adc, const uint32_t ul_sh);
#endif

#if  SAM3SD8_SERIES || SAM4S_SERIES
void adc_set_calibmode(Adc *p_adc);
#endif

#if SAM3U_SERIES
uint32_t adc12b_init(Adc12b *p_adc, const uint32_t ul_mck, const uint32_t ul_adc_clock,
                                   const uint32_t ul_startuptime, const uint32_t ul_offmode_startuptime);
void adc12b_set_resolution(Adc12b *p_adc, const enum adc_resolution_t resolution);
void adc12b_configure_trigger(Adc12b *p_adc, const enum adc_trigger_t trigger);
void adc12b_configure_power_save(Adc12b *p_adc, const uint8_t uc_sleep, const uint8_t uc_offmode);
void adc12b_configure_timing(Adc12b *p_adc, const uint32_t ul_sh);
void adc12b_start(Adc12b *p_adc);
void adc12b_stop(Adc12b *p_adc);
void adc12b_enable_channel(Adc12b *p_adc, const enum adc_channel_num_t adc_ch);
void adc12b_disable_channel(Adc12b *p_adc, const enum adc_channel_num_t adc_ch);
void adc12b_enable_all_channel(Adc12b *p_adc);
void adc12b_disable_all_channel(Adc12b *p_adc);
uint32_t adc12b_get_channel_status(const Adc12b *p_adc,const enum adc_channel_num_t adc_ch);
uint32_t adc12b_get_channel_value(const Adc12b *p_adc, const enum adc_channel_num_t adc_ch);
uint32_t adc12b_get_latest_value(const Adc12b *p_adc);
void adc12b_enable_differential_input(Adc12b *p_adc);
void adc12b_disable_differential_input(Adc12b *p_adc);
void adc12b_enable_input_offset(Adc12b *p_adc);
void adc12b_disable_input_offset(Adc12b *p_adc);
void adc12b_set_input_gain(Adc12b *p_adc, const enum adc_gainvalue_t uc_gain);
uint32_t adc12b_get_actual_adc_clock(const Adc12b *p_adc, const uint32_t ul_mck);
void adc12b_enable_interrupt(Adc12b *p_adc, const uint32_t ul_source);
void adc12b_disable_interrupt(Adc12b *p_adc, const uint32_t ul_source);
uint32_t adc12b_get_interrupt_mask(const Adc12b *p_adc);
uint32_t adc12b_get_status(const Adc12b *p_adc);
void adc12b_set_bias_current(Adc12b *p_adc, const uint8_t uc_ibctl);
Pdc *adc12b_get_pdc_base(const Adc12b *p_adc);
#endif

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* ADC_H_INCLUDED */
