/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id$
*/

#include "wiring_private.h"

#ifdef __cplusplus
 extern "C" {
#endif

uint32_t millis( void )
{
// todo: ensure no interrupts
	return GetTickCount() ;
}

uint32_t micros( void )
{
  uint32_t dwTicks ;

  __disable_irq() ;
  dwTicks=SysTick->VAL ;
  __enable_irq() ;

  return (GetTickCount()*1000) + ((SysTick->LOAD + 1 - dwTicks)/(VARIANT_MCK/1000000)) ;
}

/// \brief Pauses the program for the amount of time (in miliseconds) specified as parameter.
/// (There are 1000 milliseconds in a second.)
///
/// \param dwMs the number of milliseconds to pause (uint32_t)
///
void delay( uint32_t dwMs )
{
	Wait( dwMs ) ;
}

/// \brief Pauses the program for the amount of time (in microseconds) specified as parameter.
///
/// \param dwUs the number of microseconds to pause (uint32_t)
///
void delayMicroseconds( uint32_t dwUs )
{
  uint32_t dwStartMicros=micros() ;

	while ( (micros() - dwStartMicros) < dwUs )
	{
		//	do nothing
	}
}

/*
 * Cortex-M3 Systick IT handler
 */
extern void SysTick_Handler( void )
{
  // Increment tick count each ms
  TimeTick_Increment() ;
}

extern void init( void )
{
	SystemInit() ;

    /* Set Systick to 1ms interval, common to all SAM3 variants */
	if ( SysTick_Config( SystemCoreClock / 1000 ) )
    {
        /* Capture error */
    	while ( 1 ) ;
	}

    /* Disable watchdog, common to all SAM variants */
    WDT_Disable( WDT ) ;

    // Initialize Serial port UART0, common to all SAM3 variants
    PIO_Configure( APinDescription[PINS_UART].pPort, APinDescription[PINS_UART].dwPinType,
                 APinDescription[PINS_UART].dwPin, APinDescription[PINS_UART].dwPinAttribute ) ;

    // Switch off Power LED
    PIO_Configure( APinDescription[PIN_LED_RED].pPort, APinDescription[PIN_LED_RED].dwPinType,
                   APinDescription[PIN_LED_RED].dwPin, APinDescription[PIN_LED_RED].dwPinAttribute ) ;
    PIO_Clear( APinDescription[PIN_LED_RED].pPort, APinDescription[PIN_LED_RED].dwPin ) ;
}

/**
 * \brief
 *
 * \param c  Character to output.
 *
 * \return The character that was output.
 */
extern WEAK signed int putchar( signed int c )
{
    return c ;
}

#ifdef __cplusplus
}
#endif
