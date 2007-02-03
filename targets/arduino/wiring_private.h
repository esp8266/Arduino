/*
  wiring_private.h - Internal header file.
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

  $Id: wiring.h 239 2007-01-12 17:58:39Z mellis $
*/

#ifndef WiringPrivate_h
#define WiringPrivate_h

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/delay.h>
#include <stdio.h>
#include <stdarg.h>

#include "wiring.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define NOT_A_PIN 0
#define NOT_A_PORT -1

#define NOT_ON_TIMER -1
#define TIMER0A 0
#define TIMER0B 1
#define TIMER1A 2
#define TIMER1B 3
#define TIMER2  4
#define TIMER2A 5
#define TIMER2B 6

typedef struct {
	int port;
	int bit;
} pin_t;

extern int port_to_mode[];
extern int port_to_input[];
extern int port_to_output[];
extern pin_t *digital_pin_to_port;
extern pin_t *analog_in_pin_to_port;
extern int *analog_out_pin_to_timer;

#define EXTERNAL_INT_0 0
#define EXTERNAL_INT_1 1

#define EXTERNAL_NUM_INTERRUPTS 2

typedef void (*voidFuncPtr)(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
