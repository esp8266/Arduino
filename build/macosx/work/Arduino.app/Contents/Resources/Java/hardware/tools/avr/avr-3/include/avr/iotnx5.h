/* Copyright (c) 2005,2007 Anatoly Sokolov
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

/* $Id: iotnx5.h,v 1.7 2007/10/30 10:39:06 joerg_wunsch Exp $ */

/* avr/iotnx5.h - definitions for ATtiny25, ATtiny45 and ATtiny85 */

#ifndef _AVR_IOTNX5_H_
#define _AVR_IOTNX5_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotnx5.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* Reserved [0x00..0x02] */

#define ADCSRB  _SFR_IO8 (0x03)
#define BIN     7
#define ACME    6
#define IPR     5
#define ADTS2   2
#define ADTS1   1
#define ADTS0   0

#ifndef __ASSEMBLER__
#define ADC     _SFR_IO16(0x04)
#endif
#define ADCW    _SFR_IO16(0x04)
#define ADCL    _SFR_IO8(0x04)
#define ADCH    _SFR_IO8(0x05)

#define ADCSRA  _SFR_IO8 (0x06)
#define ADEN    7
#define ADSC    6
#define ADATE   5
#define ADIF    4
#define ADIE    3
#define ADPS2   2
#define ADPS1   1
#define ADPS0   0

#define ADMUX   _SFR_IO8(0x07)
#define REFS1   7
#define REFS0   6
#define ADLAR   5
#define REFS2   4
#define MUX3    3
#define MUX2    2
#define MUX1    1
#define MUX0    0

#define ACSR    _SFR_IO8(0x08)
#define ACD     7
#define ACBG    6
#define ACO     5
#define ACI     4
#define ACIE    3
#define ACIS1   1
#define ACIS0   0

/* Reserved [0x09..0x0C] */

#define USICR   _SFR_IO8(0x0D)
#define USISIE  7
#define USIOIE  6
#define USIWM1  5
#define USIWM0  4
#define USICS1  3
#define USICS0  2
#define USICLK  1
#define USITC   0

#define USISR   _SFR_IO8(0x0E)
#define USISIF  7 
#define USIOIF  6
#define USIPF   5
#define USIDC   4
#define USICNT3 3
#define USICNT2 2
#define USICNT1 1
#define USICNT0 0

#define USIDR   _SFR_IO8(0x0F)
#define USIBR   _SFR_IO8(0x10)

#define GPIOR0  _SFR_IO8(0x11)
#define GPIOR1  _SFR_IO8(0x12)
#define GPIOR2  _SFR_IO8(0x13)

#define DIDR0    _SFR_IO8(0x14)
#define ADC0D   5
#define ADC2D   4
#define ADC3D   3
#define ADC1D   2
#define AIN1D   1 
#define AIN0D   0

#define PCMSK   _SFR_IO8(0x15)
#define PCINT5  5
#define PCINT4  4
#define PCINT3  3
#define PCINT2  2
#define PCINT1  1
#define PCINT0  0

#define PINB    _SFR_IO8(0x16)
#define PINB5   5
#define PINB4   4
#define PINB3   3
#define PINB2   2
#define PINB1   1
#define PINB0   0

#define DDRB    _SFR_IO8(0x17)
#define DDB5    5
#define DDB4    4
#define DDB3    3
#define DDB2    2
#define DDB1    1
#define DDB0    0

#define PORTB   _SFR_IO8(0x18)
#define PB5     5
#define PB4     4
#define PB3     3
#define PB2     2
#define PB1     1
#define PB0     0

/* Reserved [0x19..0x1B] */

/* EEPROM Control Register EECR */
#define EECR	_SFR_IO8(0x1C)
#define EEPM1   5
#define EEPM0   4
#define EERIE   3
#define EEMPE   2
#define EEPE    1
#define EERE    0

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO16(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEARH	_SFR_IO8(0x1F)

#define PRR     _SFR_IO8(0x20)
#define PRTIM1  3
#define PRTIM0  2
#define PRUSI   1
#define PRADC   0

#define WDTCR   _SFR_IO8(0x21)
#define WDIF    7
#define WDIE    6
#define WDP3    5
#define WDCE    4
#define WDE     3
#define WDP2    2
#define WDP1    1
#define WDP0    0

#define DWDR    _SFR_IO8(0x22)

#define DTPS1   _SFR_IO8(0x23)
#define DTPS11   1
#define DTPS10   0

#define DT1B    _SFR_IO8(0x24)
#define DT1BH3  7
#define DT1BH2  6
#define DT1BH1  5
#define DT1BH0  4
#define DT1BL3  3
#define DT1BL2  2
#define DT1BL1  1
#define DT1BL0  0

#define DT1A    _SFR_IO8(0x25)
#define DT1AH3  7
#define DT1AH2  6
#define DT1AH1  5
#define DT1AH0  4
#define DT1AL3  3
#define DT1AL2  2
#define DT1AL1  1
#define DT1AL0  0

#define CLKPR   _SFR_IO8(0x26)
#define CLKPCE  7
#define CLKPS3  3
#define CLKPS2  2
#define CLKPS1  1
#define CLKPS0  0

#define PLLCSR  _SFR_IO8(0x27)
#define LSM     7
#define PCKE    2
#define PLLE    1
#define PLOCK   0

#define OCR0B   _SFR_IO8(0x28)

#define OCR0A   _SFR_IO8(0x29)

#define TCCR0A  _SFR_IO8(0x2A)
#define COM0A1  7
#define COM0A0  6
#define COM0B1  5
#define COM0B0  4
#define WGM01   1
#define WGM00   0

#define OCR1B   _SFR_IO8(0x2B)

#define GTCCR   _SFR_IO8(0x2C)
#define TSM     7
#define PWM1B   6
#define COM1B1  5
#define COM1B0  4
#define FOC1B   3
#define FOC1A   2
#define PSR1    1
#define PSR0    0

#define OCR1C   _SFR_IO8(0x2D)

#define OCR1A   _SFR_IO8(0x2E)

#define TCNT1   _SFR_IO8(0x2F)

#define TCCR1   _SFR_IO8(0x30)
#define CTC1    7
#define PWM1A   6
#define COM1A1  5
#define COM1A0  4
#define CS13    3
#define CS12    2
#define CS11    1
#define CS10    0

#define OSCCAL  _SFR_IO8(0x31)

#define TCNT0   _SFR_IO8(0x32)

#define TCCR0B  _SFR_IO8(0x33)
#define FOC0A   7
#define FOC0B   6
#define WGM02   3
#define CS02    2
#define CS01    1
#define CS00    0

#define MCUSR   _SFR_IO8(0x34)
#define WDRF    3
#define BORF    2
#define EXTRF   1
#define PORF    0

#define MCUCR   _SFR_IO8(0x35)
#define PUD     6
#define SE      5
#define SM1     4
#define SM0     3
#define ISC01   1
#define ISC00   0

/* Reserved [0x36] */

#define SPMCSR  _SFR_IO8(0x37)
#define CTPB    4
#define RFLB    3
#define PGWRT   2
#define PGERS   1
#define SPMEN   0

#define TIFR    _SFR_IO8(0x38)
#define OCF1A   6
#define OCF1B   5
#define OCF0A   4
#define OCF0B   3
#define TOV1    2
#define TOV0    1

#define TIMSK   _SFR_IO8(0x39)
#define OCIE1A  6
#define OCIE1B  5
#define OCIE0A  4
#define OCIE0B  3
#define TOIE1   2
#define TOIE0   1

#define GIFR    _SFR_IO8(0x3A)
#define INTF0   6
#define PCIF    5

#define GIMSK   _SFR_IO8(0x3B)
#define INT0    6
#define PCIE    5

/* Reserved [0x3C] */

/* 0x3D..0x3E SP  [defined in <avr/io.h>] */
/* 0x3F SREG      [defined in <avr/io.h>] */

///---

/* Interrupt vectors */
/* Interrupt vector 0 is the reset vector. */
/* External Interrupt 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* Pin change Interrupt Request 0 */
#define PCINT0_vect			_VECTOR(2)
#define SIG_PIN_CHANGE			_VECTOR(2)

/* Timer/Counter1 Compare Match 1A */
#define TIM1_COMPA_vect			_VECTOR(3)
#define TIMER1_COMPA_vect		_VECTOR(3)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(3)

/* Timer/Counter1 Overflow */
#define TIM1_OVF_vect			_VECTOR(4)
#define TIMER1_OVF_vect			_VECTOR(4)
#define SIG_OVERFLOW1			_VECTOR(4)

/* Timer/Counter0 Overflow */
#define TIM0_OVF_vect			_VECTOR(5)
#define TIMER0_OVF_vect			_VECTOR(5)
#define SIG_OVERFLOW0			_VECTOR(5)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(6)
#define SIG_EEPROM_READY		_VECTOR(6)

/* Analog comparator */
#define ANA_COMP_vect			_VECTOR(7)
#define SIG_COMPARATOR			_VECTOR(7)

/* ADC Conversion ready */
#define ADC_vect			_VECTOR(8)
#define SIG_ADC				_VECTOR(8)

/* Timer/Counter1 Compare Match B */
#define TIM1_COMPB_vect			_VECTOR(9)
#define TIMER1_COMPB_vect		_VECTOR(9)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(9)

/* Timer/Counter0 Compare Match A */
#define TIM0_COMPA_vect			_VECTOR(10)
#define TIMER0_COMPA_vect		_VECTOR(10)
#define SIG_OUTPUT_COMPARE0A		_VECTOR(10)

/* Timer/Counter0 Compare Match B */
#define TIM0_COMPB_vect			_VECTOR(11)
#define TIMER0_COMPB_vect		_VECTOR(11)
#define SIG_OUTPUT_COMPARE0B		_VECTOR(11)

/* Watchdog Time-out */
#define WDT_vect			_VECTOR(12)
#define SIG_WATCHDOG_TIMEOUT		_VECTOR(12)

/* USI START */
#define USI_START_vect			_VECTOR(13)
#define SIG_USI_START			_VECTOR(13)

/* USI Overflow */
#define USI_OVF_vect			_VECTOR(14)
#define SIG_USI_OVERFLOW		_VECTOR(14)

#define _VECTORS_SIZE 30

#endif /* _AVR_IOTNX5_H_ */
