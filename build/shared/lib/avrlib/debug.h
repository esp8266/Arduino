/*! \file debug.h \brief Debugging function library. */
//*****************************************************************************
//
// File Name	: 'debug.h'
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

#ifndef DEBUG_H
#define DEBUG_H

#include "global.h"

// defines

// function prototypes

//! Print a part of memory as a formatted hex table with ascii translation
void debugPrintHexTable(u16 length, u08 *buffer);


#endif
