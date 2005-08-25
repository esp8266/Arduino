/*! \file nmea.c \brief NMEA protocol function library. */
//*****************************************************************************
//
// File Name	: 'nmea.c'
// Title		: NMEA protocol function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.27
// Revised		: 2002.08.27
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
	#include <avr/signal.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"
#include "buffer.h"
#include "rprintf.h"
#include "gps.h"

#include "nmea.h"

// Program ROM constants

// Global variables
extern GpsInfoType GpsInfo;
u08 NmeaPacket[NMEA_BUFFERSIZE];

void nmeaInit(void)
{
}

u08* nmeaGetPacketBuffer(void)
{
	return NmeaPacket;
}

u08 nmeaProcess(cBuffer* rxBuffer)
{
	u08 foundpacket = NMEA_NODATA;
	u08 startFlag = FALSE;
	//u08 data;
	u16 i,j;

	// process the receive buffer
	// go through buffer looking for packets
	while(rxBuffer->datalength)
	{
		// look for a start of NMEA packet
		if(bufferGetAtIndex(rxBuffer,0) == '$')
		{
			// found start
			startFlag = TRUE;
			// when start is found, we leave it intact in the receive buffer
			// in case the full NMEA string is not completely received.  The
			// start will be detected in the next nmeaProcess iteration.

			// done looking for start
			break;
		}
		else
			bufferGetFromFront(rxBuffer);
	}
	
	// if we detected a start, look for end of packet
	if(startFlag)
	{
		for(i=1; i<(rxBuffer->datalength)-1; i++)
		{
			// check for end of NMEA packet <CR><LF>
			if((bufferGetAtIndex(rxBuffer,i) == '\r') && (bufferGetAtIndex(rxBuffer,i+1) == '\n'))
			{
				// have a packet end
				// dump initial '$'
				bufferGetFromFront(rxBuffer);
				// copy packet to NmeaPacket
				for(j=0; j<(i-1); j++)
				{
					// although NMEA strings should be 80 characters or less,
					// receive buffer errors can generate erroneous packets.
					// Protect against packet buffer overflow
					if(j<(NMEA_BUFFERSIZE-1))
						NmeaPacket[j] = bufferGetFromFront(rxBuffer);
					else
						bufferGetFromFront(rxBuffer);
				}
				// null terminate it
				NmeaPacket[j] = 0;
				// dump <CR><LF> from rxBuffer
				bufferGetFromFront(rxBuffer);
				bufferGetFromFront(rxBuffer);

				#ifdef NMEA_DEBUG_PKT
				rprintf("Rx NMEA packet type: ");
				rprintfStrLen(NmeaPacket, 0, 5);
				rprintfStrLen(NmeaPacket, 5, (i-1)-5);
				rprintfCRLF();
				#endif
				// found a packet
				// done with this processing session
				foundpacket = NMEA_UNKNOWN;
				break;
			}
		}
	}

	if(foundpacket)
	{
		// check message type and process appropriately
		if(!strncmp(NmeaPacket, "GPGGA", 5))
		{
			// process packet of this type
			nmeaProcessGPGGA(NmeaPacket);
			// report packet type
			foundpacket = NMEA_GPGGA;
		}
		else if(!strncmp(NmeaPacket, "GPVTG", 5))
		{
			// process packet of this type
			nmeaProcessGPVTG(NmeaPacket);
			// report packet type
			foundpacket = NMEA_GPVTG;
		}
	}
	else if(rxBuffer->datalength >= rxBuffer->size)
	{
		// if we found no packet, and the buffer is full
		// we're logjammed, flush entire buffer
		bufferFlush(rxBuffer);
	}
	return foundpacket;
}

void nmeaProcessGPGGA(u08* packet)
{
	u08 i;
	char* endptr;
	double degrees, minutesfrac;

	#ifdef NMEA_DEBUG_GGA
	rprintf("NMEA: ");
	rprintfStr(packet);
	rprintfCRLF();
	#endif

	// start parsing just after "GPGGA,"
	i = 6;
	// attempt to reject empty packets right away
	if(packet[i]==',' && packet[i+1]==',')
		return;

	// get UTC time [hhmmss.sss]
	GpsInfo.PosLLA.TimeOfFix.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: latitude
	
	// get latitude [ddmm.mmmmm]
	GpsInfo.PosLLA.lat.f = strtod(&packet[i], &endptr);
	// convert to pure degrees [dd.dddd] format
	minutesfrac = modf(GpsInfo.PosLLA.lat.f/100, &degrees);
	GpsInfo.PosLLA.lat.f = degrees + (minutesfrac*100)/60;
	// convert to radians
	GpsInfo.PosLLA.lat.f *= (M_PI/180);
	while(packet[i++] != ',');				// next field: N/S indicator
	
	// correct latitute for N/S
	if(packet[i] == 'S') GpsInfo.PosLLA.lat.f = -GpsInfo.PosLLA.lat.f;
	while(packet[i++] != ',');				// next field: longitude
	
	// get longitude [ddmm.mmmmm]
	GpsInfo.PosLLA.lon.f = strtod(&packet[i], &endptr);
	// convert to pure degrees [dd.dddd] format
	minutesfrac = modf(GpsInfo.PosLLA.lon.f/100, &degrees);
	GpsInfo.PosLLA.lon.f = degrees + (minutesfrac*100)/60;
	// convert to radians
	GpsInfo.PosLLA.lon.f *= (M_PI/180);
	while(packet[i++] != ',');				// next field: E/W indicator

	// correct latitute for E/W
	if(packet[i] == 'W') GpsInfo.PosLLA.lon.f = -GpsInfo.PosLLA.lon.f;
	while(packet[i++] != ',');				// next field: position fix status

	// position fix status
	// 0 = Invalid, 1 = Valid SPS, 2 = Valid DGPS, 3 = Valid PPS
	// check for good position fix
	if( (packet[i] != '0') && (packet[i] != ',') )
		GpsInfo.PosLLA.updates++;
	while(packet[i++] != ',');				// next field: satellites used
	
	// get number of satellites used in GPS solution
	GpsInfo.numSVs = atoi(&packet[i]);
	while(packet[i++] != ',');				// next field: HDOP (horizontal dilution of precision)
	while(packet[i++] != ',');				// next field: altitude
	
	// get altitude (in meters)
	GpsInfo.PosLLA.alt.f = strtod(&packet[i], &endptr);

	while(packet[i++] != ',');				// next field: altitude units, always 'M'
	while(packet[i++] != ',');				// next field: geoid seperation
	while(packet[i++] != ',');				// next field: seperation units
	while(packet[i++] != ',');				// next field: DGPS age
	while(packet[i++] != ',');				// next field: DGPS station ID
	while(packet[i++] != '*');				// next field: checksum
}

void nmeaProcessGPVTG(u08* packet)
{
	u08 i;
	char* endptr;

	#ifdef NMEA_DEBUG_VTG
	rprintf("NMEA: ");
	rprintfStr(packet);
	rprintfCRLF();
	#endif

	// start parsing just after "GPVTG,"
	i = 6;
	// attempt to reject empty packets right away
	if(packet[i]==',' && packet[i+1]==',')
		return;

	// get course (true north ref) in degrees [ddd.dd]
	GpsInfo.VelHS.heading.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'T'
	while(packet[i++] != ',');				// next field: course (magnetic north)

	// get course (magnetic north ref) in degrees [ddd.dd]
	//GpsInfo.VelHS.heading.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'M'
	while(packet[i++] != ',');				// next field: speed (knots)

	// get speed in knots
	//GpsInfo.VelHS.speed.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'N'
	while(packet[i++] != ',');				// next field: speed (km/h)

	// get speed in km/h
	GpsInfo.VelHS.speed.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'K'
	while(packet[i++] != '*');				// next field: checksum

	GpsInfo.VelHS.updates++;
}

