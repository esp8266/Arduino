/* Copyright (c) 2007, Anatoly Sokolov 
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

/* $Id: iomxxhva.h,v 1.1 2007/05/13 20:21:05 aesok Exp $ */

/* iomxxhva.h - definitions for ATmega8HVA and ATmega16HVA.  */

#ifndef _AVR_IOMXXHVA_H_
#define _AVR_IOMXXHVA_H_ 1

/* This file should only be included from <avr/io.h>, never directly.  */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iomxxhva.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* Registers and associated bit numbers */

#define PINA    _SFR_IO8(0X00)
#define PINA1   1
#define PINA0   0

#define DDRA    _SFR_IO8(0x01)
#define DDA1    1
#define DDA0    0

#define PORTA   _SFR_IO8(0x02)
#define PA1     1
#define PA0     0

#define PINB    _SFR_IO8(0X03)
#define PINB3   3
#define PINB2   2
#define PINB1   1
#define PINB0   0

#define DDRB    _SFR_IO8(0x04)
#define DDB3    3
#define DDB2    2
#define DDB1    1
#define DDB0    0

#define PORTB   _SFR_IO8(0x05)
#define PB3     3
#define PB2     2
#define PB1     1
#define PB0     0

#define PINC    _SFR_IO8(0x06)
#define PINC0   0

/* Reserved [0x7] */

#define PORTC   _SFR_IO8(0x08)
#define PC0     0

/* Reserved [0x9..0x14] */

#define TIFR0   _SFR_IO8(0x15)
#define ICF0    3
#define OCF0B   2
#define OCF0A   1
#define TOV0    0

#define TIFR1   _SFR_IO8(0x16)
#define ICF1    3
#define OCF1B   2
#define OCF1A   1
#define TOV1    0

#define OSICSR  _SFR_IO8(0x17)
#define OSISEL0 4
#define OSIST   1
#define OSIEN   0

/* Reserved [0x18..0x1B] */

#define EIFR    _SFR_IO8(0x1C)
#define INTF2   2
#define INTF1   1
#define INTF0   0

#define EIMSK   _SFR_IO8(0x1D)
#define INT2    2
#define INT1    1 
#define INT0    0

#define GPIOR0  _SFR_IO8(0x1E)

#define EECR    _SFR_IO8(0x1F)
#define EEPM1   5
#define EEPM0   4
#define EERIE   3
#define EEMPE   2
#define EEPE    1
#define EERE    0

#define EEDR    _SFR_IO8(0x20)

#define EEAR    _SFR_IO8(0x21)
#define EEARL   _SFR_IO8(0x21)

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

/* Reserved [0x22] */

#define GTCCR   _SFR_IO8(0x23)
#define TSM     7
#define PSRSYNC 0

#define TCCR0A  _SFR_IO8(0x24)
#define TCW0    7
#define ICEN0   6
#define ICNC0   5
#define ICES0   4
#define ICS0    3
#define WGM00   0

#define TCCR0B  _SFR_IO8(0x25)
#define CS02    2
#define CS01    1
#define CS00    0

#define TCNT0   _SFR_IO16(0X26)
#define TCNT0L  _SFR_IO8(0X26)
#define TCNT0H  _SFR_IO8(0X27)

#define OCR0A   _SFR_IO8(0x28)

#define OCR0B   _SFR_IO8(0X29)

#define GPIOR1  _SFR_IO8(0x2A)

#define GPIOR2  _SFR_IO8(0x2B)

#define SPCR    _SFR_IO8(0x2C)
#define SPIE    7
#define SPE     6
#define DORD    5
#define MSTR    4
#define CPOL    3
#define CPHA    2
#define SPR1    1
#define SPR0    0

#define SPSR    _SFR_IO8(0x2D)
#define SPIF    7
#define WCOL    6
#define SPI2X   0

#define SPDR    _SFR_IO8(0x2E)

/* Reserved [0x2F..0x30] */

#define DWDR    _SFR_IO8(0x31)
#define IDRD    7

/* Reserved [0x32] */

#define SMCR    _SFR_IO8(0x33)
#define SM2     3
#define SM1     2
#define SM0     1
#define SE      0

#define MCUSR   _SFR_IO8(0x34)
#define OCDRF   4
#define WDRF    3
#define BORF    2
#define EXTRF   1
#define PORF    0

#define MCUCR   _SFR_IO8(0x35)
#define CKOE    5
#define PUD     4

/* Reserved [0x36] */

#define SPMCSR  _SFR_IO8(0x37)
#define SIGRD   5
#define CTPB    4
#define RFLB    3
#define PGWRT   2
#define PGERS   1
#define SPMEN   0

/* Reserved [0x38..0x3C] */

/* SP [0x3D..0x3E] */
/* SREG [0x3F] */

#define WDTCSR  _SFR_MEM8(0x60)
#define WDIF    7
#define WDIE    6
#define WDP3    5
#define WDCE    4
#define WDE     3
#define WDP2    2
#define WDP1    1
#define WDP0    0

#define CLKPR   _SFR_MEM8(0x61)
#define CLKPCE  7
#define CLKPS1  1
#define CLKPS0  0

/* Reserved [0x62..0x63] */

#define PRR0    _SFR_MEM8(0x64)
#define PRVRM   5
#define PRSPI   3
#define PRTIM1  2
#define PRTIM0  1
#define PRVADC  0

/* Reserved [0x65] */

#define FOSCCAL _SFR_MEM8(0x66)

/* Reserved [0x67..0x68] */

#define EICRA   _SFR_MEM8(0x69)
#define ISC21   5
#define ISC20   4
#define ISC11   3
#define ISC10   2
#define ISC01   1
#define ISC00   0

/* Reserved [0x6A..0x6D] */

#define TIMSK0  _SFR_MEM8(0x6E)
#define ICIE0   3
#define OCIE0B  2
#define OCIE0A  1
#define TOIE0   0

#define TIMSK1  _SFR_MEM8(0x6F)
#define ICIE1   3
#define OCIE1B  2
#define OCIE1A  1
#define TOIE1   0

/* Reserved [0x70..0x77] */

#define VADC	_SFR_MEM16(0x78)
#define VADCL	_SFR_MEM8(0x78)
#define VADCH	_SFR_MEM8(0x79)

#define VADCSR	_SFR_MEM8(0x7A)
#define VADEN   3
#define VADSC   2
#define VADCCIF 1
#define VADCCIE 0

/* Reserved [0x7B] */

#define VADMUX	_SFR_MEM8(0x7C)
#define VADMUX3 3
#define VADMUX2 2
#define VADMUX1 1
#define VADMUX0 0

/* Reserved [0x7D] */

#define DIDR0	_SFR_MEM8(0x7E)
#define PA1DID  1
#define PA0DID  0

/* Reserved [0x7F] */

#define TCCR1A  _SFR_MEM8(0x80)
#define TCW1    7
#define ICEN1   6
#define ICNC1   5
#define ICES1   4
#define ICS1    3
#define WGM10   0

#define TCCR1B  _SFR_MEM8(0x81)
#define CS12    2
#define CS11    1
#define CS10    0

/* Reserved [0x82..0x83] */

#define TCNT1   _SFR_MEM16(0x84)
#define TCNT1L  _SFR_MEM8(0x84)
#define TCNT1H  _SFR_MEM8(0x85)

/* Reserved [0x86..0x87] */

#define OCR1A   _SFR_MEM8(0x88)

#define OCR1B   _SFR_MEM8(0x89)

/* Reserved [0x8A..0xC7] */

#define ROCR    _SFR_MEM8(0xC8)
#define ROCS    7
#define ROCWIF  1
#define ROCWIE  0

/* Reserved [0xC9..0xCF] */

#define BGCCR   _SFR_MEM8(0xD0)
#define BGD     7
#define BGCC5   5
#define BGCC4   4
#define BGCC3   3
#define BGCC2   2
#define BGCC1   1
#define BGCC0   0

#define BGCRR   _SFR_MEM8(0xD1)
#define BGCR7   7
#define BGCR6   6
#define BGCR5   5
#define BGCR4   4
#define BGCR3   3
#define BGCR2   2
#define BGCR1   1
#define BGCR0   0

/* Reserved [0xD2..0xDF] */

/* CC-ADC Accumulate Current */
/* TODO: Add _SFR_MEM32 */
/* #define CADAC   _SFR_MEM32(0xE0) */
#define CADAC0  _SFR_MEM8(0xE0)
#define CADAC1  _SFR_MEM8(0xE1)
#define CADAC2  _SFR_MEM8(0xE2)
#define CADAC3  _SFR_MEM8(0xE3)

#define CADCSRA _SFR_MEM8(0xE4)
#define CADEN   7
#define CADPOL  6
#define CADUB   5
#define CADAS1  4
#define CADAS0  3
#define CADSI1  2
#define CADSI0  1
#define CADSE   0

#define CADCSRB _SFR_MEM8(0xE5)
#define CADACIE 6
#define CADRCIE 5
#define CADICIE 4
#define CADACIF 2
#define CADRCIF 1
#define CADICIF 0

#define CADRC   _SFR_MEM8(0xE6)

/* Reserved [0xE7] */

#define CADIC   _SFR_MEM16(0xE8)
#define CADICL  _SFR_MEM8(0xE8)
#define CADICH  _SFR_MEM8(0xE9)

/* Reserved [0xEA..0xEF] */

#define FCSR    _SFR_MEM8(0xF0)
#define DUVRD   3
#define CPS     2
#define DFE     1
#define CFE     0

/* Reserved [0xF1] */

#define BPIMSK  _SFR_MEM8(0xF2)
#define SCIE    4
#define DOCIE   3
#define COCIE   2
#define DHCIE   1
#define CHCIE   0

#define BPIFR   _SFR_MEM8(0xF3)
#define SCIF    4
#define DOCIF   3
#define COCIF   2
#define DHCIF   1
#define CHCIF   0

/* Reserved [0xF4] */

#define BPSCD   _SFR_MEM8(0xF5)

#define BPDOCD  _SFR_MEM8(0xF6)

#define BPCOCD  _SFR_MEM8(0xF7)

#define BPDHCD  _SFR_MEM8(0xF8)

#define BPCHCD  _SFR_MEM8(0xF9)

#define BPSCTR  _SFR_MEM8(0xFA)

#define BPOCTR  _SFR_MEM8(0xFB)

#define BPHCTR  _SFR_MEM8(0xFC)

#define BPCR    _SFR_MEM8(0xFD)
#define SCD     4
#define DOCD    3
#define COCD    2
#define DHCD    1
#define CHCD    0

#define BPPLR   _SFR_MEM8(0xFE)
#define BPPLE   1
#define BPPL    0

/* Reserved [0xFF] */

/* Interrupt vectors */
/* Battery Protection Interrupt */
#define BPINT_vect			_VECTOR(1)

/* Voltage Regulator Monitor Interrupt */
#define VREGMON_vect			_VECTOR(2)

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(3)

/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(4)

/* External Interrupt Request 2 */
#define INT2_vect			_VECTOR(5)

/* Watchdog Timeout Interrupt */
#define WDT_vect			_VECTOR(6)

/* Timer/Counter 1 Input Capture */
#define TIMER1_IC_vect			_VECTOR(7)

/* Timer/Counter 1 Compare A Match */
#define TIMER1_COMPA_vect		_VECTOR(8)

/* Timer/Counter 1 Compare B Match */
#define TIMER1_COMPB_vect		_VECTOR(9)

/* Timer/Counter 1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(10)

/* Timer/Counter 0 Input Capture */
#define TIMER0_IC_vect			_VECTOR(11)

/* Timer/Counter0 Compare A Match */
#define TIMER0_COMPA_vect		_VECTOR(12)

/* Timer/Counter0 Compare B Match */
#define TIMER0_COMPB_vect		_VECTOR(13)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(14)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(15)

/* Voltage ADC Conversion Complete */
#define VADC_vect			_VECTOR(16)

/* Coulomb Counter ADC Conversion Complete */
#define CCADC_CONV_vect			_VECTOR(17)

/* Coloumb Counter ADC Regular Current */
#define CCADC_REG_CUR_vect		_VECTOR(18)

/* Coloumb Counter ADC Accumulator */
#define CCADC_ACC_vect			_VECTOR(19)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(20)

#if defined (__AVR_ATmega16HVA__)
#  define _VECTORS_SIZE 84
#else
#  define _VECTORS_SIZE 42
#endif


#endif  /* _AVR_IOMXXHVA_H_ */
