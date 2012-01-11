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
 


#ifndef ADC12_H_INCLUDED
#define ADC12_H_INCLUDED

#include "../chip.h"
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
extern uint32_t adc12_init(Adc12b *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint32_t ul_startuptime, uint32_t ul_offmode_startuptime);
/**
 * \brief Configures conversion resolution.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param resolution  ADC resolution.
 */
extern void adc12_set_resolution(Adc12b *p_adc, adc_resolution_t resolution);
/**
 * \brief Configures conversion trigger and free run mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param trigger  Conversion trigger.
 */
extern void adc12_configure_trigger(Adc12b *p_adc, adc_trigger_t trigger);
/**
 * \brief Configures ADC power saving mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_sleep  ADC_MR_SLEEP_NORMAL keeps the ADC Core and reference voltage circuitry ON between conversions
 *                  ADC_MR_SLEEP_SLEEP keeps the ADC Core and reference voltage circuitry OFF between conversions
 * \param uc_offmode  0 Standby Mode (if Sleep Bit = 1)
 *                             1 Off Mode
 */
extern void adc12_configure_power_save(Adc12b *p_adc, uint8_t uc_sleep, uint8_t uc_offmode);
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_sh  ADC sample and hold time = uc_sh / ADC clock.
 */
extern void adc12_configure_timing(Adc12b *p_adc, uint32_t ul_sh);
/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
extern void adc12_start(Adc12b *p_adc);
/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 */
extern void adc12_stop(Adc12b *p_adc);
/**
 * \brief Enables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
extern void adc12_enable_channel(Adc12b *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Disables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
extern void adc12_disable_channel(Adc12b *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the ADC channel status.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval 1 means the specified channel is enabled.
 *         0 means the specified channel is disabled.
 */
extern uint32_t adc12_get_channel_status(Adc12b *p_adc, adc_channel_num_t adc_ch);

/**
 * \brief Reads the ADC status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC status register content.
 */
extern uint32_t adc12_get_status(Adc12b *p_adc);

/**
 * \brief Reads the ADC result data of the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval ADC data of the specified channel.
 */
extern uint32_t adc12_get_value(Adc12b *p_adc, adc_channel_num_t adc_ch);
/**
 * \brief Reads the last ADC result data.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC data.
 */
extern uint32_t adc12_get_latest_value(Adc12b *p_adc);

/**
 * \brief Enables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
extern void adc12_enable_differential_input(Adc12b *p_adc);

/**
 * \brief Disables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc12_disable_differential_input(Adc12b *p_adc);

/**
 * \brief Enables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc12_enable_input_offset(Adc12b *p_adc);

/**
 * \brief Disables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
extern void adc12_disable_input_offset(Adc12b *p_adc);

/**
 * \brief Configures input gain for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param gain  Gain value for the input.
 *
 */
extern void adc12_set_input_gain(Adc12b *p_adc, adc_gainvalue_t uc_gain);
/**
 * \brief Returns the actual ADC clock.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_mck Main clock of the device (value in Hz).
 *
 * \retval 0  The actual ADC clock (value in Hz).
 */
extern uint32_t adc12_get_actual_adc_clock(Adc12b *p_adc, uint32_t ul_mck);

/**
 * \brief Enables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be enabled.
 *
 */
extern void adc12_enable_interrupt(Adc12b *p_adc, uint32_t ul_source);

/**
 * \brief Disables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be disabled.
 */
extern void adc12_disable_interrupt(Adc12b *p_adc, uint32_t ul_source);

/** \brief Read ADC interrupt mask.
 *
 *  \param p_uart pointer to a UART instance.
 *
 *  \return The interrupt mask value.
 */
extern uint32_t adc12_get_interrupt_mask(Adc12b *p_adc);
/**
 * \brief Reads ADC interrupt status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC interrupt status.
 */
extern uint32_t adc12_get_interrupt_status(Adc12b *p_adc);
/**
 * \brief Reads overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status.
 */
extern uint32_t adc12_check_ovr(Adc12b *p_adc,adc_channel_num_t adc_ch);
/**
 * \brief Adapts performance versus power consumption.
 *
 * \note Please refer to ADC Characteristics in the product datasheet for the details.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ibctl  ADC Bias current control.
 *
 */
extern void adc12_set_bias_current(Adc12b *p_adc, uint8_t uc_ibctl);
/**
 * \brief Gets PDC registers base address.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval PDC registers base for PDC driver to access.
 */
extern Pdc *adc12_get_pdc_base(Adc12b *p_adc);

#endif // ADC12_H_INCLUDED
