/*! \file rtc.c \brief Real-time clock function library. */
//*****************************************************************************
//
// File Name	: 'rtc.c'
// Title		: Real-time clock function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 5/10/2002
// Revised		: 9/30/2002
// Version		: 0.6
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

#include "global.h"
// include timer support
#ifdef __AVR_ATmega128__
	#include "timer128.h"
#else
	#include "timer.h"
#endif
// include rtc header
#include "rtc.h"

// Program ROM constants
static char __attribute__ ((progmem)) MonthDayTable[] = {31,28,31,30,31,30,31,31,30,31,30,31};

// Global variables
// time registers
RtcTimeType RtcTime;

void rtcInit(void)
{
	// set up timer for RTC operation
	// initialize real-time registers
	RtcTime.totaltics = 0;
	RtcTime.tics = 0;
	RtcTime.seconds = 0;
	RtcTime.minutes = 0;
	RtcTime.hours = 0;
	RtcTime.day = 1;
	RtcTime.month = 1;
	RtcTime.year = 2000;

	// select the correct RTC timer based on bit defines
	#ifdef AS2
		// use timer2 for most AVRs
		// initialize timer 2
		timer2Init();
		// count with 32.768KHz/8
		timer2SetPrescaler(TIMER_CLK_DIV8);
		// switch to asynchronous input (32KHz crystal)
		sbi(ASSR, AS2);
		// attach service to real-time clock interrupt
		// rtcService() will be called at ((32768/8)/256) = 16Hz
		timerAttach(TIMER2OVERFLOW_INT, rtcService);
	#else
	#ifdef AS0
		// use timer0 for ATmega103, ATmega128
		// initialize timer 0
		timer0Init();
		// count with 32.768KHz/8
		timer0SetPrescaler(TIMER_CLK_DIV8);
		// switch to asynchronous input (32KHz crystal)
		sbi(ASSR, AS0);
		// attach service to real-time clock interrupt
		// rtcService() will be called at ((32768/8)/256) = 16Hz
		timerAttach(TIMER0OVERFLOW_INT, rtcService);
	#endif
	#endif
}

void rtcService(void)
{
	// update real-time clock registers
	RtcTime.totaltics++;
	RtcTime.tics++;
	// check for overflows
	if(RtcTime.tics == 16)							// tics
	{
		RtcTime.tics = 0;
		RtcTime.seconds++;							// increment seconds
		if(RtcTime.seconds > 59)					// check seconds overflow
		{
			RtcTime.seconds -= 60;
			RtcTime.minutes++;						// increment minutes
			if(RtcTime.minutes > 59)				// check minutes overflow
			{
				RtcTime.minutes -= 60;
				RtcTime.hours++;					// increment hours
				if(RtcTime.hours > 23)				// check hours overflow
				{
					RtcTime.hours -= 24;
					RtcTime.day++;					// increment days
					// check days overflow
					if(RtcTime.day == pgm_read_byte(&MonthDayTable[RtcTime.month-1]))
					{
						RtcTime.day = 1;
						RtcTime.month++;			// increment months
						if(RtcTime.month == 13)		// check months overflow
						{
							RtcTime.month = 1;
							RtcTime.year++;			// increment years
						}
					}
				}
			}
		}
	}
}

RtcTimeType* rtcGetTime(void)
{
	// return the real-time clock data
	return &RtcTime;
}

