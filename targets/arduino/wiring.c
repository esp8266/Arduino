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

// The number of times timer 0 has overflowed since the program started.
// Must be volatile or gcc will optimize away some uses of it.
volatile unsigned long timer0_overflow_count;

SIGNAL(SIG_OVERFLOW0)
{
	timer0_overflow_count++;
}

unsigned long millis()
{
	// timer 0 increments every 64 cycles, and overflows when it reaches
	// 256.  we would calculate the total number of clock cycles, then
	// divide by the number of clock cycles per millisecond, but this
	// overflows too often.
	//return timer0_overflow_count * 64UL * 256UL / (F_CPU / 1000UL);
	
	// instead find 1/128th the number of clock cycles and divide by
	// 1/128th the number of clock cycles per millisecond
	return timer0_overflow_count * 64UL * 2UL / (F_CPU / 128000UL);
}

void delay(unsigned long ms)
{
	unsigned long start = millis();
	
	while (millis() - start < ms)
		;
}

/* Delay for the given number of microseconds.  Assumes a 16 MHz clock. 
 * Disables interrupts, which will disrupt the millis() function if used
 * too frequently. */
void delayMicroseconds(unsigned int us)
{
	// calling avrlib's delay_us() function with low values (e.g. 1 or
	// 2 microseconds) gives delays longer than desired.
	//delay_us(us);

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call yields a delay of approximately 1 1/8 us.
	if (--us == 0)
		return;

	// the following loop takes a quarter of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2;

	// account for the time taken in the preceeding commands.
	us -= 2;

	// disable interrupts, otherwise the timer 0 overflow interrupt that
	// tracks milliseconds will make us delay longer than we want.
	cli();

	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);

	// reenable interrupts.
	sei();
}

/*
unsigned long pulseIn(int pin, int state)
{
	unsigned long width = 0;

	while (digitalRead(pin) == !state)
		;
		
	while (digitalRead(pin) != !state)
		width++;
		
	return width * 17 / 2; // convert to microseconds
}
*/

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 10 microseconds
 * to 3 minutes in length, but must be called at least N microseconds before
 * the start of the pulse. */
unsigned long pulseIn(int pin, int state)
{
	// cache the port and bit of the pin in order to speed up the
	// pulse width measuring loop and achieve finer resolution.  calling
	// digitalRead() instead yields much coarser resolution.
	int r = port_to_input[digitalPinToPort(pin)];
	int bit = digitalPinToBit(pin);
	int mask = 1 << bit;
	unsigned long width = 0;

	// compute the desired bit pattern for the port reading (e.g. set or
	// clear the bit corresponding to the pin being read).  the !!state
	// ensures that the function treats any non-zero value of state as HIGH.
	state = (!!state) << bit;

	// wait for the pulse to start
	while ((_SFR_IO8(r) & mask) != state)
		;
	
	// wait for the pulse to stop
	while ((_SFR_IO8(r) & mask) == state)
		width++;
	
	// convert the reading to microseconds.  the slower the CPU speed, the
	// proportionally fewer iterations of the loop will occur (e.g. a 
	// 4 MHz clock will yield a width that is one-fourth of that read with
	// a 16 MHz clock).  each loop was empirically determined to take
	// approximately 23/20 of a microsecond with a 16 MHz clock.
	return width * (16000000UL / F_CPU) * 20 / 23;
}

void shiftOut(int dataPin, int clockPin, int bitOrder, byte val) {
	int i;

	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST)
			digitalWrite(dataPin, !!(val & (1 << i)));
		else	
			digitalWrite(dataPin, !!(val & (1 << (7 - i))));
			
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);		
	}
}

void init()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();
	
	// timer 0 is used for millis() and delay()
	timer0_overflow_count = 0;
	// on the ATmega168, timer 0 is also used for fast hardware pwm
	// (using phase-correct PWM would mean that timer 0 overflowed half as often
	// resulting in different millis() behavior on the ATmega8 and ATmega168)
#if defined(__AVR_ATmega168__)
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);
#endif  
	// set timer 0 prescale factor to 64
#if defined(__AVR_ATmega168__)
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
#else
	sbi(TCCR0, CS01);
	sbi(TCCR0, CS00);
#endif
	// enable timer 0 overflow interrupt
#if defined(__AVR_ATmega168__)
	sbi(TIMSK0, TOIE0);
#else
	sbi(TIMSK, TOIE0);
#endif

	// timers 1 and 2 are used for phase-correct hardware pwm
	// this is better for motors as it ensures an even waveform
	// note, however, that fast pwm mode can achieve a frequency of up
	// 8 MHz (with a 16 MHz clock) at 50% duty cycle

	// set timer 1 prescale factor to 64
	sbi(TCCR1B, CS11);
	sbi(TCCR1B, CS10);
	// put timer 1 in 8-bit phase correct pwm mode
	sbi(TCCR1A, WGM10);

	// set timer 2 prescale factor to 64
#if defined(__AVR_ATmega168__)
	sbi(TCCR2B, CS22);
#else
	sbi(TCCR2, CS22);
#endif
	// configure timer 2 for phase correct pwm (8-bit)
#if defined(__AVR_ATmega168__)
	sbi(TCCR2A, WGM20);
#else
	sbi(TCCR2, WGM20);
#endif

	// set a2d reference to AVCC (5 volts)
	cbi(ADMUX, REFS1);
	sbi(ADMUX, REFS0);

	// set a2d prescale factor to 128
	// 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
	// XXX: this will not work properly for other clock speeds, and
	// this code should use F_CPU to determine the prescale factor.
	sbi(ADCSRA, ADPS2);
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);

	// enable a2d conversions
	sbi(ADCSRA, ADEN);

	// the bootloader connects pins 0 and 1 to the USART; disconnect them
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
#if defined(__AVR_ATmega168__)
	UCSR0B = 0;
#else
	UCSRB = 0;
#endif
}