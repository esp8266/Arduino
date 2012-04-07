/* Copyright (c) 2002, Marek Michalkiewicz
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id: io8534.h,v 1.7 2005/07/09 14:01:11 aesok Exp $ */

/* avr/io8534.h - definitions for AT90C8534 */

#ifndef _AVR_IO8534_
#define _AVR_IO8534_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "io8534.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* 0x00..0x03 reserved */

/* ADC Data Register */
#ifndef __ASSEMBLER__
#define ADC 	_SFR_IO16(0x04)
#endif
#define ADCW	_SFR_IO16(0x04)
#define ADCL	_SFR_IO8(0x04)
#define ADCH	_SFR_IO8(0x05)

/* ADC Control and Status Register */
#define ADCSR	_SFR_IO8(0x06)

/* ADC Multiplexer Select Register */
#define ADMUX	_SFR_IO8(0x07)

/* 0x08..0x0F reserved */

/* General Interrupt Pin Register */
#define GIPR	_SFR_IO8(0x10)

/* 0x11..0x19 reserved */

/* Data Direction Register, Port A */
#define DDRA	_SFR_IO8(0x1A)

/* Data Register, Port A */
#define PORTA	_SFR_IO8(0x1B)

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO16(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEARH	_SFR_IO8(0x1F)

/* 0x20..0x2B reserved */

/* Timer/Counter1 */
#define TCNT1	_SFR_IO16(0x2C)
#define TCNT1L	_SFR_IO8(0x2C)
#define TCNT1H	_SFR_IO8(0x2D)

/* Timer/Counter1 Control Register */
#define TCCR1	_SFR_IO8(0x2E)

/* 0x2F..0x31 reserved */

/* Timer/Counter0 (8-bit) */
#define TCNT0	_SFR_IO8(0x32)

/* Timer/Counter0 Control Register */
#define TCCR0	_SFR_IO8(0x33)

/* 0x34 reserved */

/* MCU general Control Register */
#define MCUCR	_SFR_IO8(0x35)

/* 0x36..0x37 reserved */

/* Timer/Counter Interrupt Flag Register */
#define TIFR	_SFR_IO8(0x38)

/* Timer/Counter Interrupt MaSK Register */
#define TIMSK	_SFR_IO8(0x39)

/* General Interrupt Flag Register */
#define GIFR	_SFR_IO8(0x3A)

/* General Interrupt MaSK register */
#define GIMSK	_SFR_IO8(0x3B)

/* 0x3C reserved */

/* 0x3D..0x3E SP */

/* 0x3F SREG */

/* Interrupt vectors */

#define SIG_INTERRUPT0		_VECTOR(1)
#define SIG_INTERRUPT1		_VECTOR(2)
#define SIG_OVERFLOW1		_VECTOR(3)
#define SIG_OVERFLOW0		_VECTOR(4)
#define SIG_ADC			_VECTOR(5)
#define SIG_EEPROM_READY	_VECTOR(6)

#define _VECTORS_SIZE 14

/* Bit numbers */

/* GIMSK */
#define INT1	7
#define INT0	6

/* GIFR */
#define INTF1	7
#define INTF0	6

/* GIPR */
#define IPIN1	3
#define IPIN0	2

/* TIMSK */
#define TOIE1	2
#define TOIE0	0

/* TIFR */
#define TOV1	2
#define TOV0	0

/* MCUCR */
#define SE	6
#define SM	5
#define ISC1	2
#define ISC0	0

/* TCCR0 */
#define CS02	2
#define CS01	1
#define CS00	0

/* TCCR1 */
#define CS12	2
#define CS11	1
#define CS10	0

/* PORTA */
#define PA7	7
#define PA6	6
#define PA5	5
#define PA4	4
#define PA3	3
#define PA2	2
#define PA1	1
#define PA0	0

/* DDRA */
#define DDA7	7
#define DDA6	6
#define DDA5	5
#define DDA4	4
#define DDA3	3
#define DDA2	2
#define DDA1	1
#define DDA0	0

/* EEPROM Control Register */
#define EERIE   3
#define EEMWE   2
#define EEWE    1
#define EERE    0

/* Last memory addresses */
#define RAMEND		0x15F
#define XRAMEND		0x15F
#define E2END		0x1FF
#define FLASHEND	0x1FFF

#endif /* _AVR_IO8534_H_ */
