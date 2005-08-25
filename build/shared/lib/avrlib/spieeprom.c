/*! \file spieeprom.c \brief Interface for standard SPI EEPROM memories. */
//*****************************************************************************
//
// File Name	: 'spieeprom.c'
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

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "spieeprom.h"

// functions
void spieepromInit(void)
{
	// although there is no code here
	// don't forget to initialize the SPI interface itself
//	sbi(DDRB, 0);
}

u08 spieepromReadByte(u32 memAddr)
{
	u08 data;
//	cbi(PORTB,0);
	// send command
	spiTransferByte(SPIEEPROM_CMD_READ);
	// send address
	spiTransferByte(memAddr>>8);
	spiTransferByte(memAddr&0x00FF);
	// read contents of memory address
	data = spiTransferByte(0xFF);
	// return data
	return data;
//	sbi(PORTB,0);
}

void spieepromWriteByte(u32 memAddr, u08 data)
{
	// wait for any previous write to complete
	while(spieepromReadStatus() & SPIEEPROM_STATUS_WIP);

//	cbi(PORTB,0);
	// send command
	spiTransferByte(SPIEEPROM_CMD_WRITE);
	// send address
	spiTransferByte(memAddr>>8);
	spiTransferByte(memAddr&0x00FF);
	// send data to be written
	spiTransferByte(data);
//	sbi(PORTB,0);
}

void spieepromWriteEnable(void)
{
//	cbi(PORTB,0);
	// send command
	spiTransferByte(SPIEEPROM_CMD_WREN);
//	sbi(PORTB,0);
}

void spieepromWriteDisable(void)
{
//	cbi(PORTB,0);
	// send command
	spiTransferByte(SPIEEPROM_CMD_WRDI);
//	sbi(PORTB,0);
}

u08 spieepromReadStatus(void)
{
	u08 status;
//	cbi(PORTB,0);
	// send command
	spiTransferByte(SPIEEPROM_CMD_RDSR);
	// get status register value
	status = spiTransferByte(0xFF);
//	sbi(PORTB,0);
	return status;
}
