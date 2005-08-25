/*! \file avrcore.h \brief AVR-Core Board Driver Functions. */
//*****************************************************************************
//
// File Name	: 'avrcore.h'
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

#ifndef AVRCORE_H
#define AVRCORE_H

// defines and typedefs
#define AVRCORELATCH	(*((unsigned char*)0x4000))
#define AVRCORELATCH_ADDRMASK	0x0F
#define AVRCORELATCH_LEDMASK	0xF0

// functions

//! Initialize AVRCore hardware
void avrcoreInit(void);

//! Set the current external RAM page
//  The AVRCore on-board external RAM is typically 512KBytes.
//  The RAM is memory-mapped into the 32KByte address space from
//  0x8000-0xFFFF, and must therefore be accessed in pages (32KB chunks).
//  Use this function to select which of the 16 (0-15) 32KByte pages
//  you wish to access.
void avrcoreSetRamPage(u08 page);

//! Set the state of the four LEDs on AVRCore
//  leds bit0 => LED1 (0=off, 1=on)
//  leds bit1 => LED2 (0=off, 1=on)
//  leds bit2 => LED3 (0=off, 1=on)
//  leds bit3 => LED4 (0=off, 1=on)
void avrcoreSetLeds(u08 leds);

//! Turn on selected LEDs
//  '0' bit = no change
//  '1' bit = turn on
void avrcoreSetLedsOn(u08 leds);

//! Turn off selected LEDs
//  '0' bit = no change
//  '1' bit = turn off
void avrcoreSetLedsOff(u08 leds);

//! Set on/off power setting of AVRCore serial port
//  (0=off, 1=on)
void avrcoreSetSerialPortPower(u08 on);

#endif
