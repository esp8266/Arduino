/*! \file uartsw2.h \brief Interrupt-driven Software UART Driver. */
//*****************************************************************************
//
// File Name	: 'uartsw2.h'
// Title		: Interrupt-driven Software UART Driver
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

#ifndef UARTSW2_H
#define UARTSW2_H

#include "global.h"
#include "buffer.h"

// include configuration
#include "uartsw2conf.h"

// constants/macros/typdefs

// functions

//! enable and initialize the software uart
void uartswInit(void);
//! create and initialize the uart buffers
void uartswInitBuffers(void);
//! turns off software UART
void uartswOff(void);
//! returns the receive buffer structure 
cBuffer* uartswGetRxBuffer(void);
//! sets the uart baud rate
void uartswSetBaudRate(u32 baudrate);
//! sends a single byte over the uart
void uartswSendByte(u08 data);

//! gets a single byte from the uart receive buffer
// Function returns TRUE if data was available, FALSE if not.
// Actual data is returned in variable pointed to by "data".
// example usage:
// char myReceivedByte;
// uartswReceiveByte( &myReceivedByte );
u08 uartswReceiveByte(u08* rxData);

//! internal transmit bit handler
void uartswTxBitService(void);
//! internal receive bit handler
void uartswRxBitService(void);

#endif
