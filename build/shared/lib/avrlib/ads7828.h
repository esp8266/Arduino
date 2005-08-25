/*! \file ads7828.h \brief TI ADS7828 12-bit 8ch A/D Converter Driver Library. */
//*****************************************************************************
//
// File Name	: 'ads7828.h'
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

#ifndef ADS7828_H
#define ADS7828_H

#include "global.h"

// constants/macros/typdefs
#define ADS7828_I2C_ADDR	0x90	//< Base I2C address of ADS7828 devices

// command register bit defines
#define ADS7828_CMD_PD0		0x04	//< ADS7828 Power-down bit 0
#define ADS7828_CMD_PD1		0x08	//< ADS7828 Power-down bit 1
#define ADS7828_CMD_C0		0x10	//< ADS7828 Channel Select bit 0
#define ADS7828_CMD_C1		0x20	//< ADS7828 Channel Select bit 1
#define ADS7828_CMD_C2		0x40	//< ADS7828 Channel Select bit 2
#define ADS7828_CMD_SD		0x80	//< ADS7828 Single-ended/Differential Select bit

// single-ended channel order defines
#define ADS7828_CMD_CH0		0x00	//< ADS7828 Convert Channel 0
#define ADS7828_CMD_CH1		0x04	//< ADS7828 Convert Channel 1
#define ADS7828_CMD_CH2		0x01	//< ADS7828 Convert Channel 2
#define ADS7828_CMD_CH3		0x05	//< ADS7828 Convert Channel 3
#define ADS7828_CMD_CH4		0x02	//< ADS7828 Convert Channel 4
#define ADS7828_CMD_CH5		0x06	//< ADS7828 Convert Channel 5
#define ADS7828_CMD_CH6		0x03	//< ADS7828 Convert Channel 6
#define ADS7828_CMD_CH7		0x07	//< ADS7828 Convert Channel 7

// power-down mode defines
#define ADS7828_CMD_PDMODE0	0x00	//< ADS7828 Power-down Mode 0
#define ADS7828_CMD_PDMODE1	0x04	//< ADS7828 Power-down Mode 1
#define ADS7828_CMD_PDMODE2	0x08	//< ADS7828 Power-down Mode 2
#define ADS7828_CMD_PDMODE3	0x0C	//< ADS7828 Power-down Mode 3

// functions

//! Initialize the ADS7828 chip
//	returns:
//	TRUE if successful
//	FALSE if unsuccessful (chip not present)
u08 ads7828Init(u08 i2cAddr);

//! Set the voltage reference to use for A/D conversion
//  ref = 0  =>  External reference voltage on Ref pin
//  ref = 1  =>  Internal 2.5V reference voltage (Ref pin left open)
void ads7828SetReference(u08 ref);

//! Begin single-ended conversion on given logical channel#, and return result
u16 ads7828Convert(u08 i2cAddr, u08 channel);

//! Begin differential conversion on given channel pair, and return result
u16 ads7828ConvertDiff(u08 i2cAddr, u08 channel);

//! Begin conversion on given raw channel#, and return result
u16 ads7828ConvertRaw(u08 i2cAddr, u08 channel);

#endif
