/*! \file i2csw.h \brief software-driven I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2csw.h'
// Title		: software-driven I2C interface using port pins
// Author		: Pascal Stang
// Created		: 11/22/2000
// Revised		: 5/2/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef I2CSW_H
#define I2CSW_H

#include "global.h"

// include project-dependent settings
#include "i2cswconf.h"

// defines and constants
#define READ		0x01	// I2C READ bit

// functions

// initialize I2C interface pins
void i2cInit(void);

// send I2C data to <device> register <sub>
void i2cSend(BYTE device, BYTE sub, BYTE length, BYTE *data);

// receive I2C data from <device> register <sub>
void i2cReceive(BYTE device, BYTE sub, BYTE length, BYTE *data);

#endif
