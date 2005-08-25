/*! \file gps.c \brief GPS position storage and processing library. */
//*****************************************************************************
//
// File Name	: 'gps.c'
// Title		: GPS position storage and processing function library
// Author		: Pascal Stang - Copyright (C) 2002-2005
// Created		: 2005.01.14
// Revised		: 2002.07.17
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
	#include <math.h>
	#include <stdlib.h>
#endif

#include "global.h"
#include "rprintf.h"
#include "gps.h"

// Global variables
GpsInfoType GpsInfo;

// Functions
void gpsInit(void)
{
	
}

GpsInfoType* gpsGetInfo(void)
{
	return &GpsInfo;
}

void gpsInfoPrint(void)
{

	rprintfProgStrM("TOW:      ");	rprintfFloat(8, GpsInfo.TimeOfWeek.f);			rprintfCRLF();
	rprintfProgStrM("WkNum:    ");	rprintfNum(10,4,0,' ',GpsInfo.WeekNum);			rprintfCRLF();
	rprintfProgStrM("UTCoffset:");	rprintfFloat(8, GpsInfo.UtcOffset.f);			rprintfCRLF();
	rprintfProgStrM("Num SVs:  ");	rprintfNum(10,4,0,' ',GpsInfo.numSVs);			rprintfCRLF();

	rprintfProgStrM("X_ECEF:   ");	rprintfFloat(8, GpsInfo.PosECEF.x.f);			rprintfCRLF();
	rprintfProgStrM("Y_ECEF:   ");	rprintfFloat(8, GpsInfo.PosECEF.y.f);			rprintfCRLF();
	rprintfProgStrM("Z_ECEF:   ");	rprintfFloat(8, GpsInfo.PosECEF.z.f);			rprintfCRLF();
	rprintfProgStrM("TOF:      ");	rprintfFloat(8, GpsInfo.PosECEF.TimeOfFix.f);	rprintfCRLF();
	rprintfProgStrM("Updates:  ");	rprintfNum(10,6,0,' ',GpsInfo.PosECEF.updates);	rprintfCRLF();

	//u08 str[20];
	//rprintfProgStrM(" PosLat: ");		rprintfStr(dtostrf(GpsInfo.PosLat.f, 10, 5, str));
	rprintfProgStrM("PosLat:   ");	rprintfFloat(8, 180*(GpsInfo.PosLLA.lat.f/PI));	rprintfCRLF();
	rprintfProgStrM("PosLon:   ");	rprintfFloat(8, 180*(GpsInfo.PosLLA.lon.f/PI));	rprintfCRLF();
	rprintfProgStrM("PosAlt:   ");	rprintfFloat(8, GpsInfo.PosLLA.alt.f);			rprintfCRLF();
	rprintfProgStrM("TOF:      ");	rprintfFloat(8, GpsInfo.PosLLA.TimeOfFix.f);	rprintfCRLF();
	rprintfProgStrM("Updates:  ");	rprintfNum(10,6,0,' ',GpsInfo.PosLLA.updates);	rprintfCRLF();

	rprintfProgStrM("Vel East: ");	rprintfFloat(8, GpsInfo.VelENU.east.f);			rprintfCRLF();
	rprintfProgStrM("Vel North:");	rprintfFloat(8, GpsInfo.VelENU.north.f);		rprintfCRLF();
	rprintfProgStrM("Vel Up:   ");	rprintfFloat(8, GpsInfo.VelENU.up.f);			rprintfCRLF();
//	rprintfProgStrM("TOF:      ");	rprintfFloat(8, GpsInfo.VelENU.TimeOfFix.f);	rprintfCRLF();
	rprintfProgStrM("Updates:  ");	rprintfNum(10,6,0,' ',GpsInfo.VelENU.updates);	rprintfCRLF();

	rprintfProgStrM("Vel Head: ");	rprintfFloat(8, GpsInfo.VelHS.heading.f);		rprintfCRLF();
	rprintfProgStrM("Vel Speed:");	rprintfFloat(8, GpsInfo.VelHS.speed.f);			rprintfCRLF();
//	rprintfProgStrM("TOF:      ");	rprintfFloat(8, GpsInfo.VelHS.TimeOfFix.f);	rprintfCRLF();
	rprintfProgStrM("Updates:  ");	rprintfNum(10,6,0,' ',GpsInfo.VelHS.updates);	rprintfCRLF();

}


