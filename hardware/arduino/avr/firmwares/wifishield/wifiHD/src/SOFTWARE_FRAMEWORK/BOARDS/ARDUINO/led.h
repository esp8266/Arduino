/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief AT32UC3A EVK1100 board LEDs support package.
 *
 * This file contains definitions and services related to the LED features of
 * the EVK1100 board.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 AT32UC3A devices can be used.
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

#ifndef _LED_H_
#define _LED_H_

#include "compiler.h"


/*! \name Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED0  0x01
#define LED1  0x02
#define LED2  0x04
#define LED3  0x08
#define LED4  0x10
#define LED5  0x20
#define LED6  0x40
#define LED7  0x80
//! @}


/*! \brief Gets the last state of all LEDs set through the LED API.
 *
 * \return State of all LEDs (1 bit per LED).
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern U32 LED_Read_Display(void);

/*! \brief Sets the state of all LEDs.
 *
 * \param leds New state of all LEDs (1 bit per LED).
 *
 * \note The pins of all LEDs are set to GPIO output mode.
 */
extern void LED_Display(U32 leds);

/*! \brief Gets the last state of the specified LEDs set through the LED API.
 *
 * \param mask LEDs of which to get the state (1 bit per LED).
 *
 * \return State of the specified LEDs (1 bit per LED).
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern U32 LED_Read_Display_Mask(U32 mask);

/*! \brief Sets the state of the specified LEDs.
 *
 * \param mask LEDs of which to set the state (1 bit per LED).
 *
 * \param leds New state of the specified LEDs (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Display_Mask(U32 mask, U32 leds);

/*! \brief Tests the last state of the specified LEDs set through the LED API.
 *
 * \param leds LEDs of which to test the state (1 bit per LED).
 *
 * \return \c TRUE if at least one of the specified LEDs has a state on, else
 *         \c FALSE.
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern Bool LED_Test(U32 leds);

/*! \brief Turns off the specified LEDs.
 *
 * \param leds LEDs to turn off (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Off(U32 leds);

/*! \brief Turns on the specified LEDs.
 *
 * \param leds LEDs to turn on (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_On(U32 leds);

/*! \brief Toggles the specified LEDs.
 *
 * \param leds LEDs to toggle (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Toggle(U32 leds);

/*! \brief Gets as a bit-field the last state of the specified LEDs set through
 *         the LED API.
 *
 * \param field LEDs of which to get the state (1 bit per LED).
 *
 * \return State of the specified LEDs (1 bit per LED, beginning with the first
 *         specified LED).
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern U32 LED_Read_Display_Field(U32 field);

/*! \brief Sets as a bit-field the state of the specified LEDs.
 *
 * \param field LEDs of which to set the state (1 bit per LED).
 * \param leds New state of the specified LEDs (1 bit per LED, beginning with
 *             the first specified LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Display_Field(U32 field, U32 leds);

/*! \brief Gets the intensity of the specified LED.
 *
 * \param led LED of which to get the intensity (1 bit per LED; only the least
 *            significant set bit is used).
 *
 * \return Intensity of the specified LED (0x00 to 0xFF).
 *
 * \warning The PWM channel of the specified LED is supposed to be used only by
 *          this module.
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern U8 LED_Get_Intensity(U32 led);

/*! \brief Sets the intensity of the specified LEDs.
 *
 * \param leds LEDs of which to set the intensity (1 bit per LED).
 * \param intensity New intensity of the specified LEDs (0x00 to 0xFF).
 *
 * \warning The PWM channels of the specified LEDs are supposed to be used only
 *          by this module.
 *
 * \note The pins of the specified LEDs are set to PWM output mode.
 */
extern void LED_Set_Intensity(U32 leds, U8 intensity);


#endif  // _LED_H_
