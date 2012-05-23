/*
  Copyright (c) 2012 Google, Inc.  All right reserved.

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

#ifndef _VARIANT_ADK2_
#define _VARIANT_ADK2_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

// We have native USB on this variant
//#define USBCON

#include "Arduino.h"
#ifdef __cplusplus
#include "UARTClass.h"
#include "USARTClass.h"
#endif

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

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

#ifndef adk2
#define adk2
#endif

/** Name of the board */
#define VARIANT_NAME "ADK2"

/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC           12000000

/** Master clock frequency */
#define VARIANT_MCK               84000000

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// Number of pins defined in PinDescription array
#define PINS_COUNT           (75u)

// LEDs
#define PIN_LED_13           (13u)
#define PIN_LED_RXL          (73u) // fixme
#define PIN_LED_TXL          (74u) // fixme
#define PIN_LED              PIN_LED_13
#define PIN_LED2             PIN_LED_RXL // fixme
#define PIN_LED3             PIN_LED_TXL // fixme

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define SPI_INTERFACE        SPI
#define SPI_INTERFACE_ID     ID_SPI
#define PIN_SPI_SS           (53u)
#define PIN_SPI_MOSI         (51u)
#define PIN_SPI_MISO         (50u)
#define PIN_SPI_SCK          (52u)

static const uint8_t SS   = 53 ;
static const uint8_t MOSI = 51 ;
static const uint8_t MISO = 50 ;
static const uint8_t SCK  = 52 ;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 2

#define PIN_WIRE_SDA         (20u)
#define PIN_WIRE_SCL         (21u)
#define WIRE_INTERFACE       TWI0
#define WIRE_INTERFACE_ID    ID_TWI0
#define WIRE_ISR_HANDLER     TWI1_IrqHandler

#define PIN_WIRE1_SDA        (64u)
#define PIN_WIRE1_SCL        (65u)
#define WIRE1_INTERFACE      TWI1
#define WIRE1_INTERFACE_ID   ID_TWI1
#define WIRE1_ISR_HANDLER    TWI0_IrqHandler

/*
 * UART/USART Interfaces
 */
#define PINS_UART            (70u)

#define PINS_USART0          (71u)
#define PINS_USART1          (72u)
#define PINS_USART2          (73u)
#define PINS_USART3          (74u)

/*
 * USB Interfaces
 */
#define PINS_USB             (75u)


/*
 * Analog pins
 */
static const uint8_t A0  = 54;
static const uint8_t A1  = 55;
static const uint8_t A2  = 56;
static const uint8_t A3  = 57;
static const uint8_t A4  = 58;
static const uint8_t A5  = 59;
static const uint8_t A6  = 60;
static const uint8_t A7  = 61;
static const uint8_t A8  = 62;
static const uint8_t A9  = 63;
//static const uint8_t A10 = 64;
//static const uint8_t A11 = 65;
//static const uint8_t A12 = 66;
//static const uint8_t A13 = 67;
// static const uint8_t A14 = ;
// static const uint8_t A15 = ;

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

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

extern UARTClass Serial1 ;

extern USARTClass Serial2 ;
extern USARTClass Serial3 ;
extern USARTClass Serial4 ;
extern USARTClass Serial5 ;

#endif

#endif /* _VARIANT_ADK2_ */

