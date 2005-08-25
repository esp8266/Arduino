/*! \file mmcconf.h \brief MultiMedia and SD Flash Card Interface Configuration. */
//*****************************************************************************
//
// File Name	: 'mmc.h'
// Title		: MultiMedia and SD Flash Card Interface Configuration
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.09.22
// Revised		: 2004.09.22
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

#ifndef MMCCONF_H
#define MMCCONF_H

// define to enable debugging print statements
//#define MMC_DEBUG

// MMC card chip select pin defines
#define MMC_CS_PORT			PORTB
#define MMC_CS_DDR			DDRB
#define MMC_CS_PIN			0

#endif
