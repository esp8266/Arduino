/*
 %atmel_license%
*/

#ifndef _VARIANT_ARDUINO_DUE_
#define _VARIANT_ARDUINO_DUE_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "Arduino.h"
#include "UART.h"
#include "USART.h"

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

#ifndef arduino_due
#define arduino_due
#endif

/** Name of the board */
#define VARIANT_NAME "ARDUINO_DUE"

/*
#define VARIANT_REV_A
#define VARIANT_REV_B
*/

/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC           12000000

/** Master clock frequency */
#define VARIANT_MCK               96000000

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

#define PIN_LED_13           (0u)
#define PIN_LED_RXL          (1u)
#define PIN_LED_TXL          (2u)
#define PIN_LED              PIN_LED_13
#define PIN_LED2             PIN_LED_RXL
#define PIN_LED3             PIN_LED_TXL

#define PINS_UART            (3u)

#define PINS_USART0          (6u)
#define PINS_USART1          (9u)
#define PINS_USART2          (12u)

static const uint8_t SS   = 18 ;
static const uint8_t MOSI = 16 ;
static const uint8_t MISO = 15 ;
static const uint8_t SCK  = 17 ;

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

extern UARTClass Serial ;

extern USARTClass Serial2 ;
extern USARTClass Serial3 ;
extern USARTClass Serial4 ;

#endif

#endif /* _VARIANT_ARDUINO_DUE_ */

