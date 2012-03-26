/*! \file *********************************************************************
 *
 * \brief API for SAM3 Analog-to-Digital Converter (ADC/ADC12B) controller.
 *
 * ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 *
 * - Compiler:           IAR EWARM and CodeSourcery GCC for ARM
 * - Supported devices:  All SAM devices with a Analog-to-Digital Converter can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.com/
 *
 *******************************************************************************/
 
#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include "../chip.h"

#define ADC_FREQ_MAX      5000000
#define ADC_STARTUP            20

#define ADC12_FREQ_MAX   20000000
#define ADC12_FREQ_MIN    1000000

#define ADC12_STARTUP_NORM     40
#define ADC12_STARTUP_FAST     12

/* Definitions for ADC resolution */
typedef enum _adc_resolution_t {
#if SAM3N_SERIES || SAM3U_SERIES
    ADC_8_BITS = ADC_MR_LOWRES_BITS_8,  /* ADC 8-bit resolution */
#endif /* SAM3N_SERIES || SAM3U_SERIES */
    ADC_10_BITS = ADC_MR_LOWRES_BITS_10,  /* ADC 10-bit resolution */
#if SAM3S_SERIES || SAM3XA_SERIES
    ADC_12_BITS = ADC_MR_LOWRES_BITS_12,  /* ADC 12-bit resolution */
#endif /* SAM3S_SERIES || SAM3XA_SERIES */
    ADC_NONE
} adc_resolution_t;

#if SAM3U_SERIES
typedef enum _adc12_resolution_t {
    ADC12_10_BITS = ADC12B_MR_LOWRES_BITS_10,  /* ADC 10-bit resolution */
    ADC12_12_BITS = ADC12B_MR_LOWRES_BITS_12  /* ADC 12-bit resolution */
} adc12_resolution_t;
#endif /* SAM3U_SERIES */

/* Definitions for ADC trigger */
typedef enum _adc_trigger_t {
    ADC_TRIG_SW               = ADC_MR_TRGEN_DIS,  /* Starting a conversion is only possible by software. */
    ADC_TRIG_EXT              = ((ADC_MR_TRGSEL_ADC_TRIG0 <<ADC_MR_TRGSEL_Pos) & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* External trigger */
    ADC_TRIG_TIO_CH_0         = (ADC_MR_TRGSEL_ADC_TRIG1 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 0 */
    ADC_TRIG_TIO_CH_1         = (ADC_MR_TRGSEL_ADC_TRIG2 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 1 */
    ADC_TRIG_TIO_CH_2         = (ADC_MR_TRGSEL_ADC_TRIG3 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 2 */
#if SAM3S_SERIES || SAM3XA_SERIES || SAM3U_SERIES
    ADC_TRIG_PWM_EVENT_LINE_0 = (ADC_MR_TRGSEL_ADC_TRIG4 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* PWM Event Line 0 */
    ADC_TRIG_PWM_EVENT_LINE_1 = (ADC_MR_TRGSEL_ADC_TRIG5 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN  /* PWM Event Line 1 */
#endif
} adc_trigger_t;

/* Definitions for ADC channel number */
typedef enum _adc_channel_num_t {
    ADC_CHANNEL_0  = 0,
    ADC_CHANNEL_1  = 1,
    ADC_CHANNEL_2  = 2,
    ADC_CHANNEL_3  = 3,
    ADC_CHANNEL_4  = 4,
    ADC_CHANNEL_5  = 5,
    ADC_CHANNEL_6  = 6,
    ADC_CHANNEL_7  = 7,
#if SAM3S_SERIES ||  SAM3N_SERIES || SAM3XA_SERIES
    ADC_CHANNEL_8  = 8,
    ADC_CHANNEL_9  = 9,
    ADC_CHANNEL_10 = 10,
    ADC_CHANNEL_11 = 11,
    ADC_CHANNEL_12 = 12,
    ADC_CHANNEL_13 = 13,
    ADC_CHANNEL_14 = 14,
    ADC_TEMPERATURE_SENSOR = 15,
    ADC_ALL_CHANNEL = 0xffff
#elif SAM3U_SERIES
    ADC_ALL_CHANNEL = 0xff
#endif /* SAM3U_SERIES */
} adc_channel_num_t;

typedef enum _adc_gainvalue_t {
    ADC_GAINVALUE_0 = 0,
    ADC_GAINVALUE_1 = 1,
    ADC_GAINVALUE_2 = 2,
    ADC_GAINVALUE_3 = 3
} adc_gainvalue_t ;

#if SAM3S_SERIES ||  SAM3XA_SERIES
typedef enum {
    ADC_SETTLING_TIME_0 = ADC_MR_SETTLING_AST3,
    ADC_SETTLING_TIME_1 = ADC_MR_SETTLING_AST5,
    ADC_SETTLING_TIME_2 = ADC_MR_SETTLING_AST9,
    ADC_SETTLING_TIME_3 = ADC_MR_SETTLING_AST17
}adc_settling_time_t;
#endif

#if SAM3S_SERIES ||  SAM3N_SERIES || SAM3XA_SERIES

#include "adc_sam3snxa.h"

#elif SAM3U_SERIES

#include "adc10_sam3u.h"
#include "adc12_sam3u.h"

#endif

#endif /* #ifndef ADC_H_INCLUDED */
