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

/* $Id: iom163.h,v 1.14.2.5 2008/10/17 23:27:47 arcanum Exp $ */

/* avr/iom163.h - definitions for ATmega163 */

#ifndef _AVR_IOM163_H_
#define _AVR_IOM163_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom163.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

#define TWBR	_SFR_IO8(0x00)
#define TWSR	_SFR_IO8(0x01)
#define TWAR	_SFR_IO8(0x02)
#define TWDR	_SFR_IO8(0x03)

/* ADC */
#ifndef __ASSEMBLER__
#define ADC 	_SFR_IO16(0x04)
#endif
#define ADCW	_SFR_IO16(0x04)
#define ADCL	_SFR_IO8(0x04)
#define ADCH	_SFR_IO8(0x05)
#define ADCSR	_SFR_IO8(0x06)
#define ADMUX	_SFR_IO8(0x07)

/* analog comparator */
#define ACSR	_SFR_IO8(0x08)

/* UART */
#define UBRR	_SFR_IO8(0x09)
#define UCSRB	_SFR_IO8(0x0A)
#define UCSRA	_SFR_IO8(0x0B)
#define UDR	_SFR_IO8(0x0C)

/* SPI */
#define SPCR	_SFR_IO8(0x0D)
#define SPSR	_SFR_IO8(0x0E)
#define SPDR	_SFR_IO8(0x0F)

/* Port D */
#define PIND	_SFR_IO8(0x10)
#define DDRD	_SFR_IO8(0x11)
#define PORTD	_SFR_IO8(0x12)

/* Port C */
#define PINC	_SFR_IO8(0x13)
#define DDRC	_SFR_IO8(0x14)
#define PORTC	_SFR_IO8(0x15)

/* Port B */
#define PINB	_SFR_IO8(0x16)
#define DDRB	_SFR_IO8(0x17)
#define PORTB	_SFR_IO8(0x18)

/* Port A */
#define PINA	_SFR_IO8(0x19)
#define DDRA	_SFR_IO8(0x1A)
#define PORTA	_SFR_IO8(0x1B)

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO16(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEARH	_SFR_IO8(0x1F)

#define UBRRHI	_SFR_IO8(0x20)

#define WDTCR	_SFR_IO8(0x21)

#define ASSR	_SFR_IO8(0x22)

/* Timer 2 */
#define OCR2	_SFR_IO8(0x23)
#define TCNT2	_SFR_IO8(0x24)
#define TCCR2	_SFR_IO8(0x25)

/* Timer 1 */
#define ICR1	_SFR_IO16(0x26)
#define ICR1L	_SFR_IO8(0x26)
#define ICR1H	_SFR_IO8(0x27)
#define OCR1B	_SFR_IO16(0x28)
#define OCR1BL	_SFR_IO8(0x28)
#define OCR1BH	_SFR_IO8(0x29)
#define OCR1A	_SFR_IO16(0x2A)
#define OCR1AL	_SFR_IO8(0x2A)
#define OCR1AH	_SFR_IO8(0x2B)
#define TCNT1	_SFR_IO16(0x2C)
#define TCNT1L	_SFR_IO8(0x2C)
#define TCNT1H	_SFR_IO8(0x2D)
#define TCCR1B	_SFR_IO8(0x2E)
#define TCCR1A	_SFR_IO8(0x2F)

#define SFIOR	_SFR_IO8(0x30)

#define OSCCAL	_SFR_IO8(0x31)

/* Timer 0 */
#define TCNT0	_SFR_IO8(0x32)
#define TCCR0	_SFR_IO8(0x33)

#define MCUSR	_SFR_IO8(0x34)
#define MCUCR	_SFR_IO8(0x35)

#define TWCR	_SFR_IO8(0x36)

#define SPMCR	_SFR_IO8(0x37)

#define TIFR	_SFR_IO8(0x38)
#define TIMSK	_SFR_IO8(0x39)

#define GIFR	_SFR_IO8(0x3A)
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

/* Timer/Counter2 Compare Match */
#define TIMER2_COMP_vect		_VECTOR(3)
#define SIG_OUTPUT_COMPARE2		_VECTOR(3)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(4)
#define SIG_OVERFLOW2			_VECTOR(4)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(5)
#define SIG_INPUT_CAPTURE1		_VECTOR(5)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(6)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(6)

/* Timer/Counter1 Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(7)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(7)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(8)
#define SIG_OVERFLOW1			_VECTOR(8)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(9)
#define SIG_OVERFLOW0			_VECTOR(9)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(10)
#define SIG_SPI				_VECTOR(10)

/* UART, RX Complete */
#define UART_RX_vect			_VECTOR(11)
#define SIG_UART_RECV			_VECTOR(11)

/* UART Data Register Empty */
#define UART_UDRE_vect			_VECTOR(12)
#define SIG_UART_DATA			_VECTOR(12)

/* UART, TX Complete */
#define UART_TX_vect			_VECTOR(13)
#define SIG_UART_TRANS			_VECTOR(13)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(14)
#define SIG_ADC				_VECTOR(14)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(15)
#define SIG_EEPROM_READY		_VECTOR(15)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(16)
#define SIG_COMPARATOR			_VECTOR(16)

/* 2-Wire Serial Interface */
#define TWI_vect			_VECTOR(17)
#define SIG_2WIRE_SERIAL		_VECTOR(17)

#define _VECTORS_SIZE 72

/* Bit numbers */

/* GIMSK */
#define INT1	7
#define INT0	6
/* bit 5 reserved, undefined */
/* bits 4-0 reserved */

/* GIFR */
#define INTF1	7
#define INTF0	6
/* bits 5-0 reserved */

/* TIMSK */
#define OCIE2	7
#define TOIE2	6
#define TICIE1	5
#define OCIE1A	4
#define OCIE1B	3
#define TOIE1	2
/* bit 1 reserved */
#define TOIE0	0

/* TIFR */
#define OCF2	7
#define TOV2	6
#define ICF1	5
#define OCF1A	4
#define OCF1B	3
#define TOV1	2
/* bit 1 reserved, undefined */
#define TOV0	0

/* SPMCR */
/* bit 7 reserved */
#define ASB	6
/* bit 5 reserved */
#define ASRE	4
#define BLBSET	3
#define PGWRT	2
#define PGERS	1
#define SPMEN	0

/* TWCR */
#define TWINT	7
#define TWEA	6
#define TWSTA	5
#define TWSTO	4
#define TWWC	3
#define TWEN	2
/* bit 1 reserved */
#define TWIE	0

/* TWAR */
#define TWGCE	0

/* TWSR */
#define    TWS7         7
#define    TWS6         6
#define    TWS5         5
#define    TWS4         4
#define    TWS3         3
/* bits 2-0 reserved */

/* MCUCR */
/* bit 7 reserved */
#define SE	6
#define SM1	5
#define SM0	4
#define ISC11	3
#define ISC10	2
#define ISC01	1
#define ISC00	0

/* MCUSR */
/* bits 7-4 reserved */
#define WDRF	3
#define BORF	2
#define EXTRF	1
#define PORF	0

/* SFIOR */
/* bits 7-4 reserved */
#define ACME	3
#define PUD	2
#define PSR2	1
#define PSR10	0

/* TCCR0 */
/* bits 7-3 reserved */
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
/* bits 7-4 reserved */
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
/* bits 5-4 reserved */
#define CTC1	3
#define CS12	2
#define CS11	1
#define CS10	0

/* WDTCR */
/* bits 7-5 reserved */
#define WDTOE	4
#define WDE	3
#define WDP2	2
#define WDP1	1
#define WDP0	0

/* PA7-PA0 = ADC7-ADC0 */
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
   PB3 = AIN1
   PB2 = AIN0
   PB1 = T1
   PB0 = T0
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
   PC7 = TOSC2
   PC6 = TOSC1
   PC1 = SDA
   PC0 = SCL
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
   PD7 = OC2
   PD6 = ICP
   PD5 = OC1A
   PD4 = OC1B
   PD3 = INT1
   PD2 = INT0
   PD1 = TXD
   PD0 = RXD
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

/* SPSR */
#define SPIF	7
#define WCOL	6
/* bits 5-1 reserved */
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

/* UCSRA */
#define RXC	7
#define TXC	6
#define UDRE	5
#define FE	4
#define DOR	3
/* bit 2 reserved */
#define U2X	1
#define MPCM	0

/* UCSRB */
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

/* ADCSR */
#define ADEN	7
#define ADSC	6
#define ADFR	5
#define ADIF	4
#define ADIE	3
#define ADPS2	2
#define ADPS1	1
#define ADPS0	0

/* ADMUX */
#define REFS1	7
#define REFS0	6
#define ADLAR	5
#define MUX4	4
#define MUX3	3
#define MUX2	2
#define MUX1	1
#define MUX0	0

/* EEPROM Control Register */
#define EERIE   3
#define EEMWE   2
#define EEWE    1
#define EERE    0

/* Constants */
#define SPM_PAGESIZE 128
#define RAMEND		0x45F
#define XRAMEND		0x45F
#define E2END		0x1FF
#define E2PAGESIZE  0
#define FLASHEND	0x3FFF


/* Fuses */

#define FUSE_MEMORY_SIZE 2

/* Low Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_CKSEL2      (unsigned char)~_BV(2)
#define FUSE_CKSEL3      (unsigned char)~_BV(3)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_BODEN       (unsigned char)~_BV(6)
#define FUSE_BODLEVEL    (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SPIEN)

/* High Fuse Byte */
#define FUSE_BOOTRST     (unsigned char)~_BV(0)
#define FUSE_BOOTSZ0     (unsigned char)~_BV(1)
#define FUSE_BOOTSZ1     (unsigned char)~_BV(2)
#define HFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x94
#define SIGNATURE_2 0x02


#endif /* _AVR_IOM163_H_ */
