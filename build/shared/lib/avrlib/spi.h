/*! \file spi.h \brief SPI interface driver. */
//*****************************************************************************
//
// File Name	: 'spi.h'
// Title		: SPI interface driver
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 06/06/2002
// Version		: 0.6
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef SPI_H
#define SPI_H

#include "global.h"

// function prototypes

// SPI interface initializer
void spiInit(void);

// spiSendByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  This command
// does not receive anything.
void spiSendByte(u08 data);

// spiTransferByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  The function also
// returns the byte that was received during transmission.
u08 spiTransferByte(u08 data);

// spiTransferWord(u08 data) works just like spiTransferByte but
// operates on a whole word (16-bits of data).
u16 spiTransferWord(u16 data);

#endif
