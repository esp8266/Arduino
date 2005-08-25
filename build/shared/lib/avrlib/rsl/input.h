/*! \file input.h \brief User-Input Functions. */
//*****************************************************************************
//
// File Name	: 'input.h'
// Title		: User-Input Functions
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.09.11
// Revised		: 2003.09.11
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef INPUT_H
#define INPUT_H

// defines and typedefs

// functions
u08 inputString(u08 termChar, u08 termLen, u08* data);
u08 asciiHexToByte(u08* string);
u08 asciiHexToNibble(u08 character);

#endif
