/*
  Copyright (c) 2011 Arduino.  All right reserved.

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

#include "Arduino.h"
#include "wiring_private.h"

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse. */
extern uint32_t pulseIn( uint32_t pin, uint32_t state, uint32_t timeout )
{
	// cache the port and bit of the pin in order to speed up the
	// pulse width measuring loop and achieve finer resolution.  calling
	// digitalRead() instead yields much coarser resolution.
	PinDescription p = g_APinDescription[pin];
	uint32_t width = 0; // keep initialization out of time critical area
	
	// convert the timeout from microseconds to a number of times through
	// the initial loop; it takes 22 clock cycles per iteration.
	uint32_t numloops = 0;
	uint32_t maxloops = microsecondsToClockCycles(timeout) / 22;
	
	// wait for any previous pulse to end
	while (PIO_Get(p.pPort, PIO_INPUT, p.ulPin) == state)
		if (numloops++ == maxloops)
			return 0;
	
	// wait for the pulse to start
	while (PIO_Get(p.pPort, PIO_INPUT, p.ulPin) != state)
		if (numloops++ == maxloops)
			return 0;
	
	// wait for the pulse to stop
	while (PIO_Get(p.pPort, PIO_INPUT, p.ulPin) == state) {
		if (numloops++ == maxloops)
			return 0;
		width++;
	}

	// convert the reading to microseconds. The loop has been determined
	// to be 52 clock cycles long and have about 16 clocks between the edge
	// and the start of the loop. There will be some error introduced by
	// the interrupt handlers.
	return clockCyclesToMicroseconds(width * 52 + 16);
}
