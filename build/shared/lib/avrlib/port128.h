/*! \file port128.h \brief Additional include for Mega128 to define individual port pins. */
//*****************************************************************************
//
// File Name	: 'port128.h'
// Title		: Additional include for Mega128 to define individual port pins
// Author		: Pascal Stang
// Created		: 11/18/2002
// Revised		: 11/18/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : This include file contains additional port and pin defines
//		to help make code transparently compatible with the mega128.  As in
//		the other AVR processors, using defines like PD2 to denote PORTD, pin2
//		is not absolutely necessary but enhances readability.  The mega128 io.h
//		no longer defines individual pins of ports (like PD2 or PA5, for
//		example).  Instead, port pins are defines universally for all ports as
//		PORT0 through PORT7.  However, this renaming causes a code-portability
//		issue from non-mega128 AVRs to the mega128.  Including this file will
//		replace the missing defines.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef PORT128_H
#define PORT128_H

// Mega128 individual port defines
// (using these is technically unnecessary but improves code compatibility to
//  the mega128 from other AVR processors where these values were still defined
//  in the io.h for that processor)

// PORTA
#define PA0		PORT0
#define PA1		PORT1
#define PA2		PORT2
#define PA3		PORT3
#define PA4		PORT4
#define PA5		PORT5
#define PA6		PORT6
#define PA7		PORT7
// PORTB
#define PB0		PORT0
#define PB1		PORT1
#define PB2		PORT2
#define PB3		PORT3
#define PB4		PORT4
#define PB5		PORT5
#define PB6		PORT6
#define PB7		PORT7
// PORTC
#define PC0		PORT0
#define PC1		PORT1
#define PC2		PORT2
#define PC3		PORT3
#define PC4		PORT4
#define PC5		PORT5
#define PC6		PORT6
#define PC7		PORT7
// PORTD
#define PD0		PORT0
#define PD1		PORT1
#define PD2		PORT2
#define PD3		PORT3
#define PD4		PORT4
#define PD5		PORT5
#define PD6		PORT6
#define PD7		PORT7
// PORTE
#define PE0		PORT0
#define PE1		PORT1
#define PE2		PORT2
#define PE3		PORT3
#define PE4		PORT4
#define PE5		PORT5
#define PE6		PORT6
#define PE7		PORT7
// PORTF
#define PF0		PORT0
#define PF1		PORT1
#define PF2		PORT2
#define PF3		PORT3
#define PF4		PORT4
#define PF5		PORT5
#define PF6		PORT6
#define PF7		PORT7
// PORTG
#define PG0		PORT0
#define PG1		PORT1
#define PG2		PORT2
#define PG3		PORT3
#define PG4		PORT4
#define PG5		PORT5

#endif
