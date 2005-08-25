/*! \file buffer.c \brief Multipurpose byte buffer structure and methods. */
//*****************************************************************************
//
// File Name	: 'buffer.c'
// Title		: Multipurpose byte buffer structure and methods
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 9/23/2001
// Revised		: 9/23/2001
// Version		: 1.0
// Target MCU	: any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "buffer.h"

// global variables

// initialization

void bufferInit(cBuffer* buffer, unsigned char *start, unsigned short size)
{
	// set start pointer of the buffer
	buffer->dataptr = start;
	buffer->size = size;
	// initialize index and length
	buffer->dataindex = 0;
	buffer->datalength = 0;
}

// access routines
unsigned char  bufferGetFromFront(cBuffer* buffer)
{
	unsigned char data = 0;
	
	// check to see if there's data in the buffer
	if(buffer->datalength)
	{
		// get the first character from buffer
		data = buffer->dataptr[buffer->dataindex];
		// move index down and decrement length
		buffer->dataindex++;
		if(buffer->dataindex >= buffer->size)
		{
			buffer->dataindex %= buffer->size;
		}
		buffer->datalength--;
	}
	// return
	return data;
}

void bufferDumpFromFront(cBuffer* buffer, unsigned short numbytes)
{
	// dump numbytes from the front of the buffer
	// are we dumping less than the entire buffer?
	if(numbytes < buffer->datalength)
	{
		// move index down by numbytes and decrement length by numbytes
		buffer->dataindex += numbytes;
		if(buffer->dataindex >= buffer->size)
		{
			buffer->dataindex %= buffer->size;
		}
		buffer->datalength -= numbytes;
	}
	else
	{
		// flush the whole buffer
		buffer->datalength = 0;
	}
}

unsigned char bufferGetAtIndex(cBuffer* buffer, unsigned short index)
{
	// return character at index in buffer
	return buffer->dataptr[(buffer->dataindex+index)%(buffer->size)];
}

unsigned char bufferAddToEnd(cBuffer* buffer, unsigned char data)
{
	// make sure the buffer has room
	if(buffer->datalength < buffer->size)
	{
		// save data byte at end of buffer
		buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = data;
		// increment the length
		buffer->datalength++;
		// return success
		return -1;
	}
	else return 0;
}

unsigned char bufferIsNotFull(cBuffer* buffer)
{
	// check to see if the buffer has room
	// return true if there is room
	return (buffer->datalength < buffer->size);
}

void bufferFlush(cBuffer* buffer)
{
	// flush contents of the buffer
	buffer->datalength = 0;
}

