/*! \file mmc.h \brief MultiMedia and SD Flash Card Interface. */
//*****************************************************************************
//
// File Name	: 'mmc.h'
// Title		: MultiMedia and SD Flash Card Interface
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.09.22
// Revised		: 2004.09.22
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Description	: This library offers some simple function which can be used
//		to read and write data on a MultiMedia or SecureDigital (SD) Flash
//		Card.  Although MM and SD Cards are designed to operate with their own
//		special bus wiring and protocols, both types of cards also provide a
//		simple SPI-like interface mode which is exceptionally useful when 
//		attempting to use the cards in embedded systems.
//
//		To work with this library, the card must be wired to the SPI port of
//		the Atmel microcontroller as described below.
//		  _________________
//		 /  1 2 3 4 5 6 78 |	<- view of MMC/SD card looking at contacts
//		/ 9                |	Pins 8 and 9 are present only on SD cards
//		|    MMC/SD Card   |
//		/\/\/\/\/\/\/\/\/\/
//
//		1 - CS   (chip select)			- wire to any available I/O pin(*)
//		2 - DIN  (data in, card<-host)	- wire to SPI MOSI pin
//		3 - VSS  (ground)				- wire to ground
//		4 - VDD  (power, 3.3V only?)	- wire to power (MIGHT BE 3.3V ONLY!)
//		5 -	SCLK (data clock)			- wire to SPI SCK pin
//		6 - VSS  (ground)				- wire to ground
//		7 - DOUT (data out, card->host)	- wire to SPI MISO pin
//
//		(*) you must define this chip select I/O pin in mmcconf.h
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef MMC_H
#define MMC_H

#include "global.h"

// constants/macros/typdefs
// MMC commands (taken from sandisk MMC reference)
#define MMC_GO_IDLE_STATE			0
#define MMC_SEND_OP_COND			1
#define MMC_SEND_CSD				9
#define MMC_SEND_CID				10
#define MMC_SEND_STATUS				13
#define MMC_SET_BLOCKLEN			16
#define MMC_READ_SINGLE_BLOCK		17
#define MMC_WRITE_BLOCK				24
#define MMC_PROGRAM_CSD				27
#define MMC_SET_WRITE_PROT			28
#define MMC_CLR_WRITE_PROT			29
#define MMC_SEND_WRITE_PROT			30
#define MMC_TAG_SECTOR_START		32
#define MMC_TAG_SECTOR_END			33
#define MMC_UNTAG_SECTOR			34
#define MMC_TAG_ERASE_GROUP_START 	35
#define MMC_TAG_ERARE_GROUP_END		36
#define MMC_UNTAG_ERASE_GROUP		37
#define MMC_ERASE					38
#define MMC_CRC_ON_OFF				59
// R1 Response bit-defines
#define MMC_R1_BUSY					0x80
#define MMC_R1_PARAMETER			0x40
#define MMC_R1_ADDRESS				0x20
#define MMC_R1_ERASE_SEQ			0x10
#define MMC_R1_COM_CRC				0x08
#define MMC_R1_ILLEGAL_COM			0x04
#define MMC_R1_ERASE_RESET			0x02
#define MMC_R1_IDLE_STATE			0x01
// Data Start tokens
#define MMC_STARTBLOCK_READ			0xFE
#define MMC_STARTBLOCK_WRITE		0xFE
#define MMC_STARTBLOCK_MWRITE		0xFC
// Data Stop tokens
#define MMC_STOPTRAN_WRITE			0xFD
// Data Error Token values
#define MMC_DE_MASK					0x1F
#define MMC_DE_ERROR				0x01
#define MMC_DE_CC_ERROR				0x02
#define MMC_DE_ECC_FAIL				0x04
#define MMC_DE_OUT_OF_RANGE			0x04
#define MMC_DE_CARD_LOCKED			0x04
// Data Response Token values
#define MMC_DR_MASK					0x1F
#define MMC_DR_ACCEPT				0x05
#define MMC_DR_REJECT_CRC			0x0B
#define MMC_DR_REJECT_WRITE_ERROR	0x0D

// functions

//! Initialize hardware interface
void mmcInit(void);

//! Initialize the card and prepare it for use
// returns zero if successful
u08 mmcReset(void);

//! Send card an MMC command
// returns R1 result code
u08 mmcSendCommand(u08 cmd, u32 arg);

//! Read 512-byte sector from card to buffer
// returns zero if successful
u08 mmcRead(u32 sector, u08* buffer);
//! Write 512-byte sector from buffer to card
// returns zero if successful
u08 mmcWrite(u32 sector, u08* buffer);

//! internal command function
u08 mmcCommand(u08 cmd, u32 arg);

#endif
