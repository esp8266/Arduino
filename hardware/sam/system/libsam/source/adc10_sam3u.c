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
uint32_t adc_init(Adc *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint32_t ul_startuptime)
{
    p_adc->ADC_CR = ADC_CR_SWRST;
	
    /* Reset Mode Register */
    p_adc->ADC_MR = 0;	
	
     /* Reset PDC transfer */
    p_adc->ADC_PTCR = (ADC_PTCR_RXTDIS | ADC_PTCR_TXTDIS);
    p_adc->ADC_RCR = 0;
    p_adc->ADC_RNCR = 0;
    p_adc->ADC_TCR = 0;
    p_adc->ADC_TNCR = 0;		
    uint32_t prescal = ul_mck/(2 * ul_adc_clock) - 1;
    // check for rounding errors
    if ( (ul_mck/((prescal+1)*2)) > ul_adc_clock ) {
    	prescal++;
    	ul_adc_clock = ul_mck/((prescal+1)*2);
    }
    uint32_t startup = ((ul_adc_clock/1000000) * ul_startuptime / 8) - 1;
    p_adc->ADC_MR |= ADC_MR_PRESCAL(prescal) | ADC_MR_STARTUP(startup);
    return 0;	
}

/**
 * \brief Configures conversion resolution.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param resolution  ADC resolution.
 */
void adc_set_resolution(Adc *p_adc, adc_resolution_t resolution)
{
    p_adc->ADC_MR |= (resolution<<4) & ADC_MR_LOWRES;
}

/**
 * \brief Configures conversion trigger and free run mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param trigger  Conversion trigger.
 */
void adc_configure_trigger(Adc *p_adc, adc_trigger_t trigger)
{
    p_adc->ADC_MR |=  trigger;
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
void adc_configure_power_save(Adc *p_adc, uint8_t uc_sleep, uint8_t uc_offmode)
{
    p_adc->ADC_MR |= ((uc_sleep<<5) & ADC_MR_SLEEP) ;
}

/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_sh  ADC sample and hold time = uc_sh / ADC clock.
 */
void adc_configure_timing(Adc *p_adc, uint32_t ul_sh)
{
     p_adc->ADC_MR |= ADC_MR_SHTIM( ul_sh ) ;
}

/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 */
void adc_start(Adc *p_adc)
{
    p_adc->ADC_CR = ADC_CR_START;
}

/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 */
void adc_stop(Adc *p_adc)
{
    p_adc->ADC_CR = ADC_CR_SWRST;
}
/**
 * \brief Enables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
void adc_enable_channel(Adc *p_adc, adc_channel_num_t adc_ch)
{
    p_adc->ADC_CHER = 1 << adc_ch;
}

/**
 * \brief Disables the specified ADC channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 */
void adc_disable_channel(Adc *p_adc, adc_channel_num_t adc_ch)
{
    p_adc->ADC_CHDR = 1 << adc_ch;
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
uint32_t adc_get_channel_status(Adc *p_adc, adc_channel_num_t adc_ch)
{
    return p_adc->ADC_CHSR & (1 << adc_ch);
}

/**
 * \brief Reads the ADC status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC status register content.
 */
uint32_t adc_get_status(Adc *p_adc)
{
    return p_adc->ADC_SR;
}

/**
 * \brief Reads the ADC result data of the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param adc_ch  ADC channel number.
 *
 * \retval ADC data of the specified channel.
 */
uint32_t adc_get_value(Adc *p_adc, adc_channel_num_t adc_ch)
{
    uint32_t dwData = 0;

    if ( 15 >= adc_ch )
    {
        dwData=*(p_adc->ADC_CDR+adc_ch) ;
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
uint32_t adc_get_latest_value(Adc *p_adc)
{
    return p_adc->ADC_LCDR;
}

/**
 * \brief Returns the actual ADC clock.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ul_mck  Main clock of the device (value in Hz).
 *
 * \retval 0  The actual ADC clock (value in Hz).
 */
uint32_t adc_get_actual_adc_clock(Adc *p_adc, uint32_t  ul_mck)
{
    uint32_t ul_adcfreq;
    uint32_t ul_prescal;
   
    /* ADCClock = MCK / ( (PRESCAL+1) * 2 ) */
    ul_prescal = (( p_adc->ADC_MR & ADC_MR_PRESCAL_Msk) >> ADC_MR_PRESCAL_Pos);
    ul_adcfreq = ul_mck / ((ul_prescal+1)*2); 
    return ul_adcfreq;	
}

/**
 * \brief Enables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be enabled.
 */
void adc_enable_interrupt(Adc *p_adc, uint32_t ul_source)
{
    p_adc->ADC_IER = ul_source;
}

/**
 * \brief Disables ADC interrupt(s).
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_source  Interrupt(s) to be disabled.
 */
void adc_disable_interrupt(Adc *p_adc, uint32_t ul_source)
{
    p_adc->ADC_IDR = ul_source;
}

/**
 * \brief Reads ADC interrupt mask.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC interrupt status.
 */
 uint32_t adc_get_interrupt_status(Adc *p_adc)
{
    return p_adc->ADC_SR ;
}

/** \brief Read ADC interrupt mask.
 *
 *  \param p_uart pointer to a UART instance.
 *
 *  \return The interrupt mask value.
 */
uint32_t adc_get_interrupt_mask(Adc *p_adc)
{
	return p_adc->ADC_IMR;
}

/**
 * \brief Reads overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status.
 */
uint32_t adc_check_ovr(Adc *p_adc,adc_channel_num_t adc_ch)
{
    return p_adc->ADC_SR & (0x01u << (adc_ch+8));
}

/**
 * \brief Gets PDC registers base address.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval PDC registers base for PDC driver to access.
 */
Pdc *adc_get_pdc_base(Adc *p_adc)
{
    return PDC_ADC;
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif /* __cplusplus */
/**INDENT-ON**/
/// @endcond

#endif /* SAM3U */
