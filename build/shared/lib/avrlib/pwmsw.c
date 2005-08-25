/*! \file pwmsw.c \brief Software interrupt-driven multi-output PWM function library. */
//*****************************************************************************
//
// File Name	: 'pwmsw.c'
// Title		: Software interrupt-driven multi-output PWM function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 7/20/2002
// Revised		: 7/31/2002
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// WARNING: this PWM library does not work perfectly.  It has known and
//	understood problems when two or more PWM outputs are set to nearly the
//	same duty cycle.  IT MAY NOT BE WORTH USING!  YOU HAVE BEEN WARNED!
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
	#include <avr/io.h>
#endif

#include "global.h"
#include "pwmsw.h"

// Program ROM constants

// Global variables
// PWM channel registers
u16 PosTics;
u16 PeriodTics;
u08 Channel;
SwPwmChannelType SwPwmChannels[SWPWM_NUM_CHANNELS];

// functions

// initializes software PWM system
void pwmswInit(u16 periodTics)
{
	u08 index;

	// attach the software PWM service routine to timer1 output compare A
	timerAttach(TIMER1OUTCOMPAREA_INT, pwmswService);
	// set PeriodTics
	PeriodTics = periodTics;
	// set PosTics
	PosTics = 0;
	// clear channels
	for(index=0; index<SWPWM_NUM_CHANNELS; index++)
	{
		SwPwmChannels[index].duty = 0;
		SwPwmChannels[index].setduty = 0;
	}
	// set initial interrupt time
	u16 OCValue;
	// read in current value of output compare register OCR1A
	OCValue =  inb(OCR1AL);		// read low byte of OCR1A
	OCValue += inb(OCR1AH)<<8;	// read high byte of OCR1A
	// increment OCR1A value by nextTics
	OCValue += PeriodTics; 
	// set future output compare time to this new value
	outb(OCR1AH, (OCValue>>8));			// write high byte
	outb(OCR1AL, (OCValue & 0x00FF));	// write low byte

	// enable the timer1 output compare A interrupt
	sbi(TIMSK, OCIE1A);
}

// turns off software PWM system
void pwmswOff(void)
{
	// disable the timer1 output compare A interrupt
	cbi(TIMSK, OCIE1A);
	// detach the service routine
	timerDetach(TIMER1OUTCOMPAREA_INT);
}

// set duty on channel
void pwmswPWMSet(u08 channel, u16 duty)
{
	// compare with max value of PeriodTics
	duty = MIN(duty, PeriodTics);
	SwPwmChannels[channel].setduty = duty;
}

void pwmswService(void)
{
	u16 nextTics=PeriodTics;
	u08 index;

	// check for beginning of period
	if(PosTics == 0)
	{
		// examine all channels
		for(index=0; index<SWPWM_NUM_CHANNELS; index++)
		{
			// transfer set-duty to active-duty
			SwPwmChannels[index].duty = SwPwmChannels[index].setduty;
			// find next channel event to schedule
			//		if no channel has a duty setting greater than 0 (PosTics);
			//		nextTics will remain at PeriodTics for the next cycle
			if(SwPwmChannels[index].duty)
			{
				nextTics = MIN(nextTics, SwPwmChannels[index].duty);
				// set all non-zero channels to on
				outb(SWPWMPORT, inb(SWPWMPORT) | (1<<index));
			}
		}
	}
	else
	{
		// examine all channels
		for(index=0; index<SWPWM_NUM_CHANNELS; index++)
		{
			// check if we have a duty cycle match
			if(PosTics == SwPwmChannels[index].duty)
			{
				// clear output channel
				outb(SWPWMPORT, inb(SWPWMPORT) & ~(1<<index));
			}
			// find next channel event to schedule
			//		if no channel has a duty setting greater than PosTics;
			//		nextTics will remain at PeriodTics and is handled below
			if(SwPwmChannels[index].duty > PosTics)
				nextTics = MIN(nextTics, SwPwmChannels[index].duty-PosTics);
		}
		if(nextTics == PeriodTics)
		{
			// no more channels to schedule
			// schedule next cycle
			nextTics = PeriodTics - PosTics;
		}
	}

	// schedule next interrupt
	u16 OCValue;
	// read in current value of output compare register OCR1A
	OCValue =  inb(OCR1AL);		// read low byte of OCR1A
	OCValue += inb(OCR1AH)<<8;	// read high byte of OCR1A
	// increment OCR1A value by nextTics
	OCValue += nextTics;
//	OCR1A+=nextTics;
	// set future output compare time to this new value
	outb(OCR1AH, (OCValue>>8));			// write high byte
	outb(OCR1AL, (OCValue & 0x00FF));	// write low byte
	// set our new tic position
	PosTics += nextTics;
	if(PosTics >= PeriodTics) PosTics -= PeriodTics;
}

