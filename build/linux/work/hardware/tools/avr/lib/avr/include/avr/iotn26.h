/* Copyright (c) 2004,2005 Eric B. Weddington
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

/* $Id: iotn26.h,v 1.15.2.6 2008/10/17 23:27:52 arcanum Exp $ */

/* avr/iotn26.h - definitions for ATtiny26 */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn26.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

#ifndef _AVR_IOTN26_H_
#define _AVR_IOTN26_H_ 1

/* Registers and associated bit numbers */

/* Reserved [0x00..0x03] */

#define ADCW    _SFR_IO16(0x04)
#ifndef __ASSEMBLER__
#define ADC     _SFR_IO16(0x04)
#endif

#define ADCL    _SFR_IO8(0x04)
#define ADCH    _SFR_IO8(0x05)

#define ADCSR   _SFR_IO8(0x06)
#define ADPS0   0
#define ADPS1   1
#define ADPS2   2
#define ADIE    3
#define ADIF    4
#define ADFR    5
#define ADSC    6
#define ADEN    7

#define ADMUX   _SFR_IO8(0x07)
#define MUX0    0
#define MUX1    1
#define MUX2    2
#define MUX3    3
#define MUX4    4
#define ADLAR   5
#define REFS0   6
#define REFS1   7

#define ACSR    _SFR_IO8(0x08)
#define ACIS0   0
#define ACIS1   1
#define ACME    2
#define ACIE    3
#define ACI     4
#define ACO     5
#define ACBG    6
#define ACD     7

/* Reserved [0x09..0x0C] */

#define USICR   _SFR_IO8(0x0D)
#define USITC   0
#define USICLK  1
#define USICS0  2
#define USICS1  3
#define USIWM0  4
#define USIWM1  5
#define USIOIE  6
#define USISIE  7

#define USISR   _SFR_IO8(0x0E)
#define USICNT0 0
#define USICNT1 1
#define USICNT2 2
#define USICNT3 3
#define USIDC   4
#define USIPF   5
#define USIOIF  6
#define USISIF  7

#define USIDR   _SFR_IO8(0x0F)

/* Reserved [0x10..0x15] */


#define PINB    _SFR_IO8(0x16)
#define PINB0   0
#define PINB1   1
#define PINB2   2
#define PINB3   3
#define PINB4   4
#define PINB5   5
#define PINB6   6
#define PINB7   7

#define DDRB    _SFR_IO8(0x17)
#define DDB0    0
#define DDB1    1
#define DDB2    2
#define DDB3    3
#define DDB4    4
#define DDB5    5
#define DDB6    6
#define DDB7    7

#define PORTB   _SFR_IO8(0x18)
#define PB0     0
#define PB1     1
#define PB2     2
#define PB3     3
#define PB4     4
#define PB5     5
#define PB6     6
#define PB7     7

#define PINA    _SFR_IO8(0x19)
#define PINA0   0
#define PINA1   1
#define PINA2   2
#define PINA3   3
#define PINA4   4
#define PINA5   5
#define PINA6   6
#define PINA7   7

#define DDRA    _SFR_IO8(0x1A)
#define DDA0    0
#define DDA1    1
#define DDA2    2
#define DDA3    3
#define DDA4    4
#define DDA5    5
#define DDA6    6
#define DDA7    7

#define PORTA   _SFR_IO8(0x1B)
#define PA0     0
#define PA1     1
#define PA2     2
#define PA3     3
#define PA4     4
#define PA5     5
#define PA6     6
#define PA7     7

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)
#define EERE    0
#define EEWE    1
#define EEMWE   2
#define EERIE   3

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO8(0x1E)
#define EEARL	_SFR_IO8(0x1E)

/* Reserved [0x1F..0x20] */

#define WDTCR   _SFR_IO8(0x21)
#define WDP0    0
#define WDP1    1
#define WDP2    2
#define WDE     3
#define WDCE    4

/* Reserved [0x22..0x28] */

#define PLLCSR  _SFR_IO8(0x29)
#define PLOCK   0
#define PLLE    1
#define PCKE    2

/* Reserved [0x2A] */

#define OCR1C   _SFR_IO8(0x2B)

#define OCR1B   _SFR_IO8(0x2C)

#define OCR1A   _SFR_IO8(0x2D)

#define TCNT1   _SFR_IO8(0x2E)

#define TCCR1B  _SFR_IO8(0x2F)
#define CS10    0
#define CS11    1
#define CS12    2
#define CS13    3
#define PSR1    6
#define CTC1    7

#define TCCR1A  _SFR_IO8(0x30)
#define PWM1B   0
#define PWM1A   1
#define FOC1B   2
#define FOC1A   3
#define COM1B0  4
#define COM1B1  5
#define COM1A0  6
#define COM1A1  7

#define OSCCAL  _SFR_IO8(0x31)

#define TCNT0   _SFR_IO8(0x32)

#define TCCR0   _SFR_IO8(0x33)
#define CS00    0
#define CS01    1
#define CS02    2
#define PSR0    3

#define MCUSR   _SFR_IO8(0x34)
#define PORF    0
#define EXTRF   1
#define BORF    2
#define WDRF    3

#define MCUCR   _SFR_IO8(0x35)
#define ISC00   0
#define ISC01   1
#define SM0     3
#define SM1     4
#define SE      5
#define PUD     6

/* Reserved [0x36..0x37] */

#define TIFR    _SFR_IO8(0x38)
#define TOV0    1
#define TOV1    2
#define OCF1B   5
#define OCF1A   6

#define TIMSK   _SFR_IO8(0x39)
#define TOIE0   1
#define TOIE1   2
#define OCIE1B  5
#define OCIE1A  6

#define GIFR    _SFR_IO8(0x3A)
#define PCIF    5
#define INTF0   6

#define GIMSK   _SFR_IO8(0x3B)
#define PCIE0   4
#define PCIE1   5
#define INT0    6

/* Reserved [0x3C] */

/* SP [0x3D] */

/* Reserved [0x3E] */

/* SREG [0x3F] */


/* Interrupt vectors */
/* Interrupt vector 0 is the reset vector. */
/* External Interrupt 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt Request 0 */
#define IO_PINS_vect			_VECTOR(2)
#define SIG_PIN_CHANGE			_VECTOR(2)

/* Timer/Counter1 Compare Match 1A */
#define TIMER1_CMPA_vect		_VECTOR(3)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(3)

/* Timer/Counter1 Compare Match 1B */
#define TIMER1_CMPB_vect		_VECTOR(4)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(4)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF1_vect		_VECTOR(5)
#define SIG_OVERFLOW1			_VECTOR(5)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF0_vect		_VECTOR(6)
#define SIG_OVERFLOW0			_VECTOR(6)

/* USI Start */
#define USI_STRT_vect			_VECTOR(7)
#define SIG_USI_START			_VECTOR(7)

/* USI Overflow */
#define USI_OVF_vect			_VECTOR(8)
#define SIG_USI_OVERFLOW		_VECTOR(8)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(9)
#define SIG_EEPROM_READY		_VECTOR(9)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(10)
#define SIG_ANA_COMP			_VECTOR(10)
#define SIG_COMPARATOR			_VECTOR(10)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(11)
#define SIG_ADC				_VECTOR(11)

#define _VECTORS_SIZE 24


/* Constants */
#define RAMEND      0xDF
#define XRAMEND     0xDF
#define E2END       0x7F
#define E2PAGESIZE  4
#define FLASHEND    0x07FF


/* Fuses */

#define FUSE_MEMORY_SIZE 2

/* Low Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_CKSEL2      (unsigned char)~_BV(2)
#define FUSE_CKSEL3      (unsigned char)~_BV(3)
#define FUSE_SUT0        (unsigned char)~_BV(4)
#define FUSE_SUT1        (unsigned char)~_BV(5)
#define FUSE_CKOPT       (unsigned char)~_BV(6)
#define FUSE_PLLCK       (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_CKSEL1 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0)

/* High Fuse Byte */
#define FUSE_BODEN       (unsigned char)~_BV(0)
#define FUSE_BODLEVEL    (unsigned char)~_BV(1)
#define FUSE_EESAVE      (unsigned char)~_BV(2)
#define FUSE_SPIEN       (unsigned char)~_BV(3)
#define FUSE_RSTDISBL    (unsigned char)~_BV(4)
#define HFUSE_DEFAULT (FUSE_SPIEN)


/* Lock Bits */
#define __LOCK_BITS_EXIST


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x91
#define SIGNATURE_2 0x09


#endif  /* _AVR_IOTN26_H_ */
