/* Copyright (c) 2002, Nils Kristian Strom <nilsst@omegav.ntnu.no>
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

/* $Id: iom162.h,v 1.13.2.5 2008/10/17 23:27:47 arcanum Exp $ */

/* iom162.h - definitions for ATmega162 */

#ifndef _AVR_IOM162_H_
#define _AVR_IOM162_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom162.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* Memory mapped I/O registers */

/* Timer/Counter3 Control Register A */
#define TCCR3A	_SFR_MEM8(0x8B)

/* Timer/Counter3 Control Register B */
#define TCCR3B	_SFR_MEM8(0x8A)

/* Timer/Counter3 - Counter Register */
#define TCNT3H	_SFR_MEM8(0x89)
#define TCNT3L	_SFR_MEM8(0x88)
#define TCNT3	_SFR_MEM16(0x88)

/* Timer/Counter3 - Output Compare Register A */
#define OCR3AH	_SFR_MEM8(0x87)
#define OCR3AL	_SFR_MEM8(0x86)
#define OCR3A	_SFR_MEM16(0x86)

/* Timer/Counter3 - Output Compare Register B */
#define OCR3BH	_SFR_MEM8(0x85)
#define OCR3BL	_SFR_MEM8(0x84)
#define OCR3B	_SFR_MEM16(0x84)

/* Timer/Counter3 - Input Capture Register */
#define ICR3H	_SFR_MEM8(0x81)
#define ICR3L	_SFR_MEM8(0x80)
#define ICR3	_SFR_MEM16(0x80)

/* Extended Timer/Counter Interrupt Mask */
#define ETIMSK	_SFR_MEM8(0x7D)

/* Extended Timer/Counter Interrupt Flag Register */
#define ETIFR	_SFR_MEM8(0x7C)

/* Pin Change Mask Register 1 */
#define PCMSK1	_SFR_MEM8(0x6C)

/* Pin Change Mask Register 0 */
#define PCMSK0	_SFR_MEM8(0x6B)

/* Clock PRescale */
#define CLKPR	_SFR_MEM8(0x61)


/* Standard I/O registers */

/* 0x3F SREG */
/* 0x3D..0x3E SP */
#define	UBRR1H  _SFR_IO8(0x3C)  /* USART 1 Baud Rate Register High Byte, Shared with UCSR1C */
#define UCSR1C  _SFR_IO8(0x3C)  /* USART 1 Control and Status Register, Shared with UBRR1H */
#define GICR    _SFR_IO8(0x3B)	/* General Interrupt Control Register */
#define GIFR	_SFR_IO8(0x3A)	/* General Interrupt Flag Register */
#define TIMSK	_SFR_IO8(0x39)	/* Timer Interrupt Mask */
#define TIFR	_SFR_IO8(0x38)	/* Timer Interrupt Flag Register */
#define SPMCR	_SFR_IO8(0x37)	/* Store Program Memory Control Register */
#define EMCUCR	_SFR_IO8(0x36)	/* Extended MCU Control Register */
#define MCUCR	_SFR_IO8(0x35)	/* MCU Control Register */
#define MCUCSR	_SFR_IO8(0x34)	/* MCU Control and Status Register */
#define TCCR0	_SFR_IO8(0x33)	/* Timer/Counter 0 Control Register */
#define TCNT0	_SFR_IO8(0x32)	/* TImer/Counter 0 */
#define OCR0	_SFR_IO8(0x31)	/* Output Compare Register 0 */
#define SFIOR	_SFR_IO8(0x30)	/* Special Function I/O Register */
#define TCCR1A	_SFR_IO8(0x2F)	/* Timer/Counter 1 Control Register A */
#define TCCR1B	_SFR_IO8(0x2E)	/* Timer/Counter 1 Control Register A */
#define TCNT1H	_SFR_IO8(0x2D)	/* Timer/Counter 1 High Byte */
#define TCNT1L	_SFR_IO8(0x2C)	/* Timer/Counter 1 Low Byte */
#define TCNT1	_SFR_IO16(0x2C)	/* Timer/Counter 1 */
#define OCR1AH	_SFR_IO8(0x2B)	/* Timer/Counter 1 Output Compare Register A High Byte */
#define OCR1AL	_SFR_IO8(0x2A)  /* Timer/Counter 1 Output Compare Register A Low Byte */
#define OCR1A	_SFR_IO16(0x2A) /* Timer/Counter 1 Output Compare Register A */
#define OCR1BH	_SFR_IO8(0x29)	/* Timer/Counter 1 Output Compare Register B High Byte */
#define OCR1BL	_SFR_IO8(0x28)	/* Timer/Counter 1 Output Compare Register B Low Byte */
#define OCR1B	_SFR_IO16(0x28)	/* Timer/Counter 1 Output Compare Register B */
#define TCCR2	_SFR_IO8(0x27)	/* Timer/Counter 2 Control Register */
#define ASSR	_SFR_IO8(0x26)	/* Asynchronous Status Register */
#define ICR1H	_SFR_IO8(0x25)	/* Input Capture Register 1 High Byte */
#define ICR1L	_SFR_IO8(0x24)	/* Input Capture Register 1 Low Byte */
#define ICR1	_SFR_IO16(0x24)	/* Input Capture Register 1 */
#define TCNT2	_SFR_IO8(0x23)	/* Timer/Counter 2 */
#define OCR2	_SFR_IO8(0x22)	/* Timer/Counter 2 Output Compare Register */
#define WDTCR	_SFR_IO8(0x21)	/* Watchdow Timer Control Register */
#define UBRR0H	_SFR_IO8(0x20)	/* USART 0 Baud-Rate Register High Byte, Shared with UCSR0C */
#define UCSR0C	_SFR_IO8(0x20)	/* USART 0 Control and Status Register C, Shared with UBRR0H */
#define EEARH	_SFR_IO8(0x1F)  /* EEPROM Address Register High Byte */
#define EEARL	_SFR_IO8(0x1E)  /* EEPROM Address Register Low Byte */
#define EEAR	_SFR_IO16(0x1E) /* EEPROM Address Register */
#define EEDR	_SFR_IO8(0x1D)  /* EEPROM Data Register */
#define EECR	_SFR_IO8(0x1C)  /* EEPROM Control Register */
#define PORTA	_SFR_IO8(0x1B)	/* Port A */
#define DDRA	_SFR_IO8(0x1A)	/* Port A Data Direction Register */
#define PINA	_SFR_IO8(0x19)	/* Port A Pin Register */
#define PORTB	_SFR_IO8(0x18)	/* Port B */
#define DDRB	_SFR_IO8(0x17)	/* Port B Data Direction Register */
#define PINB	_SFR_IO8(0x16)	/* Port B Pin Register */
#define PORTC	_SFR_IO8(0x15)	/* Port C */
#define DDRC	_SFR_IO8(0x14)	/* Port C Data Direction Register */
#define PINC	_SFR_IO8(0x13)	/* Port C Pin Register */
#define PORTD	_SFR_IO8(0x12)	/* Port D */
#define DDRD	_SFR_IO8(0x11)	/* Port D Data Direction Register */
#define PIND	_SFR_IO8(0x10)	/* Port D Pin Register */
#define SPDR	_SFR_IO8(0x0F)  /* SPI Data Register */
#define SPSR	_SFR_IO8(0x0E)	/* SPI Status Register */
#define SPCR	_SFR_IO8(0x0D)	/* SPI Control Register */
#define UDR0	_SFR_IO8(0x0C)	/* USART 0 Data Register */
#define UCSR0A	_SFR_IO8(0x0B)	/* USART 0 Control and Status Register A */
#define UCSR0B	_SFR_IO8(0x0A)	/* USART 0 Control and Status Register B */
#define UBRR0L	_SFR_IO8(0x09)	/* USART 0 Baud-Rate Register Low Byte */
#define ACSR	_SFR_IO8(0x08)	/* Analog Comparator Status Register */
#define PORTE	_SFR_IO8(0x07)	/* Port E */
#define DDRE	_SFR_IO8(0x06)	/* Port E Data Direction Register */
#define PINE	_SFR_IO8(0x05)	/* Port E Pin Register */
#define OSCCAL	_SFR_IO8(0x04) 	/* Oscillator Calibration, Shared with OCDR */
#define OCDR	_SFR_IO8(0x04) 	/* On-Chip Debug Register, Shared with OSCCAL */
#define UDR1	_SFR_IO8(0x03)	/* USART 1 Data Register */
#define UCSR1A	_SFR_IO8(0x02)	/* USART 1 Control and Status Register A */
#define UCSR1B	_SFR_IO8(0x01)	/* USART 1 Control and Status Register B */
#define	UBRR1L  _SFR_IO8(0x00)  /* USART 0 Baud Rate Register High Byte */
 

/* Interrupt vectors (byte addresses) */

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)

/* External Interrupt Request 2 */
#define INT2_vect			_VECTOR(3)
#define SIG_INTERRUPT2			_VECTOR(3)

/* Pin Change Interrupt Request 0 */
#define PCINT0_vect			_VECTOR(4)
#define SIG_PIN_CHANGE0			_VECTOR(4)

/* Pin Change Interrupt Request 1 */
#define PCINT1_vect			_VECTOR(5)
#define SIG_PIN_CHANGE1			_VECTOR(5)

/* Timer/Counter3 Capture Event */
#define TIMER3_CAPT_vect		_VECTOR(6)
#define SIG_INPUT_CAPTURE3		_VECTOR(6)

/* Timer/Counter3 Compare Match A */
#define TIMER3_COMPA_vect		_VECTOR(7)
#define SIG_OUTPUT_COMPARE3A		_VECTOR(7)

/* Timer/Counter3 Compare Match B */
#define TIMER3_COMPB_vect		_VECTOR(8)
#define SIG_OUTPUT_COMPARE3B		_VECTOR(8)

/* Timer/Counter3 Overflow */
#define TIMER3_OVF_vect			_VECTOR(9)
#define SIG_OVERFLOW3			_VECTOR(9)

/* Timer/Counter2 Compare Match */
#define TIMER2_COMP_vect		_VECTOR(10)
#define SIG_OUTPUT_COMPARE2		_VECTOR(10)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(11)
#define SIG_OVERFLOW2			_VECTOR(11)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(12)
#define SIG_INPUT_CAPTURE1		_VECTOR(12)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(13)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(13)

/* Timer/Counter Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(14)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(14)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(15)
#define SIG_OVERFLOW1			_VECTOR(15)

/* Timer/Counter0 Compare Match */
#define TIMER0_COMP_vect		_VECTOR(16)
#define SIG_OUTPUT_COMPARE0		_VECTOR(16)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(17)
#define SIG_OVERFLOW0			_VECTOR(17)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(18)
#define SIG_SPI				_VECTOR(18)

/* USART0, Rx Complete */
#define USART0_RXC_vect			_VECTOR(19)
#define SIG_USART0_RECV			_VECTOR(19)

/* USART1, Rx Complete */
#define USART1_RXC_vect			_VECTOR(20)
#define SIG_USART1_RECV			_VECTOR(20)

/* USART0 Data register Empty */
#define USART0_UDRE_vect		_VECTOR(21)
#define SIG_USART0_DATA			_VECTOR(21)

/* USART1, Data register Empty */
#define USART1_UDRE_vect		_VECTOR(22)
#define SIG_USART1_DATA			_VECTOR(22)

/* USART0, Tx Complete */
#define USART0_TXC_vect			_VECTOR(23)
#define SIG_USART0_TRANS		_VECTOR(23)

/* USART1, Tx Complete */
#define USART1_TXC_vect			_VECTOR(24)
#define SIG_USART1_TRANS		_VECTOR(24)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(25)
#define SIG_EEPROM_READY		_VECTOR(25)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(26)
#define SIG_COMPARATOR			_VECTOR(26)

/* Store Program Memory Read */
#define SPM_RDY_vect			_VECTOR(27)
#define SIG_SPM_READY			_VECTOR(27)

#define _VECTORS_SIZE 112 /* = (num vec+1) * 4 */





/* TCCR3B bit definitions, memory mapped I/O */

#define ICNC3	7
#define ICES3	6
#define WGM33	4
#define WGM32	3
#define CS32	2
#define CS31	1
#define CS30	0



/* TCCR3A bit definitions, memory mapped I/O */

#define COM3A1	7
#define COM3A0	6
#define COM3B1	5
#define COM3B0	4
#define FOC3A	3
#define FOC3B	2
#define WGM31	1
#define WGM30	0



/* ETIMSK bit definitions, memory mapped I/O */

#define TICIE3		5
#define OCIE3A		4
#define OCIE3B		3
#define TOIE3		2



/* ETIFR bit definitions, memory mapped I/O */

#define ICF3		5
#define OCF3A		4
#define OCF3B		3
#define TOV3		2



/* PCMSK1 bit definitions, memory mapped I/O */
#define PCINT15	7
#define PCINT14	6
#define PCINT13	5
#define PCINT12	4
#define PCINT11	3
#define PCINT10	2
#define PCINT9	1
#define PCINT8	0



/* PCMSK0 bit definitions, memory mapped I/O */

#define PCINT7	7
#define PCINT6	6
#define PCINT5	5
#define PCINT4	4
#define PCINT3	3
#define PCINT2	2
#define PCINT1	1
#define PCINT0	0



/* CLKPR bit definitions, memory mapped I/O */

#define CLKPCE	7
#define CLKPS3	3
#define CLKPS2	2
#define CLKPS1	1
#define CLKPS0	0



/* SPH bit definitions */

#define SP15	15
#define SP14	14
#define SP13	13
#define SP12	12
#define SP11	11
#define SP10	10
#define SP9	9
#define SP8	8



/* SPL bit definitions */

#define SP7	7
#define SP6	6
#define SP5	5
#define SP4	4
#define SP3	3
#define SP2	2
#define SP1	1
#define SP0	0



/* UBRR1H bit definitions */

#define URSEL1	7
#define UBRR111	3
#define UBRR110	2
#define UBRR19	1
#define UBRR18	0



/* UCSR1C bit definitions */

#define URSEL1	7
#define UMSEL1	6
#define UPM11	5
#define UPM10	4
#define USBS1	3
#define UCSZ11	2
#define UCSZ10	1
#define UCPOL1	0



/* GICR bit definitions */

#define INT1	7
#define INT0	6
#define INT2	5
#define PCIE1	4
#define PCIE0	3
#define IVSEL	1
#define IVCE	0



/* GIFR bit definitions */

#define INTF1	7
#define INTF0	6
#define INTF2	5
#define PCIF1	4
#define PCIF0	3



/* TIMSK bit definitions */

#define TOIE1	7
#define OCIE1A	6
#define OCIE1B  5
#define OCIE2	4
#define TICIE1	3
#define TOIE2	2
#define TOIE0	1
#define OCIE0	0



/* TIFR bit definitions */

#define TOV1	7
#define OCF1A	6
#define OCF1B	5
#define OCF2	4
#define ICF1	3
#define TOV2	2
#define TOV0	1
#define OCF0	0



/* SPMCR bit definitions */ 

#define SPMIE	7
#define RWWSB	6
#define RWWSRE	4
#define BLBSET	3
#define PGWRT	2
#define PGERS	1
#define SPMEN	0



/* EMCUCR bit definitions */

#define SM0	7
#define SRL2	6
#define SRL1	5
#define SRL0	4
#define SRW01	3
#define SRW00	2
#define SRW11	1
#define ISC2	0



/* MCUCR bit definitions */

#define SRE	7
#define SRW10	6
#define SE	5
#define SM1	4
#define ISC11	3
#define ISC10	2
#define ISC01	1
#define ISC00	0



/* MCUCSR bit definitions */

#define JTD	7
#define SM2	5
#define JTRF	4
#define WDRF	3
#define BORF	2
#define EXTRF	1
#define PORF	0



/* TCCR0 bit definitions */

#define FOC0	7
#define WGM00	6
#define COM01	5
#define COM00	4
#define WGM01	3
#define CS02	2
#define CS01	1
#define CS00	0



/* SFIOR bit definitions */

#define TSM	7
#define XMBK	6
#define XMM2	5
#define XMM1	4
#define XMM0	3
#define PUD	2
#define PSR2	1
#define PSR310	0



/* TCCR1A bit definitions */

#define COM1A1  7
#define COM1A0  6
#define COM1B1  5
#define COM1B0  4
#define FOC1A   3
#define FOC1B   2
#define WGM11   1
#define WGM10   0




/* TCCR1B bit definitions */

#define ICNC1	7		/* Input Capture Noise Canceler */
#define ICES1	6		/* Input Capture Edge Select */
#define WGM13	4		/* Waveform Generation Mode 3 */
#define WGM12	3		/* Waveform Generation Mode 2 */
#define CS12	2		/* Clock Select 2 */
#define CS11	1		/* Clock Select 1 */
#define CS10	0		/* Clock Select 0 */



/* TCCR2 bit definitions */

#define FOC2	7
#define WGM20	6
#define COM21	5
#define COM20	4
#define WGM21	3
#define CS22	2
#define CS21	1
#define CS20	0



/* ASSR bit definitions */

#define AS2	3
#define TCON2UB	2
#define OCR2UB	1
#define TCR2UB	0



/* WDTCR bit definitions */

#define WDCE	4
#define WDE	3
#define WDP2	2
#define WDP1	1
#define WDP0	0



/* UBRR0H bif definitions */

#define URSEL0	7
#define UBRR011	3
#define UBRR010	2
#define UBRR09	1
#define UBRR08	0



/* UCSR0C bit definitions */

#define URSEL0	7
#define UMSEL0	6
#define UPM01	5
#define UPM00	4
#define USBS0	3
#define UCSZ01	2
#define UCSZ00	1
#define UCPOL0	0



/* EEARH bit definitions */

#define EEAR8	0



/* EECR bit definitions */

#define EERIE	3
#define EEMWE	2
#define EEWE	1
#define EERE	0



/* PORTA bit definitions */

#define PA7	7
#define PA6	6
#define PA5	5
#define PA4	4
#define PA3	3
#define PA2	2
#define PA1	1
#define PA0	0



/* DDRA bit definitions */

#define DDA7	7
#define DDA6	6
#define DDA5	5
#define DDA4	4
#define DDA3	3
#define DDA2	2
#define DDA1	1
#define DDA0	0



/* PINA bit definitions */

#define PINA7	7
#define PINA6	6
#define PINA5	5
#define PINA4	4
#define PINA3	3
#define PINA2	2
#define PINA1	1
#define PINA0	0


/* PORTB bit definitions */

#define PB7	7
#define PB6	6
#define PB5	5
#define PB4	4
#define PB3	3
#define PB2	2
#define PB1	1
#define PB0	0



/* DDRB bit definitions */

#define DDB7	7
#define DDB6	6
#define DDB5	5
#define DDB4	4
#define DDB3	3
#define DDB2	2
#define DDB1	1
#define DDB0	0



/* PINB bit definitions */

#define PINB7	7
#define PINB6	6
#define PINB5	5
#define PINB4	4
#define PINB3	3
#define PINB2	2
#define PINB1	1
#define PINB0	0



/* PORTC bit definitions */

#define PC7	 7
#define PC6	 6
#define PC5	 5
#define PC4	 4
#define PC3	 3
#define PC2	 2
#define PC1	 1
#define PC0	 0



/* DDRC bit definitions */

#define DDC7	7
#define DDC6	6
#define DDC5	5
#define DDC4	4
#define DDC3	3
#define DDC2	2
#define DDC1	1
#define DDC0	0



/* PINC bit definitions */

#define PINC7	7
#define PINC6	6
#define PINC5	5
#define PINC4	4
#define PINC3	3
#define PINC2	2
#define PINC1	1
#define PINC0	0



/* PORTD bit definitions */

#define PD7	 7
#define PD6	 6
#define PD5	 5
#define PD4	 4
#define PD3	 3
#define PD2	 2
#define PD1	 1
#define PD0	 0



/* DDRD bit definitions */

#define DDD7	7
#define DDD6	6
#define DDD5	5
#define DDD4	4
#define DDD3	3
#define DDD2	2
#define DDD1	1
#define DDD0	0



/* PIND bit definitions */

#define PIND7	7
#define PIND6	6
#define PIND5	5
#define PIND4	4
#define PIND3	3
#define PIND2	2
#define PIND1	1
#define PIND0	0



/* SPSR bit definitions */

#define SPIF	7
#define WCOL	6
#define SPI2X	0



/* SPCR bit definitions */

#define SPIE	7
#define SPE	6
#define DORD	5
#define MSTR	4
#define CPOL	3
#define CPHA	2
#define SPR1	1
#define SPR0	0



/* UCSR0A bit definitions */

#define RXC0	7
#define TXC0	6
#define UDRE0	5
#define FE0	4
#define DOR0	3
#define UPE0	2
#define U2X0	1
#define MPCM0	0



/* UCSR0B bit definitions */

#define RXCIE0	7
#define TXCIE0	6
#define UDRIE0	5
#define RXEN0	4
#define	TXEN0	3
#define UCSZ02 	2
#define RXB80	1
#define TXB80	0



/* ACSR bit definitions */

#define ACD	7
#define ACBG	6
#define ACO	5
#define ACI	4
#define ACIE	3
#define ACIC	2
#define ACIS1	1
#define ACIS0	0



/* PORTE bit definitions */

#define PE2	2
#define PE1	1
#define PE0	0



/* DDRE bit definitions */

#define DDE2	2
#define DDE1	1
#define DDE0	0



/* PINE bit definitions */

#define PINE2	2
#define PINE1	1
#define PINE0	0



/* UCSR1A bit definitions */

#define RXC1	7
#define TXC1	6
#define UDRE1	5
#define FE1	4
#define DOR1	3
#define UPE1    2
#define U2X1	1
#define MPCM1	0



/* UCSR1B bit definitions */

#define RXCIE1	7
#define TXCIE1	6
#define UDRIE1	5
#define RXEN1	4
#define TXEN1	3
#define UCSZ12	2
#define RXB81	1
#define TXB81	0


/* Constants */
#define SPM_PAGESIZE 128
#define RAMEND		0x4FF
#define XRAMEND		0xFFFF
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
#define FUSE_M161C       (unsigned char)~_BV(4)
#define EFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x94
#define SIGNATURE_2 0x04


#endif  /* _AVR_IOM162_H_ */
