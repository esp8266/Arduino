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

/**
 * \file
 *
 * Interface for Real Time Clock (RTC) controller.
 *
 */

#ifndef _RTC_
#define _RTC_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "../chip.h"

#include <stdint.h>

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

#define RTC_HOUR_BIT_LEN_MASK   0x3F
#define RTC_MIN_BIT_LEN_MASK    0x7F
#define RTC_SEC_BIT_LEN_MASK    0x7F
#define RTC_CENT_BIT_LEN_MASK   0x7F
#define RTC_YEAR_BIT_LEN_MASK   0xFF
#define RTC_MONTH_BIT_LEN_MASK  0x1F
#define RTC_DATE_BIT_LEN_MASK   0x3F
#define RTC_WEEK_BIT_LEN_MASK   0x07

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus
 extern "C" {
#endif

extern void RTC_SetHourMode( Rtc* pRtc, uint32_t dwMode ) ;

extern uint32_t RTC_GetHourMode( Rtc* pRtc ) ;

extern void RTC_EnableIt( Rtc* pRtc, uint32_t dwSources ) ;

extern void RTC_DisableIt( Rtc* pRtc, uint32_t dwSources ) ;

extern int RTC_SetTime( Rtc* pRtc, uint8_t ucHour, uint8_t ucMinute, uint8_t ucSecond ) ;

extern void RTC_GetTime( Rtc* pRtc, uint8_t *pucHour, uint8_t *pucMinute, uint8_t *pucSecond ) ;

extern int RTC_SetTimeAlarm( Rtc* pRtc, uint8_t *pucHour, uint8_t *pucMinute, uint8_t *pucSecond ) ;

extern void RTC_GetDate( Rtc* pRtc, uint16_t *pwYear, uint8_t *pucMonth, uint8_t *pucDay, uint8_t *pucWeek ) ;

extern int RTC_SetDate( Rtc* pRtc, uint16_t wYear, uint8_t ucMonth, uint8_t ucDay, uint8_t ucWeek ) ;

extern int RTC_SetDateAlarm( Rtc* pRtc, uint8_t *pucMonth, uint8_t *pucDay ) ;

extern void RTC_ClearSCCR( Rtc* pRtc, uint32_t dwMask ) ;

extern uint32_t RTC_GetSR( Rtc* pRtc, uint32_t dwMask ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _RTC_ */

