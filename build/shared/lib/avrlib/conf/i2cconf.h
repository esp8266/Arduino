/*! \file i2cconf.h \brief I2C (TWI) interface configuration. */
//*****************************************************************************
//
// File Name	: 'i2cconf.h'
// Title		: I2C (TWI) interface configuration
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.7
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef I2CCONF_H
#define I2CCONF_H

// define I2C data buffer sizes
// These buffers are used in interrupt-driven Master sending and receiving,
// and in slave sending and receiving.  They must be large enough to store
// the largest I2C packet you expect to send and receive, respectively.
#define I2C_SEND_DATA_BUFFER_SIZE		0x20
#define I2C_RECEIVE_DATA_BUFFER_SIZE	0x20

#endif
