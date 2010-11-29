/* Copyright (c) 2004,2005,2006 Colin O'Flynn <coflynn@newae.com>
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

/* $Id: iocanxx.h,v 1.7 2007/10/25 20:33:23 aesok Exp $ */

/* This file is based largely on:
   - iom128.h by Peter Jansen (bit defines)
   - iom169.h by Juergen Schilling <juergen.schilling@honeywell.com>
     (register addresses)
   - AT90CAN128 Datasheet (bit defines and register addresses)
   - Appnote on Mega128 --> AT90Can128 Conversion (for what registers I need
     to change) */

/* iocanxx.h - definitions for AT90CAN32, AT90CAN64 and AT90CAN128 */

#ifndef _AVR_IOCANXX_H_
#define _AVR_IOCANXX_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iocanxx.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers and bit definitions. */

/* RegDef:  Port A */
#define PINA   _SFR_IO8(0x00)
#define DDRA   _SFR_IO8(0x01)
#define PORTA  _SFR_IO8(0x02)

/* RegDef:  Port B */
#define PINB   _SFR_IO8(0x03)
#define DDRB   _SFR_IO8(0x04)
#define PORTB  _SFR_IO8(0x05)

/* RegDef:  Port C */
#define PINC   _SFR_IO8(0x06)
#define DDRC   _SFR_IO8(0x07)
#define PORTC  _SFR_IO8(0x08)

/* RegDef:  Port D */
#define PIND   _SFR_IO8(0x09)
#define DDRD   _SFR_IO8(0x0A)
#define PORTD  _SFR_IO8(0x0B)

/* RegDef:  Port E */
#define PINE   _SFR_IO8(0x0C)
#define DDRE   _SFR_IO8(0x0D)
#define PORTE  _SFR_IO8(0x0E)

/* RegDef:  Port F */
#define PINF   _SFR_IO8(0x0F)
#define DDRF   _SFR_IO8(0x10)
#define PORTF  _SFR_IO8(0x11)

/* RegDef:  Port G */
#define PING   _SFR_IO8(0x12)
#define DDRG   _SFR_IO8(0x13)
#define PORTG  _SFR_IO8(0x14)

/* RegDef:  Timer/Counter 0 interrupt Flag Register */
#define TIFR0  _SFR_IO8(0x15)

/* RegDef:  Timer/Counter 1 interrupt Flag Register */
#define TIFR1  _SFR_IO8(0x16)

/* RegDef:  Timer/Counter 2 interrupt Flag Register */
#define TIFR2  _SFR_IO8(0x17)

/* RegDef:  Timer/Counter 3 interrupt Flag Register */
#define TIFR3  _SFR_IO8(0x18)

/* RegDef:  External Interrupt Flag Register */
#define EIFR   _SFR_IO8(0x1C)

/* RegDef:  External Interrupt Mask Register */
#define EIMSK  _SFR_IO8(0x1D)

/* RegDef:  General Purpose I/O Register 0 */
#define GPIOR0 _SFR_IO8(0x1E)

/* RegDef:  EEPROM Control Register */
#define EECR   _SFR_IO8(0x1F)

/* RegDef:  EEPROM Data Register */
#define EEDR   _SFR_IO8(0x20)

/* RegDef:  EEPROM Address Register */
#define EEAR   _SFR_IO16(0x21)
#define EEARL  _SFR_IO8(0x21)
#define EEARH  _SFR_IO8(0x22)

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

/* RegDef:  General Timer/Counter Control Register */
#define GTCCR  _SFR_IO8(0x23)

/* RegDef:  Timer/Counter Control Register A */
#define TCCR0A _SFR_IO8(0x24)

/* RegDef:  Timer/Counter Register */
#define TCNT0  _SFR_IO8(0x26)

/* RegDef:  Output Compare Register A */
#define OCR0A  _SFR_IO8(0x27)

/* RegDef:  General Purpose I/O Register 1 */
#define GPIOR1 _SFR_IO8(0x2A)

/* RegDef:  General Purpose I/O Register 2 */
#define GPIOR2 _SFR_IO8(0x2B)

/* RegDef:  SPI Control Register */
#define SPCR   _SFR_IO8(0x2C)

/* RegDef:  SPI Status Register */
#define SPSR   _SFR_IO8(0x2D)

/* RegDef:  SPI Data Register */
#define SPDR   _SFR_IO8(0x2E)

/* RegDef:  Analog Comperator Control and Status Register */
#define ACSR   _SFR_IO8(0x30)

/* RegDef:  On-chip Debug Register */
#define OCDR   _SFR_IO8(0x31)

/* RegDef:  Sleep Mode Control Register */
#define SMCR   _SFR_IO8(0x33)

/* RegDef:  MCU Status Register */
#define MCUSR  _SFR_IO8(0x34)

/* RegDef:  MCU Control Rgeister */
#define MCUCR  _SFR_IO8(0x35)

/* RegDef:  Store Program Memory Control and Status Register */
#define SPMCSR _SFR_IO8(0x37)

/* RegDef:  RAMPZ register. */
#define RAMPZ  _SFR_IO8(0x3B)

/* RegDef:  Watchdog Timer Control Register */
#define WDTCR  _SFR_MEM8(0x60)

/* RegDef:  Clock Prescale Register */
#define CLKPR  _SFR_MEM8(0x61)

/* RegDef:  Oscillator Calibration Register */
#define OSCCAL _SFR_MEM8(0x66)

/* RegDef:  External Interrupt Control Register A */
#define EICRA  _SFR_MEM8(0x69)

/* RegDef:  External Interrupt Control Register B */
#define EICRB  _SFR_MEM8(0x6A)

/* RegDef:  Timer/Counter 0 Interrupt Mask Register */
#define TIMSK0 _SFR_MEM8(0x6E)

/* RegDef:  Timer/Counter 1 Interrupt Mask Register */
#define TIMSK1 _SFR_MEM8(0x6F)

/* RegDef:  Timer/Counter 2 Interrupt Mask Register */
#define TIMSK2 _SFR_MEM8(0x70)

/* RegDef:  Timer/Counter 3 Interrupt Mask Register */
#define TIMSK3 _SFR_MEM8(0x71)

/* RegDef:  External Memory Control Register A */
#define XMCRA _SFR_MEM8(0x74)

/* RegDef:  External Memory Control Register A */
#define XMCRB _SFR_MEM8(0x75)

/* RegDef:  ADC Data Register */
#ifndef __ASSEMBLER__
#define ADC    _SFR_MEM16(0x78)
#endif
#define ADCW   _SFR_MEM16(0x78)
#define ADCL   _SFR_MEM8(0x78)
#define ADCH   _SFR_MEM8(0x79)

/* RegDef:  ADC Control and Status Register A */
#define ADCSRA _SFR_MEM8(0x7A)

/* RegDef:  ADC Control and Status Register B */
#define ADCSRB _SFR_MEM8(0x7B)

/* RegDef:  ADC Multiplex Selection Register */
#define ADMUX  _SFR_MEM8(0x7C)

/* RegDef:  Digital Input Disable Register 0 */
#define DIDR0  _SFR_MEM8(0x7E)

/* RegDef:  Digital Input Disable Register 1 */
#define DIDR1  _SFR_MEM8(0x7F)

/* RegDef:  Timer/Counter1 Control Register A */
#define TCCR1A _SFR_MEM8(0x80)

/* RegDef:  Timer/Counter1 Control Register B */
#define TCCR1B _SFR_MEM8(0x81)

/* RegDef:  Timer/Counter1 Control Register C */
#define TCCR1C _SFR_MEM8(0x82)

/* RegDef:  Timer/Counter1 Register */
#define TCNT1  _SFR_MEM16(0x84)
#define TCNT1L _SFR_MEM8(0x84)
#define TCNT1H _SFR_MEM8(0x85)

/* RegDef:  Timer/Counter1 Input Capture Register */
#define ICR1   _SFR_MEM16(0x86)
#define ICR1L  _SFR_MEM8(0x86)
#define ICR1H  _SFR_MEM8(0x87)

/* RegDef:  Timer/Counter1 Output Compare Register A */
#define OCR1A  _SFR_MEM16(0x88)
#define OCR1AL _SFR_MEM8(0x88)
#define OCR1AH _SFR_MEM8(0x89)

/* RegDef:  Timer/Counter1 Output Compare Register B */
#define OCR1B  _SFR_MEM16(0x8A)
#define OCR1BL _SFR_MEM8(0x8A)
#define OCR1BH _SFR_MEM8(0x8B)

/* RegDef:  Timer/Counter1 Output Compare Register C */
#define OCR1C  _SFR_MEM16(0x8C)
#define OCR1CL _SFR_MEM8(0x8C)
#define OCR1CH _SFR_MEM8(0x8D)

/* RegDef:  Timer/Counter3 Control Register A */
#define TCCR3A _SFR_MEM8(0x90)

/* RegDef:  Timer/Counter3 Control Register B */
#define TCCR3B _SFR_MEM8(0x91)

/* RegDef:  Timer/Counter3 Control Register C */
#define TCCR3C _SFR_MEM8(0x92)

/* RegDef:  Timer/Counter3 Register */
#define TCNT3  _SFR_MEM16(0x94)
#define TCNT3L _SFR_MEM8(0x94)
#define TCNT3H _SFR_MEM8(0x95)

/* RegDef:  Timer/Counter3 Input Capture Register */
#define ICR3   _SFR_MEM16(0x96)
#define ICR3L  _SFR_MEM8(0x96)
#define ICR3H  _SFR_MEM8(0x97)

/* RegDef:  Timer/Counter3 Output Compare Register A */
#define OCR3A  _SFR_MEM16(0x98)
#define OCR3AL _SFR_MEM8(0x98)
#define OCR3AH _SFR_MEM8(0x99)

/* RegDef:  Timer/Counter3 Output Compare Register B */
#define OCR3B  _SFR_MEM16(0x9A)
#define OCR3BL _SFR_MEM8(0x9A)
#define OCR3BH _SFR_MEM8(0x9B)

/* RegDef:  Timer/Counter3 Output Compare Register C */
#define OCR3C  _SFR_MEM16(0x9C)
#define OCR3CL _SFR_MEM8(0x9C)
#define OCR3CH _SFR_MEM8(0x9D)

/* RegDef:  Timer/Counter2 Control Register A */
#define TCCR2A _SFR_MEM8(0xB0)

/* RegDef:  Timer/Counter2 Register */
#define TCNT2  _SFR_MEM8(0xB2)

/* RegDef:  Timer/Counter2 Output Compare Register */
#define OCR2A  _SFR_MEM8(0xB3)

/* RegDef:  Asynchronous Status Register */
#define ASSR   _SFR_MEM8(0xB6)

/* RegDef:  TWI Bit Rate Register */
#define TWBR   _SFR_MEM8(0xB8)

/* RegDef:  TWI Status Register */
#define TWSR   _SFR_MEM8(0xB9)

/* RegDef:  TWI (Slave) Address Register */
#define TWAR   _SFR_MEM8(0xBA)

/* RegDef:  TWI Data Register */
#define TWDR   _SFR_MEM8(0xBB)

/* RegDef:  TWI Control Register */
#define TWCR   _SFR_MEM8(0xBC)

/* RegDef:  USART0 Control and Status Register A */
#define UCSR0A _SFR_MEM8(0xC0)

/* RegDef:  USART0 Control and Status Register B */
#define UCSR0B _SFR_MEM8(0xC1)

/* RegDef:  USART0 Control and Status Register C */
#define UCSR0C _SFR_MEM8(0xC2)

/* RegDef:  USART0 Baud Rate Register */
#define UBRR0  _SFR_MEM16(0xC4)
#define UBRR0L _SFR_MEM8(0xC4)
#define UBRR0H _SFR_MEM8(0xC5)

/* RegDef:  USART0 I/O Data Register */
#define UDR0   _SFR_MEM8(0xC6)

/* RegDef:  USART1 Control and Status Register A */
#define UCSR1A _SFR_MEM8(0xC8)

/* RegDef:  USART1 Control and Status Register B */
#define UCSR1B _SFR_MEM8(0xC9)

/* RegDef:  USART1 Control and Status Register C */
#define UCSR1C _SFR_MEM8(0xCA)

/* RegDef:  USART1 Baud Rate Register */
#define UBRR1  _SFR_MEM16(0xCC)
#define UBRR1L _SFR_MEM8(0xCC)
#define UBRR1H _SFR_MEM8(0xCD)

/* RegDef:  USART1 I/O Data Register */
#define UDR1   _SFR_MEM8(0xCE)

/* RegDef:  CAN General Control Register*/
#define CANGCON _SFR_MEM8(0xD8)

/* RegDef:  CAN General Status Register*/
#define CANGSTA _SFR_MEM8(0xD9)

/* RegDef:  CAN General Interrupt Register*/
#define CANGIT _SFR_MEM8(0xDA)

/* RegDef:  CAN General Interrupt Enable Register*/
#define CANGIE _SFR_MEM8(0xDB)

/* RegDef:  CAN Enable MOb Register*/
#define CANEN2 _SFR_MEM8(0xDC)

/* RegDef:  CAN Enable MOb Register*/
#define CANEN1 _SFR_MEM8(0xDD)

/* RegDef:  CAN Enable Interrupt MOb Register*/
#define CANIE2 _SFR_MEM8(0xDE)

/* RegDef:  CAN Enable Interrupt MOb Register*/
#define CANIE1 _SFR_MEM8(0xDF)

/* RegDef:  CAN Status Interrupt MOb Register*/
/*
 * WARNING: Do not apply the SIT8...SIT14 constants to bits in the CANSIT
 * register.
 */
#define CANSIT  _SFR_MEM16(0xE0)
#define CANSIT2 _SFR_MEM8(0xE0)
#define CANSIT1 _SFR_MEM8(0xE1)

/* RegDef:  CAN Bit Timing Register 1*/
#define CANBT1 _SFR_MEM8(0xE2)

/* RegDef:  CAN Bit Timing Register 2*/
#define CANBT2 _SFR_MEM8(0xE3)

/* RegDef:  CAN Bit Timing Register 3*/
#define CANBT3 _SFR_MEM8(0xE4)

/* RegDef:  CAN Timer Control Register*/
#define CANTCON _SFR_MEM8(0xE5)

/* RegDef:  CAN Timer Register*/
#define CANTIM _SFR_MEM16(0xE6)
#define CANTIML _SFR_MEM8(0xE6)
#define CANTIMH _SFR_MEM8(0xE7)

/* RegDef:  CAN TTC Timer Register*/
#define CANTTC _SFR_MEM16(0xE8)
#define CANTTCL _SFR_MEM8(0xE8)
#define CANTTCH _SFR_MEM8(0xE9)

/* RegDef:  CAN Transmitt Error Counter Register*/
#define CANTEC _SFR_MEM8(0xEA)

/* RegDef:  CAN Receive Error Counter Register*/
#define CANREC _SFR_MEM8(0xEB)

/* RegDef:  CAN Highest Priority MOb Register*/
#define CANHPMOB _SFR_MEM8(0xEC)

/* RegDef:  CAN Page MOb Register*/
#define CANPAGE _SFR_MEM8(0xED)

/* RegDef:  CAN MOb Status Register*/
#define CANSTMOB _SFR_MEM8(0xEE)

/* RegDef:  CAN MOb Control and DLC Register*/
#define CANCDMOB _SFR_MEM8(0xEF)

/* RegDef:  CAN Identifier Tag Registers*/
#define CANIDT  _SFR_MEM32(0xF0)

#define CANIDT4 _SFR_MEM8(0xF0)
#define CANIDT3 _SFR_MEM8(0xF1)
#define CANIDT2 _SFR_MEM8(0xF2)
#define CANIDT1 _SFR_MEM8(0xF3)

/* RegDef:  CAN Identifier Mask Registers */
#define CANIDM  _SFR_MEM32(0xF4)

#define CANIDM4 _SFR_MEM8(0xF4)
#define CANIDM3 _SFR_MEM8(0xF5)
#define CANIDM2 _SFR_MEM8(0xF6)
#define CANIDM1 _SFR_MEM8(0xF7)

/* RegDef:  CAN TTC Timer Register*/
#define CANSTM _SFR_MEM16(0xF8)
#define CANSTML _SFR_MEM8(0xF8)
#define CANSTMH _SFR_MEM8(0xF9)

/* RegDef:  CAN Message Register*/
#define CANMSG _SFR_MEM8(0xFA)

/* Interrupt vectors */

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)

/* External Interrupt Request 2 */
#define INT2_vect			_VECTOR(3)
#define SIG_INTERRUPT2			_VECTOR(3)

/* External Interrupt Request 3 */
#define INT3_vect			_VECTOR(4)
#define SIG_INTERRUPT3			_VECTOR(4)

/* External Interrupt Request 4 */
#define INT4_vect			_VECTOR(5)
#define SIG_INTERRUPT4			_VECTOR(5)

/* External Interrupt Request 5 */
#define INT5_vect			_VECTOR(6)
#define SIG_INTERRUPT5			_VECTOR(6)

/* External Interrupt Request 6 */
#define INT6_vect			_VECTOR(7)
#define SIG_INTERRUPT6			_VECTOR(7)

/* External Interrupt Request 7 */
#define INT7_vect			_VECTOR(8)
#define SIG_INTERRUPT7			_VECTOR(8)

/* Timer/Counter2 Compare Match */
#define TIMER2_COMP_vect		_VECTOR(9)
#define SIG_OUTPUT_COMPARE2		_VECTOR(9)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(10)
#define SIG_OVERFLOW2			_VECTOR(10)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(11)
#define SIG_INPUT_CAPTURE1		_VECTOR(11)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(12)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(12)

/* Timer/Counter Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(13)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(13)

/* Timer/Counter1 Compare Match C */
#define TIMER1_COMPC_vect		_VECTOR(14)
#define SIG_OUTPUT_COMPARE1C		_VECTOR(14)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(15)
#define SIG_OVERFLOW1			_VECTOR(15)

/* Timer/Counter0 Compare Match */
#define TIMER0_COMP_vect		_VECTOR(16)
#define SIG_OUTPUT_COMPARE0		_VECTOR(16)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(17)
#define SIG_OVERFLOW0			_VECTOR(17)

/* CAN Transfer Complete or Error */
#define CANIT_vect			_VECTOR(18)
#define SIG_CAN_INTERRUPT1		_VECTOR(18)

/* CAN Timer Overrun */
#define OVRIT_vect			_VECTOR(19)
#define SIG_CAN_OVERFLOW1		_VECTOR(19)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(20)
#define SIG_SPI				_VECTOR(20)

/* USART0, Rx Complete */
#define USART0_RX_vect			_VECTOR(21)
#define SIG_UART0_RECV			_VECTOR(21)
#define SIG_USART0_RECV			_VECTOR(21)

/* USART0 Data Register Empty */
#define USART0_UDRE_vect		_VECTOR(22)
#define SIG_UART0_DATA			_VECTOR(22)
#define SIG_USART0_DATA			_VECTOR(22)

/* USART0, Tx Complete */
#define USART0_TX_vect			_VECTOR(23)
#define SIG_UART0_TRANS			_VECTOR(23)
#define SIG_USART0_TRANS		_VECTOR(23)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(24)
#define SIG_COMPARATOR			_VECTOR(24)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(25)
#define SIG_ADC				_VECTOR(25)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(26)
#define SIG_EEPROM_READY		_VECTOR(26)

/* Timer/Counter3 Capture Event */
#define TIMER3_CAPT_vect		_VECTOR(27)
#define SIG_INPUT_CAPTURE3		_VECTOR(27)

/* Timer/Counter3 Compare Match A */
#define TIMER3_COMPA_vect		_VECTOR(28)
#define SIG_OUTPUT_COMPARE3A		_VECTOR(28)

/* Timer/Counter3 Compare Match B */
#define TIMER3_COMPB_vect		_VECTOR(29)
#define SIG_OUTPUT_COMPARE3B		_VECTOR(29)

/* Timer/Counter3 Compare Match C */
#define TIMER3_COMPC_vect		_VECTOR(30)
#define SIG_OUTPUT_COMPARE3C		_VECTOR(30)

/* Timer/Counter3 Overflow */
#define TIMER3_OVF_vect			_VECTOR(31)
#define SIG_OVERFLOW3			_VECTOR(31)

/* USART1, Rx Complete */
#define USART1_RX_vect			_VECTOR(32)
#define SIG_UART1_RECV			_VECTOR(32)
#define SIG_USART1_RECV			_VECTOR(32)

/* USART1, Data Register Empty */
#define USART1_UDRE_vect		_VECTOR(33)
#define SIG_UART1_DATA			_VECTOR(33)
#define SIG_USART1_DATA			_VECTOR(33)

/* USART1, Tx Complete */
#define USART1_TX_vect			_VECTOR(34)
#define SIG_UART1_TRANS			_VECTOR(34)
#define SIG_USART1_TRANS		_VECTOR(34)

/* 2-wire Serial Interface */
#define TWI_vect			_VECTOR(35)
#define SIG_2WIRE_SERIAL		_VECTOR(35)

/* Store Program Memory Read */
#define SPM_READY_vect			_VECTOR(36)
#define SIG_SPM_READY			_VECTOR(36)

#define _VECTORS_SIZE 148

/* The Register Bit names are represented by their bit number (0-7). */

/* Register Bits [ASSR]  */
/* Asynchronous Status Register */
#define    EXCLK      4
#define    AS2        3
#define    TCN2UB     2
#define    OCR2UB     1
#define    TCR2UB     0
/* End Register Bits */

/* Register Bits [TWCR] */
/* 2-wire Control Register - TWCR */
#define    TWINT        7
#define    TWEA         6
#define    TWSTA        5
#define    TWSTO        4
#define    TWWC         3
#define    TWEN         2
#define    TWIE         0
/* End Register Bits */

/* Register Bits [TWAR]  */
/* 2-wire Address Register - TWAR */
#define    TWA6         7
#define    TWA5         6
#define    TWA4         5
#define    TWA3         4
#define    TWA2         3
#define    TWA1         2
#define    TWA0         1
#define    TWGCE        0
/* End Register Bits */

/* Register Bits [TWSR]  */
/* 2-wire Status Register - TWSR */
#define    TWS7         7
#define    TWS6         6
#define    TWS5         5
#define    TWS4         4
#define    TWS3         3
#define    TWPS1        1
#define    TWPS0        0
/* End Register Bits */

/* Register Bits [XMCRB]  */
/* External Memory Control Register B - XMCRB */
#define    XMBK         7
#define    XMM2         2
#define    XMM1         1
#define    XMM0         0
/* End Register Bits */

/* Register Bits [XMCRA]  */
/* External Memory Control Register A - XMCRA */
#define    SRE         7
#define    SRL2        6
#define    SRL1        5
#define    SRL0        4
#define    SRW11       3
#define    SRW10       2
#define    SRW01       1
#define    SRW00       0
/* End Register Bits */

/* Register Bits [RAMPZ]  */
/* RAM Page Z select register - RAMPZ */
#define     RAMPZ0      0
/* End Register Bits */

/* Register Bits [EICRA]  */
/* External Interrupt Control Register A - EICRA */
#define    ISC31        7
#define    ISC30        6
#define    ISC21        5
#define    ISC20        4
#define    ISC11        3
#define    ISC10        2
#define    ISC01        1
#define    ISC00        0
/* End Register Bits */

/* Register Bits [EICRB]  */
/* External Interrupt Control Register B - EICRB */
#define    ISC71        7
#define    ISC70        6
#define    ISC61        5
#define    ISC60        4
#define    ISC51        3
#define    ISC50        2
#define    ISC41        1
#define    ISC40        0
/* End Register Bits */

/* Register Bits [SPMCSR]  */
/* Store Program Memory Control Register - SPMCSR, SPMCR */
#define    SPMIE        7
#define    RWWSB        6
#define    RWWSRE       4
#define    BLBSET       3
#define    PGWRT        2
#define    PGERS        1
#define    SPMEN        0
/* End Register Bits */

/* Register Bits [EIMSK]  */
/* External Interrupt MaSK register - EIMSK */
#define    INT7         7
#define    INT6         6
#define    INT5         5
#define    INT4         4
#define    INT3         3
#define    INT2         2
#define    INT1         1
#define    INT0         0
/* End Register Bits */

/* Register Bits [EIFR]  */
/* External Interrupt Flag Register - EIFR */
#define    INTF7        7
#define    INTF6        6
#define    INTF5        5
#define    INTF4        4
#define    INTF3        3
#define    INTF2        2
#define    INTF1        1
#define    INTF0        0
/* End Register Bits */

/* Register Bits [TCCR2]  */
/* Timer/Counter 2 Control Register - TCCR2 */
#define    FOC2A        7
#define    WGM20        6
#define    COM2A1       5
#define    COM2A0       4
#define    WGM21        3
#define    CS22         2
#define    CS21         1
#define    CS20         0
/* End Register Bits */

/* Register Bits [TCCR1A]  */
/* Timer/Counter 1 Control and Status Register A - TCCR1A */
#define    COM1A1       7
#define    COM1A0       6
#define    COM1B1       5
#define    COM1B0       4
#define    COM1C1       3
#define    COM1C0       2
#define    WGM11        1
#define    WGM10        0
/* End Register Bits */

/* Register Bits [TCCR3A]  */
/* Timer/Counter 3 Control and Status Register A - TCCR3A */
#define    COM3A1       7
#define    COM3A0       6
#define    COM3B1       5
#define    COM3B0       4
#define    COM3C1       3
#define    COM3C0       2
#define    WGM31        1
#define    WGM30        0
/* End Register Bits */

/* Register Bits [TCCR1B]  */
/* Timer/Counter 1 Control and Status Register B - TCCR1B */
#define    ICNC1        7
#define    ICES1        6
#define    WGM13        4
#define    WGM12        3
#define    CS12         2
#define    CS11         1
#define    CS10         0
/* End Register Bits */

/* Register Bits [TCCR3B]  */
/* Timer/Counter 3 Control and Status Register B - TCCR3B */
#define    ICNC3        7
#define    ICES3        6
#define    WGM33        4
#define    WGM32        3
#define    CS32         2
#define    CS31         1
#define    CS30         0
/* End Register Bits */

/* Register Bits [TCCR3C]  */
/* Timer/Counter 3 Control Register C - TCCR3C */
#define    FOC3A        7
#define    FOC3B        6
#define    FOC3C        5
/* End Register Bits */

/* Register Bits [TCCR1C]  */
/* Timer/Counter 1 Control Register C - TCCR1C */
#define    FOC1A        7
#define    FOC1B        6
#define    FOC1C        5
/* End Register Bits */

/* Register Bits [OCDR]  */
/* On-chip Debug Register - OCDR */
#define    IDRD         7
#define    OCDR7        7
#define    OCDR6        6
#define    OCDR5        5
#define    OCDR4        4
#define    OCDR3        3
#define    OCDR2        2
#define    OCDR1        1
#define    OCDR0        0
/* End Register Bits */

/* Register Bits [WDTCR]  */
/* Watchdog Timer Control Register - WDTCR */
#define    WDCE         4
#define    WDE          3
#define    WDP2         2
#define    WDP1         1
#define    WDP0         0
/* End Register Bits */

/* Register Bits [SPSR]  */
/* SPI Status Register - SPSR */
#define    SPIF         7
#define    WCOL         6
#define    SPI2X        0
/* End Register Bits */

/* Register Bits [SPCR]  */
/* SPI Control Register - SPCR */
#define    SPIE         7
#define    SPE          6
#define    DORD         5
#define    MSTR         4
#define    CPOL         3
#define    CPHA         2
#define    SPR1         1
#define    SPR0         0
/* End Register Bits */

/* Register Bits [UCSR1C]  */
/* USART1 Register C - UCSR1C */
#define    UMSEL1       6
#define    UPM11        5
#define    UPM10        4
#define    USBS1        3
#define    UCSZ11       2
#define    UCSZ10       1
#define    UCPOL1       0
/* End Register Bits */

/* Register Bits [UCSR0C]  */
/* USART0 Register C - UCSR0C */
#define    UMSEL0       6
#define    UPM01        5
#define    UPM00        4
#define    USBS0        3
#define    UCSZ01       2
#define    UCSZ00       1
#define    UCPOL0       0
/* End Register Bits */

/* Register Bits [UCSR1A]  */
/* USART1 Status Register A - UCSR1A */
#define    RXC1         7
#define    TXC1         6
#define    UDRE1        5
#define    FE1          4
#define    DOR1         3
#define    UPE1         2
#define    U2X1         1
#define    MPCM1        0
/* End Register Bits */

/* Register Bits [UCSR0A]  */
/* USART0 Status Register A - UCSR0A */
#define    RXC0         7
#define    TXC0         6
#define    UDRE0        5
#define    FE0          4
#define    DOR0         3
#define    UPE0         2
#define    U2X0         1
#define    MPCM0        0
/* End Register Bits */

/* Register Bits [UCSR1B]  */
/* USART1 Control Register B - UCSR1B */
#define    RXCIE1       7
#define    TXCIE1       6
#define    UDRIE1       5
#define    RXEN1        4
#define    TXEN1        3
#define    UCSZ12       2
#define    RXB81        1
#define    TXB81        0
/* End Register Bits */

/* Register Bits [UCSR0B]  */
/* USART0 Control Register B - UCSR0B */
#define    RXCIE0       7
#define    TXCIE0       6
#define    UDRIE0       5
#define    RXEN0        4
#define    TXEN0        3
#define    UCSZ02       2
#define    RXB80        1
#define    TXB80        0
/* End Register Bits */

/* Register Bits [ACSR]  */
/* Analog Comparator Control and Status Register - ACSR */
#define    ACD          7
#define    ACBG         6
#define    ACO          5
#define    ACI          4
#define    ACIE         3
#define    ACIC         2
#define    ACIS1        1
#define    ACIS0        0
/* End Register Bits */

/* Register Bits [ADCSRA]  */
/* ADC Control and status register - ADCSRA */
#define    ADEN         7
#define    ADSC         6
#define    ADATE        5
#define    ADIF         4
#define    ADIE         3
#define    ADPS2        2
#define    ADPS1        1
#define    ADPS0        0
/* End Register Bits */

/* 
   The ADHSM bit has been removed from all documentation, 
   as being not needed at all since the comparator has proven 
   to be fast enough even without feeding it more power.
*/

/* Register Bits [ADCSRB]  */
/* ADC Control and status register - ADCSRB */
#define    ACME         6
#define    ADTS2        2
#define    ADTS1        1
#define    ADTS0        0
/* End Register Bits */

/* Register Bits [ADMUX]  */
/* ADC Multiplexer select - ADMUX */
#define    REFS1        7
#define    REFS0        6
#define    ADLAR        5
#define    MUX4         4
#define    MUX3         3
#define    MUX2         2
#define    MUX1         1
#define    MUX0         0
/* End Register Bits */

/* Register Bits [DIDR0]  */
/* Digital Input Disable Register 0 */
#define    ADC7D        7
#define    ADC6D        6
#define    ADC5D        5
#define    ADC4D        4
#define    ADC3D        3
#define    ADC2D        2
#define    ADC1D        1
#define    ADC0D        0
/* End Register Bits */

/* Register Bits [DIDR1]  */
/* Digital Input Disable Register 1 */
#define    AIN1D        1
#define    AIN0D        0
/* End Register Bits */

/* Register Bits [PORTA]  */
/* Port A Data Register - PORTA */
#define    PA7          7
#define    PA6          6
#define    PA5          5
#define    PA4          4
#define    PA3          3
#define    PA2          2
#define    PA1          1
#define    PA0          0
/* End Register Bits */

/* Register Bits [DDRA]  */
/* Port A Data Direction Register - DDRA */
#define    DDA7         7
#define    DDA6         6
#define    DDA5         5
#define    DDA4         4
#define    DDA3         3
#define    DDA2         2
#define    DDA1         1
#define    DDA0         0
/* End Register Bits */

/* Register Bits [PINA]  */
/* Port A Input Pins - PINA */
#define    PINA7        7
#define    PINA6        6
#define    PINA5        5
#define    PINA4        4
#define    PINA3        3
#define    PINA2        2
#define    PINA1        1
#define    PINA0        0
/* End Register Bits */

/* Register Bits [PORTB]  */
/* Port B Data Register - PORTB */
#define    PB7          7
#define    PB6          6
#define    PB5          5
#define    PB4          4
#define    PB3          3
#define    PB2          2
#define    PB1          1
#define    PB0          0
/* End Register Bits */

/* Register Bits [DDRB]  */
/* Port B Data Direction Register - DDRB */
#define    DDB7         7
#define    DDB6         6
#define    DDB5         5
#define    DDB4         4
#define    DDB3         3
#define    DDB2         2
#define    DDB1         1
#define    DDB0         0
/* End Register Bits */

/* Register Bits [PINB]  */
/* Port B Input Pins - PINB */
#define    PINB7        7
#define    PINB6        6
#define    PINB5        5
#define    PINB4        4
#define    PINB3        3
#define    PINB2        2
#define    PINB1        1
#define    PINB0        0
/* End Register Bits */

/* Register Bits [PORTC]  */
/* Port C Data Register - PORTC */
#define    PC7          7
#define    PC6          6
#define    PC5          5
#define    PC4          4
#define    PC3          3
#define    PC2          2
#define    PC1          1
#define    PC0          0
/* End Register Bits */

/* Register Bits [DDRC]  */
/* Port C Data Direction Register - DDRC */
#define    DDC7         7
#define    DDC6         6
#define    DDC5         5
#define    DDC4         4
#define    DDC3         3
#define    DDC2         2
#define    DDC1         1
#define    DDC0         0
/* End Register Bits */

/* Register Bits [PINC]  */
/* Port C Input Pins - PINC */
#define    PINC7        7
#define    PINC6        6
#define    PINC5        5
#define    PINC4        4
#define    PINC3        3
#define    PINC2        2
#define    PINC1        1
#define    PINC0        0
/* End Register Bits */

/* Register Bits [PORTD]  */
/* Port D Data Register - PORTD */
#define    PD7          7
#define    PD6          6
#define    PD5          5
#define    PD4          4
#define    PD3          3
#define    PD2          2
#define    PD1          1
#define    PD0          0
/* End Register Bits */

/* Register Bits [DDRD]  */
/* Port D Data Direction Register - DDRD */
#define    DDD7         7
#define    DDD6         6
#define    DDD5         5
#define    DDD4         4
#define    DDD3         3
#define    DDD2         2
#define    DDD1         1
#define    DDD0         0
/* End Register Bits */

/* Register Bits [PIND]  */
/* Port D Input Pins - PIND */
#define    PIND7        7
#define    PIND6        6
#define    PIND5        5
#define    PIND4        4
#define    PIND3        3
#define    PIND2        2
#define    PIND1        1
#define    PIND0        0
/* End Register Bits */

/* Register Bits [PORTE]  */
/* Port E Data Register - PORTE */
#define    PE7          7
#define    PE6          6
#define    PE5          5
#define    PE4          4
#define    PE3          3
#define    PE2          2
#define    PE1          1
#define    PE0          0
/* End Register Bits */

/* Register Bits [DDRE]  */
/* Port E Data Direction Register - DDRE */
#define    DDE7         7
#define    DDE6         6
#define    DDE5         5
#define    DDE4         4
#define    DDE3         3
#define    DDE2         2
#define    DDE1         1
#define    DDE0         0
/* End Register Bits */

/* Register Bits [PINE]  */
/* Port E Input Pins - PINE */
#define    PINE7        7
#define    PINE6        6
#define    PINE5        5
#define    PINE4        4
#define    PINE3        3
#define    PINE2        2
#define    PINE1        1
#define    PINE0        0
/* End Register Bits */

/* Register Bits [PORTF]  */
/* Port F Data Register - PORTF */
#define    PF7          7
#define    PF6          6
#define    PF5          5
#define    PF4          4
#define    PF3          3
#define    PF2          2
#define    PF1          1
#define    PF0          0
/* End Register Bits */

/* Register Bits [DDRF]  */
/* Port F Data Direction Register - DDRF */
#define    DDF7         7
#define    DDF6         6
#define    DDF5         5
#define    DDF4         4
#define    DDF3         3
#define    DDF2         2
#define    DDF1         1
#define    DDF0         0
/* End Register Bits */

/* Register Bits [PINF]  */
/* Port F Input Pins - PINF */
#define    PINF7        7
#define    PINF6        6
#define    PINF5        5
#define    PINF4        4
#define    PINF3        3
#define    PINF2        2
#define    PINF1        1
#define    PINF0        0
/* End Register Bits */

/* Register Bits [PORTG]  */
/* Port G Data Register - PORTG */
#define    PG4          4
#define    PG3          3
#define    PG2          2
#define    PG1          1
#define    PG0          0
/* End Register Bits */

/* Register Bits [DDRG]  */
/* Port G Data Direction Register - DDRG */
#define    DDG4         4
#define    DDG3         3
#define    DDG2         2
#define    DDG1         1
#define    DDG0         0
/* End Register Bits */

/* Register Bits [PING]  */
/* Port G Input Pins - PING */
#define    PING4        4
#define    PING3        3
#define    PING2        2
#define    PING1        1
#define    PING0        0
/* End Register Bits */


/* Register Bits [TIFR0]  */
/* Timer/Counter 0 interrupt Flag Register */
#define    OCF0A        1
#define    TOV0         0
/* End Register Bits */

/* Register Bits [TIFR1]  */
/* Timer/Counter 1 interrupt Flag Register */
#define    ICF1       5
#define    OCF1C      3
#define    OCF1B      2
#define    OCF1A      1
#define    TOV1       0
/* End Register Bits */

/* Register Bits [TIFR2]  */
/* Timer/Counter 2 interrupt Flag Register */
#define    OCF2A      1
#define    TOV2       0
/* End Register Bits */

/* Register Bits [TIFR3]  */
/* Timer/Counter 3 interrupt Flag Register */
#define    ICF3       5
#define    OCF3C      3
#define    OCF3B      2
#define    OCF3A      1
#define    TOV3       0
/* End Register Bits */

/* Register Bits [GPIOR0]  */
/* General Purpose I/O Register 0 */
#define    GPIOR07     7
#define    GPIOR06     6
#define    GPIOR05     5
#define    GPIOR04     4
#define    GPIOR03     3
#define    GPIOR02     2
#define    GPIOR01     1
#define    GPIOR00     0
/* End Register Bits */

/* Register Bits [GPIOR1]  */
/* General Purpose I/O Register 1 */
#define    GPIOR17     7
#define    GPIOR16     6
#define    GPIOR15     5
#define    GPIOR14     4
#define    GPIOR13     3
#define    GPIOR12     2
#define    GPIOR11     1
#define    GPIOR10     0
/* End Register Bits */

/* Register Bits [GPIOR2]  */
/* General Purpose I/O Register 2 */
#define    GPIOR27     7
#define    GPIOR26     6
#define    GPIOR25     5
#define    GPIOR24     4
#define    GPIOR23     3
#define    GPIOR22     2
#define    GPIOR21     1
#define    GPIOR20     0
/* End Register Bits */

/* Register Bits [EECR]  */
/* EEPROM Control Register */
#define    EERIE       3
#define    EEMWE       2
#define    EEWE        1
#define    EERE        0
/* End Register Bits */

/* Register Bits [EEDR]  */
/* EEPROM Data Register */
#define    EEDR7     7
#define    EEDR6     6
#define    EEDR5     5
#define    EEDR4     4
#define    EEDR3     3
#define    EEDR2     2
#define    EEDR1     1
#define    EEDR0     0
/* End Register Bits */

/* Register Bits [EEARL]  */
/* EEPROM Address Register */
#define    EEAR7     7
#define    EEAR6     6
#define    EEAR5     5
#define    EEAR4     4
#define    EEAR3     3
#define    EEAR2     2
#define    EEAR1     1
#define    EEAR0     0
/* End Register Bits */

/* Register Bits [EEARH]  */
/* EEPROM Address Register */
#define    EEAR11    3
#define    EEAR10    2
#define    EEAR9     1
#define    EEAR8     0
/* End Register Bits */

/* Register Bits [GTCCR]  */
/* General Timer/Counter Control Register  */
#define    TSM      7
#define    PSR2     1
#define    PSR310   0
/* End Register Bits */

/* Register Bits [TCCR0A]  */
/* Timer/Counter Control Register A */
/* ALSO COVERED IN GENERIC SECTION */
#define    FOC0A    7
#define    WGM00    6
#define    COM0A1   5
#define    COM0A0   4
#define    WGM01    3
#define    CS02     2
#define    CS01     1
#define    CS00     0
/* End Register Bits */

/* Register Bits [OCR0A]  */
/* Output Compare Register A */
#define    OCR0A7     7
#define    OCR0A6     6
#define    OCR0A5     5
#define    OCR0A4     4
#define    OCR0A3     3
#define    OCR0A2     2
#define    OCR0A1     1
#define    OCR0A0     0
/* End Register Bits */


/* Register Bits [SPIDR]  */
/* SPI Data Register */
#define    SPD7     7
#define    SPD6     6
#define    SPD5     5
#define    SPD4     4
#define    SPD3     3
#define    SPD2     2
#define    SPD1     1
#define    SPD0     0
/* End Register Bits */

/* Register Bits [SMCR]  */
/* Sleep Mode Control Register */
#define    SM2     3
#define    SM1     2
#define    SM0     1
#define    SE      0
/* End Register Bits */

/* Register Bits [MCUSR]  */
/* MCU Status Register */
#define    JTRF    4
#define    WDRF    3
#define    BORF    2
#define    EXTRF   1
#define    PORF    0
/* End Register Bits */

/* Register Bits [MCUCR]  */
/* MCU Control Register */
#define    JTD     7
#define    PUD     4
#define    IVSEL   1
#define    IVCE    0
/* End Register Bits */

/* Register Bits [CLKPR]  */
/* Clock Prescale Register */
#define    CLKPCE     7
#define    CLKPS3     3
#define    CLKPS2     2
#define    CLKPS1     1
#define    CLKPS0     0
/* End Register Bits */

/* Register Bits [OSCCAL]  */
/* Oscillator Calibration Register */
#define    CAL6     6
#define    CAL5     5
#define    CAL4     4
#define    CAL3     3
#define    CAL2     2
#define    CAL1     1
#define    CAL0     0
/* End Register Bits */

/* Register Bits [TIMSK0]  */
/* Timer/Counter 0 interrupt mask Register */
#define    OCIE0A      1
#define    TOIE0       0
/* End Register Bits */

/* Register Bits [TIMSK1]  */
/* Timer/Counter 1 interrupt mask Register */
#define    ICIE1       5
#define    OCIE1C      3
#define    OCIE1B      2
#define    OCIE1A      1
#define    TOIE1       0
/* End Register Bits */

/* Register Bits [TIMSK2]  */
/* Timer/Counter 2 interrupt mask Register */
#define    OCIE2A      1
#define    TOIE2       0
/* End Register Bits */

/* Register Bits [TIMSK3]  */
/* Timer/Counter 3 interrupt mask Register */
#define    ICIE3       5
#define    OCIE3C      3
#define    OCIE3B      2
#define    OCIE3A      1
#define    TOIE3       0
/* End Register Bits */

//Begin CAN specific parts

/* Register Bits [CANGCON]  */
/* CAN General Control Register */
#define    ABRQ       7
#define    OVRQ       6
#define    TTC        5
#define    SYNTTC     4
#define    LISTEN     3
#define    TEST       2
#define    ENASTB     1
#define    SWRES      0
/* End Register Bits */

/* Register Bits [CANGSTA]  */
/* CAN General Status Register */
#define    OVFG       6
#define    OVRG       6
#define    TXBSY      4
#define    RXBSY      3
#define    ENFG       2
#define    BOFF       1
#define    ERRP       0
/* End Register Bits */

/* Register Bits [CANGIT]  */
/* CAN General Interrupt Register */
#define    CANIT      7
#define    BOFFIT     6
#define    OVRTIM     5
#define    BXOK       4
#define    SERG       3
#define    CERG       2
#define    FERG       1
#define    AERG       0
/* End Register Bits */

/* Register Bits [CANGIE]  */
/* CAN General Interrupt Enable */
#define    ENIT        7
#define    ENBOFF      6
#define    ENRX        5
#define    ENTX        4
#define    ENERR       3
#define    ENBX        2
#define    ENERG       1
#define    ENOVRT      0
/* End Register Bits */

/* Register Bits [CANEN2]  */
/* CAN Enable MOb Register */
#define    ENMOB7      7
#define    ENMOB6      6
#define    ENMOB5      5
#define    ENMOB4      4
#define    ENMOB3      3
#define    ENMOB2      2
#define    ENMOB1      1
#define    ENMOB0      0
/* End Register Bits */

/* Register Bits [CANEN1]  */
/* CAN Enable MOb Register */
#define    ENMOB14      6
#define    ENMOB13      5
#define    ENMOB12      4
#define    ENMOB11      3
#define    ENMOB10      2
#define    ENMOB9       1
#define    ENMOB8       0
/* End Register Bits */

/* Register Bits [CANIE2]  */
/* CAN Interrupt Enable MOb Register */
#define    IEMOB7      7
#define    IEMOB6      6
#define    IEMOB5      5
#define    IEMOB4      4
#define    IEMOB3      3
#define    IEMOB2      2
#define    IEMOB1      1
#define    IEMOB0      0
/* End Register Bits */

/* Register Bits [CANIE1]  */
/* CAN Interrupt Enable MOb Register */
#define    IEMOB14      6
#define    IEMOB13      5
#define    IEMOB12      4
#define    IEMOB11      3
#define    IEMOB10      2
#define    IEMOB9       1
#define    IEMOB8       0
/* End Register Bits */

/* Register Bits [CANSIT2]  */
/* CAN Status Interrupt MOb Register */
#define    SIT7      7
#define    SIT6      6
#define    SIT5      5
#define    SIT4      4
#define    SIT3      3
#define    SIT2      2
#define    SIT1      1
#define    SIT0      0
/* End Register Bits */

/* Register Bits [CANSIT1]  */
/* CAN Status Interrupt MOb Register */
#define    SIT14      6
#define    SIT13      5
#define    SIT12      4
#define    SIT11      3
#define    SIT10      2
#define    SIT9       1
#define    SIT8       0
/* End Register Bits */

/* Register Bits [CANBT1]  */
/* Bit Timing Register 1 */
#define    BRP5       6
#define    BRP4       5
#define    BRP3       4
#define    BRP2       3
#define    BRP1       2
#define    BRP0       1
/* End Register Bits */

/* Register Bits [CANBT2]  */
/* Bit Timing Register 2 */
#define    SJW1       6
#define    SJW0       5
#define    PRS2       3
#define    PRS1       2
#define    PRS0       1
/* End Register Bits */

/* Register Bits [CANBT3]  */
/* Bit Timing Register 3 */
#define    PHS22      6
#define    PHS21      5
#define    PHS20      4
#define    PHS12      3
#define    PHS11      2
#define    PHS10      1
#define    SMP        0
/* End Register Bits */

/* Register Bits [CANTCON]  */
/* CAN Timer Control Register */
#define    TPRSC7      7
#define    TPRSC6      6
#define    TPRSC5      5
#define    TPRSC4      4
#define    TPRSC3      3
#define    TPRSC2      2
#define    TPRSC1      1
#define    TPRSC0      0
/* End Register Bits */

/* Register Bits [CANTIML]  */
/* CAN Timer Register Low */
#define    CANTIM7      7
#define    CANTIM6      6
#define    CANTIM5      5
#define    CANTIM4      4
#define    CANTIM3      3
#define    CANTIM2      2
#define    CANTIM1      1
#define    CANTIM0      0
/* End Register Bits */

/* Register Bits [CANTIMH]  */
/* CAN Timer Register High */
#define    CANTIM15     7
#define    CANTIM14     6
#define    CANTIM13     5
#define    CANTIM12     4
#define    CANTIM11     3
#define    CANTIM10     2
#define    CANTIM9      1
#define    CANTIM8      0
/* End Register Bits */

/* Register Bits [CANTTCL]  */
/* CAN TTC Timer Register Low */
#define    TIMTTC7      7
#define    TIMTTC6      6
#define    TIMTTC5      5
#define    TIMTTC4      4
#define    TIMTTC3      3
#define    TIMTTC2      2
#define    TIMTTC1      1
#define    TIMTTC0      0
/* End Register Bits */

/* Register Bits [CANTTCH]  */
/* CAN TTC Timer Register High */
#define    TIMTTC15     7
#define    TIMTTC14     6
#define    TIMTTC13     5
#define    TIMTTC12     4
#define    TIMTTC11     3
#define    TIMTTC10     2
#define    TIMTTC9      1
#define    TIMTTC8      0
/* End Register Bits */

/* Register Bits [CANTEC]  */
/* CAN Transmitt Error Counter */
#define    TEC7      7
#define    TEC6      6
#define    TEC5      5
#define    TEC4      4
#define    TEC3      3
#define    TEC2      2
#define    TEC1      1
#define    TEC0      0
/* End Register Bits */

/* Register Bits [CANREC]  */
/* CAN Receive Error Counter */
#define    REC7      7
#define    REC6      6
#define    REC5      5
#define    REC4      4
#define    REC3      3
#define    REC2      2
#define    REC1      1
#define    REC0      0
/* End Register Bits */

/* Register Bits [CANHPMOB]  */
/* Highest Priority MOb */
#define    HPMOB3     7
#define    HPMOB2     6
#define    HPMOB1     5
#define    HPMOB0     4
#define    CGP3       3
#define    CGP2       2
#define    CGP1       1
#define    CGP0       0
/* End Register Bits */

/* Register Bits [CANPAGE]  */
/* CAN Page MOb Register */
#define    MOBNB3     7
#define    MOBNB2     6
#define    MOBNB1     5
#define    MOBNB0     4
#define    AINC       3
#define    INDX2      2
#define    INDX1      1
#define    INDX0      0
/* End Register Bits */

/* Register Bits [CANSTMOB]  */
/* CAN MOb Status Register */
#define    DLCW       7
#define    TXOK       6
#define    RXOK       5
#define    BERR       4
#define    SERR       3
#define    CERR       2
#define    FERR       1
#define    AERR       0
/* End Register Bits */

/* Register Bits [CANCDMOB]  */
/* CAN MOb Control and DLC Register */
#define    CONMOB1    7
#define    CONMOB0    6
#define    RPLV       5
#define    IDE        4
#define    DLC3       3
#define    DLC2       2
#define    DLC1       1
#define    DLC0       0
/* End Register Bits */

/* Register Bits [CANIDT4]  */
/* CAN Identifier Tag Register 4 */
#define    IDT4       7
#define    IDT3       6
#define    IDT2       5
#define    IDT1       4
#define    IDT0       3
#define    RTRTAG     2
#define    RB1TAG     1
#define    RB0TAG     0
/* End Register Bits */

/* Register Bits [CANIDT3]  */
/* CAN Identifier Tag Register 3 */
#define    IDT12      7
#define    IDT11      6
#define    IDT10      5
#define    IDT9       4
#define    IDT8       3
#define    IDT7       2
#define    IDT6       1
#define    IDT5       0
/* End Register Bits */

/* Register Bits [CANIDT2]  */
/* CAN Identifier Tag Register 2 */
#define    IDT20      7
#define    IDT19      6
#define    IDT18      5
#define    IDT17      4
#define    IDT16      3
#define    IDT15      2
#define    IDT14      1
#define    IDT13      0
/* End Register Bits */

/* Register Bits [CANIDT1]  */
/* CAN Identifier Tag Register 1 */
#define    IDT28      7
#define    IDT27      6
#define    IDT26      5
#define    IDT25      4
#define    IDT24      3
#define    IDT23      2
#define    IDT22      1
#define    IDT21      0
/* End Register Bits */

/* Register Bits [CANIDM4]  */
/* CAN Identifier Mask Register 4 */
#define    IDMSK4       7
#define    IDMSK3       6
#define    IDMSK2       5
#define    IDMSK1       4
#define    IDMSK0       3
#define    RTRMSK       2
#define    IDEMSK       0
/* End Register Bits */

/* Register Bits [CANIDM3]  */
/* CAN Identifier Mask Register 3 */
#define    IDMSK12      7
#define    IDMSK11      6
#define    IDMSK10      5
#define    IDMSK9       4
#define    IDMSK8       3
#define    IDMSK7       2
#define    IDMSK6       1
#define    IDMSK5       0
/* End Register Bits */

/* Register Bits [CANIDM2]  */
/* CAN Identifier Mask Register 2 */
#define    IDMSK20      7
#define    IDMSK19      6
#define    IDMSK18      5
#define    IDMSK17      4
#define    IDMSK16      3
#define    IDMSK15      2
#define    IDMSK14      1
#define    IDMSK13      0
/* End Register Bits */

/* Register Bits [CANIDM1]  */
/* CAN Identifier Mask Register 1 */
#define    IDMSK28      7
#define    IDMSK27      6
#define    IDMSK26      5
#define    IDMSK25      4
#define    IDMSK24      3
#define    IDMSK23      2
#define    IDMSK22      1
#define    IDMSK21      0
/* End Register Bits */

/* Register Bits [CANSTML]  */
/* CAN Timer Register of some sort, low*/
#define    TIMSTM7       7
#define    TIMSTM6       6
#define    TIMSTM5       5
#define    TIMSTM4       4
#define    TIMSTM3       3
#define    TIMSTM2       2
#define    TIMSTM1       1
#define    TIMSTM0       0
/* End Register Bits */

/* Register Bits [CANSTMH]  */
/* CAN Timer Register of some sort, high */
#define    TIMSTM15       7
#define    TIMSTM14       6
#define    TIMSTM13       5
#define    TIMSTM12       4
#define    TIMSTM11       3
#define    TIMSTM10       2
#define    TIMSTM9        1
#define    TIMSTM8        0
/* End Register Bits */

/* Register Bits [CANMSG]  */
/* CAN Message Register */
#define    MSG7           7
#define    MSG6           6
#define    MSG5           5
#define    MSG4           4
#define    MSG3           3
#define    MSG2           2
#define    MSG1           1
#define    MSG0           0
/* End Register Bits */

/* Begin Verbatim */

/* Timer/Counter Control Register (generic) */
#define    FOC          7
#define    WGM0         6
#define    COM1         5
#define    COM0         4
#define    WGM1         3
#define    CS2          2
#define    CS1          1
#define    CS0          0

/* Timer/Counter Control Register A (generic) */
#define    COMA1        7
#define    COMA0        6
#define    COMB1        5
#define    COMB0        4
#define    COMC1        3
#define    COMC0        2
#define    WGMA1        1
#define    WGMA0        0

/* Timer/Counter Control and Status Register B (generic) */
#define    ICNC         7
#define    ICES         6
#define    WGMB3        4
#define    WGMB2        3
#define    CSB2         2
#define    CSB1         1
#define    CSB0         0

/* Timer/Counter Control Register C (generic) */
#define    FOCA         7
#define    FOCB         6
#define    FOCC         5

/* Port Data Register (generic) */
#define    PORT7        7
#define    PORT6        6
#define    PORT5        5
#define    PORT4        4
#define    PORT3        3
#define    PORT2        2
#define    PORT1        1
#define    PORT0        0

/* Port Data Direction Register (generic) */
#define    DD7          7
#define    DD6          6
#define    DD5          5
#define    DD4          4
#define    DD3          3
#define    DD2          2
#define    DD1          1
#define    DD0          0

/* Port Input Pins (generic) */
#define    PIN7         7
#define    PIN6         6
#define    PIN5         5
#define    PIN4         4
#define    PIN3         3
#define    PIN2         2
#define    PIN1         1
#define    PIN0         0

/* USART Status Register A (generic) */
#define    RXC          7
#define    TXC          6
#define    UDRE         5
#define    FE           4
#define    DOR          3
#define    UPE          2
#define    U2X          1
#define    MPCM         0

/* USART Control Register B (generic) */
#define    RXCIE        7
#define    TXCIE        6
#define    UDRIE        5
#define    RXEN         4
#define    TXEN         3
#define    UCSZ         2
#define    UCSZ2        2       /* new name in datasheet (2467E-AVR-05/02) */
#define    RXB8         1
#define    TXB8         0

/* USART Register C (generic) */
#define    UMSEL        6
#define    UPM1         5
#define    UPM0         4
#define    USBS         3
#define    UCSZ1        2
#define    UCSZ0        1
#define    UCPOL        0

/* End Verbatim */

#endif  /* _AVR_IOCANXX_H_ */
