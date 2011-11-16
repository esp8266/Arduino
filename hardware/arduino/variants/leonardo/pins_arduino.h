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

#define ARDUINO_MODEL_USB_PID	0x0034

#define TX_RX_LED_INIT	DDRD |= (1<<5), DDRB |= (1<<0)
#define TXLED0			PORTD |= (1<<5)
#define TXLED1			PORTD &= ~(1<<5)
#define RXLED0			PORTB |= (1<<0)
#define RXLED1			PORTB &= ~(1<<0)

const static uint8_t SDA = 2;
const static uint8_t SCL = 3;

// Map SPI port to 'new' pins D14..D17
const static uint8_t SS   = 14;
const static uint8_t MOSI = 16;
const static uint8_t MISO = 17;
const static uint8_t SCK  = 15;

// Mapping of analog pins as digital I/O
// A6-A11 share with digital pins
const static uint8_t A0 = 18;
const static uint8_t A1 = 19;
const static uint8_t A2 = 20;
const static uint8_t A3 = 21;
const static uint8_t A4 = 22;
const static uint8_t A5 = 23;
const static uint8_t A6 = 24;	// D4
const static uint8_t A7 = 25;	// D6
const static uint8_t A8 = 26;	// D8
const static uint8_t A9 = 27;	// D9
const static uint8_t A10 = 28;	// D10
const static uint8_t A11 = 29;	// D12

//	__AVR_ATmega32U4__ has an unusual mapping of pins to channels
extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATMEL ATMEGA32U4 / ARDUINO LEONARDO
//
// D0				PD2					RXD1/INT2
// D1				PD3					TXD1/INT3
// D2				PD1		SDA			SDA/INT1
// D3#				PD0		PWM8/SCL	OC0B/SCL/INT0
// D4		A6		PD4					ADC8
// D5#				PC6		???			OC3A/#OC4A
// D6#		A7		PD7		FastPWM		#OC4D/ADC10
// D7		Ain0	PE6					INT6/AIN0
//
// D8		A11		PB4					ADC11/PCINT4
// D9#		A8		PB5		PWM16		OC1A/#OC4B/ADC12/PCINT5
// D10#		A9		PB6		PWM16		OC1B/0c4B/ADC13/PCINT6
// D11#				PB7		PWM8/16		0C0A/OC1C/#RTS/PCINT7
// D12		A10		PD6					T1/#OC4D/ADC9
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
// TXLED			PD5
// RXLED		    PB0
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
	PC, /* 5 */
	PD, /* 6 */
	PE, /* 7 */
	
	PB, /* 8 */
	PB,
	PB,
	PB,
	
	PD, /* 12 */
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
	_BV(6), /* 5, port C */
	_BV(7),	/* 6, port D */
	_BV(6), /* 7, port E */
	
	_BV(4), /* 8, port B */
	_BV(5),
	_BV(6),
	_BV(7),
	
	_BV(6),	/* 12 port D */
	_BV(7),
	
	_BV(0), /* 14, port B */
	_BV(1),
	_BV(2),
	_BV(3),
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[18] = {
	NOT_ON_TIMER,	
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	TIMER0B,		/* 3 */
	NOT_ON_TIMER,
	TIMER3A,		/* 5 */
	TIMER4D,		/* 6 */
	NOT_ON_TIMER,	
	
	NOT_ON_TIMER,	
	TIMER1A,		/* 9 */
	TIMER1B,		/* 10 */
	TIMER0A,		/* 11 */
	
	NOT_ON_TIMER,	
	TIMER4A,		/* 13 */
	
	NOT_ON_TIMER,	
	NOT_ON_TIMER,
};

const uint8_t PROGMEM analog_pin_to_channel_PGM[12] = {
	7,	// A0				PF7					ADC7
	6,	// A1				PF6					ADC6	
	5,	// A2				PF5					ADC5	
	4,	// A3				PF4					ADC4
	1,	// A4				PF1					ADC1	
	0,	// A5				PF0					ADC0	
	8,	// A6		D4		PD4					ADC8
	10,	// A7		D6		PD7					ADC10
	11,	// A8		D8		PB4					ADC11
	12,	// A9		D9		PB5					ADC12
	13,	// A10		D10		PB6					ADC13
	9	// A11		D12		PD6					ADC9
};

#endif /* ARDUINO_MAIN */
#endif /* Pins_Arduino_h */
