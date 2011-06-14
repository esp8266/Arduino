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

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

/*
SIGNAL(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
}
*/

uint32_t millis( void )
{
// todo: ensure no interrupts
	return GetTickCount() ;
}

unsigned long micros( void )
{
/*
	unsigned long m;
	uint8_t oldSREG = SREG, t;

	cli();
	m = timer0_overflow_count;
#if defined(TCNT0)
	t = TCNT0;
#elif defined(TCNT0L)
	t = TCNT0L;
#else
	#error TIMER 0 not defined
#endif


#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t < 255))
		m++;
#else
	if ((TIFR & _BV(TOV0)) && (t < 255))
		m++;
#endif

	SREG = oldSREG;

	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
*/
  return 0 ;
}

void delay( uint32_t dwMs )
{
	Wait( dwMs ) ;
}

/* Delay for the given number of microseconds.  Assumes a 64 MHz clock. */
void delayMicroseconds(unsigned int us)
{
  unsigned long	startMicros	=	micros();

	while ((micros() - startMicros) < us)
	{
		//*	do nothing
	}
}

/*
 * Cortex-M3 Systick IT handler
 */
void SysTick_Handler( void )
{
  // Increment tick count each ms
  TimeTick_Increment() ;
}

void init( void )
{
  // Disable watchdog
  WDT_Disable( WDT ) ;

  // Set Main clock to 64MHz using external 12MHz
  LowLevelInit() ;

  // Set Systick to 1ms interval
  SysTick_Config( BOARD_MCK/1000 ) ;

  // Initialize Serial port UART0
  PIO_Configure( APinDescription[PINS_UART].pPort, APinDescription[PINS_UART].dwPinType, 
                 APinDescription[PINS_UART].dwPin, APinDescription[PINS_UART].dwPinAttribute ) ;
}
