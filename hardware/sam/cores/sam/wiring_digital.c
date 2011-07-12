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

  Modified 28 September 2010 by Mark Sproul

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include "wiring_private.h"
//#include "pins_arduino.h"

/// \brief Configures the specified pin to behave either as an input or an output. See the description of digital pins for details.
///
/// \param dwPin the number of the pin whose mode you wish to set 
/// \param dwMode either INPUT or OUTPUT
///
void pinMode( uint32_t dwPin, uint32_t dwMode )
{
	if ( APinDescription[dwPin].dwPinType == PIO_NOT_A_PIN )
  { 
    return ;
  }

	switch ( dwMode )
  { 
    case INPUT:
      PIO_Configure( APinDescription[dwPin].pPort, PIO_INPUT, APinDescription[dwPin].dwPin, 0 ) ;
    break ;

    case OUTPUT:
      PIO_Configure( APinDescription[dwPin].pPort, PIO_OUTPUT_1, APinDescription[dwPin].dwPin, APinDescription[dwPin].dwPinAttribute ) ;
    break ;

    default:
    break ;
	}
}

/// \brief Write a HIGH or a LOW value to a digital pin. 
///
/// \desc If the pin has been configured as an OUTPUT with pinMode(), its voltage will be set to the 
/// corresponding value: 5V (or 3.3V on 3.3V boards) for HIGH, 0V (ground) for LOW.
///
/// If the pin is configured as an INPUT, writing a HIGH value with digitalWrite() will enable an internal
/// 20K pullup resistor (see the tutorial on digital pins). Writing LOW will disable the pullup. The pullup 
/// resistor is enough to light an LED dimly, so if LEDs appear to work, but very dimly, this is a likely 
/// cause. The remedy is to set the pin to an output with the pinMode() function.
///
/// NOTE: Digital pin PIN_LED is harder to use as a digital input than the other digital pins because it has an LED
/// and resistor attached to it that's soldered to the board on most boards. If you enable its internal 20k pull-up
/// resistor, it will hang at around 1.7 V instead of the expected 5V because the onboard LED and series resistor 
/// pull the voltage level down, meaning it always returns LOW. If you must use pin PIN_LED as a digital input, use an 
/// external pull down resistor. 
///
/// \param dwPin the pin number
/// \param dwVal HIGH or LOW
///
void digitalWrite( uint32_t dwPin, uint32_t dwVal )
{
	if ( APinDescription[dwPin].dwPinType == PIO_NOT_A_PIN )
  { 
    return ;
  }

  PIO_SetOutput( APinDescription[dwPin].pPort, APinDescription[dwPin].dwPin, dwVal, 0, PIO_PULLUP ) ;
}

/// \brief Reads the value from a specified digital pin, either HIGH or LOW. 
///
/// \param dwPin the number of the digital pin you want to read (int) 
///
int digitalRead( uint32_t dwPin )
{
	if ( APinDescription[dwPin].dwPinType == PIO_NOT_A_PIN )
  { 
    return LOW ;
  }

	if ( PIO_Get( APinDescription[dwPin].pPort, PIO_INPUT, APinDescription[dwPin].dwPin ) == 1 )
  {
    return HIGH ;
  }

	return LOW ;
}
