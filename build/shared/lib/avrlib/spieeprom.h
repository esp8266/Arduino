/*! \file spieeprom.h \brief Interface for standard SPI EEPROM memories. */
//*****************************************************************************
//
// File Name	: 'spieeprom.h'
// Title		: Interface for standard SPI EEPROM memories
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.10.07
// Revised		: 2004.10.07
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef SPIEEPROM_H
#define SPIEEPROM_H

#include "global.h"

// defines and constants
// commands
#define SPIEEPROM_CMD_READ		0x03	//< Read byte(s)
#define SPIEEPROM_CMD_WRITE		0x02	//< Write byte(s)
#define SPIEEPROM_CMD_WREN		0x06	//< Write Enable
#define SPIEEPROM_CMD_WRDI		0x04	//< Write Disable
#define SPIEEPROM_CMD_RDSR		0x05	//< Read Status Register
#define SPIEEPROM_CMD_WRSR		0x01	//< Write Status Register

// status register bit defines
#define SPIEEPROM_STATUS_WIP	0x01	//< Write in progress
#define SPIEEPROM_STATUS_WEL	0x01	//< Write enable
#define SPIEEPROM_STATUS_BP0	0x01	//< Block Proection 0
#define SPIEEPROM_STATUS_BP1	0x01	//< Block Proection 1
#define SPIEEPROM_STATUS_WPEN	0x01	//< Write Protect Enable

// functions

//! Initialize SPI EEPROM interface
void spieepromInit(void);

//! In the SPI EEPROM read a byte from memory location [memAddr]
u08 spieepromReadByte(u32 memAddr);

//! In the SPI EEPROM write a byte [data] to the memory location [memAddr]
void spieepromWriteByte(u32 memAddr, u08 data);

void spieepromWriteEnable(void);
void spieepromWriteDisable(void);
u08 spieepromReadStatus(void);

#endif
