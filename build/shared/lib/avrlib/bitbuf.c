/*! \file bitbuf.c \brief Multipurpose bit buffer structure and methods. */
//*****************************************************************************
//
// File Name	: 'bitbuf.c'
// Title		: Multipurpose bit buffer structure and methods
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 7/10/2002
// Revised		: 7/10/2002
// Version		: 0.5
// Target MCU	: any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "bitbuf.h"

// global variables

//! Initialize the bit buffer
//	sets the start location and size of the buffer in memory
void bitbufInit(BitBuf* bitBuffer, unsigned char *start, unsigned short bytesize)
{
	// set start pointer of the buffer
	bitBuffer->dataptr = start;
	bitBuffer->size = bytesize;
	// initialize indexing and length
	bitBuffer->dataindex = 0;
	bitbufFlush(bitBuffer);
}

// access routines

//! Get a bit from the current position in the buffer
//	returns the bit at the current position in the buffer
//	and increments the bit position
unsigned char bitbufGet(BitBuf* bitBuffer)
{
	unsigned char byte;
	unsigned char bit;
	
	// get current working byte
	byte = bitBuffer->dataptr[bitBuffer->bytePos];
	// read data bit
	bit = (byte & (1<<bitBuffer->bitPos))?(1):(0);

	// increment bit counter
	if(bitBuffer->bitPos < 7)
	{
		bitBuffer->bitPos++;
	}
	else
	{
		// increment byte counter
		bitBuffer->bitPos = 0;
		bitBuffer->bytePos++;
	}

	// return bit value
	return bit;
}

//! Get a bit from a given index into the buffer
//	returns the bit at position [bitIndex] in the buffer
unsigned char bitbufGetAtIndex(BitBuf* bitBuffer, unsigned short bitIndex)
{
	// return bit at index in buffer
	return (bitBuffer->dataptr[bitIndex>>3] & (1<<(bitIndex & 0x07)))?(1):(0);
}

//! Store a bit at the current position in the buffer
//	stores the bit at the current position in the buffer
//	and increments the bit position
void bitbufStore(BitBuf* bitBuffer, unsigned char bit)
{
	unsigned char byte;
	// get current working byte
	byte = bitBuffer->dataptr[bitBuffer->bytePos];
	// apply data bit
	if(bit)
		byte |=  (1<<bitBuffer->bitPos);
	else
		byte &= ~(1<<bitBuffer->bitPos);
	// store data
	bitBuffer->dataptr[bitBuffer->bytePos] = byte;
	bitBuffer->datalength++;

	// increment bit counter
	if(bitBuffer->bitPos < 7)
	{
		bitBuffer->bitPos++;
	}
	else
	{
		// increment byte counter
		bitBuffer->bitPos = 0;
		bitBuffer->bytePos++;
	}
}

void bitbufReset(BitBuf* bitBuffer)
{
	// reset counters
	bitBuffer->bytePos = 0;
	bitBuffer->bitPos = 0;
}

void bitbufFlush(BitBuf* bitBuffer)
{
	// flush contents of the buffer
	bitBuffer->datalength = 0;
	// reset indexing
	bitbufReset(bitBuffer);
}

unsigned short bitbufGetDataLength(BitBuf* bitBuffer)
{
	return bitBuffer->datalength;
}

/*
unsigned char bitbufIsNotFull(cBuffer* buffer)
{
	// check to see if the buffer has room
	// return true if there is room
	return (buffer->datalength < buffer->size);
}
*/

