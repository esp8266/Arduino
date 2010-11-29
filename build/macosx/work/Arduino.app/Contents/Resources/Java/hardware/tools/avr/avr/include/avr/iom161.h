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

/* $Id: iom161.h,v 1.10.2.5 2008/10/17 23:27:47 arcanum Exp $ */

/* avr/iom161.h - definitions for ATmega161 */

#ifndef _AVR_IOM161_H_
#define _AVR_IOM161_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom161.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* UART1 Baud Rate Register */
#define UBRR1	_SFR_IO8(0x00)

/* UART1 Control and Status Registers */
#define UCSR1B	_SFR_IO8(0x01)
#define UCSR1A	_SFR_IO8(0x02)

/* UART1 I/O Data Register */
#define UDR1	_SFR_IO8(0x03)

/* 0x04 reserved */

/* Input Pins, Port E */
#define PINE	_SFR_IO8(0x05)

/* Data Direction Register, Port E */
#define DDRE	_SFR_IO8(0x06)

/* Data Register, Port E */
#define PORTE	_SFR_IO8(0x07)

/* Analog Comparator Control and Status Register */
#define ACSR	_SFR_IO8(0x08)

/* UART0 Baud Rate Register */
#define UBRR0	_SFR_IO8(0x09)

/* UART0 Control and Status Registers */
#define UCSR0B	_SFR_IO8(0x0A)
#define UCSR0A	_SFR_IO8(0x0B)

/* UART0 I/O Data Register */
#define UDR0	_SFR_IO8(0x0C)

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
#define EEAR	_SFR_IO16(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEARH	_SFR_IO8(0x1F)

/* UART Baud Register HIgh */
#define UBRRH	_SFR_IO8(0x20)

/* Watchdog Timer Control Register */
#define WDTCR	_SFR_IO8(0x21)

/* Timer/Counter2 Output Compare Register */
#define OCR2	_SFR_IO8(0x22)

/* Timer/Counter2 (8-bit) */
#define TCNT2	_SFR_IO8(0x23)

/* Timer/Counter1 Input Capture Register */
#define ICR1	_SFR_IO16(0x24)
#define ICR1L	_SFR_IO8(0x24)
#define ICR1H	_SFR_IO8(0x25)

/* ASynchronous mode Status Register */
#define ASSR	_SFR_IO8(0x26)

/* Timer/Counter2 Control Register */
#define TCCR2	_SFR_IO8(0x27)

/* Timer/Counter1 Output Compare RegisterB */
#define OCR1B	_SFR_IO16(0x28)
#define OCR1BL	_SFR_IO8(0x28)
#define OCR1BH	_SFR_IO8(0x29)

/* Timer/Counter1 Output Compare RegisterA */
#define OCR1A	_SFR_IO16(0x2A)
#define OCR1AL	_SFR_IO8(0x2A)
#define OCR1AH	_SFR_IO8(0x2B)

/* Timer/Counter1 */
#define TCNT1	_SFR_IO16(0x2C)
#define TCNT1L	_SFR_IO8(0x2C)
#define TCNT1H	_SFR_IO8(0x2D)

/* Timer/Counter1 Control Register B */
#define TCCR1B	_SFR_IO8(0x2E)

/* Timer/Counter1 Control Register A */
#define TCCR1A	_SFR_IO8(0x2F)

/* Special Function IO Register */
#define SFIOR	_SFR_IO8(0x30)

/* Timer/Counter0 Output Compare Register */
#define OCR0	_SFR_IO8(0x31)

/* Timer/Counter0 (8-bit) */
#define TCNT0	_SFR_IO8(0x32)

/* Timer/Counter0 Control Register */
#define TCCR0	_SFR_IO8(0x33)

/* MCU general Status Register */
#define MCUSR	_SFR_IO8(0x34)

/* MCU general Control Register */
#define MCUCR	_SFR_IO8(0x35)

/* Extended MCU general Control Register */
#define EMCUCR	_SFR_IO8(0x36)

/* Store Program Memory Control Register */
#define SPMCR	_SFR_IO8(0x37)

/* Timer/Counter Interrupt Flag Register */
#define TIFR	_SFR_IO8(0x38)

/* Timer/Counter Interrupt MaSK Register */
#define TIMSK	_SFR_IO8(0x39)

/* General Interrupt Flag Register */
#define GIFR	_SFR_IO8(0x3A)

/* General Interrupt MaSK register */
#define GIMSK	_SFR_IO8(0x3B)

/* 0x3C reserved */

/* 0x3D..0x3E SP */

/* 0x3F SREG */

/* Interrupt vectors */

/* External Interrupt 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)

/* External Interrupt 2 */
#define INT2_vect			_VECTOR(3)
#define SIG_INTERRUPT2			_VECTOR(3)

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

/* Timer/Counter1 Compare Match B */
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

/* Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(12)
#define SIG_SPI				_VECTOR(12)

/* UART0, Rx Complete */
#define UART0_RX_vect			_VECTOR(13)
#define SIG_UART0_RECV			_VECTOR(13)

/* UART1, Rx Complete */
#define UART1_RX_vect			_VECTOR(14)
#define SIG_UART1_RECV			_VECTOR(14)

/* UART0 Data Register Empty */
#define UART0_UDRE_vect			_VECTOR(15)
#define SIG_UART0_DATA			_VECTOR(15)

/* UART1 Data Register Empty */
#define UART1_UDRE_vect			_VECTOR(16)
#define SIG_UART1_DATA			_VECTOR(16)

/* UART0, Tx Complete */
#define UART0_TX_vect			_VECTOR(17)
#define SIG_UART0_TRANS			_VECTOR(17)

/* UART1, Tx Complete */
#define UART1_TX_vect			_VECTOR(18)
#define SIG_UART1_TRANS			_VECTOR(18)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(19)
#define SIG_EEPROM_READY		_VECTOR(19)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(20)
#define SIG_COMPARATOR			_VECTOR(20)

#define _VECTORS_SIZE 84

/* Bit numbers */

/* GIMSK */
#define INT1	7
#define INT0	6
#define INT2	5

/* GIFR */
#define INTF1	7
#define INTF0	6
#define INTF2	5

/* TIMSK */
#define TOIE1	7
#define OCIE1A	6
#define OCIE1B	5
#define TOIE2	4
#define TICIE1	3
#define OCIE2	2
#define TOIE0	1
#define OCIE0	0

/* TIFR */
#define TOV1	7
#define	OCF1A	6
#define	OCF1B	5
#define TOV2	4
#define ICF1	3
#define OCF2	2
#define TOV0	1
#define OCF0	0

/* MCUCR */
#define SRE	7
#define SRW10	6
#define SE	5
#define SM1	4
#define ISC11	3
#define ISC10	2
#define ISC01	1
#define ISC00	0

/* EMCUCR */
#define SM0	7
#define SRL2	6
#define SRL1	5
#define SRL0	4
#define SRW01	3
#define SRW00	2
#define SRW11	1
#define ISC2	0

/* SPMCR */
#define BLBSET	3
#define PGWRT	2
#define PGERS	1
#define SPMEN	0

/* SFIOR */
#define PSR2	1
#define PSR10	0

/* TCCR0 */
#define FOC0	7
#define PWM0	6
#define COM01	5
#define COM00	4
#define CTC0	3
#define CS02	2
#define CS01	1
#define CS00	0

/* TCCR2 */
#define FOC2	7
#define PWM2	6
#define COM21	5
#define COM20	4
#define CTC2	3
#define CS22	2
#define CS21	1
#define CS20	0

/* ASSR */
#define AS2	3
#define TCN2UB	2
#define OCR2UB	1
#define TCR2UB	0

/* TCCR1A */
#define COM1A1	7
#define COM1A0	6
#define COM1B1	5
#define COM1B0	4
#define FOC1A	3
#define FOC1B	2
#define PWM11	1
#define PWM10	0

/* TCCR1B */
#define ICNC1	7
#define ICES1	6
#define CTC1	3
#define CS12	2
#define CS11	1
#define CS10	0

/* WDTCR */
#define WDTOE	4
#define WDE	3
#define WDP2	2
#define WDP1	1
#define WDP0	0

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
   PB7 = SCK
   PB6 = MISO
   PB5 = MOSI
   PB4 = SS#
   PB3 = TXD1 / AIN1
   PB2 = RXD1 / AIN0
   PB1 = OC2 / T1
   PB0 = OC0 / T0
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
   PD7 = RD#
   PD6 = WR#
   PD5 = TOSC2 / OC1A
   PD4 = TOSC1
   PD3 = INT1
   PD2 = INT0
   PD1 = TXD0
   PD0 = RXD0
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
   PE2 = ALE
   PE1 = OC1B
   PE0 = ICP / INT2
 */

/* PORTE */
#define PE2	2
#define PE1	1
#define PE0	0

/* DDRE */
#define DDE2	2
#define DDE1	1
#define DDE0	0

/* PINE */
#define PINE2	2
#define PINE1	1
#define PINE0	0

/* SPSR */
#define SPIF	7
#define WCOL	6
#define SPI2X	0

/* SPCR */
#define SPIE	7
#define SPE	6
#define DORD	5
#define MSTR	4
#define CPOL	3
#define CPHA	2
#define SPR1	1
#define SPR0	0

/* UCSR0A, UCSR1A */
#define RXC	7
#define TXC	6
#define UDRE	5
#define FE	4
#define DOR	3
#define U2X	1
#define MPCM	0

/* UCSR0B, UCSR1B */
#define RXCIE	7
#define TXCIE	6
#define UDRIE	5
#define RXEN	4
#define	TXEN	3
#define CHR9	2
#define RXB8	1
#define TXB8	0

/* ACSR */
#define ACD	7
#define AINBG	6
#define ACO	5
#define ACI	4
#define ACIE	3
#define ACIC	2
#define ACIS1	1
#define ACIS0	0

/* EEPROM Control Register */
#define    EERIE        3
#define    EEMWE        2
#define    EEWE         1
#define    EERE         0

/* Constants */
#define SPM_PAGESIZE 128
#define RAMEND		0x45F
#define XRAMEND		0xFFFF
#define E2END		0x1FF
#define E2PAGESIZE  0
#define FLASHEND	0x3FFF


/* Fuses */

#define FUSE_MEMORY_SIZE 1

/* Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_CKSEL2      (unsigned char)~_BV(2)
#define FUSE_SUT         (unsigned char)~_BV(4)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_BOOTRST     (unsigned char)~_BV(6)
#define FUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_SPIEN)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x94
#define SIGNATURE_2 0x01


#endif /* _AVR_IOM161_H_ */
