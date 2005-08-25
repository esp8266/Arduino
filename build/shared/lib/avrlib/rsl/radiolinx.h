/*! \file radiolinx.h \brief Linx Radio Driver. */
//*****************************************************************************
//
// File Name	: 'radiolinx.h'
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

#ifndef RADIOLINX_H
#define RADIOLINX_H

// functions
void radioInit(void);
void radioSend(u08 status, u08 type, u08 datalength, u08* dataptr);
cBuffer* radioGetRxBuffer(void);

#endif
