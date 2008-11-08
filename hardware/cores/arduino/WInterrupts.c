/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Wiring project - http://wiring.uniandes.edu.co

  Copyright (c) 2004-05 Hernando Barragan

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
  
  Modified 24 November 2006 by David A. Mellis
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "WConstants.h"
#include "wiring_private.h"

volatile static voidFuncPtr intFunc[EXTERNAL_NUM_INTERRUPTS];
// volatile static voidFuncPtr twiIntFunc;

#if defined(__AVR_ATmega8__)
#define EICRA MCUCR
#define EIMSK GICR
#endif

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) {
  if(interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    intFunc[interruptNum] = userFunc;
    
    if (interruptNum == 0) {
      // Configure the interrupt mode (trigger on low input, any change, rising
      // edge, or falling edge).  The mode constants were chosen to correspond
      // to the configuration bits in the hardware register, so we simply shift
      // the mode into place.
      EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      
      // Enable the interrupt.
      EIMSK |= (1 << INT0);
    } else {
      EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
      EIMSK |= (1 << INT1);
    }
  }
}

void detachInterrupt(uint8_t interruptNum) {
  if(interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    if (interruptNum == 0)
      // Disable the interrupt.
      EIMSK &= ~(1 << INT0);
    else
      EIMSK &= ~(1 << INT1);
      
    intFunc[interruptNum] = 0;
  }
}

/*
void attachInterruptTwi(void (*userFunc)(void) ) {
  twiIntFunc = userFunc;
}
*/

SIGNAL(INT0_vect) {
  if(intFunc[EXTERNAL_INT_0])
    intFunc[EXTERNAL_INT_0]();
}

SIGNAL(INT1_vect) {
  if(intFunc[EXTERNAL_INT_1])
    intFunc[EXTERNAL_INT_1]();
}

/*
SIGNAL(SIG_2WIRE_SERIAL) {
  if(twiIntFunc)
    twiIntFunc();
}
*/

