/*
  wiring_pulse.c - pulseIn() function
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
