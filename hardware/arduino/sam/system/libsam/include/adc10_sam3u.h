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
 
#ifndef ADC10_SAM3U_H_INCLUDED
#define ADC10_SAM3U_H_INCLUDED

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond


#if SAM3U_SERIES

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
extern uint32_t adc_init(Adc *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint32_t ul_startuptime);

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
 * \brief Configures conversion sequence.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ch_list  Channel sequence list.
 * \param number  Number of channels in the list.
 *
 */
extern void adc_configure_sequence(Adc *p_adc, adc_channel_num_t ch_list[], uint8_t uc_num);

/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_sh  ADC sample and hold time = uc_sh / ADC clock.
 *
 */
extern void adc_configure_timing(Adc *p_adc, uint32_t ul_sh);

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

#endif /* SAM3U_SERIES */

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

#endif /* #ifndef ADC10_SAM3U_H_INCLUDED */
