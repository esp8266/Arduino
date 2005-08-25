/*! \file servo.c \brief Interrupt-driven RC Servo function library. */
//*****************************************************************************
//
// File Name	: 'servo.c'
// Title		: Interrupt-driven RC Servo function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 7/31/2002
// Revised		: 8/02/2002
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
	#include <avr/io.h>
#endif

#include "global.h"
#include "servo.h"

// Program ROM constants

// Global variables
// servo channel registers
u16 ServoPosTics;
u16 ServoPeriodTics;
u08 ServoChannel;
ServoChannelType ServoChannels[SERVO_NUM_CHANNELS];

// functions

//! initializes software PWM system
void servoInit(void)
{
	u08 channel;
	// disble the timer1 output compare A interrupt
	cbi(TIMSK, OCIE1A);
	// set the prescaler for timer1
	timer1SetPrescaler(TIMER_CLK_DIV256);
	// attach the software PWM service routine to timer1 output compare A
	timerAttach(TIMER1OUTCOMPAREA_INT, servoService);
	// enable and clear channels
	for(channel=0; channel<SERVO_NUM_CHANNELS; channel++)
	{
		// set minimum position as default
		ServoChannels[channel].duty = SERVO_MIN;
		// set default port and pins assignments
		ServoChannels[channel].port = _SFR_IO_ADDR(SERVO_DEFAULT_PORT);
		//ServoChannels[channel].port = (unsigned char)&SERVO_DEFAULT_PORT;
		ServoChannels[channel].pin = (1<<channel);
		// set channel pin to output
		// THIS IS OBSOLETED BY THE DYNAMIC CHANNEL TO PORT,PIN ASSIGNMENTS
		//outb(SERVODDR, inb(SERVODDR) | (1<<channel));
	}
	// set PosTics
	ServoPosTics = 0;
	// set PeriodTics
	ServoPeriodTics = SERVO_MAX*9;
	// set initial interrupt time
	u16 OCValue;
	// read in current value of output compare register OCR1A
	OCValue =  inb(OCR1AL);		// read low byte of OCR1A
	OCValue += inb(OCR1AH)<<8;	// read high byte of OCR1A
	// increment OCR1A value by nextTics
	OCValue += ServoPeriodTics; 
	// set future output compare time to this new value
	outb(OCR1AH, (OCValue>>8));			// write high byte
	outb(OCR1AL, (OCValue & 0x00FF));	// write low byte
	// enable the timer1 output compare A interrupt
	sbi(TIMSK, OCIE1A);
}

//! turns off software PWM system
void servoOff(void)
{
	// disable the timer1 output compare A interrupt
	cbi(TIMSK, OCIE1A);
	// detach the service routine
	timerDetach(TIMER1OUTCOMPAREA_INT);
}

//! set port and I/O pin for channel
void servoSetChannelIO(u08 channel, u08 port, u08 pin)
{
	ServoChannels[channel].port = port;
	ServoChannels[channel].pin = (1<<(pin&0x07));
}

//! set servo position on channel
void servoSetPosition(u08 channel, u08 position)
{
	// input should be between 0 and SERVO_POSITION_MAX
	u16 pos_scaled;
	// calculate scaled position
	pos_scaled = ((u16)position*(SERVO_MAX-SERVO_MIN)/SERVO_POSITION_MAX)+SERVO_MIN;
	// set position
	servoSetPositionRaw(channel, pos_scaled);
}

//! get servo position on channel
u08 servoGetPosition(u08 channel)
{
	return (u08)( ((servoGetPositionRaw(channel)-SERVO_MIN)*SERVO_POSITION_MAX)/(SERVO_MAX-SERVO_MIN) );
}

//! set servo position on channel (raw unscaled format)
void servoSetPositionRaw(u08 channel, u16 position)
{
	// bind to limits
	position = MAX(position, SERVO_MIN);
	position = MIN(position, SERVO_MAX);
	// set position
	ServoChannels[channel].duty = position;
}

//! get servo position on channel (raw unscaled format)
u16 servoGetPositionRaw(u08 channel)
{
	return ServoChannels[channel].duty;
}

void servoService(void)
{
	u16 nextTics;

	if(ServoChannel < SERVO_NUM_CHANNELS)
	{
		// turn off current channel
		outb(_SFR_IO8(ServoChannels[ServoChannel].port), inb(_SFR_IO8(ServoChannels[ServoChannel].port)) & ~(ServoChannels[ServoChannel].pin));
	}
	
	// next channel
	ServoChannel++;

	if(ServoChannel != SERVO_NUM_CHANNELS)
	{
		// loop to channel 0 if needed
		if(ServoChannel > SERVO_NUM_CHANNELS)	ServoChannel = 0;
		// turn on new channel
		outb(_SFR_IO8(ServoChannels[ServoChannel].port), inb(_SFR_IO8(ServoChannels[ServoChannel].port)) | (ServoChannels[ServoChannel].pin));
		// schedule turn off time
		nextTics = ServoChannels[ServoChannel].duty;
	}
	else //(Channel == SERVO_NUM_CHANNELS)
	{
		// ***we could save time by precalculating this
		// schedule end-of-period
		nextTics = ServoPeriodTics-ServoPosTics;
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
	ServoPosTics += nextTics;
	if(ServoPosTics >= ServoPeriodTics) ServoPosTics = 0;
}
