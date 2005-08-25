/*! \file sramsw.h \brief Software-driven SRAM memory bus access functions. */
//*****************************************************************************
//
// File Name	: 'sramsw.h'
// Title		: Software-driven SRAM memory bus access functions
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 11/11/2002
// Revised		: 11/13/2002
// Version		: 1.0
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef SRAMSW_H
#define SRAMSW_H

#include "global.h"
// include project-dependent configurations
// sramswconf.h allows the user to choose which ports
// and pins are used in the memory bus
#include "sramswconf.h"

// function prototypes

//! Initialize the memory bus
void sramswInit(void);
//! Write data using the memory bus
void sramswWrite(u32 addr, u08 data);
//! Read data using the memory bus
u08 sramswRead(u32 addr);
//! Set memory page
void sramswSetPage(u08 page);

#endif
