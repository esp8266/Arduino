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

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

// Map SPI port to 'new' pins D14..D17
// D14				PB0					RXLED,SS/PCINT0
// D15				PB1					SCK,PCINT1
// D16				PB2					MOSI,PCINT2
// D17				PB3					MISO,PCINT3
const static uint8_t SS   = 14;
const static uint8_t MOSI = 16;
const static uint8_t MISO = 17;
const static uint8_t SCK  = 15;

const static uint8_t A0 = 14;
const static uint8_t A1 = 15;
const static uint8_t A2 = 16;
const static uint8_t A3 = 17;
const static uint8_t A4 = 18;
const static uint8_t A5 = 19;
const static uint8_t A6 = 20;
const static uint8_t A7 = 21;

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATMEL ATMEGA8 & 168 / ARDUINO
//
//                  +-\/-+
//            PC6  1|    |28  PC5 (AI 5)
//      (D 0) PD0  2|    |27  PC4 (AI 4)
//      (D 1) PD1  3|    |26  PC3 (AI 3)
//      (D 2) PD2  4|    |25  PC2 (AI 2)
// PWM+ (D 3) PD3  5|    |24  PC1 (AI 1)
//      (D 4) PD4  6|    |23  PC0 (AI 0)
//            VCC  7|    |22  GND
//            GND  8|    |21  AREF
//            PB6  9|    |20  AVCC
//            PB7 10|    |19  PB5 (D 13)
// PWM+ (D 5) PD5 11|    |18  PB4 (D 12)
// PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM
//      (D 7) PD7 13|    |16  PB2 (D 10) PWM
//      (D 8) PB0 14|    |15  PB1 (D 9) PWM
//                  +----+
//
// (PWM+ indicates the additional PWM pins on the ATmega168.)

// ATMEL ATMEGA1280 / ARDUINO
//
// 0-7 PE0-PE7   works
// 8-13 PB0-PB5  works
// 14-21 PA0-PA7 works 
// 22-29 PH0-PH7 works
// 30-35 PG5-PG0 works
// 36-43 PC7-PC0 works
// 44-51 PJ7-PJ0 works
// 52-59 PL7-PL0 works
// 60-67 PD7-PD0 works
// A0-A7 PF0-PF7
// A8-A15 PK0-PK7

// ATMEL ATMEGA32U4 / ARDUINO LEONARDO
//
// D0				PD2					RXD1/INT2
// D1				PD3					TXD1/INT3
// D2				PD1		SDA			SDA/INT1
// D3#				PD0		PWM8/SCL	OC0B/SCL/INT0
// D4		A6		PD4					ADC8
// D5#		A7		PD6		FastPWM		OC4D/ADC9
// D6#		A8		PD7		FastPWM		#OC4D/ADC10
// D7				PD5					#CTS
//
// D8				PB4					ADC11/PCINT4
// D9#		A9		PB5		PWM16		OC1A/#OC4B/ADC13/PCINT5
// D10#		A10		PB6		PWM16		OC1B/0c4B/ADC12/PCINT6
// D11#				PB7		PWM8/16		0C0A/OC1C/#RTS/PCINT7
// D12#				PC6		PWM16		0C3A/#OC4A
// D13#				PC7		PWM10		CLK0/OC4A
//
//			A0		PF7					ADC7
//			A1		PF6					ADC6
//			A2		PF5					ADC5
//			A3		PF4					ADC4
//			A4		PF1					ADC1
//			A5		PF0					ADC0
//
// New pins D14..D17 to map SPI port to digitial pins
//
// D14				PB0					RXLED,SS/PCINT0
// D15				PB1					SCK,PCINT1
// D16				PB2					MOSI,PCINT2
// D17				PB3					MISO,PCINT3
//
// TXLED			PE6					INT6
// HWB				PE2					HWB


// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
	(uint16_t) &DDRE,
	(uint16_t) &DDRF,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
	(uint16_t) &PORTE,
	(uint16_t) &PORTF,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
	(uint16_t) &PINE,
	(uint16_t) &PINF,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[18] = {
	PD, /* 0 */
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	
	PB, /* 8 */
	PB,
	PB,
	PB,
	
	PC, /* 12 */
	PC, /* 13 */
	
	PB,	/* 14 */
	PB,	/* 15 */
	PB,	/* 16 */
	PB,	/* 17 */
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[18] = {
	_BV(2), /* 0, port D */
	_BV(3),
	_BV(1),
	_BV(0),
	_BV(4),
	_BV(6),
	_BV(7),
	_BV(5),
	
	_BV(4), /* 8, port B */
	_BV(5),
	_BV(6),
	_BV(7),
	
	_BV(6),	/* 12 port C */
	_BV(7),
	
	_BV(0), /* 14, port B */
	_BV(1),
	_BV(2),
	_BV(3),
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[18] = {
	NOT_ON_TIMER,	/* 0, port D */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	TIMER0B,
	NOT_ON_TIMER,
	NOT_ON_TIMER,	// TIMER4D		TODO - ZE - Fix this when there's a decision on what to do about these PWM pins
	NOT_ON_TIMER,	// TIMER4D_NOT	TODO - ZE - Complementary output of TIMER4D on Digital Pin 6. Not sure this was intended.
	NOT_ON_TIMER,
	
	NOT_ON_TIMER,	/* 8 port B */
	TIMER1A,
	TIMER1B,
	TIMER0A,
	
	TIMER3A,		/* 12 port C */
	TIMER4A,
	
	NOT_ON_TIMER,	/* 14 port B */
	NOT_ON_TIMER,
};

#endif

#endif