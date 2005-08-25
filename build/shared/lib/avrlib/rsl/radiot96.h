/*! \file radiot96.h \brief DataRadio T96-SR Radio Driver. */
//*****************************************************************************
//
// File Name	: 'radiot96.h'
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

#ifndef RADIOT96_H
#define RADIOT96_H

// Radio PTT
#define RADIO_PTT_DDR		DDRD
#define RADIO_PTT_PORT		PORTD
#define RADIO_PTT_PIN		PD7

#define RADIO_PPT_DELAYMS	100

#define COMM_UART			1

// functions
void radioInit(void);
void radioSend(u08 status, u08 type, u08 datalength, u08* dataptr);
cBuffer* radioGetRxBuffer(void);

#endif
