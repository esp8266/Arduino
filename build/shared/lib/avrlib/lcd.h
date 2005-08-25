/*! \file lcd.h \brief Character LCD driver for HD44780/SED1278 displays. */
//*****************************************************************************
//
// File Name	: 'lcd.h'
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

#ifndef LCD_H
#define LCD_H

#include "global.h"

// include project-dependent configurations
#include "lcdconf.h"

// HD44780 LCD controller command set (do not modify these)
// writing:
#define LCD_CLR             0      // DB0: clear display
#define LCD_HOME            1      // DB1: return to home position
#define LCD_ENTRY_MODE      2      // DB2: set entry mode
#define LCD_ENTRY_INC       1      //   DB1: increment
#define LCD_ENTRY_SHIFT     0      //   DB2: shift
#define LCD_ON_CTRL         3      // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY      2      //   DB2: turn display on
#define LCD_ON_CURSOR       1      //   DB1: turn cursor on
#define LCD_ON_BLINK        0      //   DB0: blinking cursor
#define LCD_MOVE            4      // DB4: move cursor/display
#define LCD_MOVE_DISP       3      //   DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2      //   DB2: move right (0-> left)
#define LCD_FUNCTION        5      // DB5: function set
#define LCD_FUNCTION_8BIT   4      //   DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3      //   DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS 2      //   DB2: 5x10 font (0->5x7 font)
#define LCD_CGRAM           6      // DB6: set CG RAM address
#define LCD_DDRAM           7      // DB7: set DD RAM address
// reading:
#define LCD_BUSY            7      // DB7: LCD is busy

// Default LCD setup
// this default setup is loaded on LCD initialization
#ifdef LCD_DATA_4BIT
	#define LCD_FDEF_1			(0<<LCD_FUNCTION_8BIT)
#else
	#define LCD_FDEF_1			(1<<LCD_FUNCTION_8BIT)
#endif
#define LCD_FDEF_2				(1<<LCD_FUNCTION_2LINES)
#define LCD_FUNCTION_DEFAULT	((1<<LCD_FUNCTION) | LCD_FDEF_1 | LCD_FDEF_2)
#define LCD_MODE_DEFAULT		((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC))

// custom LCD characters
extern unsigned char __attribute__ ((progmem)) LcdCustomChar[];
#define LCDCHAR_PROGRESS05		0	// 0/5 full progress block
#define LCDCHAR_PROGRESS15		1	// 1/5 full progress block
#define LCDCHAR_PROGRESS25		2	// 2/5 full progress block
#define LCDCHAR_PROGRESS35		3	// 3/5 full progress block
#define LCDCHAR_PROGRESS45		4	// 4/5 full progress block
#define LCDCHAR_PROGRESS55		5	// 5/5 full progress block
#define LCDCHAR_REWINDARROW		6	// rewind arrow
#define LCDCHAR_STOPBLOCK		7	// stop block
#define LCDCHAR_PAUSEBARS		8	// pause bars
#define LCDCHAR_FORWARDARROW	9	// fast-forward arrow
#define LCDCHAR_SCROLLUPARROW	10	// scroll up arrow
#define LCDCHAR_SCROLLDNARROW	11	// scroll down arrow
#define LCDCHAR_BLANK			12	// scroll down arrow
#define LCDCHAR_ANIPLAYICON0	13	// animated play icon frame 0
#define LCDCHAR_ANIPLAYICON1	14	// animated play icon frame 1
#define LCDCHAR_ANIPLAYICON2	15	// animated play icon frame 2
#define LCDCHAR_ANIPLAYICON3	16	// animated play icon frame 3

// progress bar defines
#define PROGRESSPIXELS_PER_CHAR	6


// ****** Low-level functions ******
// the following functions are the only ones which deal with the CPU
// memory or port pins directly.  If you decide to use a fundamentally
// different hardware interface to your LCD, only these functions need
// to be changed, after which all the high-level functions will
// work again.

// initializes I/O pins connected to LCD
void lcdInitHW(void);
// waits until LCD is not busy
void lcdBusyWait(void);
// writes a control command to the LCD
void lcdControlWrite(u08 data);
// read the control status from the LCD
u08 lcdControlRead(void);
// writes a data byte to the LCD screen at the current position
void lcdDataWrite(u08 data);
// reads the data byte on the LCD screen at the current position
u08 lcdDataRead(void);


// ****** High-levlel functions ******
// these functions provide the high-level control of the LCD
// such as clearing the display, setting cursor positions,
// displaying text and special characters

// initializes the LCD display (gets it ready for use)
void lcdInit(void);

// moves the cursor/position to Home (upper left corner)
void lcdHome(void);

// clears the LCD display
void lcdClear(void);

// moves the cursor/position to the row,col requested
// ** this may not be accurate for all displays
void lcdGotoXY(u08 row, u08 col);

// loads a special user-defined character into the LCD
// <lcdCustomCharArray> is a pointer to a ROM array containing custom characters
// <romCharNum> is the index of the character to load from lcdCustomCharArray
// <lcdCharNum> is the RAM location in the LCD (legal value: 0-7)
void lcdLoadCustomChar(u08* lcdCustomCharArray, u08 romCharNum, u08 lcdCharNum);

// prints a series of bytes/characters to the display
void lcdPrintData(char* data, u08 nBytes);

// displays a horizontal progress bar at the current cursor location
// <progress> is the value the bargraph should indicate
// <maxprogress> is the value at the end of the bargraph
// <length> is the number of LCD characters that the bargraph should cover
void lcdProgressBar(u16 progress, u16 maxprogress, u08 length);

#endif
