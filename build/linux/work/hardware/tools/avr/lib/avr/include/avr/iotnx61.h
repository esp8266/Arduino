/* Copyright (c) 2006, 2007 Anatoly Sokolov
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

/* $Id: iotnx61.h,v 1.6.2.1 2008/09/08 16:47:37 arcanum Exp $ */

/* avr/iotnx61.h - definitions for ATtiny261, ATtiny461 and ATtiny861 */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotnx61.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

#ifndef _AVR_IOTNx61_H_
#define _AVR_IOTNx61_H_ 1

/* Registers and associated bit numbers */

#define TCCR1E  _SFR_IO8(0x00)
#define OC1OE0  0
#define OC1OE1  1
#define OC1OE2  2
#define OC1OE3  3
#define OC1OE4  4
#define OC1OE5  5

#define DIDR0   _SFR_IO8(0x01)
#define ADC0D   0
#define ADC1D   1
#define ADC2D   2
#define AREFD   3
#define ADC3D   4
#define ADC4D   5
#define ADC5D   6
#define ADC6D   7

#define DIDR1   _SFR_IO8(0x02)
#define ADC7D   4
#define ADC8D   5
#define ADC9D   6
#define ADC10D  7

#define ADCSRB  _SFR_IO8(0x03)
#define ADTS0   0
#define ADTS1   1
#define ADTS2   2
#define MUX5    3
#define REFS2   4
#define IRP     5
#define GSEL    6
#define BIN     7

#define ADCW    _SFR_IO16(0x04)
#ifndef __ASSEMBLER__
#define ADC     _SFR_IO16(0x04)
#endif

#define ADCL    _SFR_IO8(0x04)
#define ADCH    _SFR_IO8(0x05)

#define ADCSRA  _SFR_IO8(0x06)
#define ADPS0   0
#define ADPS1   1
#define ADPS2   2
#define ADIE    3
#define ADIF    4
#define ADATE   5
#define ADSC    6
#define ADEN    7

#define ADMUX   _SFR_IO8(0x07)
#define MUX0    0
#define MUX1    1
#define MUX2    2
#define MUX3    3
#define MUX4    4
#define ADLAR   5
#define REFS0   6
#define REFS1   7

#define ACSRA   _SFR_IO8(0x08)
#define ACIS0   0
#define ACIS1   1
#define ACME    2
#define ACIE    3
#define ACI     4
#define ACO     5
#define ACBG    6
#define ACD     7

#define ACSRB   _SFR_IO8(0x09)
#define ACM0    0
#define ACM1    1
#define ACM2    2
#define HLEV    6
#define HSEL    7

#define GPIOR0  _SFR_IO8(0x0A)

#define GPIOR1  _SFR_IO8(0x0B)

#define GPIOR2  _SFR_IO8(0x0C)

#define USICR   _SFR_IO8(0x0D)
#define USITC   0
#define USICLK  1
#define USICS0  2
#define USICS1  3
#define USIWM0  4
#define USIWM1  5
#define USIOIE  6
#define USISIE  7

#define USISR   _SFR_IO8(0x0E)
#define USICNT0 0
#define USICNT1 1
#define USICNT2 2
#define USICNT3 3
#define USIDC   4
#define USIPF   5
#define USIOIF  6
#define USISIF  7

#define USIDR   _SFR_IO8(0x0F)

#define USIBR   _SFR_IO8(0x10)

#define USIPP   _SFR_IO8(0x11)
#define USIPOS  0

#define OCR0B   _SFR_IO8(0x12)

#define OCR0A   _SFR_IO8(0x13)

#define TCNT0H  _SFR_IO8(0x14)

#define TCCR0A  _SFR_IO8(0x15)
#define WGM00   0
#define ACIC0   3
#define ICES0   4
#define ICNC0   5
#define ICEN0   6
#define TCW0    7

#define PINB    _SFR_IO8(0x16)
#define PINB0   0
#define PINB1   1
#define PINB2   2
#define PINB3   3
#define PINB4   4
#define PINB5   5
#define PINB6   6
#define PINB7   7

#define DDRB    _SFR_IO8(0x17)
#define DDB0    0
#define DDB1    1
#define DDB2    2
#define DDB3    3
#define DDB4    4
#define DDB5    5
#define DDB6    6
#define DDB7    7

#define PORTB   _SFR_IO8(0x18)
#define PB0     0
#define PB1     1
#define PB2     2
#define PB3     3
#define PB4     4
#define PB5     5
#define PB6     6
#define PB7     7

#define PINA    _SFR_IO8(0x19)
#define PINA0   0
#define PINA1   1
#define PINA2   2
#define PINA3   3
#define PINA4   4
#define PINA5   5
#define PINA6   6
#define PINA7   7

#define DDRA    _SFR_IO8(0x1A)
#define DDA0    0
#define DDA1    1
#define DDA2    2
#define DDA3    3
#define DDA4    4
#define DDA5    5
#define DDA6    6
#define DDA7    7

#define PORTA   _SFR_IO8(0x1B)
#define PA0     0
#define PA1     1
#define PA2     2
#define PA3     3
#define PA4     4
#define PA5     5
#define PA6     6
#define PA7     7

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)
#define EERE    0
#define EEPE    1
#define EEMPE   2
#define EERIE   3
#define EEPM0   4
#define EEPM1   5

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO16(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEARH	_SFR_IO8(0x1F)

#define DWDR    _SFR_IO8(0x20)

#define WDTCR   _SFR_IO8(0x21)
#define WDP0    0
#define WDP1    1
#define WDP2    2
#define WDE     3
#define WDCE    4
#define WDP3    5
#define WDIE    6
#define WDIF    7

#define PCMSK1  _SFR_IO8(0x22)
#define PCINT8  0
#define PCINT9  1
#define PCINT10 2
#define PCINT11 3
#define PCINT12 4
#define PCINT13 5
#define PCINT14 6
#define PCINT15 7

#define PCMSK0  _SFR_IO8(0x23)
#define PCINT0  0
#define PCINT1  1
#define PCINT2  2
#define PCINT3  3
#define PCINT4  4
#define PCINT5  5
#define PCINT6  6
#define PCINT7  7

#define DT1     _SFR_IO8(0x24)
#define DT1L0   0
#define DT1L1   1
#define DT1L2   2
#define DT1L3   3
#define DT1H0   4
#define DT1H1   5
#define DT1H2   6
#define DT1H3   7

#define TC1H    _SFR_IO8(0x25)
#define TC18    0
#define TC19    1

#define TCCR1D  _SFR_IO8(0x26)
#define WGM10   0
#define WGM11   1
#define FPF1    2
#define FPAC1   3
#define FPES1   4
#define FPNC1   5
#define FPEN1   6
#define FPIE1   7

#define TCCR1C  _SFR_IO8(0x27)
#define PWM1D   0
#define FOC1D   1
#define COM1D0  2
#define COM1D1  3
#define COM1B0S 4
#define COM1B1S 5
#define COM1A0S 6
#define COM1A1S 7

#define CLKPR   _SFR_IO8(0x28)
#define CLKPS0  0
#define CLKPS1  1
#define CLKPS2  2
#define CLKPS3  3
#define CLKPCE  7

#define PLLCSR  _SFR_IO8(0x29)
#define PLOCK   0
#define PLLE    1
#define PCKE    2
#define LSM     7

#define OCR1D   _SFR_IO8(0x2A)

#define OCR1C   _SFR_IO8(0x2B)

#define OCR1B   _SFR_IO8(0x2C)

#define OCR1A   _SFR_IO8(0x2D)

#define TCNT1   _SFR_IO8(0x2E)

#define TCCR1B  _SFR_IO8(0x2F)
#define CS10    0
#define CS11    1
#define CS12    2
#define CS13    3
#define DTPS10  4
#define DTPS11  5
#define PSR1    6
#define PWM1X   7

#define TCCR1A  _SFR_IO8(0x30)
#define PWM1B   0
#define PWM1A   1
#define FOC1B   2
#define FOC1A   3
#define COM1B0  4
#define COM1B1  5
#define COM1A0  6
#define COM1A1  7

#define OSCCAL  _SFR_IO8(0x31)

#define TCNT0L  _SFR_IO8(0x32)

#define TCCR0B  _SFR_IO8(0x33)
#define CS00    0
#define CS01    1
#define CS02    2
#define PSR0    3
#define TSM     4

#define MCUSR   _SFR_IO8(0x34)
#define PORF    0
#define EXTRF   1
#define BORF    2
#define WDRF    3

#define MCUCR   _SFR_IO8(0x35)
#define ISC00   0
#define ISC01   1
#define SM0     3
#define SM1     4
#define SE      5
#define PUD     6

#define PRR     _SFR_IO8(0x36)
#define PRADC   0
#define PRUSI   1
#define PRTIM0  2
#define PRTIM1  3

#define SPMCSR  _SFR_IO8(0x37)
#define SPMEN   0
#define PGERS   1
#define PGWRT 2
#define RFLB 3
#define CTPB 4

#define TIFR    _SFR_IO8(0x38)
#define ICF0    0
#define TOV0    1
#define TOV1    2
#define OCF0B   3
#define OCF0A   4
#define OCF1B   5
#define OCF1A   6
#define OCF1D   7

#define TIMSK   _SFR_IO8(0x39)
#define TICIE0  0
#define TOIE0   1
#define TOIE1   2
#define OCIE0B  3
#define OCIE0A  4
#define OCIE1B  5
#define OCIE1A  6
#define OCIE1D  7

#define GIFR    _SFR_IO8(0x3A)
#define PCIF    5
#define INTF0   6
#define INTF1   7

#define GIMSK   _SFR_IO8(0x3B)
#define PCIE0   4
#define PCIE1   5
#define INT0    6
#define INT1    7

/* Reserved [0x3C] */

/* 0x3D..0x3E SP  [defined in <avr/io.h>] */
/* 0x3F SREG      [defined in <avr/io.h>] */


/* Interrupt vectors */
/* Interrupt vector 0 is the reset vector. */
/* External Interrupt 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* Pin Change Interrupt */
#define PCINT_vect			_VECTOR(2)
#define SIG_PIN_CHANGE			_VECTOR(2)

/* Timer/Counter1 Compare Match 1A */
#define TIMER1_COMPA_vect		_VECTOR(3)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(3)

/* Timer/Counter1 Compare Match 1B */
#define TIMER1_COMPB_vect		_VECTOR(4)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(4)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(5)
#define SIG_OVERFLOW1			_VECTOR(5)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(6)
#define SIG_OVERFLOW0			_VECTOR(6)

/* USI Start */
#define USI_START_vect			_VECTOR(7)
#define SIG_USI_START			_VECTOR(7)

/* USI Overflow */
#define USI_OVF_vect			_VECTOR(8)
#define SIG_USI_OVERFLOW		_VECTOR(8)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(9)
#define SIG_EEPROM_READY		_VECTOR(9)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(10)
#define SIG_ANA_COMP			_VECTOR(10)
#define SIG_COMPARATOR			_VECTOR(10)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(11)
#define SIG_ADC				_VECTOR(11)

/* Watchdog Time-Out */
#define WDT_vect			_VECTOR(12)
#define SIG_WDT				_VECTOR(12)

/* External Interrupt 1 */
#define INT1_vect			_VECTOR(13)
#define SIG_INTERRUPT1			_VECTOR(13)

/* Timer/Counter0 Compare Match A */
#define TIMER0_COMPA_vect		_VECTOR(14)
#define SIG_OUTPUT_COMPARE0A		_VECTOR(14)

/* Timer/Counter0 Compare Match B */
#define TIMER0_COMPB_vect		_VECTOR(15)
#define SIG_OUTPUT_COMPARE0B		_VECTOR(15)

/* ADC Conversion Complete */
#define TIMER0_CAPT_vect		_VECTOR(16)
#define SIG_INPUT_CAPTURE0		_VECTOR(16)

/* Timer/Counter1 Compare Match D */
#define TIMER1_COMPD_vect		_VECTOR(17)
#define SIG_OUTPUT_COMPARE0D		_VECTOR(17)

/* Timer/Counter1 Fault Protection */
#define FAULT_PROTECTION_vect		_VECTOR(18)

#define _VECTORS_SIZE 38

#endif  /* _AVR_IOTNx61_H_ */
