/*! \file pulse.h \brief Pulse/frequency generation function library. */
//*****************************************************************************
//
// File Name	: 'pulse.h'
// Title		: Pulse/frequency generation function library
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 2002-08-19
// Revised		: 2003-05-29
// Version		: 0.7
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Description	: This library is designed to facilitate the output of square
//		wave pulses at a frequency determined by the user.  The library uses
//		the AVR processor built-in timers and the output is on the timer Output
//		Compare (OC) pins.
//
//		The allowable range of frequencies which can be generated is governed
//		by the tic rate of the timer (therefore the CPU clock rate and the
//		timer prescaler), and the computing speed of the processor itself. See
//		the SetFreq commands for more details.
//
//		NOTE: in order for the pulse library to work, pulseInit() will attach
//		the pulse service routines to the timer interrupts using the
//		timerAttach function. You must not detach the service routines during
//		pulse library operation.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef PULSE_H
#define PULSE_H

#include "global.h"

// constants/macros/typdefs

// functions

// Master Pulse Commands
// pulseInit()
//		Initializes the pulse system/library.
void pulseInit(void);

// Pulse commands for timer1
// pulseT1Init()
//		configures the timer1 hardware to produce pulses on pins OC1A and OC1B.
//		A "pulse" is considered to be one high and low period of a square wave.
void pulseT1Init(void);

// pulseT1Off()
//		Turns pulse output off immediately (cancels running pulse operations).
//		Unconfigures hardware and interrupts.
void pulseT1Off(void);

//	pulseT1ASetFreq() and pulseT1BSetFreq()
//		sets the frequency in hertz for each channel of square-wave pulse output
//		Note1: the freqency <freqHz> must always be greater than zero
//		Note2: both channels share the same frequency range which is governed
//		by the timer1 prescaler setting.  A prescaler setting of DIV/8 allows
//		frequencies of a few hertz through a few kilohertz.
//
//		Lower frequency bound = overflow rate of timer1 at current prescaling
//		Upper frequency bound = the tics rate of timer1 at current prescaling,
//								or approx. the (clock rate of the processor)/50,
//								whichever is smaller
void pulseT1ASetFreq(u16 freqHz);
void pulseT1BSetFreq(u16 freqHz);

// pulseT1ARun() and pulseT1BRun();
//		Sets the number of square-wave pulses to be output on the given channel.
//		For continuous (unlimited) pulse output, use nPulses = 0.  Pulses begin
//		coming out immediately.
//		Note: <nPulses> must be between 0 and 32767
void pulseT1ARun(u16 nPulses);
void pulseT1BRun(u16 nPulses);

// pulseT1AStop() and pulseT1BStop();
//		Stop pulse output at the next cycle (regardless of the number of
//		remaining pulses).
void pulseT1AStop(void);
void pulseT1BStop(void);

// pulseT1ARemaining() and pulseT1BRemaining()
//		Returns the number of pulses remaining to be output for each channel.
//		This function is useful for figuring out if the pulses are done.
u16  pulseT1ARemaining(void);
u16  pulseT1BRemaining(void);

// pulseT1AService() and pulseT1BService()
//		Interrupt service routines for pulse output (do not call these functions directly)
void pulseT1AService(void);
void pulseT1BService(void);


#endif
