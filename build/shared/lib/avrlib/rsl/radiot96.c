/*! \file radiot96.c \brief DataRadio T96-SR Radio Driver. */
//*****************************************************************************
//
// File Name	: 'radiot96.c'
// Title		: DataRadio T96-SR Radio Driver
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
#include "timer128.h"	// include timer function library
#include "uart2.h"		// include software UART driver
#include "stxetx.h"		// include STX/ETX protocol library
#include "radiot96.h"

// global variables

// functions
void radioInit(void)
{
	// Initialize radio interface
	// set baud rate of comm
	uartSetBaudRate(COMM_UART, 19200);
	// initialize stxetx to use the UART for sending data
	#if COMM_UART == 0
	stxetxInit(uart0SendByte);
	#else
	stxetxInit(uart1SendByte);
	#endif
	// prepare PTT
	cbi(RADIO_PTT_PORT, RADIO_PTT_PIN);
	sbi(RADIO_PTT_DDR, RADIO_PTT_PIN);
}

void radioPTT(u08 pttFlag)
{
	if(pttFlag)
		sbi(RADIO_PTT_PORT, RADIO_PTT_PIN);
	else
		cbi(RADIO_PTT_PORT, RADIO_PTT_PIN);
}

void radioSend(u08 status, u08 type, u08 datalength, u08* dataptr)
{
	radioPTT(TRUE);
	timerPause(RADIO_PPT_DELAYMS);
	stxetxSend(status, type, datalength, dataptr);
	radioPTT(FALSE);
}

cBuffer* radioGetRxBuffer(void)
{
	return uartGetRxBuffer(COMM_UART);
}
