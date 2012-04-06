/* Copyright (c) 2007 Atmel Corporation
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
  POSSIBILITY OF SUCH DAMAGE. 
*/

/* $Id: iotn43u.h,v 1.4.2.5 2008/10/17 23:27:52 arcanum Exp $ */

/* avr/iotn43u.h - definitions for ATtiny43U */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn43u.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_IOTN43U_H_
#define _AVR_IOTN43U_H_ 1

/* Registers and associated bit numbers */

#define PRR _SFR_IO8(0x00)
#define PRADC  0
#define PRUSI  1
#define PRTIM0 2
#define PRTIM1 3

#define DIDR0 _SFR_IO8(0x01)
#define ADC0D  0
#define ADC1D  1
#define ADC2D  2
#define ADC3D  3
#define AIN0D  4
#define AIN1D  5

/* Reserved [0x02] */

#define ADCSRB _SFR_IO8(0x03)
#define ADTS0  0
#define ADTS1  1
#define ADTS2  2
#define ADLAR  4
#define ACME   6

#define ADC _SFR_IO16(0x04)

#define ADCL _SFR_IO8(0x04)
#define ADCL0  0
#define ADCL1  1
#define ADCL2  2
#define ADCL3  3
#define ADCL4  4
#define ADCL5  5
#define ADCL6  6
#define ADCL7  7

#define ADCH _SFR_IO8(0x05)
#define ADCH0  0
#define ADCH1  1
#define ADCH2  2
#define ADCH3  3
#define ADCH4  4
#define ADCH5  5
#define ADCH6  6
#define ADCH7  7

#define ADCSRA _SFR_IO8(0x06)
#define ADPS0  0
#define ADPS1  1
#define ADPS2  2
#define ADIE   3
#define ADIF   4
#define ADATE  5
#define ADSC   6
#define ADEN   7

#define ADMUX _SFR_IO8(0x07)
#define MUX0   0
#define MUX1   1
#define MUX2   2
#define REFS0  6

#define ACSR _SFR_IO8(0x08)
#define ACIS0  0
#define ACIS1  1
#define ACIE   3
#define ACI    4
#define ACO    5
#define ACBG   6
#define ACD    7

/* Reserved [0x09], [0x0A] */

#define TIFR1 _SFR_IO8(0x0B)
#define TOV1   0
#define OCF1A  1
#define OCF1B  2

#define TIMSK1 _SFR_IO8(0x0C)
#define TOIE1  0
#define OCIE1A 1
#define OCIE1B 2

#define USICR _SFR_IO8(0x0D)
#define USITC  0
#define USICLK 1
#define USICS0 2
#define USICS1 3
#define USIWM0 4
#define USIWM1 5
#define USIOIE 6
#define USISIE 7

#define USISR _SFR_IO8(0x0E)
#define USICNT0 0
#define USICNT1 1
#define USICNT2 2
#define USICNT3 3
#define USIDC   4
#define USIPF   5
#define USIOIF  6
#define USISIF  7

#define USIDR _SFR_IO8(0x0F)
#define USIDR0 0
#define USIDR1 1
#define USIDR2 2
#define USIDR3 3
#define USIDR4 4
#define USIDR5 5
#define USIDR6 6
#define USIDR7 7

#define USIBR _SFR_IO8(0x10)
#define USIBR0 0
#define USIBR1 1
#define USIBR2 2
#define USIBR3 3
#define USIBR4 4
#define USIBR5 5
#define USIBR6 6
#define USIBR7 7

/* Reserved [0x11] */

#define PCMSK0 _SFR_IO8(0x12)
#define PCINT0 0
#define PCINT1 1
#define PCINT2 2
#define PCINT3 3
#define PCINT4 4
#define PCINT5 5
#define PCINT6 6
#define PCINT7 7

#define GPIOR0 _SFR_IO8(0x13)
#define GPIOR00 0
#define GPIOR01 1
#define GPIOR02 2
#define GPIOR03 3
#define GPIOR04 4
#define GPIOR05 5
#define GPIOR06 6
#define GPIOR07 7

#define GPIOR1 _SFR_IO8(0x14)
#define GPIOR10 0
#define GPIOR11 1
#define GPIOR12 2
#define GPIOR13 3
#define GPIOR14 4
#define GPIOR15 5
#define GPIOR16 6
#define GPIOR17 7

#define GPIOR2 _SFR_IO8(0x15)
#define GPIOR20 0
#define GPIOR21 1
#define GPIOR22 2
#define GPIOR23 3
#define GPIOR24 4
#define GPIOR25 5
#define GPIOR26 6
#define GPIOR27 7

#define PINB _SFR_IO8(0x16)
#define PINB0 0
#define PINB1 1
#define PINB2 2
#define PINB3 3
#define PINB4 4
#define PINB5 5
#define PINB6 6
#define PINB7 7

#define DDRB _SFR_IO8(0x17)
#define DDB0  0
#define DDB1  1
#define DDB2  2
#define DDB3  3
#define DDB4  4
#define DDB5  5
#define DDB6  6
#define DDB7  7

#define PORTB _SFR_IO8(0x18)
#define PORTB0 0
#define PORTB1 1
#define PORTB2 2
#define PORTB3 3
#define PORTB4 4
#define PORTB5 5
#define PORTB6 6
#define PORTB7 7

#define PINA _SFR_IO8(0x19)
#define PINA0 0
#define PINA1 1
#define PINA2 2
#define PINA3 3
#define PINA4 4
#define PINA5 5
#define PINA6 6
#define PINA7 7

#define DDRA _SFR_IO8(0x1A)
#define DDA0 0
#define DDA1 1
#define DDA2 2
#define DDA3 3
#define DDA4 4
#define DDA5 5
#define DDA6 6
#define DDA7 7

#define PORTA _SFR_IO8(0x1B)
#define PORTA0 0
#define PORTA1 1
#define PORTA2 2
#define PORTA3 3
#define PORTA4 4
#define PORTA5 5
#define PORTA6 6
#define PORTA7 7

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)
#define EERE    0
#define EEPE    1
#define EEMPE   2
#define EERIE   3
#define EEPM0   4
#define EEPM1   5

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEARL	_SFR_IO8(0x1E)

/* Reserved [0x1F] */

#define PCMSK1 _SFR_IO8(0x20)
#define PCINT8  0
#define PCINT9  1
#define PCINT10 2
#define PCINT11 3

#define WDTCSR _SFR_IO8(0x21)
#define WDP0 0
#define WDP1 1
#define WDP2 2
#define WDE  3
#define WDCE 4
#define WDP3 5
#define WDIE 6
#define WDIF 7

/* Reserved [0x22] */

#define GTCCR _SFR_IO8(0x23)
#define PSR10 0
#define TSM   7

/* Reserved [0x24], [0x25] */

#define CLKPR _SFR_IO8(0x26)
#define CLKPS0 0
#define CLKPS1 1
#define CLKPS2 2
#define CLKPS3 3
#define CLKPCE 7

/* Reserved [0x27],[0x28],[0x29],[0x2A] */

#define OCR1B _SFR_IO8(0x2B)
#define OCR1B_0 0
#define OCR1B_1 1
#define OCR1B_2 2
#define OCR1B_3 3
#define OCR1B_4 4
#define OCR1B_5 5
#define OCR1B_6 6
#define OCR1B_7 7

#define OCR1A _SFR_IO8(0x2C)
#define OCR1A_0 0
#define OCR1A_1 1
#define OCRA1_2 2
#define OCRA1_3 3
#define OCRA1_4 4
#define OCRA1_5 5
#define OCRA1_6 6
#define OCRA1_7 7

#define TCNT1 _SFR_IO8(0x2D)
#define TCNT1_0 0
#define TCNT1_1 1
#define TCNT1_2 2
#define TCNT1_3 3
#define TCNT1_4 4
#define TCNT1_5 5
#define TCNT1_6 6
#define TCNT1_7 7

#define TCCR1B _SFR_IO8(0x2E)
#define CS10  0
#define CS11  1
#define CS12  2
#define WGM12 3
#define FOC1B 6
#define FOC1A 7

#define TCCR1A _SFR_IO8(0x2F)
#define WGM10  0
#define WGM11  1
#define COM1B0 4
#define COM1B1 5
#define COM1A0 6
#define COM1A1 7

#define TCCR0A _SFR_IO8(0x30)
#define WGM00  0
#define WGM01  1
#define COM0B0 4
#define COM0B1 5
#define COM0A0 6
#define COM0A1 7

#define OSCCAL _SFR_IO8(0x31)
#define CAL0 0
#define CAL1 1
#define CAL2 2
#define CAL3 3
#define CAL4 4
#define CAL5 5
#define CAL6 6
#define CAL7 7

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
#define CS00  0
#define CS01  1
#define CS02  2
#define WGM02 3
#define FOC0B 6
#define FOC0A 7

#define MCUSR _SFR_IO8(0x34)
#define PORF  0
#define EXTRF 1
#define BORF  2
#define WDRF  3

#define MCUCR _SFR_IO8(0x35)
#define ISC00 0
#define ISC01 1
#define BODSE 2
#define SM0   3
#define SM1   4
#define SE    5
#define PUD   6
#define BODS  7

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
#define RFLB  3
#define CTPB  4

#define TIFR0 _SFR_IO8(0x38)
#define TOV0  0
#define OCF0A 1
#define OCF0B 2

#define TIMSK0 _SFR_IO8(0x39)
#define TOIE0  0
#define OCIE0A 1
#define OCIE0B 2

#define GIFR _SFR_IO8(0x3A)
#define PCIF0 4
#define PCIF1 5
#define INTF0 6

#define GIMSK _SFR_IO8(0x3B)
#define PCIE0 4
#define PCIE1 5
#define INT0  6

#define OCR0B _SFR_IO8(0x3C)
#define OCR0B_0 0
#define OCR0B_1 1
#define OCR0B_2 2
#define OCR0B_3 3
#define OCR0B_4 4
#define OCR0B_5 5
#define OCR0B_6 6
#define OCR0B_7 7



/* Interrupt Vectors */
/* Interrupt vector 0 is the reset vector. */

/* External Interrupt Request 0 */
#define INT0_vect           _VECTOR(1)

/* Pin Change Interrupt Request 0 */
#define PCINT0_vect         _VECTOR(2)

/* Pin Change Interrupt Request 1 */
#define PCINT1_vect         _VECTOR(3)

/* Watchdog Time-out */
#define WDT_vect            _VECTOR(4)

/* Timer/Counter1 Compare Match A */
#define TIM1_COMPA_vect     _VECTOR(5)

/* Timer/Counter1 Compare Match B */
#define TIM1_COMPB_vect     _VECTOR(6)

/* Timer/Counter1 Overflow */
#define TIM1_OVF_vect       _VECTOR(7)

/* Timer/Counter0 Compare Match A */
#define TIM0_COMPA_vect     _VECTOR(8)

/* Timer/Counter0 Compare Match B */
#define TIM0_COMPB_vect     _VECTOR(9)

/* Timer/Counter0 Overflow */
#define TIM0_OVF_vect       _VECTOR(10)

/* Analog Comparator */
#define ANA_COMP_vect       _VECTOR(11)

/* ADC Conversion Complete */
#define ADC_vect            _VECTOR(12)

/* EEPROM Ready */
#define EE_RDY_vect         _VECTOR(13)

/* USI START */
#define USI_START_vect      _VECTOR(14)

/* USI Overflow */
#define USI_OVF_vect        _VECTOR(15)

#define _VECTORS_SIZE 32


/* Constants */
#define SPM_PAGESIZE   64
#define RAMEND         0x15F
#define XRAMEND        0x00
#define E2END          0x3F
#define E2PAGESIZE     4
#define FLASHEND       0xFFF


/* Fuse Information */
#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0  (unsigned char)~_BV(0)
#define FUSE_CKSEL1  (unsigned char)~_BV(1)
#define FUSE_CKSEL2  (unsigned char)~_BV(2)
#define FUSE_CKSEL3  (unsigned char)~_BV(3)
#define FUSE_SUT0    (unsigned char)~_BV(4)
#define FUSE_SUT1    (unsigned char)~_BV(5)
#define FUSE_CKOUT   (unsigned char)~_BV(6)
#define FUSE_CKDIV8  (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0 & FUSE_CKDIV8)

/* High Fuse Byte */
#define FUSE_BODLEVEL0   (unsigned char)~_BV(0)
#define FUSE_BODLEVEL1   (unsigned char)~_BV(1)
#define FUSE_BODLEVEL2   (unsigned char)~_BV(2)
#define FUSE_EESAVE      (unsigned char)~_BV(3)
#define FUSE_WDTON       (unsigned char)~_BV(4)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_DWEN        (unsigned char)~_BV(6)
#define FUSE_RSTDISBL    (unsigned char)~_BV(7)
#define HFUSE_DEFAULT (FUSE_SPIEN)    

/* Extended Fuse Byte */
#define FUSE_SELFPRGEN   (unsigned char)~_BV(0)
#define EFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x92
#define SIGNATURE_2 0x0C


#endif /* _AVR_IOTN43U_H_ */
