/*! \file debug.c \brief Debugging function library. */
//*****************************************************************************
//
// File Name	: 'debug.c'
// Title		: Helpful debugging functions
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003-03-13
// Revised		: 2003-03-13
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Description	: This file contains a set of functions which may be useful
//		for general debugging.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "global.h"
#include "debug.h"

#include "rprintf.h"		// include printf support

// global variables

// functions

// Print a part of memory as a formatted hex table with ascii translation
void debugPrintHexTable(u16 length, u08 *buffer)
{
	u08 i;
	u16 j;
	u08 *buf;
	u08 s;

	buf = buffer;
	
	// print the low order address indicies and ASCII header
	rprintfProgStrM("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  0123456789ABCDEF\r\n");
	rprintfProgStrM("     -----------------------------------------------  ---- ASCII -----\r\n");
	
	// print the data
	for(j=0; j<((length+15)>>4); j++)
	{
		// print the high order address index for this line
		rprintfu16(j<<4);
		rprintfChar(' ');

		// print the hex data
		for(i=0; i<0x10; i++)
		{
			// be nice and print only up to the exact end of the data
			if( ((j<<4)+i) < length)
			{
				// print hex byte
				rprintfu08(buf[(j<<4)+i]);
				rprintfChar(' ');
			}
			else
			{
				// we're past the end of the data's length
				// print spaces
				rprintfProgStrM("   ");
			}
		}
		
		// leave some space
		rprintfChar(' ');

		// print the ascii data
		for(i=0; i<0x10; i++)
		{
			// be nice and print only up to the exact end of the data
			if( ((j<<4)+i) < length)
			{
				// get the character
				s = buf[(j<<4)+i]; 
				// make sure character is printable
				if(s >= 0x20)
					rprintfChar(s);
				else
					rprintfChar('.');
			}
			else
			{
				// we're past the end of the data's length
				// print a space
				rprintfChar(' ');
			}
		}
		rprintfCRLF();
	}
}
