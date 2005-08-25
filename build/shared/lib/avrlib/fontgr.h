/*! \file fontgr.h \brief Graphic LCD Font (Graphic Characters). */
//*****************************************************************************
//
// File Name	: 'fontgr.h'
// Title		: Graphic LCD Font (Graphic Charaters)
// Author		: Pascal Stang
// Date			: 10/19/2001
// Revised		: 10/19/2001
// Version		: 0.1
// Target MCU	: Atmel AVR
// Editor Tabs	: 4
//
//*****************************************************************************

#ifndef FONTGR_H
#define FONTGR_H

#ifndef WIN32
// AVR specific includes
	#include <avr/pgmspace.h>
#endif

static unsigned char __attribute__ ((progmem)) FontGr[] =
{
// format is one character per line:
// length, byte array[length]
	0x0B,0x3E,0x41,0x41,0x41,0x41,0x42,0x42,0x42,0x42,0x3C,0x00,// 0. Folder Icon
	0x06,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF							// 1. Solid 6x8 block
};

#endif
