/*! \file .c \brief Mitel GPS STX/ETX driver function library. */
//*****************************************************************************
//
// File Name	: 'mitelgps.c'
// Title		: Mitel GPS STX/ETX driver function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2003.04.11
// Revised		: 2003.08.26
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

#ifndef WIN32
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include <string.h>
	#include <stdlib.h>
#endif

#include "global.h"
#include "buffer.h"
#include "rprintf.h"
#include "uart2.h"
#include "gps.h"

#include "mitelgps.h"

// Program ROM constants

// Global variables
// external GPS information structure/repository (in gps.h/gps.c)
extern GpsInfoType GpsInfo;
// packet processing buffer
u08 MitelGpsPacket[MITELGPS_BUFFERSIZE];
// debug flag
u08 debug;
#define MITELGPS_DEBUG_PKTPARSE	0x01
#define MITELGPS_DEBUG_EXTRACT	0x02
// function pointer to single byte output routine
static void (*TxByteFunc)(unsigned char c);

void mitelgpsInit(void (*txbytefunc)(unsigned char c))
{
	// set transmit function
	// (this function will be used for all SendPacket commands)
	TxByteFunc = txbytefunc;
	// set debug status
	debug = 0;
}

void mitelgpsSendPacket(u08* data, u08 dataLength)
{
	u08 i;
	u08 dataIdx = 0;
	u08 checksum = 0;

	// start of packet
	MitelGpsPacket[dataIdx++] = STX;
	// add packet type and packet data
	for(i=0; i<dataLength; i++)
	{
		checksum ^= MitelGpsPacket[dataIdx];
		MitelGpsPacket[dataIdx++] = *data++;
	}
	// checksum
	convertIntToAsciiHex(checksum, &MitelGpsPacket[dataIdx], 2);
	dataIdx += 2;
	// end of packet
	MitelGpsPacket[dataIdx++] = ETX;

	// send it
	for(i=0; i<dataIdx; i++)
		TxByteFunc(MitelGpsPacket[i]);
}

u08 mitelgpsProcess(cBuffer* rxBuffer)
{
	u08 foundpacket = FALSE;
	u08 startFlag = FALSE;
	u08 checksum = 0;
	u08 packetType;
	u16 i,j;

	// process the receive buffer
	// go through buffer looking for packets
	while(rxBuffer->datalength > 1)
	{
		// look for a start of Mitel GPS STX/ETX packet
		if(bufferGetAtIndex(rxBuffer,0) == STX)
		{
			// found start
			startFlag = TRUE;
			// done looking for start
			break;
		}
		else
			// not STX, dump character from buffer
			bufferGetFromFront(rxBuffer);
	}
	
	// if we detected a start, look for end of packet
	if(startFlag)
	{
		for(i=1; i<(rxBuffer->datalength); i++)
		{
			// check for end of Mitel GPS STX/ETX packet
			if(bufferGetAtIndex(rxBuffer,i) == ETX)
			{
				// have a packet end
				// dump initial STX
				bufferGetFromFront(rxBuffer);
				// copy data to MitelGpsPacket
				for(j=0; j<(i-1); j++)
				{
					MitelGpsPacket[j] = bufferGetFromFront(rxBuffer);
					checksum ^= MitelGpsPacket[j];
				}
				// null-terminate copied string
				MitelGpsPacket[j] = 0;
				// dump ending ETX
				bufferGetFromFront(rxBuffer);

				// verify checksum
				// undo checksum summing of the checksum itself
				checksum ^= MitelGpsPacket[j-2];
				checksum ^= MitelGpsPacket[j-1];
				if( checksum == convertAsciiHexToInt(&MitelGpsPacket[j-2], 2) )
				{
					// found a good packet
					if(debug & MITELGPS_DEBUG_PKTPARSE)
					{
						rprintf("Rx Mitel GPS packet type: %c%c%c  len: %d\r\n",
							MitelGpsPacket[0], MitelGpsPacket[1], MitelGpsPacket[2], j);
						rprintfStr(MitelGpsPacket);
						rprintfCRLF();
					}
					// done with this processing session
					foundpacket = TRUE;
					break;
				}
				else
				{
					if(debug & MITELGPS_DEBUG_PKTPARSE)
					{
						rprintf("Rx Mitel GPS packet type: %c%c%c len: %d  Bad Checksum Rcvd: 0x%c%c Calc: 0x%x\r\n",
							MitelGpsPacket[0], MitelGpsPacket[1], MitelGpsPacket[2], j, MitelGpsPacket[j-2], MitelGpsPacket[j-1], checksum);
					}
				}
			}
		}
	}

	// handle and direct the received packet
	if(foundpacket)
	{
		// switch on the packet type
		packetType = convertAsciiHexToInt(&MitelGpsPacket[1], 2);
		switch( packetType )
		{
		case MITELTYPE_NAVDATAGND:	mitelgpsProcessNAVDATAGND(MitelGpsPacket); break;
		case MITELTYPE_CHNLSTATGND:	mitelgpsProcessCHNLSTATGND(MitelGpsPacket); break;
		case MITELTYPE_NAVDATA:		mitelgpsProcessNAVDATA(MitelGpsPacket); break;
		case MITELTYPE_RAWDATA:		mitelgpsProcessRAWDATA(MitelGpsPacket); break;
		case MITELTYPE_CHNLSTAT:	mitelgpsProcessCHNLSTAT(MitelGpsPacket); break;
		case MITELTYPE_RELNAVECEF:	break;
		case MITELTYPE_RELNAVRTN:	break;
		default:
			if(debug & MITELGPS_DEBUG_PKTPARSE)
				rprintf("Unhandled Mitel GPS packet type: 0x%x\r\n", packetType);
			break;
		}
	}

	return foundpacket;
}

void mitelgpsProcessNAVDATAGND(u08* packet)
{
	// process "F00" report packets - Navigation Data (Ground)
	char* endptr;

	if(debug & MITELGPS_DEBUG_EXTRACT)
	{
		rprintf("MITELGPS: ");
		rprintfStr(packet);
		rprintfCRLF();
	}

	// start parsing just after "F00"
	// get latitude [sdd.dddddd]
	GpsInfo.PosLLA.lat.f = strtod(&packet[3], &endptr);
	// get longitude [sddd.dddddd]
	GpsInfo.PosLLA.lon.f = strtod(&packet[3+10], &endptr);
	// get altitude [sxxxxxx.x]
	GpsInfo.PosLLA.alt.f = strtod(&packet[3+10+11], &endptr);
	// get speed [sxxx.xx]
	GpsInfo.VelHS.speed.f = strtod(&packet[3+10+11+9], &endptr);
	// get heading [ddd]
	GpsInfo.VelHS.heading.f = strtod(&packet[3+10+11+9+7], &endptr);

	// get # of SVs tracked [xx]
	GpsInfo.numSVs = atoi(&packet[3+10+11+9+7+5+7+5+5+5]);
}

void mitelgpsProcessCHNLSTATGND(u08* packet)
{
	// process "F03" report packets - Channel Status (Ground)
}

void mitelgpsProcessNAVDATA(u08* packet)
{
	// process "F40" report packets - Navigation Data
	char* endptr;

	// start parsing just after "F40"
	// get gps week number [xxxx]=4
	GpsInfo.WeekNum = atoi(&packet[3]);
	// get gps time of week [xxxxxx.xxxxx]=12
	GpsInfo.TimeOfWeek.f = strtod(&packet[3+4], &endptr);
	// gps-utc time difference? [xx]=2
	// get ECEF X [sxxxxxxxx.xx]=12
	GpsInfo.PosECEF.x.f = strtod(&packet[3+4+12+2], &endptr);
	// get ECEF Y [sxxxxxxxx.xx]=12
	GpsInfo.PosECEF.y.f = strtod(&packet[3+4+12+2+12], &endptr);
	// get ECEF Z [sxxxxxxxx.xx]=12
	GpsInfo.PosECEF.z.f = strtod(&packet[3+4+12+2+12+12], &endptr);
	// get ECEF vX [sxxxxxxxx.xx]=12
	GpsInfo.VelECEF.x.f = strtod(&packet[3+4+12+2+12+12+12], &endptr);
	// get ECEF vY [sxxxxxxxx.xx]=12
	GpsInfo.VelECEF.y.f = strtod(&packet[3+4+12+2+12+12+12+12], &endptr);
	// get ECEF vZ [sxxxxxxxx.xx]=12
	GpsInfo.VelECEF.z.f = strtod(&packet[3+4+12+2+12+12+12+12+12], &endptr);
}

void mitelgpsProcessRAWDATA(u08* packet)
{
	// process "F42" report packets - Pseudorange, carrier phase, doppler
}

void mitelgpsProcessCHNLSTAT(u08* packet)
{
	// process "F43" report packets - Channel Status
}

// data conversions
u32 convertAsciiHexToInt(u08* string, u08 numdigits)
{
	u08 i;
	u32 num = 0;

	for(i=0; i<numdigits; i++)
	{
		// shift number up
		num = num<<4;
		// decode hex digit
		if(string[i] >= 'a')
			num |= string[i]-'a'+10;
		else if(string[i] >= 'A')
			num |= string[i]-'A'+10;
		else
			num |= string[i]-'0';
	}
	return num;
}

void convertIntToAsciiHex(u32 num, u08* string, u08 numdigits)
{
	u08 i;

	for(i=0; i<numdigits; i++)
	{
		if((num & 0x0000000F) < 10)
			string[numdigits-1-i] = (num & 0x0000000F)+'0';
		else
			string[numdigits-1-i] = (num & 0x0000000F)+'A'-10;
		// next digit
		num = num>>4;
	}
}
