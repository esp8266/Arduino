/* Copyright (c) 2002, Peter Jansen
   Copyright (c) 2007, Atmel Corporation
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

/* $Id: iom128.h,v 1.21.2.5 2008/10/17 23:27:46 arcanum Exp $ */

/* avr/iom128.h - defines for ATmega128 

   As of 2002-08-27:
   - This should be up to date with data sheet 2467E-AVR-05/02 */

#ifndef _AVR_IOM128_H_
#define _AVR_IOM128_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom128.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* Input Pins, Port F */
#define PINF      _SFR_IO8(0x00)

/* Input Pins, Port E */
#define PINE      _SFR_IO8(0x01)

/* Data Direction Register, Port E */
#define DDRE      _SFR_IO8(0x02)

/* Data Register, Port E */
#define PORTE     _SFR_IO8(0x03)

/* ADC Data Register */
#define ADCW      _SFR_IO16(0x04) /* for backwards compatibility */
#ifndef __ASSEMBLER__
#define ADC       _SFR_IO16(0x04)
#endif
#define ADCL      _SFR_IO8(0x04)
#define ADCH      _SFR_IO8(0x05)

/* ADC Control and status register */
#define ADCSR     _SFR_IO8(0x06)
#define ADCSRA    _SFR_IO8(0x06) /* new name in datasheet (2467E-AVR-05/02) */

/* ADC Multiplexer select */
#define ADMUX     _SFR_IO8(0x07)

/* Analog Comparator Control and Status Register */
#define ACSR      _SFR_IO8(0x08)

/* USART0 Baud Rate Register Low */
#define UBRR0L    _SFR_IO8(0x09)

/* USART0 Control and Status Register B */
#define UCSR0B    _SFR_IO8(0x0A)

/* USART0 Control and Status Register A */
#define UCSR0A    _SFR_IO8(0x0B)

/* USART0 I/O Data Register */
#define UDR0      _SFR_IO8(0x0C)

/* SPI Control Register */
#define SPCR      _SFR_IO8(0x0D)

/* SPI Status Register */
#define SPSR      _SFR_IO8(0x0E)

/* SPI I/O Data Register */
#define SPDR      _SFR_IO8(0x0F)

/* Input Pins, Port D */
#define PIND      _SFR_IO8(0x10)

/* Data Direction Register, Port D */
#define DDRD      _SFR_IO8(0x11)

/* Data Register, Port D */
#define PORTD     _SFR_IO8(0x12)

/* Input Pins, Port C */
#define PINC      _SFR_IO8(0x13)

/* Data Direction Register, Port C */
#define DDRC      _SFR_IO8(0x14)

/* Data Register, Port C */
#define PORTC     _SFR_IO8(0x15)

/* Input Pins, Port B */
#define PINB      _SFR_IO8(0x16)

/* Data Direction Register, Port B */
#define DDRB      _SFR_IO8(0x17)

/* Data Register, Port B */
#define PORTB     _SFR_IO8(0x18)

/* Input Pins, Port A */
#define PINA      _SFR_IO8(0x19)

/* Data Direction Register, Port A */
#define DDRA      _SFR_IO8(0x1A)

/* Data Register, Port A */
#define PORTA     _SFR_IO8(0x1B)

/* EEPROM Control Register */
#define EECR      _SFR_IO8(0x1C)

/* EEPROM Data Register */
#define EEDR      _SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR      _SFR_IO16(0x1E)
#define EEARL     _SFR_IO8(0x1E)
#define EEARH     _SFR_IO8(0x1F)

/* Special Function I/O Register */
#define SFIOR     _SFR_IO8(0x20)

/* Watchdog Timer Control Register */
#define WDTCR     _SFR_IO8(0x21)

/* On-chip Debug Register */
#define OCDR      _SFR_IO8(0x22)

/* Timer2 Output Compare Register */
#define OCR2      _SFR_IO8(0x23)

/* Timer/Counter 2 */
#define TCNT2     _SFR_IO8(0x24)

/* Timer/Counter 2 Control register */
#define TCCR2     _SFR_IO8(0x25)

/* T/C 1 Input Capture Register */
#define ICR1      _SFR_IO16(0x26)
#define ICR1L     _SFR_IO8(0x26)
#define ICR1H     _SFR_IO8(0x27)

/* Timer/Counter1 Output Compare Register B */
#define OCR1B     _SFR_IO16(0x28)
#define OCR1BL    _SFR_IO8(0x28)
#define OCR1BH    _SFR_IO8(0x29)

/* Timer/Counter1 Output Compare Register A */
#define OCR1A     _SFR_IO16(0x2A)
#define OCR1AL    _SFR_IO8(0x2A)
#define OCR1AH    _SFR_IO8(0x2B)

/* Timer/Counter 1 */
#define TCNT1     _SFR_IO16(0x2C)
#define TCNT1L    _SFR_IO8(0x2C)
#define TCNT1H    _SFR_IO8(0x2D)

/* Timer/Counter 1 Control and Status Register */
#define TCCR1B    _SFR_IO8(0x2E)

/* Timer/Counter 1 Control Register */
#define TCCR1A    _SFR_IO8(0x2F)

/* Timer/Counter 0 Asynchronous Control & Status Register */
#define ASSR      _SFR_IO8(0x30)

/* Output Compare Register 0 */
#define OCR0      _SFR_IO8(0x31)

/* Timer/Counter 0 */
#define TCNT0     _SFR_IO8(0x32)

/* Timer/Counter 0 Control Register */
#define TCCR0     _SFR_IO8(0x33)

/* MCU Status Register */
#define MCUSR     _SFR_IO8(0x34)
#define MCUCSR    _SFR_IO8(0x34) /* new name in datasheet (2467E-AVR-05/02) */

/* MCU general Control Register */
#define MCUCR     _SFR_IO8(0x35)

/* Timer/Counter Interrupt Flag Register */
#define TIFR      _SFR_IO8(0x36)

/* Timer/Counter Interrupt MaSK register */
#define TIMSK     _SFR_IO8(0x37)

/* External Interrupt Flag Register */
#define EIFR      _SFR_IO8(0x38)

/* External Interrupt MaSK register */
#define EIMSK     _SFR_IO8(0x39)

/* External Interrupt Control Register B */
#define EICRB     _SFR_IO8(0x3A)

/* RAM Page Z select register */
#define RAMPZ     _SFR_IO8(0x3B)

/* XDIV Divide control register */
#define XDIV      _SFR_IO8(0x3C)

/* 0x3D..0x3E SP */

/* 0x3F SREG */

/* Extended I/O registers */

/* Data Direction Register, Port F */
#define DDRF      _SFR_MEM8(0x61)

/* Data Register, Port F */
#define PORTF     _SFR_MEM8(0x62)

/* Input Pins, Port G */
#define PING      _SFR_MEM8(0x63)

/* Data Direction Register, Port G */
#define DDRG      _SFR_MEM8(0x64)

/* Data Register, Port G */
#define PORTG     _SFR_MEM8(0x65)

/* Store Program Memory Control and Status Register */
#define SPMCR     _SFR_MEM8(0x68)
#define SPMCSR    _SFR_MEM8(0x68) /* new name in datasheet (2467E-AVR-05/02) */

/* External Interrupt Control Register A */
#define EICRA     _SFR_MEM8(0x6A)

/* External Memory Control Register B */
#define XMCRB     _SFR_MEM8(0x6C)

/* External Memory Control Register A */
#define XMCRA     _SFR_MEM8(0x6D)

/* Oscillator Calibration Register */
#define OSCCAL    _SFR_MEM8(0x6F)

/* 2-wire Serial Interface Bit Rate Register */
#define TWBR      _SFR_MEM8(0x70)

/* 2-wire Serial Interface Status Register */
#define TWSR      _SFR_MEM8(0x71)

/* 2-wire Serial Interface Address Register */
#define TWAR      _SFR_MEM8(0x72)

/* 2-wire Serial Interface Data Register */
#define TWDR      _SFR_MEM8(0x73)

/* 2-wire Serial Interface Control Register */
#define TWCR      _SFR_MEM8(0x74)

/* Time Counter 1 Output Compare Register C */
#define OCR1C     _SFR_MEM16(0x78)
#define OCR1CL    _SFR_MEM8(0x78)
#define OCR1CH    _SFR_MEM8(0x79)

/* Timer/Counter 1 Control Register C */
#define TCCR1C    _SFR_MEM8(0x7A)

/* Extended Timer Interrupt Flag Register */
#define ETIFR     _SFR_MEM8(0x7C)

/* Extended Timer Interrupt Mask Register */
#define ETIMSK    _SFR_MEM8(0x7D)

/* Timer/Counter 3 Input Capture Register */
#define ICR3      _SFR_MEM16(0x80)
#define ICR3L     _SFR_MEM8(0x80)
#define ICR3H     _SFR_MEM8(0x81)

/* Timer/Counter 3 Output Compare Register C */
#define OCR3C     _SFR_MEM16(0x82)
#define OCR3CL    _SFR_MEM8(0x82)
#define OCR3CH    _SFR_MEM8(0x83)

/* Timer/Counter 3 Output Compare Register B */
#define OCR3B     _SFR_MEM16(0x84)
#define OCR3BL    _SFR_MEM8(0x84)
#define OCR3BH    _SFR_MEM8(0x85)

/* Timer/Counter 3 Output Compare Register A */
#define OCR3A     _SFR_MEM16(0x86)
#define OCR3AL    _SFR_MEM8(0x86)
#define OCR3AH    _SFR_MEM8(0x87)

/* Timer/Counter 3 Counter Register */
#define TCNT3     _SFR_MEM16(0x88)
#define TCNT3L    _SFR_MEM8(0x88)
#define TCNT3H    _SFR_MEM8(0x89)

/* Timer/Counter 3 Control Register B */
#define TCCR3B    _SFR_MEM8(0x8A)

/* Timer/Counter 3 Control Register A */
#define TCCR3A    _SFR_MEM8(0x8B)

/* Timer/Counter 3 Control Register C */
#define TCCR3C    _SFR_MEM8(0x8C)

/* USART0 Baud Rate Register High */
#define UBRR0H    _SFR_MEM8(0x90)

/* USART0 Control and Status Register C */
#define UCSR0C    _SFR_MEM8(0x95)

/* USART1 Baud Rate Register High */
#define UBRR1H    _SFR_MEM8(0x98)

/* USART1 Baud Rate Register Low*/
#define UBRR1L    _SFR_MEM8(0x99)

/* USART1 Control and Status Register B */
#define UCSR1B    _SFR_MEM8(0x9A)

/* USART1 Control and Status Register A */
#define UCSR1A    _SFR_MEM8(0x9B)

/* USART1 I/O Data Register */
#define UDR1      _SFR_MEM8(0x9C)

/* USART1 Control and Status Register C */
#define UCSR1C    _SFR_MEM8(0x9D)

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

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(14)
#define SIG_OVERFLOW1			_VECTOR(14)

/* Timer/Counter0 Compare Match */
#define TIMER0_COMP_vect		_VECTOR(15)
#define SIG_OUTPUT_COMPARE0		_VECTOR(15)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(16)
#define SIG_OVERFLOW0			_VECTOR(16)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(17)
#define SIG_SPI				_VECTOR(17)

/* USART0, Rx Complete */
#define USART0_RX_vect			_VECTOR(18)
#define SIG_USART0_RECV			_VECTOR(18)
#define SIG_UART0_RECV			_VECTOR(18)

/* USART0 Data Register Empty */
#define USART0_UDRE_vect		_VECTOR(19)
#define SIG_USART0_DATA			_VECTOR(19)
#define SIG_UART0_DATA			_VECTOR(19)

/* USART0, Tx Complete */
#define USART0_TX_vect			_VECTOR(20)
#define SIG_USART0_TRANS		_VECTOR(20)
#define SIG_UART0_TRANS			_VECTOR(20)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(21)
#define SIG_ADC				_VECTOR(21)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(22)
#define SIG_EEPROM_READY		_VECTOR(22)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(23)
#define SIG_COMPARATOR			_VECTOR(23)

/* Timer/Counter1 Compare Match C */
#define TIMER1_COMPC_vect		_VECTOR(24)
#define SIG_OUTPUT_COMPARE1C		_VECTOR(24)

/* Timer/Counter3 Capture Event */
#define TIMER3_CAPT_vect		_VECTOR(25)
#define SIG_INPUT_CAPTURE3		_VECTOR(25)

/* Timer/Counter3 Compare Match A */
#define TIMER3_COMPA_vect		_VECTOR(26)
#define SIG_OUTPUT_COMPARE3A		_VECTOR(26)

/* Timer/Counter3 Compare Match B */
#define TIMER3_COMPB_vect		_VECTOR(27)
#define SIG_OUTPUT_COMPARE3B		_VECTOR(27)

/* Timer/Counter3 Compare Match C */
#define TIMER3_COMPC_vect		_VECTOR(28)
#define SIG_OUTPUT_COMPARE3C		_VECTOR(28)

/* Timer/Counter3 Overflow */
#define TIMER3_OVF_vect			_VECTOR(29)
#define SIG_OVERFLOW3			_VECTOR(29)

/* USART1, Rx Complete */
#define USART1_RX_vect			_VECTOR(30)
#define SIG_USART1_RECV			_VECTOR(30)
#define SIG_UART1_RECV			_VECTOR(30)

/* USART1, Data Register Empty */
#define USART1_UDRE_vect		_VECTOR(31)
#define SIG_USART1_DATA			_VECTOR(31)
#define SIG_UART1_DATA			_VECTOR(31)

/* USART1, Tx Complete */
#define USART1_TX_vect			_VECTOR(32)
#define SIG_USART1_TRANS		_VECTOR(32)
#define SIG_UART1_TRANS			_VECTOR(32)

/* 2-wire Serial Interface */
#define TWI_vect			_VECTOR(33)
#define SIG_2WIRE_SERIAL		_VECTOR(33)

/* Store Program Memory Read */
#define SPM_READY_vect			_VECTOR(34)
#define SIG_SPM_READY			_VECTOR(34)

#define _VECTORS_SIZE 140

/*
   The Register Bit names are represented by their bit number (0-7).
*/

/* 2-wire Control Register - TWCR */
#define    TWINT        7
#define    TWEA         6
#define    TWSTA        5
#define    TWSTO        4
#define    TWWC         3
#define    TWEN         2
#define    TWIE         0

/* 2-wire Address Register - TWAR */
#define    TWA6         7
#define    TWA5         6
#define    TWA4         5
#define    TWA3         4
#define    TWA2         3
#define    TWA1         2
#define    TWA0         1
#define    TWGCE        0

/* 2-wire Status Register - TWSR */
#define    TWS7         7
#define    TWS6         6
#define    TWS5         5
#define    TWS4         4
#define    TWS3         3
#define    TWPS1        1
#define    TWPS0        0

/* External Memory Control Register A - XMCRA */
#define    SRL2         6
#define    SRL1         5
#define    SRL0         4
#define    SRW01        3
#define    SRW00        2
#define    SRW11        1

/* External Memory Control Register B - XMCRA */
#define    XMBK         7
#define    XMM2         2
#define    XMM1         1
#define    XMM0         0

/* XDIV Divide control register - XDIV */
#define    XDIVEN       7
#define    XDIV6        6
#define    XDIV5        5
#define    XDIV4        4
#define    XDIV3        3
#define    XDIV2        2
#define    XDIV1        1
#define    XDIV0        0

/* RAM Page Z select register - RAMPZ */
#define     RAMPZ0      0

/* External Interrupt Control Register A - EICRA */
#define    ISC31        7
#define    ISC30        6
#define    ISC21        5
#define    ISC20        4
#define    ISC11        3
#define    ISC10        2
#define    ISC01        1
#define    ISC00        0

/* External Interrupt Control Register B - EICRB */
#define    ISC71        7
#define    ISC70        6
#define    ISC61        5
#define    ISC60        4
#define    ISC51        3
#define    ISC50        2
#define    ISC41        1
#define    ISC40        0

/* Store Program Memory Control Register - SPMCSR, SPMCR */
#define    SPMIE        7
#define    RWWSB        6
#define    RWWSRE       4
#define    BLBSET       3
#define    PGWRT        2
#define    PGERS        1
#define    SPMEN        0

/* External Interrupt MaSK register - EIMSK */
#define    INT7         7
#define    INT6         6
#define    INT5         5
#define    INT4         4
#define    INT3         3
#define    INT2         2
#define    INT1         1
#define    INT0         0

/* External Interrupt Flag Register - EIFR */
#define    INTF7        7
#define    INTF6        6
#define    INTF5        5
#define    INTF4        4
#define    INTF3        3
#define    INTF2        2
#define    INTF1        1
#define    INTF0        0

/* Timer/Counter Interrupt MaSK register - TIMSK */
#define    OCIE2        7
#define    TOIE2        6
#define    TICIE1       5
#define    OCIE1A       4
#define    OCIE1B       3
#define    TOIE1        2
#define    OCIE0        1
#define    TOIE0        0

/* Timer/Counter Interrupt Flag Register - TIFR */
#define    OCF2         7
#define    TOV2         6
#define    ICF1         5
#define    OCF1A        4
#define    OCF1B        3
#define    TOV1         2
#define    OCF0         1
#define    TOV0         0

/* Extended Timer Interrupt MaSK register - ETIMSK */
#define    TICIE3       5
#define    OCIE3A       4
#define    OCIE3B       3
#define    TOIE3        2
#define    OCIE3C       1
#define    OCIE1C       0

/* Extended Timer Interrupt Flag Register - ETIFR */
#define    ICF3         5
#define    OCF3A        4
#define    OCF3B        3
#define    TOV3         2
#define    OCF3C        1
#define    OCF1C        0

/* MCU general Control Register - MCUCR */
#define    SRE          7
#define    SRW          6
#define    SRW10        6       /* new name in datasheet (2467E-AVR-05/02) */
#define    SE           5
#define    SM1          4
#define    SM0          3
#define    SM2          2
#define    IVSEL        1
#define    IVCE         0

/* MCU Status Register - MCUSR, MCUCSR */
#define    JTD          7
#define    JTRF         4
#define    WDRF         3
#define    BORF         2
#define    EXTRF        1
#define    PORF         0

/* Timer/Counter Control Register (generic) */
#define    FOC          7
#define    WGM0         6
#define    COM1         5
#define    COM0         4
#define    WGM1         3
#define    CS2          2
#define    CS1          1
#define    CS0          0

/* Timer/Counter 0 Control Register - TCCR0 */
#define    FOC0         7
#define    WGM00        6
#define    COM01        5
#define    COM00        4
#define    WGM01        3
#define    CS02         2
#define    CS01         1
#define    CS00         0

/* Timer/Counter 2 Control Register - TCCR2 */
#define    FOC2         7
#define    WGM20        6
#define    COM21        5
#define    COM20        4
#define    WGM21        3
#define    CS22         2
#define    CS21         1
#define    CS20         0

/* Timer/Counter 0 Asynchronous Control & Status Register - ASSR */
#define    AS0          3
#define    TCN0UB       2
#define    OCR0UB       1
#define    TCR0UB       0

/* Timer/Counter Control Register A (generic) */
#define    COMA1        7
#define    COMA0        6
#define    COMB1        5
#define    COMB0        4
#define    COMC1        3
#define    COMC0        2
#define    WGMA1        1
#define    WGMA0        0

/* Timer/Counter 1 Control and Status Register A - TCCR1A */
#define    COM1A1       7
#define    COM1A0       6
#define    COM1B1       5
#define    COM1B0       4
#define    COM1C1       3
#define    COM1C0       2
#define    WGM11        1
#define    WGM10        0

/* Timer/Counter 3 Control and Status Register A - TCCR3A */
#define    COM3A1       7
#define    COM3A0       6
#define    COM3B1       5
#define    COM3B0       4
#define    COM3C1       3
#define    COM3C0       2
#define    WGM31        1
#define    WGM30        0

/* Timer/Counter Control and Status Register B (generic) */
#define    ICNC         7
#define    ICES         6
#define    WGMB3        4
#define    WGMB2        3
#define    CSB2         2
#define    CSB1         1
#define    CSB0         0

/* Timer/Counter 1 Control and Status Register B - TCCR1B */
#define    ICNC1        7
#define    ICES1        6
#define    WGM13        4
#define    WGM12        3
#define    CS12         2
#define    CS11         1
#define    CS10         0

/* Timer/Counter 3 Control and Status Register B - TCCR3B */
#define    ICNC3        7
#define    ICES3        6
#define    WGM33        4
#define    WGM32        3
#define    CS32         2
#define    CS31         1
#define    CS30         0

/* Timer/Counter Control Register C (generic) */
#define    FOCA         7
#define    FOCB         6
#define    FOCC         5

/* Timer/Counter 3 Control Register C - TCCR3C */
#define    FOC3A        7
#define    FOC3B        6
#define    FOC3C        5

/* Timer/Counter 1 Control Register C - TCCR1C */
#define    FOC1A        7
#define    FOC1B        6
#define    FOC1C        5

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

/* Watchdog Timer Control Register - WDTCR */
#define    WDCE         4
#define    WDE          3
#define    WDP2         2
#define    WDP1         1
#define    WDP0         0

/* 
   The ADHSM bit has been removed from all documentation, 
   as being not needed at all since the comparator has proven 
   to be fast enough even without feeding it more power.
*/

/* Special Function I/O Register - SFIOR */
#define    TSM          7
#define    ACME         3
#define    PUD          2
#define    PSR0         1
#define    PSR321       0

/* SPI Status Register - SPSR */
#define    SPIF         7
#define    WCOL         6
#define    SPI2X        0

/* SPI Control Register - SPCR */
#define    SPIE         7
#define    SPE          6
#define    DORD         5
#define    MSTR         4
#define    CPOL         3
#define    CPHA         2
#define    SPR1         1
#define    SPR0         0

/* USART Register C (generic) */
#define    UMSEL        6
#define    UPM1         5
#define    UPM0         4
#define    USBS         3
#define    UCSZ1        2
#define    UCSZ0        1
#define    UCPOL        0

/* USART1 Register C - UCSR1C */
#define    UMSEL1       6
#define    UPM11        5
#define    UPM10        4
#define    USBS1        3
#define    UCSZ11       2
#define    UCSZ10       1
#define    UCPOL1       0

/* USART0 Register C - UCSR0C */
#define    UMSEL0       6
#define    UPM01        5
#define    UPM00        4
#define    USBS0        3
#define    UCSZ01       2
#define    UCSZ00       1
#define    UCPOL0       0

/* USART Status Register A (generic) */
#define    RXC          7
#define    TXC          6
#define    UDRE         5
#define    FE           4
#define    DOR          3
#define    UPE          2
#define    U2X          1
#define    MPCM         0

/* USART1 Status Register A - UCSR1A */
#define    RXC1         7
#define    TXC1         6
#define    UDRE1        5
#define    FE1          4
#define    DOR1         3
#define    UPE1         2
#define    U2X1         1
#define    MPCM1        0

/* USART0 Status Register A - UCSR0A */
#define    RXC0         7
#define    TXC0         6
#define    UDRE0        5
#define    FE0          4
#define    DOR0         3
#define    UPE0         2
#define    U2X0         1
#define    MPCM0        0

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

/* USART1 Control Register B - UCSR1B */
#define    RXCIE1       7
#define    TXCIE1       6
#define    UDRIE1       5
#define    RXEN1        4
#define    TXEN1        3
#define    UCSZ12       2
#define    RXB81        1
#define    TXB81        0

/* USART0 Control Register B - UCSR0B */
#define    RXCIE0       7
#define    TXCIE0       6
#define    UDRIE0       5
#define    RXEN0        4
#define    TXEN0        3
#define    UCSZ02       2
#define    RXB80        1
#define    TXB80        0

/* Analog Comparator Control and Status Register - ACSR */
#define    ACD          7
#define    ACBG         6
#define    ACO          5
#define    ACI          4
#define    ACIE         3
#define    ACIC         2
#define    ACIS1        1
#define    ACIS0        0

/* ADC Control and status register - ADCSRA */
#define    ADEN         7
#define    ADSC         6
#define    ADFR         5
#define    ADIF         4
#define    ADIE         3
#define    ADPS2        2
#define    ADPS1        1
#define    ADPS0        0

/* ADC Multiplexer select - ADMUX */
#define    REFS1        7
#define    REFS0        6
#define    ADLAR        5
#define    MUX4         4
#define    MUX3         3
#define    MUX2         2
#define    MUX1         1
#define    MUX0         0

/* Port A Data Register - PORTA */
#define    PA7       7
#define    PA6       6
#define    PA5       5
#define    PA4       4
#define    PA3       3
#define    PA2       2
#define    PA1       1
#define    PA0       0

/* Port A Data Direction Register - DDRA */
#define    DDA7         7
#define    DDA6         6
#define    DDA5         5
#define    DDA4         4
#define    DDA3         3
#define    DDA2         2
#define    DDA1         1
#define    DDA0         0

/* Port A Input Pins - PINA */
#define    PINA7        7
#define    PINA6        6
#define    PINA5        5
#define    PINA4        4
#define    PINA3        3
#define    PINA2        2 
#define    PINA1        1
#define    PINA0        0

/* Port B Data Register - PORTB */
#define    PB7       7
#define    PB6       6
#define    PB5       5
#define    PB4       4
#define    PB3       3
#define    PB2       2
#define    PB1       1
#define    PB0       0

/* Port B Data Direction Register - DDRB */
#define    DDB7         7
#define    DDB6         6
#define    DDB5         5
#define    DDB4         4
#define    DDB3         3
#define    DDB2         2
#define    DDB1         1
#define    DDB0         0

/* Port B Input Pins - PINB */
#define    PINB7        7
#define    PINB6        6
#define    PINB5        5
#define    PINB4        4
#define    PINB3        3
#define    PINB2        2 
#define    PINB1        1
#define    PINB0        0

/* Port C Data Register - PORTC */
#define    PC7       7
#define    PC6       6
#define    PC5       5
#define    PC4       4
#define    PC3       3
#define    PC2       2
#define    PC1       1
#define    PC0       0

/* Port C Data Direction Register - DDRC */
#define    DDC7         7
#define    DDC6         6
#define    DDC5         5
#define    DDC4         4
#define    DDC3         3
#define    DDC2         2
#define    DDC1         1
#define    DDC0         0

/* Port C Input Pins - PINC */
#define    PINC7        7
#define    PINC6        6
#define    PINC5        5
#define    PINC4        4
#define    PINC3        3
#define    PINC2        2 
#define    PINC1        1
#define    PINC0        0

/* Port D Data Register - PORTD */
#define    PD7       7
#define    PD6       6
#define    PD5       5
#define    PD4       4
#define    PD3       3
#define    PD2       2
#define    PD1       1
#define    PD0       0

/* Port D Data Direction Register - DDRD */
#define    DDD7         7
#define    DDD6         6
#define    DDD5         5
#define    DDD4         4
#define    DDD3         3
#define    DDD2         2
#define    DDD1         1
#define    DDD0         0

/* Port D Input Pins - PIND */
#define    PIND7        7
#define    PIND6        6
#define    PIND5        5
#define    PIND4        4
#define    PIND3        3
#define    PIND2        2 
#define    PIND1        1
#define    PIND0        0

/* Port E Data Register - PORTE */
#define    PE7       7
#define    PE6       6
#define    PE5       5
#define    PE4       4
#define    PE3       3
#define    PE2       2
#define    PE1       1
#define    PE0       0

/* Port E Data Direction Register - DDRE */
#define    DDE7         7
#define    DDE6         6
#define    DDE5         5
#define    DDE4         4
#define    DDE3         3
#define    DDE2         2
#define    DDE1         1
#define    DDE0         0

/* Port E Input Pins - PINE */
#define    PINE7        7
#define    PINE6        6
#define    PINE5        5
#define    PINE4        4
#define    PINE3        3
#define    PINE2        2 
#define    PINE1        1
#define    PINE0        0

/* Port F Data Register - PORTF */
#define    PF7       7
#define    PF6       6
#define    PF5       5
#define    PF4       4
#define    PF3       3
#define    PF2       2
#define    PF1       1
#define    PF0       0

/* Port F Data Direction Register - DDRF */
#define    DDF7         7
#define    DDF6         6
#define    DDF5         5
#define    DDF4         4
#define    DDF3         3
#define    DDF2         2
#define    DDF1         1
#define    DDF0         0

/* Port F Input Pins - PINF */
#define    PINF7        7
#define    PINF6        6
#define    PINF5        5
#define    PINF4        4
#define    PINF3        3
#define    PINF2        2 
#define    PINF1        1
#define    PINF0        0

/* Port G Data Register - PORTG */
#define    PG4       4
#define    PG3       3
#define    PG2       2
#define    PG1       1
#define    PG0       0

/* Port G Data Direction Register - DDRG */
#define    DDG4         4
#define    DDG3         3
#define    DDG2         2
#define    DDG1         1
#define    DDG0         0

/* Port G Input Pins - PING */
#define    PING4        4
#define    PING3        3
#define    PING2        2 
#define    PING1        1
#define    PING0        0

/* EEPROM Control Register */
#define    EERIE        3
#define    EEMWE        2
#define    EEWE         1
#define    EERE         0

/* Constants */
#define SPM_PAGESIZE 256
#define RAMEND     0x10FF     /* Last On-Chip SRAM Location */
#define XRAMEND    0xFFFF
#define E2END      0x0FFF
#define E2PAGESIZE 8
#define FLASHEND   0x1FFFF


/* Fuses */

#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_CKSEL2      (unsigned char)~_BV(2)
#define FUSE_CKSEL3      (unsigned char)~_BV(3)
#define FUSE_SUT0        (unsigned char)~_BV(4)
#define FUSE_SUT1        (unsigned char)~_BV(5)
#define FUSE_BODEN       (unsigned char)~_BV(6)
#define FUSE_BODLEVEL    (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_CKSEL1 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0)

/* High Fuse Byte */
#define FUSE_BOOTRST     (unsigned char)~_BV(0)
#define FUSE_BOOTSZ0     (unsigned char)~_BV(1)
#define FUSE_BOOTSZ1     (unsigned char)~_BV(2)
#define FUSE_EESAVE      (unsigned char)~_BV(3)
#define FUSE_CKOPT       (unsigned char)~_BV(4)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_JTAGEN      (unsigned char)~_BV(6)
#define FUSE_OCDEN       (unsigned char)~_BV(7)
#define HFUSE_DEFAULT (FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_SPIEN & FUSE_JTAGEN)

/* Extended Fuse Byte */
#define FUSE_WDTON       (unsigned char)~_BV(0)
#define FUSE_M103C       (unsigned char)~_BV(1)
#define EFUSE_DEFAULT (FUSE_M103C)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x97
#define SIGNATURE_2 0x02


#endif /* _AVR_IOM128_H_ */
