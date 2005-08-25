/*! \file i2cswconf.h \brief Software-driven I2C interface configuration. */
//*****************************************************************************
//
// File Name	: 'i2cswconf.h'
// Title		: software-driven I2C interface using port pins
// Author		: Pascal Stang - Copyright (C) 2000-2002
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

#ifndef I2CSWCONF_H
#define I2CSWCONF_H

// clock line port
#define SCLPORT	PORTD	// i2c clock port
#define SCLDDR	DDRD	// i2c clock port direction
// data line port
#define SDAPORT	PORTD	// i2c data port
#define SDADDR	DDRD	// i2c data port direction
#define SDAPIN	PIND	// i2c data port input
// pin assignments
#define SCL		PD0		// i2c clock pin
#define SDA		PD1		// i2c data pin

#endif
