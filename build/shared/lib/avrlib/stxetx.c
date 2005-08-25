/*! \file stxetx.c \brief STX/ETX Packet Protocol Implementation Library. */
//*****************************************************************************
//
// File Name	: 'stxetx.c'
// Title		: STX/ETX Packet Protocol Implementation Library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 10/9/2002
// Revised		: 6/30/2003
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

#include "global.h"
#include "stxetx.h"
//#include "rprintf.h"

// function pointer to data output routine
static void (*stxetxDataOut)(unsigned char data);

// received packet data buffer
unsigned char stxetxRxPacket[STXETX_MAXRXPACKETSIZE];

// functions


// Initialize STX/ETX packet protocol library
void stxetxInit(void (*dataout_func)(unsigned char data))
{
	stxetxDataOut = dataout_func;
}

// Send/Create STX/ETX packet
void stxetxSend(unsigned char status, unsigned char type, unsigned char datalength, unsigned char* dataptr)
{
	unsigned char checksum = 0;
	unsigned short i;
	
	// write packet header
	stxetxDataOut(STX);
	stxetxDataOut(status);
	stxetxDataOut(type);
	stxetxDataOut(datalength);
	// update checksum
	checksum += status + type + datalength;
	// copy data into packet
	for(i = 0; i < datalength; i++)
	{
		stxetxDataOut(*dataptr);
		checksum += *dataptr;
		dataptr++;
	}
	// write packet trailer
	stxetxDataOut(checksum);
	stxetxDataOut(ETX);
}

// process buffer containing STX/ETX packets
unsigned char stxetxProcess(cBuffer* rxBuffer)
{
	unsigned char foundpacket = FALSE;
	unsigned short i;
	unsigned char length, checksum;
	//unsigned char type;

	// process the buffer
	// go through buffer looking for packets
	// the STX must be located at least STXETX_HEADERLENGTH+STXETX_TRAILERLENGTH from end
	// otherwise we must not have a complete packet
	while( rxBuffer->datalength >= ((u16)STXETX_HEADERLENGTH+(u16)STXETX_TRAILERLENGTH) )
	{
		// look for a potential start of packet
		if(bufferGetAtIndex(rxBuffer, 0) == STX)
		{
			// if this is a start, then get the length
			length = bufferGetAtIndex(rxBuffer, STXETX_LENGTHOFFSET);

			// now we must have at least STXETX_HEADERLENGTH+length+STXETX_TRAILERLENGTH in buffer to continue
			if(rxBuffer->datalength >= ((u16)STXETX_HEADERLENGTH+length+(u16)STXETX_TRAILERLENGTH))
			{
				// check to see if ETX is in the right position
				if(bufferGetAtIndex(rxBuffer, STXETX_HEADERLENGTH+length+STXETX_TRAILERLENGTH-1) == ETX)
				{
					// found potential packet
					// test checksum
					checksum = 0;
					// sum data between STX and ETX, not including checksum itself
					// (u16) casting needed to avoid unsigned/signed mismatch
					for(i = 0; i<((u16)STXETX_HEADERLENGTH+length+(u16)STXETX_TRAILERLENGTH-(u16)STXETX_NOETXSTXCHECKSUM); i++)
					{
						checksum += bufferGetAtIndex(rxBuffer, i+STXETX_STATUSOFFSET);
					}
					// compare checksums
					if(checksum == bufferGetAtIndex(rxBuffer, STXETX_CHECKSUMOFFSET+length))
					{
						//we have a packet!
						foundpacket = TRUE;
					
						// copy data to buffer
						// (don't copy STX, ETX, or CHECKSUM)
						for(i = 0; i < ((u16)STXETX_HEADERLENGTH+length-1); i++)
						{
							stxetxRxPacket[i] = bufferGetAtIndex(rxBuffer, i+1);
						}

						// debug
						//rprintf("STXETX Received packet type: 0x%x\n", bufferGetAtIndex(rxBuffer, STXETX_TYPEOFFSET));

						// dump this packet from the
						bufferDumpFromFront(rxBuffer, STXETX_HEADERLENGTH+length+STXETX_TRAILERLENGTH);

						// done with this processing session
						break;
					}
					else
					{
						// checksum bad
						//rprintf("STXETX Received packet with bad checksum\r\n");
						// for now, we dump these
						// dump this STX
						bufferGetFromFront(rxBuffer);
					}
				}
				else
				{
					// no ETX or ETX in wrong position
					// dump this STX
					bufferGetFromFront(rxBuffer);
				}
			}
			else
			{
				// not enough data in buffer to decode pending packet
				// wait until next time
				break;
			}
		}
		else
		{
			// this is not a start, dump it
			bufferGetFromFront(rxBuffer);
		}
	}

	// check if receive buffer is full with no packets decoding
	// (ie. deadlocked on garbage data or packet that exceeds buffer size)
	if(!bufferIsNotFull(rxBuffer))
	{
		// dump receive buffer contents to relieve deadlock
		bufferFlush(rxBuffer);
	}
	
	return foundpacket;
}

unsigned char stxetxGetRxPacketStatus(void)
{
	// return the packet's status
	// (subtract 1 from the offset because the STX has already been discarded)
	return stxetxRxPacket[STXETX_STATUSOFFSET-1];
}

unsigned char stxetxGetRxPacketType(void)
{
	// return the packet's type
	// (subtract 1 from the offset because the STX has already been discarded)
	return stxetxRxPacket[STXETX_TYPEOFFSET-1];
}

unsigned char stxetxGetRxPacketDatalength(void)
{
	// return the packet's datalength
	// (subtract 1 from the offset because the STX has already been discarded)
	return stxetxRxPacket[STXETX_LENGTHOFFSET-1];
}

unsigned char* stxetxGetRxPacketData(void)
{
	// return a pointer to the packet's data payload
	// (subtract 1 from the offset because the STX has already been discarded)
	return stxetxRxPacket+STXETX_DATAOFFSET-1;
}
