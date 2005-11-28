/*
  pins_arduino.c - pin definitions for the Arduino board
  Part of Arduino / Wiring Lite

  Copyright (c) 2005 David A. Mellis

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

  $Id: pins_arduino.c,v 1.4 2005/05/24 17:47:41 mellis Exp $
*/

#include <avr/io.h>
#include "wiring.h"

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATMEL ATMEGA8 / ARDUINO
//
//             +-\/-+
//       PC6  1|    |28  PC5 (AI 0)
// (D 0) PD0  2|    |27  PC4 (AI 1)
// (D 1) PD1  3|    |26  PC3 (AI 2)
// (D 2) PD2  4|    |25  PC2 (AI 3)
// (D 3) PD3  5|    |24  PC1 (AI 4)
// (D 4) PD4  6|    |23  PC0 (AI 5)
//       VCC  7|    |22  GND
//       GND  8|    |21  AREF
//       PB6  9|    |20  AVCC
//       PB7 10|    |19  PB5 (D 13)
// (D 5) PD5 11|    |18  PB4 (D 12)
// (D 6) PD6 12|    |17  PB3 (D 11)
// (D 7) PD7 13|    |16  PB2 (D 10)
// (D 8) PB0 14|    |15  PB1 (D 9)
//             +----+

#define NUM_DIGITAL_PINS 14
#define NUM_ANALOG_OUT_PINS 11
#define NUM_ANALOG_IN_PINS 6
#define NUM_PORTS 4

#define PB 2
#define PC 3
#define PD 4

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
int port_to_mode[NUM_PORTS + 1] = {
	NOT_A_PORT,
	NOT_A_PORT,
	_SFR_IO_ADDR(DDRB),
	_SFR_IO_ADDR(DDRC),
	_SFR_IO_ADDR(DDRD),
};

int port_to_output[NUM_PORTS + 1] = {
	NOT_A_PORT,
	NOT_A_PORT,
	_SFR_IO_ADDR(PORTB),
	_SFR_IO_ADDR(PORTC),
	_SFR_IO_ADDR(PORTD),
};

int port_to_input[NUM_PORTS + 1] = {
	NOT_A_PORT,
	NOT_A_PORT,
	_SFR_IO_ADDR(PINB),
	_SFR_IO_ADDR(PINC),
	_SFR_IO_ADDR(PIND),
};

// these arrays map the pin numbers on the arduino
// board to the atmega8 port and pin numbers
pin_t digital_pin_to_port_array[NUM_DIGITAL_PINS] = {
	{ PD, 0 },
	{ PD, 1 },
	{ PD, 2 },
	{ PD, 3 },
	{ PD, 4 },
	{ PD, 5 },
	{ PD, 6 },
	{ PD, 7 },
	{ PB, 0 },
	{ PB, 1 },
	{ PB, 2 },
	{ PB, 3 },
	{ PB, 4 },
	{ PB, 5 },
};

pin_t *digital_pin_to_port = digital_pin_to_port_array;

// Some of the digital pins also support hardware PWM (analog output).
pin_t analog_out_pin_to_port_array[NUM_DIGITAL_PINS] = {
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ PB, 1 },
	{ PB, 2 },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
};

pin_t *analog_out_pin_to_port = analog_out_pin_to_port_array;

pin_t analog_in_pin_to_port_array[NUM_ANALOG_IN_PINS] = {
	{ PC, 0 },
	{ PC, 1 },
	{ PC, 2 },
	{ PC, 3 },
	{ PC, 4 },
	{ PC, 5 },
};

pin_t *analog_in_pin_to_port = analog_in_pin_to_port_array;
