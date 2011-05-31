/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
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
 * ----------------------------------------------------------------------------
 */

/**
 *  \file
 *
 *  \section Purpose
 *
 *  Interface for configuration the Analog-to-Digital Converter (ADC) peripheral.
 *
 *  \section Usage
 *
 *  -# Configurate the pins for ADC
 *  -# Initialize the ADC with ADC_Initialize().
 *  -# Select the active channel using ADC_EnableChannel()
 *  -# Start the conversion with ADC_StartConversion()
 *  -# Wait the end of the conversion by polling status with ADC_GetStatus()
 *  -# Finally, get the converted data using ADC_GetConvertedData()
 *
*/
#ifndef _ADC_
#define _ADC_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"

#include <assert.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *------------------------------------------------------------------------------*/
/* SAM3S */
#define ADC_FREQ_MAX   20000000
#define ADC_FREQ_MIN    1000000

#define ADC_STARTUP_NORM     40
#define ADC_STARTUP_FAST     12

#define ADC_CHANNEL_0  0
#define ADC_CHANNEL_1  1
#define ADC_CHANNEL_2  2
#define ADC_CHANNEL_3  3
#define ADC_CHANNEL_4  4
#define ADC_CHANNEL_5  5
#define ADC_CHANNEL_6  6
#define ADC_CHANNEL_7  7
#define ADC_CHANNEL_8  8
#define ADC_CHANNEL_9  9
#define ADC_CHANNEL_10 10
#define ADC_CHANNEL_11 11
#define ADC_CHANNEL_12 12
#define ADC_CHANNEL_13 13
#define ADC_CHANNEL_14 14
#define ADC_CHANNEL_15 15

#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------------------------------------------------
 *         Macros function of register access
 *------------------------------------------------------------------------------*/

#define ADC_GetModeReg( pAdc )                ((pAdc)->ADC_MR)

#define ADC_StartConversion( pAdc )           ((pAdc)->ADC_CR = ADC_CR_START)


#define ADC_EnableChannel( pAdc, channel )    {\
            assert( channel < 16 ) ;\
            (pAdc)->ADC_CHER = (1 << (channel));\
        }

#define ADC_DisableChannel(pAdc, channel)  {\
            assert( (channel) < 16 ) ;\
            (pAdc)->ADC_CHDR = (1 << (channel));\
        }

#define ADC_EnableIt(pAdc, dwMode)            {\
            (pAdc)->ADC_IER = (dwMode);\
        }

#define ADC_DisableIt(pAdc, dwMode)           {\
            (pAdc)->ADC_IDR = (dwMode);\
        }

#define ADC_EnableTS(pAdc,dwMode)             {\
            (pAdc)->ADC_ACR |= dwMode;\
        }

#define ADC_EnableDataReadyIt(pAdc)         ((pAdc)->ADC_IER = AT91C_ADC_DRDY)

#define ADC_GetStatus(pAdc)                 ((pAdc)->ADC_ISR)

#define ADC_GetCompareMode(pAdc)            (((pAdc)->ADC_EMR)& (ADC_EMR_CMPMODE_Msk))

#define ADC_GetChannelStatus(pAdc)          ((pAdc)->ADC_CHSR)

#define ADC_GetInterruptMaskStatus(pAdc)    ((pAdc)->ADC_IMR)

#define ADC_GetLastConvertedData(pAdc)      ((pAdc)->ADC_LCDR)

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/
extern void ADC_Initialize( Adc* pAdc, uint32_t idAdc );
extern void ADC_CfgTiming( Adc* pAdc, uint32_t tracking, uint32_t settling, uint32_t transfer );
extern void ADC_cfgFrequency( Adc* pAdc, uint32_t startup, uint32_t prescal );
extern void ADC_CfgTrigering( Adc* pAdc, uint32_t trgEn, uint32_t trgSel, uint32_t freeRun );
extern void ADC_CfgLowRes( Adc* pAdc, uint32_t resolution );
extern void ADC_CfgPowerSave( Adc* pAdc, uint32_t sleep, uint32_t fwup );
extern void ADC_CfgChannelMode( Adc* pAdc, uint32_t useq, uint32_t anach );
extern void ADC_check( Adc* pAdc, uint32_t mck_freq );

extern uint32_t ADC_GetConvertedData( Adc* pAdc, uint32_t dwChannel ) ;
extern void ADC_SetCompareChannel( Adc* pAdc, uint32_t dwChannel ) ;
extern void ADC_SetCompareMode( Adc* pAdc, uint32_t dwMode ) ;
extern void ADC_SetComparisonWindow( Adc* pAdc, uint32_t dwHi_Lo ) ;
extern uint32_t ADC_IsInterruptMasked( Adc* pAdc, uint32_t dwFlag ) ;
extern uint32_t ADC_IsStatusSet( Adc* pAdc, uint32_t dwFlag ) ;
extern uint32_t ADC_IsChannelInterruptStatusSet( uint32_t adc_sr, uint32_t dwChannel ) ;
extern uint32_t ADC_ReadBuffer( Adc* pADC, int16_t *pwBuffer, uint32_t dwSize ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ADC_ */
