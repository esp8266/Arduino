/*! \file uartsw2conf.h \brief Interrupt-driven Software UART Driver Configuration. */
//*****************************************************************************
//
// File Name	: 'uartsw2conf.h'
// Title		: Interrupt-driven Software UART Driver Configuration
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 7/20/2002
// Revised		: 4/27/2004
// Version		: 0.6
// Target MCU	: Atmel AVR Series (intended for the ATmega16 and ATmega32)
// Editor Tabs	: 4
//
// Description	:
//		This uart library emulates the operation of a UART (serial port) using
//	the AVR's hardware timers, I/O pins, and some software.
//
//	Specifically, this code uses:
//		-Timer 2 Output Capture for transmit timing
//		-Timer 0 Output Capture for receive timing
//		-External Interrupt 2 for receive triggering
//
//	The above resources cannot be used for other purposes while this software
//	UART is enabled.  The overflow interrupts from Timer0 and Timer2 can still
//	be used for other timing, but the prescalers for these timers must not be
//	changed.
//
//	Serial output from this UART can be routed to any I/O pin.  Serial input
//	for this UART must come from the External Interrupt 2 (INT2) I/O pin.
//	These options should be configured by editing your local copy of
//	"uartsw2conf.h".
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef UARTSW2CONF_H
#define UARTSW2CONF_H

// constants/macros/typdefs

#define UARTSW_RX_BUFFER_SIZE	0x20	///< UART receive buffer size in bytes

#define UARTSW_INVERT					///< define to invert polarity of RX/TX signals
// when non-inverted, the serial line is appropriate for passing though
// an RS232 driver like the MAX232.  When inverted, the serial line can
// directly drive/receive RS232 signals to/from a DB9 connector.  Be sure
// to use a current-limiting resistor and perhaps a diode-clamp circuit when
// connecting incoming RS232 signals to a microprocessor I/O pin.

// if non-inverted, the serial line idles high (logic 1) between bytes
// if inverted, the serial line idles low (logic 0) between bytes


// UART transmit pin defines
#define UARTSW_TX_PORT			PORTB	///< UART Transmit Port
#define UARTSW_TX_DDR			DDRB	///< UART Transmit DDR
#define UARTSW_TX_PIN			PB3		///< UART Transmit Pin

// UART receive pin defines
// This pin must correspond to the
// External Interrupt 2 (INT2) pin for your processor
#define UARTSW_RX_PORT			PORTB	///< UART Receive Port
#define UARTSW_RX_DDR			DDRB	///< UART Receive DDR
#define UARTSW_RX_PORTIN		PINB	///< UART Receive Port Input
#define UARTSW_RX_PIN			PB2		///< UART Receive Pin

#endif
