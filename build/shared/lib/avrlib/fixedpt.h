/*! \file fixedpt.h \brief Fixed-point math function library. */
//*****************************************************************************
//
// File Name	: 'fixedpt.h'
// Title		: Fixed-point math function library
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.01.26
// Revised		: 2003.02.04
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef FIXEDPT_H
#define FIXEDPT_H

#include "global.h"

// constants/macros/typdefs

// functions

//! fixedptInit() initializes fixed-point math function library
//   set the number of bits to use behind the point
void fixedptInit(u08 fixedPtBits);

//! convert integer to fixed-point number
s32 fixedptConvertFromInt(s32 int_number);

//! convert fixed-point number to integer
s32 fixedptConvertToInt(s32 fp_number);

//! add a and b (a+b) with fixed-point math
s32 fixedptAdd(s32 a, s32 b);

//! subtract a and b (a-b) with fixed-point math
s32 fixedptSubtract(s32 a, s32 b);

//! multiply a and b (a*b) with fixed-point math
s32 fixedptMultiply(s32 a, s32 b);

//! divide numer by denom (numer/denom) with fixed-point math
s32 fixedptDivide(s32 numer, s32 denom);

#endif
