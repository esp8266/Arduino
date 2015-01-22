/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

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

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#include "../leonardo/pins_arduino.h"

#undef TXLED0
#undef TXLED1
#undef RXLED0
#undef RXLED1
#undef TX_RX_LED_INIT

#define TXLED0			PORTD &= ~(1<<5)
#define TXLED1			PORTD |= (1<<5)
#define RXLED0			PORTB &= ~(1<<0)
#define RXLED1			PORTB |= (1<<0)
#define TX_RX_LED_INIT	DDRD |= (1<<5), DDRB |= (1<<0), TXLED0, RXLED0