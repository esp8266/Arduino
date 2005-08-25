/*! \file megaioreg.h \brief MegaIO register definitions. */
//*****************************************************************************
//
// File Name	: 'megaioreg.h'
// Title		: MegaIO register definitions
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.07.16
// Revised		: 2003.07.17
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef MEGAIOREG_H
#define MEGAIOREG_H

// define MEGAIO I2C address
#define MEGAIO_I2C_ADDR				0x4C

// define MEGAIO registers
// General Registers
#define MEGAIOREG_IDSTRING			0x00

// UART Registers
#define MEGAIOREG_UARTDATA			0x10
#define MEGAIOREG_UARTBAUD			0x14
#define MEGAIOREG_UARTBAUDSEL		0x15
#define MEGAIOREG_UARTRXBUFBYTES	0x18
#define MEGAIOREG_UARTTXBUFBYTES	0x19

// PWM Registers
#define MEGAIOREG_PWM1CTRL			0x20
#define MEGAIOREG_PWM1FREQ			0x21
#define MEGAIOREG_PWM1ADUTY			0x24
#define MEGAIOREG_PWM1BDUTY			0x25

// A/D Converter Registers
#define MEGAIOREG_ADCCTRL			0x30
#define MEGAIOREG_ADCCHSEL			0x31
#define MEGAIOREG_ADCRESULT			0x32

// PORT Access Registers
#define MEGAIOREG_PORTA				0x40
#define MEGAIOREG_DDRA				0x41
#define MEGAIOREG_PINA				0x42
#define MEGAIOREG_PORTB				0x43
#define MEGAIOREG_DDRB				0x44
#define MEGAIOREG_PINB				0x45
#define MEGAIOREG_PORTC				0x46
#define MEGAIOREG_DDRC				0x47
#define MEGAIOREG_PINC				0x48
#define MEGAIOREG_PORTD				0x49
#define MEGAIOREG_DDRD				0x4A
#define MEGAIOREG_PIND				0x4B
#define MEGAIOREG_PORTE				0x4C
#define MEGAIOREG_DDRE				0x4D
#define MEGAIOREG_PINE				0x4E
#define MEGAIOREG_PORTF				0x4F
#define MEGAIOREG_DDRF				0x50
#define MEGAIOREG_PINF				0x51

// Direct Access Registers
#define MEGAIOREG_DIRECTIO			0x80
#define MEGAIOREG_DIRECTMEM			0x81

// define MEGAIO register values
#define UARTBAUDSEL_300				0x00
#define UARTBAUDSEL_600				0x01
#define UARTBAUDSEL_1200			0x02
#define UARTBAUDSEL_2400			0x03
#define UARTBAUDSEL_4800			0x04
#define UARTBAUDSEL_9600			0x05
#define UARTBAUDSEL_19200			0x06
#define UARTBAUDSEL_38400			0x07
#define UARTBAUDSEL_115200			0x08

#define PWM1FREQ_STOP				0x00
#define PWM1FREQ_MAX				0x01
#define PWM1FREQ_DIV8				0x02
#define PWM1FREQ_DIV64				0x03
#define PWM1FREQ_DIV256				0x04
#define PWM1FREQ_DIV1024			0x05

#endif
