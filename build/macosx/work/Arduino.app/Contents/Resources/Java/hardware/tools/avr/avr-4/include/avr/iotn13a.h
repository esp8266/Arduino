/* Copyright (c) 2008 Atmel Corporation
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

/* $Id: iotn13a.h,v 1.1.2.5 2008/08/06 22:45:08 arcanum Exp $ */

/* avr/iotn13a.h - definitions for ATtiny13 */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn13a.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_ATTINY13A_H_
#define _AVR_ATTINY13A_H_ 1


/* Registers and associated bit numbers. */

#define ADCSRB _SFR_IO8(0x03)
#define ADTS0 0
#define ADTS1 1
#define ADTS2 2
#define ACME 6

#ifndef __ASSEMBLER__
#define ADC _SFR_IO16(0x04)
#endif
#define ADCW _SFR_IO16(0x04)

#define ADCL _SFR_IO8(0x04)
#define ADCL0 0
#define ADCL1 1
#define ADCL2 2
#define ADCL3 3
#define ADCL4 4
#define ADCL5 5
#define ADCL6 6
#define ADCL7 7

#define ADCH _SFR_IO8(0x05)
#define ADCH0 0
#define ADCH1 1
#define ADCH2 2
#define ADCH3 3
#define ADCH4 4
#define ADCH5 5
#define ADCH6 6
#define ADCH7 7

#define ADCSRA _SFR_IO8(0x06)
#define ADPS0 0
#define ADPS1 1
#define ADPS2 2
#define ADIE 3
#define ADIF 4
#define ADATE 5
#define ADSC 6
#define ADEN 7

#define ADMUX _SFR_IO8(0x07)
#define MUX0 0
#define MUX1 1
#define ADLAR 5
#define REFS0 6

#define ACSR _SFR_IO8(0x08)
#define ACIS0 0
#define ACIS1 1
#define ACIE 3
#define ACI 4
#define ACO 5
#define ACBG 6
#define ACD 7

#define DIDR0 _SFR_IO8(0x14)
#define AIN0D 0
#define AIN1D 1
#define ADC1D 2
#define ADC3D 3
#define ADC2D 4
#define ADC0D 5

#define PCMSK _SFR_IO8(0x15)
#define PCINT0 0
#define PCINT1 1
#define PCINT2 2
#define PCINT3 3
#define PCINT4 4
#define PCINT5 5

#define PINB _SFR_IO8(0x16)
#define PINB0 0
#define PINB1 1
#define PINB2 2
#define PINB3 3
#define PINB4 4
#define PINB5 5

#define DDRB _SFR_IO8(0x17)
#define DDB0 0
#define DDB1 1
#define DDB2 2
#define DDB3 3
#define DDB4 4
#define DDB5 5

#define PORTB _SFR_IO8(0x18)
#define PORTB0 0
#define PORTB1 1
#define PORTB2 2
#define PORTB3 3
#define PORTB4 4
#define PORTB5 5

#define EECR _SFR_IO8(0x1C)
#define EERE 0
#define EEWE 1
#define EEMWE 2
#define EERIE 3
#define EEPM0 4
#define EEPM1 5

#define EEDR _SFR_IO8(0x1D)
#define EEDR0 0
#define EEDR1 1
#define EEDR2 2
#define EEDR3 3
#define EEDR4 4
#define EEDR5 5
#define EEDR6 6
#define EEDR7 7

#define EEARL _SFR_IO8(0x1E)

#define EEAR _SFR_IO8(0x1E)
#define EEAR0 0
#define EEAR1 1
#define EEAR2 2
#define EEAR3 3
#define EEAR4 4
#define EEAR5 5

#define WDTCR _SFR_IO8(0x21)
#define WDP0 0
#define WDP1 1
#define WDP2 2
#define WDE 3
#define WDCE 4
#define WDP3 5
#define WDTIE 6
#define WDTIF 7

#define PRR _SFR_IO8(0x25)
#define PRADC 0
#define PRSPI 1
#define PRTIM0 2

#define CLKPR _SFR_IO8(0x26)
#define CLKPS0 0
#define CLKPS1 1
#define CLKPS2 2
#define CLKPS3 3
#define CLKPCE 7

#define GTCCR _SFR_IO8(0x28)
#define PSR10 0
#define TSM 7

#define OCR0B _SFR_IO8(0x29)
#define OCR0B_0 0
#define OCR0B_1 1
#define OCR0B_2 2
#define OCR0B_3 3
#define OCR0B_4 4
#define OCR0B_5 5
#define OCR0B_6 6
#define OCR0B_7 7

#define DWDR _SFR_IO8(0x2E)
#define DWDR0 0
#define DWDR1 1
#define DWDR2 2
#define DWDR3 3
#define DWDR4 4
#define DWDR5 5
#define DWDR6 6
#define DWDR7 7

#define TCCR0A _SFR_IO8(0x2F)
#define WGM00 0
#define WGM01 1
#define COM0B0 4
#define COM0B1 5
#define COM0A0 6
#define COM0A1 7

#define BODCR _SFR_IO8(0x30)
#define BPDSE 0
#define BPDS 1

#define OSCCAL _SFR_IO8(0x31)
#define CAL0 0
#define CAL1 1
#define CAL2 2
#define CAL3 3
#define CAL4 4
#define CAL5 5
#define CAL6 6

#define TCNT0 _SFR_IO8(0x32)
#define TCNT0_0 0
#define TCNT0_1 1
#define TCNT0_2 2
#define TCNT0_3 3
#define TCNT0_4 4
#define TCNT0_5 5
#define TCNT0_6 6
#define TCNT0_7 7

#define TCCR0B _SFR_IO8(0x33)
#define CS00 0
#define CS01 1
#define CS02 2
#define WGM02 3
#define FOC0B 6
#define FOC0A 7

#define MCUSR _SFR_IO8(0x34)
#define PORF 0
#define EXTRF 1
#define BORF 2
#define WDRF 3

#define MCUCR _SFR_IO8(0x35)
#define ISC00 0
#define ISC01 1
#define SM0 3
#define SM1 4
#define SE 5
#define PUD 6

#define OCR0A _SFR_IO8(0x36)
#define OCR0A_0 0
#define OCR0A_1 1
#define OCR0A_2 2
#define OCR0A_3 3
#define OCR0A_4 4
#define OCR0A_5 5
#define OCR0A_6 6
#define OCR0A_7 7

#define SPMCSR _SFR_IO8(0x37)
#define SPMEN 0
#define PGERS 1
#define PGWRT 2
#define RFLB 3
#define CTPB 4

#define TIFR0 _SFR_IO8(0x38)
#define TOV0 1
#define OCF0A 2
#define OCF0B 3

#define TIMSK0 _SFR_IO8(0x39)
#define TOIE0 1
#define OCIE0A 2
#define OCIE0B 3

#define GIFR _SFR_IO8(0x3A)
#define PCIF 5
#define INTF0 6

#define GIMSK _SFR_IO8(0x3B)
#define PCIE 5
#define INT0 6


/* Interrupt vectors */
/* Vector 0 is the reset vector */
#define INT0_vect_num  1
#define INT0_vect      _VECTOR(1)  /* External Interrupt 0 */
#define PCINT0_vect_num  2
#define PCINT0_vect      _VECTOR(2)  /* External Interrupt Request 0 */
#define TIM0_OVF_vect_num  3
#define TIM0_OVF_vect      _VECTOR(3)  /* Timer/Counter0 Overflow */
#define EE_RDY_vect_num  4
#define EE_RDY_vect      _VECTOR(4)  /* EEPROM Ready */
#define ANA_COMP_vect_num  5
#define ANA_COMP_vect      _VECTOR(5)  /* Analog Comparator */
#define TIM0_COMPA_vect_num  6
#define TIM0_COMPA_vect      _VECTOR(6)  /* Timer/Counter Compare Match A */
#define TIM0_COMPB_vect_num  7
#define TIM0_COMPB_vect      _VECTOR(7)  /* Timer/Counter Compare Match B */
#define WDT_vect_num  8
#define WDT_vect      _VECTOR(8)  /* Watchdog Time-out */
#define ADC_vect_num  9
#define ADC_vect      _VECTOR(9)  /* ADC Conversion Complete */

#define _VECTOR_SIZE 2 /* Size of individual vector. */
#define _VECTORS_SIZE (10 * _VECTOR_SIZE)


/* Constants */
#define SPM_PAGESIZE (32)
#define RAMSTART     (0x60)
#define RAMSIZE      (64)
#define RAMEND       (RAMSTART + RAMSIZE - 1)
#define XRAMSTART    (NA)
#define XRAMSIZE     (0)
#if XRAMSIZE > 0 && XRAMSTART > 0
#define XRAMEND      (XRAMSTART + XRAMSIZE - 1)
#else
#define XRAMEND      (RAMEND + XRAMSIZE)
#endif
#define E2END        (64 - 1)
#define E2PAGESIZE   (4)
#define FLASHEND     (1024 - 1)


/* Fuses */
#define FUSE_MEMORY_SIZE 2

/* Low Fuse Byte */
#define FUSE_CKSEL0 (unsigned char)~_BV(0)  /* Select Clock Source */
#define FUSE_CKSEL1 (unsigned char)~_BV(1)  /* Select Clock Source */
#define FUSE_SUT0 (unsigned char)~_BV(2)  /* Select start-up time */
#define FUSE_SUT1 (unsigned char)~_BV(3)  /* Select start-up time */
#define FUSE_CKDIV8 (unsigned char)~_BV(4)  /* Start up with system clock divided by 8 */
#define FUSE_WDTON (unsigned char)~_BV(5)  /* Watch dog timer always on */
#define FUSE_EESAVE (unsigned char)~_BV(6)  /* Keep EEprom contents during chip erase */
#define FUSE_SPIEN (unsigned char)~_BV(7)  /* SPI programming enable */
#define LFUSE_DEFAULT (FUSE_SPIEN & FUSE_CKDIV8 & FUSE_SUT0 & FUSE_CKSEL0)

/* High Fuse Byte */
#define FUSE_RSTDISBL (unsigned char)~_BV(0)  /* Disable external reset */
#define FUSE_BODLEVEL0 (unsigned char)~_BV(1)  /* Enable BOD and select level */
#define FUSE_BODLEVEL1 (unsigned char)~_BV(2)  /* Enable BOD and select level */
#define FUSE_DWEN (unsigned char)~_BV(3)  /* DebugWire Enable */
#define FUSE_SELFPRGEN (unsigned char)~_BV(4)  /* Self Programming Enable */
#define HFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x90
#define SIGNATURE_2 0x07


#endif /* _AVR_ATTINY13A_H_ */

