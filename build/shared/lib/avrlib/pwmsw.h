/*! \file pwmsw.h \brief Software interrupt-driven multi-output PWM function library. */
//*****************************************************************************
//
// File Name	: 'pwmsw.h'
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

#ifndef PWMSW_H
#define PWMSW_H

#include "global.h"
#include "timer.h"

// constants/macros/typdefs
typedef struct struct_SwPwmChannel
{	
	u08 port;		///< channel's hardware I/O port
	u08 pin;		///< channel's hardware I/O pin
	u16 duty;		///< active PWM duty setting
	u16 setduty;	///< requested PWM duty setting
} SwPwmChannelType;

// number of PWM channels
#define SWPWM_NUM_CHANNELS		3
// define port 
#define SWPWMPORT				PORTB
#define SWPWMDDR				DDRB

// functions

//! initializes software PWM system
void pwmswInit(u16 periodTics);

//! turns off software PWM system
void pwmswOff(void);

//! set duty on channel
void pwmswPWMSet(u08 channel, u16 duty);

//! software PWM interrupt service routine
void pwmswService(void);

#endif
