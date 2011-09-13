/*
 %atmel_license%
*/

#ifndef _VARIANT_ARDUINO_DUE_
#define _VARIANT_ARDUINO_DUE_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "Arduino.h"
#include "common/UART.h"
#include "common/USART.h"

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

#define PIN_LED_BLUE         (0u)
#define PIN_LED_GREEN        (1u)
#define PIN_LED_RED          (2u)
#define PIN_LED              PIN_LED_BLUE

static const uint8_t SS   = 23 ;
static const uint8_t MOSI = 21 ;
static const uint8_t MISO = 20 ;
static const uint8_t SCK  = 22 ;

#define PINS_UART            (6u)

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

extern UARTClass Serial ;

extern USARTClass Serial2 ;
extern USARTClass Serial3 ;

#endif

#endif /* _VARIANT_ARDUINO_DUE_ */

