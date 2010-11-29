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

/* $Id: iotn28.h,v 1.9.2.5 2008/10/17 23:27:52 arcanum Exp $ */

/* avr/iotn28.h - definitions for ATtiny28 */

#ifndef _AVR_IOTN28_H_
#define _AVR_IOTN28_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn28.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

#ifndef __ASSEMBLER__
#  warning "MCU not supported by the C compiler"
#endif

/* I/O registers */

#define OSCCAL	_SFR_IO8(0x00)

#define WDTCR	_SFR_IO8(0x01)

#define MODCR	_SFR_IO8(0x02)

#define TCNT0	_SFR_IO8(0x03)
#define TCCR0	_SFR_IO8(0x04)

#define IFR	_SFR_IO8(0x05)
#define ICR	_SFR_IO8(0x06)

#define MCUCS	_SFR_IO8(0x07)

#define ACSR	_SFR_IO8(0x08)

/* 0x09..0x0F reserved */

#define PIND	_SFR_IO8(0x10)
#define DDRD	_SFR_IO8(0x11)
#define PORTD	_SFR_IO8(0x12)

/* 0x13..0x15 reserved */

#define PINB	_SFR_IO8(0x16)

/* 0x17..0x18 reserved */

#define PINA	_SFR_IO8(0x19)
#define PACR	_SFR_IO8(0x1A)
#define PORTA	_SFR_IO8(0x1B)

/* 0x1C..0x3E reserved */

/* 0x3F SREG */

/* Interrupt vectors */

/* External Interrupt 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)

/* Low-level Input on Port B */
#define LOWLEVEL_IO_PINS_vect		_VECTOR(3)
#define SIG_PIN				_VECTOR(3)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(4)
#define SIG_OVERFLOW0			_VECTOR(4)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(5)
#define SIG_COMPARATOR			_VECTOR(5)

#define _VECTORS_SIZE 12


/* Bit numbers */

/* ICR */
#define INT1	7
#define INT0	6
#define LLIE	5
#define TOIE0	4
#define ISC11	3
#define ISC10	2
#define ISC01	1
#define ISC00

/* IFR */
#define INTF1	7
#define INTF0	6
#define TOV0	4

/* MCUCS */
#define PLUPB	7
#define SE	5
#define SM	4
#define WDRF	3
#define EXTRF	1
#define PORF	0

/* TCCR0 */
#define FOV0	7
#define OOM01	4
#define OOM00	3
#define CS02	2
#define CS01	1
#define CS00	0

/* MODCR */
#define ONTIM4	7
#define ONTIM3	6
#define ONTIM2	5
#define ONTIM1	4
#define ONTIM0	3
#define MCONF2	2
#define MCONF1	1
#define MCONF0	0

/* WDTCR */
#define WDTOE	4
#define WDE	3
#define WDP2	2
#define WDP1	1
#define WDP0	0

/*
   PA2 = IR
 */

/* PORTA */
#define PA3	3
#define PA2	2
#define PA1	1
#define PA0	0

/* PACR */
#define DDA3	3
#define PA2HC	2
#define DDA1	1
#define DDA0	0

/* PINA */
#define PINA3	3
#define PINA1	1
#define PINA0	0

/*
   PB4 = INT1
   PB3 = INT0
   PB2 = T0
   PB1 = AIN1
   PB0 = AIN0
 */

/* PINB */
#define PINB7	7
#define PINB6	6
#define PINB5	5
#define PINB4	4
#define PINB3	3
#define PINB2	2
#define PINB1	1
#define PINB0	0

/* PORTD */
#define PD7	7
#define PD6	6
#define PD5	5
#define PD4	4
#define PD3	3
#define PD2	2
#define PD1	1
#define PD0	0

/* DDRD */
#define DDD7	7
#define DDD6	6
#define DDD5	5
#define DDD4	4
#define DDD3	3
#define DDD2	2
#define DDD1	1
#define DDD0	0

/* PIND */
#define PIND7	7
#define PIND6	6
#define PIND5	5
#define PIND4	4
#define PIND3	3
#define PIND2	2
#define PIND1	1
#define PIND0	0

/* ACSR */
#define ACD	7
#define ACO	5
#define ACI	4
#define ACIE	3
#define ACIS1	1
#define ACIS0	0

/* Last memory addresses */
#define RAMEND		0x1F
#define XRAMEND		0x0
#define E2END		0x0
#define E2PAGESIZE  0
#define FLASHEND	0x7FF


/* Fuses */

#define FUSE_MEMORY_SIZE 1

/* Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_CKSEL2      (unsigned char)~_BV(2)
#define FUSE_CKSEL3      (unsigned char)~_BV(3)
#define FUSE_INTCAP      (unsigned char)~_BV(4)
#define FUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3)


/* Lock Bits */
#define __LOCK_BITS_EXIST


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x91
#define SIGNATURE_2 0x07


#endif /* _AVR_IOTN28_H_ */
