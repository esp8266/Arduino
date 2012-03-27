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
uint32_t adc_init(Adc *p_adc, uint32_t ul_mck, uint32_t ul_adc_clock, uint8_t uc_startup)
{
    uint32_t ul_prescal;
	
    /*  Reset the controller */	
    p_adc->ADC_CR = ADC_CR_SWRST;
	
    /* Reset Mode Register */
    p_adc->ADC_MR = 0;
   
    /* Reset PDC transfer */
    p_adc->ADC_PTCR = (ADC_PTCR_RXTDIS | ADC_PTCR_TXTDIS);
    p_adc->ADC_RCR = 0;
    p_adc->ADC_RNCR = 0;
	
    ul_prescal = ul_mck/(2 * ul_adc_clock) - 1;
    p_adc->ADC_MR |= ADC_MR_PRESCAL( ul_prescal ) | ( (uc_startup<<ADC_MR_STARTUP_Pos) & ADC_MR_STARTUP_Msk);
    return 0;	
}

/**
 * \brief Configures conversion resolution.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param resolution  ADC resolution.
 *
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
 * \param uc_freerun  ADC_MR_FREERUN_ON enables freerun mode
 *                    ADC_MR_FREERUN_OFF disables freerun mode
 *
 */
void adc_configure_trigger(Adc *p_adc, adc_trigger_t trigger, uint8_t uc_freerun)
{
    p_adc->ADC_MR |=  trigger | ((uc_freerun<<7) & ADC_MR_FREERUN);
}

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
void adc_configure_power_save(Adc *p_adc, uint8_t uc_sleep, uint8_t uc_fwup)
{
    p_adc->ADC_MR |= ( ((uc_sleep<<5) & ADC_MR_SLEEP) | ((uc_fwup<<6) & ADC_MR_FWUP) );
}

/**
 * \brief Configures conversion sequence.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param ch_list  Channel sequence list.
 * \param number  Number of channels in the list.
 *
 */
void adc_configure_sequence(Adc *p_adc, adc_channel_num_t ch_list[], uint8_t uc_num)
{
    uint8_t uc_counter;
    if(uc_num < 8) 	
    {
        for(uc_counter=0;uc_counter < uc_num;uc_counter++)
	  {
	        p_adc->ADC_SEQR1 |= ch_list[uc_counter] << (4*uc_counter);
	  }
    }
    else
    {
      for(uc_counter=0;uc_counter < 8;uc_counter++)
	  {
	        p_adc->ADC_SEQR1 |= ch_list[uc_counter] << (4*uc_counter);
	  }
	  for(uc_counter=0;uc_counter < uc_num-8;uc_counter++)
	  {
	        p_adc->ADC_SEQR2 |= ch_list[uc_counter] << (4*uc_counter);
	  }	
    }
}

#if SAM3S_SERIES ||  SAM3XA_SERIES
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_tracking  ADC tracking time = uc_tracking / ADC clock.
 * \param uc_settling  Analog settling time = (uc_settling + 1) / ADC clock.
 * \param uc_transfer  Data transfer time = (uc_transfer * 2 + 3) / ADC clock.
 *
 */
void adc_configure_timing(Adc *p_adc, uint8_t uc_tracking, adc_settling_time_t settling, uint8_t uc_transfer)
{
     p_adc->ADC_MR |= ADC_MR_TRANSFER( uc_transfer )
                 | settling
                 | ADC_MR_TRACKTIM( uc_tracking ) ;
}
#elif  SAM3N_SERIES 
/**
 * \brief Configures ADC timing.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param uc_tracking  ADC tracking time = uc_tracking / ADC clock.
 *
 */
void adc_configure_timing(Adc *p_adc, uint8_t uc_tracking)
{
     p_adc->ADC_MR |= ADC_MR_TRACKTIM( uc_tracking ) ;
}
#endif

#if SAM3S_SERIES || SAM3XA_SERIES
/**
 * \brief enable analog change.
 *
 * note  it allows different analog settings for each channel,
 *
 * \param pAdc   Pointer to an Adc instance.
 */
void adc_enable_anch( Adc *pAdc )
{
    pAdc->ADC_MR |=  ADC_MR_ANACH;
}
#endif

#if SAM3S_SERIES || SAM3XA_SERIES
/**
 * \brief disable analog change.
 * 
 * note DIFF0, GAIN0 and OFF0 are used for all channels.
 *
 * \param pAdc   Pointer to an Adc instance.
 */
void adc_disable_anch( Adc *pAdc )
{
    pAdc->ADC_MR &= ~ADC_MR_ANACH;
}
#endif

/**
 * \brief Starts analog-to-digital conversion.
 *
 * \note  If one of the hardware event is selected as ADC trigger, this function can NOT start analog to digital conversion.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
 
void adc_start(Adc *p_adc)
{
    p_adc->ADC_CR = ADC_CR_START;
}

/**
 * \brief Stop analog-to-digital conversion.
 * \param p_adc  Pointer to an ADC instance.
 *
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
 *
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
 *
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
    return p_adc->ADC_ISR;
}

/**
 * \brief Reads the ADC overrun status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC overrun status register content.
 */
uint32_t adc_get_overrun_status(Adc *p_adc)
{
    return p_adc->ADC_OVER;
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
 * \brief Enables TAG option so that the number of the last converted channel can be indicated.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
void adc_enable_tag(Adc *p_adc)
{
    p_adc->ADC_EMR |= ADC_EMR_TAG;
}

/**
 * \brief Disables TAG option.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
void adc_disable_tag(Adc *p_adc)
{
    p_adc->ADC_EMR &= ~ADC_EMR_TAG;
}

/**
 * \brief Indicates the last converted channel.
 *
 * \note If TAG option is NOT enabled before, an incorrect channel number is returned.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval The last converted channel number.
 */
adc_channel_num_t adc_get_tag(Adc *p_adc)
{
    return  (p_adc->ADC_LCDR & ADC_LCDR_CHNB_Msk) >> ADC_LCDR_CHNB_Pos;
}

/**
 * \brief Enables conversion sequencer.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
void adc_start_sequencer(Adc *p_adc)
{
    p_adc->ADC_MR |= ADC_MR_USEQ;
}

/**
 * \brief Disables conversion sequencer.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
void adc_stop_sequencer(Adc *p_adc)
{
    p_adc->ADC_MR &= ~ADC_MR_USEQ; 
}

/**
 * \brief Configures comparsion mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param mode  ADC comparsion mode.
 *
 */
void adc_set_comparison_mode(Adc *p_adc, uint8_t uc_mode)
{
    p_adc->ADC_EMR &= (uint32_t)~(ADC_EMR_CMPMODE_Msk);
    p_adc->ADC_EMR |= (uc_mode & ADC_EMR_CMPMODE_Msk);
}

/**
 * \brief get comparsion mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param mode  ADC comparsion mode.
 *
 * \retval compare mode value.
 */
uint32_t adc_get_comparison_mode(Adc *p_adc)
{
    return p_adc->ADC_EMR & ADC_EMR_CMPMODE_Msk;
}

/**
 * \brief Configures ADC compare window.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param w_low_threshold  Low threshold of compare window.
 * \param w_high_threshold  High threshold of compare window.
 *
 */
void adc_set_comparsion_window(Adc *p_adc, uint16_t us_low_threshold, uint16_t us_high_threshold)
{
    p_adc->ADC_CWR = ADC_CWR_LOWTHRES(us_low_threshold) | ADC_CWR_HIGHTHRES(us_high_threshold);
}

/**
 * \brief Configures comparison selected channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Comparison selected channel.
 *
 */
void adc_set_comparison_channel(Adc *p_adc, adc_channel_num_t channel)
{
    if ( channel < 16 )
    {
        p_adc->ADC_EMR &= (uint32_t)~(ADC_EMR_CMPALL);
        p_adc->ADC_EMR &= (uint32_t)~(ADC_EMR_CMPSEL_Msk);
        p_adc->ADC_EMR |= (channel << ADC_EMR_CMPSEL_Pos);
    }
    else
    {  
        p_adc->ADC_EMR |= ADC_EMR_CMPALL;
    }
}

#if SAM3S_SERIES || SAM3XA_SERIES
/**
 * \brief Enables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
 void adc_enable_differential_input(Adc *p_adc, adc_channel_num_t channel)
{
    p_adc->ADC_COR |= 0x01u << (16+ channel);
}

/**
 * \brief Disables differential input for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
void adc_disable_differential_input(Adc *p_adc, adc_channel_num_t channel)
{
    uint32_t ul_temp;
    ul_temp = p_adc->ADC_COR;	
    p_adc->ADC_COR &= 0xfffeffffu << channel;
    p_adc->ADC_COR |= ul_temp;	
}

/**
 * \brief Enables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
void adc_enable_input_offset(Adc *p_adc, adc_channel_num_t channel)
{
    p_adc->ADC_COR |= 0x01u <<  channel;
}

/**
 * \brief Disables analog signal offset for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 *
 */
void adc_disable_input_offset(Adc *p_adc, adc_channel_num_t channel)
{
    uint32_t ul_temp;
    ul_temp = p_adc->ADC_COR;
    p_adc->ADC_COR &= (0xfffffffeu << channel);
    p_adc->ADC_COR |= ul_temp;	
}

/**
 * \brief Configures input gain for the specified channel.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param channel  Channel number.
 * \param gain  Gain value for the input.
 *
 */
void adc_set_input_gain(Adc *p_adc, adc_channel_num_t channel, adc_gainvalue_t gain)
{
    p_adc->ADC_CGR |= (0x03u << (2*channel)) & (gain << (2*channel));
}
#endif /* SAM3S_SERIES || SAM3XA_SERIES */

#if SAM3SD8_SERIES
/**
 * \brief set adc auto calibration mode.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
void adc_set_calibmode(Adc *p_adc)
{
    p_adc->ADC_CR |= ADC_CR_AUTOCAL;
}
#endif /* SAM3SD8_SERIES */

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
 *
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
 *
 */
void adc_disable_interrupt(Adc *p_adc, uint32_t ul_source)
{
    p_adc->ADC_IDR = ul_source;
}

/**
 * \brief Reads ADC interrupt status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \retval ADC interrupt status.
 */
uint32_t adc_get_interrupt_status(Adc *p_adc)
{
    return p_adc->ADC_ISR ;
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
    return p_adc->ADC_OVER & (0x01u << adc_ch);
}

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
void adc_set_bias_current(Adc *p_adc, uint8_t uc_ibctl)
{
     p_adc->ADC_ACR |= ADC_ACR_IBCTL(uc_ibctl);
}
#endif

#if SAM3S_SERIES ||  SAM3XA_SERIES
/**
 * \brief turn on temperature sensor.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
void adc_enable_ts(Adc *p_adc)
{
     p_adc->ADC_ACR |= ADC_ACR_TSON;
}
#endif

#if SAM3S_SERIES ||  SAM3XA_SERIES
/**
 * \brief turn off temperature sensor.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 */
void adc_disable_ts(Adc *p_adc)
{
     p_adc->ADC_ACR &= ~ADC_ACR_TSON;
}
#endif

/**
 * \brief Enables or disables write protection of ADC registers.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_eanble  1 to eanble, 0 to disable.
 */
void adc_set_writeprotect(Adc *p_adc, uint32_t ul_enable)
{
    p_adc->ADC_WPMR |= ADC_WPMR_WPKEY(ul_enable);
}

/**
 * \brief Indicates write protect status.
 *
 * \param p_adc  Pointer to an ADC instance.
 *
 * \return 0 if the peripheral is not protected, or 16-bit Write Protect Violation Status.
 */
uint32_t adc_get_writeprotect_status(Adc *p_adc)
{
    return p_adc->ADC_WPSR & ADC_WPSR_WPVS;
}

#if 0
/**
 * \brief calcul_startup
 */
static uint32_t calcul_startup( uint32_t ul_startup )
{
    uint32_t ul_startup_value=0;

    if( ul_startup == 0 )
        ul_startup_value = 0;
    else if( ul_startup == 1 )
        ul_startup_value = 8;
    else if( ul_startup == 2 )
        ul_startup_value = 16;
    else if( ul_startup == 3 )
        ul_startup_value = 24;
    else if( ul_startup == 4 )
        ul_startup_value = 64;
    else if( ul_startup == 5 )
        ul_startup_value = 80;
    else if( ul_startup == 6 )
        ul_startup_value = 96;
    else if( ul_startup == 7 )
        ul_startup_value = 112;
    else if( ul_startup == 8 )
        ul_startup_value = 512;
    else if( ul_startup == 9 )
        ul_startup_value = 576;
    else if( ul_startup == 10 )
        ul_startup_value = 640;
    else if( ul_startup == 11 )
        ul_startup_value = 704;
    else if( ul_startup == 12 )
        ul_startup_value = 768;
    else if( ul_startup == 13 )
        ul_startup_value = 832;
    else if( ul_startup == 14 )
        ul_startup_value = 896;
    else if( ul_startup == 15 )
        ul_startup_value = 960;

    return ul_startup_value;
}

/**
 * \brief Checks ADC configurations.
 *
 * \param p_adc  Pointer to an ADC instance.
 * \param dw_mck  Main clock of the device (value in Hz).
 */
void adc_check(Adc* p_adc, uint32_t ul_mck)
{
    uint32_t ul_adcfreq;
    uint32_t ul_prescal;
    uint32_t ul_startup;

    /* ADCClock = MCK / ( (PRESCAL+1) * 2 ) */
    ul_prescal = (( p_adc->ADC_MR & ADC_MR_PRESCAL_Msk) >> ADC_MR_PRESCAL_Pos);
    ul_adcfreq = ul_mck / ((ul_prescal+1)*2);
    printf("ADC clock frequency =  %d Hz\r\n", (int)ul_adcfreq );

    if( ul_adcfreq < ADC_FREQ_MIN )
    {
        printf("adc frequency too low (out of specification: %d Hz)\r\n", (int)ADC_FREQ_MIN);
    }
    if( ul_adcfreq > ADC_FREQ_MAX )
    {
        printf("adc frequency too high (out of specification: %d Hz)\r\n", (int)ADC_FREQ_MAX);
    }

    ul_startup = (( p_adc->ADC_MR & ADC_MR_STARTUP_Msk) >> ADC_MR_STARTUP_Pos);
    if( !(p_adc->ADC_MR & ADC_MR_SLEEP_SLEEP) )
    {
        /* 40ms */
        if( ADC_STARTUP_NORM * ul_adcfreq / 1000000 > calcul_startup(ul_startup) )
        {
            printf("Startup time too small: %d, programmed: %d\r\n", (int)(ADC_STARTUP_NORM * ul_adcfreq / 1000000), (int)calcul_startup(ul_startup));
        }
    }
    else
    {
        if(p_adc->ADC_MR & ADC_MR_FREERUN_ON)
        {
            printf("FreeRun forbidden in sleep mode\n\r");
        }
        if( !(p_adc->ADC_MR & ADC_MR_FWUP_ON) )
        {
            /* Sleep 40ms */
            if( ADC_STARTUP_NORM * ul_adcfreq / 1000000 > calcul_startup(ul_startup) )
            {
                printf("Startup time too small: %d, programmed: %d\r\n", (int)(ADC_STARTUP_NORM * ul_adcfreq / 1000000), (int)(calcul_startup(ul_startup)));
            }
        }
        else
        {
            if( p_adc->ADC_MR & ADC_MR_FWUP_ON )
            {
                /* Fast Wake Up Sleep Mode: 12ms */
                if( ADC_STARTUP_FAST * ul_adcfreq / 1000000 > calcul_startup(ul_startup) )
                {
                    printf("Startup time too small: %d, programmed: %d\r\n", (int)(ADC_STARTUP_NORM * ul_adcfreq / 1000000), (int)(calcul_startup(ul_startup)));
                }
            }
        }
    }
}
#endif /* 0 */

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
#endif
/**INDENT-ON**/
/// @endcond

#endif /* SAM3S_SERIES || SAM3N_SERIES || SAM3XA_SERIES */

