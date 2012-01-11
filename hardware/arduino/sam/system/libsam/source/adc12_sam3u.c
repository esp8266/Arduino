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

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**INDENT-ON**/
/// @endcond

#if SAM3U

/**
 * \brief Initializes the given ADC with the specified ADC clock and startup time.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_mck  Main clock of the device (value in Hz).
 * \param dw_adc_clock  Analog-to-Digital conversion clock (value in Hz).
 * \param ul_startuptime  ADC start up time value(value in us). Please refer to the product datasheet for details.
 * \param ul_offmode_startuptime  ADC off mode startup Time value(value in us). Please refer to the product datasheet for details.
 *
 * \retval 0  The initialization operation succeeds.
 * \retval others  The initialization operation fails.
 */
uint32_t adc12_init(Adc12b *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint32_t ul_startuptime, uint32_t ul_offmode_startuptime)
{
    p_adc->ADC12B_CR = ADC12B_CR_SWRST;	
	
     /* Reset Mode Register */
    p_adc->ADC12B_MR = 0;
	 
     /* Reset PDC transfer */
    p_adc->ADC12B_PTCR = (ADC12B_PTCR_RXTDIS | ADC12B_PTCR_TXTDIS);
    p_adc->ADC12B_RCR = 0;
    p_adc->ADC12B_RNCR = 0;
    p_adc->ADC12B_TCR = 0;
    p_adc->ADC12B_TNCR = 0;		
    uint32_t prescal = ul_mck/(2 * ul_adc_clock) - 1;
    // check for rounding errors
    if ( (ul_mck/((prescal+1)*2)) > ul_adc_clock ) {
    	prescal++;
    	ul_adc_clock = ul_mck/((prescal+1)*2);
    }
    uint32_t startup = ((ul_adc_clock/1000000) * ul_startuptime / 8) - 1;
    p_adc->ADC12B_MR |= ADC12B_MR_PRESCAL(prescal) | ADC12B_MR_STARTUP(startup);

    uint32_t offmode = ((ul_adc_clock/1000000) * ul_offmode_startuptime / 8) - 1;
    p_adc->ADC12B_EMR |= ADC12B_EMR_OFF_MODE_STARTUP_TIME(offmode);
    return 0;	
}
/**
 * \brief Configures conversion resolution.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param resolution  ADC resolution.
 */
void adc12_set_resolution(Adc12b *p_adc, adc_resolution_t resolution)
{
    p_adc->ADC12B_MR |= (resolution<<4) & ADC12B_MR_LOWRES;
}
/**
 * \brief Configures conversion trigger and free run mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param trigger  Conversion trigger.
 */
void adc12_configure_trigger(Adc12b *p_adc, adc_trigger_t trigger)
{
    p_adc->ADC12B_MR |=  trigger;
}
/**
 * \brief Configures ADC power saving mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_sleep  ADC_MR_SLEEP_NORMAL keeps the ADC Core and reference voltage circuitry ON between conversions
 *                  ADC_MR_SLEEP_SLEEP keeps the ADC Core and reference voltage circuitry OFF between conversions
 * \param uc_offmode  0 Standby Mode (if Sleep Bit = 1)
 *                             1 Off Mode
 */
void adc12_configure_power_save(Adc12b *p_adc, uint8_t uc_sleep, uint8_t uc_offmode)
{
    p_adc->ADC12B_MR |= ((uc_sleep<<5) & ADC12B_MR_SLEEP) ;
    p_adc->ADC12B_EMR |= uc_offmode;	
}
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_sh  ADC sample and hold time = uc_sh / ADC clock.
 *
 */
void adc12_configure_timing(Adc12b *p_adc, uint32_t ul_sh)
{
     p_adc->ADC12B_MR |= ADC12B_MR_SHTIM( ul_sh ) ;
}
/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
 
void adc12_start(Adc12b *p_adc)
{
    p_adc->ADC12B_CR = ADC12B_CR_START;
}
/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 */
void adc12_stop(Adc12b *p_adc)
{
    p_adc->ADC12B_CR = ADC12B_CR_SWRST;
}
/**
 * \brief Enables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
void adc12_enable_channel(Adc12b *p_adc, adc_channel_num_t adc_ch)
{
    p_adc->ADC12B_CHER = 1 << adc_ch;
}

/**
 * \brief Disables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
void adc12_disable_channel(Adc12b *p_adc, adc_channel_num_t adc_ch)
{
    p_adc->ADC12B_CHDR = 1 << adc_ch;
}

/**
 * \brief Reads the ADC channel status.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval 1 means the specified channel is enabled.
 *         0 means the specified channel is disabled.
 */
uint32_t adc12_get_channel_status(Adc12b *p_adc, adc_channel_num_t adc_ch)
{
    return p_adc->ADC12B_CHSR & (1 << adc_ch);
}

/**
 * \brief Reads the ADC status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC status register content.
 */
uint32_t adc12_get_status(Adc12b *p_adc)
{
    return p_adc->ADC12B_SR;
}

/**
 * \brief Reads the ADC result data of the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval ADC data of the specified channel.
 */
uint32_t adc12_get_value(Adc12b *p_adc, adc_channel_num_t adc_ch)
{
    uint32_t dwData = 0;

    if ( 15 >= adc_ch )
    {
        dwData=*(p_adc->ADC12B_CDR+adc_ch) ;
    }

    return dwData ;
}

/**
 * \brief Reads the last ADC result data.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC data.
 */
uint32_t adc12_get_latest_value(Adc12b *p_adc)
{
    return p_adc->ADC12B_LCDR;
}
/**
 * \brief Enables differential input for all channels.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
 void adc12_enable_differential_input(Adc12b *p_adc)
{
    p_adc->ADC12B_ACR |= (0x01u << 16);
}

/**
 * \brief Disables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
void adc12_disable_differential_input(Adc12b*p_adc)
{
    p_adc->ADC12B_ACR &= (0x01u << 16);
}

/**
 * \brief Enables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
void adc12_enable_input_offset(Adc12b *p_adc)
{
    p_adc->ADC12B_ACR |= (0x01u << 17);
}

/**
 * \brief Disables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
void adc12_disable_input_offset(Adc12b *p_adc)
{
    p_adc->ADC12B_ACR &= (0x01u << 17);
}
/**
 * \brief Configures input gain for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param gain  Gain value for the input.
 */
void adc12_set_input_gain(Adc12b *p_adc, adc_gainvalue_t gain)
{
    p_adc->ADC12B_ACR |= (0x03u & gain);  
}
uint32_t adc12_get_actual_adc_clock(Adc12b *p_adc, uint32_t  ul_mck)
{
    uint32_t ul_adcfreq;
    uint32_t ul_prescal;
   
    /* ADCClock = MCK / ( (PRESCAL+1) * 2 ) */
    ul_prescal = (( p_adc->ADC12B_MR & ADC12B_MR_PRESCAL_Msk) >> ADC12B_MR_PRESCAL_Pos);
    ul_adcfreq = ul_mck / ((ul_prescal+1)*2); 
    return ul_adcfreq;	
}
/**
 * \brief Enables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be enabled.
 */
void adc12_enable_interrupt(Adc12b *p_adc, uint32_t ul_source)
{
    p_adc->ADC12B_IER = ul_source;
}

/**
 * \brief Disables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be disabled.
 */
void adc12_disable_interrupt(Adc12b *p_adc, uint32_t ul_source)
{
    p_adc->ADC12B_IDR = ul_source;
}
/** \brief Read ADC interrupt mask.
 *
 *  \param p_uart pointer to a UART instance.
 *
 *  \return The interrupt mask value.
 */
uint32_t adc12_get_interrupt_mask(Adc12b *p_adc)
{
	return p_adc->ADC12B_IMR;
}
/**
 * \brief Reads ADC interrupt status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC interrupt status.
 */
uint32_t adc12_get_interrupt_status(Adc12b *p_adc)
{
    return p_adc->ADC12B_SR ;
}
/**
 * \brief Reads overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status.
 */
uint32_t adc12_check_ovr(Adc12b *p_adc,adc_channel_num_t adc_ch)
{
    return p_adc->ADC12B_SR & (0x01u << (adc_ch+8));
}
/**
 * \brief Adapts performance versus power consumption.
 *
 * \note Please refer to ADC Characteristics in the product datasheet for the details.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ibctl  ADC Bias current control.
 */
void adc12_set_bias_current(Adc12b *p_adc, uint8_t uc_ibctl)
{
     p_adc->ADC12B_ACR |= ADC12B_ACR_IBCTL(uc_ibctl);
}
/**
 * \brief Gets PDC registers base address.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval PDC registers base for PDC driver to access.
 */
Pdc *adc12_get_pdc_base(Adc12b *p_adc)
{
    return PDC_ADC12B;
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif /* __cplusplus */
/**INDENT-ON**/
/// @endcond

#endif /* SAM3U */
