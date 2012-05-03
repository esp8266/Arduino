/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_SAM3X_EK_
#define _VARIANT_SAM3X_EK_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

// We have native USB on this variant
#define USBCON

#include "Arduino.h"
#ifdef __cplusplus
#include "UARTClass.h"
#include "USARTClass.h"
#endif

/**
 * Libc porting layers
 */
#if defined (  __GNUC__  ) /* GCC CS3 */
#    include <syscalls.h> /** RedHat Newlib minimal stub */
#endif

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

#ifndef sam3x_ek
#define sam3x_ek
#endif

/** Name of the board */
#define VARIANT_NAME "SAM3X-EK"


/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC           12000000

/** Master clock frequency */
#define VARIANT_MCK               84000000

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// Number of pins defined in PinDescription array
#define PINS_COUNT           (10u)

// LEDs
#define PIN_LED_BLUE         (2u)
#define PIN_LED_AMBER        (3u)
#define PIN_LED_GREEN        (4u)
#define PIN_LED_RED          (5u)
#define PIN_LED              PIN_LED_AMBER
#define PIN_LED2             PIN_LED_BLUE
#define PIN_LED3             PIN_LED_GREEN

/*
 * UART/USART Interfaces
 */
#define PINS_UART            (8u)

#define PINS_USART0          (9u)


/*
 * Analog pins
 */
static const uint8_t A0  = 10;

/*
 * PWM
 */
#define PWM_INTERFACE        PWM
#define PWM_INTERFACE_ID     ID_PWM
#define PWM_FREQUENCY        1000
#define PWM_MAX_DUTY_CYCLE   255
#define PWM_MIN_DUTY_CYCLE   0

/*
 * TC
 */
#define TC_INTERFACE        TC0
#define TC_INTERFACE_ID     ID_TC0
#define TC_FREQUENCY        1000
#define TC_MAX_DUTY_CYCLE   255
#define TC_MIN_DUTY_CYCLE   0

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

extern UARTClass Serial1 ;

extern USARTClass Serial2 ;

#endif

#endif /* _VARIANT_SAM3X_EK_ */

