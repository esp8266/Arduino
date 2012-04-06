/* Copyright (c) 2006, Anatoly Sokolov 
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

/* $Id: iousbxx6_7.h,v 1.4.2.1 2008/05/05 01:58:21 arcanum Exp $ */

/* iousbxx6_7.h - definitions for AT90USB646, AT90USB647, AT90USB1286 
   and AT90USB1287 */

#ifndef _AVR_IOUSBXX6_7_H_
#define _AVR_IOUSBXX6_7_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iousbxx6_7.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

#if defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#  define __AT90USBxx6__ 1
#elif defined(__AVR_AT90USB647__) || defined(__AVR_AT90USB1287__)
#  define __AT90USBxx7__ 1
#endif

/* Registers and associated bit numbers */

#define PINA    _SFR_IO8(0X00)
#define PINA7   7
#define PINA6   6
#define PINA5   5
#define PINA4   4
#define PINA3   3
#define PINA2   2
#define PINA1   1
#define PINA0   0

#define DDRA    _SFR_IO8(0X01)
#define DDA7    7
#define DDA6    6
#define DDA5    5
#define DDA4    4
#define DDA3    3
#define DDA2    2
#define DDA1    1
#define DDA0    0

#define PORTA   _SFR_IO8(0X02)
#define PA7  7
#define PA6  6
#define PA5  5
#define PA4  4
#define PA3  3
#define PA2  2
#define PA1  1
#define PA0  0

#define PINB    _SFR_IO8(0X03)
#define PINB7   7
#define PINB6   6
#define PINB5   5
#define PINB4   4
#define PINB3   3
#define PINB2   2
#define PINB1   1
#define PINB0   0

#define DDRB    _SFR_IO8(0x04)
#define DDB7    7
#define DDB6    6
#define DDB5    5
#define DDB4    4
#define DDB3    3
#define DDB2    2
#define DDB1    1
#define DDB0    0

#define PORTB   _SFR_IO8(0x05)
#define PB7  7
#define PB6  6
#define PB5  5
#define PB4  4
#define PB3  3
#define PB2  2
#define PB1  1
#define PB0  0

#define PINC    _SFR_IO8(0x06)
#define PINC7   7
#define PINC6   6
#define PINC5   5
#define PINC4   4
#define PINC3   3
#define PINC2   2
#define PINC1   1
#define PINC0   0

#define DDRC    _SFR_IO8(0x07)
#define DDC7    7
#define DDC6    6
#define DDC5    5
#define DDC4    4
#define DDC3    3
#define DDC2    2
#define DDC1    1
#define DDC0    0

#define PORTC   _SFR_IO8(0x08)
#define PC7  7
#define PC6  6
#define PC5  5
#define PC4  4
#define PC3  3
#define PC2  2
#define PC1  1
#define PC0  0

#define PIND    _SFR_IO8(0x09)
#define PIND7   7
#define PIND6   6
#define PIND5   5
#define PIND4   4
#define PIND3   3
#define PIND2   2
#define PIND1   1
#define PIND0   0

#define DDRD    _SFR_IO8(0x0A)
#define DDD7    7
#define DDD6    6
#define DDD5    5
#define DDD4    4
#define DDD3    3
#define DDD2    2
#define DDD1    1
#define DDD0    0

#define PORTD   _SFR_IO8(0x0B)
#define PD7  7
#define PD6  6
#define PD5  5
#define PD4  4
#define PD3  3
#define PD2  2
#define PD1  1
#define PD0  0

#define PINE    _SFR_IO8(0x0C)
#define PINE7   7
#define PINE6   6
#define PINE5   5
#define PINE4   4
#define PINE3   3
#define PINE2   2
#define PINE1   1
#define PINE0   0

#define DDRE    _SFR_IO8(0x0D)
#define DDE7    7
#define DDE6    6
#define DDE5    5
#define DDE4    4
#define DDE3    3
#define DDE2    2
#define DDE1    1
#define DDE0    0

#define PORTE   _SFR_IO8(0x0E)
#define PE7  7
#define PE6  6
#define PE5  5
#define PE4  4
#define PE3  3
#define PE2  2
#define PE1  1
#define PE0  0

#define PINF    _SFR_IO8(0x0F)
#define PINF7   7
#define PINF6   6
#define PINF5   5
#define PINF4   4
#define PINF3   3
#define PINF2   2
#define PINF1   1
#define PINF0   0

#define DDRF    _SFR_IO8(0x10)
#define DDF7    7
#define DDF6    6
#define DDF5    5
#define DDF4    4
#define DDF3    3
#define DDF2    2
#define DDF1    1
#define DDF0    0

#define PORTF   _SFR_IO8(0x11)
#define PF7  7
#define PF6  6
#define PF5  5
#define PF4  4
#define PF3  3
#define PF2  2
#define PF1  1
#define PF0  0

/* Reserved [0x12..0x14] */

#define TIFR0   _SFR_IO8(0x15)
#define OCF0B   2
#define OCF0A   1
#define TOV0    0

#define TIFR1   _SFR_IO8(0x16)
#define ICF1    5
#define OCF1C   3
#define OCF1B   2
#define OCF1A   1
#define TOV1    0

#define TIFR2   _SFR_IO8(0x17)
#define OCF2B   2
#define OCF2A   1
#define TOV2    0

#define TIFR3   _SFR_IO8(0x18)
#define ICF3    5
#define OCF3C   3
#define OCF3B   2
#define OCF3A   1
#define TOV3    0

/* Reserved [0x19..0x1A] */

#define PCIFR   _SFR_IO8(0x1B)
#define PCIF0   0

#define EIFR    _SFR_IO8(0x1C)
#define INTF7   7
#define INTF6   6
#define INTF5   5
#define INTF4   4
#define INTF3   3
#define INTF2   2
#define INTF1   1
#define INTF0   0

#define EIMSK   _SFR_IO8(0x1D)
#define INT7    7
#define INT6    6
#define INT5    5
#define INT4    4
#define INT3    3
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

#define EEAR    _SFR_IO16(0x21)
#define EEARL   _SFR_IO8(0x21)
#define EEARH   _SFR_IO8(0x22)

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

#define GTCCR   _SFR_IO8(0x23)
#define TSM     7
#define PSRASY  1
#define PSRSYNC 0

#define TCCR0A  _SFR_IO8(0x24)
#define COM0A1  7
#define COM0A0  6
#define COM0B1  5
#define COM0B0  4
#define WGM01   1
#define WGM00   0

#define TCCR0B  _SFR_IO8(0x25)
#define FOC0A   7
#define FOC0B   6
#define WGM02   3
#define CS02    2
#define CS01    1
#define CS00    0

#define TCNT0   _SFR_IO8(0X26)

#define OCR0A   _SFR_IO8(0x27)

#define OCR0B   _SFR_IO8(0X28)

#define PLLCSR  _SFR_IO8(0x29)
#define PLLP2   4
#define PLLP1   3
#define PLLP0   2
#define PLLE    1
#define PLOCK   0

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

/* Reserved [0x2F] */

#define ACSR    _SFR_IO8(0x30)
#define ACD     7
#define ACBG    6
#define ACO     5
#define ACI     4
#define ACIE    3
#define ACIC    2
#define ACIS1   1
#define ACIS0   0

#define MONDR   _SFR_IO8(0x31)
#define OCDR    _SFR_IO8(0x31)
#define IDRD    7
#define OCDR7   7
#define OCDR6   6
#define OCDR5   5
#define OCDR4   4
#define OCDR3   3
#define OCDR2   2
#define OCDR1   1
#define OCDR0   0

/* Reserved [0x32] */

#define SMCR    _SFR_IO8(0x33)
#define SM2     3
#define SM1     2
#define SM0     1
#define SE      0

#define MCUSR   _SFR_IO8(0x34)
#define JTRF    4
#define WDRF    3
#define BORF    2
#define EXTRF   1
#define PORF    0

#define MCUCR   _SFR_IO8(0x35)
#define JTD     7
#define PUD     4
#define IVSEL   1
#define IVCE    0

/* Reserved [0x36] */

#define SPMCSR  _SFR_IO8(0x37)
#define SPMIE   7
#define RWWSB   6
#define SIGRD   5
#define RWWSRE  4
#define BLBSET  3
#define PGWRT   2
#define PGERS   1
#define SPMEN   0

/* Reserved [0x38..0x3A] */

#if defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__)
#define RAMPZ   _SFR_IO8(0x3B)
#endif

/* Reserved [0x3C] */

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
#define CLKPS3  3
#define CLKPS2  2
#define CLKPS1  1
#define CLKPS0  0

/* Reserved [0x62..0x63] */

#define PRR0    _SFR_MEM8(0x64)
#define PRTWI   7
#define PRTIM2  6
#define PRTIM0  5
#define PRTIM1  3
#define PRSPI   2
#define PRADC   0

#define PRR1    _SFR_MEM8(0x65)
#define PRUSB   7
#define PRTIM3  3
#define PRUSART1 0

#define OSCCAL  _SFR_MEM8(0x66)

/* Reserved [0x67] */

#define PCICR   _SFR_MEM8(0x68)
#define PCIE0   0

#define EICRA   _SFR_MEM8(0x69)
#define ISC31   7
#define ISC30   6
#define ISC21   5
#define ISC20   4
#define ISC11   3
#define ISC10   2
#define ISC01   1
#define ISC00   0

#define EICRB   _SFR_MEM8(0x6A)
#define ISC71   7
#define ISC70   6
#define ISC61   5
#define ISC60   4
#define ISC51   3
#define ISC50   2
#define ISC41   1
#define ISC40   0

#define PCMSK0  _SFR_MEM8(0x6B)
#define PCINT7  7
#define PCINT6  6
#define PCINT5  5
#define PCINT4  4
#define PCINT3  3
#define PCINT2  2
#define PCINT1  1
#define PCINT0  0

/* Reserved [0x6C..0x6D] */

#define TIMSK0  _SFR_MEM8(0x6E)
#define OCIE0B  2
#define OCIE0A  1
#define TOIE0   0

#define TIMSK1  _SFR_MEM8(0x6F)
#define ICIE1   5
#define OCIE1C  3
#define OCIE1B  2
#define OCIE1A  1
#define TOIE1   0

#define TIMSK2  _SFR_MEM8(0x70)
#define OCIE2B  2
#define OCIE2A  1
#define TOIE2   0

#define TIMSK3  _SFR_MEM8(0x71)
#define ICIE3   5
#define OCIE3C  3
#define OCIE3B  2
#define OCIE3A  1
#define TOIE3   0

/* Reserved [0x72..0x73] */

#define XMCRA   _SFR_MEM8(0x74)
#define SRE     7
#define SRL2    6
#define SRL1    5
#define SRL0    4
#define SRW11   3
#define SRW10   2
#define SRW01   1
#define SRW00   0

#define XMCRB   _SFR_MEM8(0x75)
#define XMBK    7
#define XMM2    2
#define XMM1    1
#define XMM0    0

/* Reserved [0x76..0x77] */

/* RegDef:  ADC Data Register */
#ifndef __ASSEMBLER__
#define ADC    _SFR_MEM16(0x78)
#endif
#define ADCW   _SFR_MEM16(0x78)
#define ADCL   _SFR_MEM8(0x78)
#define ADCH   _SFR_MEM8(0x79)

#define ADCSRA  _SFR_MEM8(0x7A)
#define ADEN    7
#define ADSC    6
#define ADATE   5
#define ADIF    4
#define ADIE    3
#define ADPS2   2
#define ADPS1   1
#define ADPS0   0

#define ADCSRB  _SFR_MEM8(0x7B)
#define ACME    6
#define ADTS2   2
#define ADTS1   1
#define ADTS0   0

#define ADMUX   _SFR_MEM8(0x7C)
#define REFS1   7
#define REFS0   6
#define ADLAR   5
#define MUX4    4
#define MUX3    3
#define MUX2    2
#define MUX1    1
#define MUX0    0

/* Reserved [0x7D] */

#define DIDR0   _SFR_MEM8(0x7E)
#define ADC7D   7
#define ADC6D   6
#define ADC5D   5
#define ADC4D   4
#define ADC3D   3
#define ADC2D   2
#define ADC1D   1
#define ADC0D   0

#define DIDR1   _SFR_MEM8(0x7F)
#define AIN1D   1
#define AIN0D   0

#define TCCR1A  _SFR_MEM8(0x80)
#define COM1A1  7
#define COM1A0  6
#define COM1B1  5
#define COM1B0  4
#define COM1C1  3
#define COM1C0  2
#define WGM11   1
#define WGM10   0

#define TCCR1B  _SFR_MEM8(0x81)
#define ICNC1   7
#define ICES1   6
#define WGM13   4
#define WGM12   3
#define CS12    2
#define CS11    1
#define CS10    0

#define TCCR1C  _SFR_MEM8(0x82)
#define FOC1A   7
#define FOC1B   6
#define FOC1C   5

/* Reserved [0x83] */

/* Combine TCNT1L and TCNT1H */
#define TCNT1   _SFR_MEM16(0x84)

#define TCNT1L  _SFR_MEM8(0x84)
#define TCNT1H  _SFR_MEM8(0x85)

/* Combine ICR1L and ICR1H */
#define ICR1    _SFR_MEM16(0x86)

#define ICR1L   _SFR_MEM8(0x86)
#define ICR1H   _SFR_MEM8(0x87)

/* Combine OCR1AL and OCR1AH */
#define OCR1A   _SFR_MEM16(0x88)

#define OCR1AL  _SFR_MEM8(0x88)
#define OCR1AH  _SFR_MEM8(0x89)

/* Combine OCR1BL and OCR1BH */
#define OCR1B   _SFR_MEM16(0x8A)

#define OCR1BL  _SFR_MEM8(0x8A)
#define OCR1BH  _SFR_MEM8(0x8B)

/* Combine OCR1CL and OCR1CH */
#define OCR1C   _SFR_MEM16(0x8C)

#define OCR1CL  _SFR_MEM8(0x8C)
#define OCR1CH  _SFR_MEM8(0x8D)

/* Reserved [0x8E..0x8F] */

#define TCCR3A  _SFR_MEM8(0x90)
#define COM3A1  7
#define COM3A0  6
#define COM3B1  5
#define COM3B0  4
#define COM3C1  3
#define COM3C0  2
#define WGM31   1
#define WGM30   0

#define TCCR3B  _SFR_MEM8(0x91)
#define ICNC3   7
#define ICES3   6
#define WGM33   4
#define WGM32   3
#define CS32    2
#define CS31    1
#define CS30    0

#define TCCR3C  _SFR_MEM8(0x92)
#define FOC3A   7
#define FOC3B   6
#define FOC3C   5

/* Reserved [0x93] */

/* Combine TCNT3L and TCNT3H */
#define TCNT3   _SFR_MEM16(0x94)

#define TCNT3L  _SFR_MEM8(0x94)
#define TCNT3H  _SFR_MEM8(0x95)

/* Combine ICR3L and ICR3H */
#define ICR3    _SFR_MEM16(0x96)

#define ICR3L   _SFR_MEM8(0x96)
#define ICR3H   _SFR_MEM8(0x97)

/* Combine OCR3AL and OCR3AH */
#define OCR3A   _SFR_MEM16(0x98)

#define OCR3AL  _SFR_MEM8(0x98)
#define OCR3AH  _SFR_MEM8(0x99)

/* Combine OCR3BL and OCR3BH */
#define OCR3B   _SFR_MEM16(0x9A)

#define OCR3BL  _SFR_MEM8(0x9A)
#define OCR3BH  _SFR_MEM8(0x9B)

/* Combine OCR3CL and OCR3CH */
#define OCR3C   _SFR_MEM16(0x9C)

#define OCR3CL  _SFR_MEM8(0x9C)
#define OCR3CH  _SFR_MEM8(0x9D)

#if defined(__AT90USBxx7__)

#define UHCON   _SFR_MEM8(0x9E)
#define RESUME  2
#define RESET   1
#define SOFEN   0

#define UHINT   _SFR_MEM8(0x9F)
#define HWUPI   6
#define HSOFI   5
#define RXRSMI  4
#define RSMEDI  3
#define RSTI    2
#define DDISCI  1
#define DCONNI  0

#define UHIEN   _SFR_MEM8(0xA0)
#define HWUPE   6
#define HSOFE   5
#define RXRSME  4
#define RSMEDE  3
#define RSTE    2
#define DDISCE  1
#define DCONNE  0

#define UHADDR  _SFR_MEM8(0xA1)

/* Combine UHFNUML and UHFNUMH */
#define UHFNUM  _SFR_MEM16(0xA2)

#define UHFNUML _SFR_MEM8(0xA2)
#define UHFNUMH _SFR_MEM8(0xA3)

#define UHFLEN  _SFR_MEM8(0xA4)

#define UPINRQX _SFR_MEM8(0xA5)

#define UPINTX  _SFR_MEM8(0xA6)
#define FIFOCON 7
#define NAKEDI  6
#define RWAL    5
#define PERRI   4
#define TXSTPI  3
#define TXOUTI  2
#define RXSTALLI 1
#define RXINI   0

#define UPNUM   _SFR_MEM8(0xA7)

#define UPRST   _SFR_MEM8(0xA8)
#define PRST6   6
#define PRST5   5
#define PRST4   4
#define PRST3   3
#define PRST2   2
#define PRST1   1
#define PRST0   0

#define UPCONX  _SFR_MEM8(0xA9)
#define PFREEZE 6
#define INMODE  5
/* #define AUTOSW  4 */ /* Reserved */
#define RSTDT   3
#define PEN     0

#define UPCFG0X _SFR_MEM8(0XAA)
#define PTYPE1  7
#define PTYPE0  6
#define PTOKEN1 5
#define PTOKEN0 4
#define PEPNUM3 3
#define PEPNUM2 2
#define PEPNUM1 1
#define PEPNUM0 0

#define UPCFG1X _SFR_MEM8(0XAB)
#define PSIZE2  6
#define PSIZE1  5
#define PSIZE0  4
#define PBK1    3
#define PBK0    2
#define ALLOC   1

#define UPSTAX  _SFR_MEM8(0XAC)
#define CFGOK   7
#define OVERFI  6
#define UNDERFI 5
#define DTSEQ1  3
#define DTSEQ0  2
#define NBUSYBK1 1
#define NBUSYBK0 0

#define UPCFG2X _SFR_MEM8(0XAD)

#define UPIENX  _SFR_MEM8(0XAE)
#define FLERRE  7
#define NAKEDE  6
#define PERRE   4
#define TXSTPE  3
#define TXOUTE  2
#define RXSTALLE 1
#define RXINE   0

#define UPDATX  _SFR_MEM8(0XAF)

#endif /* __AT90USBxx7__ */

#define TCCR2A  _SFR_MEM8(0xB0)
#define COM2A1  7
#define COM2A0  6
#define COM2B1  5
#define COM2B0  4
#define WGM21   1
#define WGM20   0

#define TCCR2B  _SFR_MEM8(0xB1)
#define FOC2A   7
#define FOC2B   6
#define WGM22   3
#define CS22    2
#define CS21    1
#define CS20    0

#define TCNT2   _SFR_MEM8(0xB2)

#define OCR2A   _SFR_MEM8(0xB3)

#define OCR2B   _SFR_MEM8(0xB4)

/* Reserved [0xB5] */

#define ASSR    _SFR_MEM8(0xB6)
#define EXCLK   6
#define AS2     5
#define TCN2UB  4
#define OCR2AUB 3
#define OCR2BUB 2
#define TCR2AUB 1
#define TCR2BUB 0

/* Reserved [0xB7] */

#define TWBR    _SFR_MEM8(0xB8)

#define TWSR    _SFR_MEM8(0xB9)
#define TWS7    7
#define TWS6    6
#define TWS5    5
#define TWS4    4
#define TWS3    3
#define TWPS1   1
#define TWPS0   0

#define TWAR    _SFR_MEM8(0xBA)
#define TWA6    7
#define TWA5    6
#define TWA4    5
#define TWA3    4
#define TWA2    3
#define TWA1    2
#define TWA0    1
#define TWGCE   0

#define TWDR    _SFR_MEM8(0xBB)

#define TWCR    _SFR_MEM8(0xBC)
#define TWINT   7
#define TWEA    6
#define TWSTA   5
#define TWSTO   4
#define TWWC    3
#define TWEN    2
#define TWIE    0

#define TWAMR   _SFR_MEM8(0xBD)
#define TWAM6   7
#define TWAM5   6
#define TWAM4   5
#define TWAM3   4
#define TWAM2   3
#define TWAM1   2
#define TWAM0   1

/* Reserved [0xBE..0xC7] */

#define UCSR1A  _SFR_MEM8(0xC8)
#define RXC1    7
#define TXC1    6
#define UDRE1   5
#define FE1     4
#define DOR1    3
#define UPE1    2
#define U2X1    1
#define MPCM1   0

#define UCSR1B  _SFR_MEM8(0XC9)
#define RXCIE1  7
#define TXCIE1  6
#define UDRIE1  5
#define RXEN1   4
#define TXEN1   3
#define UCSZ12  2
#define RXB81   1
#define TXB81   0

#define UCSR1C  _SFR_MEM8(0xCA)
#define UMSEL11 7
#define UMSEL10 6
#define UPM11   5
#define UPM10   4
#define USBS1   3
#define UCSZ11  2
#define UCSZ10  1
#define UCPOL1  0

/* Reserved [0xCB] */

/* Combine UBRR1L and UBRR1H */
#define UBRR1   _SFR_MEM16(0xCC)

#define UBRR1L  _SFR_MEM8(0xCC)
#define UBRR1H  _SFR_MEM8(0xCD)

#define UDR1    _SFR_MEM8(0XCE)

/* Reserved [0xCF..0xD6] */

#define UHWCON  _SFR_MEM8(0XD7)
#define UIMOD   7
#define UIDE    6
#define UVCONE  4
#define UVREGE  0

#define USBCON  _SFR_MEM8(0XD8)
#define USBE    7
#define HOST    6
#define FRZCLK  5
#define OTGPADE 4
#define IDTE    1
#define VBUSTE  0

#define USBSTA  _SFR_MEM8(0XD9)
#define SPEED   3
#define ID      1
#define VBUS    0

#define USBINT  _SFR_MEM8(0XDA)
#define IDTI    1
#define VBUSTI  0

/* Combine UDPADDL and UDPADDH */
#define UDPADD  _SFR_MEM16(0xDB)

#define UDPADDL _SFR_MEM8(0xDB)
#define UDPADDH _SFR_MEM8(0xDC)
#define DPACC   7

#if defined(__AT90USBxx7__)

#define OTGCON  _SFR_MEM8(0XDD)
#define HNPREQ  5
#define SRPREQ  4
#define SRPSEL  3
#define VBUSHWC 2
#define VBUSREQ 1
#define VBUSRQC 0

#define OTGIEN  _SFR_MEM8(0XDE)
#define STOE    5
#define HNPERRE 4
#define ROLEEXE 3
#define BCERRE  2
#define VBERRE  1
#define SRPE    0

#define OTGINT  _SFR_MEM8(0XDF)
#define STOI    5
#define HNPERRI 4
#define ROLEEXI 3
#define BCERRI  2
#define VBERRI  1
#define SRPI    0

#endif /* __AT90USBxx7__ */

#define UDCON   _SFR_MEM8(0XE0)
#define LSM     2
#define RMWKUP  1
#define DETACH  0

#define UDINT   _SFR_MEM8(0XE1)
#define UPRSMI  6
#define EORSMI  5
#define WAKEUPI 4
#define EORSTI  3
#define SOFI    2
/* #define MSOFI   1 */ /* Reserved */
#define SUSPI   0

#define UDIEN   _SFR_MEM8(0XE2)
#define UPRSME  6
#define EORSME  5
#define WAKEUPE 4
#define EORSTE  3
#define SOFE    2
/* #define MSOFE   1 */ /* Reserved */
#define SUSPE   0

#define UDADDR  _SFR_MEM8(0XE3)
#define ADDEN   7

/* Combine UDFNUML and UDFNUMH */
#define UDFNUM  _SFR_MEM16(0xE4)

#define UDFNUML _SFR_MEM8(0xE4)
#define UDFNUMH _SFR_MEM8(0xE5)

#define UDMFN   _SFR_MEM8(0XE6)
#define FNCERR  4

#define UDTST   _SFR_MEM8(0XE7)
#define OPMODE2 5
#define TSTPCKT 4
#define TSTK    3
#define TSTJ    2

#define UEINTX  _SFR_MEM8(0XE8)
#define FIFOCON 7
#define NAKINI  6
#define RWAL    5
#define NAKOUTI 4
#define RXSTPI  3
#define RXOUTI  2
#define STALLEDI 1
#define TXINI   0

#define UENUM   _SFR_MEM8(0XE9)

#define UERST   _SFR_MEM8(0XEA)
#define EPRST6  6
#define EPRST5  5
#define EPRST4  4
#define EPRST3  3
#define EPRST2  2
#define EPRST1  1
#define EPRST0  0

#define UECONX  _SFR_MEM8(0XEB)
#define STALLRQ 5
#define STALLRQC 4
#define RSTDT   3
#define EPEN    0

#define UECFG0X _SFR_MEM8(0XEC)
#define EPTYPE1 7
#define EPTYPE0 6
/* #define ISOSW   3 */ /* Reserved */
/* #define AUTOSW  2 */ /* Reserved */
/* #define NYETSDIS 1 */ /* Reserved */
#define EPDIR   0

#define UECFG1X  _SFR_MEM8(0XED)
#define EPSIZE2 6
#define EPSIZE1 5
#define EPSIZE0 4
#define EPBK1   3
#define EPBK0   2
#define ALLOC   1

#define UESTA0X _SFR_MEM8(0XEE)
#define CFGOK   7
#define OVERFI  6
#define UNDERFI 5
#define ZLPSEEN 4
#define DTSEQ1  3
#define DTSEQ0  2
#define NBUSYBK1 1
#define NBUSYBK0 0

#define UESTA1X _SFR_MEM8(0XEF)
#define CTRLDIR 2
#define CURRBK1 1
#define CURRBK0 0

#define UEIENX  _SFR_MEM8(0XF0)
#define FLERRE  7
#define NAKINE  6
#define NAKOUTE 4
#define RXSTPE  3
#define RXOUTE  2
#define STALLEDE 1
#define TXINE   0

#define UEDATX  _SFR_MEM8(0XF1)

/* Combine UEBCLX and UEBCHX */
#define UEBCX   _SFR_MEM16(0xF2)

#define UEBCLX  _SFR_MEM8(0xF2)
#define UEBCHX  _SFR_MEM8(0xF3)

#define UEINT   _SFR_MEM8(0XF4)
#define EPINT6  6
#define EPINT5  5
#define EPINT4  4
#define EPINT3  3
#define EPINT2  2
#define EPINT1  1
#define EPINT0  0

#if defined(__AT90USBxx7__)

#define UPERRX  _SFR_MEM8(0XF5)
#define COUNTER1 6
#define COUNTER0 5
#define CRC16    4
#define TIMEOUT  3
#define PID      2
#define DATAPID  1
#define DATATGL  0

/* Combine UPBCLX and UPBCHX */
#define UPBCX   _SFR_MEM16(0xF6)

#define UPBCLX  _SFR_MEM8(0xF6)
#define UPBCHX  _SFR_MEM8(0xF7)

#define UPINT   _SFR_MEM8(0XF8)
#define PINT6   6
#define PINT5   5
#define PINT4   4
#define PINT3   3
#define PINT2   2
#define PINT1   1
#define PINT0   0

#define OTGTCON _SFR_MEM8(0XF9)
#define PAGE1   6
#define PAGE0   5
#define VALUE1  1
#define VALUE0  0

#endif /* __AT90USBxx7__ */

/* Reserved [0xFA..0xFF] */

/* Interrupt vectors */

/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)

/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(2)

/* External Interrupt Request 2 */
#define INT2_vect			_VECTOR(3)

/* External Interrupt Request 3 */
#define INT3_vect			_VECTOR(4)

/* External Interrupt Request 4 */
#define INT4_vect			_VECTOR(5)

/* External Interrupt Request 5 */
#define INT5_vect			_VECTOR(6)

/* External Interrupt Request 6 */
#define INT6_vect			_VECTOR(7)

/* External Interrupt Request 7 */
#define INT7_vect			_VECTOR(8)

/* Pin Change Interrupt Request 0 */
#define PCINT0_vect			_VECTOR(9)

/* USB General Interrupt Request */
#define USB_GEN_vect			_VECTOR(10)

/* USB Endpoint/Pipe Interrupt Communication Request */
#define USB_COM_vect			_VECTOR(11)

/* Watchdog Time-out Interrupt */
#define WDT_vect			_VECTOR(12)

/* Timer/Counter2 Compare Match A */
#define TIMER2_COMPA_vect		_VECTOR(13)

/* Timer/Counter2 Compare Match B */
#define TIMER2_COMPB_vect		_VECTOR(14)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(15)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(16)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(17)

/* Timer/Counter1 Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(18)

/* Timer/Counter1 Compare Match C */
#define TIMER1_COMPC_vect		_VECTOR(19)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(20)

/* Timer/Counter0 Compare Match A */
#define TIMER0_COMPA_vect		_VECTOR(21)

/* Timer/Counter0 Compare Match B */
#define TIMER0_COMPB_vect		_VECTOR(22)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(23)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(24)

/* USART1, Rx Complete */
#define USART1_RX_vect			_VECTOR(25)

/* USART1 Data register Empty */
#define USART1_UDRE_vect		_VECTOR(26)

/* USART1, Tx Complete */
#define USART1_TX_vect			_VECTOR(27)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(28)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(29)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(30)

/* Timer/Counter3 Capture Event */
#define TIMER3_CAPT_vect		_VECTOR(31)

/* Timer/Counter3 Compare Match A */
#define TIMER3_COMPA_vect		_VECTOR(32)

/* Timer/Counter3 Compare Match B */
#define TIMER3_COMPB_vect		_VECTOR(33)

/* Timer/Counter3 Compare Match C */
#define TIMER3_COMPC_vect		_VECTOR(34)

/* Timer/Counter3 Overflow */
#define TIMER3_OVF_vect			_VECTOR(35)

/* 2-wire Serial Interface */
#define TWI_vect			_VECTOR(36)

/* Store Program Memory Read */
#define SPM_READY_vect			_VECTOR(37)

#define _VECTORS_SIZE 152

#if defined(__AT90USBxx6__)
# undef __AT90USBxx6__
#endif /* __AT90USBxx6__ */

#if defined(__AT90USBxx7__)
# undef __AT90USBxx7__
#endif /* __AT90USBxx7__ */

#endif  /* _AVR_IOUSBXX6_7_H_ */
