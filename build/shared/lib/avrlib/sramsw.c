/*! \file sramsw.c \brief Software-driven SRAM memory bus access functions. */
//*****************************************************************************
//
// File Name	: 'sramsw.c'
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

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "global.h"
#include "sramsw.h"

// global variables

// functions
void sramswInit(void)
{
	// initialize port state
	outb(SRAM_ADL, 0xFF);		// addr/data port set to 0xFF (pull-ups enabled)
	outb(SRAM_AH, 0x00);		// high addr port set to 0x00
	// initialize port directions
	outb(SRAM_ADL_DDR, 0x00);	// addr/data port set to input
	outb(SRAM_AH_DDR, 0xFF);	// high addr port set to output
	// initialize control line states
	sbi(SRAM_CTRL, SRAM_WR);	// de-assert write (active low)
	sbi(SRAM_CTRL, SRAM_RD);	// de-assert read (active low)
	cbi(SRAM_CTRL, SRAM_ALE);	// de-assert ALE (active high)
	// set control line direction
	sbi(SRAM_CTRL_DDR, SRAM_WR);
	sbi(SRAM_CTRL_DDR, SRAM_RD);
	sbi(SRAM_CTRL_DDR, SRAM_ALE);
	// set page lines direction
	outb(SRAM_PAGE_DDR, inb(SRAM_PAGE_DDR) | SRAM_PAGE_MASK );
	// initialize page
	sramswSetPage(0);
}

void sramswOff(void)
{
}

void sramswWrite(u32 addr, u08 data)
{
	// set page
	sramswSetPage( (addr & 0x00FF0000)>>16 );
	// set high-order address
	outb(SRAM_AH, (addr & 0x0000FF00)>>8 );
	// set low-order address
	outb(SRAM_ADL, addr & 0x000000FF);
	// apply low-order address to latch
	outb(SRAM_ADL_DDR, 0xFF);
	// clock latch to save low-order address
	sbi(SRAM_CTRL, SRAM_ALE);	// assert ALE (active high)
	asm volatile ("nop");
	cbi(SRAM_CTRL, SRAM_ALE);	// de-assert ALE (active high)

	// apply data to memory
	outb(SRAM_ADL, data);
	// clock write line to store data
	cbi(SRAM_CTRL, SRAM_WR);	// assert write (active low)
	asm volatile ("nop");
	sbi(SRAM_CTRL, SRAM_WR);	// de-assert write (active low)
}

u08 sramswRead(u32 addr)
{
	u08 data;

	// set page
	sramswSetPage( (addr & 0x00FF0000)>>16 );
	// set high-order address
	outb(SRAM_AH, (addr & 0x0000FF00)>>8 );
	// set low-order address
	outb(SRAM_ADL, addr & 0x000000FF);
	// apply low-order address to latch
	outb(SRAM_ADL_DDR, 0xFF);
	// clock latch to save low-order address
	sbi(SRAM_CTRL, SRAM_ALE);	// assert ALE (active high)
	asm volatile ("nop");
	cbi(SRAM_CTRL, SRAM_ALE);	// de-assert ALE (active high)

	// switch data bus to input
	outb(SRAM_ADL_DDR, 0x00);
	// clear pullups
	outb(SRAM_ADL, 0x00);
	// request data from memory
	cbi(SRAM_CTRL, SRAM_RD);	// assert read (active low)
	// retrieve data
	asm volatile ("nop");
	data = inb(SRAM_ADL_IN);
	// release read line
	sbi(SRAM_CTRL, SRAM_RD);	// de-assert read (active low)
	// switch data bus to output
	outb(SRAM_ADL_DDR, 0xFF);

	return data;
}

void sramswSetPage(u08 page)
{
	outb(SRAM_PAGE, (page & SRAM_PAGE_MASK));
}
