/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief RTC driver for AVR32 UC3.
 *
 * AVR32 Real Time Counter driver module.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with an RTC and a PM module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef _RTC_H_
#define _RTC_H_

#include "compiler.h"
#include <avr32/io.h>


/*! \name Oscillator Types
 */
//! @{
#define RTC_OSC_32KHZ    1
#define RTC_OSC_RC       0
//! @}

/*! \name Predefined PSEL Values
 */
//! @{

//! The PSEL value to set the RTC source clock (after the prescaler) to 1 Hz,
//! when using an external 32-kHz crystal.
#define RTC_PSEL_32KHZ_1HZ    14

//! The PSEL value to set the RTC source clock (after the prescaler) to 1.76 Hz,
//! when using the internal RC oscillator (~ 115 kHz).
#define RTC_PSEL_RC_1_76HZ    15

//! @}


/*!
 * \brief This function will initialise the RTC module.
 *        If you use the 32 KHz oscillator, it will enable this module.
 *        This function also set the top value of the RTC to 0xFFFFFFFF
 *        and the value to 0.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \param osc_type The oscillator you want to use. If you need a better
 *        accuracy, use the 32 KHz oscillator (i.e. RTC_OSC_32KHZ).
 * \param psel The preselector value for the corresponding oscillator (4-bits).
 *        To obtain this value, you can use this formula:
 *        psel = log(Fosc/Frtc)/log(2)-1, where Fosc is the frequency of the
 *        oscillator you are using (32 KHz or 115 KHz) and Frtc the frequency
 *        desired.
 * \return 1 if the initialisation succeds otherwize it will return 0.
 */
extern int rtc_init(volatile avr32_rtc_t *rtc, unsigned char osc_type, unsigned char psel);

/*!
 * \brief Enable the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_enable(volatile avr32_rtc_t *rtc);

/*!
 * \brief Disable the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_disable(volatile avr32_rtc_t *rtc);

/*!
 * \brief Enable the wake up feature of the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_enable_wake_up(volatile avr32_rtc_t *rtc);

/*!
 * \brief Disable the wake up feature of the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_disable_wake_up(volatile avr32_rtc_t *rtc);

/*!
 * \brief Enable the interrupt feature of the RTC.
 *        An interrupt is raised when the value of the RTC
 *        is equal to its top value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_enable_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief Disable the interrupt feature of the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_disable_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief Clear the interrupt flag.
 *        Call this function once you handled the interrupt.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_clear_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief Get the status of interrupts.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return 1 if the interrupts are enabled otherwize it returns 0.
 */
extern int rtc_interrupt_enabled(volatile avr32_rtc_t *rtc);

/*!
 * \brief Check if an interrupt is raised.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return 1 if an interrupt is currently raised otherwize it returns 0.
 */
extern int rtc_is_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief This function sets the RTC current top value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \param top The top value you want to store.
 */
extern void rtc_set_top_value(volatile avr32_rtc_t *rtc, unsigned long top);

/*!
 * \brief This function returns the RTC current top value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return The RTC current top value.
 */
extern unsigned long rtc_get_top_value(volatile avr32_rtc_t *rtc);

/*!
 * \brief This function checks if the RTC is busy or not.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return 1 if the RTC is busy otherwize it will return 0.
 */
extern int rtc_is_busy(volatile avr32_rtc_t *rtc);

/*!
 * \brief This function sets the RTC current value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \param val The value you want to store.
 */
extern void rtc_set_value(volatile avr32_rtc_t *rtc, unsigned long val);

/*!
 * \brief This function returns the RTC current value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return The RTC current value.
 */
extern unsigned long rtc_get_value(volatile avr32_rtc_t *rtc);


#endif  // _RTC_H_
