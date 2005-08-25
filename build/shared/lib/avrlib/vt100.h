/*! \file vt100.h \brief VT100 terminal function library. */
//*****************************************************************************
//
// File Name	: 'vt100.h'
// Title		: VT100 terminal function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.27
// Revised		: 2002.08.27
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

#ifndef VT100_H
#define VT100_H

#include "global.h"

// constants/macros/typdefs
// text attributes
#define VT100_ATTR_OFF		0
#define VT100_BOLD			1
#define VT100_USCORE		4
#define VT100_BLINK			5
#define VT100_REVERSE		7
#define VT100_BOLD_OFF		21
#define VT100_USCORE_OFF	24
#define VT100_BLINK_OFF		25
#define VT100_REVERSE_OFF	27

// functions

// vt100Init() initializes terminal and vt100 library
//		Run this init routine once before using any other vt100 function.
void vt100Init(void);

// vt100ClearScreen() clears the terminal screen
void vt100ClearScreen(void);

// vt100SetAttr() sets the text attributes like BOLD or REVERSE
//		Text written to the terminal after this function is called will have
//		the desired attribuutes.
void vt100SetAttr(u08 attr);

// vt100SetCursorMode() sets the cursor to visible or invisible
void vt100SetCursorMode(u08 visible);

// vt100SetCursorPos() sets the cursor position
//		All text which is written to the terminal after a SetCursorPos command
//		will begin at the new location of the cursor.
void vt100SetCursorPos(u08 line, u08 col);

#endif
