/* Copyright (c) 2002, 2003, 2004, 2005 
   Juergen Schilling <juergen.schilling@honeywell.com>
   Eric B. Weddington
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

/* $Id: iom169.h,v 1.26.2.5 2008/10/17 23:27:48 arcanum Exp $ */

/* iom169.h - definitions for ATmega169 */

/* This should be up to date with data sheet version 2514J-AVR-12/03. */

#ifndef _AVR_IOM169_H_
#define _AVR_IOM169_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom169.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* Port A */
#define PINA   _SFR_IO8(0x00)
#define DDRA   _SFR_IO8(0x01)
#define PORTA  _SFR_IO8(0x02)

/* Port B */
#define PINB   _SFR_IO8(0x03)
#define DDRB   _SFR_IO8(0x04)
#define PORTB  _SFR_IO8(0x05)

/* Port C */
#define PINC   _SFR_IO8(0x06)
#define DDRC   _SFR_IO8(0x07)
#define PORTC  _SFR_IO8(0x08)

/* Port D */
#define PIND   _SFR_IO8(0x09)
#define DDRD   _SFR_IO8(0x0A)
#define PORTD  _SFR_IO8(0x0B)

/* Port E */
#define PINE   _SFR_IO8(0x0C)
#define DDRE   _SFR_IO8(0x0D)
#define PORTE  _SFR_IO8(0x0E)

/* Port F */
#define PINF   _SFR_IO8(0x0F)
#define DDRF   _SFR_IO8(0x10)
#define PORTF  _SFR_IO8(0x11)

/* Port G */
#define PING   _SFR_IO8(0x12)
#define DDRG   _SFR_IO8(0x13)
#define PORTG  _SFR_IO8(0x14)

/* Timer/Counter 0 interrupt Flag Register */
#define TIFR0  _SFR_IO8(0x15)

/* Timer/Counter 1 interrupt Flag Register */
#define TIFR1  _SFR_IO8(0x16)

/* Timer/Counter 2 interrupt Flag Register */
#define TIFR2  _SFR_IO8(0x17)

/* External Interrupt Flag Register */
#define EIFR   _SFR_IO8(0x1C)

/* External Interrupt Mask Register */
#define EIMSK  _SFR_IO8(0x1D)

/* General Purpose I/O Register 0 */
#define GPIOR0 _SFR_IO8(0x1E)

#define EECR   _SFR_IO8(0x1F)

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

/* Timer/Counter Control Register A */
#define TCCR0A _SFR_IO8(0x24)

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

/* SPI Status Register */
#define SPSR   _SFR_IO8(0x2D)

/* SPI Data Register */
#define SPDR   _SFR_IO8(0x2E)

/* Analog Comperator Control and Status Register */
#define ACSR   _SFR_IO8(0x30)

/* On-chip Debug Register */
#define OCDR   _SFR_IO8(0x31)

/* Sleep Mode Control Register */
#define SMCR   _SFR_IO8(0x33)

/* MCU Status Register */
#define MCUSR  _SFR_IO8(0x34)

/* MCU Control Rgeister */
#define MCUCR  _SFR_IO8(0x35)

/* Store Program Memory Control and Status Register */
#define SPMCSR _SFR_IO8(0x37)

/* Watchdog Timer Control Register */
#define WDTCR  _SFR_MEM8(0x60)

/* Clock Prescale Register */
#define CLKPR  _SFR_MEM8(0x61)

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

/* Pin Change Mask Register */
#define PCMSK  _SFR_MEM16(0x6B)
#define PCMSK0 _SFR_MEM8(0x6B)
#define PCMSK1 _SFR_MEM8(0x6C)

/* Timer/Counter 0 Interrupt Mask Register */
#define TIMSK0 _SFR_MEM8(0x6E)

/* Timer/Counter 1 Interrupt Mask Register */
#define TIMSK1 _SFR_MEM8(0x6F)

/* Timer/Counter 2 Interrupt Mask Register */
#define TIMSK2 _SFR_MEM8(0x70)

/* ADC Data Register */
#ifndef __ASSEMBLER__
#define ADC    _SFR_MEM16(0x78)
#endif
#define ADCW   _SFR_MEM16(0x78)
#define ADCL   _SFR_MEM8(0x78)
#define ADCH   _SFR_MEM8(0x79)

/* ADC Control and Status Register A */
#define ADCSRA _SFR_MEM8(0x7A)

/* ADC Control and Status Register B */
#define ADCSRB _SFR_MEM8(0x7B)

/* ADC Multiplex Selection Register */
#define ADMUX  _SFR_MEM8(0x7C)

/* NOTE: DIDR0 and DIDR1 are swapped in the register summary of the data sheet
   (2514D-AVR-01/03), but seem to be correct in the discussions of the
   registers. */

/* Digital Input Disable Register 0 */
#define DIDR0  _SFR_MEM8(0x7E)

/* Digital Input Disable Register 1 */
#define DIDR1  _SFR_MEM8(0x7F)

/* Timer/Counter1 Control Register A */
#define TCCR1A _SFR_MEM8(0x80)

/* Timer/Counter1 Control Register B */
#define TCCR1B _SFR_MEM8(0x81)

/* Timer/Counter1 Control Register C */
#define TCCR1C _SFR_MEM8(0x82)

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

/* Timer/Counter2 Register */
#define TCNT2  _SFR_MEM8(0xB2)

/* Timer/Counter2 Output Compare Register */
#define OCR2A  _SFR_MEM8(0xB3)

/* Asynchronous Status Register */
#define ASSR   _SFR_MEM8(0xB6)

/* USI Control Register */
#define USICR  _SFR_MEM8(0xB8)

/* USI Status Register */
#define USISR  _SFR_MEM8(0xB9)

/* USI Data Register */
#define USIDR  _SFR_MEM8(0xBA)

/* USART0 Control and Status Register A */
#define UCSRA  _SFR_MEM8(0xC0)

/* USART0 Control and Status Register B */
#define UCSRB  _SFR_MEM8(0xC1)

/* USART0 Control and Status Register C */
#define UCSRC  _SFR_MEM8(0xC2)

/* USART0 Baud Rate Register */
#define UBRR   _SFR_MEM16(0xC4)
#define UBRRL  _SFR_MEM8(0xC4)
#define UBRRH  _SFR_MEM8(0xC5)

/* USART0 I/O Data Register */
#define UDR    _SFR_MEM8(0xC6)

/* LCD Control and Status Register A */
#define LCDCRA _SFR_MEM8(0xE4)

/* LCD Control and Status Register B */
#define LCDCRB _SFR_MEM8(0xE5)

/* LCD Frame Rate Register */
#define LCDFRR _SFR_MEM8(0xE6)

/* LCD Contrast Control Register */
#define LCDCCR _SFR_MEM8(0xE7)

/* LCD Memory mapping */
#define LCDDR0 _SFR_MEM8(0xEC)
#define LCDDR1 _SFR_MEM8(0xED)
#define LCDDR2 _SFR_MEM8(0xEE)
#define LCDDR3 _SFR_MEM8(0xEF)
#define LCDDR5 _SFR_MEM8(0xF1)
#define LCDDR6 _SFR_MEM8(0xF2)
#define LCDDR7 _SFR_MEM8(0xF3)
#define LCDDR8 _SFR_MEM8(0xF4)
#define LCDDR10 _SFR_MEM8(0xF6)
#define LCDDR11 _SFR_MEM8(0xF7)
#define LCDDR12 _SFR_MEM8(0xF8)
#define LCDDR13 _SFR_MEM8(0xF9)
#define LCDDR15 _SFR_MEM8(0xFB)
#define LCDDR16 _SFR_MEM8(0xFC)
#define LCDDR17 _SFR_MEM8(0xFD)
#define LCDDR18 _SFR_MEM8(0xFE)

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

/* Bit numbers */

/*
   PA7 = SEG3
   PA6 = SEG2
   PA5 = SEG1
   PA4 = SEG0
   PA3 = COM3
   PA2 = COM2
   PA1 = COM1
   PA0 = COM0
*/

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

/* PINA */
#define PINA7	7
#define PINA6	6
#define PINA5	5
#define PINA4	4
#define PINA3	3
#define PINA2	2
#define PINA1	1
#define PINA0	0

/*
   PB7 = OC2A / PCINT15
   PB6 = OC1B / PCINT14
   PB5 = OC1A / PCINT13
   PB4 = OC0A / PCINT12
   PB3 = MISO / PCINT11
   PB2 = MOSI / PCINT10
   PB1 = SCK / PCINT9
   PB0 = SS# / PCINT8
 */

/* PORTB */
#define PB7	7
#define PB6	6
#define PB5	5
#define PB4	4
#define PB3	3
#define PB2	2
#define PB1	1
#define PB0	0

/* DDRB */
#define DDB7	7
#define DDB6	6
#define DDB5	5
#define DDB4	4
#define DDB3	3
#define DDB2	2
#define DDB1	1
#define DDB0	0

/* PINB */
#define PINB7	7
#define PINB6	6
#define PINB5	5
#define PINB4	4
#define PINB3	3
#define PINB2	2
#define PINB1	1
#define PINB0	0

/*
   PC7 = SEG5
   PC6 = SEG6
   PC5 = SEG7
   PC4 = SEG8
   PC3 = SEG9
   PC2 = SEG10
   PC1 = SEG11
   PC0 = SEG12
*/

/* PORTC */
#define PC7	 7
#define PC6	 6
#define PC5	 5
#define PC4	 4
#define PC3	 3
#define PC2	 2
#define PC1	 1
#define PC0	 0

/* DDRC */
#define DDC7	7
#define DDC6	6
#define DDC5	5
#define DDC4	4
#define DDC3	3
#define DDC2	2
#define DDC1	1
#define DDC0	0

/* PINC */
#define PINC7	7
#define PINC6	6
#define PINC5	5
#define PINC4	4
#define PINC3	3
#define PINC2	2
#define PINC1	1
#define PINC0	0

/*
   PD7 = SEG15
   PD6 = SEG16
   PD5 = SEG17
   PD4 = SEG18
   PD3 = SEG19
   PD2 = SEG20
   PD1 = INT0 / SEG21
   PD0 = ICP / SEG22
 */

/* PORTD */
#define PD7	 7
#define PD6	 6
#define PD5	 5
#define PD4	 4
#define PD3	 3
#define PD2	 2
#define PD1	 1
#define PD0	 0

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

/*
   PE7 = CLK0 / PCINT7
   PE6 = DO / PCINT6
   PE5 = DI / SDA / PCINT5
   PE4 = USCK / SCL / PCINT4
   PE3 = AIN1 / PCINT3
   PE2 = XCK / AIN0 / PCINT2
   PE1 = TXD / PCINT1
   PE0 = RXD / PCINT0
 */

/* PORTE */
#define PE7 7
#define PE6 6
#define PE5 5
#define PE4 4
#define PE3 3
#define PE2 2
#define PE1 1
#define PE0 0

/* DDRE */
#define DDE7	7
#define DDE6	6
#define DDE5	5
#define DDE4	4
#define DDE3	3
#define DDE2	2
#define DDE1	1
#define DDE0	0

/* PINE */
#define PINE7	7
#define PINE6	6
#define PINE5	5
#define PINE4	4
#define PINE3	3
#define PINE2	2
#define PINE1	1
#define PINE0	0

/*
   PF7 = ADC7 / TDI
   PF6 = ADC6 / TDO
   PF5 = ADC5 / TMS
   PF4 = ADC4 / TCK
   PF3 = ADC3
   PF2 = ADC2
   PF1 = ADC1
   PF0 = ADC0
 */

/* PORTF */
#define PF7 7
#define PF6 6
#define PF5 5
#define PF4 4
#define PF3 3
#define PF2 2
#define PF1 1
#define PF0 0

/* DDRF */
#define DDF7	7
#define DDF6	6
#define DDF5	5
#define DDF4	4
#define DDF3	3
#define DDF2	2
#define DDF1	1
#define DDF0	0

/* PINF */
#define PINF7	7
#define PINF6	6
#define PINF5	5
#define PINF4	4
#define PINF3	3
#define PINF2	2
#define PINF1	1
#define PINF0	0

/*
   PG5 = RESET#
   PG4 = T0 / SEG23
   PG3 = T1 / SEG24
   PG2 = SEG4
   PG1 = SEG13
   PG0 = SEG14
 */

/* PORTG */
#define PG4 4
#define PG3 3
#define PG2 2
#define PG1 1
#define PG0 0

/* DDRG */
#define DDG4	4
#define DDG3	3
#define DDG2	2
#define DDG1	1
#define DDG0	0

/* PING */
#define PING5	5
#define PING4	4
#define PING3	3
#define PING2	2
#define PING1	1
#define PING0	0

/* TIFR0 */
#define OCF0A	1
#define TOV0	0

/* TIFR1 */
#define ICF1   5
#define OCF1B  2
#define OCF1A	1
#define TOV1	0

/* TIFR2 */
#define OCF2A	1
#define TOV2	0

/* EIFR */
#define PCIF1  7
#define PCIF0  6
#define INTF0  0

/* EIMSK */
#define PCIE1  7
#define PCIE0  6
#define INT0   0

/* EECR */
#define EERIE   3
#define EEMWE   2
#define EEWE    1
#define EERE    0

/* GTCCR */
#define TSM    7
#define PSR2   1
#define PSR10  0

/* TCCR0A */
#define FOC0A	7
#define WGM00	6
#define COM0A1	5
#define COM0A0	4
#define WGM01	3
#define CS02	2
#define CS01	1
#define CS00	0

/* SPCR */
#define SPIE	7
#define SPE	   6
#define DORD	5
#define MSTR	4
#define CPOL	3
#define CPHA	2
#define SPR1	1
#define SPR0	0

/* SPSR */
#define SPIF	7
#define WCOL	6
#define SPI2X	0

/* ACSR */
#define ACD	   7
#define ACBG	6
#define ACO	   5
#define ACI	   4
#define ACIE	3
#define ACIC	2
#define ACIS1	1
#define ACIS0	0

/* OCDR */
#define IDRD   7
#define OCD    7
#define OCDR6  6
#define OCDR5  5
#define OCDR4  4
#define OCDR3  3
#define OCDR2  2
#define OCDR1  1
#define OCDR0  0

/* SMCR */
#define SM2    3
#define SM1    2
#define SM0    1
#define SE     0

/* MCUSR */
#define JTRF   4
#define WDRF   3
#define BORF   2
#define EXTRF  1
#define PORF   0

/* MCUCR */
#define JTD    7
#define PUD    4
#define IVSEL  1
#define IVCE   0

/* SPMCSR */
#define SPMIE  7
#define RWWSB  6
#define RWWSRE 4
#define BLBSET 3
#define PGWRT  2
#define PGERS  1
#define SPMEN  0

/* WDTCR */
#define WDCE	4
#define WDE	   3
#define WDP2	2
#define WDP1	1
#define WDP0	0

/* CLKPR */
#define CLKPCE 7
#define CLKPS3 3
#define CLKPS2 2
#define CLKPS1 1
#define CLKPS0 0

/* EICRA */
#define ISC01  1
#define ISC00  0

/* PCMSK0 */
#define PCINT7 7
#define PCINT6 6
#define PCINT5 5
#define PCINT4 4
#define PCINT3 3
#define PCINT2 2
#define PCINT1 1
#define PCINT0 0

/* PCMSK1 */
#define PCINT15 7
#define PCINT14 6
#define PCINT13 5
#define PCINT12 4
#define PCINT11 3
#define PCINT10 2
#define PCINT9 1
#define PCINT8 0

/* TIMSK0 */
#define OCIE0A 1
#define TOIE0  0

/* TIMSK1 */
#define ICIE1  5
#define OCIE1B 2
#define OCIE1A 1
#define TOIE1  0

/* TIMSK2 */
#define OCIE2A 1
#define TOIE2  0

/* ADCSRA */
#define ADEN   7
#define ADSC   6
#define ADATE  5
#define ADIF   4
#define ADIE   3
#define ADPS2  2
#define ADPS1  1
#define ADPS0  0

/* ADCSRB */
#define ACME   6
#define ADTS2  2
#define ADTS1  1
#define ADTS0  0

/* ADMUX */
#define REFS1  7
#define REFS0  6
#define ADLAR  5
#define MUX4   4
#define MUX3   3
#define MUX2   2
#define MUX1   1
#define MUX0   0

/* DIDR1 */
#define AIN1D  1
#define AIN0D  0

/* DIDR0 */
#define ADC7D  7
#define ADC6D  6
#define ADC5D  5
#define ADC4D  4
#define ADC3D  3
#define ADC2D  2
#define ADC1D  1
#define ADC0D  0

/* TCCR1A */
#define COM1A1	7
#define COM1A0	6
#define COM1B1	5
#define COM1B0	4
#define WGM11	1
#define WGM10	0

/* TCCR1B */
#define ICNC1	7
#define ICES1	6
#define WGM13  4
#define WGM12	3
#define CS12	2
#define CS11	1
#define CS10	0

/* TCCR1C */
#define FOC1A  7
#define FOC1B  6

/* TCCR2A */
#define FOC2A	7
#define WGM20	6
#define COM2A1	5
#define COM2A0	4
#define WGM21	3
#define CS22	2
#define CS21	1
#define CS20	0

/* ASSR */
#define EXCLK  4
#define AS2	   3
#define TCN2UB	2
#define OCR2UB	1
#define TCR2UB	0

/* USICR */
#define USISIE 7
#define USIOIE 6
#define USIWM1 5
#define USIWM0 4
#define USICS1 3
#define USICS0 2
#define USICLK 1
#define USITC  0

/* USISR */
#define USISIF 7
#define USIOIF 6
#define USIPF  5
#define USIDC  4
#define USICNT3 3
#define USICNT2 2
#define USICNT1 1
#define USICNT0 0

/* UCSRA */
#define RXC	7
#define TXC	6
#define UDRE	5
#define FE	4
#define DOR	3
#define UPE 	2
#define U2X     1
#define MPCM	0

/* UCSRB */
#define RXCIE	7
#define TXCIE	6
#define UDRIE	5
#define RXEN	4
#define TXEN	3
#define UCSZ2   2
#define RXB8	1
#define TXB8	0

/* UCSRC */
#define UMSEL   6
#define UPM1    5
#define UPM0    4
#define USBS    3
#define UCSZ1   2
#define UCSZ0   1
#define UCPOL   0

/* LCDCRA */
#define LCDEN  7
#define LCDAB  6
#define LCDIF  4
#define LCDIE  3
#define LCDBD  2 /* Only in Rev. F */
#define LCDCCD 1 /* Only in Rev. F */
#define LCDBL  0

/* LCDCRB */
#define LCDCS  7
#define LCD2B  6
#define LCDMUX1 5
#define LCDMUX0 4
#define LCDPM2 2
#define LCDPM1 1
#define LCDPM0 0

/* LCDFRR */
#define LCDPS2 6
#define LCDPS1 5
#define LCDPS0 4
#define LCDCD2 2
#define LCDCD1 1
#define LCDCD0 0

/* LCDCCR */
#define LCDDC2 7
#define LCDDC1 6
#define LCDDC0 5
#define LCDMDT 4 /* Only in Rev. F */
#define LCDCC3 3
#define LCDCC2 2
#define LCDCC1 1
#define LCDCC0 0

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


#endif  /* _AVR_IOM169_H_ */
