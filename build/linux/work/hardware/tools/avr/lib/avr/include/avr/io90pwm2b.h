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

/* $Id: io90pwm2b.h,v 1.3.2.10 2008/10/17 23:27:45 arcanum Exp $ */

/* avr/io90pwm2b.h - definitions for AT90PWM2B */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "io90pwm2b.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_IO90PWM2B_H_
#define _AVR_IO90PWM2B_H_ 1

/* Registers and associated bit numbers */

#define PINB _SFR_IO8(0x03)
#define PINB0 0
#define PINB1 1
#define PINB2 2
#define PINB3 3
#define PINB4 4
#define PINB5 5
#define PINB6 6
#define PINB7 7

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

#define GPIOR3 _SFR_IO8(0x1B)
#define GPIOR30 0
#define GPIOR31 1
#define GPIOR32 2
#define GPIOR33 3
#define GPIOR34 4
#define GPIOR35 5
#define GPIOR36 6
#define GPIOR37 7

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
#define EEWE 1
#define EEMWE 2
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
#define EEARL0 0
#define EEARL1 1
#define EEARL2 2
#define EEARL3 3
#define EEARL4 4
#define EEARL5 5
#define EEARL6 6
#define EEARL7 7

#define EEARH _SFR_IO8(0x22)
#define EEAR8 0
#define EEAR9 1
#define EEAR10 2
#define EEAR11 3

#define GTCCR _SFR_IO8(0x23)
#define PSR10 0
#define PSRSYNC 0
#define ICPSEL1 2
#define TSM 3

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
#define OCR0_0 0
#define OCR0_1 1
#define OCR0_2 2
#define OCR0_3 3
#define OCR0_4 4
#define OCR0_5 5
#define OCR0_6 6
#define OCR0_7 7

#define OCR0B _SFR_IO8(0x28)
#define OCR0_0 0
#define OCR0_1 1
#define OCR0_2 2
#define OCR0_3 3
#define OCR0_4 4
#define OCR0_5 5
#define OCR0_6 6
#define OCR0_7 7

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
#define AC0IF 4
#define AC1IF 5
#define AC2IF 6
#define ACCKDIV 7

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
#define WDE3 3
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
#define PRUSART0 1
#define PRSPI 2
#define PRTIM0 3
#define PRTIM1 4
#define PRPSC0 5
#define PRPSC1 6
#define PRPSC2 7

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

#define TIMSK0 _SFR_MEM8(0x6E)
#define TOIE0 0
#define OCIE0A 1
#define OCIE0B 2

#define TIMSK1 _SFR_MEM8(0x6F)
#define TOIE1 0
#define OCIE1A 1
#define OCIE1B 2
#define ICIE1 5

#define AMP0CSR _SFR_MEM8(0x76)
#define AMP0TS0 0
#define AMP0TS1 1
#define AMP0G0 4
#define AMP0G1 5
#define AMP0IS 6
#define AMP0EN 7

#define AMP1CSR _SFR_MEM8(0x77)
#define AMP1TS0 0
#define AMP1TS1 1
#define AMP1G0 4
#define AMP1G1 5
#define AMP1IS 6
#define AMP1EN 7

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
#define ADASCR 4
#define ADHSM 7

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

#define PIFR0 _SFR_MEM8(0xA0)
#define PEOP0 0
#define PRN00 1
#define PRN01 2
#define PEV0A 3
#define PEV0B 4
#define PSEI0 5
#define POAC0A 6
#define POAC0B 7

#define PIM0 _SFR_MEM8(0xA1)
#define PEOPE0 0
#define PEVE0A 3
#define PEVE0B 4
#define PSEIE0 5

#define PIFR1 _SFR_MEM8(0xA2)
#define PEOP1 0
#define PRN10 1
#define PRN11 2
#define PEV1A 3
#define PEV1B 4
#define PSEI1 5
#define POAC1A 6
#define POAC1B 7

#define PIM1 _SFR_MEM8(0xA3)
#define PEOPE1 0
#define PEVE1A 3
#define PEVE1B 4
#define PSEIE1 5

#define PIFR2 _SFR_MEM8(0xA4)
#define PEOP2 0
#define PRN20 1
#define PRN21 2
#define PEV2A 3
#define PEV2B 4
#define PSEI2 5
#define POAC2A 6
#define POAC2B 7

#define PIM2 _SFR_MEM8(0xA5)
#define PEOPE2 0
#define PEVE2A 3
#define PEVE2B 4
#define PSEIE2 5

#define DACON _SFR_MEM8(0xAA)
#define DAEN 0
#define DAOE 1
#define DALA 2
#define DATS0 4
#define DATS1 5
#define DATS2 6
#define DAATE 7

#define DAC _SFR_MEM16(0xAB)

#define DACL _SFR_MEM8(0xAB)
#define DACL0 0
#define DACL1 1
#define DACL2 2
#define DACL3 3
#define DACL4 4
#define DACL5 5
#define DACL6 6
#define DACL7 7

#define DACH _SFR_MEM8(0xAC)
#define DACH0 0
#define DACH1 1
#define DACH2 2
#define DACH3 3
#define DACH4 4
#define DACH5 5
#define DACH6 6
#define DACH7 7

#define AC0CON _SFR_MEM8(0xAD)
#define AC0M0 0
#define AC0M1 1
#define AC0M2 2
#define AC0IS0 4
#define AC0IS1 5
#define AC0IE 6
#define AC0EN 7

#define AC1CON _SFR_MEM8(0xAE)
#define AC1M0 0
#define AC1M1 1
#define AC1M2 2
#define AC1ICE 3
#define AC1IS0 4
#define AC1IS1 5
#define AC1IE 6
#define AC1EN 7

#define AC2CON _SFR_MEM8(0xAF)
#define AC2M0 0
#define AC2M1 1
#define AC2M2 2
#define AC2IS0 4
#define AC2IS1 5
#define AC2IE 6
#define AC2EN 7

#define UCSRA _SFR_MEM8(0xC0)
#define MPCM 0
#define U2X 1
#define UPE 2
#define DOR 3
#define FE 4
#define UDRE 5
#define TXC 6
#define RXC 7

#define UCSRB _SFR_MEM8(0xC1)
#define TXB8 0
#define RXB8 1
#define UCSZ2 2
#define TXEN 3
#define RXEN 4
#define UDRIE 5
#define TXCIE 6
#define RXCIE 7

#define UCSRC _SFR_MEM8(0xC2)
#define UCPOL 0
#define UCSZ0 1
#define UCSZ1 2
#define USBS 3
#define UPM0 4
#define UPM1 5
#define UMSEL0 6

#define UBRR _SFR_MEM16(0xC4)

#define UBRRL _SFR_MEM8(0xC4)
#define UBRR0 0
#define UBRR1 1
#define UBRR2 2
#define UBRR3 3
#define UBRR4 4
#define UBRR5 5
#define UBRR6 6
#define UBRR7 7

#define UBRRH _SFR_MEM8(0xC5)
#define UBRR8 0
#define UBRR9 1
#define UBRR10 2
#define UBRR11 3

#define UDR _SFR_MEM8(0xC6)
#define UDR0 0
#define UDR1 1
#define UDR2 2
#define UDR3 3
#define UDR4 4
#define UDR5 5
#define UDR6 6
#define UDR7 7

#define EUCSRA _SFR_MEM8(0xC8)
#define URxS0 0
#define URxS1 1
#define URxS2 2
#define URxS3 3
#define UTxS0 4
#define UTxS1 5
#define UTxS2 6
#define UTxS3 7

#define EUCSRB _SFR_MEM8(0xC9)
#define BODR 0
#define EMCH 1
#define EUSBS 3
#define EUSART 4

#define EUCSRC _SFR_MEM8(0xCA)
#define STP0 0
#define STP1 1
#define F1617 2
#define FEM 3 

#define MUBRR _SFR_MEM16(0xCC)

#define MUBRRL _SFR_MEM8(0xCC)
#define MUBRR0 0
#define MUBRR1 1
#define MUBRR2 2
#define MUBRR3 3
#define MUBRR4 4
#define MUBRR5 5
#define MUBRR6 6
#define MUBRR7 7

#define MUBRRH _SFR_MEM8(0xCD)
#define MUBRR8 0
#define MUBRR9 1
#define MUBRR10 2
#define MUBRR11 3
#define MUBRR12 4
#define MUBRR13 5
#define MUBRR14 6
#define MUBRR15 7

#define EUDR _SFR_MEM8(0xCE)
#define EUDR0 0
#define EUDR1 1
#define EUDR2 2
#define EUDR3 3
#define EUDR4 4
#define EUDR5 5
#define EUDR6 6
#define EUDR7 7

#define PSOC0 _SFR_MEM8(0xD0)
#define POEN0A 0
#define POEN0B 2
#define PSYNC00 4
#define PSYNC01 5

#define OCR0SA _SFR_MEM16(0xD2)

#define OCR0SAL _SFR_MEM8(0xD2)
#define OCR0SA_0 0
#define OCR0SA_1 1
#define OCR0SA_2 2
#define OCR0SA_3 3
#define OCR0SA_4 4
#define OCR0SA_5 5
#define OCR0SA_6 6
#define OCR0SA_7 7

#define OCR0SAH _SFR_MEM8(0xD3)
#define OCR0SA_8 0
#define OCR0SA_9 1
#define OCR0SA_00 2
#define OCR0SA_01 3

#define OCR0RA _SFR_MEM16(0xD4)

#define OCR0RAL _SFR_MEM8(0xD4)
#define OCR0RA_0 0
#define OCR0RA_1 1
#define OCR0RA_2 2
#define OCR0RA_3 3
#define OCR0RA_4 4
#define OCR0RA_5 5
#define OCR0RA_6 6
#define OCR0RA_7 7

#define OCR0RAH _SFR_MEM8(0xD5)
#define OCR0RA_8 0
#define OCR0RA_9 1
#define OCR0RA_00 2
#define OCR0RA_01 3

#define OCR0SB _SFR_MEM16(0xD6)

#define OCR0SBL _SFR_MEM8(0xD6)
#define OCR0SB_0 0
#define OCR0SB_1 1
#define OCR0SB_2 2
#define OCR0SB_3 3
#define OCR0SB_4 4
#define OCR0SB_5 5
#define OCR0SB_6 6
#define OCR0SB_7 7

#define OCR0SBH _SFR_MEM8(0xD7)
#define OCR0SB_8 0
#define OCR0SB_9 1
#define OCR0SB_00 2
#define OCR0SB_01 3

#define OCR0RB _SFR_MEM16(0xD8)

#define OCR0RBL _SFR_MEM8(0xD8)
#define OCR0RB_0 0
#define OCR0RB_1 1
#define OCR0RB_2 2
#define OCR0RB_3 3
#define OCR0RB_4 4
#define OCR0RB_5 5
#define OCR0RB_6 6
#define OCR0RB_7 7

#define OCR0RBH _SFR_MEM8(0xD9)
#define OCR0RB_8 0
#define OCR0RB_9 1
#define OCR0RB_00 2
#define OCR0RB_01 3
#define OCR0RB_02 4
#define OCR0RB_03 5
#define OCR0RB_04 6
#define OCR0RB_05 7

#define PCNF0 _SFR_MEM8(0xDA)
#define PCLKSEL0 1
#define POP0 2
#define PMODE00 3
#define PMODE01 4
#define PLOCK0 5
#define PALOCK0 6
#define PFIFTY0 7

#define PCTL0 _SFR_MEM8(0xDB)
#define PRUN0 0
#define PCCYC0 1
#define PARUN0 2
#define PAOC0A 3
#define PAOC0B 4
#define PBFM0 5
#define PPRE00 6
#define PPRE01 7

#define PFRC0A _SFR_MEM8(0xDC)
#define PRFM0A0 0
#define PRFM0A1 1
#define PRFM0A2 2
#define PRFM0A3 3
#define PFLTE0A 4
#define PELEV0A 5
#define PISEL0A 6
#define PCAE0A 7

#define PFRC0B _SFR_MEM8(0xDD)
#define PRFM0B0 0
#define PRFM0B1 1
#define PRFM0B2 2
#define PRFM0B3 3
#define PFLTE0B 4
#define PELEV0B 5
#define PISEL0B 6
#define PCAE0B 7

#define PICR0 _SFR_MEM16(0xDE)

#define PICR0L _SFR_MEM8(0xDE)
#define PICR0_0 0
#define PICR0_1 1
#define PICR0_2 2
#define PICR0_3 3
#define PICR0_4 4
#define PICR0_5 5
#define PICR0_6 6
#define PICR0_7 7

#define PICR0H _SFR_MEM8(0xDF)
#define PICR0_8 0
#define PICR0_9 1
#define PICR0_10 2
#define PICR0_11 3
#define PCST0 7

#define PSOC1 _SFR_MEM8(0xE0)
#define POEN1A 0
#define POEN1B 2
#define PSYNC1_0 4
#define PSYNC1_1 5

#define OCR1SA _SFR_MEM16(0xE2)

#define OCR1SAL _SFR_MEM8(0xE2)
#define OCR1SA_0 0
#define OCR1SA_1 1
#define OCR1SA_2 2
#define OCR1SA_3 3
#define OCR1SA_4 4
#define OCR1SA_5 5
#define OCR1SA_6 6
#define OCR1SA_7 7

#define OCR1SAH _SFR_MEM8(0xE3)
#define OCR1SA_8 0
#define OCR1SA_9 1
#define OCR1SA_10 2
#define OCR1SA_11 3

#define OCR1RA _SFR_MEM16(0xE4)

#define OCR1RAL _SFR_MEM8(0xE4)
#define OCR1RA_0 0
#define OCR1RA_1 1
#define OCR1RA_2 2
#define OCR1RA_3 3
#define OCR1RA_4 4
#define OCR1RA_5 5
#define OCR1RA_6 6
#define OCR1RA_7 7

#define OCR1RAH _SFR_MEM8(0xE5)
#define OCR1RA_8 0
#define OCR1RA_9 1
#define OCR1RA_10 2
#define OCR1RA_11 3

#define OCR1SB _SFR_MEM16(0xE6)

#define OCR1SBL _SFR_MEM8(0xE6)
#define OCR1SB_0 0
#define OCR1SB_1 1
#define OCR1SB_2 2
#define OCR1SB_3 3
#define OCR1SB_4 4
#define OCR1SB_5 5
#define OCR1SB_6 6
#define OCR1SB_7 7

#define OCR1SBH _SFR_MEM8(0xE7)
#define OCR1SB_8 0
#define OCR1SB_9 1
#define OCR1SB_10 2
#define OCR1SB_11 3

#define OCR1RB _SFR_MEM16(0xE8)

#define OCR1RBL _SFR_MEM8(0xE8)
#define OCR1RB_0 0
#define OCR1RB_1 1
#define OCR1RB_2 2
#define OCR1RB_3 3
#define OCR1RB_4 4
#define OCR1RB_5 5
#define OCR1RB_6 6
#define OCR1RB_7 7

#define OCR1RBH _SFR_MEM8(0xE9)
#define OCR1RB_8 0
#define OCR1RB_9 1
#define OCR1RB_10 2
#define OCR1RB_11 3
#define OCR1RB_12 4
#define OCR1RB_13 5
#define OCR1RB_14 6
#define OCR1RB_15 7

#define PCNF1 _SFR_MEM8(0xEA)
#define PCLKSEL1 1
#define POP1 2
#define PMODE10 3
#define PMODE11 4
#define PLOCK1 5
#define PALOCK1 6
#define PFIFTY1 7

#define PCTL1 _SFR_MEM8(0xEB)
#define PRUN1 0
#define PCCYC1 1
#define PARUN1 2
#define PAOC1A 3
#define PAOC1B 4
#define PBFM1 5
#define PPRE10 6
#define PPRE11 7

#define PFRC1A _SFR_MEM8(0xEC)
#define PRFM1A0 0
#define PRFM1A1 1
#define PRFM1A2 2
#define PRFM1A3 3
#define PFLTE1A 4
#define PELEV1A 5
#define PISEL1A 6
#define PCAE1A 7

#define PFRC1B _SFR_MEM8(0xED)
#define PRFM1B0 0
#define PRFM1B1 1
#define PRFM1B2 2
#define PRFM1B3 3
#define PFLTE1B 4
#define PELEV1B 5
#define PISEL1B 6
#define PCAE1B 7

#define PICR1 _SFR_MEM16(0xEE)

#define PICR1L _SFR_MEM8(0xEE)
#define PICR1_0 0
#define PICR1_1 1
#define PICR1_2 2
#define PICR1_3 3
#define PICR1_4 4
#define PICR1_5 5
#define PICR1_6 6
#define PICR1_7 7

#define PICR1H _SFR_MEM8(0xEF)
#define PICR1_8 0
#define PICR1_9 1
#define PICR1_10 2
#define PICR1_11 3
#define PCST1 7

#define PSOC2 _SFR_MEM8(0xF0)
#define POEN2A 0
#define POEN2C 1
#define POEN2B 2
#define POEN2D 3
#define PSYNC2_0 4
#define PSYNC2_1 5
#define POS22 6
#define POS23 7

#define POM2 _SFR_MEM8(0xF1)
#define POMV2A0 0
#define POMV2A1 1
#define POMV2A2 2
#define POMV2A3 3
#define POMV2B0 4
#define POMV2B1 5
#define POMV2B2 6
#define POMV2B3 7

#define OCR2SA _SFR_MEM16(0xF2)

#define OCR2SAL _SFR_MEM8(0xF2)
#define OCR2SA_0 0
#define OCR2SA_1 1
#define OCR2SA_2 2
#define OCR2SA_3 3
#define OCR2SA_4 4
#define OCR2SA_5 5
#define OCR2SA_6 6
#define OCR2SA_7 7

#define OCR2SAH _SFR_MEM8(0xF3)
#define OCR2SA_8 0
#define OCR2SA_9 1
#define OCR2SA_10 2
#define OCR2SA_11 3

#define OCR2RA _SFR_MEM16(0xF4)

#define OCR2RAL _SFR_MEM8(0xF4)
#define OCR2RA_0 0
#define OCR2RA_1 1
#define OCR2RA_2 2
#define OCR2RA_3 3
#define OCR2RA_4 4
#define OCR2RA_5 5
#define OCR2RA_6 6
#define OCR2RA_7 7

#define OCR2RAH _SFR_MEM8(0xF5)
#define OCR2RA_8 0
#define OCR2RA_9 1
#define OCR2RA_10 2
#define OCR2RA_11 3

#define OCR2SB _SFR_MEM16(0xF6)

#define OCR2SBL _SFR_MEM8(0xF6)
#define OCR2SB_0 0
#define OCR2SB_1 1
#define OCR2SB_2 2
#define OCR2SB_3 3
#define OCR2SB_4 4
#define OCR2SB_5 5
#define OCR2SB_6 6
#define OCR2SB_7 7

#define OCR2SBH _SFR_MEM8(0xF7)
#define OCR2SB_8 0
#define OCR2SB_9 1
#define OCR2SB_10 2
#define OCR2SB_11 3

#define OCR2RB _SFR_MEM16(0xF8)

#define OCR2RBL _SFR_MEM8(0xF8)
#define OCR2RB_0 0
#define OCR2RB_1 1
#define OCR2RB_2 2
#define OCR2RB_3 3
#define OCR2RB_4 4
#define OCR2RB_5 5
#define OCR2RB_6 6
#define OCR2RB_7 7

#define OCR2RBH _SFR_MEM8(0xF9)
#define OCR2RB_8 0
#define OCR2RB_9 1
#define OCR2RB_10 2
#define OCR2RB_11 3
#define OCR2RB_12 4
#define OCR2RB_13 5
#define OCR2RB_14 6
#define OCR2RB_15 7

#define PCNF2 _SFR_MEM8(0xFA)
#define POME2 0
#define PCLKSEL2 1
#define POP2 2
#define PMODE20 3
#define PMODE21 4
#define PLOCK2 5
#define PALOCK2 6
#define PFIFTY2 7

#define PCTL2 _SFR_MEM8(0xFB)
#define PRUN2 0
#define PCCYC2 1
#define PARUN2 2
#define PAOC2A 3
#define PAOC2B 4
#define PBFM2 5
#define PPRE20 6
#define PPRE21 7

#define PFRC2A _SFR_MEM8(0xFC)
#define PRFM2A0 0
#define PRFM2A1 1
#define PRFM2A2 2
#define PRFM2A3 3
#define PFLTE2A 4
#define PELEV2A 5
#define PISEL2A 6
#define PCAE2A 7

#define PFRC2B _SFR_MEM8(0xFD)
#define PRFM2B0 0
#define PRFM2B1 1
#define PRFM2B2 2
#define PRFM2B3 3
#define PFLTE2B 4
#define PELEV2B 5
#define PISEL2B 6
#define PCAE2B 7

#define PICR2 _SFR_MEM16(0xFE)

#define PICR2L _SFR_MEM8(0xFE)
#define PICR2_0 0
#define PICR2_1 1
#define PICR2_2 2
#define PICR2_3 3
#define PICR2_4 4
#define PICR2_5 5
#define PICR2_6 6
#define PICR2_7 7

#define PICR2H _SFR_MEM8(0xFF)
#define PICR2_8 0
#define PICR2_9 1
#define PICR2_10 2
#define PICR2_11 3
#define PCST2 7



/* Interrupt Vectors */
/* Interrupt vector 0 is the reset vector. */
#define PSC2_CAPT_vect      _VECTOR(1)   /* PSC2 Capture Event */
#define PSC2_EC_vect        _VECTOR(2)   /* PSC2 End Cycle */
#define PSC1_CAPT_vect      _VECTOR(3)   /* PSC1 Capture Event */
#define PSC1_EC_vect        _VECTOR(4)   /* PSC1 End Cycle */
#define PSC0_CAPT_vect      _VECTOR(5)   /* PSC0 Capture Event */
#define PSC0_EC_vect        _VECTOR(6)   /* PSC0 End Cycle */
#define ANALOG_COMP_0_vect  _VECTOR(7)   /* Analog Comparator 0 */
#define ANALOG_COMP_1_vect  _VECTOR(8)   /* Analog Comparator 1 */
#define ANALOG_COMP_2_vect  _VECTOR(9)   /* Analog Comparator 2 */
#define INT0_vect           _VECTOR(10)  /* External Interrupt Request 0 */
#define TIMER1_CAPT_vect    _VECTOR(11)  /* Timer/Counter1 Capture Event */
#define TIMER1_COMPA_vect   _VECTOR(12)  /* Timer/Counter1 Compare Match A */
#define TIMER1_COMPB_vect   _VECTOR(13)  /* Timer/Counter Compare Match B */
/* Vector 14, Reserved */
#define TIMER1_OVF_vect     _VECTOR(15)  /* Timer/Counter1 Overflow */
#define TIMER0_COMPA_vect   _VECTOR(16)  /* Timer/Counter0 Compare Match A */
#define TIMER0_OVF_vect     _VECTOR(17)  /* Timer/Counter0 Overflow */
#define ADC_vect            _VECTOR(18)  /* ADC Conversion Complete */
#define INT1_vect           _VECTOR(19)  /* External Interrupt Request 1 */
#define SPI_STC_vect        _VECTOR(20)  /* SPI Serial Transfer Complete */
#define USART_RX_vect       _VECTOR(21)  /* USART, Rx Complete */
#define USART_UDRE_vect     _VECTOR(22)  /* USART Data Register Empty */
#define USART_TX_vect       _VECTOR(23)  /* USART, Tx Complete */
#define INT2_vect           _VECTOR(24)  /* External Interrupt Request 2 */
#define WDT_vect            _VECTOR(25)  /* Watchdog Timeout Interrupt */
#define EE_READY_vect       _VECTOR(26)  /* EEPROM Ready */
#define TIMER0_COMPB_vect   _VECTOR(27)  /* Timer Counter 0 Compare Match B */
#define INT3_vect           _VECTOR(28)  /* External Interrupt Request 3 */
/* Vector 29, Reserved */
/* Vector 30, Reserved */
#define SPM_READY_vect      _VECTOR(31)  /* Store Program Memory Read */

#define _VECTORS_SIZE 64



/* Memory Sizes */
#define RAMEND         0x2FF
#define XRAMSIZE       0
#define XRAMEND        (RAMEND + XRAMSIZE)
#define E2END          0x1FF
#define E2PAGESIZE     4
#define FLASHEND       0x1FFF
#define SPM_PAGESIZE   32



/* Fuse Information */

#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0  (unsigned char)~_BV(0)  /* Select Clock Source */
#define FUSE_CKSEL1  (unsigned char)~_BV(1)  /* Select Clock Source */
#define FUSE_CKSEL2  (unsigned char)~_BV(2)  /* Select Clock Source */
#define FUSE_CKSEL3  (unsigned char)~_BV(3)  /* Select Clock Source */
#define FUSE_SUT0    (unsigned char)~_BV(4)  /* Select start-up time */
#define FUSE_SUT1    (unsigned char)~_BV(5)  /* Select start-up time */
#define FUSE_CKOUT   (unsigned char)~_BV(6) /* Oscillator output option */
#define FUSE_CKDIV8  (unsigned char)~_BV(7)  /* Divide clock by 8 */
#define LFUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0 & FUSE_CKDIV8)


/* High Fuse Byte */
#define FUSE_BODLEVEL0 (unsigned char)~_BV(0)  /* Brown-out Detector trigger level */
#define FUSE_BODLEVEL1 (unsigned char)~_BV(1)  /* Brown-out Detector trigger level */
#define FUSE_BODLEVEL2 (unsigned char)~_BV(2)  /* Brown out detector trigger level */
#define FUSE_EESAVE    (unsigned char)~_BV(3)  /* EEPROM memory is preserved through chip erase */
#define FUSE_WDTON     (unsigned char)~_BV(4)  /* Watchdog timer always on */
#define FUSE_SPIEN     (unsigned char)~_BV(5)  /* Enable Serial programming and Data Downloading */
#define FUSE_DWEN      (unsigned char)~_BV(6)  /* debugWIRE Enable */
#define FUSE_RSTDISBL  (unsigned char)~_BV(7)  /* External Reset Disable */
#define HFUSE_DEFAULT (FUSE_SPIEN)    


/* Extended Fuse Byte */
#define FUSE_BOOTRST (unsigned char)~_BV(0)  /* Select Reset Vector */
#define FUSE_BOOTSZ0 (unsigned char)~_BV(1)  /* Select Boot Size */
#define FUSE_BOOTSZ1 (unsigned char)~_BV(2)  /* Select Boot Size */
#define FUSE_PSCRV   (unsigned char)~_BV(4)  /* PSCOUT Reset Value */
#define FUSE_PSC0RB  (unsigned char)~_BV(5)  /* PSC0 Reset Behaviour */
#define FUSE_PSC1RB  (unsigned char)~_BV(6)  /* PSC1 Reset Behaviour */
#define FUSE_PSC2RB  (unsigned char)~_BV(7)  /* PSC2 Reset Behaviour */
#define EFUSE_DEFAULT (FUSE_BOOTSZ0 & FUSE_BOOTSZ1)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x93
#define SIGNATURE_2 0x83


#endif /* _AVR_IO90PWM2B_H_ */
