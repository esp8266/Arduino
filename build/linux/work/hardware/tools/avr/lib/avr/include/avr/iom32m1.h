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
  POSSIBILITY OF SUCH DAMAGE. 
*/

/* $Id: iom32m1.h,v 1.1.2.7 2008/10/17 23:27:49 arcanum Exp $ */

/* avr/iom32m1.h - definitions for ATmega32M1. */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom32m1.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_IOM32M1_H_
#define _AVR_IOM32M1_H_ 1

/* Registers and associated bit numbers */


#define PINB _SFR_IO8(0x03)
#define PINB0  0
#define PINB1  1
#define PINB2  2
#define PINB3  3
#define PINB4  4
#define PINB5  5
#define PINB6  6
#define PINB7  7

#define DDRB _SFR_IO8(0x04)
#define DDB0 0
#define DDB1 1
#define DDB2 2
#define DDB3 3
#define DDB4 4
#define DDB5 5
#define DDB6 6
#define DDB7 7

#define PORTB _SFR_IO8(0x05)
#define PORTB0 0
#define PORTB1 1
#define PORTB2 2
#define PORTB3 3
#define PORTB4 4
#define PORTB5 5
#define PORTB6 6
#define PORTB7 7

#define PINC _SFR_IO8(0x06)
#define PINC0 0
#define PINC1 1
#define PINC2 2
#define PINC3 3
#define PINC4 4
#define PINC5 5
#define PINC6 6
#define PINC7 7

#define DDRC _SFR_IO8(0x07)
#define DDC0 0
#define DDC1 1
#define DDC2 2
#define DDC3 3
#define DDC4 4
#define DDC5 5
#define DDC6 6
#define DDC7 7

#define PORTC _SFR_IO8(0x08)
#define PORTC0 0
#define PORTC1 1
#define PORTC2 2
#define PORTC3 3
#define PORTC4 4
#define PORTC5 5
#define PORTC6 6
#define PORTC7 7

#define PIND _SFR_IO8(0x09)
#define PIND0 0
#define PIND1 1
#define PIND2 2
#define PIND3 3
#define PIND4 4
#define PIND5 5
#define PIND6 6
#define PIND7 7

#define DDRD _SFR_IO8(0x0A)
#define DDD0 0
#define DDD1 1
#define DDD2 2
#define DDD3 3
#define DDD4 4
#define DDD5 5
#define DDD6 6
#define DDD7 7

#define PORTD _SFR_IO8(0x0B)
#define PORTD0 0
#define PORTD1 1
#define PORTD2 2
#define PORTD3 3
#define PORTD4 4
#define PORTD5 5
#define PORTD6 6
#define PORTD7 7

#define PINE _SFR_IO8(0x0C)
#define PINE0 0
#define PINE1 1
#define PINE2 2

#define DDRE _SFR_IO8(0x0D)
#define DDE0 0
#define DDE1 1
#define DDE2 2

#define PORTE _SFR_IO8(0x0E)
#define PORTE0 0
#define PORTE1 1
#define PORTE2 2

#define TIFR0 _SFR_IO8(0x15)
#define TOV0 0
#define OCF0A 1
#define OCF0B 2

#define TIFR1 _SFR_IO8(0x16)
#define TOV1 0
#define OCF1A 1
#define OCF1B 2
#define ICF1 5

#define GPIOR1 _SFR_IO8(0x19)
#define GPIOR10 0
#define GPIOR11 1
#define GPIOR12 2
#define GPIOR13 3
#define GPIOR14 4
#define GPIOR15 5
#define GPIOR16 6
#define GPIOR17 7

#define GPIOR2 _SFR_IO8(0x1A)
#define GPIOR20 0
#define GPIOR21 1
#define GPIOR22 2
#define GPIOR23 3
#define GPIOR24 4
#define GPIOR25 5
#define GPIOR26 6
#define GPIOR27 7

#define PCIFR _SFR_IO8(0x1B)

#define EIFR _SFR_IO8(0x1C)
#define INTF0 0
#define INTF1 1
#define INTF2 2
#define INTF3 3

#define EIMSK _SFR_IO8(0x1D)
#define INT0 0
#define INT1 1
#define INT2 2
#define INT3 3

#define GPIOR0 _SFR_IO8(0x1E)
#define GPIOR00 0
#define GPIOR01 1
#define GPIOR02 2
#define GPIOR03 3
#define GPIOR04 4
#define GPIOR05 5
#define GPIOR06 6
#define GPIOR07 7

#define EECR _SFR_IO8(0x1F)
#define EERE 0
#define EEPE 1
#define EEMPE 2
#define EERIE 3
#define EEPM0 4
#define EEPM1 5

#define EEDR _SFR_IO8(0x20)
#define EEDR0 0
#define EEDR1 1
#define EEDR2 2
#define EEDR3 3
#define EEDR4 4
#define EEDR5 5
#define EEDR6 6
#define EEDR7 7

#define EEAR _SFR_IO16(0x21)

#define EEARL _SFR_IO8(0x21)
#define EEAR0 0
#define EEAR1 1
#define EEAR2 2
#define EEAR3 3
#define EEAR4 4
#define EEAR5 5
#define EEAR6 6
#define EEAR7 7

#define EEARH _SFR_IO8(0x22)
#define EEAR8 0
#define EEAR9 1

#define __EEPROM_REG_LOCATIONS__ 1F2021

#define GTCCR _SFR_IO8(0x23)
#define PSR10 0
#define PSRSYNC 0
#define ICPSEL1 6
#define TSM 7

#define TCCR0A _SFR_IO8(0x24)
#define WGM00 0
#define WGM01 1
#define COM0B0 4
#define COM0B1 5
#define COM0A0 6
#define COM0A1 7

#define TCCR0B _SFR_IO8(0x25)
#define CS00 0
#define CS01 1
#define CS02 2
#define WGM02 3
#define FOC0B 6
#define FOC0A 7

#define TCNT0 _SFR_IO8(0x26)
#define TCNT0_0 0
#define TCNT0_1 1
#define TCNT0_2 2
#define TCNT0_3 3
#define TCNT0_4 4
#define TCNT0_5 5
#define TCNT0_6 6
#define TCNT0_7 7

#define OCR0A _SFR_IO8(0x27)
#define OCR0A_0 0
#define OCR0A_1 1
#define OCR0A_2 2
#define OCR0A_3 3
#define OCR0A_4 4
#define OCR0A_5 5
#define OCR0A_6 6
#define OCR0A_7 7

#define OCR0B _SFR_IO8(0x28)
#define OCR0B_0 0
#define OCR0B_1 1
#define OCR0B_2 2
#define OCR0B_3 3
#define OCR0B_4 4
#define OCR0B_5 5
#define OCR0B_6 6
#define OCR0B_7 7

#define PLLCSR _SFR_IO8(0x29)
#define PLOCK 0
#define PLLE 1
#define PLLF 2

#define SPCR _SFR_IO8(0x2C)
#define SPR0 0
#define SPR1 1
#define CPHA 2
#define CPOL 3
#define MSTR 4
#define DORD 5
#define SPE 6
#define SPIE 7

#define SPSR _SFR_IO8(0x2D)
#define SPI2X 0
#define WCOL 6
#define SPIF 7

#define SPDR _SFR_IO8(0x2E)
#define SPDR0 0
#define SPDR1 1
#define SPDR2 2
#define SPDR3 3
#define SPDR4 4
#define SPDR5 5
#define SPDR6 6
#define SPDR7 7

#define ACSR _SFR_IO8(0x30)
#define AC0O 0
#define AC1O 1
#define AC2O 2
#define AC3O 3
#define AC0IF 4
#define AC1IF 5
#define AC2IF 6
#define AC3IF 7

#define DWDR _SFR_IO8(0x31)

#define SMCR _SFR_IO8(0x33)
#define SE 0
#define SM0 1
#define SM1 2
#define SM2 3

#define MCUSR _SFR_IO8(0x34)
#define PORF 0
#define EXTRF 1
#define BORF 2
#define WDRF 3

#define MCUCR _SFR_IO8(0x35)
#define IVCE 0
#define IVSEL 1
#define PUD 4
#define SPIPS 7

#define SPMCSR _SFR_IO8(0x37)
#define SPMEN 0
#define PGERS 1
#define PGWRT 2
#define BLBSET 3
#define RWWSRE 4
#define RWWSB 6
#define SPMIE 7

#define WDTCSR _SFR_MEM8(0x60)
#define WDP0 0
#define WDP1 1
#define WDP2 2
#define WDE 3
#define WDCE 4
#define WDP3 5
#define WDIE 6
#define WDIF 7

#define CLKPR _SFR_MEM8(0x61)
#define CLKPS0 0
#define CLKPS1 1
#define CLKPS2 2
#define CLKPS3 3
#define CLKPCE 7

#define PRR _SFR_MEM8(0x64)
#define PRADC 0
#define PRLIN 1
#define PRSPI 2
#define PRTIM0 3
#define PRTIM1 4
#define PRPSC 5
#define PRCAN 6

#define OSCCAL _SFR_MEM8(0x66)
#define CAL0 0
#define CAL1 1
#define CAL2 2
#define CAL3 3
#define CAL4 4
#define CAL5 5
#define CAL6 6

#define EICRA _SFR_MEM8(0x69)
#define ISC00 0
#define ISC01 1
#define ISC10 2
#define ISC11 3
#define ISC20 4
#define ISC21 5
#define ISC30 6
#define ISC31 7

#define PCMSK0 _SFR_MEM8(0x6A)

#define PCMSK1 _SFR_MEM8(0x6B)

#define PCMSK2 _SFR_MEM8(0x6C)

#define PCMSK3 _SFR_MEM8(0x6D)

#define TIMSK0 _SFR_MEM8(0x6E)
#define TOIE0 0
#define OCIE0A 1
#define OCIE0B 2

#define TIMSK1 _SFR_MEM8(0x6F)
#define TOIE1 0
#define OCIE1A 1
#define OCIE1B 2
#define ICIE1 5

#define AMP0CSR _SFR_MEM8(0x75)
#define AMP0TS0 0
#define AMP0TS1 1
#define AMP0TS2 2
#define AMPCMP0 3
#define AMP0G0 4
#define AMP0G1 5
#define AMP0IS 6
#define AMP0EN 7

#define AMP1CSR _SFR_MEM8(0x76)
#define AMP1TS0 0
#define AMP1TS1 1
#define AMP1TS2 2
#define AMPCMP1 3
#define AMP1G0 4
#define AMP1G1 5
#define AMP1IS 6
#define AMP1EN 7

#define AMP2CSR _SFR_MEM8(0x77)
#define AMP2TS0 0
#define AMP2TS1 1
#define AMP2TS2 2
#define AMPCMP2 3
#define AMP2G0 4
#define AMP2G1 5
#define AMP2IS 6
#define AMP2EN 7

#ifndef __ASSEMBLER__
#define ADC     _SFR_MEM16(0x78)
#endif
#define ADCW    _SFR_MEM16(0x78)

#define ADCL _SFR_MEM8(0x78)
#define ADCL0 0
#define ADCL1 1
#define ADCL2 2
#define ADCL3 3
#define ADCL4 4
#define ADCL5 5
#define ADCL6 6
#define ADCL7 7

#define ADCH _SFR_MEM8(0x79)
#define ADCH0 0
#define ADCH1 1
#define ADCH2 2
#define ADCH3 3
#define ADCH4 4
#define ADCH5 5
#define ADCH6 6
#define ADCH7 7

#define ADCSRA _SFR_MEM8(0x7A)
#define ADPS0 0
#define ADPS1 1
#define ADPS2 2
#define ADIE 3
#define ADIF 4
#define ADATE 5
#define ADSC 6
#define ADEN 7

#define ADCSRB _SFR_MEM8(0x7B)
#define ADTS0 0
#define ADTS1 1
#define ADTS2 2
#define ADTS3 3
#define AREFEN 4
#define ISRCEN 5
#define ADHSM 6

#define ADMUX _SFR_MEM8(0x7C)
#define MUX0 0
#define MUX1 1
#define MUX2 2
#define MUX3 3
#define ADLAR 5
#define REFS0 6
#define REFS1 7

#define DIDR0 _SFR_MEM8(0x7E)
#define ADC0D 0
#define ADC1D 1
#define ADC2D 2
#define ADC3D 3
#define ADC4D 4
#define ADC5D 5
#define ADC6D 6
#define ADC7D 7

#define DIDR1 _SFR_MEM8(0x7F)
#define ADC8D 0
#define ADC9D 1
#define ADC10D 2
#define AMP0ND 3
#define AMP0PD 4
#define ACMP0D 5
#define AMP2PD 6

#define TCCR1A _SFR_MEM8(0x80)
#define WGM10 0
#define WGM11 1
#define COM1B0 4
#define COM1B1 5
#define COM1A0 6
#define COM1A1 7

#define TCCR1B _SFR_MEM8(0x81)
#define CS10 0
#define CS11 1
#define CS12 2
#define WGM12 3
#define WGM13 4
#define ICES1 6
#define ICNC1 7

#define TCCR1C _SFR_MEM8(0x82)
#define FOC1B 6
#define FOC1A 7

#define TCNT1 _SFR_MEM16(0x84)

#define TCNT1L _SFR_MEM8(0x84)
#define TCNT1L0 0
#define TCNT1L1 1
#define TCNT1L2 2
#define TCNT1L3 3
#define TCNT1L4 4
#define TCNT1L5 5
#define TCNT1L6 6
#define TCNT1L7 7

#define TCNT1H _SFR_MEM8(0x85)
#define TCNT1H0 0
#define TCNT1H1 1
#define TCNT1H2 2
#define TCNT1H3 3
#define TCNT1H4 4
#define TCNT1H5 5
#define TCNT1H6 6
#define TCNT1H7 7

#define ICR1 _SFR_MEM16(0x86)

#define ICR1L _SFR_MEM8(0x86)
#define ICR1L0 0
#define ICR1L1 1
#define ICR1L2 2
#define ICR1L3 3
#define ICR1L4 4
#define ICR1L5 5
#define ICR1L6 6
#define ICR1L7 7

#define ICR1H _SFR_MEM8(0x87)
#define ICR1H0 0
#define ICR1H1 1
#define ICR1H2 2
#define ICR1H3 3
#define ICR1H4 4
#define ICR1H5 5
#define ICR1H6 6
#define ICR1H7 7

#define OCR1A _SFR_MEM16(0x88)

#define OCR1AL _SFR_MEM8(0x88)
#define OCR1AL0 0
#define OCR1AL1 1
#define OCR1AL2 2
#define OCR1AL3 3
#define OCR1AL4 4
#define OCR1AL5 5
#define OCR1AL6 6
#define OCR1AL7 7

#define OCR1AH _SFR_MEM8(0x89)
#define OCR1AH0 0
#define OCR1AH1 1
#define OCR1AH2 2
#define OCR1AH3 3
#define OCR1AH4 4
#define OCR1AH5 5
#define OCR1AH6 6
#define OCR1AH7 7

#define OCR1B _SFR_MEM16(0x8A)

#define OCR1BL _SFR_MEM8(0x8A)
#define OCR1BL0 0
#define OCR1BL1 1
#define OCR1BL2 2
#define OCR1BL3 3
#define OCR1BL4 4
#define OCR1BL5 5
#define OCR1BL6 6
#define OCR1BL7 7

#define OCR1BH _SFR_MEM8(0x8B)
#define OCR1BH0 0
#define OCR1BH1 1
#define OCR1BH2 2
#define OCR1BH3 3
#define OCR1BH4 4
#define OCR1BH5 5
#define OCR1BH6 6
#define OCR1BH7 7

#define DACON _SFR_MEM8(0x90)
#define DAEN 0
#define DAOE 1
#define DALA 2
#define DATS0 4
#define DATS1 5
#define DATS2 6
#define DAATE 7

#define DAC _SFR_MEM16(0x91)

#define DACL _SFR_MEM8(0x91)
#define DACL0 0
#define DACL1 1
#define DACL2 2
#define DACL3 3
#define DACL4 4
#define DACL5 5
#define DACL6 6
#define DACL7 7

#define DACH _SFR_MEM8(0x92)
#define DACH0 0
#define DACH1 1
#define DACH2 2
#define DACH3 3
#define DACH4 4
#define DACH5 5
#define DACH6 6
#define DACH7 7

#define AC0CON _SFR_MEM8(0x94)
#define AC0M0 0
#define AC0M1 1
#define AC0M2 2
#define ACCKSEL 3
#define AC0IS0 4
#define AC0IS1 5
#define AC0IE 6
#define AC0EN 7

#define AC1CON _SFR_MEM8(0x95)
#define AC1M0 0
#define AC1M1 1
#define AC1M2 2
#define AC1ICE 3
#define AC1IS0 4
#define AC1IS1 5
#define AC1IE 6
#define AC1EN 7

#define AC2CON _SFR_MEM8(0x96)
#define AC2M0 0
#define AC2M1 1
#define AC2M2 2
#define AC2IS0 4
#define AC2IS1 5
#define AC2IE 6
#define AC2EN 7

#define AC3CON _SFR_MEM8(0x97)
#define AC3M0 0
#define AC3M1 1
#define AC3M2 2
#define AC3IS0 4
#define AC3IS1 5
#define AC3IE 6
#define AC3EN 7

#define POCR0SA _SFR_MEM16(0xA0)

#define POCR0SAL _SFR_MEM8(0xA0)
#define POCR0SA_0 0
#define POCR0SA_1 1
#define POCR0SA_2 2
#define POCR0SA_3 3
#define POCR0SA_4 4
#define POCR0SA_5 5
#define POCR0SA_6 6
#define POCR0SA_7 7

#define POCR0SAH _SFR_MEM8(0xA1)
#define POCR0SA_8 0
#define POCR0SA_9 1
#define POCR0SA_00 2
#define POCR0SA_01 3

#define POCR0RA _SFR_MEM16(0xA2)

#define POCR0RAL _SFR_MEM8(0xA2)
#define POCR0RA_0 0
#define POCR0RA_1 1
#define POCR0RA_2 2
#define POCR0RA_3 3
#define POCR0RA_4 4
#define POCR0RA_5 5
#define POCR0RA_6 6
#define POCR0RA_7 7

#define POCR0RAH _SFR_MEM8(0xA3)
#define POCR0RA_8 0
#define POCR0RA_9 1
#define POCR0RA_00 2
#define POCR0RA_01 3

#define POCR0SB _SFR_MEM16(0xA4)

#define POCR0SBL _SFR_MEM8(0xA4)
#define POCR0SB_0 0
#define POCR0SB_1 1
#define POCR0SB_2 2
#define POCR0SB_3 3
#define POCR0SB_4 4
#define POCR0SB_5 5
#define POCR0SB_6 6
#define POCR0SB_7 7

#define POCR0SBH _SFR_MEM8(0xA5)
#define POCR0SB_8 0
#define POCR0SB_9 1
#define POCR0SB_00 2
#define POCR0SB_01 3

#define POCR1SA _SFR_MEM16(0xA6)

#define POCR1SAL _SFR_MEM8(0xA6)
#define POCR1SA_0 0
#define POCR1SA_1 1
#define POCR1SA_2 2
#define POCR1SA_3 3
#define POCR1SA_4 4
#define POCR1SA_5 5
#define POCR1SA_6 6
#define POCR1SA_7 7

#define POCR1SAH _SFR_MEM8(0xA7)
#define POCR1SA_8 0
#define POCR1SA_9 1
#define POCR1SA_00 2
#define POCR1SA_01 3

#define POCR1RA _SFR_MEM16(0xA8)

#define POCR1RAL _SFR_MEM8(0xA8)
#define POCR1RA_0 0
#define POCR1RA_1 1
#define POCR1RA_2 2
#define POCR1RA_3 3
#define POCR1RA_4 4
#define POCR1RA_5 5
#define POCR1RA_6 6
#define POCR1RA_7 7

#define POCR1RAH _SFR_MEM8(0xA9)
#define POCR1RA_8 0
#define POCR1RA_9 1
#define POCR1RA_00 2
#define POCR1RA_01 4

#define POCR1SB _SFR_MEM16(0xAA)

#define POCR1SBL _SFR_MEM8(0xAA)
#define POCR1SB_0 0
#define POCR1SB_1 1
#define POCR1SB_2 2
#define POCR1SB_3 3
#define POCR1SB_4 4
#define POCR1SB_5 5
#define POCR1SB_6 6
#define POCR1SB_7 7

#define POCR1SBH _SFR_MEM8(0xAB)
#define POCR1SB_8 0
#define POCR1SB_9 1
#define POCR1SB_00 2
#define POCR1SB_01 3

#define POCR2SA _SFR_MEM16(0xAC)

#define POCR2SAL _SFR_MEM8(0xAC)
#define POCR2SA_0 0
#define POCR2SA_1 1
#define POCR2SA_2 2
#define POCR2SA_3 3
#define POCR2SA_4 4
#define POCR2SA_5 5
#define POCR2SA_6 6
#define POCR2SA_7 7

#define POCR2SAH _SFR_MEM8(0xAD)
#define POCR2SA_8 0
#define POCR2SA_9 1
#define POCR2SA_00 2
#define POCR2SA_01 3

#define POCR2RA _SFR_MEM16(0xAE)

#define POCR2RAL _SFR_MEM8(0xAE)
#define POCR2RA_0 0
#define POCR2RA_1 1
#define POCR2RA_2 2
#define POCR2RA_3 3
#define POCR2RA_4 4
#define POCR2RA_5 5
#define POCR2RA_6 6
#define POCR2RA_7 7

#define POCR2RAH _SFR_MEM8(0xAF)
#define POCR2RA_8 0
#define POCR2RA_9 1
#define POCR2RA_00 2
#define POCR2RA_01 3

#define POCR2SB _SFR_MEM16(0xB0)

#define POCR2SBL _SFR_MEM8(0xB0)
#define POCR2SB_0 0
#define POCR2SB_1 1
#define POCR2SB_2 2
#define POCR2SB_3 3
#define POCR2SB_4 4
#define POCR2SB_5 5
#define POCR2SB_6 6
#define POCR2SB_7 7

#define POCR2SBH _SFR_MEM8(0xB1)
#define POCR2SB_8 0
#define POCR2SB_9 1
#define POCR2SB_00 2
#define POCR2SB_01 3

#define POCRxRB _SFR_MEM16(0xB2)

#define POCRxRBL _SFR_MEM8(0xB2)
#define POCR_RB_0 0
#define POCR_RB_1 1
#define POCR_RB_2 2
#define POCR_RB_3 3
#define POCR_RB_4 4
#define POCR_RB_5 5
#define POCR_RB_6 6
#define POCR_RB_7 7

#define POCRxRBH _SFR_MEM8(0xB3)
#define POCR_RB_8 0
#define POCR_RB_9 1
#define POCR_RB_00 2
#define POCR_RB_01 3

#define PSYNC _SFR_MEM8(0xB4)
#define PSYNC00 0
#define PSYNC01 1
#define PSYNC10 2
#define PSYNC11 3
#define PSYNC20 4
#define PSYNC21 5

#define PCNF _SFR_MEM8(0xB5)
#define POPA 2
#define POPB 3
#define PMODE 4
#define PULOCK 5

#define POC _SFR_MEM8(0xB6)
#define POEN0A 0
#define POEN0B 1
#define POEN1A 2
#define POEN1B 3
#define POEN2A 4
#define POEN2B 5

#define PCTL _SFR_MEM8(0xB7)
#define PRUN 0
#define PCCYC 1
#define PCLKSEL 5
#define PPRE0 6
#define PPRE1 7

#define PMIC0 _SFR_MEM8(0xB8)
#define PRFM00 0
#define PRFM01 1
#define PRFM02 2
#define PAOC0 3
#define PFLTE0 4
#define PELEV0 5
#define PISEL0 6
#define POVEN0 7

#define PMIC1 _SFR_MEM8(0xB9)
#define PRFM10 0
#define PRFM11 1
#define PRFM12 2
#define PAOC1 3
#define PFLTE1 4
#define PELEV1 5
#define PISEL1 6
#define POVEN1 7

#define PMIC2 _SFR_MEM8(0xBA)
#define PRFM20 0
#define PRFM21 1
#define PRFM22 2
#define PAOC2 3
#define PFLTE2 4
#define PELEV2 5
#define PISEL2 6
#define POVEN2 7

#define PIM _SFR_MEM8(0xBB)
#define PEOPE 0
#define PEVE0 1
#define PEVE1 2
#define PEVE2 3

#define PIFR _SFR_MEM8(0xBC)
#define PEOP 0
#define PEV0 1
#define PEV1 2
#define PEV2 3

#define LINCR _SFR_MEM8(0xC8)
#define LCMD0 0
#define LCMD1 1
#define LCMD2 2
#define LENA 3
#define LCONF0 4
#define LCONF1 5
#define LIN13 6
#define LSWRES 7

#define LINSIR _SFR_MEM8(0xC9)
#define LRXOK 0
#define LTXOK 1
#define LIDOK 2
#define LERR 3
#define LBUSY 4
#define LIDST0 5
#define LIDST1 6
#define LIDST2 7

#define LINENIR _SFR_MEM8(0xCA)
#define LENRXOK 0
#define LENTXOK 1
#define LENIDOK 2
#define LENERR 3

#define LINERR _SFR_MEM8(0xCB)
#define LBERR 0
#define LCERR 1
#define LPERR 2
#define LSERR 3
#define LFERR 4
#define LOVERR 5
#define LTOERR 6
#define LABORT 7

#define LINBTR _SFR_MEM8(0xCC)
#define LBT0 0
#define LBT1 1
#define LBT2 2
#define LBT3 3
#define LBT4 4
#define LBT5 5
#define LDISR 7

#define LINBRR _SFR_MEM16(0xCD)

#define LINBRRL _SFR_MEM8(0xCD)
#define LDIV0 0
#define LDIV1 1
#define LDIV2 2
#define LDIV3 3
#define LDIV4 4
#define LDIV5 5
#define LDIV6 6
#define LDIV7 7

#define LINBRRH _SFR_MEM8(0xCE)
#define LDIV8 0
#define LDIV9 1
#define LDIV10 2
#define LDIV11 3

#define LINDLR _SFR_MEM8(0xCF)
#define LRXDL0 0
#define LRXDL1 1
#define LRXDL2 2
#define LRXDL3 3
#define LTXDL0 4
#define LTXDL1 5
#define LTXDL2 6
#define LTXDL3 7

#define LINIDR _SFR_MEM8(0xD0)
#define LID0 0
#define LID1 1
#define LID2 2
#define LID3 3
#define LID4 4
#define LID5 5
#define LP0 6
#define LP1 7

#define LINSEL _SFR_MEM8(0xD1)
#define LINDX0 0
#define LINDX1 1
#define LINDX2 2
#define LAINC 3

#define LINDAT _SFR_MEM8(0xD2)
#define LDATA0 0
#define LDATA1 1
#define LDATA2 2
#define LDATA3 3
#define LDATA4 4
#define LDATA5 5
#define LDATA6 6
#define LDATA7 7

#define CANGCON _SFR_MEM8(0xD8)
#define SWRES 0
#define ENASTB 1
#define TEST 2
#define LISTEN 3
#define SYNTTC 4
#define TTC 5
#define OVRQ 6
#define ABRQ 7

#define CANGSTA _SFR_MEM8(0xD9)
#define ERRP 0
#define BOFF 1
#define ENFG 2
#define RXBSY 3
#define TXBSY 4
#define OVFG 6

#define CANGIT _SFR_MEM8(0xDA)
#define AERG 0
#define FERG 1
#define CERG 2
#define SERG 3
#define BXOK 4
#define OVRTIM 5
#define BOFFIT 6
#define CANIT 7

#define CANGIE _SFR_MEM8(0xDB)
#define ENOVRT 0
#define ENERG 1
#define ENBX 2
#define ENERR 3
#define ENTX 4
#define ENRX 5
#define ENBOFF 6
#define ENIT 7

#define CANEN2 _SFR_MEM8(0xDC)
#define ENMOB0 0
#define ENMOB1 1
#define ENMOB2 2
#define ENMOB3 3
#define ENMOB4 4
#define ENMOB5 5

#define CANEN1 _SFR_MEM8(0xDD)

#define CANIE2 _SFR_MEM8(0xDE)
#define IEMOB0 0
#define IEMOB1 1
#define IEMOB2 2
#define IEMOB3 3
#define IEMOB4 4
#define IEMOB5 5

#define CANIE1 _SFR_MEM8(0xDF)

#define CANSIT2 _SFR_MEM8(0xE0)
#define SIT0 0
#define SIT1 1
#define SIT2 2
#define SIT3 3
#define SIT4 4
#define SIT5 5

#define CANSIT1 _SFR_MEM8(0xE1)

#define CANBT1 _SFR_MEM8(0xE2)
#define BRP0 1
#define BRP1 2
#define BRP2 3
#define BRP3 4
#define BRP4 5
#define BRP5 6

#define CANBT2 _SFR_MEM8(0xE3)
#define PRS0 1
#define PRS1 2
#define PRS2 3
#define SJW0 5
#define SJW1 6

#define CANBT3 _SFR_MEM8(0xE4)
#define SMP 0
#define PHS10 1
#define PHS11 2
#define PHS12 3
#define PHS20 4
#define PHS21 5
#define PHS22 6

#define CANTCON _SFR_MEM8(0xE5)

#define CANTIML _SFR_MEM8(0xE6)

#define CANTIMH _SFR_MEM8(0xE7)

#define CANTTCL _SFR_MEM8(0xE8)

#define CANTTCH _SFR_MEM8(0xE9)

#define CANTEC _SFR_MEM8(0xEA)

#define CANREC _SFR_MEM8(0xEB)

#define CANHPMOB _SFR_MEM8(0xEC)
#define CGP0 0
#define CGP1 1
#define CGP2 2
#define CGP3 3
#define HPMOB0 4
#define HPMOB1 5
#define HPMOB2 6
#define HPMOB3 7

#define CANPAGE _SFR_MEM8(0xED)
#define INDX0 0
#define INDX1 1
#define INDX2 2
#define AINC 3
#define MOBNB0 4
#define MOBNB1 5
#define MOBNB2 6
#define MOBNB3 7

#define CANSTMOB _SFR_MEM8(0xEE)
#define AERR 0
#define FERR 1
#define CERR 2
#define SERR 3
#define BERR 4
#define RXOK 5
#define TXOK 6
#define DLCW 7

#define CANCDMOB _SFR_MEM8(0xEF)
#define DLC0 0
#define DLC1 1
#define DLC2 2
#define DLC3 3
#define IDE 4 
#define RPLV 5
#define CONMOB0 6
#define CONMOB1 7

#define CANIDT4 _SFR_MEM8(0xF0)
#define RB0TAG 0
#define RB1TAG 1
#define RTRTAG 2
#define IDT0 3
#define IDT1 4
#define IDT2 5
#define IDT3 6
#define IDT4 7

#define CANIDT3 _SFR_MEM8(0xF1)
#define IDT5 0
#define IDT6 1
#define IDT7 2
#define IDT8 3
#define IDT9 4
#define IDT10 5
#define IDT11 6
#define IDT12 7

#define CANIDT2 _SFR_MEM8(0xF2)
#define IDT13 0
#define IDT14 1
#define IDT15 2
#define IDT16 3
#define IDT17 4
#define IDT18 5
#define IDT19 6
#define IDT20 7

#define CANIDT1 _SFR_MEM8(0xF3)
#define IDT21 0
#define IDT22 1
#define IDT23 2
#define IDT24 3
#define IDT25 4
#define IDT26 5
#define IDT27 6
#define IDT28 7

#define CANIDM4 _SFR_MEM8(0xF4)
#define IDEMSK 0
#define RTRMSK 2
#define IDMSK0 3
#define IDMSK1 4
#define IDMSK2 5
#define IDMSK3 6
#define IDMSK4 7

#define CANIDM3 _SFR_MEM8(0xF5)
#define IDMSK5 0
#define IDMSK6 1
#define IDMSK7 2
#define IDMSK8 3
#define IDMSK9 4
#define IDMSK10 5
#define IDMSK11 6
#define IDMSK12 7

#define CANIDM2 _SFR_MEM8(0xF6)
#define IDMSK13 0
#define IDMSK14 1
#define IDMSK15 2
#define IDMSK16 3
#define IDMSK17 4
#define IDMSK18 5
#define IDMSK19 6
#define IDMSK20 7

#define CANIDM1 _SFR_MEM8(0xF7)
#define IDMSK21 0
#define IDMSK22 1
#define IDMSK23 2
#define IDMSK24 3
#define IDMSK25 4
#define IDMSK26 5
#define IDMSK27 6
#define IDMSK28 7

#define CANSTML _SFR_MEM8(0xF8)

#define CANSTMH _SFR_MEM8(0xF9)

#define CANMSG _SFR_MEM8(0xFA)



/* Interrupt Vectors */
/* Interrupt Vector 0 is the reset vector. */
#define ANACOMP0_vect       _VECTOR(1)  /* Analog Comparator 0 */
#define ANACOMP1_vect       _VECTOR(2)  /* Analog Comparator 1 */
#define ANACOMP2_vect       _VECTOR(3)  /* Analog Comparator 2 */
#define ANACOMP3_vect       _VECTOR(4)  /* Analog Comparator 3 */
#define PSC_FAULT_vect      _VECTOR(5)  /* PSC Fault */
#define PSC_EC_vect         _VECTOR(6)  /* PSC End of Cycle */
#define INT0_vect           _VECTOR(7)  /* External Interrupt Request 0 */
#define INT1_vect           _VECTOR(8)  /* External Interrupt Request 1 */
#define INT2_vect           _VECTOR(9)  /* External Interrupt Request 2 */
#define INT3_vect           _VECTOR(10)  /* External Interrupt Request 3 */
#define TIMER1_CAPT_vect    _VECTOR(11)  /* Timer/Counter1 Capture Event */
#define TIMER1_COMPA_vect   _VECTOR(12)  /* Timer/Counter1 Compare Match A */
#define TIMER1_COMPB_vect   _VECTOR(13)  /* Timer/Counter1 Compare Match B */
#define TIMER1_OVF_vect     _VECTOR(14)  /* Timer1/Counter1 Overflow */
#define TIMER0_COMPA_vect   _VECTOR(15)  /* Timer/Counter0 Compare Match A */
#define TIMER0_COMPB_vect   _VECTOR(16)  /* Timer/Counter0 Compare Match B */
#define TIMER0_OVF_vect     _VECTOR(17)  /* Timer/Counter0 Overflow */
#define CAN_INT_vect        _VECTOR(18)  /* CAN MOB, Burst, General Errors */
#define CAN_TOVF_vect       _VECTOR(19)  /* CAN Timer Overflow */
#define LIN_TC_vect         _VECTOR(20)  /* LIN Transfer Complete */
#define LIN_ERR_vect        _VECTOR(21)  /* LIN Error */
#define PCINT0_vect         _VECTOR(22)  /* Pin Change Interrupt Request 0 */
#define PCINT1_vect         _VECTOR(23)  /* Pin Change Interrupt Request 1 */
#define PCINT2_vect         _VECTOR(24)  /* Pin Change Interrupt Request 2 */
#define PCINT3_vect         _VECTOR(25)  /* Pin Change Interrupt Request 3 */
#define SPI_STC_vect        _VECTOR(26)  /* SPI Serial Transfer Complete */
#define ADC_vect            _VECTOR(27)  /* ADC Conversion Complete */
#define WDT_vect            _VECTOR(28)  /* Watchdog Time-Out Interrupt */
#define EE_READY_vect       _VECTOR(29)  /* EEPROM Ready */
#define SPM_READY_vect      _VECTOR(30)  /* Store Program Memory Read */

#define _VECTORS_SIZE (31 * 4)


/* Constants */
#define SPM_PAGESIZE (64) 
#define RAMSTART     (0x100)
#define RAMSIZE      (0x800)
#define RAMEND       (RAMSTART + RAMSIZE - 1)  /* Last On-Chip SRAM Location */
#define XRAMSIZE     (0)
#define XRAMEND      (RAMEND + XRAMSIZE)
#define E2END        (0x3FF)
#define E2PAGESIZE   (4)
#define FLASHEND     (0x7FFF)


/* Fuses */
#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0 (unsigned char)~_BV(0)  /* Select Clock Source */
#define FUSE_CKSEL1 (unsigned char)~_BV(1)  /* Select Clock Source */
#define FUSE_CKSEL2 (unsigned char)~_BV(2)  /* Select Clock Source */
#define FUSE_CKSEL3 (unsigned char)~_BV(3)  /* Select Clock Source */
#define FUSE_SUT0   (unsigned char)~_BV(4)  /* Select start-up time */
#define FUSE_SUT1   (unsigned char)~_BV(5)  /* Select start-up time */
#define FUSE_CKOUT  (unsigned char)~_BV(6)  /* Oscillator output option */
#define FUSE_CKDIV8 (unsigned char)~_BV(7)  /* Divide clock by 8 */
#define LFUSE_DEFAULT (FUSE_CKSEL1 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0 & FUSE_SUT1 & FUSE_CKDIV8)

/* High Fuse Byte */
#define FUSE_BOOTRST  (unsigned char)~_BV(0)  /* Select Reset Vector */
#define FUSE_BOOTSZ0  (unsigned char)~_BV(1)  /* Select Boot Size */
#define FUSE_BOOTSZ1  (unsigned char)~_BV(2)  /* Select Boot Size */
#define FUSE_EESAVE   (unsigned char)~_BV(3)  /* EEPROM memory is preserved through chip erase */
#define FUSE_WDTON    (unsigned char)~_BV(4)  /* Watchdog timer always on */
#define FUSE_SPIEN    (unsigned char)~_BV(5)  /* Enable Serial programming and Data Downloading */
#define FUSE_DWEN     (unsigned char)~_BV(6)  /* debugWIRE Enable */
#define FUSE_RSTDISBL (unsigned char)~_BV(7)  /* External Reset Disable */
#define HFUSE_DEFAULT (FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_SPIEN)

/* Extended Fuse Byte */
#define FUSE_BODLEVEL0 (unsigned char)~_BV(0)  /* Brown-out Detector Trigger Level */
#define FUSE_BODLEVEL1 (unsigned char)~_BV(1)  /* Brown-out Detector Trigger Level */
#define FUSE_BODLEVEL2 (unsigned char)~_BV(2)  /* Brown-out Detector Trigger Level */
#define FUSE_PSCRVB    (unsigned char)~_BV(3)  /* PSC Outputs xB Reset Value */
#define FUSE_PSCRVA    (unsigned char)~_BV(4)  /* PSC Outputs xA Reset Value */
#define FUSE_PSCRB     (unsigned char)~_BV(5)  /* PSC Reset Behavior */
#define EFUSE_DEFAULT (FUSE_BODLEVEL1 & FUSE_BODLEVEL2)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x95
#define SIGNATURE_2 0x84


#endif  /* _AVR_IOM32M1_H_ */
