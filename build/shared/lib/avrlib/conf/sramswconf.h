/*! \file sramswconf.h \brief Software-driven SRAM memory bus access configuration. */
//*****************************************************************************
//
// File Name	: 'sramswconf.h'
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

#ifndef SRAMSWCONF_H
#define SRAMSWCONF_H

// defines

// data bus (DATA[0:7]) and low address (ADDR[0:7]) port
#define SRAM_ADL		PORTA
#define SRAM_ADL_DDR	DDRA
#define SRAM_ADL_IN		PINA
// high address port (ADDR[8:15])
#define SRAM_AH			PORTC
#define SRAM_AH_DDR		DDRC
// page address port (PAGE[0:3])
#define SRAM_PAGE		PORTB
#define SRAM_PAGE_DDR	DDRB
#define SRAM_PAGE_MASK	0x0F
// control port
#define SRAM_CTRL		PORTD
#define SRAM_CTRL_DDR	DDRD
// control lines
#define SRAM_ALE		PD5
#define SRAM_WR			PD6
#define SRAM_RD			PD7

#endif
