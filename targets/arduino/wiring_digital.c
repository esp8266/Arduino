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

// Get the hardware port of the given virtual pin number.  This comes from
// the pins_*.c file for the active board configuration.
int digitalPinToPort(int pin)
{
	return digital_pin_to_port[pin].port;
}

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
int digitalPinToBit(int pin)
{
	return digital_pin_to_port[pin].bit;
}

int analogOutPinToTimer(int pin)
{
	return analog_out_pin_to_timer[pin];
}

int analogInPinToBit(int pin)
{
	return analog_in_pin_to_port[pin].bit;
}

void pinMode(int pin, int mode)
{
	if (digitalPinToPort(pin) != NOT_A_PIN) {
		if (mode == INPUT)
			cbi(_SFR_IO8(port_to_mode[digitalPinToPort(pin)]),
				digitalPinToBit(pin));
		else
			sbi(_SFR_IO8(port_to_mode[digitalPinToPort(pin)]),
				digitalPinToBit(pin));
	}
}

void digitalWrite(int pin, int val)
{
	if (digitalPinToPort(pin) != NOT_A_PIN) {
		// If the pin that support PWM output, we need to turn it off
		// before doing a digital write.

		if (analogOutPinToTimer(pin) == TIMER1A)
			cbi(TCCR1A, COM1A1);

		if (analogOutPinToTimer(pin) == TIMER1B)
			cbi(TCCR1A, COM1B1);
			
#if defined(__AVR_ATmega168__)
		if (analogOutPinToTimer(pin) == TIMER0A)
			cbi(TCCR0A, COM0A1);
			
		if (analogOutPinToTimer(pin) == TIMER0B)
			cbi(TCCR0A, COM0B1);

		if (analogOutPinToTimer(pin) == TIMER2A)
			cbi(TCCR2A, COM2A1);
			
		if (analogOutPinToTimer(pin) == TIMER2B)
			cbi(TCCR2A, COM2B1);
#else
		if (analogOutPinToTimer(pin) == TIMER2)
			cbi(TCCR2, COM21);
#endif

		if (val == LOW)
			cbi(_SFR_IO8(port_to_output[digitalPinToPort(pin)]),
				digitalPinToBit(pin));
		else
			sbi(_SFR_IO8(port_to_output[digitalPinToPort(pin)]),
				digitalPinToBit(pin));
	}
}

int digitalRead(int pin)
{
	if (digitalPinToPort(pin) != NOT_A_PIN) {
		// If the pin that support PWM output, we need to turn it off
		// before getting a digital reading.

		if (analogOutPinToTimer(pin) == TIMER1A)
			cbi(TCCR1A, COM1A1);

		if (analogOutPinToTimer(pin) == TIMER1B)
			cbi(TCCR1A, COM1B1);
			
#if defined(__AVR_ATmega168__)
		if (analogOutPinToTimer(pin) == TIMER0A)
			cbi(TCCR0A, COM0A1);
			
		if (analogOutPinToTimer(pin) == TIMER0B)
			cbi(TCCR0A, COM0B1);

		if (analogOutPinToTimer(pin) == TIMER2A)
			cbi(TCCR2A, COM2A1);
			
		if (analogOutPinToTimer(pin) == TIMER2B)
			cbi(TCCR2A, COM2B1);
#else
		if (analogOutPinToTimer(pin) == TIMER2)
			cbi(TCCR2, COM21);
#endif

		return (_SFR_IO8(port_to_input[digitalPinToPort(pin)]) >>
			digitalPinToBit(pin)) & 0x01;
	}
	
	return LOW;
}
