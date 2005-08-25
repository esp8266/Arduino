/*! \file rtc.h \brief real-time clock function library. */
//*****************************************************************************
//
// File Name	: 'rtc.h'
// Title		: real-time clock function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 5/10/2002
// Revised		: 7/12/2002
// Version		: 0.5
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

#ifndef RTC_H
#define RTC_H

#include "global.h"

// constants/macros/typdefs
typedef struct struct_RtcTime
{	
	// hardware
	u08 tics;
	u16 totaltics;
	// time of day
	u08 hours;
	u08 minutes;
	u08 seconds;
	// date
	u08 day;
	u08 month;
	u16 year;
} RtcTimeType;


// functions
void rtcInit(void);
void rtcService(void);
RtcTimeType* rtcGetTime(void);

#endif
