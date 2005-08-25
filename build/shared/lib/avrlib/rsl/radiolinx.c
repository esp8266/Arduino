/*! \file radiolinx.c \brief Linx Radio Driver. */
//*****************************************************************************
//
// File Name	: 'radiolinx.c'
// Title		: Linx Radio Driver
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 09/01/2003
// Revised		: 09/03/2003
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>	// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "buffer.h"		// include buffer support
#include "uartsw.h"		// include software UART driver
#include "stxetx.h"		// include STX/ETX protocol library

#include "radiolinx.h"

// global variables

// functions
void radioInit(void)
{
	// Initialize radio interface
	// Since this radio creates a special serial interface,
	// we initialize it here.
	uartswInit();
	// set baud rate of comm
	uartswSetBaudRate(4800);
	// initialize stxetx to use the software UART for sending data
	stxetxInit(uartswSendByte);
}

void radioSend(u08 status, u08 type, u08 datalength, u08* dataptr)
{
	stxetxSend(status, type, datalength, dataptr);
}

cBuffer* radioGetRxBuffer(void)
{
	return uartswGetRxBuffer();
}
