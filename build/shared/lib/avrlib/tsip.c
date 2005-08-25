/*! \file tsip.c \brief TSIP (Trimble Standard Interface Protocol) function library. */
//*****************************************************************************
//
// File Name	: 'tsip.c'
// Title		: TSIP (Trimble Standard Interface Protocol) function library
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.08.27
// Revised		: 2003.07.17
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
	#include <math.h>
	#include <stdlib.h>
#endif

#include "global.h"
#include "buffer.h"
#include "rprintf.h"
#include "uart2.h"
#include "gps.h"

#include "tsip.h"

// Program ROM constants

// Global variables
extern GpsInfoType GpsInfo;
#define BUFFERSIZE 0x40
u08 TsipPacket[BUFFERSIZE];
u08 debug;

// function pointer to single byte output routine
static void (*TsipTxByteFunc)(unsigned char c);

void tsipInit(void (*txbytefunc)(unsigned char c))
{
	// set transmit function
	// (this function will be used for all SendPacket commands)
	TsipTxByteFunc = txbytefunc;

	// set debug status
	debug = 0;

	// compose GPS receiver configuration packet
	u08 packet[4];
	packet[0] = BV(POS_LLA);
	packet[1] = BV(VEL_ENU);
	packet[2] = 0;
	packet[3] = 0;
	// send configuration
	tsipSendPacket(TSIPTYPE_SET_IO_OPTIONS, 4, packet);
}

void tsipSendPacket(u08 tsipType, u08 dataLength, u08* data)
{
	u08 i;
	u08 dataIdx = 0;

	// start of packet
	TsipPacket[dataIdx++] = DLE;
	// packet type
	TsipPacket[dataIdx++] = tsipType;
	// add packet data
	for(i=0; i<dataLength; i++)
	{
		if(*data == DLE)
		{
			// do double-DLE escape sequence
			TsipPacket[dataIdx++] = *data;
			TsipPacket[dataIdx++] = *data++;
		}
		else
			TsipPacket[dataIdx++] = *data++;
	}
	// end of packet
	TsipPacket[dataIdx++] = DLE;
	TsipPacket[dataIdx++] = ETX;

	for(i=0; i<dataIdx; i++)
		TsipTxByteFunc(TsipPacket[i]);
}

u08 tsipProcess(cBuffer* rxBuffer)
{
	u08 foundpacket = FALSE;
	u08 startFlag = FALSE;
	u08 data;
	u08 i,j,k;

	u08 TsipPacketIdx;
	
	// process the receive buffer
	// go through buffer looking for packets
	while(rxBuffer->datalength > 1)
	{
		// look for a potential start of TSIP packet
		if(bufferGetAtIndex(rxBuffer,0) == DLE)
		{
			// make sure the next byte is not DLE or ETX
			data = bufferGetAtIndex(rxBuffer,1);
			if((data != DLE) && (data != ETX))
			{
				// found potential start
				startFlag = TRUE;
				// done looking for start
				break;
			}
		}
		else
			// not DLE, dump character from buffer
			bufferGetFromFront(rxBuffer);
	}
	
	// if we detected a start, look for end of packet
	if(startFlag)
	{
		for(i=1; i<(rxBuffer->datalength)-1; i++)
		{
			// check for potential end of TSIP packet
			if((bufferGetAtIndex(rxBuffer,i) == DLE) && (bufferGetAtIndex(rxBuffer,i+1) == ETX))
			{
				// have a packet end
				// dump initial DLE
				bufferGetFromFront(rxBuffer);
				// copy data to TsipPacket
				TsipPacketIdx = 0;
				for(j=0; j<(i-1); j++)
				{
					data = bufferGetFromFront(rxBuffer);
					if(data == DLE)
					{
						if(bufferGetAtIndex(rxBuffer,0) == DLE)
						{
							// found double-DLE escape sequence, skip one of them
							bufferGetFromFront(rxBuffer);
							j++;
						}
					}
					TsipPacket[TsipPacketIdx++] = data;
				}
				// dump ending DLE+ETX
				bufferGetFromFront(rxBuffer);
				bufferGetFromFront(rxBuffer);

				// found a packet
				if(debug)
				{
					rprintf("Rx TSIP packet type: 0x%x  len: %d  rawlen: %d\r\n",
						TsipPacket[0],
						TsipPacketIdx,
						i);
					for(k=0; k<TsipPacketIdx; k++)
					{
						rprintfu08(TsipPacket[k]);
						rprintfChar(' ');
					}
					//rprintfu08(bufferGetFromFront(rxBuffer)); rprintfChar(' ');
					//rprintfu08(bufferGetFromFront(rxBuffer)); rprintfChar(' ');

					rprintfCRLF();
				}
				// done with this processing session
				foundpacket = TRUE;
				break;
			}
		}
	}

	if(foundpacket)
	{
		// switch on the packet type
		switch(TsipPacket[0])
		{
		case TSIPTYPE_GPSTIME:			tsipProcessGPSTIME(TsipPacket); break;
		case TSIPTYPE_POSFIX_XYZ_SP:	tsipProcessPOSFIX_XYZ_SP(TsipPacket); break;
		case TSIPTYPE_VELFIX_XYZ:		tsipProcessVELFIX_XYZ(TsipPacket); break;

		case TSIPTYPE_POSFIX_LLA_SP:	tsipProcessPOSFIX_LLA_SP(TsipPacket); break;
		case TSIPTYPE_VELFIX_ENU:		tsipProcessVELFIX_ENU(TsipPacket); break;

		case TSIPTYPE_RAWDATA: break;
		default:
			//if(debug) rprintf("Unhandled TSIP packet type: 0x%x\r\n",TsipPacket[0]);
			break;
		}
	}

	return foundpacket;
}

void tsipProcessGPSTIME(u08* packet)
{
	// NOTE: check endian-ness if porting to processors other than the AVR
	GpsInfo.TimeOfWeek.b[3] = packet[1];
	GpsInfo.TimeOfWeek.b[2] = packet[2];
	GpsInfo.TimeOfWeek.b[1] = packet[3];
	GpsInfo.TimeOfWeek.b[0] = packet[4];

	GpsInfo.WeekNum	= ((u16)packet[5]<<8)|((u16)packet[6]);

	GpsInfo.UtcOffset.b[3]	= packet[7];
	GpsInfo.UtcOffset.b[2]	= packet[8];
	GpsInfo.UtcOffset.b[1]	= packet[9];
	GpsInfo.UtcOffset.b[0]	= packet[10];
}

void tsipProcessPOSFIX_XYZ_SP(u08* packet)
{
	// NOTE: check endian-ness if porting to processors other than the AVR
	GpsInfo.PosECEF.x.b[3] = packet[1];
	GpsInfo.PosECEF.x.b[2] = packet[2];
	GpsInfo.PosECEF.x.b[1] = packet[3];
	GpsInfo.PosECEF.x.b[0] = packet[4];

	GpsInfo.PosECEF.y.b[3] = packet[5];
	GpsInfo.PosECEF.y.b[2] = packet[6];
	GpsInfo.PosECEF.y.b[1] = packet[7];
	GpsInfo.PosECEF.y.b[0] = packet[8];

	GpsInfo.PosECEF.z.b[3] = packet[9];
	GpsInfo.PosECEF.z.b[2] = packet[10];
	GpsInfo.PosECEF.z.b[1] = packet[11];
	GpsInfo.PosECEF.z.b[0] = packet[12];

	GpsInfo.PosECEF.TimeOfFix.b[3] = packet[13];
	GpsInfo.PosECEF.TimeOfFix.b[2] = packet[14];
	GpsInfo.PosECEF.TimeOfFix.b[1] = packet[15];
	GpsInfo.PosECEF.TimeOfFix.b[0] = packet[16];

	GpsInfo.PosECEF.updates++;

//	GpsInfo.TimeOfFix_ECEF.f = *((float*)&packet[13]);
}

void tsipProcessVELFIX_XYZ(u08* packet)
{
}

void tsipProcessPOSFIX_LLA_SP(u08* packet)
{
	// NOTE: check endian-ness if porting to processors other than the AVR
	GpsInfo.PosLLA.lat.b[3] = packet[1];
	GpsInfo.PosLLA.lat.b[2] = packet[2];
	GpsInfo.PosLLA.lat.b[1] = packet[3];
	GpsInfo.PosLLA.lat.b[0] = packet[4];

	GpsInfo.PosLLA.lon.b[3] = packet[5];
	GpsInfo.PosLLA.lon.b[2] = packet[6];
	GpsInfo.PosLLA.lon.b[1] = packet[7];
	GpsInfo.PosLLA.lon.b[0] = packet[8];

	GpsInfo.PosLLA.alt.b[3] = packet[9];
	GpsInfo.PosLLA.alt.b[2] = packet[10];
	GpsInfo.PosLLA.alt.b[1] = packet[11];
	GpsInfo.PosLLA.alt.b[0] = packet[12];

	GpsInfo.PosLLA.TimeOfFix.b[3] = packet[17];
	GpsInfo.PosLLA.TimeOfFix.b[2] = packet[18];
	GpsInfo.PosLLA.TimeOfFix.b[1] = packet[18];
	GpsInfo.PosLLA.TimeOfFix.b[0] = packet[20];

	GpsInfo.PosLLA.updates++;
}

void tsipProcessVELFIX_ENU(u08* packet)
{
	// NOTE: check endian-ness if porting to processors other than the AVR
	GpsInfo.VelENU.east.b[3] = packet[1];
	GpsInfo.VelENU.east.b[2] = packet[2];
	GpsInfo.VelENU.east.b[1] = packet[3];
	GpsInfo.VelENU.east.b[0] = packet[4];

	GpsInfo.VelENU.north.b[3] = packet[5];
	GpsInfo.VelENU.north.b[2] = packet[6];
	GpsInfo.VelENU.north.b[1] = packet[7];
	GpsInfo.VelENU.north.b[0] = packet[8];

	GpsInfo.VelENU.up.b[3] =	packet[9];
	GpsInfo.VelENU.up.b[2] =	packet[10];
	GpsInfo.VelENU.up.b[1] =	packet[11];
	GpsInfo.VelENU.up.b[0] =	packet[12];

	GpsInfo.VelENU.TimeOfFix.b[3] =	packet[17];
	GpsInfo.VelENU.TimeOfFix.b[2] =	packet[18];
	GpsInfo.VelENU.TimeOfFix.b[1] =	packet[19];
	GpsInfo.VelENU.TimeOfFix.b[0] =	packet[20];

	GpsInfo.VelENU.updates++;
}

void tsipProcessRAWDATA(cBuffer* packet)
{
/*
	char oft = 1;
	// process the data in TSIPdata
	unsigned char SVnum = TSIPdata[oft];
	unsigned __int32 SNR32 =		(TSIPdata[oft+5] << 24) + (TSIPdata[oft+6] << 16) + (TSIPdata[oft+7] << 8) + (TSIPdata[oft+8]);
	unsigned __int32 codephase32 =	(TSIPdata[oft+9] << 24) + (TSIPdata[oft+10] << 16) + (TSIPdata[oft+11] << 8) + (TSIPdata[oft+12]);
	unsigned __int32 doppler32 =	(TSIPdata[oft+13] << 24) + (TSIPdata[oft+14] << 16) + (TSIPdata[oft+15] << 8) + (TSIPdata[oft+16]);
	
	unsigned __int64 meastimeH32 =	(TSIPdata[oft+17] << 24) | (TSIPdata[oft+18] << 16) | (TSIPdata[oft+19] << 8) | (TSIPdata[oft+20]);
	unsigned __int64 meastimeL32 =	(TSIPdata[oft+21] << 24) | (TSIPdata[oft+22] << 16) | (TSIPdata[oft+23] << 8) | (TSIPdata[oft+24]);
	unsigned __int64 meastime64 =	(meastimeH32 << 32) | (meastimeL32);
	
	float SNR =			*((float*) &SNR32);
	float codephase =	*((float*) &codephase32);
	float doppler =		*((float*) &doppler32);
	double meastime =	*((double*) &meastime64);
	
	// output to screen
	printf("SV%2d SNR: %5.2f PH: %11.4f DOP: %11.4f TIME: %5.0I64f EPOCH: %7.2I64f\n",SVnum,SNR,codephase,doppler,meastime,meastime/1.5);
	//printf("SV%2d  SNR: %5.2f  PH: %10.4f  DOP: %10.4f  TIME: %I64x\n",SVnum,SNR,codephase,doppler,meastime64);

	// output to file
	fprintf( logfile, "%2d  %5.2f  %11.4f  %11.4f  %5.0I64f  %7.2I64f\n",SVnum,SNR,codephase,doppler,meastime,meastime/1.5);
*/
}

