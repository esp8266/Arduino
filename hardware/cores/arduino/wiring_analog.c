/*
  wiring_analog.c - analog input and output
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

uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	analog_reference = mode;
}

int analogRead(uint8_t pin)
{
	uint8_t low, high;

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
	ADMUX = (analog_reference << 6) | (pin & 0x0f);
  
#if defined(__AVR_ATmega1280__)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	// without a delay, we seem to read from the wrong channel
	//delay(1);

	// start the conversion
	sbi(ADCSRA, ADSC);

	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC));

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low = ADCL;
	high = ADCH;

	// combine the two bytes
	return (high << 8) | low;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistenty with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);
	
	if (digitalPinToTimer(pin) == TIMER1A) {
		// connect pwm to pin on timer 1, channel A
		sbi(TCCR1A, COM1A1);
		// set pwm duty
		OCR1A = val;
	} else if (digitalPinToTimer(pin) == TIMER1B) {
		// connect pwm to pin on timer 1, channel B
		sbi(TCCR1A, COM1B1);
		// set pwm duty
		OCR1B = val;
#if defined(__AVR_ATmega8__)
	} else if (digitalPinToTimer(pin) == TIMER2) {
		// connect pwm to pin on timer 2, channel B
		sbi(TCCR2, COM21);
		// set pwm duty
		OCR2 = val;
#else
	} else if (digitalPinToTimer(pin) == TIMER0A) {
		if (val == 0) {
			digitalWrite(pin, LOW);
		} else {
			// connect pwm to pin on timer 0, channel A
			sbi(TCCR0A, COM0A1);
			// set pwm duty
			OCR0A = val;      
		}
	} else if (digitalPinToTimer(pin) == TIMER0B) {
		if (val == 0) {
			digitalWrite(pin, LOW);
		} else {
			// connect pwm to pin on timer 0, channel B
			sbi(TCCR0A, COM0B1);
			// set pwm duty
			OCR0B = val;
		}
	} else if (digitalPinToTimer(pin) == TIMER2A) {
		// connect pwm to pin on timer 2, channel A
		sbi(TCCR2A, COM2A1);
		// set pwm duty
		OCR2A = val;	
	} else if (digitalPinToTimer(pin) == TIMER2B) {
		// connect pwm to pin on timer 2, channel B
		sbi(TCCR2A, COM2B1);
		// set pwm duty
		OCR2B = val;
#endif
#if defined(__AVR_ATmega1280__)
	// XXX: need to handle other timers here
	} else if (digitalPinToTimer(pin) == TIMER3A) {
		// connect pwm to pin on timer 3, channel A
		sbi(TCCR3A, COM3A1);
		// set pwm duty
		OCR3A = val;
	} else if (digitalPinToTimer(pin) == TIMER3B) {
		// connect pwm to pin on timer 3, channel B
		sbi(TCCR3A, COM3B1);
		// set pwm duty
		OCR3B = val;
	} else if (digitalPinToTimer(pin) == TIMER3C) {
		// connect pwm to pin on timer 3, channel C
		sbi(TCCR3A, COM3C1);
		// set pwm duty
		OCR3C = val;
	} else if (digitalPinToTimer(pin) == TIMER4A) {
		// connect pwm to pin on timer 4, channel A
		sbi(TCCR4A, COM4A1);
		// set pwm duty
		OCR4A = val;
	} else if (digitalPinToTimer(pin) == TIMER4B) {
		// connect pwm to pin on timer 4, channel B
		sbi(TCCR4A, COM4B1);
		// set pwm duty
		OCR4B = val;
	} else if (digitalPinToTimer(pin) == TIMER4C) {
		// connect pwm to pin on timer 4, channel C
		sbi(TCCR4A, COM4C1);
		// set pwm duty
		OCR4C = val;
	} else if (digitalPinToTimer(pin) == TIMER5A) {
		// connect pwm to pin on timer 5, channel A
		sbi(TCCR5A, COM5A1);
		// set pwm duty
		OCR5A = val;
	} else if (digitalPinToTimer(pin) == TIMER5B) {
		// connect pwm to pin on timer 5, channel B
		sbi(TCCR5A, COM5B1);
		// set pwm duty
		OCR5B = val;
#endif
	} else if (val < 128)
		digitalWrite(pin, LOW);
	else
		digitalWrite(pin, HIGH);
}
