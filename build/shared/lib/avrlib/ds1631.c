/*! \file ds1631.c \brief Dallas DS1631 Temperature Sensor Driver Library. */
//*****************************************************************************
//
// File Name	: 'ds1631.c'
// Title		: Dallas DS1631 Temperature Sensor Driver Library
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
#include "timer.h"
#include "i2c.h"
#include "ds1631.h"

// global variables

// Functions
u08 ds1631Init(u08 i2cAddr)
{
	u08 chip_ok;
	// issue a reset
	if(ds1631Reset(i2cAddr) == I2C_OK)
		chip_ok = TRUE;
	else
		chip_ok = FALSE;
	// set a default configuration
	// (1-shot mode, T_OUT active high, and 12-bit conversion)
	ds1631SetConfig(i2cAddr,
		DS1631_CONFIG_1SHOT | DS1631_CONFIG_POL |
		DS1631_CONFIG_R0 | DS1631_CONFIG_R1);
	return chip_ok;
}

u08 ds1631Reset(u08 i2cAddr)
{
	u08 buffer[1];
	// return the DS1631 to power-on reset defaults
	buffer[0] = DS1631_CMD_SWPOR;
	return i2cMasterSendNI(i2cAddr, 1, buffer);
}

void ds1631SetConfig(u08 i2cAddr, u08 config)
{
	u08 buffer[2];
	// write the DS1631 configuration byte
	buffer[0] = DS1631_CMD_ACCESSCONFIG;
	buffer[1] = config;
	i2cMasterSendNI(i2cAddr, 2, buffer);
}

u08 ds1631GetConfig(u08 i2cAddr)
{
	u08 buffer[1];
	// write the DS1631 configuration byte
	buffer[0] = DS1631_CMD_ACCESSCONFIG;
	i2cMasterSendNI(i2cAddr, 2, buffer);
	i2cMasterReceiveNI(i2cAddr, 2, buffer);
	return buffer[0];
}

void ds1631StartConvert(u08 i2cAddr)
{
	u08 buffer[1];
	// send the DS1631 Start Convert command
	buffer[0] = DS1631_CMD_STARTCONV;
	i2cMasterSendNI(i2cAddr, 1, buffer);
}

void ds1631StopConvert(u08 i2cAddr)
{
	u08 buffer[1];
	// send the DS1631 Stop Convert command
	buffer[0] = DS1631_CMD_STOPCONV;
	i2cMasterSendNI(i2cAddr, 1, buffer);
}

s16 ds1631ReadTemp(u08 i2cAddr)
{
	// read the Temperature register and return the result
	return ds1631ReadTempReg(i2cAddr, DS1631_CMD_READTEMP);
}

void ds1631SetTH(u08 i2cAddr, s16 value)
{
	// write the TH register
	ds1631WriteTempReg(i2cAddr, DS1631_CMD_ACCESSTH, value);
}

void ds1631SetTL(u08 i2cAddr, s16 value)
{
	// write the TL register
	ds1631WriteTempReg(i2cAddr, DS1631_CMD_ACCESSTL, value);
}

s16 ds1631GetTH(u08 i2cAddr)
{
	// read the TH register and return the result
	return ds1631ReadTempReg(i2cAddr, DS1631_CMD_ACCESSTH);
}

s16 ds1631GetTL(u08 i2cAddr)
{
	// read the TL register and return the result
	return ds1631ReadTempReg(i2cAddr, DS1631_CMD_ACCESSTL);
}


s16 ds1631ReadTempReg(u08 i2cAddr, u08 cmd)
{
	u08 buffer[2];
	s16 T;

	// read the temperature value from the requested register
	i2cMasterSendNI(i2cAddr, 1, &cmd);
	i2cMasterReceiveNI(i2cAddr, 2, buffer);
	// pack bytes
	T = (s16)((buffer[0]<<8) | buffer[1]);
	// return result
	return T;
}

void ds1631WriteTempReg(u08 i2cAddr, u08 cmd, s16 value)
{
	u08 buffer[3];

	// write the requested register with a temperature value
	buffer[0] = cmd;
	buffer[1] = value>>8;
	buffer[2] = value;
	i2cMasterSendNI(i2cAddr, 3, buffer);
}
