/*! \file nmea.h \brief NMEA protocol function library. */
//*****************************************************************************
//
// File Name	: 'nmea.h'
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

#ifndef NMEA_H
#define NMEA_H

#include "global.h"
#include "buffer.h"

// constants/macros/typdefs
#define NMEA_BUFFERSIZE		80

// Message Codes
#define NMEA_NODATA		0	// No data. Packet not available, bad, or not decoded
#define NMEA_GPGGA		1	// Global Positioning System Fix Data
#define NMEA_GPVTG		2	// Course over ground and ground speed
#define NMEA_GPGLL		3	// Geographic position - latitude/longitude
#define NMEA_GPGSV		4	// GPS satellites in view
#define NMEA_GPGSA		5	// GPS DOP and active satellites
#define NMEA_GPRMC		6	// Recommended minimum specific GPS data
#define NMEA_UNKNOWN	0xFF// Packet received but not known

// Debugging
//#define NMEA_DEBUG_PKT	///< define to enable debug of all NMEA messages
//#define NMEA_DEBUG_GGA	///< define to enable debug of GGA messages
//#define NMEA_DEBUG_VTG	///< define to enable debug of VTG messages

// functions
void nmeaInit(void);
u08* nmeaGetPacketBuffer(void);
u08 nmeaProcess(cBuffer* rxBuffer);
void nmeaProcessGPGGA(u08* packet);
void nmeaProcessGPVTG(u08* packet);

#endif
