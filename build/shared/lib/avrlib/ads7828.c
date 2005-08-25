/*! \file ads7828.c \brief TI ADS7828 12-bit 8ch A/D Converter Driver Library. */
//*****************************************************************************
//
// File Name	: 'ads7828.c'
// Title		: TI ADS7828 12-bit 8ch A/D Converter Driver Library
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.02.10
// Revised		: 2004.02.19
// Version		: 0.1
// Target MCU	: Atmel AVR Series
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

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "global.h"
#include "i2c.h"
#include "ads7828.h"

// global variables
u08 Ads7282RefMode;

// Functions
u08 ads7828Init(u08 i2cAddr)
{
	u08 channel = 0x80;

	// setup default A/D voltage reference
	ads7828SetReference(0);

	// issue a convserion to test chip presence
	// return TRUE if chip detected
	// return FALSE if chip does not respond
	return (i2cMasterSendNI(i2cAddr, 1, &channel) == I2C_OK);
}

u16 ads7828Convert(u08 i2cAddr, u08 channel)
{
	// re-order channel bits for
	// logical single-ended channel selection
	// channel bit0 -> C2
	// channel bit1 -> C0
	// channel bit2 -> C1
	channel = (((channel>>1) | (channel&0x01)<<2)<<4) | ADS7828_CMD_SD;
	// do conversion
	return ads7828ConvertRaw(i2cAddr, channel);
}

u16 ads7828ConvertDiff(u08 i2cAddr, u08 channel)
{
	// clear single-ended channel bit
	channel = (channel&0x07)<<4;
	// do conversion
	return ads7828ConvertRaw(i2cAddr, channel);
}

u16 ads7828ConvertRaw(u08 i2cAddr, u08 channel)
{
	u08 buffer[2];
	// combine raw channel and reference bits
	channel &= 0xF0;
	channel |= Ads7282RefMode;
	// start conversion on requested channel
	i2cMasterSendNI(i2cAddr, 1, &channel);
	// retrieve conversion result
	i2cMasterReceiveNI(i2cAddr, 2, buffer);
	// pack bytes and return result
	return ((buffer[0]<<8) | buffer[1]);
}

void ads7828SetReference(u08 ref)
{
	if(ref)
	{
		// use internal reference
		Ads7282RefMode = ADS7828_CMD_PDMODE2;
	}
	else
	{
		// use external reference
		Ads7282RefMode = ADS7828_CMD_PDMODE0;
	}
}
