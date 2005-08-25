/*! \file lcdconf.h \brief Character LCD driver configuration. */
//*****************************************************************************
//
// File Name	: 'lcdconf.h'
// Title		: Character LCD driver for HD44780/SED1278 displays
//					(usable in mem-mapped, or I/O mode)
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 4/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef LCDCONF_H
#define LCDCONF_H

// Define type of interface used to access the LCD
//	LCD_MEMORY_INTERFACE:
//		To use this mode you must supply the necessary hardware to connect the
//		LCD to the CPU's memory bus.  The CONTROL and DATA registers of the LCD
//		(HD44780 chip) must appear in the CPU's memory map.  This mode is faster
//		than the port interface but requires a little extra hardware to make it
//		work.  It is especially useful when your CPU is already configured to
//		use an external memory bus for other purposes (like accessing memory).
//
// LCD_PORT_INTERFACE:
//		This mode allows you to connect the control and data lines of the LCD
//		directly to the I/O port pins (no interfacing hardware is needed),
//		but it generally runs slower than the LCD_MEMORY_INTERFACE.
//		Depending on your needs, when using the LCD_PORT_INTERFACE, the LCD may
//		be accessed in 8-bit or 4-bit mode.  In 8-bit mode, one whole I/O port
//		(pins 0-7) is required for the LCD data lines, but transfers are faster.
//		In 4-bit mode, only I/O port pins 4-7 are needed for data lines, but LCD
//		access is slower.  In either mode, three additional port pins are
//		required for the LCD interface control lines (RS, R/W, and E).

// Enable one of the following interfaces to your LCD
//#define LCD_MEMORY_INTERFACE
#define LCD_PORT_INTERFACE

// Enter the parameters for your chosen interface'
// if you chose the LCD_PORT_INTERFACE:
#ifdef LCD_PORT_INTERFACE
	#ifndef LCD_CTRL_PORT
		// port and pins you will use for control lines
		#define LCD_CTRL_PORT	PORTC
		#define LCD_CTRL_DDR	DDRC
		#define LCD_CTRL_RS		2
		#define LCD_CTRL_RW		3
		#define LCD_CTRL_E		4
	#endif
	#ifndef LCD_DATA_POUT
		// port you will use for data lines
		#define LCD_DATA_POUT	PORTA
		#define LCD_DATA_PIN	PINA
		#define LCD_DATA_DDR	DDRA
		// access mode you will use (default is 8bit unless 4bit is selected)
		//#define LCD_DATA_4BIT
	#endif
#endif

// if you chose the LCD_MEMORY_INTERFACE:
#ifdef LCD_MEMORY_INTERFACE
	#ifndef LCD_CTRL_ADDR
		// CPU memory address of the LCD control register
		#define LCD_CTRL_ADDR	0x1000
	#endif
	#ifndef LCD_DATA_ADDR
		// CPU memory address of the LCD data register
		#define LCD_DATA_ADDR	0x1001
	#endif
#endif


// LCD display geometry
// change these definitions to adapt settings
#define LCD_LINES				4	// visible lines
#define LCD_LINE_LENGTH			20	// line length (in characters)
// cursor position to DDRAM mapping
#define LCD_LINE0_DDRAMADDR		0x00
#define LCD_LINE1_DDRAMADDR		0x40
#define LCD_LINE2_DDRAMADDR		0x14
#define LCD_LINE3_DDRAMADDR		0x54

// LCD delay
// This delay affects how quickly accesses are made to the LCD controller.
// If your clock frequency is low, you can reduce the number of NOPs in the
// delay.  If your clock frequency is high, you may need to add NOPs.
// The number of NOPs should be between at least 1 and up to 20.
#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n nop\n");

#endif
