/*! \file lcd.c \brief Character LCD driver for HD44780/SED1278 displays. */
//*****************************************************************************
//
// File Name	: 'lcd.c'
// Title		: Character LCD driver for HD44780/SED1278 displays
//					(usable in mem-mapped, or I/O mode)
// Author		: Pascal Stang
// Created		: 11/22/2000
// Revised		: 4/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "global.h"
#include "timer.h"

#include "lcd.h"

// custom LCD characters
unsigned char __attribute__ ((progmem)) LcdCustomChar[] =
{
	0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, // 0. 0/5 full progress block
	0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 1. 1/5 full progress block
	0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, // 2. 2/5 full progress block
	0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00, // 3. 3/5 full progress block
	0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00, // 4. 4/5 full progress block
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 5. 5/5 full progress block
	0x03, 0x07, 0x0F, 0x1F, 0x0F, 0x07, 0x03, 0x00, // 6. rewind arrow
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 7. stop block
	0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x00, // 8. pause bars
	0x18, 0x1C, 0x1E, 0x1F, 0x1E, 0x1C, 0x18, 0x00, // 9. fast-forward arrow
	0x00, 0x04, 0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x00, // 10. scroll up arrow
	0x00, 0x1F, 0x1F, 0x0E, 0x0E, 0x04, 0x04, 0x00, // 11. scroll down arrow
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 12. blank character
	0x00, 0x0E, 0x19, 0x15, 0x13, 0x0E, 0x00, 0x00,	// 13. animated play icon frame 0
	0x00, 0x0E, 0x15, 0x15, 0x15, 0x0E, 0x00, 0x00,	// 14. animated play icon frame 1
	0x00, 0x0E, 0x13, 0x15, 0x19, 0x0E, 0x00, 0x00,	// 15. animated play icon frame 2
	0x00, 0x0E, 0x11, 0x1F, 0x11, 0x0E, 0x00, 0x00,	// 16. animated play icon frame 3
};

/*************************************************************/
/********************** LOCAL FUNCTIONS **********************/
/*************************************************************/

void lcdInitHW(void)
{
	// initialize I/O ports
	// if I/O interface is in use
#ifdef LCD_PORT_INTERFACE
	// initialize LCD control lines
	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);
	cbi(LCD_CTRL_PORT, LCD_CTRL_E);
	// initialize LCD control lines to output
	sbi(LCD_CTRL_DDR, LCD_CTRL_RS);
	sbi(LCD_CTRL_DDR, LCD_CTRL_RW);
	sbi(LCD_CTRL_DDR, LCD_CTRL_E);
	// initialize LCD data port to input
	// initialize LCD data lines to pull-up
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);						// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);						// set pull-ups to on (8bit)
	#endif
#else
	// enable external memory bus if not already enabled
	sbi(MCUCR, SRE);			// enable bus interface
#endif
}

void lcdBusyWait(void)
{
	// wait until LCD busy bit goes to zero
	// do a read from control register
#ifdef LCD_PORT_INTERFACE
	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);				// set RS to "control"
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);	// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);					// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);					// set pull-ups to on (8bit)
	#endif
	sbi(LCD_CTRL_PORT, LCD_CTRL_RW);				// set R/W to "read"
	sbi(LCD_CTRL_PORT, LCD_CTRL_E);					// set "E" line
	LCD_DELAY;								// wait
	while(inb(LCD_DATA_PIN) & 1<<LCD_BUSY)
	{
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);		// clear "E" line
		LCD_DELAY;									// wait
		LCD_DELAY;									// wait
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);		// set "E" line
		LCD_DELAY;									// wait
		LCD_DELAY;									// wait
		#ifdef LCD_DATA_4BIT						// do an extra clock for 4 bit reads
			cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
			LCD_DELAY;								// wait
			LCD_DELAY;								// wait
			sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
			LCD_DELAY;								// wait
			LCD_DELAY;								// wait
		#endif
	}
	cbi(LCD_CTRL_PORT, LCD_CTRL_E);			// clear "E" line
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	// memory bus read
	// sbi(MCUCR, SRW);			// enable RAM waitstate
	// wait until LCD busy bit goes to zero
	while( (*((volatile unsigned char *) (LCD_CTRL_ADDR))) & (1<<LCD_BUSY) );
	// cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}

void lcdControlWrite(u08 data) 
{
// write the control byte to the display controller
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();							// wait until LCD not busy
	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);			// set RS to "control"
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);			// set R/W to "write"
	#ifdef LCD_DATA_4BIT
		// 4 bit write
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)|0xF0);	// set data I/O lines to output (4bit)
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data&0xF0) );	// output data, high 4 bits
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data<<4) );	// output data, low 4 bits
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit write
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, 0xFF);				// set data I/O lines to output (8bit)
		outb(LCD_DATA_POUT, data);				// output data, 8bits
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
#else
	// memory bus write
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	*((volatile unsigned char *) (LCD_CTRL_ADDR)) = data;
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}

u08 lcdControlRead(void)
{
// read the control byte from the display controller
	register u08 data;
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();				// wait until LCD not busy
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);		// set RS to "control"
	sbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "read"
	#ifdef LCD_DATA_4BIT
		// 4 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		data = inb(LCD_DATA_PIN)&0xF0;	// input data, high 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		data |= inb(LCD_DATA_PIN)>>4;	// input data, low 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		data = inb(LCD_DATA_PIN);		// input data, 8bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	data = *((volatile unsigned char *) (LCD_CTRL_ADDR));
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
	return data;
}

void lcdDataWrite(u08 data) 
{
// write a data byte to the display
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();							// wait until LCD not busy
	sbi(LCD_CTRL_PORT, LCD_CTRL_RS);		// set RS to "data"
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "write"
	#ifdef LCD_DATA_4BIT
		// 4 bit write
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)|0xF0);	// set data I/O lines to output (4bit)
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data&0xF0) );	// output data, high 4 bits
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data<<4) );	// output data, low 4 bits
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit write
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		outb(LCD_DATA_DDR, 0xFF);			// set data I/O lines to output (8bit)
		outb(LCD_DATA_POUT, data);			// output data, 8bits
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
#else
	// memory bus write
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	*((volatile unsigned char *) (LCD_DATA_ADDR)) = data;
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}

u08 lcdDataRead(void)
{
// read a data byte from the display
	register u08 data;
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();				// wait until LCD not busy
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
	sbi(LCD_CTRL_PORT, LCD_CTRL_RS);		// set RS to "data"
	sbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "read"
	#ifdef LCD_DATA_4BIT
		// 4 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		data = inb(LCD_DATA_PIN)&0xF0;	// input data, high 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		data |= inb(LCD_DATA_PIN)>>4;			// input data, low 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		data = inb(LCD_DATA_PIN);			// input data, 8bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	// memory bus read
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	data = *((volatile unsigned char *) (LCD_DATA_ADDR));
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
	return data;
}



/*************************************************************/
/********************* PUBLIC FUNCTIONS **********************/
/*************************************************************/

void lcdInit()
{
	// initialize hardware
	lcdInitHW();
	// LCD function set
	lcdControlWrite(LCD_FUNCTION_DEFAULT);
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
	delay(60000);	// wait 60ms
	// set entry mode
	lcdControlWrite(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC);
	// set display to on
	//lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_BLINK);
	lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY );
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
	// set data address to 0
	lcdControlWrite(1<<LCD_DDRAM | 0x00);

	// load the first 8 custom characters
	lcdLoadCustomChar((u08*)LcdCustomChar,0,0);
	lcdLoadCustomChar((u08*)LcdCustomChar,1,1);
	lcdLoadCustomChar((u08*)LcdCustomChar,2,2);
	lcdLoadCustomChar((u08*)LcdCustomChar,3,3);
	lcdLoadCustomChar((u08*)LcdCustomChar,4,4);
	lcdLoadCustomChar((u08*)LcdCustomChar,5,5);
	lcdLoadCustomChar((u08*)LcdCustomChar,6,6);
	lcdLoadCustomChar((u08*)LcdCustomChar,7,7);
}

void lcdHome(void)
{
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
}

void lcdClear(void)
{
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
}

void lcdGotoXY(u08 x, u08 y)
{
	register u08 DDRAMAddr;

	// remap lines into proper order
	switch(y)
	{
	case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
	case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
	case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
	case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
	default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}

	// set data address
	lcdControlWrite(1<<LCD_DDRAM | DDRAMAddr);
}

void lcdLoadCustomChar(u08* lcdCustomCharArray, u08 romCharNum, u08 lcdCharNum)
{
	register u08 i;
	u08 saveDDRAMAddr;

	// backup the current cursor position
	saveDDRAMAddr = lcdControlRead() & 0x7F;

	// multiply the character index by 8
	lcdCharNum = (lcdCharNum<<3);	// each character occupies 8 bytes
	romCharNum = (romCharNum<<3);	// each character occupies 8 bytes

	// copy the 8 bytes into CG (character generator) RAM
	for(i=0; i<8; i++)
	{
		// set CG RAM address
		lcdControlWrite((1<<LCD_CGRAM) | (lcdCharNum+i));
		// write character data
		lcdDataWrite( pgm_read_byte(lcdCustomCharArray+romCharNum+i) );
	}

	// restore the previous cursor position
	lcdControlWrite(1<<LCD_DDRAM | saveDDRAMAddr);

}

void lcdPrintData(char* data, u08 nBytes)
{
	register u08 i;

	// check to make sure we have a good pointer
	if (!data) return;

	// print data
	for(i=0; i<nBytes; i++)
	{
		lcdDataWrite(data[i]);
	}
}

void lcdProgressBar(u16 progress, u16 maxprogress, u08 length)
{
	u08 i;
	u32 pixelprogress;
	u08 c;

	// draw a progress bar displaying (progress / maxprogress)
	// starting from the current cursor position
	// with a total length of "length" characters
	// ***note, LCD chars 0-5 must be programmed as the bar characters
	// char 0 = empty ... char 5 = full

	// total pixel length of bargraph equals length*PROGRESSPIXELS_PER_CHAR;
	// pixel length of bar itself is
	pixelprogress = ((progress*(length*PROGRESSPIXELS_PER_CHAR))/maxprogress);
	
	// print exactly "length" characters
	for(i=0; i<length; i++)
	{
		// check if this is a full block, or partial or empty
		// (u16) cast is needed to avoid sign comparison warning
		if( ((i*(u16)PROGRESSPIXELS_PER_CHAR)+5) > pixelprogress )
		{
			// this is a partial or empty block
			if( ((i*(u16)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
			{
				// this is an empty block
				// use space character?
				c = 0;
			}
			else
			{
				// this is a partial block
				c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
			}
		}
		else
		{
			// this is a full block
			c = 5;
		}
		
		// write character to display
		lcdDataWrite(c);
	}

}

