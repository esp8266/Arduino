/*! \file ds1631.h \brief Dallas DS1631 Temperature Sensor Driver Library. */
//*****************************************************************************
//
// File Name	: 'ds1631.h'
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

#ifndef DS1631_H
#define DS1631_H

#include "global.h"

// constants/macros/typdefs
#define DS1631_I2C_ADDR			0x90	//< Base I2C address of DS1631 devices

#define DS1631_CMD_STARTCONV	0x51	//< DS1631 Start conversion command byte
#define DS1631_CMD_STOPCONV		0x22	//< DS1631 Stop conversion command byte
#define DS1631_CMD_READTEMP		0xAA	//< DS1631 Read Temperature command byte
#define DS1631_CMD_ACCESSTH		0xA1	//< DS1631 TH read/write command byte
#define DS1631_CMD_ACCESSTL		0xA2	//< DS1631 TL read/write command byte
#define DS1631_CMD_ACCESSCONFIG	0xAC	//< DS1631 Config read/write command byte
#define DS1631_CMD_SWPOR		0x54	//< DS1631 Software Reset command byte

#define DS1631_CONFIG_1SHOT		0x01
#define DS1631_CONFIG_POL		0x02
#define DS1631_CONFIG_R0		0x04
#define DS1631_CONFIG_R1		0x08
#define DS1631_CONFIG_NVB		0x10
#define DS1631_CONFIG_TLF		0x20
#define DS1631_CONFIG_THF		0x40
#define DS1631_CONFIG_DONE		0x80

// functions

//! Initialize the DS1631 chip
u08 ds1631Init(u08 i2cAddr);

//! Reset the DS1631 chip to its power-on defaults
u08 ds1631Reset(u08 i2cAddr);

//! Set the configuration byte of the DS1631
void ds1631SetConfig(u08 i2cAddr, u08 config);

//! Get the configuration byte of the DS1631
u08 ds1631GetConfig(u08 i2cAddr);

//! Start a temperature conversion
void ds1631StartConvert(u08 i2cAddr);

//! Stop a temperature conversion (or stop continuous conversion mode)
void ds1631StopConvert(u08 i2cAddr);

//! Read the result of a temperature conversion
s16 ds1631ReadTemp(u08 i2cAddr);

//! Set the Temp-High threshold
void ds1631SetTH(u08 i2cAddr, s16 value);

//! Set the Temp-Low threshold
void ds1631SetTL(u08 i2cAddr, s16 value);

//! Get the Temp-High threshold
s16 ds1631GetTH(u08 i2cAddr);

//! Get the Temp-Low threshold
s16 ds1631GetTL(u08 i2cAddr);

void ds1631WriteTempReg(u08 i2cAddr, u08 cmd, s16 value);
s16 ds1631ReadTempReg(u08 i2cAddr, u08 cmd);


#endif
