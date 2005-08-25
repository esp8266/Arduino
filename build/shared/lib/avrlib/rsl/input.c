/*! \file input.c \brief User-Input Functions. */
//*****************************************************************************
//
// File Name	: 'input.c'
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

//----- Include Files ---------------------------------------------------------
#include "global.h"		// include our global settings
#ifdef __AVR_ATmega128__
#include "uart2.h"
#else
#include "uart.h"
#endif
#include "rprintf.h"	// include printf function library

#include "input.h"

// globals

// functions
u08 inputString(u08 termChar, u08 termLen, u08* data)
{
	u08 s=0;
	u08 length=0;

	while(length < termLen)
	{
		// get input
		#ifdef __AVR_ATmega128__
		while(!uartReceiveByte(1,&s));
		#else
		while(!uartReceiveByte(&s));
		#endif
	
		// handle special characters
		if(s == 0x08)			// backspace
		{
			if(length > 0)
			{
				// echo backspace-space-backspace
				rprintfChar(0x08);
				rprintfChar(' ');
				rprintfChar(0x08);
				length--;
			}
		}
		else if(s == termChar)	// termination character
		{
			// save null-termination
			data[length] = 0;
			break;
		}
		else
		{
			// echo character
			rprintfChar(s);
			// save character
			data[length++] = s;
		}
	}
	return length;
}

u08 asciiHexToByte(u08* string)
{
	// convert 2-byte hex string to byte
	return (asciiHexToNibble(string[0])<<4) + asciiHexToNibble(string[1]);
}

u08 asciiHexToNibble(u08 character)
{
	// convert 1-byte hex ascii character to nibble
	if((character >= 0x30) && (character <= 0x39))
		return character-0x30;
	else if((character >=  'A') && (character <=  'F'))
		return character-'A'+10;
	else if((character >=  'a') && (character <=  'f'))
		return character-'a'+10;
	else return 0;
}
