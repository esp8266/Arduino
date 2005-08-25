/*! \file avrcore.c \brief AVR-Core Board Driver Functions. */
//*****************************************************************************
//
// File Name	: 'avrcore.c'
// Title		: AVR-Core Board Driver Functions
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.10.1
// Revised		: 2004.10.1
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>	// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "avrcore.h"

// globals
u08 AvrcoreLatch;

// functions
void avrcoreInit(void)
{
	// initialize ports to input with pullup
	// (this is done to avoid contentions and input-pin oscillation)
	outb(DDRA, 0x00);
	outb(DDRB, 0x00);
	outb(DDRC, 0x00);
	outb(DDRD, 0x00);
	outb(DDRE, 0x00);
	outb(DDRF, 0x00);
	outb(PORTA, 0xFF);
	outb(PORTB, 0xFF);
	outb(PORTC, 0xFF);
	outb(PORTD, 0xFF);
	outb(PORTE, 0xFF);
	outb(PORTF, 0xFF);
	// turn on RAM interface
	sbi(MCUCR, SRE);
	// initialize RAM page
	avrcoreSetRamPage(0);
	// initialize LEDs
	avrcoreSetLeds(0);
	// set serial power to on by default
	avrcoreSetSerialPortPower(1);
}

void avrcoreSetRamPage(u08 page)
{
	// update latch state
	AvrcoreLatch &= ~AVRCORELATCH_ADDRMASK;
	AvrcoreLatch |= page & AVRCORELATCH_ADDRMASK;
	// write new latch state to latch
	AVRCORELATCH = AvrcoreLatch;
}

void avrcoreSetLeds(u08 leds)
{
	// NOTE: LEDs are negative-logic (active-low)
	// update latch state
	AvrcoreLatch |= AVRCORELATCH_LEDMASK;
	AvrcoreLatch &= ~(leds<<4);
	// write new latch state to latch
	AVRCORELATCH = AvrcoreLatch;
}

void avrcoreSetLedsOn(u08 leds)
{
	// NOTE: LEDs are negative-logic (active-low)
	// update latch state to turn on inidicated leds
	AvrcoreLatch &= ~(leds<<4);
	// write new latch state to latch
	AVRCORELATCH = AvrcoreLatch;
}

void avrcoreSetLedsOff(u08 leds)
{
	// NOTE: LEDs are negative-logic (active-low)
	// update latch state to turn off inidicated leds
	AvrcoreLatch |= (leds<<4);
	// write new latch state to latch
	AVRCORELATCH = AvrcoreLatch;
}

void avrcoreSetSerialPortPower(u08 on)
{
	// this function simply manipulates LED3/power control
	if(on)
		AvrcoreLatch &= ~(0x80);
	else
		AvrcoreLatch |= (0x80);
	// write new latch state to latch
	AVRCORELATCH = AvrcoreLatch;
}
