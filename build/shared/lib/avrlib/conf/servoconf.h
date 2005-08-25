/*! \file servoconf.h \brief Interrupt-driven RC Servo configuration. */
//*****************************************************************************
//
// File Name	: 'servoconf.h'
// Title		: Interrupt-driven RC Servo function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 07/31/2002
// Revised		: 09/30/2002
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

#ifndef SERVOCONF_H
#define SERVOCONF_H

// set number of servo channels (1 to 8)
//		This is the number of servos you would like to drive
//		Each "Channel" can control one servo and by default will
//		map directly to the port pin of the same number on the
//		SERVO_DEFAULT_PORT.  You can change this default port/pin
//		assignment for a given channel to any port/pin you like.
//		See the "servoSetChannelIO" function.
#define SERVO_NUM_CHANNELS		4
// set default SERVO output port
//		This is the AVR port which you have connected to your servos 
//		See top of file for how servo "channels" map to port pins
#define SERVO_DEFAULT_PORT		PORTB
// set servo characteristics (min and max raw position)
//		You must find these by testing using your brand/type of servos.
//		The min/max settings will change proportional to F_CPU, the CPU
//		clock frequency.
// The numbers below good for parallax servos at an F_CPU of ~8MHz.
//#define SERVO_MAX				71
//#define SERVO_MIN				17
// The numbers below good for parallax servos at an F_CPU of ~14.745MHz.
#define SERVO_MAX				138
#define SERVO_MIN				34

// set servo scaled range
//		This sets the scaled position range of the servo.  Allowed scaled
//		positions are 0 -> SERVO_POSITION_MAX, and correspond to raw
//		positions of SERVO_MIN -> SERVO_MAX.
#define SERVO_POSITION_MAX		255

#endif
