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

#ifdef __cplusplus
 extern "C" {
#endif

extern void pinMode( uint32_t dwPin, uint32_t dwMode )
{
	if ( APinDescription[dwPin].dwPinType == PIO_NOT_A_PIN )
    {
        return ;
    }

	switch ( dwMode )
    {
        case INPUT:
            /* Enable peripheral for clocking input */
            PMC_EnablePeripheral( APinDescription[dwPin].dwPeripheralId ) ;
            PIO_Configure( APinDescription[dwPin].pPort, PIO_INPUT, APinDescription[dwPin].dwPin, 0 ) ;
        break ;

        case OUTPUT:
            /* if all pins are output, disable PIO Controller clocking, reduce power consomption */
            if ( APinDescription[dwPin].pPort->PIO_OSR == 0xffffffff )
            {
                PMC_DisablePeripheral( APinDescription[dwPin].dwPeripheralId ) ;
            }
            PIO_Configure( APinDescription[dwPin].pPort, PIO_OUTPUT_1, APinDescription[dwPin].dwPin, APinDescription[dwPin].dwPinAttribute ) ;
        break ;

        default:
        break ;
    }
}

extern void digitalWrite( uint32_t dwPin, uint32_t dwVal )
{
	if ( APinDescription[dwPin].dwPinType == PIO_NOT_A_PIN )
    {
        return ;
    }

  PIO_SetOutput( APinDescription[dwPin].pPort, APinDescription[dwPin].dwPin, dwVal, 0, PIO_PULLUP ) ;
}

extern int digitalRead( uint32_t dwPin )
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

#ifdef __cplusplus
}
#endif

