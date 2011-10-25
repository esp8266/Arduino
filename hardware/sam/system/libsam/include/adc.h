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

#if SAM3U
#define ADC_12B
#endif

#define ADC_FREQ_MAX   20000000
#define ADC_FREQ_MIN    1000000

#define ADC_STARTUP_NORM     40
#define ADC_STARTUP_FAST     12

/* Definitions for ADC resolution */
#if SAM3S || SAM3XA
typedef enum _adc_resolution_t {
    ADC_10_BITS = ADC_MR_LOWRES_BITS_10,  /* ADC 10-bit resolution */
    ADC_12_BITS = ADC_MR_LOWRES_BITS_12  /* ADC 12-bit resolution */
} adc_resolution_t;
#elif SAM3N
typedef enum _adc_resolution_t {
    ADC_8_BITS = ADC_MR_LOWRES_BITS_8,  /* ADC 8-bit resolution */
    ADC_10_BITS = ADC_MR_LOWRES_BITS_10  /* ADC 10-bit resolution */
} adc_resolution_t;
#elif SAM3U
#ifdef ADC_12B
typedef enum _adc_resolution_t {
    ADC_10_BITS = ADC12B_MR_LOWRES_BITS_10,  /* ADC 10-bit resolution */
    ADC_12_BITS = ADC12B_MR_LOWRES_BITS_12  /* ADC 12-bit resolution */
} adc_resolution_t;
#else
typedef enum _adc_resolution_t {
    ADC_8_BITS = ADC_MR_LOWRES_BITS_8,  /* ADC 8-bit resolution */
    ADC_10_BITS = ADC_MR_LOWRES_BITS_10  /* ADC 10-bit resolution */
} adc_resolution_t;
#endif

#endif
/* Definitions for ADC trigger */
typedef enum _adc_trigger_t {
    ADC_TRIG_SW               = ADC_MR_TRGEN_DIS,  /* Starting a conversion is only possible by software. */
    ADC_TRIG_EXT              = ((ADC_MR_TRGSEL_ADC_TRIG0 <<ADC_MR_TRGSEL_Pos) & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* External trigger */
    ADC_TRIG_TIO_CH_0         = (ADC_MR_TRGSEL_ADC_TRIG1 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 0 */
    ADC_TRIG_TIO_CH_1         = (ADC_MR_TRGSEL_ADC_TRIG2 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 1 */
    ADC_TRIG_TIO_CH_2         = (ADC_MR_TRGSEL_ADC_TRIG3 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 2 */
#if SAM3S || SAM3XA || SAM3U
    ADC_TRIG_PWM_EVENT_LINE_0 = (ADC_MR_TRGSEL_ADC_TRIG4 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* PWM Event Line 0 */
    ADC_TRIG_PWM_EVENT_LINE_1 = (ADC_MR_TRGSEL_ADC_TRIG5 & ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN  /* PWM Event Line 1 */
#endif
} adc_trigger_t;

#if SAM3S ||  SAM3N || SAM3XA
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
    ADC_CHANNEL_8  = 8,
    ADC_CHANNEL_9  = 9,
    ADC_CHANNEL_10 = 10,
    ADC_CHANNEL_11 = 11,
    ADC_CHANNEL_12 = 12,
    ADC_CHANNEL_13 = 13,
    ADC_CHANNEL_14 = 14,
    ADC_TEMPERATURE_SENSOR = 15,
    ADC_ALL_CHANNEL
} adc_channel_num_t;
#elif SAM3U
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
    ADC_ALL_CHANNEL
} adc_channel_num_t;
#endif

typedef enum{
    ADC_GAINVALUE_0 = 0,
    ADC_GAINVALUE_1 = 1,
    ADC_GAINVALUE_2 = 2,
    ADC_GAINVALUE_3 = 3
}adc_gainvalue_t;

#if SAM3S ||  SAM3XA
typedef enum{
    ADC_SETTLING_TIME_0 = ADC_MR_SETTLING_AST3,
    ADC_SETTLING_TIME_1 = ADC_MR_SETTLING_AST5,
    ADC_SETTLING_TIME_2 = ADC_MR_SETTLING_AST9,
    ADC_SETTLING_TIME_3 = ADC_MR_SETTLING_AST17
}adc_settling_time_t;
#endif
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Initializes the given ADC with the specified ADC clock and startup time.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_mck  Main clock of the device (value in Hz).
 * \param dw_adc_clock  Analog-to-Digital conversion clock (value in Hz).
 * \param uc_startup  ADC start up time. Please refer to the product datasheet for details.
 *
 * \retval 0  The initialization operation succeeds.
 * \retval others  The initialization operation fails.
 */
extern uint32_t adc_init(Adc *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint8_t uc_startup);

#endif

 #if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Configures conversion resolution.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param resolution  ADC resolution.
 *
 */
extern void adc_set_resolution(Adc *p_adc, adc_resolution_t resolution);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Configures conversion trigger and free run mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param trigger  Conversion trigger.
 * \param uc_freerun  ADC_MR_FREERUN_ON enables freerun mode
 *                    ADC_MR_FREERUN_OFF disables freerun mode
 *
 */
extern void adc_configure_trigger(Adc *p_adc, adc_trigger_t trigger, uint8_t uc_freerun);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Configures ADC power saving mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_sleep  ADC_MR_SLEEP_NORMAL keeps the ADC Core and reference voltage circuitry ON between conversions
 *                  ADC_MR_SLEEP_SLEEP keeps the ADC Core and reference voltage circuitry OFF between conversions
 * \param uc_fwup  ADC_MR_FWUP_OFF configures sleep mode as uc_sleep setting
 *                 ADC_MR_FWUP_ON keeps voltage reference ON and ADC Core OFF between conversions
 *
 */
extern void adc_configure_power_save(Adc *p_adc, uint8_t uc_sleep, uint8_t uc_fwup);
/**
 * \brief Configures conversion sequence.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ch_list  Channel sequence list.
 * \param number  Number of channels in the list.
 *
 */
extern void adc_configure_sequence(Adc *p_adc, adc_channel_num_t ch_list[], uint8_t uc_num);
#endif

#if SAM3S ||  SAM3XA
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_tracking  ADC tracking time = uc_tracking / ADC clock.
 * \param uc_settling  Analog settling time = (uc_settling + 1) / ADC clock.
 * \param uc_transfer  Data transfer time = (uc_transfer * 2 + 3) / ADC clock.
 *
 */
extern void adc_configure_timing(Adc *p_adc, uint8_t uc_tracking, adc_settling_time_t settling, uint8_t uc_transfer);
#elif SAM3N
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_tracking  ADC tracking time = uc_tracking / ADC clock.
 *
 */
extern void adc_configure_timing(Adc *p_adc, uint8_t uc_tracking);

#endif

#if SAM3S || SAM3XA
/**
 * \brief enable analog change.
 *
 * note  it allows different analog settings for each channel,
 *
 * \param pAdc   Pointer to an Adc instance.
 */
extern void adc_enable_anch( Adc *pAdc );
#endif

#if SAM3S || SAM3XA
/**
 * \brief disable analog change.
 *
 * note DIFF0, GAIN0 and OFF0 are used for all channels.
 *
 * \param pAdc   Pointer to an Adc instance.
 */
extern void adc_disable_anch( Adc *pAdc );
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_start(Adc *p_adc);
/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_stop(Adc *p_adc);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Enables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 */
extern void adc_enable_channel(Adc *p_adc, adc_channel_num_t adc_ch);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Disables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 */
extern void adc_disable_channel(Adc *p_adc, adc_channel_num_t adc_ch);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Reads the ADC channel status.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval 1 means the specified channel is enabled.
 *         0 means the specified channel is disabled.
 */
extern uint32_t adc_get_status(Adc *p_adc, adc_channel_num_t adc_ch);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Reads the ADC result data of the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval ADC data of the specified channel.
 */
extern uint32_t adc_get_value(Adc *p_adc, adc_channel_num_t adc_ch);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Reads the last ADC result data.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC data.
 */
extern uint32_t adc_get_latest_value(Adc *p_adc);
#endif

#if SAM3S || SAM3N || SAM3XA
/**
 * \brief Enables TAG option so that the number of the last converted channel can be indicated.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_enable_tag(Adc *p_adc);
#endif

#if SAM3S || SAM3N || SAM3XA
/**
 * \brief Disables TAG option.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_disable_tag(Adc *p_adc);
#endif

#if SAM3S || SAM3N || SAM3XA
/**
 * \brief Indicates the last converted channel.
 *
 * \note If TAG option is NOT enabled before, an incorrect channel number is returned.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval The last converted channel number.
 */
extern adc_channel_num_t adc_get_tag(Adc *p_adc);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Enables conversion sequencer.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_start_sequencer(Adc *p_adc);

/**
 * \brief Disables conversion sequencer.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_stop_sequencer(Adc *p_adc);

/**
 * \brief Configures comparsion mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param mode  ADC comparsion mode.
 *
 */
extern void adc_set_comparison_mode(Adc *p_adc, uint8_t uc_mode);

/**
 * \brief get comparsion mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param mode  ADC comparsion mode.
 *
 * \retval compare mode value.
 */
extern uint32_t adc_get_comparison_mode(Adc *p_adc);

/**
 * \brief Configures ADC compare window.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param w_low_threshold  Low threshold of compare window.
 * \param w_high_threshold  High threshold of compare window.
 *
 */
extern void adc_set_comparsion_window(Adc *p_adc, uint16_t us_low_threshold, uint16_t us_high_threshold);

/**
 * \brief Configures comparison selected channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Comparison selected channel.
 *
 */
extern void adc_set_comparison_channel(Adc *p_adc, adc_channel_num_t channel);
#endif

#if SAM3S || SAM3XA
/**
 * \brief Enables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
extern void adc_enable_differential_input(Adc *p_adc, adc_channel_num_t channel);

/**
 * \brief Disables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
extern void adc_disable_differential_input(Adc *p_adc, adc_channel_num_t channel);

/**
 * \brief Enables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
extern void adc_enable_input_offset(Adc *p_adc, adc_channel_num_t channel);

/**
 * \brief Disables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
extern void adc_disable_input_offset(Adc *p_adc, adc_channel_num_t channel);

/**
 * \brief Configures input gain for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 * \param gain  Gain value for the input.
 *
 */
extern void adc_set_input_gain(Adc *p_adc, adc_channel_num_t channel, adc_gainvalue_t uc_gain);
#endif

#if  SAM3S8 || SAM3SD8
/**
 * \brief set adc auto calibration mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_set_calibmode(Adc *p_adc);
#endif

 #if SAM3S || SAM3N || SAM3XA
/**
 * \brief Returns the actual ADC clock.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_mck Main clock of the device (value in Hz).
 *
 * \retval 0  The actual ADC clock (value in Hz).
 */
extern uint32_t adc_get_actual_adc_clock(Adc *p_adc, uint32_t ul_mck);
#endif

 #if SAM3S || SAM3N || SAM3XA
/**
 * \brief Enables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be enabled.
 *
 */
extern void adc_enable_interrupt(Adc *p_adc, uint32_t ul_source);

/**
 * \brief Disables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be disabled.
 *
 */
extern void adc_disable_interrupt(Adc *p_adc, uint32_t ul_source);

/**
 * \brief Reads ADC interrupt status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC interrupt status.
 */
extern uint32_t adc_get_interrupt_status(Adc *p_adc);

/** \brief Read ADC interrupt mask.
 *
 *  \param p_uart pointer to a UART instance.
 *
 *  \return The interrupt mask value.
 */
extern uint32_t adc_get_interrupt_mask(Adc *p_adc);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Reads overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status.
 */
extern uint32_t adc_check_ovr(Adc *p_adc,adc_channel_num_t adc_ch);
#endif

#if SAM3S || SAM3XA
/**
 * \brief Adapts performance versus power consumption.
 *
 * \note Please refer to ADC Characteristics in the product datasheet for the details.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ibctl  ADC Bias current control.
 *
 */
extern void adc_set_bias_current(Adc *p_adc, uint8_t uc_ibctl);
#endif

#if SAM3S ||  SAM3XA
/**
 * \brief turn on temperature sensor.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_enable_ts(Adc *p_adc);

/**
 * \brief turn off temperature sensor.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_disable_ts(Adc *p_adc);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Enables or disables write protection of ADC registers.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_eanble  1 to eanble, 0 to disable.
 */
extern void adc_set_writeprotect(Adc *p_adc, uint32_t ul_enable);

/**
 * \brief Indicates write protect status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \return 0 if the peripheral is not protected, or 16-bit Write Protect Violation Status.
 */
extern uint32_t adc_get_writeprotect_status(Adc *p_adc);


/**
 * \brief Checks ADC configurations.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_mck  Main clock of the device (value in Hz).
 */
extern void adc_check(Adc* p_adc, uint32_t ul_mck);
#endif

#if SAM3S ||  SAM3N || SAM3XA
/**
 * \brief Gets PDC registers base address.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval PDC registers base for PDC driver to access.
 */
extern Pdc *adc_get_pdc_base(Adc *p_adc);
#endif

#if SAM3U

#ifdef ADC_12B
/**
 * \brief Initializes the given ADC with the specified ADC clock and startup time.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_mck  Main clock of the device (value in Hz).
 * \param dw_adc_clock  Analog-to-Digital conversion clock (value in Hz).
 * \param uc_startup  ADC start up time value(value in us). Please refer to the product datasheet for details.
 * \param ul_offmode_startuptime  ADC off mode startup Time value(value in us). Please refer to the product datasheet for details.
 *
 * \retval 0  The initialization operation succeeds.
 * \retval others  The initialization operation fails.
 */
extern uint32_t adc_init(Adc12b *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint32_t ul_startuptime, uint32_t ul_offmode_startuptime);
/**
 * \brief Configures conversion resolution.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param resolution  ADC resolution.
 */
extern void adc_set_resolution(Adc12b *p_adc, adc_resolution_t resolution);
/**
 * \brief Configures conversion trigger and free run mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param trigger  Conversion trigger.
 */
extern void adc_configure_trigger(Adc12b *p_adc, adc_trigger_t trigger);
/**
 * \brief Configures ADC power saving mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_sleep  ADC_MR_SLEEP_NORMAL keeps the ADC Core and reference voltage circuitry ON between conversions
 *                  ADC_MR_SLEEP_SLEEP keeps the ADC Core and reference voltage circuitry OFF between conversions
 * \param uc_offmode  0 Standby Mode (if Sleep Bit = 1)
 *                             1 Off Mode
 */
extern void adc_configure_power_save(Adc12b *p_adc, uint8_t uc_sleep, uint8_t uc_offmode);
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_sh  ADC sample and hold time = uc_sh / ADC clock.
 */
extern void adc_configure_timing(Adc12b *p_adc, uint32_t ul_sh);
/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
extern void adc_start(Adc12b *p_adc);
/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 */
extern void adc_stop(Adc12b *p_adc);
/**
 * \brief Enables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
extern void adc_enable_channel(Adc12b *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Disables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
extern void adc_disable_channel(Adc12b *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the ADC channel status.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval 1 means the specified channel is enabled.
 *         0 means the specified channel is disabled.
 */
extern uint32_t adc_get_status(Adc12b *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the ADC result data of the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval ADC data of the specified channel.
 */
extern uint32_t adc_get_value(Adc12b *p_adc, adc_channel_num_t adc_ch);
/**
 * \brief Reads the last ADC result data.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC data.
 */
extern uint32_t adc_get_latest_value(Adc12b *p_adc);

/**
 * \brief Enables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
extern void adc_enable_differential_input(Adc12b *p_adc);

/**
 * \brief Disables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_disable_differential_input(Adc12b *p_adc);

/**
 * \brief Enables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_enable_input_offset(Adc12b *p_adc);

/**
 * \brief Disables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_disable_input_offset(Adc12b *p_adc);

/**
 * \brief Configures input gain for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param gain  Gain value for the input.
 *
 */
extern void adc_set_input_gain(Adc12b *p_adc, adc_gainvalue_t uc_gain);
/**
 * \brief Returns the actual ADC clock.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_mck Main clock of the device (value in Hz).
 *
 * \retval 0  The actual ADC clock (value in Hz).
 */
extern uint32_t adc_get_actual_adc_clock(Adc12b *p_adc, uint32_t ul_mck);

/**
 * \brief Enables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be enabled.
 *
 */
extern void adc_enable_interrupt(Adc12b *p_adc, uint32_t ul_source);

/**
 * \brief Disables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be disabled.
 */
extern void adc_disable_interrupt(Adc12b *p_adc, uint32_t ul_source);

/** \brief Read ADC interrupt mask.
 *
 *  \param p_uart pointer to a UART instance.
 *
 *  \return The interrupt mask value.
 */
extern uint32_t adc_get_interrupt_mask(Adc12b *p_adc);
/**
 * \brief Reads ADC interrupt status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC interrupt status.
 */
extern uint32_t adc_get_interrupt_status(Adc12b *p_adc);
/**
 * \brief Reads overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status.
 */
extern uint32_t adc_check_ovr(Adc12b *p_adc,adc_channel_num_t adc_ch);
/**
 * \brief Adapts performance versus power consumption.
 *
 * \note Please refer to ADC Characteristics in the product datasheet for the details.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ibctl  ADC Bias current control.
 *
 */
extern void adc_set_bias_current(Adc12b *p_adc, uint8_t uc_ibctl);
/**
 * \brief Gets PDC registers base address.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval PDC registers base for PDC driver to access.
 */
extern Pdc *adc_get_pdc_base(Adc12b *p_adc);
#else
/**
 * \brief Initializes the given ADC with the specified ADC clock and startup time.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_mck  Main clock of the device (value in Hz).
 * \param dw_adc_clock  Analog-to-Digital conversion clock (value in Hz).
 * \param uc_startup  ADC start up time value(value in us). Please refer to the product datasheet for details.
 * \param ul_offmode_startuptime  ADC off mode startup Time value(value in us). Please refer to the product datasheet for details.
 *
 * \retval 0  The initialization operation succeeds.
 * \retval others  The initialization operation fails.
 */
extern uint32_t adc_init(Adc *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint32_t ul_startuptime, uint32_t ul_offmode_startuptime);

/**
 * \brief Configures conversion resolution.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param resolution  ADC resolution.
 *
 */
extern void adc_set_resolution(Adc *p_adc, adc_resolution_t resolution);
/**
 * \brief Configures conversion trigger and free run mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param trigger  Conversion trigger.
 *
 */
extern void adc_configure_trigger(Adc *p_adc, adc_trigger_t trigger);
/**
 * \brief Configures ADC power saving mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_sleep  ADC_MR_SLEEP_NORMAL keeps the ADC Core and reference voltage circuitry ON between conversions
 *                  ADC_MR_SLEEP_SLEEP keeps the ADC Core and reference voltage circuitry OFF between conversions
 * \param uc_offmode  0 Standby Mode (if Sleep Bit = 1)
 *                             1 Off Mode
 *
 */
extern void adc_configure_power_save(Adc *p_adc, uint8_t uc_sleep, uint8_t uc_offmode);
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_sh  ADC sample and hold time = uc_sh / ADC clock.
 *
 */
extern void adc_configure_timing(Adc *p_adc, uint32_t ul_sh);
/**
 * \brief enable analog change.
 *
 * note  it allows different analog settings for each channel,
 *
 * \param pAdc   Pointer to an Adc instance.
 */
extern void adc_enable_anch( Adc *pAdc );

/**
 * \brief disable analog change.
 *
 * note DIFF0, GAIN0 and OFF0 are used for all channels.
 *
 * \param pAdc   Pointer to an Adc instance.
 */
extern void adc_disable_anch( Adc *pAdc );

/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_start(Adc *p_adc);
/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_stop(Adc *p_adc);
/**
 * \brief Enables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 */
extern void adc_enable_channel(Adc *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Disables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 */
extern void adc_disable_channel(Adc *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the ADC channel status.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval 1 means the specified channel is enabled.
 *         0 means the specified channel is disabled.
 */
extern uint32_t adc_get_status(Adc *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the ADC result data of the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval ADC data of the specified channel.
 */
extern uint32_t adc_get_value(Adc *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the last ADC result data.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC data.
 */
extern uint32_t adc_get_latest_value(Adc *p_adc);

/**
 * \brief Returns the actual ADC clock.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_mck Main clock of the device (value in Hz).
 *
 * \retval 0  The actual ADC clock (value in Hz).
 */
extern uint32_t adc_get_actual_adc_clock(Adc *p_adc, uint32_t ul_mck);

/**
 * \brief Enables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be enabled.
 *
 */
extern void adc_enable_interrupt(Adc *p_adc, uint32_t ul_source);

/**
 * \brief Disables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be disabled.
 *
 */
extern void adc_disable_interrupt(Adc *p_adc, uint32_t ul_source);
/** \brief Read ADC interrupt mask.
 *
 *  \param p_uart pointer to a UART instance.
 *
 *  \return The interrupt mask value.
 */
extern uint32_t adc_get_interrupt_mask(Adc *p_adc);
/**
 * \brief Reads ADC interrupt status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC interrupt status.
 */
extern uint32_t adc_get_interrupt_status(Adc *p_adc);

/**
 * \brief Reads overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status.
 */
extern uint32_t adc_check_ovr(Adc *p_adc,adc_channel_num_t adc_ch);

/**
 * \brief Gets PDC registers base address.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval PDC registers base for PDC driver to access.
 */
extern Pdc *adc_get_pdc_base(Adc *p_adc);
#endif

#endif

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* #ifndef ADC_H_INCLUDED */
