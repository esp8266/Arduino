/*! \file encoder.h \brief Quadrature Encoder reader/driver. */
//*****************************************************************************
//
// File Name	: 'encoder.h'
// Title		: Quadrature Encoder reader/driver
// Author		: Pascal Stang - Copyright (C) 2003-2004
// Created		: 2003.01.26
// Revised		: 2004.06.25
// Version		: 0.3
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Description	: This library allows easy interfacing of quadrature encoders
//		to the Atmel AVR-series processors.
//
//		Quadrature encoders have two digital outputs usually called PhaseA and
//	PhaseB.  When the encoder rotates, PhaseA and PhaseB produce square wave
//	pulses where each pulse represents a fraction of a turn of the encoder
//	shaft.  Encoders are rated for a certain number of pulses (or counts) per
//	complete revolution of the shaft.  Common counts/revolution specs are 50,
//	100,128,200,250,256,500,etc.  By counting the number of pulses output on
//	one of the phases starting from time0, you can calculate the total
//	rotational distance the encoder has traveled.
//	
//  Often, however, we want current position not just total distance traveled.
//	For this it is necessary to know not only how far the encoder has traveled,
//	but also which direction it was going at each step of the way.  To do this
//	we need to use both outputs (or phases) of the quadrature encoder.
//
//  The pulses from PhaseA and PhaseB on quadrature encoders are always aligned
//	90 degrees out-of-phase (otherwise said: 1/4 wavelength apart).  This
//	special phase relationship lets us extract both the distance and direction
//	the encoder has rotated from the outputs.
//
//  To do this, consider Phase A to be the distance counter.  On each rising
//	edge of PhaseA we will count 1 "tic" of distance, but we need to know the
//	direction.  Look at the quadrature waveform plot below.  Notice that when
//	we travel forward in time (left->right), PhaseB is always low (logic 0) at
//	the rising edge of PhaseA.  When we travel backwards in time (right->left),
//	PhaseB is always high (logic 1) at the rising edge of PhaseA.  Note that
//	traveling forward or backwards in time is the same thing as rotating
//	forwards or bardwards. Thus, if PhaseA is our counter, PhaseB indicates
//	direction.
//
//	Here is an example waveform from a quadrature encoder:
/*
//                /---\   /---\   /---\   /---\   /---\   /---\
//  Phase A:      |   |   |   |   |   |   |   |   |   |   |   |
//             ---/   \---/   \---/   \---/   \---/   \---/   \-
//             -\   /---\   /---\   /---\   /---\   /---\   /---
//  Phase B:    |   |   |   |   |   |   |   |   |   |   |   |
//              \---/   \---/   \---/   \---/   \---/   \---/
//  Time:    <--------------------------------------------------->
//  Rotate FWD: >----------------------------------------------> 
//  Rotate REV: <----------------------------------------------<
*/
//	To keep track of the encoder position in software, we connect PhaseA to an
//	external processor interrupt line, and PhaseB to any I/O pin.  We set up
//	the external interrupt to trigger whenever PhaseA produces a rising edge.
//	When a rising edge is detected, our interrupt handler function is executed.
//	Inside the handler function, we quickly check the PhaseB line to see if it
//	is high or low.  If it is high, we increment the encoder's position
//	counter, otherwise we decrement it.  The encoder position counter can be
//	read at any time to find out the current position.
//
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef ENCODER_H
#define ENCODER_H

#include "global.h"

// include encoder configuration file
#include "encoderconf.h"

// constants/macros/typdefs

// defines for processor compatibility
// chose proper Interrupt Mask (IMSK)
#ifdef EIMSK
	#define IMSK	EIMSK	// for processors mega128, mega64
#else
	#define IMSK	GIMSK	// for other processors 90s8515, mega163, etc
#endif


//! Encoder state structure
//   stores the position and other information from each encoder
typedef struct struct_EncoderState
{	
	s32 position;		///< position
//	s32 velocity;		///< velocity
} EncoderStateType;


// functions

//! encoderInit() initializes hardware and encoder position readings
//		Run this init routine once before using any other encoder function.
void encoderInit(void);

//! encoderOff() disables hardware and stops encoder position updates
void encoderOff(void);

//! encoderGetPosition() reads the current position of the encoder 
s32 encoderGetPosition(u08 encoderNum);

//! encoderSetPosition() sets the current position of the encoder
void encoderSetPosition(u08 encoderNum, s32 position);

#endif
