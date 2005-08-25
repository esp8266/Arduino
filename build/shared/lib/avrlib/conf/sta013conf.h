/*! \file sta013conf.h \brief STA013 MP3 player driver configuration. */
//*****************************************************************************
//
// File Name	: 'sta013.h'
// Title		: STMicroelectronics STA013 MP3 player driver
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 10/22/2000
// Revised		: 12/04/2000
// Version		: 0.3
// Target MCU	: ATmega103 (should work for Atmel AVR Series)
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


#ifndef STA013CONF_H
#define STA013CONF_H

// STA013 Configuration

// STA013 demand line
#define STA013_DEMAND_PORT		PORTE				// port to which DEMAND line is connected
#define STA013_DEMAND_PORTIN	PINE				// input port to which DEMAND line is connected
#define STA013_DEMAND_PIN		PE4				// port pin to which DEMAND line is connected
#define STA013_DEMAND_INTR		SIG_INTERRUPT4	// interrupt to which DEMAND line is connected

#endif
