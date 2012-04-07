/* Copyright (c) 2002, Marek Michalkiewicz
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

/* $Id: io4414.h,v 1.8.4.3 2008/08/14 00:07:59 arcanum Exp $ */

/* avr/io4414.h - definitions for AT90S4414 */

#ifndef _AVR_IO4414_H_
#define _AVR_IO4414_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "io4414.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* Analog Comparator Control and Status Register */
#define ACSR	_SFR_IO8(0x08)

/* UART Baud Rate Register */
#define UBRR	_SFR_IO8(0x09)

/* UART Control Register */
#define UCR	_SFR_IO8(0x0A)

/* UART Status Register */
#define USR	_SFR_IO8(0x0B)

/* UART I/O Data Register */
#define UDR	_SFR_IO8(0x0C)

/* SPI Control Register */
#define SPCR	_SFR_IO8(0x0D)

/* SPI Status Register */
#define SPSR	_SFR_IO8(0x0E)

/* SPI I/O Data Register */
#define SPDR	_SFR_IO8(0x0F)

/* Input Pins, Port D */
#define PIND	_SFR_IO8(0x10)

/* Data Direction Register, Port D */
#define DDRD	_SFR_IO8(0x11)

/* Data Register, Port D */
#define PORTD	_SFR_IO8(0x12)

/* Input Pins, Port C */
#define PINC	_SFR_IO8(0x13)

/* Data Direction Register, Port C */
#define DDRC	_SFR_IO8(0x14)

/* Data Register, Port C */
#define PORTC	_SFR_IO8(0x15)

/* Input Pins, Port B */
#define PINB	_SFR_IO8(0x16)

/* Data Direction Register, Port B */
#define DDRB	_SFR_IO8(0x17)

/* Data Register, Port B */
#define PORTB	_SFR_IO8(0x18)

/* Input Pins, Port A */
#define PINA	_SFR_IO8(0x19)

/* Data Direction Register, Port A */
#define DDRA	_SFR_IO8(0x1A)

/* Data Register, Port A */
#define PORTA	_SFR_IO8(0x1B)

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO8(0x1E)
#define EEARL	_SFR_IO8(0x1E)

/* Watchdog Timer Control Register */
#define WDTCR	_SFR_IO8(0x21)

/* T/C 1 Input Capture Register */
#define ICR1	_SFR_IO16(0x24)
#define ICR1L	_SFR_IO8(0x24)
#define ICR1H	_SFR_IO8(0x25)

/* Timer/Counter1 Output Compare Register B */
#define OCR1B	_SFR_IO16(0x28)
#define OCR1BL	_SFR_IO8(0x28)
#define OCR1BH	_SFR_IO8(0x29)

/* Timer/Counter1 Output Compare Register A */
#define OCR1A	_SFR_IO16(0x2A)
#define OCR1AL	_SFR_IO8(0x2A)
#define OCR1AH	_SFR_IO8(0x2B)

/* Timer/Counter 1 */
#define TCNT1	_SFR_IO16(0x2C)
#define TCNT1L	_SFR_IO8(0x2C)
#define TCNT1H	_SFR_IO8(0x2D)

/* Timer/Counter 1 Control and Status Register */
#define TCCR1B	_SFR_IO8(0x2E)

/* Timer/Counter 1 Control Register */
#define TCCR1A	_SFR_IO8(0x2F)

/* Timer/Counter 0 */
#define TCNT0	_SFR_IO8(0x32)

/* Timer/Counter 0 Control Register */
#define TCCR0	_SFR_IO8(0x33)

/* MCU general Control Register */
#define MCUCR	_SFR_IO8(0x35)

/* Timer/Counter Interrupt Flag register */
#define TIFR	_SFR_IO8(0x38)

/* Timer/Counter Interrupt MaSK register */
#define TIMSK	_SFR_IO8(0x39)

/* General Interrupt Flag Register */
#define GIFR   _SFR_IO8(0x3A)

/* General Interrupt MaSK register */
#define GIMSK	_SFR_IO8(0x3B)

/* 0x3C..0x3D SP */

/* 0x3F SREG */

/* Interrupt vectors */

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)

/* Timer/Counter Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(3)
#define SIG_INPUT_CAPTURE1		_VECTOR(3)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(4)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(4)

/* Timer/Counter1 Compare MatchB */
#define TIMER1_COMPB_vect		_VECTOR(5)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(5)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(6)
#define SIG_OVERFLOW1			_VECTOR(6)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(7)
#define SIG_OVERFLOW0			_VECTOR(7)

/* Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(8)
#define SIG_SPI				_VECTOR(8)

/* UART, Rx Complete */
#define UART_RX_vect			_VECTOR(9)
#define SIG_UART_RECV			_VECTOR(9)

/* UART Data Register Empty */
#define UART_UDRE_vect			_VECTOR(10)
#define SIG_UART_DATA			_VECTOR(10)

/* UART, Tx Complete */
#define UART_TX_vect			_VECTOR(11)
#define SIG_UART_TRANS			_VECTOR(11)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(12)
#define SIG_COMPARATOR			_VECTOR(12)

#define _VECTORS_SIZE 26

/*
   The Register Bit names are represented by their bit number (0-7).
*/

/* General Interrupt MaSK register */
#define    INT1         7
#define    INT0         6

/* General Interrupt Flag Register */
#define    INTF1        7
#define    INTF0        6

/* Timer/Counter Interrupt MaSK register */
#define    TOIE1        7
#define    OCIE1A       6
#define    OCIE1B       5
#define    TICIE1       3
#define    TOIE0        1

/* Timer/Counter Interrupt Flag register */
#define    TOV1         7
#define    OCF1A        6
#define    OCF1B        5
#define    ICF1         3
#define    TOV0         1

/* MCU general Control Register */
#define    SRE          7
#define    SRW          6
#define    SE           5
#define    SM           4
#define    ISC11        3
#define    ISC10        2
#define    ISC01        1
#define    ISC00        0

/* Timer/Counter 0 Control Register */
#define    CS02         2
#define    CS01         1
#define    CS00         0

/* Timer/Counter 1 Control Register */
#define    COM1A1       7
#define    COM1A0       6
#define    COM1B1       5
#define    COM1B0       4
#define    PWM11        1
#define    PWM10        0

/* Timer/Counter 1 Control and Status Register */
#define    ICNC1        7
#define    ICES1        6
#define    CTC1         3
#define    CS12         2
#define    CS11         1
#define    CS10         0

/* Watchdog Timer Control Register */
#define    WDTOE        4
#define    WDE          3
#define    WDP2         2
#define    WDP1         1
#define    WDP0         0

/* Data Register, Port A */
#define    PA7          7
#define    PA6          6
#define    PA5          5
#define    PA4          4
#define    PA3          3
#define    PA2          2
#define    PA1          1
#define    PA0          0

/* Data Direction Register, Port A */
#define    DDA7         7
#define    DDA6         6
#define    DDA5         5
#define    DDA4         4
#define    DDA3         3
#define    DDA2         2
#define    DDA1         1
#define    DDA0         0

/* Input Pins, Port A */
#define    PINA7        7
#define    PINA6        6
#define    PINA5        5
#define    PINA4        4
#define    PINA3        3
#define    PINA2        2
#define    PINA1        1
#define    PINA0        0

/* Data Register, Port B */
#define    PB7          7
#define    PB6          6
#define    PB5          5
#define    PB4          4
#define    PB3          3
#define    PB2          2
#define    PB1          1
#define    PB0          0

/* Data Direction Register, Port B */
#define    DDB7         7
#define    DDB6         6
#define    DDB5         5
#define    DDB4         4
#define    DDB3         3
#define    DDB2         2
#define    DDB1         1
#define    DDB0         0

/* Input Pins, Port B */
#define    PINB7        7
#define    PINB6        6
#define    PINB5        5
#define    PINB4        4
#define    PINB3        3
#define    PINB2        2
#define    PINB1        1
#define    PINB0        0

/* Data Register, Port C */
#define    PC7          7
#define    PC6          6
#define    PC5          5
#define    PC4          4
#define    PC3          3
#define    PC2          2
#define    PC1          1
#define    PC0          0

/* Data Direction Register, Port C */
#define    DDC7         7
#define    DDC6         6
#define    DDC5         5
#define    DDC4         4
#define    DDC3         3
#define    DDC2         2
#define    DDC1         1
#define    DDC0         0

/* Input Pins, Port C */
#define    PINC7        7
#define    PINC6        6
#define    PINC5        5
#define    PINC4        4
#define    PINC3        3
#define    PINC2        2
#define    PINC1        1
#define    PINC0        0

/* Data Register, Port D */
#define    PD7          7
#define    PD6          6
#define    PD5          5
#define    PD4          4
#define    PD3          3
#define    PD2          2
#define    PD1          1
#define    PD0          0

/* Data Direction Register, Port D */
#define    DDD7         7
#define    DDD6         6
#define    DDD5         5
#define    DDD4         4
#define    DDD3         3
#define    DDD2         2
#define    DDD1         1
#define    DDD0         0

/* Input Pins, Port D */
#define    PIND7        7
#define    PIND6        6
#define    PIND5        5
#define    PIND4        4
#define    PIND3        3
#define    PIND2        2
#define    PIND1        1
#define    PIND0        0

/* SPI Status Register */
#define    SPIF         7
#define    WCOL         6

/* SPI Control Register */
#define    SPIE         7
#define    SPE          6
#define    DORD         5
#define    MSTR         4
#define    CPOL         3
#define    CPHA         2
#define    SPR1         1
#define    SPR0         0

/* UART Status Register */
#define    RXC          7
#define    TXC          6
#define    UDRE         5
#define    FE           4
#define    DOR          3

/* UART Control Register */
#define    RXCIE        7
#define    TXCIE        6
#define    UDRIE        5
#define    RXEN         4
#define    TXEN         3
#define    CHR9         2
#define    RXB8         1
#define    TXB8         0

/* Analog Comparator Control and Status Register */
#define    ACD          7
#define    ACO          5
#define    ACI          4
#define    ACIE         3
#define    ACIC         2
#define    ACIS1        1
#define    ACIS0        0

/* EEPROM Control Register */
#define    EERIE        3
#define    EEMWE        2
#define    EEWE         1
#define    EERE         0

/* Constants        */
#define RAMEND       0x15F    /* Last On-Chip SRAM Location */
#define XRAMEND      0xFFFF
#define E2END        0xFF
#define E2PAGESIZE   0
#define FLASHEND     0xFFF


/* Fuses */
#define FUSE_MEMORY_SIZE 1

/* Low Fuse Byte */
#define FUSE_SPIEN (unsigned char)~_BV(1)  /* Serial Program Downloading Enabled */
#define FUSE_FSTRT (unsigned char)~_BV(2)  /* Short Start-up time selected */
#define LFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x92
#define SIGNATURE_2 0x01


#endif /* _AVR_IO4414_H_ */
