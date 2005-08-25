/*! \file servo.h \brief Interrupt-driven RC Servo function library. */
//*****************************************************************************
//
// File Name	: 'servo.h'
// Title		: Interrupt-driven RC Servo function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 7/31/2002
// Revised		: 8/02/2002
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: you need the latest version (3.2+) of the AVR-GCC compiler to use this
//	function library.  Download it from http://www.avrfreaks.net/AVRGCC
//
// Description : This code allows you to drive up to 8 RC servos from any
//		combination of ports and pins on the AVR processor. Using interrupts,
//		this code continuously sends control signals to the servo to maintain
//		position even while your code is doing other work.
//
//		The servoInit and servoOff effectively turn on and turn off servo
//		control.  When you run ServoInit, it automatically assigns each
//		"channel" of servo control to be output on the SERVO_DEFAULT_PORT.
//		One "channel" of servo control can control one servo and must be
//		assigned single I/O pin for output.
//
//		If you're using all eight channels (SERVO_NUM_CHANNELS = 8), then
//		then by default the code will use SERVO_DEFAULT_PORT pins 0-7.
//		If you're only using four channels, then pins 0-3 will be used by
//		default.
//
//		The command servoSetChannelIO(channel, port, pin) allows you to
//		reassign the output of any channel to any port and I/O pin you
//		choose.  For exampe, if you have an RC servo connected to PORTC, pin 6,
//		and you wish to use channel 2 to control it, use:
//
//		servoSerChannelIO( 2, _SFR_IO_ADDR(PORTC), 6)
//
//		(NOTE: you must include the "_SRF_IO_ADDR()" part around your port)
//
//		The servoSetPostion and servoGetPosition commands allow you to command
//		a given servo to your desired position.  The position you request must
//		lie between the SERVO_MIN and SERVO_MAX limit you defined.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef SERVO_H
#define SERVO_H

#include "global.h"
#include "timer.h"

// include configuration
#include "servoconf.h"

typedef struct struct_ServoChannel
{	
	// hardware I/O port and pin for this channel
	u08 port;
	u08 pin;
	// PWM duty setting which corresponds to servo position
	u16 duty;
} ServoChannelType;

// functions

// initializes servo system
//		You must run this to begin servo control
void servoInit(void);

// turns off servo system
//		This stops controlling the servos and
//		returns control of the SERVOPORT to your code
void servoOff(void);

// set the port and I/O pin you wish to use for a given channel
//		If you do not assign a port and I/O pin for a channel (ie. you don't
//		use this command) then all output will be done through the
//		SERVO_DEFAULT_PORT.  See above definition of SERVO_DEFAULT_PORT.
void servoSetChannelIO(u08 channel, u08 port, u08 pin);

// set and get servo position on a given channel 
//		servoSetPosition() commands the servo on <channel> to the position you
//			desire.  The position input must lie between 0 and POSITION_MAX and
//			will be automatically scaled to raw positions between SERVO_MIN and
//			SERVO_MAX
//		servoGetPosition() returns the most recently set postition of the
//			servo on <channel>.  The return value will be scaled 0->POSITION_MAX
void servoSetPosition(u08 channel, u08 position);
u08 servoGetPosition(u08 channel);

// set and get raw servo position on a given channel
//		Works like non-raw commands but position is not scaled.  Position must
//		be between SERVO_MIN and SERVO_MAX
void servoSetPositionRaw(u08 channel, u16 position);
u16 servoGetPositionRaw(u08 channel);

// servo interrupt service routine
void servoService(void);

#endif
