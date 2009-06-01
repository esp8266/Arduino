/*
  wiring_digital.c - digital input and output functions
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

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include "wiring_private.h"
#include "pins_arduino.h"

void pinMode(uint8_t pin, uint8_t mode)
{
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *reg;

	if (port == NOT_A_PIN) return;

	// JWS: can I let the optimizer do this?
	reg = portModeRegister(port);

	if (mode == INPUT) *reg &= ~bit;
	else *reg |= bit;
}

// Forcing this inline keeps the callers from having to push their own stuff
// on the stack. It is a good performance win and only takes 1 more byte per
// user than calling. (It will take more bytes on the 168.)
//
// But shouldn't this be moved into pinMode? Seems silly to check and do on
// each digitalread or write.
//
static inline void turnOffPWM(uint8_t timer) __attribute__ ((always_inline));
static inline void turnOffPWM(uint8_t timer)
{
	if (timer == TIMER1A) cbi(TCCR1A, COM1A1);
	if (timer == TIMER1B) cbi(TCCR1A, COM1B1);

#if defined(__AVR_ATmega8__)
	if (timer == TIMER2) cbi(TCCR2, COM21);
#else
	if (timer == TIMER0A) cbi(TCCR0A, COM0A1);
	if (timer == TIMER0B) cbi(TCCR0A, COM0B1);
	if (timer == TIMER2A) cbi(TCCR2A, COM2A1);
	if (timer == TIMER2B) cbi(TCCR2A, COM2B1);
#endif

#if defined(__AVR_ATmega1280__)
	if (timer == TIMER3A) cbi(TCCR3A, COM3A1);
	if (timer == TIMER3B) cbi(TCCR3A, COM3B1);
	if (timer == TIMER3C) cbi(TCCR3A, COM3C1);
	if (timer == TIMER4A) cbi(TCCR4A, COM4A1);
	if (timer == TIMER4B) cbi(TCCR4A, COM4B1);
	if (timer == TIMER4C) cbi(TCCR4A, COM4C1);
	if (timer == TIMER5A) cbi(TCCR5A, COM5A1);
	if (timer == TIMER5B) cbi(TCCR5A, COM5B1);
	if (timer == TIMER5C) cbi(TCCR5A, COM5C1);
#endif
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	uint8_t timer = digitalPinToTimer(pin);
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out;

	if (port == NOT_A_PIN) return;

	// If the pin that support PWM output, we need to turn it off
	// before doing a digital write.
	if (timer != NOT_ON_TIMER) turnOffPWM(timer);

	out = portOutputRegister(port);

	if (val == LOW) *out &= ~bit;
	else *out |= bit;
}

int digitalRead(uint8_t pin)
{
	uint8_t timer = digitalPinToTimer(pin);
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);

	if (port == NOT_A_PIN) return LOW;

	// If the pin that support PWM output, we need to turn it off
	// before getting a digital reading.
	if (timer != NOT_ON_TIMER) turnOffPWM(timer);

	if (*portInputRegister(port) & bit) return HIGH;
	return LOW;
}
