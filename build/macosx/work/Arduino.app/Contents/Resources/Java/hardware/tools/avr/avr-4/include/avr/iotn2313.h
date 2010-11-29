/* Copyright (c) 2004, 2005, 2006 Bob Paddock
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

/* $Id: iotn2313.h,v 1.14.2.6 2008/10/17 23:27:51 arcanum Exp $ */

/* iotn2313.h derived from io2313.h by Bob Paddock.

   The changes between the AT90S2313 and the ATtiny2313 are extensive.

   Atmel has renamed several registers, and bits.  See Atmel application note
   AVR091, as well as the errata at the end of the current ATtiny2313 data
   sheet.  Some of the names have changed more than once during the sampling
   period of the ATtiny2313.

   Where there is no conflict the new and old names are both supported.

   In the case of a new feature in a register, only the new name is used.
   This intentionally breaks old code, so that there are no silent bugs.  The
   source code must be updated to the new name in this case.

   The hardware interrupt vector table has changed from that of the AT90S2313.

   ATtiny2313 programs in page mode rather than the byte mode of the
   AT90S2313.  Beware of programming the ATtiny2313 as a AT90S2313 device,
   when programming the Flash.

   ATtiny2313 has Signature Bytes: 0x1E 0x91 0x0A.

   Changes and/or additions are noted by "ATtiny" in the comments below. */

/* avr/iotn2313.h - definitions for ATtiny2313 */

#ifndef _AVR_IOTN2313_H_
#define _AVR_IOTN2313_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn2313.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif

/* I/O registers */

/*
 *  The Register Bit names are represented by their bit number (0-7).
 *  Example: PORTB |= _BV(PORTB7); Set MSB of PORTB.
 */

/* 0x00 Reserved */

/* ATtiny Digital Input Disable Register DIDR */
#define DIDR    _SFR_IO8(0x01)

#define AIN1D   1
#define AIN0D   0

/* ATtiny USART Baud Rate Register High UBBRH[11:8] */
#define UBRRH   _SFR_IO8(0x02)

/* ATtiny USART Control and Status Register C UCSRC */
#define UCSRC   _SFR_IO8(0x03)

#define UMSEL   6
#define UPM1    5
#define UPM0    4
#define USBS    3
#define UCSZ1   2
#define UCSZ0   1
#define UCPOL   0

/* 0x04 -> 0x07 Reserved */

/* ATtiny Analog Comparator Control and Status Register ACSR */
#define ACSR    _SFR_IO8(0x08)

#define ACD     7
#define ACBG    6
#define ACO     5
#define ACI     4
#define ACIE    3
#define ACIC    2
#define ACIS1   1
#define ACIS0   0

/* USART Baud Rate Register Low UBBRL[7:0] */
#define UBRRL   _SFR_IO8(0x09)

/* ATtiny USART Control Register UCSRB */
#define UCSRB   _SFR_IO8(0x0A)

#define RXCIE   7
#define TXCIE   6
#define UDRIE   5
#define RXEN    4
#define TXEN    3
#define UCSZ2   2
#define RXB8    1
#define TXB8    0

/* ATtiny USART Status Register UCSRA */
#define UCSRA   _SFR_IO8(0x0B)

#define RXC     7
#define TXC     6
#define UDRE    5
#define FE      4
#define DOR     3
#define UPE     2
#define U2X     1
#define MPCM    0

/* USART I/O Data Register UBR or RXB[7:0], TXB[7:0] */
#define UDR     _SFR_IO8(0x0C)
#define RXB     _SFR_IO8(0x0C)
#define TXB     _SFR_IO8(0x0C)

/* ATtiny USI Control Register USICR */
#define USICR   _SFR_IO8(0x0D)

#define USISIE  7
#define USIOIE  6
#define USIWM1  5
#define USIWM0  4
#define USICS1  3
#define USICS0  2
#define USICLK  1
#define USITC   0

/* ATtiny USI Status Register USISR */
#define USISR   _SFR_IO8(0x0E)

#define USISIF  7
#define USIOIF  6
#define USIPF   5
#define USIDC   4
#define USICNT3 3
#define USICNT2 2
#define USICNT1 1
#define USICNT0 0

/* ATtiny USI Data Register USIDR[7:0] */
#define USIDR   _SFR_IO8(0x0F)

/* Input Pins, Port D PIND[6:0] */
#define PIND    _SFR_IO8(0x10)

#define PIND6   6
#define PIND5   5
#define PIND4   4
#define PIND3   3
#define PIND2   2
#define PIND1   1
#define PIND0   0

/* Data Direction Register, Port D DDRD[6:0] */
#define DDRD    _SFR_IO8(0x11)

#define DDD6    6
#define DDD5    5
#define DDD4    4
#define DDD3    3
#define DDD2    2
#define DDD1    1
#define DDD0    0

/* Data Register, Port D PORTD[6:0] */
#define PORTD   _SFR_IO8(0x12)

#define PD6     6
#define PD5     5
#define PD4     4
#define PD3     3
#define PD2     2
#define PD1     1
#define PD0     0

/* ATtiny General Purpose I/O Register Zero GPIOR0[7:0] */
#define GPIOR0  _SFR_IO8(0x13)

/* ATtiny General Purpose I/O Register One GPIOR1[7:0] */
#define GPIOR1  _SFR_IO8(0x14)

/* ATtiny General Purpose I/O Register Two One GPIOR2[7:0] */
#define GPIOR2  _SFR_IO8(0x15)

/* Input Pins, Port B PORTB[7:0] */
#define PINB    _SFR_IO8(0x16)

#define PINB7   7
#define PINB6   6
#define PINB5   5
#define PINB4   4
#define PINB3   3
#define PINB2   2
#define PINB1   1
#define PINB0   0

/* Data Direction Register, Port B PORTB[7:0] */
#define DDRB    _SFR_IO8(0x17)

#define DDB7    7
#define DDB6    6
#define DDB5    5
#define DDB4    4
#define DDB3    3
#define DDB2    2
#define DDB1    1
#define DDB0    0

/* Data Register, Port B PORTB[7:0] */
#define PORTB   _SFR_IO8(0x18)

#define PB7     7
#define PB6     6
#define PB5     5
#define PB4     4
#define PB3     3
#define PB2     2
#define PB1     1
#define PB0     0

/* Port A Input Pins Address PINA[2:0] */
#define PINA    _SFR_IO8(0x19)

#define PINA2   2
#define PINA1   1
#define PINA0   0

/* Port A Data Direction Register DDRA[2:0] */
#define DDRA    _SFR_IO8(0x1A)

#define DDRA2   2
#define DDRA1   1
#define DDRA0   0

/* Port A Data Register PORTA[2:0] */
#define PORTA   _SFR_IO8(0x1B)

#define PA2     2
#define PA1     1
#define PA0     0

/* ATtiny EEPROM Control Register EECR */
#define EECR	_SFR_IO8(0x1C)
#define EEPM1   5
#define EEPM0   4
#define EERIE   3
#define EEMPE   2
#define EEPE    1
#define EERE    0

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* The EEPROM Address Register EEAR[6:0] */
#define EEAR	_SFR_IO8(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEAR6   6
#define EEAR5   5
#define EEAR4   4
#define EEAR3   3
#define EEAR2   2
#define EEAR1   1
#define EEAR0   0

/* 0x1F Reserved */

/* ATtiny Pin Change Mask Register PCMSK PCINT[7:0] */
#define PCMSK   _SFR_IO8(0x20)

#define PCINT7  7
#define PCINT6  6
#define PCINT5  5
#define PCINT4  4
#define PCINT3  3
#define PCINT2  2
#define PCINT1  1
#define PCINT0  0

/* ATtiny Watchdog Timer Control Register WDTCSR */
#define WDTCSR  _SFR_IO8(0x21)

#define WDIF    7
#define WDIE    6
#define WDP3    5
#define WDCE    4
#define WDE     3
#define WDP2    2
#define WDP1    1
#define WDP0    0

/* ATtiny Timer/Counter1 Control Register C TCCR1C */
#define TCCR1C  _SFR_IO8(0x22)

#define FOC1A   7
#define FOC1B   6

/* General Timer/Counter Control Register GTCCR */
#define GTCCR   _SFR_IO8(0x23)

#define PSR10   0

/* T/C 1 Input Capture Register ICR1[15:0] */
#define ICR1    _SFR_IO16(0x24)
#define ICR1L   _SFR_IO8(0x24)
#define ICR1H   _SFR_IO8(0x25)

/* ATtiny Clock Prescale Register */
#define CLKPR   _SFR_IO8(0x26)

#define CLKPCE  7
#define CLKPS3  3
#define CLKPS2  2
#define CLKPS1  1
#define CLKPS0  0

/* 0x27 Reserved */

/* ATtiny Output Compare Register 1 B OCR1B[15:0] */
#define OCR1B   _SFR_IO16(0x28)
#define OCR1BL  _SFR_IO8(0x28)
#define OCR1BH  _SFR_IO8(0x29)

/* Output Compare Register 1 OCR1A[15:0] */
#define OCR1    _SFR_IO16(0x2A)
#define OCR1L   _SFR_IO8(0x2A)
#define OCR1H   _SFR_IO8(0x2B)
#define OCR1A   _SFR_IO16(0x2A)
#define OCR1AL  _SFR_IO8(0x2A)
#define OCR1AH  _SFR_IO8(0x2B)

/* Timer/Counter 1 TCNT1[15:0] */
#define TCNT1   _SFR_IO16(0x2C)
#define TCNT1L  _SFR_IO8(0x2C)
#define TCNT1H  _SFR_IO8(0x2D)

/* ATtiny Timer/Counter 1 Control and Status Register TCCR1B */
#define TCCR1B  _SFR_IO8(0x2E)

#define ICNC1   7
#define ICES1   6
#define WGM13   4
#define WGM12   3 /* Was CTC1 in AT90S2313 */
#define CS12    2
#define CS11    1
#define CS10    0

/* ATtiny Timer/Counter 1 Control Register TCCR1A */
#define TCCR1A  _SFR_IO8(0x2F)

#define COM1A1  7
#define COM1A0  6
#define COM1B1  5
#define COM1B0  4
#define WGM11   1 /* Was PWM11 in AT90S2313 */
#define WGM10   0 /* Was PWM10 in AT90S2313 */

/* ATtiny Timer/Counter Control Register A TCCR0A */
#define TCCR0A  _SFR_IO8(0x30)

#define COM0A1  7
#define COM0A0  6
#define COM0B1  5
#define COM0B0  4
#define WGM01   1
#define WGM00   0

/* ATtiny Oscillator Calibration Register OSCCAL[6:0] */
#define OSCCAL  _SFR_IO8(0x31)

#define CAL6    6
#define CAL5    5
#define CAL4    4
#define CAL3    3
#define CAL2    2
#define CAL1    1
#define CAL0    0

/* Timer/Counter 0 TCNT0[7:0] */
#define TCNT0   _SFR_IO8(0x32)

/* ATtiny Timer/Counter 0 Control Register TCCR0B */
#define TCCR0B  _SFR_IO8(0x33)

#define FOC0A   7
#define FOC0B   6
#define WGM02   3
#define CS02    2
#define CS01    1
#define CS00    0

/* ATtiny MCU Status Register MCUSR */
#define MCUSR   _SFR_IO8(0x34)

#define WDRF    3
#define BORF    2
#define EXTRF   1
#define PORF    0

/* ATtiny MCU general Control Register MCUCR */
#define MCUCR   _SFR_IO8(0x35)

#define PUD     7
#define SM1     6
#define SE      5
#define SM0     4 /* Some preliminary ATtiny2313 data sheets incorrectly refer
                     to this bit as SMD; was SM in AT90S2313. */
#define ISC11   3
#define ISC10   2
#define ISC01   1
#define ISC00   0

/* ATtiny Output Compare Register A OCR0A[7:0] */
#define OCR0A   _SFR_IO8(0x36)

/* ATtiny Store Program Memory Control and Status Register SPMCSR */
#define SPMCSR  _SFR_IO8(0x37)

#define CTPB    4
#define RFLB    3
#define PGWRT   2
#define PGERS   1
#define SPMEN   0   /* The name is used in ATtiny2313.xml file. */
#define SELFPRGEN 0 /* The name is used in datasheet. */
#define SELFPRGE  0 /* The name is left for compatibility. */

/* ATtiny Timer/Counter Interrupt Flag register TIFR */
#define TIFR    _SFR_IO8(0x38)

#define TOV1    7
#define OCF1A   6
#define OCF1B   5
#define ICF1    3
#define OCF0B   2
#define TOV0    1
#define OCF0A   0

/* ATtiny Timer/Counter Interrupt MaSK register TIMSK */
#define TIMSK   _SFR_IO8(0x39)

#define TOIE1   7
#define OCIE1A  6
#define OCIE1B  5
#define ICIE1   3
#define OCIE0B  2
#define TOIE0   1
#define OCIE0A  0

/* ATtiny External Interrupt Flag Register EIFR, was GIFR */
#define EIFR    _SFR_IO8(0x3A)

#define INTF1   7
#define INTF0   6
#define PCIF    5

/* ATtiny General Interrupt MaSK register GIMSK */
#define GIMSK   _SFR_IO8(0x3B)

#define INT1    7
#define INT0    6
#define PCIE    5

/* ATtiny Output Compare Register B OCR0B[7:0] */
#define OCR0B   _SFR_IO8(0x3C)

/* Interrupt vectors: */

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)
#define SIG_INT0			_VECTOR(1)

/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)
#define SIG_INT1			_VECTOR(2)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(3)
#define SIG_INPUT_CAPTURE1		_VECTOR(3)
#define SIG_TIMER1_CAPT			_VECTOR(3)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(4)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(4)
#define SIG_TIMER1_COMPA		_VECTOR(4)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(5)
#define SIG_OVERFLOW1			_VECTOR(5)
#define SIG_TIMER1_OVF			_VECTOR(5)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(6)
#define SIG_OVERFLOW0			_VECTOR(6)
#define SIG_TIMER0_OVF			_VECTOR(6)

/* USART, Rx Complete */
#define USART_RX_vect			_VECTOR(7)
#define SIG_USART0_RECV			_VECTOR(7)
#define SIG_USART0_RX			_VECTOR(7)

/* USART Data Register Empty */
#define USART_UDRE_vect			_VECTOR(8)
#define SIG_USART0_DATA			_VECTOR(8)
#define SIG_USART0_UDRE			_VECTOR(8)

/* USART, Tx Complete */
#define USART_TX_vect			_VECTOR(9)
#define SIG_USART0_TRANS		_VECTOR(9)
#define SIG_USART0_TX			_VECTOR(9)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(10)
#define SIG_COMPARATOR			_VECTOR(10)
#define SIG_ANALOG_COMP			_VECTOR(10)
#define PCINT_vect			_VECTOR(11)
#define SIG_PIN_CHANGE			_VECTOR(11)
#define SIG_PCINT			_VECTOR(11)
#define TIMER1_COMPB_vect		_VECTOR(12)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(12)
#define SIG_TIMER1_COMPB		_VECTOR(12)
#define TIMER0_COMPA_vect		_VECTOR(13)
#define SIG_OUTPUT_COMPARE0A		_VECTOR(13)
#define SIG_TIMER0_COMPA		_VECTOR(13)
#define TIMER0_COMPB_vect		_VECTOR(14)
#define SIG_OUTPUT_COMPARE0B		_VECTOR(14)
#define SIG_TIMER0_COMPB		_VECTOR(14)

/* USI Start Condition */
#define USI_START_vect			_VECTOR(15)
#define SIG_USI_START			_VECTOR(15)
#define SIG_USI_START			_VECTOR(15)

/* USI Overflow */
#define USI_OVERFLOW_vect		_VECTOR(16)
#define SIG_USI_OVERFLOW		_VECTOR(16)
#define SIG_USI_OVERFLOW		_VECTOR(16)
#define EEPROM_READY_vect		_VECTOR(17)
#define SIG_EEPROM_READY		_VECTOR(17)
#define SIG_EE_READY			_VECTOR(17)

/* Watchdog Timer Overflow */
#define WDT_OVERFLOW_vect		_VECTOR(18)
#define SIG_WATCHDOG_TIMEOUT		_VECTOR(18)
#define SIG_WDT_OVERFLOW		_VECTOR(18)

/* 38 = (18*2)+2: Number of vectors times two, plus the reset vector */
#define _VECTORS_SIZE     38

/* Constants */
#define SPM_PAGESIZE 32
#define RAMEND       0xDF
#define XRAMEND      0xDF
#define E2END        0x7F
#define E2PAGESIZE   4
#define FLASHEND     0x07FF


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
#define FUSE_RSTDISBL    (unsigned char)~_BV(0)
#define FUSE_BODLEVEL0   (unsigned char)~_BV(1)
#define FUSE_BODLEVEL1   (unsigned char)~_BV(2)
#define FUSE_BODLEVEL2   (unsigned char)~_BV(3)
#define FUSE_WDTON       (unsigned char)~_BV(4)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_EESAVE      (unsigned char)~_BV(6)
#define FUSE_DWEN        (unsigned char)~_BV(7)
#define HFUSE_DEFAULT (FUSE_SPIEN)

/* Extended Fuse Byte */
#define FUSE_SELFPRGEN   (unsigned char)~_BV(0)
#define EFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x91
#define SIGNATURE_2 0x0A


#endif  /* _AVR_IOTN2313_H_ */
