/* Copyright (c) 2004, Theodore A. Roth
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

/* $Id: iotn13.h,v 1.8.2.6 2008/10/17 23:27:51 arcanum Exp $ */

/* avr/iotn13.h - definitions for ATtiny13 */

/* Verified 5/20/04 by Bruce Graham */

#ifndef _AVR_IOTN13_H_
#define _AVR_IOTN13_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn13.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers and bit names */

/* ADC Control and Status Register B */
#define ADCSRB               _SFR_IO8(0x03)
#  define ACME                 6
#  define ADTS2                2
#  define ADTS1                1
#  define ADTS0                0

/* ADC Data Register */
#ifndef __ASSEMBLER__
#define ADC                  _SFR_IO16 (0x04)
#endif
#define ADCW                 _SFR_IO16 (0x04)
#define ADCL                 _SFR_IO8(0x04)
#define ADCH                 _SFR_IO8(0x05)

/* ADC Control and Status Register A */
#define ADCSRA               _SFR_IO8(0x06)
#  define ADEN                 7
#  define ADSC                 6
#  define ADATE                5
#  define ADIF                 4
#  define ADIE                 3
#  define ADPS2                2
#  define ADPS1                1
#  define ADPS0                0

/* ADC Multiplex Selection Register */
#define ADMUX                _SFR_IO8(0x07)
#  define REFS0                6
#  define ADLAR                5
#  define MUX1                 1
#  define MUX0                 0

/* Analog Comparator Control and Status Register */
#define ACSR                 _SFR_IO8(0x08)
#  define ACD                  7
#  define ACBG                 6
#  define ACO                  5
#  define ACI                  4
#  define ACIE                 3
#  define ACIS1                1
#  define ACIS0                0

/* Digital Input Disable Register 0 */
#define DIDR0                _SFR_IO8(0x14)
#  define ADC0D                5
#  define ADC2D                4
#  define ADC3D                3
#  define ADC1D                2
#  define AIN1D                1
#  define AIN0D                0

/* PIN Change Mask Register */
#define PCMSK                _SFR_IO8(0x15)
#  define PCINT5               5
#  define PCINT4               4
#  define PCINT3               3
#  define PCINT2               2
#  define PCINT1               1
#  define PCINT0               0

/* Port B Pin Utilization [2535D-AVR-04/04]
   - PORTB5 = PCINT5/RESET#/ADC0/dW
   - PORTB4 = PCINT4/ADC2
   - PORTB3 = PCINT3/CLKI/ADC3
   - PORTB2 = SCK/ADC1/T0/PCINT2
   - PORTB1 = MISO/AIN1/OC0B/INT0/PCINT1
   - PORTB0 = MOSI/AIN0/OC0A/PCINT0 */

/* Input Pins, Port B */
#define PINB                 _SFR_IO8(0x16)
#  define PINB5                5
#  define PINB4                4
#  define PINB3                3
#  define PINB2                2
#  define PINB1                1
#  define PINB0                0

/* Data Direction Register, Port B */
#define DDRB                 _SFR_IO8(0x17)
#  define DDB5                 5
#  define DDB4                 4
#  define DDB3                 3
#  define DDB2                 2
#  define DDB1                 1
#  define DDB0                 0

/* Data Register, Port B */
#define PORTB                _SFR_IO8(0x18)
#  define PB5                  5
#  define PB4                  4
#  define PB3                  3
#  define PB2                  2
#  define PB1                  1
#  define PB0                  0

/* ATtiny EEPROM Control Register EECR */
#define EECR	             _SFR_IO8(0x1C)
#define EEPM1                  5
#define EEPM0                  4
#define EERIE                  3
#define EEMPE                  2
#define EEPE                   1
#define EERE                   0

/* EEPROM Data Register */
#define EEDR	             _SFR_IO8(0x1D)

/* The EEPROM Address Register EEAR[6:0] */
#define EEAR	             _SFR_IO8(0x1E)
#define EEARL	             _SFR_IO8(0x1E)

/* Watchdog Timer Control Register */
#define WDTCR                _SFR_IO8(0x21)
#  define WDTIF                7
#  define WDTIE                6
#  define WDP3                 5
#  define WDCE                 4
#  define WDE                  3
#  define WDP2                 2
#  define WDP1                 1
#  define WDP0                 0

/* Clock Prescale Register */
#define CLKPR                _SFR_IO8(0x26)
#  define CLKPCE               7
#  define CLKPS3               3
#  define CLKPS2               2
#  define CLKPS1               1
#  define CLKPS0               0

/* General Timer/Counter Control Register */
#define GTCCR                _SFR_IO8(0x28)
#  define TSM                  7
#  define PSR10                0

/* Output Compare 0 Register B */
#define OCR0B                _SFR_IO8(0x29)

/* debugWIRE Data Register */
#define DWDR                 _SFR_IO8(0x2e)

/* Timer/Counter 0 Control Register A */
#define TCCR0A               _SFR_IO8(0x2f)
#  define COM0A1               7
#  define COM0A0               6
#  define COM0B1               5
#  define COM0B0               4
#  define WGM01                1
#  define WGM00                0

/* Oscillator Calibration Register */
#define OSCCAL               _SFR_IO8(0x31)

/* Timer/Counter0 (8-bit) */
#define TCNT0                _SFR_IO8(0x32)

/* Timer/Counter 0 Control Register B */
#define TCCR0B               _SFR_IO8(0x33)
#  define FOC0A                7
#  define FOC0B                6
#  define WGM02                3
#  define CS02                 2
#  define CS01                 1
#  define CS00                 0

/* MCU General Status Register */
#define MCUSR                _SFR_IO8(0x34)
#  define WDRF                 3
#  define BORF                 2
#  define EXTRF                1
#  define PORF                 0

/* MCU General Control Register */
#define MCUCR                _SFR_IO8(0x35)
#  define PUD                  6
#  define SE                   5
#  define SM1                  4
#  define SM0                  3
#  define ISC01                1
#  define ISC00                0

/* Output Compare 0 REgister A */
#define OCR0A                _SFR_IO8(0x36)

/* Store Program Memory Control and Status Register */
#define SPMCSR               _SFR_IO8(0x37)
#  define CTPB                 4
#  define RFLB                 3
#  define PGWRT                2
#  define PGERS                1
#  define SPMEN                0
#  define SELFPRGEN            0

/* Timer/Counter 0 Interrupt Flag Register */
#define TIFR0                _SFR_IO8(0x38)
#  define OCF0B                3
#  define OCF0A                2
#  define TOV0                 1

/* Timer/Counter 0 Interrupt MaSK Register */
#define TIMSK0               _SFR_IO8(0x39)
#  define OCIE0B               3
#  define OCIE0A               2
#  define TOIE0                1

/* General Interrupt Flag Register */
#define GIFR                 _SFR_IO8(0x3a)
#  define INTF0                6
#  define PCIF                 5

/* General Interrupt MaSK register */
#define GIMSK                _SFR_IO8(0x3b)
#  define INT0                 6
#  define PCIE                 5

/* SPL and SREG are defined in <avr/io.h> */

/* From the datasheet:
   1 0x0000 RESET External Pin, Power-on Reset, Brown-out Reset, Watchdog Reset
   2 0x0001 INT0 External Interrupt Request 0
   3 0x0002 PCINT0 Pin Change Interrupt Request 0
   4 0x0003 TIM0_OVF Timer/Counter Overflow
   5 0x0004 EE_RDY EEPROM Ready
   6 0x0005 ANA_COMP Analog Comparator
   7 0x0006 TIM0_COMPA Timer/Counter Compare Match A
   8 0x0007 TIM0_COMPB Timer/Counter Compare Match B
   9 0x0008 WDT Watchdog Time-out
   10 0x0009 ADC ADC Conversion Complete */

/* External Interrupt 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt Request 0 */
#define PCINT0_vect			_VECTOR(2)
#define SIG_PIN_CHANGE0			_VECTOR(2)

/* Timer/Counter0 Overflow */
#define TIM0_OVF_vect			_VECTOR(3)
#define SIG_OVERFLOW0			_VECTOR(3)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(4)
#define SIG_EEPROM_READY		_VECTOR(4)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(5)
#define SIG_COMPARATOR			_VECTOR(5)

/* Timer/Counter Compare Match A */
#define TIM0_COMPA_vect			_VECTOR(6)
#define SIG_OUTPUT_COMPARE0A		_VECTOR(6)

/* Timer/Counter Compare Match B */
#define TIM0_COMPB_vect			_VECTOR(7)
#define SIG_OUTPUT_COMPARE0B		_VECTOR(7)

/* Watchdog Time-out */
#define WDT_vect			_VECTOR(8)
#define SIG_WATCHDOG_TIMEOUT		_VECTOR(8)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(9)
#define SIG_ADC				_VECTOR(9)

#define _VECTORS_SIZE 20

#define SPM_PAGESIZE 32
#define RAMEND      0x9F
#define XRAMEND     0x9F
#define E2END       0x3F
#define E2PAGESIZE  4
#define FLASHEND    0x3FF


/* Fuses */

#define FUSE_MEMORY_SIZE 2

/* Low Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_SUT0        (unsigned char)~_BV(2)
#define FUSE_SUT1        (unsigned char)~_BV(3)
#define FUSE_CKDIV8      (unsigned char)~_BV(4)
#define FUSE_WDTON       (unsigned char)~_BV(5)
#define FUSE_EESAVE      (unsigned char)~_BV(6)
#define FUSE_SPIEN       (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_CKSEL0 & FUSE_SUT0 & FUSE_CKDIV8 & FUSE_SPIEN)

/* High Fuse Byte */
#define FUSE_RSTDISBL    (unsigned char)~_BV(0)
#define FUSE_BODLEVEL0   (unsigned char)~_BV(1)
#define FUSE_BODLEVEL1   (unsigned char)~_BV(2)
#define FUSE_DWEN        (unsigned char)~_BV(3)
#define FUSE_SPMEN       (unsigned char)~_BV(4)
#define HFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x90
#define SIGNATURE_2 0x07


#endif /* _AVR_IOTN13_H_*/
