/*! \file a2d.c \brief Analog-to-Digital converter function library. */
//*****************************************************************************
//
// File Name	: 'a2d.c'
// Title		: Analog-to-digital converter functions
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002-04-08
// Revised		: 2002-09-30
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "global.h"
#include "a2d.h"

// global variables

//! software flag used to indicate when
//! the a2d conversion is complete
volatile unsigned char a2dCompleteFlag;

// functions

//! initialize a2d converter
void a2dInit(void)
{
	sbi(ADCSR, ADEN);				// enable ADC (turn on ADC power)
	cbi(ADCSR, ADFR);				// default to single sample convert mode
	a2dSetPrescaler(ADC_PRESCALE);	// set default prescaler
	a2dSetReference(ADC_REFERENCE);	// set default reference
	cbi(ADMUX, ADLAR);				// set to right-adjusted result

	sbi(ADCSR, ADIE);				// enable ADC interrupts

	a2dCompleteFlag = FALSE;		// clear conversion complete flag
	sei();							// turn on interrupts (if not already on)
}

//! turn off a2d converter
void a2dOff(void)
{
	cbi(ADCSR, ADIE);				// disable ADC interrupts
	cbi(ADCSR, ADEN);				// disable ADC (turn off ADC power)
}

//! configure A2D converter clock division (prescaling)
void a2dSetPrescaler(unsigned char prescale)
{
	outb(ADCSR, ((inb(ADCSR) & ~ADC_PRESCALE_MASK) | prescale));
}

//! configure A2D converter voltage reference
void a2dSetReference(unsigned char ref)
{
	outb(ADMUX, ((inb(ADMUX) & ~ADC_REFERENCE_MASK) | (ref<<6)));
}

//! sets the a2d input channel
void a2dSetChannel(unsigned char ch)
{
	outb(ADMUX, (inb(ADMUX) & ~ADC_MUX_MASK) | (ch & ADC_MUX_MASK));	// set channel
}

//! start a conversion on the current a2d input channel
void a2dStartConvert(void)
{
	sbi(ADCSR, ADIF);	// clear hardware "conversion complete" flag 
	sbi(ADCSR, ADSC);	// start conversion
}

//! return TRUE if conversion is complete
u08 a2dIsComplete(void)
{
	return bit_is_set(ADCSR, ADSC);
}

//! Perform a 10-bit conversion
// starts conversion, waits until conversion is done, and returns result
unsigned short a2dConvert10bit(unsigned char ch)
{
	a2dCompleteFlag = FALSE;				// clear conversion complete flag
	outb(ADMUX, (inb(ADMUX) & ~ADC_MUX_MASK) | (ch & ADC_MUX_MASK));	// set channel
	sbi(ADCSR, ADIF);						// clear hardware "conversion complete" flag 
	sbi(ADCSR, ADSC);						// start conversion
	//while(!a2dCompleteFlag);				// wait until conversion complete
	//while( bit_is_clear(ADCSR, ADIF) );		// wait until conversion complete
	while( bit_is_set(ADCSR, ADSC) );		// wait until conversion complete

	// CAUTION: MUST READ ADCL BEFORE ADCH!!!
	return (inb(ADCL) | (inb(ADCH)<<8));	// read ADC (full 10 bits);
}

//! Perform a 8-bit conversion.
// starts conversion, waits until conversion is done, and returns result
unsigned char a2dConvert8bit(unsigned char ch)
{
	// do 10-bit conversion and return highest 8 bits
	return a2dConvert10bit(ch)>>2;			// return ADC MSB byte
}

//! interrupt handler for ADC complete interrupt
SIGNAL(SIG_ADC)
{
	// set the a2d conversion flag to indicate "complete"
	a2dCompleteFlag = TRUE;
}

