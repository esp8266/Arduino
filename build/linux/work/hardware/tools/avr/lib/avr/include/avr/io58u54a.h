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

/* $Id$ */

/* avr/io58u54a.h - definitions for 58U54A_Albatross */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "io58u54a.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_58U54A_Albatross_H_
#define _AVR_58U54A_Albatross_H_ 1


/* Registers and associated bit numbers. */

#define PINA _SFR_IO8(0x000)
#define PINA0 0
#define PINA1 1
#define PINA2 2
#define PINA3 3
#define PINA4 4
#define PINA5 5
#define PINA6 6
#define PINA7 7

#define DDRA _SFR_IO8(0x001)
#define DDA0 0
#define DDA1 1
#define DDA2 2
#define DDA3 3
#define DDA4 4
#define DDA5 5
#define DDA6 6
#define DDA7 7

#define PORTA _SFR_IO8(0x002)
#define PORTA0 0
#define PORTA1 1
#define PORTA2 2
#define PORTA3 3
#define PORTA4 4
#define PORTA5 5
#define PORTA6 6
#define PORTA7 7

#define PINB _SFR_IO8(0x003)
#define PINB0 0
#define PINB1 1
#define PINB2 2
#define PINB3 3
#define PINB4 4
#define PINB5 5
#define PINB6 6
#define PINB7 7

#define DDRB _SFR_IO8(0x004)
#define DDB0 0
#define DDB1 1
#define DDB2 2
#define DDB3 3
#define DDB4 4
#define DDB5 5
#define DDB6 6
#define DDB7 7

#define PORTB _SFR_IO8(0x005)
#define PORTB0 0
#define PORTB1 1
#define PORTB2 2
#define PORTB3 3
#define PORTB4 4
#define PORTB5 5
#define PORTB6 6
#define PORTB7 7

#define PINC _SFR_IO8(0x006)
#define PINC0 0
#define PINC1 1
#define PINC2 2
#define PINC3 3
#define PINC4 4
#define PINC5 5
#define PINC6 6

#define DDRC _SFR_IO8(0x007)
#define DDC0 0
#define DDC1 1
#define DDC2 2
#define DDC3 3
#define DDC4 4
#define DDC5 5
#define DDC6 6

#define PORTC _SFR_IO8(0x008)
#define PORTC0 0
#define PORTC1 1
#define PORTC2 2
#define PORTC3 3
#define PORTC4 4
#define PORTC5 5
#define PORTC6 6

#define PIND _SFR_IO8(0x009)
#define PIND0 0
#define PIND1 1
#define PIND2 2
#define PIND3 3
#define PIND4 4
#define PIND5 5
#define PIND6 6
#define PIND7 7

#define DDRD _SFR_IO8(0x00A)
#define DDD0 0
#define DDD1 1
#define DDD2 2
#define DDD3 3
#define DDD4 4
#define DDD5 5
#define DDD6 6
#define DDD7 7

#define PORTD _SFR_IO8(0x00B)
#define PORTD0 0
#define PORTD1 1
#define PORTD2 2
#define PORTD3 3
#define PORTD4 4
#define PORTD5 5
#define PORTD6 6
#define PORTD7 7

#define PINE _SFR_IO8(0x00C)
#define PINE0 0
#define PINE1 1
#define PINE2 2
#define PINE3 3
#define PINE4 4
#define PINE5 5
#define PINE6 6
#define PINE7 7

#define DDRE _SFR_IO8(0x00D)
#define DDE0 0
#define DDE1 1
#define DDE2 2
#define DDE3 3
#define DDE4 4
#define DDE5 5
#define DDE6 6
#define DDE7 7

#define PORTE _SFR_IO8(0x00E)
#define PORTE0 0
#define PORTE1 1
#define PORTE2 2
#define PORTE3 3
#define PORTE4 4
#define PORTE5 5
#define PORTE6 6
#define PORTE7 7

#define PINF _SFR_IO8(0x00F)
#define PINF0 0
#define PINF1 1
#define PINF2 2
#define PINF3 3
#define PINF4 4
#define PINF5 5
#define PINF6 6
#define PINF7 7

#define DDRF _SFR_IO8(0x010)
#define DDF0 0
#define DDF1 1
#define DDF2 2
#define DDF3 3
#define DDF4 4
#define DDF5 5
#define DDF6 6
#define DDF7 7

#define PORTF _SFR_IO8(0x011)
#define PORTF0 0
#define PORTF1 1
#define PORTF2 2
#define PORTF3 3
#define PORTF4 4
#define PORTF5 5
#define PORTF6 6
#define PORTF7 7

#define TIFR0 _SFR_IO8(0x015)
#define TOV0 0
#define OCF0A 1
#define OCF0B 2

#define TIFR1 _SFR_IO8(0x016)
#define TOV1 0
#define OCF1A 1
#define OCF1B 2
#define ICF1 5

#define TIFR2 _SFR_IO8(0x017)
#define TOV2 0
#define OCF2A 1
#define OCF2B 2

#define TIFR3 _SFR_IO8(0x018)
#define TOV3 0
#define OCF3A 1
#define OCF3B 2
#define ICF3 5

#define TIFR4 _SFR_IO8(0x019)
#define TOV4 0
#define OCF4A 1
#define OCF4B 2

#define PCIFR _SFR_IO8(0x01B)
#define PCIF0 0
#define PCIF1 1
#define PCIF2 2
#define PCIF3 3
#define PCIF4 4
#define PCIF5 5

#define GPIOR0 _SFR_IO8(0x01E)
#define GPIOR00 0
#define GPIOR01 1
#define GPIOR02 2
#define GPIOR03 3
#define GPIOR04 4
#define GPIOR05 5
#define GPIOR06 6
#define GPIOR07 7

#define GTCCR _SFR_IO8(0x023)
#define PSRSYNC 0
#define TSM 7

#define TCCR0A _SFR_IO8(0x024)
#define WGM00 0
#define WGM01 1
#define COM0B0 4
#define COM0B1 5
#define COM0A0 6
#define COM0A1 7

#define TCCR0B _SFR_IO8(0x025)
#define CS00 0
#define CS01 1
#define CS02 2
#define WGM02 3
#define FOC0B 6
#define FOC0A 7

#define TCNT0 _SFR_IO8(0x026)
#define TCNT0_0 0
#define TCNT0_1 1
#define TCNT0_2 2
#define TCNT0_3 3
#define TCNT0_4 4
#define TCNT0_5 5
#define TCNT0_6 6
#define TCNT0_7 7

#define OCR0A _SFR_IO8(0x027)
#define OCROA_0 0
#define OCROA_1 1
#define OCROA_2 2
#define OCROA_3 3
#define OCROA_4 4
#define OCROA_5 5
#define OCROA_6 6
#define OCROA_7 7

#define OCR0B _SFR_IO8(0x028)
#define OCR0B_0 0
#define OCR0B_1 1
#define OCR0B_2 2
#define OCR0B_3 3
#define OCR0B_4 4
#define OCR0B_5 5
#define OCR0B_6 6
#define OCR0B_7 7

#define GPIOR1 _SFR_IO8(0x02A)
#define GPIOR10 0
#define GPIOR11 1
#define GPIOR12 2
#define GPIOR13 3
#define GPIOR14 4
#define GPIOR15 5
#define GPIOR16 6
#define GPIOR17 7

#define GPIOR2 _SFR_IO8(0x02B)
#define GPIOR20 0
#define GPIOR21 1
#define GPIOR22 2
#define GPIOR23 3
#define GPIOR24 4
#define GPIOR25 5
#define GPIOR26 6
#define GPIOR27 7

#define SPCR _SFR_IO8(0x02C)
#define SPR0 0
#define SPR1 1
#define CPHA 2
#define CPOL 3
#define MSTR 4
#define DORD 5
#define SPE 6
#define SPIE 7

#define SPSR _SFR_IO8(0x02D)
#define SPI2X 0
#define WCOL 6
#define SPIF 7

#define SPDR _SFR_IO8(0x02E)
#define SPDR0 0
#define SPDR1 1
#define SPDR2 2
#define SPDR3 3
#define SPDR4 4
#define SPDR5 5
#define SPDR6 6
#define SPDR7 7

#define OUTCLKR _SFR_IO8(0x02F)
#define CLK_O 0

#define ACSR _SFR_IO8(0x030)
#define SELANA 0

#define OCDR _SFR_IO8(0x031)
#define OCDR0 0
#define OCDR1 1
#define OCDR2 2
#define OCDR3 3
#define OCDR4 4
#define OCDR5 5
#define OCDR6 6
#define OCDR7 7

#define SMHIGH _SFR_IO8(0x032)
#define ON_SMHIGH 0
#define SMHIGH_IE 1
#define SMHIGH_IF 2

#define SMCR _SFR_IO8(0x033)
#define SE 0

#define MCUSR _SFR_IO8(0x034)
#define PORF 0
#define BORF 2
#define WDRF 3
#define JTRF 4

#define MCUCR _SFR_IO8(0x035)
#define IVCE 0
#define IVSEL 1
#define PUD 4
#define BODSE 5
#define JTD 7

#define POWER _SFR_IO8(0x036)
#define ONOFF 0
#define WKUPF 1
#define PORVBATF 2
#define WKUPSEL 3

#define SPMCSR _SFR_IO8(0x037)
#define SPMEN 0
#define PGERS 1
#define PGWRT 2
#define BLBSET 3
#define RWWSRE 4
#define RWWSB 6
#define SPMIE 7

#define RAMPZ _SFR_IO8(0x03B)
#define RAMPZ0 0

#define WDTCSR _SFR_MEM8(0x060)
#define WDP0 0
#define WDP1 1
#define WDP2 2
#define WDE 3
#define WDCE 4
#define WDP3 5
#define WDIE 6
#define WDIF 7

#define CLKPR _SFR_MEM8(0x061)
#define CLKPS0 0
#define CLKPS1 1
#define CLKPS2 2
#define CLKPS3 3
#define CLKPCE 7

#define PRR0 _SFR_MEM8(0x064)
#define PRUSART0 1
#define PRSPI 2
#define PRTIM1 3
#define PRLCD 4
#define PRTIM0 5
#define PRTIM2 6
#define PRTWI 7

#define PRR1 _SFR_MEM8(0x065)
#define PRTIM3 0
#define PRTIM4 1
#define PRSCI0 2
#define PRSCI1 3
#define PRSCI2 4
#define PRCL 5
#define PRDSRC 6

#define PCICR _SFR_MEM8(0x068)
#define PCIE0 0
#define PCIE1 1
#define PCIE2 2
#define PCIE3 3
#define PCIE4 4
#define PCIE5 5

#define PCMSK0 _SFR_MEM8(0x06B)
#define PCINT0 0
#define PCINT1 1
#define PCINT2 2
#define PCINT3 3
#define PCINT4 4
#define PCINT5 5
#define PCINT6 6
#define PCINT7 7

#define PCMSK1 _SFR_MEM8(0x06C)
#define PCINT8 0
#define PCINT9 1
#define PCINT10 2
#define PCINT11 3
#define PCINT12 4
#define PCINT13 5
#define PCINT14 6
#define PCINT15 7

#define PCMSK2 _SFR_MEM8(0x06D)
#define PCINT16 0
#define PCINT17 1
#define PCINT18 2
#define PCINT19 3
#define PCINT20 4

#define TIMSK0 _SFR_MEM8(0x06E)
#define TOIE0 0
#define OCIE0A 1
#define OCIE0B 2

#define TIMSK1 _SFR_MEM8(0x06F)
#define TOIE1 0
#define OCIE1A 1
#define OCIE1B 2
#define ICIE1 5

#define TIMSK2 _SFR_MEM8(0x070)
#define TOI20 0
#define OCIE2A 1
#define OCIE2B 2

#define TIMSK3 _SFR_MEM8(0x071)
#define TOIE3 0
#define OCIE3A 1
#define OCIE3B 2
#define ICIE3 5

#define TIMSK4 _SFR_MEM8(0x072)
#define TOIE4 0
#define OCIE4A 1
#define OCIE4B 2

#define PCMSK3 _SFR_MEM8(0x073)
#define PCINT24 0
#define PCINT25 1
#define PCINT26 2
#define PCINT27 3
#define PCINT28 4
#define PCINT29 5
#define PCINT30 6
#define PCINT31 7

#define PCMSK4 _SFR_MEM8(0x074)
#define PCINT32 0
#define PCINT33 1
#define PCINT34 2
#define PCINT35 3
#define PCINT36 4
#define PCINT37 5
#define PCINT38 6
#define PCINT39 7

#define PCMSK5 _SFR_MEM8(0x075)
#define PCINT40 0
#define PCINT41 1
#define PCINT42 2
#define PCINT43 3
#define PCINT44 4
#define PCINT45 5
#define PCINT46 6
#define PCINT47 7

#define TCCR1A _SFR_MEM8(0x080)
#define WGM10 0
#define WGM11 1
#define COM1B0 4
#define COM1B1 5
#define COM1A0 6
#define COM1A1 7

#define TCCR1B _SFR_MEM8(0x081)
#define CS10 0
#define CS11 1
#define CS12 2
#define WGM12 3
#define WGM13 4
#define ICES1 6
#define ICNC1 7

#define TCCR1C _SFR_MEM8(0x082)
#define FOC1B 6
#define FOC1A 7

#define TCNT1 _SFR_MEM16(0x084)

#define TCNT1L _SFR_MEM8(0x084)
#define TCNT1L0 0
#define TCNT1L1 1
#define TCNT1L2 2
#define TCNT1L3 3
#define TCNT1L4 4
#define TCNT1L5 5
#define TCNT1L6 6
#define TCNT1L7 7

#define TCNT1H _SFR_MEM8(0x085)
#define TCNT1H0 0
#define TCNT1H1 1
#define TCNT1H2 2
#define TCNT1H3 3
#define TCNT1H4 4
#define TCNT1H5 5
#define TCNT1H6 6
#define TCNT1H7 7

#define ICR1 _SFR_MEM16(0x086)

#define ICR1L _SFR_MEM8(0x086)
#define ICR1L0 0
#define ICR1L1 1
#define ICR1L2 2
#define ICR1L3 3
#define ICR1L4 4
#define ICR1L5 5
#define ICR1L6 6
#define ICR1L7 7

#define ICR1H _SFR_MEM8(0x087)
#define ICR1H0 0
#define ICR1H1 1
#define ICR1H2 2
#define ICR1H3 3
#define ICR1H4 4
#define ICR1H5 5
#define ICR1H6 6
#define ICR1H7 7

#define OCR1A _SFR_MEM16(0x088)

#define OCR1AL _SFR_MEM8(0x088)
#define OCR1AL0 0
#define OCR1AL1 1
#define OCR1AL2 2
#define OCR1AL3 3
#define OCR1AL4 4
#define OCR1AL5 5
#define OCR1AL6 6
#define OCR1AL7 7

#define OCR1AH _SFR_MEM8(0x089)
#define OCR1AH0 0
#define OCR1AH1 1
#define OCR1AH2 2
#define OCR1AH3 3
#define OCR1AH4 4
#define OCR1AH5 5
#define OCR1AH6 6
#define OCR1AH7 7

#define OCR1B _SFR_MEM16(0x08A)

#define OCR1BL _SFR_MEM8(0x08A)
#define OCR1BL0 0
#define OCR1BL1 1
#define OCR1BL2 2
#define OCR1BL3 3
#define OCR1BL4 4
#define OCR1BL5 5
#define OCR1BL6 6
#define OCR1BL7 7

#define OCR1BH _SFR_MEM8(0x08B)
#define OCR1BH0 0
#define OCR1BH1 1
#define OCR1BH2 2
#define OCR1BH3 3
#define OCR1BH4 4
#define OCR1BH5 5
#define OCR1BH6 6
#define OCR1BH7 7

#define TCCR3A _SFR_MEM8(0x090)
#define WGM30 0
#define WGM31 1
#define COM3B0 4
#define COM3B1 5
#define COM3A0 6
#define COM3A1 7

#define TCCR3B _SFR_MEM8(0x091)
#define CS30 0
#define CS31 1
#define CS32 2
#define WGM32 3
#define WGM33 4
#define ICES3 6
#define ICNC3 7

#define TCCR3C _SFR_MEM8(0x092)
#define FOC3B 6
#define FOC3A 7

#define TCNT3 _SFR_MEM16(0x094)

#define TCNT3L _SFR_MEM8(0x094)
#define TCNT3L0 0
#define TCNT3L1 1
#define TCNT3L2 2
#define TCNT3L3 3
#define TCNT3L4 4
#define TCNT3L5 5
#define TCNT3L6 6
#define TCNT3L7 7

#define TCNT3H _SFR_MEM8(0x095)
#define TCNT3H0 0
#define TCNT3H1 1
#define TCNT3H2 2
#define TCNT3H3 3
#define TCNT3H4 4
#define TCNT3H5 5
#define TCNT3H6 6
#define TCNT3H7 7

#define ICR3 _SFR_MEM16(0x096)

#define ICR3L _SFR_MEM8(0x096)
#define ICR3L0 0
#define ICR3L1 1
#define ICR3L2 2
#define ICR3L3 3
#define ICR3L4 4
#define ICR3L5 5
#define ICR3L6 6
#define ICR3L7 7

#define ICR3H _SFR_MEM8(0x097)
#define ICR3H0 0
#define ICR3H1 1
#define ICR3H2 2
#define ICR3H3 3
#define ICR3H4 4
#define ICR3H5 5
#define ICR3H6 6
#define ICR3H7 7

#define OCR3A _SFR_MEM16(0x098)

#define OCR3AL _SFR_MEM8(0x098)
#define OCR3AL0 0
#define OCR3AL3 1
#define OCR3AL2 2
#define OCR3AL4 4
#define OCR3AL5 5
#define OCR3AL6 6
#define OCR3AL7 7

#define OCR3AH _SFR_MEM8(0x099)
#define OCR3AH0 0
#define OCR3AH1 1
#define OCR3AH2 2
#define OCR3AH3 3
#define OCR3AH4 4
#define OCR3AH5 5
#define OCR3AH6 6
#define OCR3AH7 7

#define OCR3B _SFR_MEM16(0x09A)

#define OCR3BL _SFR_MEM8(0x09A)
#define OCR3BL0 0
#define OCR3BL1 1
#define OCR3BL2 2
#define OCR3BL3 3
#define OCR3BL4 4
#define OCR3BL5 5
#define OCR3BL6 6
#define OCR3BL7 7

#define OCR3BH _SFR_MEM8(0x09B)
#define OCR3BH0 0
#define OCR3BH1 1
#define OCR3BH2 2
#define OCR3BH3 3
#define OCR3BH4 4
#define OCR3BH5 5
#define OCR3BH6 6
#define OCR3BH7 7

#define TCCR2A _SFR_MEM8(0x0A0)
#define WGM20 0
#define WGM21 1
#define COM2B0 4
#define COM2B1 5
#define COM2A0 6
#define COM2A1 7

#define TCCR2B _SFR_MEM8(0x0A1)
#define CS20 0
#define CS21 1
#define CS22 2
#define WGM22 3
#define FOC2B 6
#define FOC2A 7

#define TCNT2 _SFR_MEM8(0x0A2)
#define TCNT2_0 0
#define TCNT2_1 1
#define TCNT2_2 2
#define TCNT2_3 3
#define TCNT2_4 4
#define TCNT2_5 5
#define TCNT2_6 6
#define TCNT2_7 7

#define OCR2A _SFR_MEM8(0x0A3)
#define OCR2A_0 0
#define OCR2A_1 1
#define OCR2A_2 2
#define OCR2A_3 3
#define OCR2A_4 4
#define OCR2A_5 5
#define OCR2A_6 6
#define OCR2A_7 7

#define OCR2B _SFR_MEM8(0x0A4)
#define OCR2B_0 0
#define OCR2B_1 1
#define OCR2B_2 2
#define OCR2B_3 3
#define OCR2B_4 4
#define OCR2B_5 5
#define OCR2B_6 6
#define OCR2B_7 7

#define TCCR4A _SFR_MEM8(0x0A8)
#define WGM40 0
#define WGM41 1
#define COM4B0 4
#define COM4B1 5
#define COM4A0 6
#define COM4A1 7

#define TCCR4B _SFR_MEM8(0x0A9)
#define CS40 0
#define CS41 1
#define CS42 2
#define WGM42 3
#define FOC4B 6
#define FOC4A 7

#define TCNT4 _SFR_MEM8(0x0AA)
#define TCNT4_0 0
#define TCNT4_1 1
#define TCNT4_2 2
#define TCNT4_3 3
#define TCNT4_4 4
#define TCNT4_5 5
#define TCNT4_6 6
#define TCNT4_7 7

#define OCR4A _SFR_MEM8(0x0AB)
#define OCR4A_0 0
#define OCR4A_1 1
#define OCR4A_2 2
#define OCR4A_3 3
#define OCR4A_4 4
#define OCR4A_5 5
#define OCR4A_6 6
#define OCR4A_7 7

#define OCR4B _SFR_MEM8(0x0AC)
#define OCR4B_0 0
#define OCR4B_1 1
#define OCR4B_2 2
#define OCR4B_3 3
#define OCR4B_4 4
#define OCR4B_5 5
#define OCR4B_6 6
#define OCR4B_7 7

#define TWBR _SFR_MEM8(0x0B8)

#define TWSR _SFR_MEM8(0x0B9)

#define TWAR _SFR_MEM8(0x0BA)

#define TWDR _SFR_MEM8(0x0BB)

#define TWCR _SFR_MEM8(0x0BC)

#define TWAMR _SFR_MEM8(0x0BD)

#define UCSR0A _SFR_MEM8(0x0C0)
#define MPCM0 0
#define U2X0 1
#define UPE0 2
#define DOR0 3
#define FE0 4
#define UDRE0 5
#define TXC0 6
#define RXC0 7

#define UCSR0B _SFR_MEM8(0x0C1)
#define TXB80 0
#define RXB80 1
#define UCSZ02 2
#define TXEN0 3
#define RXEN0 4
#define UDRIE0 5
#define TXCIE0 6
#define RXCIE0 7

#define UCSR0C _SFR_MEM8(0x0C2)
#define UCPOL0 0
#define UCSZ00 1
#define UCSZ01 2
#define USBS0 3
#define UPM00 4
#define UPM01 5
#define UMSEL00 6
#define UMSEL01 7

#define UBRR0 _SFR_MEM16(0x0C4)

#define UBRR0L _SFR_MEM8(0x0C4)
#define UBRR0_0 0
#define UBRR0_1 1
#define UBRR0_2 2
#define UBRR0_3 3
#define UBRR0_4 4
#define UBRR0_5 5
#define UBRR0_6 6
#define UBRR0_7 7

#define UBRR0H _SFR_MEM8(0x0C5)
#define UBRR0_8 0
#define UBRR0_9 1
#define UBRR0_10 2
#define UBRR0_11 3

#define UDR0 _SFR_MEM8(0x0C6)
#define UDR0_0 0
#define UDR0_1 1
#define UDR0_2 2
#define UDR0_3 3
#define UDR0_4 4
#define UDR0_5 5
#define UDR0_6 6
#define UDR0_7 7

#define LCDCRA _SFR_MEM8(0x0E0)
#define LCDBL 0
#define LCDIE 3
#define LCDIF 4
#define LCDAB 6
#define LCDEN 7

#define LCDFRR _SFR_MEM8(0x0E1)
#define LCDCD0 0
#define LCDCD1 1
#define LCDCD2 2
#define LCDPS0 4
#define LCDPS1 5
#define LCDPS2 6
#define LCDPBUFEN 7

#define LCDCCR _SFR_MEM8(0x0E2)
#define LCDCC0 0
#define LCDCC1 1
#define LCDCC2 2
#define LCDCC3 3
#define LCDMDT 4
#define LCDDC0 5
#define LCDDC1 6
#define LCDDC2 7

#define LCDDR0 _SFR_MEM8(0x0E3)
#define SEG000 0
#define SEG001 1
#define SEG002 2
#define SEG003 3
#define SEG004 4
#define SEG005 5
#define SEG006 6
#define SEG007 7

#define LCDDR1 _SFR_MEM8(0x0E4)
#define SEG008 0
#define SEG009 1
#define SEG010 2
#define SEG011 3
#define SEG012 4
#define SEG013 5
#define SEG014 6
#define SEG015 7

#define LCDDR2 _SFR_MEM8(0x0E5)
#define SEG100 0
#define SEG101 1
#define SEG102 2
#define SEG103 3
#define SEG104 4
#define SEG105 5
#define SEG106 6
#define SEG107 7

#define LCDDR3 _SFR_MEM8(0x0E6)
#define SEG108 0
#define SEG109 1
#define SEG024 2
#define SEG111 3
#define SEG112 4
#define SEG113 5
#define SEG114 6
#define SEG115 7

#define LCDDR4 _SFR_MEM8(0x0E7)
#define SEG200 0
#define SEG201 1
#define SEG202 2
#define SEG203 3
#define SEG204 4
#define SEG205 5
#define SEG206 6
#define SEG207 7

#define LCDDR5 _SFR_MEM8(0x0E8)
#define SEG208 0
#define SEG209 1
#define SEG210 2
#define SEG211 3
#define SEG213 5
#define SEG214 6
#define SEG215 7

#define LCDDR6 _SFR_MEM8(0x0E9)
#define SEG300 0
#define SEG301 1
#define SEG302 2
#define SEG303 3
#define SEG304 4
#define SEG305 5
#define SEG306 6
#define SEG307 7

#define LCDDR7 _SFR_MEM8(0x0EA)
#define SEG308 0
#define SEG309 1
#define SEG310 2
#define SEG311 3
#define SEG312 4
#define SEG313 5
#define SEG314 6
#define SEG315 7

#define SC0ICLK _SFR_MEM8(0x100)

#define SC0WT0 _SFR_MEM8(0x101)

#define SC0WT1 _SFR_MEM8(0x102)

#define SC0WT2 _SFR_MEM8(0x103)

#define SC0WT3 _SFR_MEM8(0x104)

#define SC0GT _SFR_MEM16(0x105)

#define SC0GTL _SFR_MEM8(0x105)

#define SC0GTH _SFR_MEM8(0x106)

#define SC0ETU _SFR_MEM16(0x107)

#define SC0ETUL _SFR_MEM8(0x107)

#define SC0ETUH _SFR_MEM8(0x108)

#define SC0IBUF _SFR_MEM8(0x109)

#define SC0SR _SFR_MEM8(0x10A)

#define SC0IER _SFR_MEM8(0x10B)

#define SC0IIR _SFR_MEM8(0x10C)

#define SC0ISR _SFR_MEM8(0x10D)

#define SC0CON _SFR_MEM8(0x10E)

#define SC0ICR _SFR_MEM8(0x10F)

#define SC1ICLK _SFR_MEM8(0x110)

#define SC1WT0 _SFR_MEM8(0x111)

#define SC1WT1 _SFR_MEM8(0x112)

#define SC1WT2 _SFR_MEM8(0x113)

#define SC1WT3 _SFR_MEM8(0x114)

#define SC1GT _SFR_MEM16(0x115)

#define SC1GTL _SFR_MEM8(0x115)

#define SC1GTH _SFR_MEM8(0x116)

#define SC1ETU _SFR_MEM16(0x117)

#define SC1ETUL _SFR_MEM8(0x117)

#define SC1ETUH _SFR_MEM8(0x118)

#define SC1IBUF _SFR_MEM8(0x119)

#define SC1SR _SFR_MEM8(0x11A)

#define SC1IER _SFR_MEM8(0x11B)

#define SC1IIR _SFR_MEM8(0x11C)

#define SC1ISR _SFR_MEM8(0x11D)

#define SC1CON _SFR_MEM8(0x11E)

#define SC1ICR _SFR_MEM8(0x11F)

#define SC2ICLK _SFR_MEM8(0x120)

#define SC2WT0 _SFR_MEM8(0x121)

#define SC2WT1 _SFR_MEM8(0x122)

#define SC2WT2 _SFR_MEM8(0x123)

#define SC2WT3 _SFR_MEM8(0x124)

#define SC2GT _SFR_MEM16(0x125)

#define SC2GTL _SFR_MEM8(0x125)

#define SC2GTH _SFR_MEM8(0x126)

#define SC2ETU _SFR_MEM16(0x127)

#define SC2ETUL _SFR_MEM8(0x127)

#define SC2ETUH _SFR_MEM8(0x128)

#define SC2IBUF _SFR_MEM8(0x129)

#define SC2SR _SFR_MEM8(0x12A)

#define SC2IER _SFR_MEM8(0x12B)

#define SC2IIR _SFR_MEM8(0x12C)

#define SC2ISR _SFR_MEM8(0x12D)

#define SC2CON _SFR_MEM8(0x12E)

#define SC2ICR _SFR_MEM8(0x12F)

#define ACLCr _SFR_MEM8(0x140)

#define ACLMr _SFR_MEM8(0x141)

#define ACLIr _SFR_MEM8(0x142)

#define ACLKey1b1r _SFR_MEM8(0x146)

#define ACLKey1b2r _SFR_MEM8(0x147)

#define ACLKey1b3r _SFR_MEM8(0x148)

#define ACLKey1b4r _SFR_MEM8(0x149)

#define ACLKey1b5r _SFR_MEM8(0x14A)

#define ACLKey1b6r _SFR_MEM8(0x14B)

#define ACLKey1b7r _SFR_MEM8(0x14C)

#define ACLKey1b8r _SFR_MEM8(0x14D)

#define ACLKey2b1r _SFR_MEM8(0x14E)

#define ACLKey2b2r _SFR_MEM8(0x14F)

#define ACLKey2b3r _SFR_MEM8(0x150)

#define ACLKey2b4r _SFR_MEM8(0x151)

#define ACLKey2b5r _SFR_MEM8(0x152)

#define ACLKey2b6r _SFR_MEM8(0x153)

#define ACLKey2b7r _SFR_MEM8(0x154)

#define ACLKey2b8r _SFR_MEM8(0x155)

#define ACLIdata1r _SFR_MEM8(0x156)

#define ACLIdata2r _SFR_MEM8(0x157)

#define ACLIdata3r _SFR_MEM8(0x158)

#define ACLIdata4r _SFR_MEM8(0x159)

#define ACLIdata5r _SFR_MEM8(0x15A)

#define ACLIdata6r _SFR_MEM8(0x15B)

#define ACLIdata7r _SFR_MEM8(0x15C)

#define ACLIdata8r _SFR_MEM8(0x15D)

#define ACLOdata1r _SFR_MEM8(0x15E)

#define ACLOdata2r _SFR_MEM8(0x15F)

#define ACLOdata3r _SFR_MEM8(0x160)

#define ACLOdata4r _SFR_MEM8(0x161)

#define ACLOdata5r _SFR_MEM8(0x162)

#define ACLOdata6r _SFR_MEM8(0x163)

#define ACLOdata7r _SFR_MEM8(0x164)

#define ACLOdata8r _SFR_MEM8(0x165)

#define ACLIv1r _SFR_MEM8(0x166)

#define ACLIv2r _SFR_MEM8(0x167)

#define ACLIv3r _SFR_MEM8(0x168)

#define ACLIv4r _SFR_MEM8(0x169)

#define ACLIv5r _SFR_MEM8(0x16A)

#define ACLIv6r _SFR_MEM8(0x16B)

#define ACLIv7r _SFR_MEM8(0x16C)

#define ACLIv8r _SFR_MEM8(0x16D)

#define ACLVr _SFR_MEM8(0x16F)

#define DSRCDvrr _SFR_MEM8(0x180)

#define DSRCDldr _SFR_MEM8(0x181)

#define DSRCDenr _SFR_MEM8(0x182)

#define DSRCIntr _SFR_MEM8(0x188)

#define DSRCInrr _SFR_MEM8(0x189)

#define DSRCItmr _SFR_MEM8(0x18A)

#define DSRCIrmr _SFR_MEM8(0x18B)

#define DSRCItcr _SFR_MEM8(0x18C)

#define DSRCIrcr _SFR_MEM8(0x18D)

#define DSRCTcdrl _SFR_MEM8(0x190)

#define DSRCTcdrh _SFR_MEM8(0x191)

#define DSRCRcdrl _SFR_MEM8(0x192)

#define DSRCRcdrh _SFR_MEM8(0x193)

#define DSRCAder _SFR_MEM8(0x196)

#define DSRCAdsr _SFR_MEM8(0x197)

#define DSRCRadr0 _SFR_MEM8(0x198)

#define DSRCRadr1 _SFR_MEM8(0x199)

#define DSRCRadr2 _SFR_MEM8(0x19A)

#define DSRCRadr3 _SFR_MEM8(0x19B)

#define DSRCPadr0 _SFR_MEM8(0x19C)

#define DSRCPadr1 _SFR_MEM8(0x19D)

#define DSRCPadr2 _SFR_MEM8(0x19E)

#define DSRCPadr3 _SFR_MEM8(0x19F)

#define DSRCPamr0 _SFR_MEM8(0x1A0)

#define DSRCPamr1 _SFR_MEM8(0x1A1)

#define DSRCPamr2 _SFR_MEM8(0x1A2)

#define DSRCPamr3 _SFR_MEM8(0x1A3)

#define DSRCGadr00 _SFR_MEM8(0x1A4)

#define DSRCGadr01 _SFR_MEM8(0x1A5)

#define DSRCGadr02 _SFR_MEM8(0x1A6)

#define DSRCGadr03 _SFR_MEM8(0x1A7)

#define DSRCGamr00 _SFR_MEM8(0x1A8)

#define DSRCGamr01 _SFR_MEM8(0x1A9)

#define DSRCGamr02 _SFR_MEM8(0x1AA)

#define DSRCGamr03 _SFR_MEM8(0x1AB)

#define DSRCGadr10 _SFR_MEM8(0x1AC)

#define DSRCGadr11 _SFR_MEM8(0x1AD)

#define DSRCGadr12 _SFR_MEM8(0x1AE)

#define DSRCGadr13 _SFR_MEM8(0x1AF)

#define DSRCGamr10 _SFR_MEM8(0x1B0)

#define DSRCGamr11 _SFR_MEM8(0x1B1)

#define DSRCGamr12 _SFR_MEM8(0x1B2)

#define DSRCGamr13 _SFR_MEM8(0x1B3)

#define DSRCGadr20 _SFR_MEM8(0x1B4)

#define DSRCGadr21 _SFR_MEM8(0x1B5)

#define DSRCGadr22 _SFR_MEM8(0x1B6)

#define DSRCGadr23 _SFR_MEM8(0x1B7)

#define DSRCGamr20 _SFR_MEM8(0x1B8)

#define DSRCGamr21 _SFR_MEM8(0x1B9)

#define DSRCGamr22 _SFR_MEM8(0x1BA)

#define DSRCGamr23 _SFR_MEM8(0x1BB)

#define DSRCGadr30 _SFR_MEM8(0x1BC)

#define DSRCGadr31 _SFR_MEM8(0x1BD)

#define DSRCGadr32 _SFR_MEM8(0x1BE)

#define DSRCGadr33 _SFR_MEM8(0x1BF)

#define DSRCGamr30 _SFR_MEM8(0x1C0)

#define DSRCGamr31 _SFR_MEM8(0x1C1)

#define DSRCGamr32 _SFR_MEM8(0x1C2)

#define DSRCGamr33 _SFR_MEM8(0x1C3)

#define DSRCBmr0 _SFR_MEM8(0x1C4)

#define DSRCBmr1 _SFR_MEM8(0x1C5)

#define DSRCBmr2 _SFR_MEM8(0x1C6)

#define DSRCBmr3 _SFR_MEM8(0x1C7)

#define DSRCTcmr _SFR_MEM8(0x1C8)

#define DSRCTcnr _SFR_MEM8(0x1C9)

#define DSRCTstr _SFR_MEM8(0x1CA)

#define DSRCTmor _SFR_MEM8(0x1CB)

#define DSRCTrdr _SFR_MEM8(0x1CC)

#define DSRCTflr0 _SFR_MEM8(0x1CD)

#define DSRCTflr1 _SFR_MEM8(0x1CE)

#define DSRCTfsr _SFR_MEM8(0x1CF)

#define DSRCTfir0 _SFR_MEM8(0x1D0)

#define DSRCTfir1 _SFR_MEM8(0x1D1)

#define DSRCTfir2 _SFR_MEM8(0x1D2)

#define DSRCTfir3 _SFR_MEM8(0x1D3)

#define DSRCTprr0 _SFR_MEM8(0x1D4)

#define DSRCTprr1 _SFR_MEM8(0x1D5)

#define DSRCTpor0 _SFR_MEM8(0x1D6)

#define DSRCTpor1 _SFR_MEM8(0x1D7)

#define DSRCPplr _SFR_MEM8(0x1D8)

#define DSRCRcmr _SFR_MEM8(0x1E0)

#define DSRCRcnr _SFR_MEM8(0x1E1)

#define DSRCRstr _SFR_MEM8(0x1E2)

#define DSRCRflr0 _SFR_MEM8(0x1E3)

#define DSRCRflr1 _SFR_MEM8(0x1E4)

#define DSRCRdtr _SFR_MEM8(0x1E5)

#define DSRCRrdr _SFR_MEM8(0x1E6)

#define DSRCRfir0 _SFR_MEM8(0x1E8)

#define DSRCRfir1 _SFR_MEM8(0x1E9)

#define DSRCRfir2 _SFR_MEM8(0x1EA)

#define DSRCRfir3 _SFR_MEM8(0x1EB)

#define DSRCRfcr0 _SFR_MEM8(0x1EC)

#define DSRCRfcr1 _SFR_MEM8(0x1ED)

#define DSRCRfcr2 _SFR_MEM8(0x1EE)

#define DSRCRfcr3 _SFR_MEM8(0x1EF)

#define DSRCTestr _SFR_MEM8(0x1F0)

#define DSRCFstr _SFR_MEM8(0x1F4)

#define DSRCTfcr _SFR_MEM8(0x1F5)

#define DSRCRfcr _SFR_MEM8(0x1F6)

#define DSRCDtdf _SFR_MEM8(0x1F7)

#define DSRCDrdf _SFR_MEM8(0x1F8)


/* Interrupt vectors */
/* Vector 0 is the reset vector */
#define PCINT0_vect_num  1
#define PCINT0_vect      _VECTOR(1)  /* Pin Change Interrupt Request 0 PortA */
#define PCINT1_vect_num  2
#define PCINT1_vect      _VECTOR(2)  /* Pin Change Interrupt Request 1 PortB */
#define PCINT2_vect_num  3
#define PCINT2_vect      _VECTOR(3)  /* Pin Change Interrupt Request 2 PortC */
#define PCINT3_vect_num  4
#define PCINT3_vect      _VECTOR(4)  /* Pin Change Interrupt Request 3 PortD-LCD */
#define PCINT4_vect_num  5
#define PCINT4_vect      _VECTOR(5)  /* Pin Change Interrupt Request 4 PortE-LCD */
#define PCINT5_vect_num  6
#define PCINT5_vect      _VECTOR(6)  /* Pin Change Interrupt Request 5 PortF-LCD */
#define WDT_vect_num  7
#define WDT_vect      _VECTOR(7)  /* Watchdog Time out Interrupt */
#define TIMER4_COMPA_vect_num  8
#define TIMER4_COMPA_vect      _VECTOR(8)  /* Timer 8bit /Counter4 Compare Match A */
#define TIMER4_COMPB_vect_num  9
#define TIMER4_COMPB_vect      _VECTOR(9)  /* Timer 8bit /Counter4 Compare Match B */
#define TIMER4_OVF_vect_num  10
#define TIMER4_OVF_vect      _VECTOR(10)  /* Timer/Counter4 Overflow */
#define TIMER3_INPUT_CAPT_vect_num  11
#define TIMER3_INPUT_CAPT_vect      _VECTOR(11)  /* Timer 16bit/Counter3 Capture Event */
#define TIMER3_COMPA_vect_num  12
#define TIMER3_COMPA_vect      _VECTOR(12)  /* Timer 16bit/Counter3 Compare Match A */
#define TIMER3_COMPB_vect_num  13
#define TIMER3_COMPB_vect      _VECTOR(13)  /* Timer 16bit/Counter3 Compare Match B */
#define TIMER3_OVF_vect_num  14
#define TIMER3_OVF_vect      _VECTOR(14)  /* Timer3 16 bit/Counter3 Overflow */
#define TIMER2_COMPA_vect_num  15
#define TIMER2_COMPA_vect      _VECTOR(15)  /* Timer 8bit/Counter2 Compare Match A */
#define TIMER2_COMPB_vect_num  16
#define TIMER2_COMPB_vect      _VECTOR(16)  /* Timer 8bit/Counter2 Compare Match B */
#define TIMER2_OVF_vect_num  17
#define TIMER2_OVF_vect      _VECTOR(17)  /* Timer2 8 bit/Counter2 Overflow */
#define TIMER1_INPUT_CAPT_vect_num  18
#define TIMER1_INPUT_CAPT_vect      _VECTOR(18)  /* Timer1 16bit/Counter1 Capture Event */
#define TIMER1_COMPA_vect_num  19
#define TIMER1_COMPA_vect      _VECTOR(19)  /* Timer 16bit/Counter1 Compare Match A */
#define TIMER1_COMPB_vect_num  20
#define TIMER1_COMPB_vect      _VECTOR(20)  /* Timer 16bit/Counter1 Compare Match B */
#define TIMER1_OVF_vect_num  21
#define TIMER1_OVF_vect      _VECTOR(21)  /* Timer1 16 bit/Counter1 Overflow */
#define TIMER0_COMP_A_vect_num  22
#define TIMER0_COMP_A_vect      _VECTOR(22)  /* Timer0/Counter0 Compare Match */
#define TIMER0_COMP_B_vect_num  23
#define TIMER0_COMP_B_vect      _VECTOR(23)  /* Timer0 8bit /Counter0 Compare Match A */
#define TIMER0_OVF_vect_num  24
#define TIMER0_OVF_vect      _VECTOR(24)  /* Timer0 8bit/Counter0 Overflow */
#define IRQ_DSRC_vect_num  25
#define IRQ_DSRC_vect      _VECTOR(25)  /* DSRC interrupt - custom IP */
#define IRQ_SCI0_vect_num  26
#define IRQ_SCI0_vect      _VECTOR(26)  /* Smart Card 0 interrupt */
#define IRQ_SCI1_vect_num  27
#define IRQ_SCI1_vect      _VECTOR(27)  /* Smart Card 1 interrupt */
#define IRQ_SCI2_vect_num  28
#define IRQ_SCI2_vect      _VECTOR(28)  /* Smart Card 2 interrupt */
#define IRQ_CL_vect_num  29
#define IRQ_CL_vect      _VECTOR(29)  /* AWAH custom IP interrupt */
#define SPI_STC_vect_num  30
#define SPI_STC_vect      _VECTOR(30)  /* SPI Serial Transfer Complete */
#define USART0_RX_vect_num  31
#define USART0_RX_vect      _VECTOR(31)  /* USART0, Rx Complete */
#define USART0_UDRE_vect_num  32
#define USART0_UDRE_vect      _VECTOR(32)  /* USART0 Data register Empty */
#define USART0_TX_vect_num  33
#define USART0_TX_vect      _VECTOR(33)  /* USART0, Tx Complete */
#define TWI_vect_num  34
#define TWI_vect      _VECTOR(34)  /* 2 wire interface */
#define SPM_READY_vect_num  35
#define SPM_READY_vect      _VECTOR(35)  /* Store Program Memory Read */
#define LCD_vect_num  36
#define LCD_vect      _VECTOR(36)  /* LCD Start of Frame */
#define BOD_SMHIGH_vect_num  37
#define BOD_SMHIGH_vect      _VECTOR(37)  /* SM High level - SWVBAT between 2V to 2.2V */

#define _VECTOR_SIZE 4 /* Size of individual vector. */
#define _VECTORS_SIZE (38 * _VECTOR_SIZE)


/* Constants */
#define SPM_PAGESIZE (256)
#define RAMSTART     (0x200)
#define RAMSIZE      (8192)
#define RAMEND       (RAMSTART + RAMSIZE - 1)
#define XRAMSTART    (NA)
#define XRAMSIZE     (0)
#define XRAMEND      (RAMEND)
#define E2END        (0x0)
#define E2PAGESIZE   (0)
#define FLASHEND     (0xFFFF)


/* Fuses */
#define FUSE_MEMORY_SIZE 1

/* Fuse Byte */
#define FUSE_BOOTRST  (unsigned char)~_BV(0)  /* Select Reset Vector */
#define FUSE_BOOTSZ0  (unsigned char)~_BV(1)  /* Select Boot Size */
#define FUSE_BOOTSZ1  (unsigned char)~_BV(2)  /* Select Boot Size */
#define FUSE_SMHIGH  (unsigned char)~_BV(3)  /* SM SWVBAT not activated by default */
#define FUSE_WDTON  (unsigned char)~_BV(4)  /* Watchdog timer always on */
#define FUSE_CKSEL0  (unsigned char)~_BV(5)  /* Select Clock Source */
#define FUSE_SUT  (unsigned char)~_BV(6)  /* Select start-up time */
#define FUSE_OCDEN  (unsigned char)~_BV(7)  /* Enable OCD */
#define FUSE_DEFAULT (FUSE_BOOTSZ1 & FUSE_BOOTSZ0)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST


/* Signature */
#define SIGNATURE_0 0x0
#define SIGNATURE_1 0x0
#define SIGNATURE_2 0x0


#endif /* _AVR_58U54A_Albatross_H_ */

