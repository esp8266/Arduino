/*! \file mmc.c \brief MultiMedia and SD Flash Card Interface. */
//*****************************************************************************
//
// File Name	: 'mmc.c'
// Title		: MultiMedia and SD Flash Card Interface
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.09.22
// Revised		: 2004.09.22
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

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>		// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "spi.h"		// include spi bus support

#include "rprintf.h"

#include "mmc.h"

// include project-specific hardware configuration
#include "mmcconf.h"

// Global variables

// Functions

void mmcInit(void)
{
	// initialize SPI interface
	spiInit();
	// release chip select
	sbi(MMC_CS_DDR, MMC_CS_PIN);
	sbi(MMC_CS_PORT,MMC_CS_PIN);
}

u08 mmcReset(void)
{
	u08 retry;
	u08 r1=0;

	retry = 0;
	do
	{
		// send dummy bytes with CS high before accessing
		spiTransferByte(0xFF);
		spiTransferByte(0xFF);
		spiTransferByte(0xFF);
		spiTransferByte(0xFF);
		// resetting card, go to SPI mode
		r1 = mmcSendCommand(MMC_GO_IDLE_STATE, 0);
		#ifdef MMC_DEBUG
		rprintf("MMC_GO_IDLE_STATE: R1=0x%x\r\n", r1);
		#endif
		// do retry counter
		retry++;
		if(retry>10) return -1;
	} while(r1 != 0x01);

	// TODO: check card parameters for voltage compliance
	// before issuing initialize command

	retry = 0;
	do
	{
		// initializing card for operation
		r1 = mmcSendCommand(MMC_SEND_OP_COND, 0);
		#ifdef MMC_DEBUG
		rprintf("MMC_SEND_OP_COND: R1=0x%x\r\n", r1);
		#endif
		// do retry counter
		retry++;
		if(retry>100) return -1;
	} while(r1);
		
	// turn off CRC checking to simplify communication
	r1 = mmcSendCommand(MMC_CRC_ON_OFF, 0);
	#ifdef MMC_DEBUG
	rprintf("MMC_CRC_ON_OFF: R1=0x%x\r\n", r1);
	#endif

	// set block length to 512 bytes
	r1 = mmcSendCommand(MMC_SET_BLOCKLEN, 512);
	#ifdef MMC_DEBUG
	rprintf("MMC_SET_BLOCKLEN: R1=0x%x\r\n", r1);
	#endif

	// return success
	return 0;
}

u08 mmcSendCommand(u08 cmd, u32 arg)
{
	u08 r1;

	// assert chip select
	cbi(MMC_CS_PORT,MMC_CS_PIN);
	// issue the command
	r1 = mmcCommand(cmd, arg);
	// release chip select
	sbi(MMC_CS_PORT,MMC_CS_PIN);

	return r1;
}

u08 mmcRead(u32 sector, u08* buffer)
{
	u08 r1;
	u16 i;

	// assert chip select
	cbi(MMC_CS_PORT,MMC_CS_PIN);
	// issue command
	r1 = mmcCommand(MMC_READ_SINGLE_BLOCK, sector<<9);
	#ifdef MMC_DEBUG
	rprintf("MMC Read Block R1=0x%x\r\n", r1);
	#endif
	// check for valid response
	if(r1 != 0x00)
		return r1;
	// wait for block start
	while(spiTransferByte(0xFF) != MMC_STARTBLOCK_READ);
	// read in data
	for(i=0; i<0x200; i++)
	{
		*buffer++ = spiTransferByte(0xFF);
	}
	// read 16-bit CRC
	spiTransferByte(0xFF);
	spiTransferByte(0xFF);
	// release chip select
	sbi(MMC_CS_PORT,MMC_CS_PIN);
	// return success
	return 0;
}

u08 mmcWrite(u32 sector, u08* buffer)
{
	u08 r1;
	u16 i;

	// assert chip select
	cbi(MMC_CS_PORT,MMC_CS_PIN);
	// issue command
	r1 = mmcCommand(MMC_WRITE_BLOCK, sector<<9);
	#ifdef MMC_DEBUG
	rprintf("MMC Write Block R1=0x%x\r\n", r1);
	#endif
	// check for valid response
	if(r1 != 0x00)
		return r1;
	// send dummy
	spiTransferByte(0xFF);
	// send data start token
	spiTransferByte(MMC_STARTBLOCK_WRITE);
	// write data
	for(i=0; i<0x200; i++)
	{
		spiTransferByte(*buffer++);
	}
	// write 16-bit CRC (dummy values)
	spiTransferByte(0xFF);
	spiTransferByte(0xFF);
	// read data response token
	r1 = spiTransferByte(0xFF);
	if( (r1&MMC_DR_MASK) != MMC_DR_ACCEPT)
		return r1;
	#ifdef MMC_DEBUG
	rprintf("Data Response Token=0x%x\r\n", r1);
	#endif
	// wait until card not busy
	while(!spiTransferByte(0xFF));
	// release chip select
	sbi(MMC_CS_PORT,MMC_CS_PIN);
	// return success
	return 0;
}

u08 mmcCommand(u08 cmd, u32 arg)
{
	u08 r1;
	u08 retry=0;
	// send command
	spiTransferByte(cmd | 0x40);
	spiTransferByte(arg>>24);
	spiTransferByte(arg>>16);
	spiTransferByte(arg>>8);
	spiTransferByte(arg);
	spiTransferByte(0x95);	// crc valid only for MMC_GO_IDLE_STATE
	// end command
	// wait for response
	// if more than 8 retries, card has timed-out
	// return the received 0xFF
	while((r1 = spiTransferByte(0xFF)) == 0xFF)
		if(retry++ > 8) break;
	// return response
	return r1;
}
