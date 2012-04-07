/* Copyright (c) 2004,2005, Theodore A. Roth
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

/* $Id: iomx8.h,v 1.13 2007/12/12 14:00:49 arcanum Exp $ */

/* avr/iomx8.h - definitions for ATmega48, ATmega88 and ATmega168 */

#ifndef _AVR_IOMX8_H_
#define _AVR_IOMX8_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iomx8.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* Port B */

#define PINB    _SFR_IO8 (0x03)
/* PINB */
#define PINB7   7
#define PINB6   6
#define PINB5   5
#define PINB4   4
#define PINB3   3
#define PINB2   2
#define PINB1   1
#define PINB0   0

#define DDRB    _SFR_IO8 (0x04)
/* DDRB */
#define DDB7    7
#define DDB6    6
#define DDB5    5
#define DDB4    4
#define DDB3    3
#define DDB2    2
#define DDB1    1
#define DDB0    0

#define PORTB   _SFR_IO8 (0x05)
/* PORTB */
#define PB7     7
#define PB6     6
#define PB5     5
#define PB4     4
#define PB3     3
#define PB2     2
#define PB1     1
#define PB0     0

/* Port C */

#define PINC    _SFR_IO8 (0x06)
/* PINC */
#define PINC6   6
#define PINC5   5
#define PINC4   4
#define PINC3   3
#define PINC2   2
#define PINC1   1
#define PINC0   0

#define DDRC    _SFR_IO8 (0x07)
/* DDRC */
#define DDC6    6
#define DDC5    5
#define DDC4    4
#define DDC3    3
#define DDC2    2
#define DDC1    1
#define DDC0    0

#define PORTC   _SFR_IO8 (0x08)
/* PORTC */
#define PC6     6
#define PC5     5
#define PC4     4
#define PC3     3
#define PC2     2
#define PC1     1
#define PC0     0

/* Port D */

#define PIND    _SFR_IO8 (0x09)
/* PIND */
#define PIND7   7
#define PIND6   6
#define PIND5   5
#define PIND4   4
#define PIND3   3
#define PIND2   2
#define PIND1   1
#define PIND0   0

#define DDRD    _SFR_IO8 (0x0A)
/* DDRD */
#define DDD7    7
#define DDD6    6
#define DDD5    5
#define DDD4    4
#define DDD3    3
#define DDD2    2
#define DDD1    1
#define DDD0    0

#define PORTD   _SFR_IO8 (0x0B)
/* PORTD */
#define PD7     7
#define PD6     6
#define PD5     5
#define PD4     4
#define PD3     3
#define PD2     2
#define PD1     1
#define PD0     0

#define TIFR0   _SFR_IO8 (0x15)
/* TIFR0 */
#define OCF0B   2
#define OCF0A   1
#define TOV0    0

#define TIFR1   _SFR_IO8 (0x16)
/* TIFR1 */
#define ICF1    5
#define OCF1B   2
#define OCF1A   1
#define TOV1    0

#define TIFR2   _SFR_IO8 (0x17)
/* TIFR2 */
#define OCF2B   2
#define OCF2A   1
#define TOV2    0

#define PCIFR   _SFR_IO8 (0x1B)
/* PCIFR */
#define PCIF2   2
#define PCIF1   1
#define PCIF0   0

#define EIFR    _SFR_IO8 (0x1C)
/* EIFR */
#define INTF1   1
#define INTF0   0

#define EIMSK   _SFR_IO8 (0x1D)
/* EIMSK */
#define INT1    1
#define INT0    0

#define GPIOR0  _SFR_IO8 (0x1E)

#define EECR    _SFR_IO8(0x1F)
/* EECT - EEPROM Control Register */
#define EEPM1   5
#define EEPM0   4
#define EERIE   3
#define EEMPE   2
#define EEPE    1
#define EERE    0

#define EEDR    _SFR_IO8(0X20)

/* Combine EEARL and EEARH */
#define EEAR    _SFR_IO16(0x21)
#define EEARL   _SFR_IO8(0x21)
#define EEARH   _SFR_IO8(0X22)
/* 
Even though EEARH is not used by the mega48, the EEAR8 bit in the register
must be written to 0, according to the datasheet, hence the EEARH register
must be defined for the mega48.
*/
/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021


#define GTCCR   _SFR_IO8 (0x23)
/* GTCCR */
#define TSM     7
#define PSRASY  1
#define PSRSYNC 0

#define TCCR0A  _SFR_IO8 (0x24)
/* TCCR0A */
#define COM0A1  7
#define COM0A0  6
#define COM0B1  5
#define COM0B0  4
#define WGM01   1
#define WGM00   0

#define TCCR0B  _SFR_IO8 (0x25)
/* TCCR0A */
#define FOC0A   7
#define FOC0B   6
#define WGM02   3
#define CS02    2
#define CS01    1
#define CS00    0

#define TCNT0   _SFR_IO8 (0x26)
#define OCR0A   _SFR_IO8 (0x27)
#define OCR0B   _SFR_IO8 (0x28)

#define GPIOR1  _SFR_IO8 (0x2A)
#define GPIOR2  _SFR_IO8 (0x2B)

#define SPCR    _SFR_IO8 (0x2C)
/* SPCR */
#define SPIE    7
#define SPE     6
#define DORD    5
#define MSTR    4
#define CPOL    3
#define CPHA    2
#define SPR1    1
#define SPR0    0

#define SPSR    _SFR_IO8 (0x2D)
/* SPSR */
#define SPIF    7
#define WCOL    6
#define SPI2X   0

#define SPDR    _SFR_IO8 (0x2E)

#define ACSR    _SFR_IO8 (0x30)
/* ACSR */
#define ACD     7
#define ACBG    6
#define ACO     5
#define ACI     4
#define ACIE    3
#define ACIC    2
#define ACIS1   1
#define ACIS0   0

#define MONDR   _SFR_IO8 (0x31)

#define SMCR    _SFR_IO8 (0x33)
/* SMCR */
#define SM2     3
#define SM1     2
#define SM0     1
#define SE      0

#define MCUSR   _SFR_IO8 (0x34)
/* MCUSR */
#define WDRF    3
#define BORF    2
#define EXTRF   1
#define PORF    0

#define MCUCR   _SFR_IO8 (0x35)
/* MCUCR */
#define PUD     4
#if defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) 
#define IVSEL   1
#define IVCE    0
#endif

#define SPMCSR  _SFR_IO8 (0x37)
/* SPMCSR */
#define SPMIE     7
#if defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
#  define RWWSB   6
#  define RWWSRE  4
#endif
#define BLBSET    3
#define PGWRT     2
#define PGERS     1
#define SELFPRGEN 0
#define SPMEN     0

/* 0x3D..0x3E SP  [defined in <avr/io.h>] */
/* 0x3F SREG      [defined in <avr/io.h>] */

#define WDTCSR  _SFR_MEM8 (0x60)
/* WDTCSR */
#define WDIF    7
#define WDIE    6
#define WDP3    5
#define WDCE    4
#define WDE     3
#define WDP2    2
#define WDP1    1
#define WDP0    0

#define CLKPR   _SFR_MEM8 (0x61)
/* CLKPR */
#define CLKPCE  7
#define CLKPS3  3
#define CLKPS2  2
#define CLKPS1  1
#define CLKPS0  0

#define PRR     _SFR_MEM8 (0x64)
/* PRR */
#define PRTWI    7
#define PRTIM2   6
#define PRTIM0   5
#define PRTIM1   3
#define PRSPI    2
#define PRUSART0 1
#define PRADC    0

#define OSCCAL  _SFR_MEM8 (0x66)

#define PCICR   _SFR_MEM8 (0x68)
/* PCICR */
#define PCIE2   2
#define PCIE1   1
#define PCIE0   0

#define EICRA   _SFR_MEM8 (0x69)
/* EICRA */
#define ISC11   3
#define ISC10   2
#define ISC01   1
#define ISC00   0

#define PCMSK0  _SFR_MEM8 (0x6B)
/* PCMSK0 */
#define PCINT7    7
#define PCINT6    6
#define PCINT5    5
#define PCINT4    4
#define PCINT3    3
#define PCINT2    2
#define PCINT1    1
#define PCINT0    0

#define PCMSK1  _SFR_MEM8 (0x6C)
/* PCMSK1 */
#define PCINT14   6
#define PCINT13   5
#define PCINT12   4
#define PCINT11   3
#define PCINT10   2
#define PCINT9    1
#define PCINT8    0

#define PCMSK2  _SFR_MEM8 (0x6D)
/* PCMSK2 */
#define PCINT23   7
#define PCINT22   6
#define PCINT21   5
#define PCINT20   4
#define PCINT19   3
#define PCINT18   2
#define PCINT17   1
#define PCINT16   0

#define TIMSK0  _SFR_MEM8 (0x6E)
/* TIMSK0 */
#define OCIE0B  2
#define OCIE0A  1
#define TOIE0   0

#define TIMSK1  _SFR_MEM8 (0x6F)
/* TIMSK1 */
#define ICIE1   5
#define OCIE1B  2
#define OCIE1A  1
#define TOIE1   0

#define TIMSK2  _SFR_MEM8 (0x70)
/* TIMSK2 */
#define OCIE2B  2
#define OCIE2A  1
#define TOIE2   0

#ifndef __ASSEMBLER__
#define ADC     _SFR_MEM16 (0x78)
#endif
#define ADCW    _SFR_MEM16 (0x78)
#define ADCL    _SFR_MEM8 (0x78)
#define ADCH    _SFR_MEM8 (0x79)

#define ADCSRA  _SFR_MEM8 (0x7A)
/* ADCSRA */
#define ADEN    7
#define ADSC    6
#define ADATE   5
#define ADIF    4
#define ADIE    3
#define ADPS2   2
#define ADPS1   1
#define ADPS0   0

#define ADCSRB  _SFR_MEM8 (0x7B)
/* ADCSRB */
#define ACME    6
#define ADTS2   2
#define ADTS1   1
#define ADTS0   0

#define ADMUX   _SFR_MEM8 (0x7C)
/* ADMUX */
#define REFS1   7
#define REFS0   6
#define ADLAR   5
#define MUX3    3
#define MUX2    2
#define MUX1    1
#define MUX0    0

#define DIDR0   _SFR_MEM8 (0x7E)
/* DIDR0 */
#define ADC5D   5
#define ADC4D   4
#define ADC3D   3
#define ADC2D   2
#define ADC1D   1
#define ADC0D   0

#define DIDR1   _SFR_MEM8 (0x7F)
/* DIDR1 */
#define AIN1D   1
#define AIN0D   0

#define TCCR1A  _SFR_MEM8 (0x80)
/* TCCR1A */
#define COM1A1  7
#define COM1A0  6
#define COM1B1  5
#define COM1B0  4
#define WGM11   1
#define WGM10   0

#define TCCR1B  _SFR_MEM8 (0x81)
/* TCCR1B */
#define ICNC1   7
#define ICES1   6
#define WGM13   4
#define WGM12   3
#define CS12    2
#define CS11    1
#define CS10    0

#define TCCR1C  _SFR_MEM8 (0x82)
/* TCCR1C */
#define FOC1A   7
#define FOC1B   6

#define TCNT1   _SFR_MEM16 (0x84)
#define TCNT1L  _SFR_MEM8 (0x84)
#define TCNT1H  _SFR_MEM8 (0x85)

#define ICR1    _SFR_MEM16 (0x86)
#define ICR1L   _SFR_MEM8 (0x86)
#define ICR1H   _SFR_MEM8 (0x87)

#define OCR1A   _SFR_MEM16 (0x88)
#define OCR1AL  _SFR_MEM8 (0x88)
#define OCR1AH  _SFR_MEM8 (0x89)

#define OCR1B   _SFR_MEM16 (0x8A)
#define OCR1BL  _SFR_MEM8 (0x8A)
#define OCR1BH  _SFR_MEM8 (0x8B)

#define TCCR2A  _SFR_MEM8 (0xB0)
/* TCCR2A */
#define COM2A1  7
#define COM2A0  6
#define COM2B1  5
#define COM2B0  4
#define WGM21   1
#define WGM20   0

#define TCCR2B  _SFR_MEM8 (0xB1)
/* TCCR2B */
#define FOC2A   7
#define FOC2B   6
#define WGM22   3
#define CS22    2
#define CS21    1
#define CS20    0

#define TCNT2   _SFR_MEM8 (0xB2)
#define OCR2A   _SFR_MEM8 (0xB3)
#define OCR2B   _SFR_MEM8 (0xB4)

#define ASSR    _SFR_MEM8 (0xB6)
/* ASSR */
#define EXCLK    6
#define AS2      5
#define TCN2UB   4
#define OCR2AUB  3
#define OCR2BUB  2
#define TCR2AUB  1
#define TCR2BUB  0

#define TWBR    _SFR_MEM8 (0xB8)

#define TWSR    _SFR_MEM8 (0xB9)
/* TWSR */
#define TWS7    7
#define TWS6    6
#define TWS5    5
#define TWS4    4
#define TWS3    3
#define TWPS1   1
#define TWPS0   0

#define TWAR    _SFR_MEM8 (0xBA)
/* TWAR */
#define TWA6    7
#define TWA5    6
#define TWA4    5
#define TWA3    4
#define TWA2    3
#define TWA1    2
#define TWA0    1
#define TWGCE   0

#define TWDR    _SFR_MEM8 (0xBB)

#define TWCR    _SFR_MEM8 (0xBC)
/* TWCR */
#define TWINT   7
#define TWEA    6
#define TWSTA   5
#define TWSTO   4
#define TWWC    3
#define TWEN    2
#define TWIE    0

#define TWAMR   _SFR_MEM8 (0xBD)
/* TWAMR */
#define TWAM6   7
#define TWAM5   6
#define TWAM4   5
#define TWAM3   4
#define TWAM2   3
#define TWAM1   2
#define TWAM0   1

#define UCSR0A  _SFR_MEM8 (0xC0)
/* UCSR0A */
#define RXC0    7
#define TXC0    6
#define UDRE0   5
#define FE0     4
#define DOR0    3
#define UPE0    2
#define U2X0    1
#define MPCM0   0

#define UCSR0B  _SFR_MEM8 (0xC1)
/* UCSR0B */
#define RXCIE0  7
#define TXCIE0  6
#define UDRIE0  5
#define RXEN0   4
#define TXEN0   3
#define UCSZ02  2
#define RXB80   1
#define TXB80   0

#define UCSR0C  _SFR_MEM8 (0xC2)
/* UCSR0C */
#define UMSEL01  7
#define UMSEL00  6
#define UPM01    5
#define UPM00    4
#define USBS0    3
#define UCSZ01   2
#define UDORD0   2
#define UCSZ00   1
#define UCPHA0   1
#define UCPOL0   0

#define UBRR0   _SFR_MEM16 (0xC4)
#define UBRR0L  _SFR_MEM8 (0xC4)
#define UBRR0H  _SFR_MEM8 (0xC5)
#define UDR0    _SFR_MEM8 (0xC6)

/* Interrupt vectors */

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)

/* Pin Change Interrupt Request 0 */
#define PCINT0_vect			_VECTOR(3)
#define SIG_PIN_CHANGE0			_VECTOR(3)

/* Pin Change Interrupt Request 0 */
#define PCINT1_vect			_VECTOR(4)
#define SIG_PIN_CHANGE1			_VECTOR(4)

/* Pin Change Interrupt Request 1 */
#define PCINT2_vect			_VECTOR(5)
#define SIG_PIN_CHANGE2			_VECTOR(5)

/* Watchdog Time-out Interrupt */
#define WDT_vect			_VECTOR(6)
#define SIG_WATCHDOG_TIMEOUT		_VECTOR(6)

/* Timer/Counter2 Compare Match A */
#define TIMER2_COMPA_vect		_VECTOR(7)
#define SIG_OUTPUT_COMPARE2A		_VECTOR(7)

/* Timer/Counter2 Compare Match A */
#define TIMER2_COMPB_vect		_VECTOR(8)
#define SIG_OUTPUT_COMPARE2B		_VECTOR(8)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(9)
#define SIG_OVERFLOW2			_VECTOR(9)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(10)
#define SIG_INPUT_CAPTURE1		_VECTOR(10)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(11)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(11)

/* Timer/Counter1 Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(12)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(12)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(13)
#define SIG_OVERFLOW1			_VECTOR(13)

/* TimerCounter0 Compare Match A */
#define TIMER0_COMPA_vect		_VECTOR(14)
#define SIG_OUTPUT_COMPARE0A		_VECTOR(14)

/* TimerCounter0 Compare Match B */
#define TIMER0_COMPB_vect		_VECTOR(15)
#define SIG_OUTPUT_COMPARE0B		_VECTOR(15)

/* Timer/Couner0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(16)
#define SIG_OVERFLOW0			_VECTOR(16)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(17)
#define SIG_SPI				_VECTOR(17)

/* USART Rx Complete */
#define USART_RX_vect			_VECTOR(18)
#define SIG_USART_RECV			_VECTOR(18)

/* USART, Data Register Empty */
#define USART_UDRE_vect			_VECTOR(19)
#define SIG_USART_DATA			_VECTOR(19)

/* USART Tx Complete */
#define USART_TX_vect			_VECTOR(20)
#define SIG_USART_TRANS			_VECTOR(20)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(21)
#define SIG_ADC				_VECTOR(21)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(22)
#define SIG_EEPROM_READY		_VECTOR(22)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(23)
#define SIG_COMPARATOR			_VECTOR(23)

/* Two-wire Serial Interface */
#define TWI_vect			_VECTOR(24)
#define SIG_TWI				_VECTOR(24)
#define SIG_2WIRE_SERIAL		_VECTOR(24)

/* Store Program Memory Read */
#define SPM_READY_vect			_VECTOR(25)
#define SIG_SPM_READY			_VECTOR(25)

/* The mega48 and mega88 vector tables are single instruction entries (16 bits
   per entry for an RJMP) while the mega168 table has double instruction
   entries (32 bits per entry for a JMP). */

#if defined (__AVR_ATmega168__)
#  define _VECTORS_SIZE 104
#else
#  define _VECTORS_SIZE 52
#endif

#endif /* _AVR_IOM8_H_ */
