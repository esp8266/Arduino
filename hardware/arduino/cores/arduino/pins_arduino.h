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

#define NOT_A_PIN 0
#define NOT_A_PORT 0

#define NOT_ON_TIMER 0
#define TIMER0A 1
#define TIMER0B 2
#define TIMER1A 3
#define TIMER1B 4
#define TIMER2  5
#define TIMER2A 6
#define TIMER2B 7

#define TIMER3A 8
#define TIMER3B 9
#define TIMER3C 10
#define TIMER4A 11
#define TIMER4B 12
#define TIMER4C 13
#define TIMER5A 14
#define TIMER5B 15
#define TIMER5C 16

#ifndef INLINED
#define INLINED static __attribute__((always_inline)) inline
#endif

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
const static uint8_t SS   = 53;
const static uint8_t MOSI = 51;
const static uint8_t MISO = 50;
const static uint8_t SCK  = 52;
#else
const static uint8_t SS   = 10;
const static uint8_t MOSI = 11;
const static uint8_t MISO = 12;
const static uint8_t SCK  = 13;
#endif

// On the ATmega1280, the addresses of some of the port registers are
// greater than 255, so we can't store them in uint8_t's.
extern const uint16_t PROGMEM port_to_mode_PGM[];
extern const uint16_t PROGMEM port_to_input_PGM[];
extern const uint16_t PROGMEM port_to_output_PGM[];

extern const uint8_t PROGMEM digital_pin_to_port_PGM[];
// extern const uint8_t PROGMEM digital_pin_to_bit_PGM[];
extern const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[];
extern const uint8_t PROGMEM digital_pin_to_timer_PGM[];

// inlined versions of lookup-table-based pin mappings. Don't use directly.

// Don't use PA, so on, might clash with sketch

#define PORT_INDEX_PA 1
#define PORT_INDEX_PB 2
#define PORT_INDEX_PC 3
#define PORT_INDEX_PD 4
#define PORT_INDEX_PE 5
#define PORT_INDEX_PF 6
#define PORT_INDEX_PG 7
#define PORT_INDEX_PH 8
#define PORT_INDEX_PJ 10
#define PORT_INDEX_PK 11
#define PORT_INDEX_PL 12

__attribute__((error("Invalid pin specified. This pin does not map to any I/O port"))) static void invalidPinSpecified(void);


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

INLINED volatile uint8_t *inlined_portModeRegister(uint8_t port_index)
{
	switch (port_index) {
	case 1: return &DDRA;
	case 2: return &DDRB;
	case 3: return &DDRC;
	case 4: return &DDRD;
	case 5: return &DDRE;
	case 6: return &DDRF;
	case 7: return &DDRG;
	case 8: return &DDRH;
	case 10: return &DDRJ;
	case 11: return &DDRK;
	case 12: return &DDRL;
	default: return NOT_A_PORT;
	}
}

INLINED volatile uint8_t *inlined_portOutputRegister(uint8_t port_index)
{
	switch (port_index) {
	case 1: return &PORTA;
	case 2: return &PORTB;
	case 3: return &PORTC;
	case 4: return &PORTD;
	case 5: return &PORTE;
	case 6: return &PORTF;
	case 7: return &PORTG;
	case 8: return &PORTH;
	case 10: return &PORTJ;
	case 11: return &PORTK;
	case 12: return &PORTL;
	default: return NOT_A_PORT;
	}
}

INLINED volatile uint8_t *inlined_portInputRegister(uint8_t port_index)
{
	switch (port_index) {
	case 1: return &PINA;
	case 2: return &PINB;
	case 3: return &PINC;
	case 4: return &PIND;
	case 5: return &PINE;
	case 6: return &PINF;
	case 7: return &PING;
	case 8: return &PINH;
	case 10: return &PINJ;
	case 11: return &PINK;
	case 12: return &PINL;
	default: return NOT_A_PIN;
	}
};

INLINED uint8_t inlined_digitalPinToPort(uint8_t pin)
{
	switch (pin) {
	case 0:  // PE 0 ** 0 ** USART0_RX
	case 1:  // PE 1 ** 1 ** USART0_TX
	case 2:  // PE 4 ** 2 ** PWM2
	case 3:  // PE 5 ** 3 ** PWM3
		return PORT_INDEX_PE;

	case 4:  // PG 5 ** 4 ** PWM4
		return PORT_INDEX_PG;

	case 5:  // PE 3 ** 5 ** PWM5
		return PORT_INDEX_PE;

	case 6:  // PH 3 ** 6 ** PWM6
	case 7:  // PH 4 ** 7 ** PWM7
	case 8:  // PH 5 ** 8 ** PWM8
	case 9:  // PH 6 ** 9 ** PWM9
		return PORT_INDEX_PH;

	case 10: // PB 4 ** 10 ** PWM10
	case 11: // PB 5 ** 11 ** PWM11
	case 12: // PB 6 ** 12 ** PWM12
	case 13: // PB 7 ** 13 ** PWM13
		return PORT_INDEX_PB;

	case 14: // PJ 1 ** 14 ** USART3_TX
	case 15: // PJ 0 ** 15 ** USART3_RX
		return PORT_INDEX_PJ;

	case 16: // PH 1 ** 16 ** USART2_TX
	case 17: // PH 0 ** 17 ** USART2_RX
		return PORT_INDEX_PH;

	case 18: // PD 3 ** 18 ** USART1_TX
	case 19: // PD 2 ** 19 ** USART1_RX
	case 20: // PD 1 ** 20 ** I2C_SDA
	case 21: // PD 0 ** 21 ** I2C_SCL
		return PORT_INDEX_PD;

	case 22: // PA 0 ** 22 ** D22
	case 23: // PA 1 ** 23 ** D23
	case 24: // PA 2 ** 24 ** D24
	case 25: // PA 3 ** 25 ** D25
	case 26: // PA 4 ** 26 ** D26
	case 27: // PA 5 ** 27 ** D27
	case 28: // PA 6 ** 28 ** D28
	case 29: // PA 7 ** 29 ** D29
		return PORT_INDEX_PA;

	case 30: // PC 7 ** 30 ** D30
	case 31: // PC 6 ** 31 ** D31
	case 32: // PC 5 ** 32 ** D32
	case 33: // PC 4 ** 33 ** D33
	case 34: // PC 3 ** 34 ** D34
	case 35: // PC 2 ** 35 ** D35
	case 36: // PC 1 ** 36 ** D36
	case 37: // PC 0 ** 37 ** D37
		return PORT_INDEX_PC;

	case 38: // PD 7 ** 38 ** D38
		return PORT_INDEX_PD;

	case 39: // PG 2 ** 39 ** D39
	case 40: // PG 1 ** 40 ** D40
	case 41: // PG 0 ** 41 ** D41
		return PORT_INDEX_PG;

	case 42: // PL 7 ** 42 ** D42
	case 43: // PL 6 ** 43 ** D43
	case 44: // PL 5 ** 44 ** D44
	case 45: // PL 4 ** 45 ** D45
	case 46: // PL 3 ** 46 ** D46
	case 47: // PL 2 ** 47 ** D47
	case 48: // PL 1 ** 48 ** D48
	case 49: // PL 0 ** 49 ** D49
		return PORT_INDEX_PL;

	case 50: // PB 3 ** 50 ** SPI_MISO
	case 51: // PB 2 ** 51 ** SPI_MOSI
	case 52: // PB 1 ** 52 ** SPI_SCK
	case 53: // PB 0 ** 53 ** SPI_SS
		return PORT_INDEX_PB;

	case 54: // PF 0 ** 54 ** A0
	case 55: // PF 1 ** 55 ** A1
	case 56: // PF 2 ** 56 ** A2
	case 57: // PF 3 ** 57 ** A3
	case 58: // PF 4 ** 58 ** A4
	case 59: // PF 5 ** 59 ** A5
	case 60: // PF 6 ** 60 ** A6
	case 61: // PF 7 ** 61 ** A7
		return PORT_INDEX_PF;

	case 62: // PK 0 ** 62 ** A8
	case 63: // PK 1 ** 63 ** A9
	case 64: // PK 2 ** 64 ** A10
	case 65: // PK 3 ** 65 ** A11
	case 66: // PK 4 ** 66 ** A12
	case 67: // PK 5 ** 67 ** A13
	case 68: // PK 6 ** 68 ** A14
	case 69: // PK 7 ** 69 ** A15
		return PORT_INDEX_PK;
	default:
		invalidPinSpecified();
	}
}

INLINED uint8_t inlined_digitalPinToBitMask(uint8_t pin)
{
	switch(pin) {
	case  0: return _BV( 0 ); // PE 0 ** 0 ** USART0_RX
	case  1: return _BV( 1 ); // PE 1 ** 1 ** USART0_TX
	case  2: return _BV( 4 ); // PE 4 ** 2 ** PWM2
	case  3: return _BV( 5 ); // PE 5 ** 3 ** PWM3
	case  4: return _BV( 5 ); // PG 5 ** 4 ** PWM4
	case  5: return _BV( 3 ); // PE 3 ** 5 ** PWM5
	case  6: return _BV( 3 ); // PH 3 ** 6 ** PWM6
	case  7: return _BV( 4 ); // PH 4 ** 7 ** PWM7
	case  8: return _BV( 5 ); // PH 5 ** 8 ** PWM8
	case  9: return _BV( 6 ); // PH 6 ** 9 ** PWM9
	case 10: return _BV( 4 ); // PB 4 ** 10 ** PWM10
	case 11: return _BV( 5 ); // PB 5 ** 11 ** PWM11
	case 12: return _BV( 6 ); // PB 6 ** 12 ** PWM12
	case 13: return _BV( 7 ); // PB 7 ** 13 ** PWM13
	case 14: return _BV( 1 ); // PJ 1 ** 14 ** USART3_TX
	case 15: return _BV( 0 ); // PJ 0 ** 15 ** USART3_RX
	case 16: return _BV( 1 ); // PH 1 ** 16 ** USART2_TX
	case 17: return _BV( 0 ); // PH 0 ** 17 ** USART2_RX
	case 18: return _BV( 3 ); // PD 3 ** 18 ** USART1_TX
	case 19: return _BV( 2 ); // PD 2 ** 19 ** USART1_RX
	case 20: return _BV( 1 ); // PD 1 ** 20 ** I2C_SDA
	case 21: return _BV( 0 ); // PD 0 ** 21 ** I2C_SCL
	case 22: return _BV( 0 ); // PA 0 ** 22 ** D22
	case 23: return _BV( 1 ); // PA 1 ** 23 ** D23
	case 24: return _BV( 2 ); // PA 2 ** 24 ** D24
	case 25: return _BV( 3 ); // PA 3 ** 25 ** D25
	case 26: return _BV( 4 ); // PA 4 ** 26 ** D26
	case 27: return _BV( 5 ); // PA 5 ** 27 ** D27
	case 28: return _BV( 6 ); // PA 6 ** 28 ** D28
	case 29: return _BV( 7 ); // PA 7 ** 29 ** D29
	case 30: return _BV( 7 ); // PC 7 ** 30 ** D30
	case 31: return _BV( 6 ); // PC 6 ** 31 ** D31
	case 32: return _BV( 5 ); // PC 5 ** 32 ** D32
	case 33: return _BV( 4 ); // PC 4 ** 33 ** D33
	case 34: return _BV( 3 ); // PC 3 ** 34 ** D34
	case 35: return _BV( 2 ); // PC 2 ** 35 ** D35
	case 36: return _BV( 1 ); // PC 1 ** 36 ** D36
	case 37: return _BV( 0 ); // PC 0 ** 37 ** D37
	case 38: return _BV( 7 ); // PD 7 ** 38 ** D38
	case 39: return _BV( 2 ); // PG 2 ** 39 ** D39
	case 40: return _BV( 1 ); // PG 1 ** 40 ** D40
	case 41: return _BV( 0 ); // PG 0 ** 41 ** D41
	case 42: return _BV( 7 ); // PL 7 ** 42 ** D42
	case 43: return _BV( 6 ); // PL 6 ** 43 ** D43
	case 44: return _BV( 5 ); // PL 5 ** 44 ** D44
	case 45: return _BV( 4 ); // PL 4 ** 45 ** D45
	case 46: return _BV( 3 ); // PL 3 ** 46 ** D46
	case 47: return _BV( 2 ); // PL 2 ** 47 ** D47
	case 48: return _BV( 1 ); // PL 1 ** 48 ** D48
	case 49: return _BV( 0 ); // PL 0 ** 49 ** D49
	case 50: return _BV( 3 ); // PB 3 ** 50 ** SPI_MISO
	case 51: return _BV( 2 ); // PB 2 ** 51 ** SPI_MOSI
	case 52: return _BV( 1 ); // PB 1 ** 52 ** SPI_SCK
	case 53: return _BV( 0 ); // PB 0 ** 53 ** SPI_SS
	case 54: return _BV( 0 ); // PF 0 ** 54 ** A0
	case 55: return _BV( 1 ); // PF 1 ** 55 ** A1
	case 56: return _BV( 2 ); // PF 2 ** 56 ** A2
	case 57: return _BV( 3 ); // PF 3 ** 57 ** A3
	case 58: return _BV( 4 ); // PF 4 ** 58 ** A4
	case 59: return _BV( 5 ); // PF 5 ** 59 ** A5
	case 60: return _BV( 6 ); // PF 6 ** 60 ** A6
	case 61: return _BV( 7 ); // PF 7 ** 61 ** A7
	case 62: return _BV( 0 ); // PK 0 ** 62 ** A8
	case 63: return _BV( 1 ); // PK 1 ** 63 ** A9
	case 64: return _BV( 2 ); // PK 2 ** 64 ** A10
	case 65: return _BV( 3 ); // PK 3 ** 65 ** A11
	case 66: return _BV( 4 ); // PK 4 ** 66 ** A12
	case 67: return _BV( 5 ); // PK 5 ** 67 ** A13
	case 68: return _BV( 6 ); // PK 6 ** 68 ** A14
	case 69: return _BV( 7 ); // PK 7 ** 69 ** A15
	default:
		// TODO: add error here
		invalidPinSpecified();
	}
}

// XXX: this needs to return false (or -1) if the pin doesn't have a timer,
// rather than throwing a compilation error.
INLINED uint8_t inlined_digitalPinToTimer(uint8_t pin)
{
	switch(pin) {
	case  2: return TIMER3B; // PE 4 ** 2 ** PWM2
	case  3: return TIMER3C; // PE 5 ** 3 ** PWM3
	case  4: return TIMER0B; // PG 5 ** 4 ** PWM4
	case  5: return TIMER3A; // PE 3 ** 5 ** PWM5
	case  6: return TIMER4A; // PH 3 ** 6 ** PWM6
	case  7: return TIMER4B; // PH 4 ** 7 ** PWM7
	case  8: return TIMER4C; // PH 5 ** 8 ** PWM8
	case  9: return TIMER2B; // PH 6 ** 9 ** PWM9
	case 10: return TIMER2A; // PB 4 ** 10 ** PWM10
	case 11: return TIMER1A; // PB 5 ** 11 ** PWM11
	case 12: return TIMER1B; // PB 6 ** 12 ** PWM12
	case 13: return TIMER0A; // PB 7 ** 13 ** PWM13
	case 44: return TIMER5C; // PL 5 ** 44 ** D44
	case 45: return TIMER5B; // PL 4 ** 45 ** D45
	case 46: return TIMER5A; // PL 3 ** 46 ** D46
	default: invalidPinSpecified();
	}
}

#else // defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

INLINED volatile uint8_t *inlined_portModeRegister(uint8_t port_index)
{
	switch (port_index) {
	case 2: return &DDRB;
	case 3: return &DDRC;
	case 4: return &DDRD;
	default: invalidPinSpecified();
	}
}

INLINED volatile uint8_t *inlined_portOutputRegister(uint8_t port_index)
{
	switch (port_index) {
	case 2: return &PORTB;
	case 3: return &PORTC;
	case 4: return &PORTD;
	default: invalidPinSpecified();
	}
}

INLINED volatile uint8_t *inlined_portInputRegister(uint8_t port_index)
{
	switch (port_index) {
	case 2: return &PINB;
	case 3: return &PINC;
	case 4: return &PIND;
	default: invalidPinSpecified();
	}
}

INLINED uint8_t inlined_digitalPinToPort(uint8_t pin)
{
	switch(pin) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		return PORT_INDEX_PD;
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		return PORT_INDEX_PB;
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
		return PORT_INDEX_PC;
	default:
		invalidPinSpecified();
	}
}

INLINED uint8_t inlined_digitalPinToBitMask(uint8_t pin)
{
	switch(pin) {
	case 0: return _BV(0); /* 0, port D */
	case 1: return _BV(1);
	case 2: return _BV(2);
	case 3: return _BV(3);
	case 4: return _BV(4);
	case 5: return _BV(5);
	case 6: return _BV(6);
	case 7: return _BV(7);
	case 8: return _BV(0); /* 8, port B */
	case 9: return _BV(1);
	case 10: return _BV(2);
	case 11: return _BV(3);
	case 12: return _BV(4);
	case 13: return _BV(5);
	case 14: return _BV(0); /* 14, port C */
	case 15: return _BV(1);
	case 16: return _BV(2);
	case 17: return _BV(3);
	case 18: return _BV(4);
	case 19: return _BV(5);
	default:
		// TODO: throw error here
		invalidPinSpecified();
	}
}

// XXX: this needs to return false (or -1) if the pin doesn't have a timer,
// rather than throwing a compilation error.
INLINED uint8_t inlined_digitalPinToTimer(uint8_t pin)
{
	switch(pin) {
#if defined(__AVR_ATmega8__)
	case 11: return TIMER2;
#else
	case  3: return TIMER2B;
	case  5: return TIMER0B;
	case  6: return TIMER0A;
	case 11: return TIMER2A;
#endif
	case  9: return TIMER1A;
	case 10: return TIMER1B;
	default: invalidPinSpecified();
	}
}

#endif // defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

#define analogInPinToBit(P) (P)

#endif
