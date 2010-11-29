/* Copyright (c) 2008 Atmel Corporation
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

/* $Id: iotn167.h,v 1.1.2.5 2008/08/06 22:45:08 arcanum Exp $ */

/* avr/iotn167.h - definitions for ATtiny167. */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn167.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_IOTN167_H_
#define _AVR_IOTN167_H_ 1


/* Registers and associated bit numbers */

#define PINA _SFR_IO8(0x00)
#define PINA0 0
#define PINA1 1
#define PINA2 2
#define PINA3 3
#define PINA4 4
#define PINA5 5
#define PINA6 6
#define PINA7 7

#define DDRA _SFR_IO8(0x01)
#define DDA0 0
#define DDA1 1
#define DDA2 2
#define DDA3 3
#define DDA4 4
#define DDA5 5
#define DDA6 6
#define DDA7 7

#define PORTA _SFR_IO8(0x02)
#define PORTA0 0
#define PORTA1 1
#define PORTA2 2
#define PORTA3 3
#define PORTA4 4
#define PORTA5 5
#define PORTA6 6
#define PORTA7 7

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

#define PORTCR _SFR_IO8(0x12)
#define PUDA 0
#define PUDB 2
#define BBMA 4
#define BBMB 5

#define TIFR0 _SFR_IO8(0x15)
#define TOV0 0
#define OCF0A 1

#define TIFR1 _SFR_IO8(0x16)
#define TOV1 0
#define OCF1A 1
#define OCF1B 2
#define ICF1 5

#define PCIFR _SFR_IO8(0x1B)
#define PCIF0 0
#define PCIF1 1

#define EIFR _SFR_IO8(0x1C)
#define INTF0 0
#define INTF1 1

#define EIMSK _SFR_IO8(0x1D)
#define INT0 0
#define INT1 1

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

#define GTCCR _SFR_IO8(0x23)
#define PSR1 0
#define PSR0 1
#define TSM 7

#define TCCR0A _SFR_IO8(0x25)
#define WGM00 0
#define WGM01 1
#define COM0A0 6
#define COM0A1 7

#define TCCR0B _SFR_IO8(0x26)
#define CS00 0
#define CS01 1
#define CS02 2
#define FOC0A 7

#define TCNT0 _SFR_IO8(0x27)
#define TCNT00 0
#define TCNT01 1
#define TCNT02 2
#define TCNT03 3
#define TCNT04 4
#define TCNT05 5
#define TCNT06 6
#define TCNT07 7

#define OCR0A _SFR_IO8(0x28)
#define OCR00 0
#define OCR01 1
#define OCR02 2
#define OCR03 3
#define OCR04 4
#define OCR05 5
#define OCR06 6
#define OCR07 7

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

#define ACSR _SFR_IO8(0x30)
#define ACIS0 0
#define ACIS1 1
#define ACIC 2
#define ACIE 3
#define ACI 4
#define ACO 5
#define ACIRS 6
#define ACD 7

#define DWDR _SFR_IO8(0x31)
#define DWDR0 0
#define DWDR1 1
#define DWDR2 2
#define DWDR3 3
#define DWDR4 4
#define DWDR5 5
#define DWDR6 6
#define DWDR7 7

#define SMCR _SFR_IO8(0x33)
#define SE 0
#define SM0 1
#define SM1 2

#define MCUSR _SFR_IO8(0x34)
#define PORF 0
#define EXTRF 1
#define BORF 2
#define WDRF 3

#define MCUCR _SFR_IO8(0x35)
#define PUD 4
#define BODS 5
#define BODSE 6

#define SPMCSR _SFR_IO8(0x37)
#define SPMEN 0
#define PGERS 1
#define PGWRT 2
#define RFLB 3
#define CTPB 4
#define SIGRD 5
#define RWWSB 6

#define WDTCR _SFR_MEM8(0x60)
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
#define CLKPS2 2
#define CLKPS3 3
#define CLKPCE 7

#define CLKCSR _SFR_MEM8(0x62)
#define CLKC0 0
#define CLKC1 1
#define CLKC2 2
#define CLKC3 3
#define CLKRDY 4
#define CLKCCE 7

#define CLKSELR _SFR_MEM8(0x63)
#define CSEL0 0
#define CSEL1 1
#define CSEL2 2
#define CSEL3 3
#define CSUT0 4
#define CSUT1 5
#define COUT 6

#define PRR _SFR_MEM8(0x64)
#define PRADC 0
#define PRUSI 1
#define PRTIM0 2
#define PRTIM1 3
#define PRSPI 4
#define PRLIN 5

#define OSCCAL _SFR_MEM8(0x66)
#define CAL0 0
#define CAL1 1
#define CAL2 2
#define CAL3 3
#define CAL4 4
#define CAL5 5
#define CAL6 6
#define CAL7 7

#define PCICR _SFR_MEM8(0x68)
#define PCIE0 0
#define PCIE1 1

#define EICRA _SFR_MEM8(0x69)
#define ISC00 0
#define ISC01 1
#define ISC10 2
#define ISC11 3

#define PCMSK0 _SFR_MEM8(0x6B)
#define PCINT0 0
#define PCINT1 1
#define PCINT2 2
#define PCINT3 3
#define PCINT4 4
#define PCINT5 5
#define PCINT6 6
#define PCINT7 7

#define PCMSK1 _SFR_MEM8(0x6C)
#define PCINT8 0
#define PCINT9 1
#define PCINT10 2
#define PCINT11 3
#define PCINT12 4
#define PCINT13 5
#define PCINT14 6
#define PCINT15 7

#define TIMSK0 _SFR_MEM8(0x6E)
#define TOIE0 0
#define OCIE0A 1

#define TIMSK1 _SFR_MEM8(0x6F)
#define TOIE1 0
#define OCIE1A 1
#define OCIE1B 2
#define ICIE1 5

#define AMISCR _SFR_MEM8(0x77)
#define ISRCEN 0
#define XREFEN 1
#define AREFEN 2

#ifndef __ASSEMBLER__
#define ADC     _SFR_MEM16(0x78)
#endif
#define ADCW    _SFR_MEM16(0x78)

#define ADCL _SFR_MEM8(0x78)
#define ADCL0 0
#define ADCL1 1
#define ADCL2 2
#define ADCL3 3
#define ADCL4 4
#define ADCL5 5
#define ADCL6 6
#define ADCL7 7

#define ADCH _SFR_MEM8(0x79)
#define ADCH0 0
#define ADCH1 1
#define ADCH2 2
#define ADCH3 3
#define ADCH4 4
#define ADCH5 5
#define ADCH6 6
#define ADCH7 7

#define ADCSRA _SFR_MEM8(0x7A)
#define ADPS0 0
#define ADPS1 1
#define ADPS2 2
#define ADIE 3
#define ADIF 4
#define ADATE 5
#define ADSC 6
#define ADEN 7

#define ADCSRB _SFR_MEM8(0x7B)
#define ADTS0 0
#define ADTS1 1
#define ADTS2 2
#define ACIR0 4
#define ACIR1 5
#define ACME 6
#define BIN 7

#define ADMUX _SFR_MEM8(0x7C)
#define MUX0 0
#define MUX1 1
#define MUX2 2
#define MUX3 3
#define MUX4 4
#define ADLAR 5
#define REFS0 6
#define REFS1 7

#define DIDR0 _SFR_MEM8(0x7E)
#define ADC0D 0
#define ADC1D 1
#define ADC2D 2
#define ADC3D 3
#define ADC4D 4
#define ADC5D 5
#define ADC6D 6
#define ADC7D 7

#define DIDR1 _SFR_MEM8(0x7F)
#define ADC8D 0
#define ADC9D 1
#define ADC10D 2

#define TCCR1A _SFR_MEM8(0x80)
#define WGM10 0
#define WGM11 1
#define COM1B0 4
#define COM1B1 5
#define COM1A0 6
#define COM1A1 7

#define TCCR1B _SFR_MEM8(0x81)
#define CS10 0
#define CS11 1
#define CS12 2
#define WGM12 3
#define WGM13 4
#define ICES1 6
#define ICNC1 7

#define TCCR1C _SFR_MEM8(0x82)
#define FOC1B 6
#define FOC1A 7

#define TCCR1D _SFR_MEM8(0x83)
#define OC1AU 0
#define OC1AV 1
#define OC1AW 2
#define OC1AX 3
#define OC1BU 4
#define OC1BV 5
#define OC1BW 6
#define OC1BX 7

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

#define ICR1 _SFR_MEM8(0x86)

#define ICR1L _SFR_MEM8(0x86)
#define ICR1L0 0
#define ICR1L1 1
#define ICR1L2 2
#define ICR1L3 3
#define ICR1L4 4
#define ICR1L5 5
#define ICR1L6 6
#define ICR1L7 7

#define ICR1H _SFR_MEM8(0x87)
#define ICR1H0 0
#define ICR1H1 1
#define ICR1H2 2
#define ICR1H3 3
#define ICR1H4 4
#define ICR1H5 5
#define ICR1H6 6
#define ICR1H7 7

#define OCR1A _SFR_MEM16(0x88)

#define OCR1AL _SFR_MEM8(0x88)
#define OCR1AL0 0
#define OCR1AL1 1
#define OCR1AL2 2
#define OCR1AL3 3
#define OCR1AL4 4
#define OCR1AL5 5
#define OCR1AL6 6
#define OCR1AL7 7

#define OCR1AH _SFR_MEM8(0x89)
#define OCR1AH0 0
#define OCR1AH1 1
#define OCR1AH2 2
#define OCR1AH3 3
#define OCR1AH4 4
#define OCR1AH5 5
#define OCR1AH6 6
#define OCR1AH7 7

#define OCR1B _SFR_MEM16(0x8A)

#define OCR1BL _SFR_MEM8(0x8A)
#define OCR1AL0 0
#define OCR1AL1 1
#define OCR1AL2 2
#define OCR1AL3 3
#define OCR1AL4 4
#define OCR1AL5 5
#define OCR1AL6 6
#define OCR1AL7 7

#define OCR1BH _SFR_MEM8(0x8B)
#define OCR1AH0 0
#define OCR1AH1 1
#define OCR1AH2 2
#define OCR1AH3 3
#define OCR1AH4 4
#define OCR1AH5 5
#define OCR1AH6 6
#define OCR1AH7 7

#define ASSR _SFR_MEM8(0xB6)
#define TCR0BUB 0
#define TCR0AUB 1
#define OCR0AUB 3
#define TCN0UB 4
#define AS0 5
#define EXCLK 6

#define USICR _SFR_MEM8(0xB8)
#define USITC 0
#define USICLK 1
#define USICS0 2
#define USICS1 3
#define USIWM0 4
#define USIWM1 5
#define USIOIE 6
#define USISIE 7

#define USISR _SFR_MEM8(0xB9)
#define USICNT0 0
#define USICNT1 1
#define USICNT2 2
#define USICNT3 3
#define USIDC 4
#define USIPF 5
#define USIOIF 6
#define USISIF 7

#define USIDR _SFR_MEM8(0xBA)
#define USIDR0 0
#define USIDR1 1
#define USIDR2 2
#define USIDR3 3
#define USIDR4 4
#define USIDR5 5
#define USIDR6 6
#define USIDR7 7

#define USIBR _SFR_MEM8(0xBB)
#define USIBR0 0
#define USIBR1 1
#define USIBR2 2
#define USIBR3 3
#define USIBR4 4
#define USIBR5 5
#define USIBR6 6
#define USIBR7 7

#define USIPP _SFR_MEM8(0xBC)
#define USIPOS 0

#define LINCR _SFR_MEM8(0xC8)
#define LCMD0 0
#define LCMD1 1
#define LCMD2 2
#define LENA 3
#define LCONF0 4
#define LCONF1 5
#define LIN13 6
#define LSWRES 7

#define LINSIR _SFR_MEM8(0xC9)
#define LRXOK 0
#define LTXOK 1
#define LIDOK 2
#define LERR 3
#define LBUSY 4
#define LIDST0 5
#define LIDST1 6
#define LIDST2 7

#define LINENIR _SFR_MEM8(0xCA)
#define LENRXOK 0
#define LENTXOK 1
#define LENIDOK 2
#define LENERR 3

#define LINERR _SFR_MEM8(0xCB)
#define LBERR 0
#define LCERR 1
#define LPERR 2
#define LSERR 3
#define LFERR 4
#define LOVERR 5
#define LTOERR 6
#define LABORT 7

#define LINBTR _SFR_MEM8(0xCC)
#define LBT0 0
#define LBT1 1
#define LBT2 2
#define LBT3 3
#define LBT4 4
#define LBT5 5
#define LDISR 7

#define LINBRR _SFR_MEM16(0xCD)

#define LINBRRL _SFR_MEM8(0xCD)
#define LDIV0 0
#define LDIV1 1
#define LDIV2 2
#define LDIV3 3
#define LDIV4 4
#define LDIV5 5
#define LDIV6 6
#define LDIV7 7

#define LINBRRH _SFR_MEM8(0xCE)
#define LDIV8 0
#define LDIV9 1
#define LDIV10 2
#define LDIV11 3

#define LINDLR _SFR_MEM8(0xCF)
#define LRXDL0 0
#define LRXDL1 1
#define LRXDL2 2
#define LRXDL3 3
#define LTXDL0 4
#define LTXDL1 5
#define LTXDL2 6
#define LTXDL3 7

#define LINIDR _SFR_MEM8(0xD0)
#define LID0 0
#define LID1 1
#define LID2 2
#define LID3 3
#define LID4 4
#define LID5 5
#define LP0 6
#define LP1 7

#define LINSEL _SFR_MEM8(0xD1)
#define LINDX0 0
#define LINDX1 1
#define LINDX2 2
#define LAINC 3

#define LINDAT _SFR_MEM8(0xD2)
#define LDATA0 0
#define LDATA1 1
#define LDATA2 2
#define LDATA3 3
#define LDATA4 4
#define LDATA5 5
#define LDATA6 6
#define LDATA7 7



/* Interrupt Vectors */
/* Interrupt Vector 0 is the reset vector. */
#define INT0_vect           _VECTOR(1)  /* External Interrupt Request 0 */
#define INT1_vect           _VECTOR(2)  /* External Interrupt Request 1 */
#define PCINT0_vect         _VECTOR(3)  /* Pin Change Interrupt Request 0 */
#define PCINT1_vect         _VECTOR(4)  /* Pin Change Interrupt Request 1 */
#define WDT_vect            _VECTOR(5)  /* Watchdog Time-Out Interrupt */
#define TIMER1_CAPT_vect    _VECTOR(6)  /* Timer/Counter1 Capture Event */
#define TIMER1_COMPA_vect   _VECTOR(7)  /* Timer/Counter1 Compare Match 1A */
#define TIMER1_COMPB_vect   _VECTOR(8)  /* Timer/Counter1 Compare Match 1B */
#define TIMER1_OVF_vect     _VECTOR(9)  /* Timer/Counter1 Overflow */
#define TIMER0_COMPA_vect   _VECTOR(10)  /* Timer/Counter0 Compare Match 0A */
#define TIMER0_OVF_vect     _VECTOR(11)  /* Timer/Counter0 Overflow */
#define LIN_TC_vect         _VECTOR(12)  /* LIN Transfer Complete */
#define LIN_ERR_vect        _VECTOR(13)  /* LIN Error */
#define SPI_STC_vect        _VECTOR(14)  /* SPI Serial Transfer Complete */
#define ADC_vect            _VECTOR(15)  /* ADC Conversion Complete */
#define EE_RDY_vect         _VECTOR(16)  /* EEPROM Ready */
#define ANA_COMP_vect       _VECTOR(17)  /* Analog Comparator */
#define USI_START_vect      _VECTOR(18)  /* USI Start */
#define USI_OVF_vect        _VECTOR(19)  /* USI Overflow */

#define _VECTORS_SIZE (20 * 4)



/* Constants */
#define SPM_PAGESIZE (64)
#define RAMSTART     (0x100)
#define RAMSIZE      (0x1FF)
#define RAMEND       (RAMSTART + RAMSIZE - 1)  /* Last On-Chip SRAM Location */
#define XRAMSIZE     (0)
#define XRAMEND      (RAMEND + XRAMSIZE)
#define E2END        (0x1FF)
#define E2PAGESIZE   (4) 
#define FLASHEND     (0x3FFF)



/* Fuses */
#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0 (unsigned char)~_BV(0)  /* Select Clock source */
#define FUSE_CKSEL1 (unsigned char)~_BV(1)  /* Select Clock source */
#define FUSE_CKSEL2 (unsigned char)~_BV(2)  /* Select Clock source */
#define FUSE_CKSEL3 (unsigned char)~_BV(3)  /* Select Clock source */
#define FUSE_SUT0   (unsigned char)~_BV(4)  /* Select start-up time */
#define FUSE_SUT1   (unsigned char)~_BV(5)  /* Select start-up time */
#define FUSE_CKOUT  (unsigned char)~_BV(6)  /* Clock Output Enable */
#define FUSE_CKDIV8 (unsigned char)~_BV(7)  /* Divide clock by 8 */
#define LFUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0 & FUSE_CKDIV8)

/* High Fuse Byte */
#define FUSE_BODLEVEL0 (unsigned char)~_BV(0)  /* Brown-out Detector trigger level */
#define FUSE_BODLEVEL1 (unsigned char)~_BV(1)  /* Brown-out Detector trigger level */
#define FUSE_BODLEVEL2 (unsigned char)~_BV(2)  /* Brown-out Detector trigger level */
#define FUSE_EESAVE    (unsigned char)~_BV(3)  /* EEPROM memory is preserved through the Chip Erase */
#define FUSE_WDTON     (unsigned char)~_BV(4)  /* Watchdog Timer always ON */
#define FUSE_SPIEN     (unsigned char)~_BV(5)  /* Enable Serial Program and Data Downloading */
#define FUSE_DWEN      (unsigned char)~_BV(6)  /* DebugWIRE Enable */
#define FUSE_RSTDISBL  (unsigned char)~_BV(7)  /* External Reset disable */
#define HFUSE_DEFAULT (FUSE_SPIEN)

/* Extended Fuse Byte */
#define FUSE_SELFPRGEN (unsigned char)~_BV(0)  /* Self-Programming Enable */
#define EFUSE_DEFAULT (0xFF)



/* Lock Bits */
#define __LOCK_BITS_EXIST



/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x94
#define SIGNATURE_2 0x87



#endif  /* _AVR_IOTN167_H_ */
