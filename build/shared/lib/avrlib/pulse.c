/*! \file pulse.c \brief Pulse/frequency generation function library. */
//*****************************************************************************
//
// File Name	: 'pulse.c'
// Title		: Pulse/frequency generation function library
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 2002-08-19
// Revised		: 2003-05-29
// Version		: 0.7
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
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
#include "timer.h"
#include "pulse.h"

// Global variables
// pulse generation registers
volatile static unsigned char  PulseT1AMode;
volatile static unsigned short PulseT1ACount;
volatile static unsigned short PulseT1APeriodTics;
volatile static unsigned char  PulseT1BMode;
volatile static unsigned short PulseT1BCount;
volatile static unsigned short PulseT1BPeriodTics;

// pulse mode bit definitions
// PULSE_MODE_COUNTED
//		if true, the requested number of pulses are output, then output is turned off
//		if false, pulses are output continuously
#define PULSE_MODE_CONTINUOUS	0x00
#define PULSE_MODE_COUNTED		0x01

// functions

void pulseInit(void)
{
	// initialize timer1 for pulse operation
	pulseT1Init();
}

void pulseT1Init(void)
{
	// try to make sure that timer1 is in "normal" mode
	// most importantly, turn off PWM mode
	timer1PWMOff();

	// set some reasonable initial values
	// in case the user forgets to
	PulseT1AMode = 0;
	PulseT1BMode = 0;
	PulseT1ACount = 0;
	PulseT1BCount = 0;
	PulseT1APeriodTics = 0x8000;
	PulseT1BPeriodTics = 0x8000;

	// attach the pulse service routines to
	// the timer 1 output compare A and B interrupts
	timerAttach(TIMER1OUTCOMPAREA_INT,pulseT1AService);
	timerAttach(TIMER1OUTCOMPAREB_INT,pulseT1BService);
}

void pulseT1Off(void)
{
	// turns pulse outputs off immediately
	
	// set pulse counters to zero (finished)
	PulseT1ACount = 0;
	PulseT1BCount = 0;
	// disconnect OutputCompare action from OC1A pin
	cbi(TCCR1A,COM1A1);
	cbi(TCCR1A,COM1A0);
	// disconnect OutputCompare action from OC1B pin
	cbi(TCCR1A,COM1B1);
	cbi(TCCR1A,COM1B0);
	// detach the pulse service routines
	timerDetach(TIMER1OUTCOMPAREA_INT);
	timerDetach(TIMER1OUTCOMPAREB_INT);
}

void pulseT1ASetFreq(u16 freqHz)
{
	// set the frequency of the pulse output
	// we need to find the requested period/2 (in timer tics)
	// from the frequency (in hertz)

	// calculate how many tics in period/2
	// this is the (timer tic rate)/(2*requested freq)
	PulseT1APeriodTics = ((u32)F_CPU/((u32)timer1GetPrescaler()*2*freqHz));
}

void pulseT1BSetFreq(u16 freqHz)
{
	// set the frequency of the pulse output
	// we need to find the requested period/2 (in timer tics)
	// from the frequency (in hertz)

	// calculate how many tics in period/2
	// this is the (timer tic rate)/(2*requested freq)
	PulseT1BPeriodTics = ((u32)F_CPU/((u32)timer1GetPrescaler()*2*freqHz));
}

void pulseT1ARun(u16 nPulses)
{
	// set the number of pulses we want and the mode
	if(nPulses)
	{
		// if the nPulses is non-zero, use "counted" mode
		PulseT1AMode |= PULSE_MODE_COUNTED;
		PulseT1ACount = nPulses<<1;
	}
	else
	{
		// if nPulses is zero, run forever
		PulseT1AMode &= ~PULSE_MODE_COUNTED;
		PulseT1ACount = 1<<1;
	}
	// set OutputCompare action to toggle OC1A pin
	cbi(TCCR1A,COM1A1);
	sbi(TCCR1A,COM1A0);

	// now the "enabling" stuff

	// set the output compare one pulse cycle ahead of current timer position 
	// to make sure we don't have to wait until the timer overflows and comes
	// back to the current value
	// set future output compare time to TCNT1 + PulseT1APeriodTics
	//outw(OCR1A, inw(TCNT1) + PulseT1APeriodTics);
	OCR1A += PulseT1APeriodTics;

	// enable OutputCompare interrupt
	sbi(TIMSK, OCIE1A);
}

void pulseT1BRun(u16 nPulses)
{
	// set the number of pulses we want and the mode
	if(nPulses)
	{
		// if the nPulses is non-zero, use "counted" mode
		PulseT1BMode |= PULSE_MODE_COUNTED;
		PulseT1BCount = nPulses<<1;
	}
	else
	{
		// if nPulses is zero, run forever
		PulseT1BMode &= ~PULSE_MODE_COUNTED;
		PulseT1BCount = 1<<1;
	}
	// set OutputCompare action to toggle OC1B pin
	// (note: with all the A's and B's flying around, TCCR1A is not a bug)
	cbi(TCCR1A,COM1B1);
	sbi(TCCR1A,COM1B0);

	// now the "enabling" stuff

	// set the output compare one pulse cycle ahead of current timer position 
	// to make sure we don't have to wait until the timer overflows and comes
	// back to the current value
	// set future output compare time to TCNT1 + PulseT1APeriodTics
	//outw(OCR1B, inw(TCNT1) + PulseT1BPeriodTics);
	OCR1B += PulseT1BPeriodTics;

	// enable OutputCompare interrupt
	sbi(TIMSK, OCIE1B);
}

void pulseT1AStop(void)
{
	// stop output regardless of remaining pulses or mode
	// go to "counted" mode
	PulseT1AMode |= PULSE_MODE_COUNTED;
	// set pulses to zero
	PulseT1ACount = 0;
}

void pulseT1BStop(void)
{
	// stop output regardless of remaining pulses or mode
	// go to "counted" mode
	PulseT1BMode |= PULSE_MODE_COUNTED;
	// set pulses to zero
	PulseT1BCount = 0;
}

u16 pulseT1ARemaining(void)
{
	// return the number of pulses remaining for channel A
	// add 1 to make sure we round up, >>1 equivalent to /2
	return (PulseT1ACount+1)>>1;
}

u16 pulseT1BRemaining(void)
{
	// return the number of pulses remaining for channel A
	// add 1 to make sure we round up, >>1 equivalent to /2
	return (PulseT1BCount+1)>>1;
}

void pulseT1AService(void)
{
	// check if TimerPulseACount is non-zero
	//		(i.e. pulses are still requested)
	if(PulseT1ACount)
	{
		//u16 OCValue;
		// read in current value of output compare register OCR1A
		//OCValue =  inp(OCR1AL);		// read low byte of OCR1A
		//OCValue += inp(OCR1AH)<<8;	// read high byte of OCR1A
		// increment OCR1A value by PulseT1APeriodTics
		//OCValue += PulseT1APeriodTics;
		// set future output compare time to this new value
		//outp((OCValue>>8),		OCR1AH);	// write high byte
		//outp((OCValue & 0x00FF),OCR1AL);	// write low byte

		// the following line should be identical in operation
		// to the lines above, but for the moment, I'm not convinced
		// this method is bug-free.  At least it's simpler!
		//outw(OCR1A, inw(OCR1A) + PulseT1APeriodTics);
		// change again
		OCR1A += PulseT1APeriodTics;
						
		// decrement the number of pulses executed
		if(PulseT1AMode & PULSE_MODE_COUNTED)
			PulseT1ACount--;
	}
	else
	{
		// pulse count has reached zero
		// disable the output compare's action on OC1A pin
		cbi(TCCR1A,COM1A1);
		cbi(TCCR1A,COM1A0);
		// and disable the output compare's interrupt to stop pulsing
		cbi(TIMSK, OCIE1A);
	}
}

void pulseT1BService(void)
{
	// check if TimerPulseACount is non-zero
	//		(i.e. pulses are still requested)
	if(PulseT1BCount)
	{
		//u16 OCValue;
		// read in current value of output compare register OCR1B
		//OCValue =  inp(OCR1BL);		// read low byte of OCR1B
		//OCValue += inp(OCR1BH)<<8;	// read high byte of OCR1B
		// increment OCR1B value by PulseT1BPeriodTics
		//OCValue += PulseT1BPeriodTics; 
		// set future output compare time to this new value
		//outp((OCValue>>8),		OCR1BH);	// write high byte
		//outp((OCValue & 0x00FF),OCR1BL);	// write low byte

		// the following line should be identical in operation
		// to the lines above, but for the moment, I'm not convinced
		// this method is bug-free.  At least it's simpler!
		//outw(OCR1B, inw(OCR1B) + PulseT1BPeriodTics);
		// change again
		OCR1B += PulseT1BPeriodTics;

		
		// decrement the number of pulses executed
		if(PulseT1BMode & PULSE_MODE_COUNTED)
			PulseT1BCount--;
	}
	else
	{
		// pulse count has reached zero
		// disable the output compare's action on OC1B pin
		cbi(TCCR1A,COM1B1);
		cbi(TCCR1A,COM1B0);
		// and disable the output compare's interrupt to stop pulsing
		cbi(TIMSK, OCIE1B);
	}
}
