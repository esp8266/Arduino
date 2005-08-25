/*! \file encoderconf.h \brief Quadrature Encoder driver configuration. */
//*****************************************************************************
//
// File Name	: 'encoderconf.h'
// Title		: Quadrature Encoder driver configuration
// Author		: Pascal Stang - Copyright (C) 2003-2004
// Created		: 2003.01.26
// Revised		: 2004.06.25
// Version		: 0.2
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// The default number of encoders supported is 2 because most AVR processors
// have two external interrupts.  To use more or fewer encoders, you must do
// four things:
//
//	1. Use a processor with at least as many external interrutps as number of
//		encoders you want to have.
//	2. Set NUM_ENCODERS to the number of encoders you will use.
//	3. Comment/Uncomment the proper ENCx_SIGNAL defines for your encoders
//		(the encoders must be used sequentially, 0 then 1 then 2 then 3)
//	4. Configure the various defines so that they match your processor and
//		specific hardware.  The notes below may help.
//
//
// -------------------- NOTES --------------------
// The external interrupt pins are mapped as follows on most AVR processors:
// (90s8515, mega161, mega163, mega323, mega16, mega32, etc)
//
// INT0 -> PD2 (PORTD, pin 2)
// INT1 -> PD3 (PORTD, pin 3)
//
// The external interrupt pins on the processors mega128 and mega64 are:
//
// INT0 -> PD0 (PORTD, pin 0)
// INT1 -> PD1 (PORTD, pin 1)
// INT2 -> PD2 (PORTD, pin 2)
// INT3 -> PD3 (PORTD, pin 3)
// INT4 -> PE4 (PORTE, pin 4)
// INT5 -> PE5 (PORTE, pin 5)
// INT6 -> PE6 (PORTE, pin 6)
// INT7 -> PE7 (PORTE, pin 7)
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef ENCODERCONF_H
#define ENCODERCONF_H

// constants/macros/typdefs

// defines for processor compatibility
// quick compatiblity for mega128, mega64 
//#ifndef MCUCR
//	#define	MCUCR	EICRA
//#endif

// Set the total number of encoders you wish to support
#define NUM_ENCODERS				2


// -------------------- Encoder 0 connections --------------------
// Phase A quadrature encoder output should connect to this interrupt line:
// *** NOTE: the choice of interrupt PORT, DDR, and PIN must match the external
// interrupt you are using on your processor.  Consult the External Interrupts
// section of your processor's datasheet for more information.

// Interrupt Configuration
#define ENC0_SIGNAL					SIG_INTERRUPT0	// Interrupt signal name
#define ENC0_INT					INT0	// matching INTx bit in GIMSK/EIMSK
#define ENC0_ICR					MCUCR	// matching Int. Config Register (MCUCR,EICRA/B)
#define ENC0_ISCX0					ISC00	// matching Interrupt Sense Config bit0
#define ENC0_ISCX1					ISC01	// matching Interrupt Sense Config bit1
// PhaseA Port/Pin Configuration
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC0_PHASEA_PORT			PORTD	// PhaseA port register
#define ENC0_PHASEA_DDR				DDRD	// PhaseA port direction register
#define ENC0_PHASEA_PORTIN			PIND	// PhaseA port input register
#define ENC0_PHASEA_PIN				PD2		// PhaseA port pin
// Phase B quadrature encoder output should connect to this direction line:
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC0_PHASEB_PORT			PORTC	// PhaseB port register
#define ENC0_PHASEB_DDR				DDRC	// PhaseB port direction register
#define ENC0_PHASEB_PORTIN			PINC	// PhaseB port input register
#define ENC0_PHASEB_PIN				PC0		// PhaseB port pin


// -------------------- Encoder 1 connections --------------------
// Phase A quadrature encoder output should connect to this interrupt line:
// *** NOTE: the choice of interrupt pin and port must match the external
// interrupt you are using on your processor.  Consult the External Interrupts
// section of your processor's datasheet for more information.

// Interrupt Configuration
#define ENC1_SIGNAL					SIG_INTERRUPT1	// Interrupt signal name
#define ENC1_INT					INT1	// matching INTx bit in GIMSK/EIMSK
#define ENC1_ICR					MCUCR	// matching Int. Config Register (MCUCR,EICRA/B)
#define ENC1_ISCX0					ISC10	// matching Interrupt Sense Config bit0
#define ENC1_ISCX1					ISC11	// matching Interrupt Sense Config bit1
// PhaseA Port/Pin Configuration
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC1_PHASEA_PORT			PORTD	// PhaseA port register
#define ENC1_PHASEA_PORTIN			PIND	// PhaseA port input register
#define ENC1_PHASEA_DDR				DDRD	// PhaseA port direction register
#define ENC1_PHASEA_PIN				PD3		// PhaseA port pin
// Phase B quadrature encoder output should connect to this direction line:
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC1_PHASEB_PORT			PORTC	// PhaseB port register
#define ENC1_PHASEB_DDR				DDRC	// PhaseB port direction register
#define ENC1_PHASEB_PORTIN			PINC	// PhaseB port input register
#define ENC1_PHASEB_PIN				PC1		// PhaseB port pin


// -------------------- Encoder 2 connections --------------------
// Phase A quadrature encoder output should connect to this interrupt line:
// *** NOTE: the choice of interrupt pin and port must match the external
// interrupt you are using on your processor.  Consult the External Interrupts
// section of your processor's datasheet for more information.

// Interrupt Configuration
//#define ENC2_SIGNAL					SIG_INTERRUPT6	// Interrupt signal name
#define ENC2_INT					INT6	// matching INTx bit in GIMSK/EIMSK
#define ENC2_ICR					EICRB	// matching Int. Config Register (MCUCR,EICRA/B)
#define ENC2_ISCX0					ISC60	// matching Interrupt Sense Config bit0
#define ENC2_ISCX1					ISC61	// matching Interrupt Sense Config bit1
// PhaseA Port/Pin Configuration
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC2_PHASEA_PORT			PORTE	// PhaseA port register
#define ENC2_PHASEA_PORTIN			PINE	// PhaseA port input register
#define ENC2_PHASEA_DDR				DDRE	// PhaseA port direction register
#define ENC2_PHASEA_PIN				PE6		// PhaseA port pin
// Phase B quadrature encoder output should connect to this direction line:
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC2_PHASEB_PORT			PORTC	// PhaseB port register
#define ENC2_PHASEB_DDR				DDRC	// PhaseB port direction register
#define ENC2_PHASEB_PORTIN			PINC	// PhaseB port input register
#define ENC2_PHASEB_PIN				PC2		// PhaseB port pin


// -------------------- Encoder 3 connections --------------------
// Phase A quadrature encoder output should connect to this interrupt line:
// *** NOTE: the choice of interrupt pin and port must match the external
// interrupt you are using on your processor.  Consult the External Interrupts
// section of your processor's datasheet for more information.

// Interrupt Configuration
//#define ENC3_SIGNAL					SIG_INTERRUPT7	// Interrupt signal name
#define ENC3_INT					INT7	// matching INTx bit in GIMSK/EIMSK
#define ENC3_ICR					EICRB	// matching Int. Config Register (MCUCR,EICRA/B)
#define ENC3_ISCX0					ISC70	// matching Interrupt Sense Config bit0
#define ENC3_ISCX1					ISC71	// matching Interrupt Sense Config bit1
// PhaseA Port/Pin Configuration
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC3_PHASEA_PORT			PORTE	// PhaseA port register
#define ENC3_PHASEA_PORTIN			PINE	// PhaseA port input register
#define ENC3_PHASEA_DDR				DDRE	// PhaseA port direction register
#define ENC3_PHASEA_PIN				PE7		// PhaseA port pin
// Phase B quadrature encoder output should connect to this direction line:
// *** PORTx, DDRx, PINx, and Pxn should all have the same letter for "x" ***
#define ENC3_PHASEB_PORT			PORTC	// PhaseB port register
#define ENC3_PHASEB_DDR				DDRC	// PhaseB port direction register
#define ENC3_PHASEB_PORTIN			PINC	// PhaseB port input register
#define ENC3_PHASEB_PIN				PC3		// PhaseB port pin

#endif
