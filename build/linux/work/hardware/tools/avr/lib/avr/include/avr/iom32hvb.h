/* Copyright (c) 2007 Atmel Corporation
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
  POSSIBILITY OF SUCH DAMAGE. 
*/

/* $Id: iom32hvb.h,v 1.3.2.4 2008/08/06 22:45:05 arcanum Exp $ */

/* avr/iom32hvb.h - definitions for ATmega32HVB. */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom32hvb.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_IOM32HVB_H_
#define _AVR_IOM32HVB_H_ 1

/* Registers and associated bit numbers */

#define PINA _SFR_IO8(0x00)
#define PINA0 0
#define PINA1 1
#define PINA2 2
#define PINA3 3

#define DDRA _SFR_IO8(0x01)
#define DDA0 0
#define DDA1 1
#define DDA2 2
#define DDA3 3

#define PORTA _SFR_IO8(0x02)
#define PORTA0 0
#define PORTA1 1
#define PORTA2 2
#define PORTA3 3

#define PINB _SFR_IO8(0x03)
#define PINB0 0
#define PINB1 1
#define PINB2 2
#define PINB3 3
#define PINB4 4
#define PINB5 5
#define PINB6 6
#define PINB7 7

#define DDRB _SFR_IO8(0x04)
#define DDB0 0
#define DDB1 1
#define DDB2 2
#define DDB3 3
#define DDB4 4
#define DDB5 5
#define DDB6 6
#define DDB7 7

#define PORTB _SFR_IO8(0x05)
#define PORTB0 0
#define PORTB1 1
#define PORTB2 2
#define PORTB3 3
#define PORTB4 4
#define PORTB5 5
#define PORTB6 6
#define PORTB7 7

#define PINC _SFR_IO8(0x06)
#define PINC0 0
#define PINC1 1
#define PINC2 2
#define PINC3 3
#define PINC4 4

#define PORTC _SFR_IO8(0x08)
#define PORTC0 0
#define PORTC1 1
#define PORTC2 2
#define PORTC3 3
#define PORTC4 4
#define PORTC5 5

#define TIFR0 _SFR_IO8(0x15)
#define TOV0 0
#define OCF0A 1
#define OCF0B 2
#define ICF0 3

#define TIFR1 _SFR_IO8(0x16)
#define TOV1 0
#define OCF1A 1
#define OCF1B 2
#define ICF1 3

#define OSICSR _SFR_IO8(0x17)
#define OSIEN 0
#define OSIST 1
#define OSISEL0 4

#define PCIFR _SFR_IO8(0x1B)
#define PCIF0 0
#define PCIF1 1

#define EIFR _SFR_IO8(0x1C)
#define INTF0 0
#define INTF1 1
#define INTF2 2
#define INTF3 3

#define EIMSK _SFR_IO8(0x1D)
#define INT0 0
#define INT1 1
#define INT2 2
#define INT3 3

#define GPIOR0 _SFR_IO8(0x1E)
#define GPIOR00 0
#define GPIOR01 1
#define GPIOR02 2
#define GPIOR03 3
#define GPIOR04 4
#define GPIOR05 5
#define GPIOR06 6
#define GPIOR07 7

#define EECR _SFR_IO8(0x1F)
#define EERE 0
#define EEPE 1
#define EEMPE 2
#define EERIE 3
#define EEPM0 4
#define EEPM1 5

#define EEDR _SFR_IO8(0x20)
#define EEDR0 0
#define EEDR1 1
#define EEDR2 2
#define EEDR3 3
#define EEDR4 4
#define EEDR5 5
#define EEDR6 6
#define EEDR7 7

#define EEAR _SFR_IO16(0x21)

#define EEARL _SFR_IO8(0x21)
#define EEAR0 0
#define EEAR1 1
#define EEAR2 2
#define EEAR3 3
#define EEAR4 4
#define EEAR5 5
#define EEAR6 6
#define EEAR7 7

#define EEARH _SFR_IO8(0x22)
#define EEAR8 0
#define EEAR9 1

#define GTCCR _SFR_IO8(0x23)
#define PSRSYNC 0
#define TSM 7

#define TCCR0A _SFR_IO8(0x24)
#define WGM00 0
#define ICS0 3
#define ICES0 4
#define ICNC0 5
#define ICEN0 6
#define TCW0 7

#define TCCR0B _SFR_IO8(0x25)
#define CS00 0
#define CS01 1
#define CS02 2

#define TCNT0 _SFR_IO16(0x26)

#define TCNT0L _SFR_IO8(0x26)
#define TCNT0L0 0
#define TCNT0L1 1
#define TCNT0L2 2
#define TCNT0L3 3
#define TCNT0L4 4
#define TCNT0L5 5
#define TCNT0L6 6
#define TCNT0L7 7

#define TCNT0H _SFR_IO8(0x27)
#define TCNT0H0 0
#define TCNT0H1 1
#define TCNT0H2 2
#define TCNT0H3 3
#define TCNT0H4 4
#define TCNT0H5 5
#define TCNT0H6 6
#define TCNT0H7 7

#define OCR0A _SFR_IO8(0x28)
#define OCR0A0 0
#define OCR0A1 1
#define OCR0A2 2
#define OCR0A3 3
#define OCR0A4 4
#define OCR0A5 5
#define OCR0A6 6
#define OCR0A7 7

#define OCR0B _SFR_IO8(0x29)
#define OCR0B0 0
#define OCR0B1 1
#define OCR0B2 2
#define OCR0B3 3
#define OCR0B4 4
#define OCR0B5 5
#define OCR0B6 6
#define OCR0B7 7

#define GPIOR1 _SFR_IO8(0x2A)
#define GPIOR10 0
#define GPIOR11 1
#define GPIOR12 2
#define GPIOR13 3
#define GPIOR14 4
#define GPIOR15 5
#define GPIOR16 6
#define GPIOR17 7

#define GPIOR2 _SFR_IO8(0x2B)
#define GPIOR20 0
#define GPIOR21 1
#define GPIOR22 2
#define GPIOR23 3
#define GPIOR24 4
#define GPIOR25 5
#define GPIOR26 6
#define GPIOR27 7

#define SPCR _SFR_IO8(0x2C)
#define SPR0 0
#define SPR1 1
#define CPHA 2
#define CPOL 3
#define MSTR 4
#define DORD 5
#define SPE 6
#define SPIE 7

#define SPSR _SFR_IO8(0x2D)
#define SPI2X 0
#define WCOL 6
#define SPIF 7

#define SPDR _SFR_IO8(0x2E)
#define SPDR0 0
#define SPDR1 1
#define SPDR2 2
#define SPDR3 3
#define SPDR4 4
#define SPDR5 5
#define SPDR6 6
#define SPDR7 7

#define SMCR _SFR_IO8(0x33)
#define SE 0
#define SM0 1
#define SM1 2
#define SM2 3

#define MCUSR _SFR_IO8(0x34)
#define PORF 0
#define EXTRF 1
#define BODRF 2
#define WDRF 3
#define OCDRF 4

#define MCUCR _SFR_IO8(0x35)
#define IVCE 0
#define IVSEL 1
#define PUD 4
#define CKOE 5

#define SPMCSR _SFR_IO8(0x37)
#define SPMEN 0
#define PGERS 1
#define PGWRT 2
#define LBSET 3
#define RWWSRE 4
#define SIGRD 5
#define RWWSB 6
#define SPMIE 7

#define WDTCSR _SFR_MEM8(0x60)
#define WDP0 0
#define WDP1 1
#define WDP2 2
#define WDE 3
#define WDCE 4
#define WDP3 5
#define WDIE 6
#define WDIF 7

#define CLKPR _SFR_MEM8(0x61)
#define CLKPS0 0
#define CLKPS1 1
#define CLKPCE 7

#define PRR0 _SFR_MEM8(0x64)
#define PRVADC 0
#define PRTIM0 1
#define PRTIM1 2
#define PRSPI 3
#define PRVRM 5
#define PRTWI 6

#define FOSCCAL _SFR_MEM8(0x66)
#define FCAL0 0
#define FCAL1 1
#define FCAL2 2
#define FCAL3 3
#define FCAL4 4
#define FCAL5 5
#define FCAL6 6
#define FCAL7 7

#define PCICR _SFR_MEM8(0x68)
#define PCIE0 0
#define PCIE1 1

#define EICRA _SFR_MEM8(0x69)
#define ISC00 0
#define ISC01 1
#define ISC10 2
#define ISC11 3
#define ISC20 4
#define ISC21 5
#define ISC30 6
#define ISC31 7

#define PCMSK0 _SFR_MEM8(0x6B)
#define PCINT0 0
#define PCINT1 1
#define PCINT2 2
#define PCINT3 3

#define PCMSK1 _SFR_MEM8(0x6C)
#define PCINT4 0
#define PCINT5 1
#define PCINT6 2
#define PCINT7 3
#define PCINT8 4
#define PCINT9 5
#define PCINT10 6
#define PCINT11 7

#define TIMSK0 _SFR_MEM8(0x6E)
#define TOIE0 0
#define OCIE0A 1
#define OCIE0B 2
#define ICIE0 3

#define TIMSK1 _SFR_MEM8(0x6F)
#define TOIE1 0
#define OCIE1A 1
#define OCIE1B 2
#define ICIE1 3

#define VADC _SFR_MEM16(0x78)

#define VADCL _SFR_MEM8(0x78)
#define VADC0 0
#define VADC1 1
#define VADC2 2
#define VADC3 3
#define VADC4 4
#define VADC5 5
#define VADC6 6
#define VADC7 7

#define VADCH _SFR_MEM8(0x79)
#define VADC8 0
#define VADC9 1
#define VADC10 2
#define VADC11 3

#define VADCSR _SFR_MEM8(0x7A)
#define VADCCIE 0
#define VADCCIF 1
#define VADSC 2
#define VADEN 3

#define VADMUX _SFR_MEM8(0x7C)
#define VADMUX0 0
#define VADMUX1 1
#define VADMUX2 2
#define VADMUX3 3

#define DIDR0 _SFR_MEM8(0x7E)
#define PA0DID 0
#define PA1DID 1

#define TCCR1A _SFR_MEM8(0x80)
#define WGM10 0
#define ICS1 3
#define ICES1 4
#define ICNC1 5
#define ICEN1 6
#define TCW1 7

#define TCCR1B _SFR_MEM8(0x81)
#define CS10 0
#define CS11 1
#define CS12 2

#define TCNT1 _SFR_MEM16(0x84)

#define TCNT1L _SFR_MEM8(0x84)
#define TCNT1L0 0
#define TCNT1L1 1
#define TCNT1L2 2
#define TCNT1L3 3
#define TCNT1L4 4
#define TCNT1L5 5
#define TCNT1L6 6
#define TCNT1L7 7

#define TCNT1H _SFR_MEM8(0x85)
#define TCNT1H0 0
#define TCNT1H1 1
#define TCNT1H2 2
#define TCNT1H3 3
#define TCNT1H4 4
#define TCNT1H5 5
#define TCNT1H6 6
#define TCNT1H7 7

#define OCR1A _SFR_MEM8(0x88)
#define OCR1A0 0
#define OCR1A1 1
#define OCR1A2 2
#define OCR1A3 3
#define OCR1A4 4
#define OCR1A5 5
#define OCR1A6 6
#define OCR1A7 7

#define OCR1B _SFR_MEM8(0x89)
#define OCR1B0 0
#define OCR1B1 1
#define OCR1B2 2
#define OCR1B3 3
#define OCR1B4 4
#define OCR1B5 5
#define OCR1B6 6
#define OCR1B7 7

#define TWBR _SFR_MEM8(0xB8)
#define TWBR0 0
#define TWBR1 1
#define TWBR2 2
#define TWBR3 3
#define TWBR4 4
#define TWBR5 5
#define TWBR6 6
#define TWBR7 7

#define TWSR _SFR_MEM8(0xB9)
#define TWPS0 0
#define TWPS1 1
#define TWS3 3
#define TWS4 4
#define TWS5 5
#define TWS6 6
#define TWS7 7

#define TWAR _SFR_MEM8(0xBA)
#define TWGCE 0
#define TWA0 1
#define TWA1 2
#define TWA2 3
#define TWA3 4
#define TWA4 5
#define TWA5 6
#define TWA6 7

#define TWDR _SFR_MEM8(0xBB)
#define TWD0 0
#define TWD1 1
#define TWD2 2
#define TWD3 3
#define TWD4 4
#define TWD5 5
#define TWD6 6
#define TWD7 7

#define TWCR _SFR_MEM8(0xBC)
#define TWIE 0
#define TWEN 2
#define TWWC 3
#define TWSTO 4
#define TWSTA 5
#define TWEA 6
#define TWINT 7

#define TWAMR _SFR_MEM8(0xBD)
#define TWAM0 0
#define TWAM1 1
#define TWAM2 2
#define TWAM3 3
#define TWAM4 4
#define TWAM5 5
#define TWAM6 6

#define TWBCSR _SFR_MEM8(0xBE)
#define TWBCIP 0
#define TWBDT0 1
#define TWBDT1 2
#define TWBCIE 6
#define TWBCIF 7

#define ROCR _SFR_MEM8(0xC8)
#define ROCWIE 0
#define ROCWIF 1
#define ROCD 4
#define ROCS 7

#define BGCCR _SFR_MEM8(0xD0)
#define BGCC0 0
#define BGCC1 1
#define BGCC2 2
#define BGCC3 3
#define BGCC4 4
#define BGCC5 5

#define BGCRR _SFR_MEM8(0xD1)
#define BGCR0 0
#define BGCR1 1
#define BGCR2 2
#define BGCR3 3
#define BGCR4 4
#define BGCR5 5
#define BGCR6 6
#define BGCR7 7

#define BGCSR _SFR_MEM8(0xD2)
#define BGSCDIE 0
#define BGSCDIF 1
#define BGSCDE 4
#define BGD 5

#define CHGDCSR _SFR_MEM8(0xD4)
#define CHGDIE 0
#define CHGDIF 1
#define CHGDISC0 2
#define CHGDISC1 3
#define BATTPVL 4

#define CADAC _SFR_MEM32(0xE0)

#define CADAC0 _SFR_MEM8(0xE0)
#define CADAC00 0
#define CADAC01 1
#define CADAC02 2
#define CADAC03 3
#define CADAC04 4
#define CADAC05 5
#define CADAC06 6
#define CADAC07 7

#define CADAC1 _SFR_MEM8(0xE1)
#define CADAC08 0
#define CADAC09 1
#define CADAC10 2
#define CADAC11 3
#define CADAC12 4
#define CADAC13 5
#define CADAC14 6
#define CADAC15 7

#define CADAC2 _SFR_MEM8(0xE2)
#define CADAC16 0
#define CADAC17 1
#define CADAC18 2
#define CADAC19 3
#define CADAC20 4
#define CADAC21 5
#define CADAC22 6
#define CADAC23 7

#define CADAC3 _SFR_MEM8(0xE3)
#define CADAC24 0
#define CADAC25 1
#define CADAC26 2
#define CADAC27 3
#define CADAC28 4
#define CADAC29 5
#define CADAC30 6
#define CADAC31 7

#define CADIC _SFR_MEM16(0xE4)

#define CADICL _SFR_MEM8(0xE4)
#define CADICL0 0
#define CADICL1 1
#define CADICL2 2
#define CADICL3 3
#define CADICL4 4
#define CADICL5 5
#define CADICL6 6
#define CADICL7 7

#define CADICH _SFR_MEM8(0xE5)
#define CADICH0 0
#define CADICH1 1
#define CADICH2 2
#define CADICH3 3
#define CADICH4 4
#define CADICH5 5
#define CADICH6 6
#define CADICH7 7

#define CADCSRA _SFR_MEM8(0xE6)
#define CADSE 0
#define CADSI0 1
#define CADSI1 2
#define CADAS0 3
#define CADAS1 4
#define CADUB 5
#define CADPOL 6
#define CADEN 7

#define CADCSRB _SFR_MEM8(0xE7)
#define CADICIF 0
#define CADRCIF 1
#define CADACIF 2
#define CADICIE 4
#define CADRCIE 5
#define CADACIE 6

#define CADCSRC _SFR_MEM8(0xE8)
#define CADVSE 0

#define CADRCC _SFR_MEM8(0xE9)
#define CADRCC0 0
#define CADRCC1 1
#define CADRCC2 2
#define CADRCC3 3
#define CADRCC4 4
#define CADRCC5 5
#define CADRCC6 6
#define CADRCC7 7

#define CADRDC _SFR_MEM8(0xEA)
#define CADRDC0 0
#define CADRDC1 1
#define CADRDC2 2
#define CADRDC3 3
#define CADRDC4 4
#define CADRDC5 5
#define CADRDC6 6
#define CADRDC7 7

#define FCSR _SFR_MEM8(0xF0)
#define CFE 0
#define DFE 1
#define CPS 2
#define DUVRD 3

#define CBCR _SFR_MEM8(0xF1)
#define CBE1 0
#define CBE2 1
#define CBE3 2
#define CBE4 3

#define BPIMSK _SFR_MEM8(0xF2)
#define CHCIE 0
#define DHCIE 1
#define COCIE 2
#define DOCIE 3
#define SCIE 4

#define BPIFR _SFR_MEM8(0xF3)
#define CHCIF 0
#define DHCIF 1
#define COCIF 2
#define DOCIF 3
#define SCIF 4

#define BPSCD _SFR_MEM8(0xF5)
#define SCDL0 0
#define SCDL1 1
#define SCDL2 2
#define SCDL3 3
#define SCDL4 4
#define SCDL5 5
#define SCDL6 6
#define SCDL7 7

#define BPDOCD _SFR_MEM8(0xF6)
#define DOCDL0 0
#define DOCDL1 1
#define DOCDL2 2
#define DOCDL3 3
#define DOCDL4 4
#define DOCDL5 5
#define DOCDL6 6
#define DOCDL7 7

#define BPCOCD _SFR_MEM8(0xF7)
#define COCDL0 0
#define COCDL1 1
#define COCDL2 2
#define COCDL3 3
#define COCDL4 4
#define COCDL5 5
#define COCDL6 6
#define COCDL7 7

#define BPDHCD _SFR_MEM8(0xF8)
#define DHCDL0 0
#define DHCDL1 1
#define DHCDL2 2
#define DHCDL3 3
#define DHCDL4 4
#define DHCDL5 5
#define DHCDL6 6
#define DHCDL7 7

#define BPCHCD _SFR_MEM8(0xF9)
#define CHCDL0 0
#define CHCDL1 1
#define CHCDL2 2
#define CHCDL3 3
#define CHCDL4 4
#define CHCDL5 5
#define CHCDL6 6
#define CHCDL7 7

#define BPSCTR _SFR_MEM8(0xFA)
#define SCPT0 0
#define SCPT1 1
#define SCPT2 2
#define SCPT3 3
#define SCPT4 4
#define SCPT5 5
#define SCPT6 6

#define BPOCTR _SFR_MEM8(0xFB)
#define OCPT0 0
#define OCPT1 1
#define OCPT2 2
#define OCPT3 3
#define OCPT4 4
#define OCPT5 5

#define BPHCTR _SFR_MEM8(0xFC)
#define HCPT0 0
#define HCPT1 1
#define HCPT2 2
#define HCPT3 3
#define HCPT4 4
#define HCPT5 5

#define BPCR _SFR_MEM8(0xFD)
#define CHCD 0
#define DHCD 1
#define COCD 2
#define DOCD 3
#define SCD 4
#define EPID 5

#define BPPLR _SFR_MEM8(0xFE)
#define BPPL 0
#define BPPLE 1



/* Interrupt Vectors */
/* Interrupt Vector 0 is the reset vector. */

#define BPINT_vect         _VECTOR(1)  /* Battery Protection Interrupt */
#define VREGMON_vect       _VECTOR(2)  /* Voltage regulator monitor interrupt */
#define INT0_vect          _VECTOR(3)  /* External Interrupt Request 0 */
#define INT1_vect          _VECTOR(4)  /* External Interrupt Request 1 */
#define INT2_vect          _VECTOR(5)  /* External Interrupt Request 2 */
#define INT3_vect          _VECTOR(6)  /* External Interrupt Request 3 */
#define PCINT0_vect        _VECTOR(7)  /* Pin Change Interrupt 0 */
#define PCINT1_vect        _VECTOR(8)  /* Pin Change Interrupt 1 */
#define WDT_vect           _VECTOR(9)  /* Watchdog Timeout Interrupt */
#define BGSCD_vect         _VECTOR(10)  /* Bandgap Buffer Short Circuit Detected */
#define CHDET_vect         _VECTOR(11)  /* Charger Detect */
#define TIMER1_IC_vect     _VECTOR(12)  /* Timer 1 Input capture */
#define TIMER1_COMPA_vect  _VECTOR(13)  /* Timer 1 Compare Match A */
#define TIMER1_COMPB_vect  _VECTOR(14)  /* Timer 1 Compare Match B */
#define TIMER1_OVF_vect    _VECTOR(15)  /* Timer 1 overflow */
#define TIMER0_IC_vect     _VECTOR(16)  /* Timer 0 Input Capture */
#define TIMER0_COMPA_vect  _VECTOR(17)  /* Timer 0 Comapre Match A */
#define TIMER0_COMPB_vect  _VECTOR(18)  /* Timer 0 Compare Match B */
#define TIMER0_OVF_vect    _VECTOR(19)  /* Timer 0 Overflow */
#define TWIBUSCD_vect      _VECTOR(20)  /* Two-Wire Bus Connect/Disconnect */
#define TWI_vect           _VECTOR(21)  /* Two-Wire Serial Interface */
#define SPI_STC_vect       _VECTOR(22)  /* SPI Serial transfer complete */
#define VADC_vect          _VECTOR(23)  /* Voltage ADC Conversion Complete */
#define CCADC_CONV_vect    _VECTOR(24)  /* Coulomb Counter ADC Conversion Complete */
#define CCADC_REG_CUR_vect _VECTOR(25)  /* Coloumb Counter ADC Regular Current */
#define CCADC_ACC_vect     _VECTOR(26)  /* Coloumb Counter ADC Accumulator */
#define EE_READY_vect      _VECTOR(27)  /* EEPROM Ready */
#define SPM_vect           _VECTOR(28)  /* SPM Ready */

#define _VECTORS_SIZE (29 * 4)


/* Constants */
#define SPM_PAGESIZE 64
#define RAMEND       0x8FF     /* Last On-Chip SRAM Location */
#define XRAMSIZE     0
#define XRAMEND      (RAMEND + XRAMSIZE)
#define E2END        0x3FF
#define FLASHEND     0x7FFF


/* Fuses */

#define FUSE_MEMORY_SIZE 2

/* Low Fuse Byte */
#define FUSE_WDTON   (unsigned char)~_BV(7)  /* Watchdog Timer Always On */
#define FUSE_EESAVE  (unsigned char)~_BV(6)  /* EEPROM memory is preserved through chip erase */
#define FUSE_SPIEN   (unsigned char)~_BV(5)  /* Enable Serial programming and Data Downloading */
#define FUSE_SUT2    (unsigned char)~_BV(4)  /* Select start-up time */
#define FUSE_SUT1    (unsigned char)~_BV(3)  /* Select start-up time */
#define FUSE_SUT0    (unsigned char)~_BV(2)  /* Select start-up time */
#define FUSE_OSCSEL1 (unsigned char)~_BV(1)  /* Oscillator Select */
#define FUSE_OSCSEL0 (unsigned char)~_BV(0)  /* Oscillator Select */
#define LFUSE_DEFAULT (FUSE_OSCSEL0 & FUSE_SPIEN)

/* High Fuse Byte */
#define FUSE_BOOTRST   (unsigned char)~_BV(0)  /* Select Reset Vector */
#define FUSE_BOOTSZ0   (unsigned char)~_BV(1)  /* Select Boot Size */
#define FUSE_BOOTSZ1   (unsigned char)~_BV(2)  /* Select Boot Size */
#define FUSE_DWEN      (unsigned char)~_BV(3)  /* Enable debugWire */
#define FUSE_DUVRDINIT (unsigned char)~_BV(4)  /* Reset Value of DUVRDRegister */
#define HFUSE_DEFAULT (FUSE_BOOTSZ0 & FUSE_DUVRDINIT)



/* Lock Bits */
#define __LOCK_BITS_EXIST


#endif  /* _AVR_IOM32HVB_H_ */
