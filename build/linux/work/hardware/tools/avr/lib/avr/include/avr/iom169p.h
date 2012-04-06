/* Copyright (c) 2002, 2003, 2004, 2005, 2006
   Juergen Schilling <juergen.schilling@honeywell.com>
   Eric B. Weddington <ericw@evcohs.com>
   Anatoly Sokolov <aesok@post.ru>
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

/* $Id: iom169p.h,v 1.2.2.5 2008/10/17 23:27:48 arcanum Exp $ */

/* iom169p.h - definitions for ATmega169P */

#ifndef _AVR_IOM169P_H_
#define _AVR_IOM169P_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom169p.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* Port A */
#define PINA   _SFR_IO8(0x00)
#define PINA7	7
#define PINA6	6
#define PINA5	5
#define PINA4	4
#define PINA3	3
#define PINA2	2
#define PINA1	1
#define PINA0	0

#define DDRA   _SFR_IO8(0x01)
#define DDA7	7
#define DDA6	6
#define DDA5	5
#define DDA4	4
#define DDA3	3
#define DDA2	2
#define DDA1	1
#define DDA0	0

#define PORTA  _SFR_IO8(0x02)
#define PA7	7
#define PA6	6
#define PA5	5
#define PA4	4
#define PA3	3
#define PA2	2
#define PA1	1
#define PA0	0

/* Port B */
#define PINB   _SFR_IO8(0x03)
#define PINB7	7
#define PINB6	6
#define PINB5	5
#define PINB4	4
#define PINB3	3
#define PINB2	2
#define PINB1	1
#define PINB0	0

#define DDRB   _SFR_IO8(0x04)
#define DDB7	7
#define DDB6	6
#define DDB5	5
#define DDB4	4
#define DDB3	3
#define DDB2	2
#define DDB1	1
#define DDB0	0

#define PORTB  _SFR_IO8(0x05)
#define PB7	7
#define PB6	6
#define PB5	5
#define PB4	4
#define PB3	3
#define PB2	2
#define PB1	1
#define PB0	0

/* Port C */
#define PINC   _SFR_IO8(0x06)
#define PINC7	7
#define PINC6	6
#define PINC5	5
#define PINC4	4
#define PINC3	3
#define PINC2	2
#define PINC1	1
#define PINC0	0

#define DDRC   _SFR_IO8(0x07)
#define DDC7	7
#define DDC6	6
#define DDC5	5
#define DDC4	4
#define DDC3	3
#define DDC2	2
#define DDC1	1
#define DDC0	0

#define PORTC  _SFR_IO8(0x08)
#define PC7	 7
#define PC6	 6
#define PC5	 5
#define PC4	 4
#define PC3	 3
#define PC2	 2
#define PC1	 1
#define PC0	 0

/* Port D */
#define PIND   _SFR_IO8(0x09)
#define PIND7	7
#define PIND6	6
#define PIND5	5
#define PIND4	4
#define PIND3	3
#define PIND2	2
#define PIND1	1
#define PIND0	0

#define DDRD   _SFR_IO8(0x0A)
#define DDD7	7
#define DDD6	6
#define DDD5	5
#define DDD4	4
#define DDD3	3
#define DDD2	2
#define DDD1	1
#define DDD0	0

#define PORTD  _SFR_IO8(0x0B)
#define PD7	 7
#define PD6	 6
#define PD5	 5
#define PD4	 4
#define PD3	 3
#define PD2	 2
#define PD1	 1
#define PD0	 0

/* Port E */
#define PINE   _SFR_IO8(0x0C)
#define PINE7	7
#define PINE6	6
#define PINE5	5
#define PINE4	4
#define PINE3	3
#define PINE2	2
#define PINE1	1
#define PINE0	0

#define DDRE   _SFR_IO8(0x0D)
#define DDE7	7
#define DDE6	6
#define DDE5	5
#define DDE4	4
#define DDE3	3
#define DDE2	2
#define DDE1	1
#define DDE0	0

#define PORTE  _SFR_IO8(0x0E)
#define PE7	7
#define PE6	6
#define PE5	5
#define PE4	4
#define PE3	3
#define PE2	2
#define PE1	1
#define PE0	0

/* Port F */
#define PINF   _SFR_IO8(0x0F)
#define PINF7	7
#define PINF6	6
#define PINF5	5
#define PINF4	4
#define PINF3	3
#define PINF2	2
#define PINF1	1
#define PINF0	0

#define DDRF   _SFR_IO8(0x10)
#define DDF7	7
#define DDF6	6
#define DDF5	5
#define DDF4	4
#define DDF3	3
#define DDF2	2
#define DDF1	1
#define DDF0	0

#define PORTF  _SFR_IO8(0x11)
#define PF7	7
#define PF6	6
#define PF5	5
#define PF4	4
#define PF3	3
#define PF2	2
#define PF1	1
#define PF0	0

/* Port G */
#define PING   _SFR_IO8(0x12)
#define PING5	5
#define PING4	4
#define PING3	3
#define PING2	2
#define PING1	1
#define PING0	0

#define DDRG   _SFR_IO8(0x13)
#define DDG4	4
#define DDG3	3
#define DDG2	2
#define DDG1	1
#define DDG0	0

#define PORTG  _SFR_IO8(0x14)
#define PG4	4
#define PG3	3
#define PG2	2
#define PG1	1
#define PG0	0

/* Timer/Counter 0 interrupt Flag Register */
#define TIFR0  _SFR_IO8(0x15)
#define OCF0A	1
#define TOV0	0

/* Timer/Counter 1 interrupt Flag Register */
#define TIFR1  _SFR_IO8(0x16)
#define ICF1	5
#define OCF1B	2
#define OCF1A	1
#define TOV1	0

/* Timer/Counter 2 interrupt Flag Register */
#define TIFR2  _SFR_IO8(0x17)
#define OCF2A	1
#define TOV2	0

/* External Interrupt Flag Register */
#define EIFR   _SFR_IO8(0x1C)
#define PCIF1  7
#define PCIF0  6
#define INTF0  0

/* External Interrupt Mask Register */
#define EIMSK  _SFR_IO8(0x1D)
#define PCIE1  7
#define PCIE0  6
#define INT0   0

/* General Purpose I/O Register 0 */
#define GPIOR0 _SFR_IO8(0x1E)

#define EECR   _SFR_IO8(0x1F)
#define EERIE   3
#define EEMWE   2
#define EEWE    1
#define EERE    0

#define EEDR   _SFR_IO8(0X20)

/* Combine EEARL and EEARH */
#define EEAR   _SFR_IO16(0x21)
#define EEARL  _SFR_IO8(0x21)
#define EEARH  _SFR_IO8(0X22)

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

/* General Timer/Counter Control Register */
#define GTCCR  _SFR_IO8(0x23)
#define TSM    7
#define PSR2   1
#define PSR10  0

/* Timer/Counter Control Register A */
#define TCCR0A _SFR_IO8(0x24)
#define FOC0A	7
#define WGM00	6
#define COM0A1	5
#define COM0A0	4
#define WGM01	3
#define CS02	2
#define CS01	1
#define CS00	0

/* Timer/Counter Register */
#define TCNT0  _SFR_IO8(0x26)

/* Output Compare Register A */
#define OCR0A  _SFR_IO8(0x27)

/* General Purpose I/O Register 1 */
#define GPIOR1 _SFR_IO8(0x2A)

/* General Purpose I/O Register 2 */
#define GPIOR2 _SFR_IO8(0x2B)

/* SPI Control Register */
#define SPCR   _SFR_IO8(0x2C)
#define SPIE	7
#define SPE	6
#define DORD	5
#define MSTR	4
#define CPOL	3
#define CPHA	2
#define SPR1	1
#define SPR0	0

/* SPI Status Register */
#define SPSR   _SFR_IO8(0x2D)
#define SPIF	7
#define WCOL	6
#define SPI2X	0

/* SPI Data Register */
#define SPDR   _SFR_IO8(0x2E)

/* Analog Comperator Control and Status Register */
#define ACSR   _SFR_IO8(0x30)
#define ACD	7
#define ACBG	6
#define ACO	5
#define ACI	4
#define ACIE	3
#define ACIC	2
#define ACIS1	1
#define ACIS0	0

/* On-chip Debug Register */
#define OCDR   _SFR_IO8(0x31)
#define IDRD   7
#define OCDR7  7
#define OCDR6  6
#define OCDR5  5
#define OCDR4  4
#define OCDR3  3
#define OCDR2  2
#define OCDR1  1
#define OCDR0  0

/* Sleep Mode Control Register */
#define SMCR   _SFR_IO8(0x33)
#define SM2    3
#define SM1    2
#define SM0    1
#define SE     0

/* MCU Status Register */
#define MCUSR  _SFR_IO8(0x34)
#define JTRF   4
#define WDRF   3
#define BORF   2
#define EXTRF  1
#define PORF   0

/* MCU Control Rgeister */
#define MCUCR  _SFR_IO8(0x35)
#define JTD    7
#define PUD    4
#define IVSEL  1
#define IVCE   0

/* Store Program Memory Control and Status Register */
#define SPMCSR _SFR_IO8(0x37)
#define SPMIE  7
#define RWWSB  6
#define RWWSRE 4
#define BLBSET 3
#define PGWRT  2
#define PGERS  1
#define SPMEN  0

/* Watchdog Timer Control Register */
#define WDTCR  _SFR_MEM8(0x60)
#define WDCE	4
#define WDE	3
#define WDP2	2
#define WDP1	1
#define WDP0	0

/* Clock Prescale Register */
#define CLKPR  _SFR_MEM8(0x61)
#define CLKPCE 7
#define CLKPS3 3
#define CLKPS2 2
#define CLKPS1 1
#define CLKPS0 0

#define PRR    _SFR_MEM8(0x64)
#define PRADC       0
#define PRUSART0    1
#define PRSPI       2
#define PRTIM1      3
#define PRLCD       4

/* Oscillator Calibration Register */
#define OSCCAL _SFR_MEM8(0x66)

/* External Interrupt Control Register A */
#define EICRA  _SFR_MEM8(0x69)
#define ISC01  1
#define ISC00  0

/* Pin Change Mask Register */
#define PCMSK   _SFR_MEM16(0x6B)

#define PCMSK0  _SFR_MEM8(0x6B)
#define PCINT7  7
#define PCINT6  6
#define PCINT5  5
#define PCINT4  4
#define PCINT3  3
#define PCINT2  2
#define PCINT1  1
#define PCINT0  0

#define PCMSK1  _SFR_MEM8(0x6C)
#define PCINT15 7
#define PCINT14 6
#define PCINT13 5
#define PCINT12 4
#define PCINT11 3
#define PCINT10 2
#define PCINT9  1
#define PCINT8  0

/* Timer/Counter 0 Interrupt Mask Register */
#define TIMSK0 _SFR_MEM8(0x6E)
#define OCIE0A 1
#define TOIE0  0

/* Timer/Counter 1 Interrupt Mask Register */
#define TIMSK1 _SFR_MEM8(0x6F)
#define ICIE1  5
#define OCIE1B 2
#define OCIE1A 1
#define TOIE1  0

/* Timer/Counter 2 Interrupt Mask Register */
#define TIMSK2 _SFR_MEM8(0x70)
#define OCIE2A 1
#define TOIE2  0

/* ADC Data Register */
#ifndef __ASSEMBLER__
#define ADC    _SFR_MEM16(0x78)
#endif
#define ADCW   _SFR_MEM16(0x78)
#define ADCL   _SFR_MEM8(0x78)
#define ADCH   _SFR_MEM8(0x79)

/* ADC Control and Status Register A */
#define ADCSRA _SFR_MEM8(0x7A)
#define ADEN   7
#define ADSC   6
#define ADATE  5
#define ADIF   4
#define ADIE   3
#define ADPS2  2
#define ADPS1  1
#define ADPS0  0


/* ADC Control and Status Register B */
#define ADCSRB _SFR_MEM8(0x7B)
#define ACME   6
#define ADTS2  2
#define ADTS1  1
#define ADTS0  0

/* ADC Multiplex Selection Register */
#define ADMUX  _SFR_MEM8(0x7C)
/* ADMUX */
#define REFS1  7
#define REFS0  6
#define ADLAR  5
#define MUX4   4
#define MUX3   3
#define MUX2   2
#define MUX1   1
#define MUX0   0

/* Digital Input Disable Register 0 */
#define DIDR0  _SFR_MEM8(0x7E)
#define ADC7D  7
#define ADC6D  6
#define ADC5D  5
#define ADC4D  4
#define ADC3D  3
#define ADC2D  2
#define ADC1D  1
#define ADC0D  0

/* Digital Input Disable Register 1 */
#define DIDR1  _SFR_MEM8(0x7F)
#define AIN1D  1
#define AIN0D  0

/* Timer/Counter1 Control Register A */
#define TCCR1A _SFR_MEM8(0x80)
#define COM1A1	7
#define COM1A0	6
#define COM1B1	5
#define COM1B0	4
#define WGM11	1
#define WGM10	0

/* Timer/Counter1 Control Register B */
#define TCCR1B _SFR_MEM8(0x81)
#define ICNC1	7
#define ICES1	6
#define WGM13	4
#define WGM12	3
#define CS12	2
#define CS11	1
#define CS10	0

/* Timer/Counter1 Control Register C */
#define TCCR1C _SFR_MEM8(0x82)
#define FOC1A  7
#define FOC1B  6

/* Timer/Counter1 Register */
#define TCNT1  _SFR_MEM16(0x84)
#define TCNT1L _SFR_MEM8(0x84)
#define TCNT1H _SFR_MEM8(0x85)

/* Timer/Counter1 Input Capture Register */
#define ICR1   _SFR_MEM16(0x86)
#define ICR1L  _SFR_MEM8(0x86)
#define ICR1H  _SFR_MEM8(0x87)

/* Timer/Counter1 Output Compare Register A */
#define OCR1A  _SFR_MEM16(0x88)
#define OCR1AL _SFR_MEM8(0x88)
#define OCR1AH _SFR_MEM8(0x89)

/* Timer/Counter1 Output Compare Registare B */
#define OCR1B  _SFR_MEM16(0x8A)
#define OCR1BL _SFR_MEM8(0x8A)
#define OCR1BH _SFR_MEM8(0x8B)

/* Timer/Counter2 Control Register A */
#define TCCR2A _SFR_MEM8(0xB0)
#define FOC2A	7
#define WGM20	6
#define COM2A1	5
#define COM2A0	4
#define WGM21	3
#define CS22	2
#define CS21	1
#define CS20	0

/* Timer/Counter2 Register */
#define TCNT2  _SFR_MEM8(0xB2)

/* Timer/Counter2 Output Compare Register */
#define OCR2A  _SFR_MEM8(0xB3)

/* Asynchronous Status Register */
#define ASSR   _SFR_MEM8(0xB6)
#define EXCLK	4
#define AS2	3
#define TCN2UB	2
#define OCR2UB	1
#define TCR2UB	0

/* USI Control Register */
#define USICR  _SFR_MEM8(0xB8)
#define USISIE 7
#define USIOIE 6
#define USIWM1 5
#define USIWM0 4
#define USICS1 3
#define USICS0 2
#define USICLK 1
#define USITC  0

/* USI Status Register */
#define USISR  _SFR_MEM8(0xB9)
#define USISIF  7
#define USIOIF  6
#define USIPF   5
#define USIDC   4
#define USICNT3 3
#define USICNT2 2
#define USICNT1 1
#define USICNT0 0

/* USI Data Register */
#define USIDR  _SFR_MEM8(0xBA)

/* USART0 Control and Status Register A */
#define UCSR0A	_SFR_MEM8(0xC0)
#define RXC0	7
#define TXC0	6
#define UDRE0	5
#define FE0	4
#define DOR0	3
#define UPE0 	2
#define U2X0	1
#define MPCM0	0

/* USART0 Control and Status Register B */
#define UCSR0B	_SFR_MEM8(0xC1)
#define RXCIE0	7
#define TXCIE0	6
#define UDRIE0	5
#define RXEN0	4
#define TXEN0	3
#define UCSZ02	2
#define RXB80	1
#define TXB80	0

/* USART0 Control and Status Register C */
#define UCSR0C	_SFR_MEM8(0xC2)
#define UMSEL0	6
#define UPM01	5
#define UPM00	4
#define USBS0	3
#define UCSZ01	2
#define UCSZ00	1
#define UCPOL0	0

/* USART0 Baud Rate Register */
#define UBRR0	_SFR_MEM16(0xC4)
#define UBRR0L	_SFR_MEM8(0xC4)
#define UBRR0H	_SFR_MEM8(0xC5)

/* USART0 I/O Data Register */
#define UDR0	_SFR_MEM8(0xC6)

/* LCD Control and Status Register A */
#define LCDCRA	_SFR_MEM8(0xE4)
#define LCDEN	7
#define LCDAB	6
#define LCDIF	4
#define LCDIE	3
#define LCDBD	2
#define LCDCCD	1
#define LCDBL	0

/* LCD Control and Status Register B */
#define LCDCRB	_SFR_MEM8(0xE5)
#define LCDCS	7
#define LCD2B	6
#define LCDMUX1	5
#define LCDMUX0	4
#define LCDPM2	2
#define LCDPM1	1
#define LCDPM0	0

/* LCD Frame Rate Register */
#define LCDFRR	_SFR_MEM8(0xE6)
#define LCDPS2	6
#define LCDPS1	5
#define LCDPS0	4
#define LCDCD2	2
#define LCDCD1	1
#define LCDCD0	0

/* LCD Contrast Control Register */
#define LCDCCR	_SFR_MEM8(0xE7)
#define LCDDC2	7
#define LCDDC1	6
#define LCDDC0	5
#define LCDMDT	4
#define LCDCC3	3
#define LCDCC2	2
#define LCDCC1	1
#define LCDCC0	0

/* LCD Memory mapping */
#define LCDDR0 _SFR_MEM8(0xEC)
#define SEG007 7
#define SEG006 6
#define SEG005 5
#define SEG004 4
#define SEG003 3
#define SEG002 2
#define SEG001 1
#define SEG000 0

#define LCDDR1 _SFR_MEM8(0xED)
#define SEG015 7
#define SEG014 6
#define SEG013 5
#define SEG012 4
#define SEG011 3
#define SEG010 2
#define SEG009 1
#define SEG008 0

#define LCDDR2 _SFR_MEM8(0xEE)
#define SEG023 7
#define SEG022 6
#define SEG021 5
#define SEG020 4
#define SEG019 3
#define SEG018 2
#define SEG017 1
#define SEG016 0

#define LCDDR3 _SFR_MEM8(0xEF)
#define SEG024 0

#define LCDDR5 _SFR_MEM8(0xF1)
#define SEG107 7
#define SEG106 6
#define SEG105 5
#define SEG104 4
#define SEG103 3
#define SEG102 2
#define SEG101 1
#define SEG100 0

#define LCDDR6 _SFR_MEM8(0xF2)
#define SEG115 7
#define SEG114 6
#define SEG113 5
#define SEG112 4
#define SEG111 3
#define SEG110 2
#define SEG109 1
#define SEG108 0

#define LCDDR7 _SFR_MEM8(0xF3)
#define SEG123 7
#define SEG122 6
#define SEG121 5
#define SEG120 4
#define SEG119 3
#define SEG118 2
#define SEG117 1
#define SEG116 0

#define LCDDR8 _SFR_MEM8(0xF4)
#define SEG124 0

#define LCDDR10 _SFR_MEM8(0xF6)
#define SEG207 7
#define SEG206 6
#define SEG205 5
#define SEG204 4
#define SEG203 3
#define SEG202 2
#define SEG201 1
#define SEG200 0

#define LCDDR11 _SFR_MEM8(0xF7)
#define SEG215 7
#define SEG214 6
#define SEG213 5
#define SEG212 4
#define SEG211 3
#define SEG210 2
#define SEG209 1
#define SEG208 0

#define LCDDR12 _SFR_MEM8(0xF8)
#define SEG223 7
#define SEG222 6
#define SEG221 5
#define SEG220 4
#define SEG219 3
#define SEG218 2
#define SEG217 1
#define SEG216 0

#define LCDDR13 _SFR_MEM8(0xF9)
#define SEG224 0

#define LCDDR15 _SFR_MEM8(0xFB)
#define SEG307 7
#define SEG306 6
#define SEG305 5
#define SEG304 4
#define SEG303 3
#define SEG302 2
#define SEG301 1
#define SEG300 0

#define LCDDR16 _SFR_MEM8(0xFC)
#define SEG315 7
#define SEG314 6
#define SEG313 5
#define SEG312 4
#define SEG311 3
#define SEG310 2
#define SEG309 1
#define SEG308 0

#define LCDDR17 _SFR_MEM8(0xFD)
#define SEG323 7
#define SEG322 6
#define SEG321 5
#define SEG320 4
#define SEG319 3
#define SEG318 2
#define SEG317 1
#define SEG316 0

#define LCDDR18 _SFR_MEM8(0xFE)
#define SEG324 0

/* LCDDR0-18 */
#define SEG24  0

#define SEG23  7
#define SEG22  6
#define SEG21  5
#define SEG20  4
#define SEG19  3
#define SEG18  2
#define SEG17  1
#define SEG16  0

#define SEG15  7
#define SEG14  6
#define SEG13  5
#define SEG12  4
#define SEG11  3
#define SEG10  2
#define SEG9   1
#define SEG8   0

#define SEG7   7
#define SEG6   6
#define SEG5   5
#define SEG4   4
#define SEG3   3
#define SEG2   2
#define SEG1   1
#define SEG0   0

/* Interrupt vectors */

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* Pin Change Interrupt Request 0 */
#define PCINT0_vect			_VECTOR(2)
#define SIG_PIN_CHANGE0			_VECTOR(2)

/* Pin Change Interrupt Request 1 */
#define PCINT1_vect			_VECTOR(3)
#define SIG_PIN_CHANGE1			_VECTOR(3)

/* Timer/Counter2 Compare Match */
#define TIMER2_COMP_vect		_VECTOR(4)
#define SIG_OUTPUT_COMPARE2		_VECTOR(4)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(5)
#define SIG_OVERFLOW2			_VECTOR(5)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(6)
#define SIG_INPUT_CAPTURE1		_VECTOR(6)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(7)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(7)

/* Timer/Counter Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(8)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(8)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(9)
#define SIG_OVERFLOW1			_VECTOR(9)

/* Timer/Counter0 Compare Match */
#define TIMER0_COMP_vect		_VECTOR(10)
#define SIG_OUTPUT_COMPARE0		_VECTOR(10)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(11)
#define SIG_OVERFLOW0			_VECTOR(11)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(12)
#define SIG_SPI				_VECTOR(12)

/* USART0, Rx Complete */
#define USART0_RX_vect			_VECTOR(13)
#define SIG_USART_RECV			_VECTOR(13)

/* USART0 Data register Empty */
#define USART0_UDRE_vect		_VECTOR(14)
#define SIG_USART_DATA			_VECTOR(14)

/* USART0, Tx Complete */
#define USART0_TX_vect			_VECTOR(15)
#define SIG_USART_TRANS			_VECTOR(15)

/* USI Start Condition */
#define USI_START_vect			_VECTOR(16)
#define SIG_USI_START			_VECTOR(16)

/* USI Overflow */
#define USI_OVERFLOW_vect		_VECTOR(17)
#define SIG_USI_OVERFLOW		_VECTOR(17)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(18)
#define SIG_COMPARATOR			_VECTOR(18)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(19)
#define SIG_ADC				_VECTOR(19)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(20)
#define SIG_EEPROM_READY		_VECTOR(20)

/* Store Program Memory Read */
#define SPM_READY_vect			_VECTOR(21)
#define SIG_SPM_READY			_VECTOR(21)

/* LCD Start of Frame */
#define LCD_vect			_VECTOR(22)
#define SIG_LCD				_VECTOR(22)

#define _VECTORS_SIZE 92

/* Constants */
#define SPM_PAGESIZE 128
#define RAMEND		0x4FF
#define XRAMEND		0x4FF
#define E2END		0x1FF
#define E2PAGESIZE  4
#define FLASHEND	0x3FFF


/* Fuses */

#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_CKSEL2      (unsigned char)~_BV(2)
#define FUSE_CKSEL3      (unsigned char)~_BV(3)
#define FUSE_SUT0        (unsigned char)~_BV(4)
#define FUSE_SUT1        (unsigned char)~_BV(5)
#define FUSE_CKOUT       (unsigned char)~_BV(6)
#define FUSE_CKDIV8      (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0 & FUSE_CKDIV8)

/* High Fuse Byte */
#define FUSE_BOOTRST     (unsigned char)~_BV(0)
#define FUSE_BOOTSZ0     (unsigned char)~_BV(1)
#define FUSE_BOOTSZ1     (unsigned char)~_BV(2)
#define FUSE_EESAVE      (unsigned char)~_BV(3)
#define FUSE_WDTON       (unsigned char)~_BV(4)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_JTAGEN      (unsigned char)~_BV(6)
#define FUSE_OCDEN       (unsigned char)~_BV(7)
#define HFUSE_DEFAULT (FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_SPIEN & FUSE_JTAGEN)

/* Extended Fuse Byte */
#define FUSE_BODLEVEL0   (unsigned char)~_BV(1)
#define FUSE_BODLEVEL1   (unsigned char)~_BV(2)
#define FUSE_BODLEVEL2   (unsigned char)~_BV(3)
#define EFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x94
#define SIGNATURE_2 0x05


#endif  /* _AVR_IOM169P_H_ */
