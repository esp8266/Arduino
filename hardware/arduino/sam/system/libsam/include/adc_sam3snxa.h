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
 
#ifndef ADC_SAM3SNXA_H_INCLUDED
#define ADC_SAM3SNXA_H_INCLUDED

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#if SAM3S_SERIES || SAM3N_SERIES || SAM3XA_SERIES

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
uint32_t adc_init(Adc *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint8_t uc_startup) ;

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
 * \param uc_freerun  ADC_MR_FREERUN_ON enables freerun mode
 *                    ADC_MR_FREERUN_OFF disables freerun mode
 *
 */
extern void adc_configure_trigger(Adc *p_adc, adc_trigger_t trigger, uint8_t uc_freerun);

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

#if SAM3S_SERIES || SAM3XA_SERIES
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
#elif SAM3N_SERIES
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_tracking  ADC tracking time = uc_tracking / ADC clock.
 *
 */
extern void adc_configure_timing(Adc *p_adc, uint8_t uc_tracking);
#endif

#if SAM3S_SERIES || SAM3XA_SERIES
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
#endif

/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_start( Adc *p_adc ) ;

/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_stop( Adc *p_adc ) ;

/**
 * \brief Enables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 */
extern void adc_enable_channel( Adc *p_adc, adc_channel_num_t adc_ch ) ;

/**
 * \brief Disables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 */
extern void adc_disable_channel( Adc *p_adc, adc_channel_num_t adc_ch ) ;

/**
 * \brief Reads the ADC channel status.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval 1 means the specified channel is enabled.
 *         0 means the specified channel is disabled.
 */
extern uint32_t adc_get_channel_status(Adc *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the ADC status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC status register content.
 */
extern uint32_t adc_get_status(Adc *p_adc);

/**
 * \brief Reads the ADC overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC ovverrun status register content.
 */
extern uint32_t adc_get_overrun_status(Adc *p_adc);

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
 * \brief Enables TAG option so that the number of the last converted channel can be indicated.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_enable_tag(Adc *p_adc);

/**
 * \brief Disables TAG option.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_disable_tag(Adc *p_adc);

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

#if SAM3S_SERIES || SAM3XA_SERIES
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

#if  SAM3SD8_SERIES
/**
 * \brief set adc auto calibration mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc_set_calibmode(Adc *p_adc);
#endif

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

/**
 * \brief Reads overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status.
 */
extern uint32_t adc_check_ovr(Adc *p_adc, adc_channel_num_t adc_ch);

#if SAM3S_SERIES || SAM3XA_SERIES
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

#if SAM3S_SERIES ||  SAM3XA_SERIES
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

/**
 * \brief Gets PDC registers base address.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval PDC registers base for PDC driver to access.
 */
extern Pdc *adc_get_pdc_base(Adc *p_adc);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif /* __cplusplus */
/**INDENT-ON**/
/// @endcond

#endif /* SAM3S_SERIES || SAM3N_SERIES || SAM3XA_SERIES */

#endif /* #ifndef ADC_SAM3SNXA_H_INCLUDED */
