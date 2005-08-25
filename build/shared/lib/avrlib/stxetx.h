/*! \file stxetx.h \brief STX/ETX Packet Protocol Implementation Library. */
//*****************************************************************************
//
// File Name	: 'stxetx.h'
// Title		: STX/ETX Packet Protocol Implementation Library
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 10/9/2002
// Revised		: 02/10/2003
// Version		: 0.1
// Target MCU	: any
// Editor Tabs	: 4
//
// Description	: This library provides a set of functions needed to send and
//		receive STX/ETX packets.  STX/ETX is a simple packet protocol that can
//		be wrapped around user data for one or more of the following reasons:
//
//			1. packetization is needed
//				- Using packets can be helpful if your data naturally forms 
//				little "bunches" or if different types of data must be sent
//				over the same channel (a serial cable, for example).  If your
//				data forms "bunches", you can send user data inside STX/ETX
//				packets with a predetermined structure, like an array of A/D
//				conversion results.  If you need a way to tell the receiver
//				what kind of data you're sending, you can use the TYPE field
//				in the STX/ETX packet.
//			2. error checking is needed
//				- STX/ETX packets will add a checksum to your data.  This
//				allows the receiver to verify that data was received correctly
//				and is error-free.  Packets which are corrupted in transmission
//				and fail the the checksum test are automatically discarded.
//				Error checking is especially useful when the data transmission
//				channel is unreliable or noisy (examples: radio, infrared, long
//				cables, etc)
//	
//		STX/ETX packets have the following structure:
//
//		[STX][status][type][length][user data...][checksum][ETX]
//
//		All fields are 1 byte except for user data which may be 0-255 bytes.
//		Uppercase fields are constant (STX=0x02, ETX=0x03), lowercase fields
//		vary.  The length field is the number of bytes in the user data area.
//		The checksum is the 8-bit sum of all bytes between but not including
//		STX/ETX.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef STXETX_H
#define STXETX_H

#include "buffer.h"

// include project-dependent configuration options
#include "stxetxconf.h"

// constants
// packet markers
#define STX		0x02				// start transmission marker
#define ETX		0x03				// end transmission marker
// packet length parameters
#define STXETX_HEADERLENGTH		4	// number of bytes required for packet header
#define STXETX_TRAILERLENGTH	2	// number of bytes required for packet trailer
// packet field offsets
#define	STXETX_STATUSOFFSET		1	// number of bytes from STX to STATUS
#define STXETX_TYPEOFFSET		2	// number of bytes from STX to TYPE
#define STXETX_LENGTHOFFSET		3	// number of bytes from STX to LENGTH
#define STXETX_DATAOFFSET		4	// number of bytes from STX to the data
#define STXETX_CHECKSUMOFFSET	4	// number of bytes from STX+[length] to CHECKSUM
#define STXETX_NOETXSTXCHECKSUM 3	// number of bytes used by STX,ETX,CHECKSUM


// function prototypes

//! Initialize STX/ETX packet protocol library
void stxetxInit(void (*dataout_func)(unsigned char data));

//! Send/Create STX/ETX packet
void stxetxSend(unsigned char status, unsigned char type, unsigned char datalength, unsigned char* dataptr);

//! Process a buffer containing STX/ETX packets
unsigned char stxetxProcess(cBuffer* rxBuffer);

//! Returns the received packet's status
unsigned char stxetxGetRxPacketStatus(void);

//! Returns the received packet's type
unsigned char stxetxGetRxPacketType(void);

//! Returns the received packet's datalength
unsigned char stxetxGetRxPacketDatalength(void);

//! Returns pointer to the received packet's data
unsigned char* stxetxGetRxPacketData(void);


#endif
