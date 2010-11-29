/* Copyright (c) 2004 Eric B. Weddington
   Copyright (c) 2005, 2006, 2007 Anatoly Sokolov 
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

/* avr/iom3290.h - definitions for ATmega3290 and ATmega3290P.  */

#ifndef _AVR_IOM3290_H_
#define _AVR_IOM3290_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom3290.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* Registers and associated bit numbers */

#define PINA    _SFR_IO8(0x00)
#define PINA7   7
#define PINA6   6
#define PINA5   5
#define PINA4   4
#define PINA3   3
#define PINA2   2
#define PINA1   1
#define PINA0   0

#define DDRA    _SFR_IO8(0x01)
#define DDA7    7
#define DDA6    6
#define DDA5    5
#define DDA4    4
#define DDA3    3
#define DDA2    2
#define DDA1    1
#define DDA0    0

#define PORTA   _SFR_IO8(0x02)
#define PA7     7
#define PA6     6
#define PA5     5
#define PA4     4
#define PA3     3
#define PA2     2
#define PA1     1
#define PA0     0

#define PINB    _SFR_IO8(0x03)
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
#define PB7     7
#define PB6     6
#define PB5     5
#define PB4     4
#define PB3     3
#define PB2     2
#define PB1     1
#define PB0     0

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
#define PC7     7
#define PC6     6
#define PC5     5
#define PC4     4
#define PC3     3
#define PC2     2
#define PC1     1
#define PC0     0

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
#define PD7     7
#define PD6     6
#define PD5     5
#define PD4     4
#define PD3     3
#define PD2     2
#define PD1     1
#define PD0     0

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
#define PE7     7
#define PE6     6
#define PE5     5
#define PE4     4
#define PE3     3
#define PE2     2
#define PE1     1
#define PE0     0

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
#define PF7     7
#define PF6     6
#define PF5     5
#define PF4     4
#define PF3     3
#define PF2     2
#define PF1     1
#define PF0     0

#define PING    _SFR_IO8(0x12)
#define PING5   5
#define PING4   4
#define PING3   3
#define PING2   2
#define PING1   1
#define PING0   0

#define DDRG    _SFR_IO8(0x13)
#define DDG4    4
#define DDG3    3
#define DDG2    2
#define DDG1    1
#define DDG0    0

#define PORTG   _SFR_IO8(0x14)
#define PG4     4
#define PG3     3
#define PG2     2
#define PG1     1
#define PG0     0

#define TIFR0   _SFR_IO8(0x15)
#define TOV0    0
#define OCF0A   1

#define TIFR1   _SFR_IO8(0x16)
#define TOV1    0
#define OCF1A   1
#define OCF1B   2
#define ICF1    5

#define TIFR2   _SFR_IO8(0x17)
#define TOV2    0
#define OCF2A   1

/* Reserved [0x18..0x1B] */

#define EIFR   _SFR_IO8(0x1C)
#define INTF0   0
#define PCIF0   4
#define PCIF1   5
#define PCIF2   6
#define PCIF3   7

#define EIMSK   _SFR_IO8(0x1D)
#define INT0    0
#define PCIE0   4
#define PCIE1   5
#define PCIE2   6
#define PCIE3   7

#define GPIOR0  _SFR_IO8(0x1E)

#define EECR   _SFR_IO8(0x1F)
#define EERIE   3
#define EEMWE   2
#define EEWE    1
#define EERE    0

#define EEDR   _SFR_IO8(0X20)

/* Combine EEARL and EEARH */
#define EEAR   _SFR_IO16(0x21)
#define EEARL  _SFR_IO8(0x21)
#define EEARH  _SFR_IO8(0X22)

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

#define GTCCR   _SFR_IO8(0x23)
#define PSR10   0
#define PSR2    1
#define TSM     7

#define TCCR0A  _SFR_IO8(0x24)
#define CS00    0
#define CS01    1
#define CS02    2
#define WGM01   3
#define COM0A0  4
#define COM0A1  5
#define WGM00   6
#define FOC0A   7

/* Reserved [0x25] */

#define TCNT0   _SFR_IO8(0X26)

#define OCR0A   _SFR_IO8(0X27)

/* Reserved [0x28..0x29] */

#define GPIOR1  _SFR_IO8(0x2A)

#define GPIOR2  _SFR_IO8(0x2B)

#define SPCR    _SFR_IO8(0x2C)
#define SPR0    0
#define SPR1    1
#define CPHA    2
#define CPOL    3
#define MSTR    4
#define DORD    5
#define SPE     6
#define SPIE    7

#define SPSR    _SFR_IO8(0x2D)
#define SPI2X   0
#define WCOL    6
#define SPIF    7

#define SPDR    _SFR_IO8(0X2E)

/* Reserved [0x2F] */

#define ACSR    _SFR_IO8(0x30)
#define ACIS0   0
#define ACIS1   1
#define ACIC    2
#define ACIE    3
#define ACI     4
#define ACO     5
#define ACBG    6
#define ACD     7

#define OCDR    _SFR_IO8(0x31)
#define OCDR0   0
#define OCDR1   1
#define OCDR2   2
#define OCDR3   3
#define OCDR4   4
#define OCDR5   5
#define OCDR6   6
#define OCDR7   7
#define IDRD    7

/* Reserved [0x32] */

#define SMCR    _SFR_IO8(0x33)
#define SE      0
#define SM0     1
#define SM1     2
#define SM2     3

#define MCUSR   _SFR_IO8(0x34)
#define PORF    0
#define EXTRF   1
#define BORF    2
#define WDRF    3
#define JTRF    4

#define MCUCR   _SFR_IO8(0X35)
#define IVCE    0
#define IVSEL   1
#define PUD     4
#if defined(__AVR_ATmega3290P__)
#define BODSE   5
#define BODS    6
#endif
#define JTD     7

/* Reserved [0x36] */

#define SPMCSR  _SFR_IO8(0x37)
#define SPMEN   0
#define PGERS   1
#define PGWRT   2
#define BLBSET  3
#define RWWSRE  4
#define RWWSB   6
#define SPMIE   7

/* Reserved [0x38..0x3C] */

/* SP [0x3D..0x3E] */
/* SREG [0x3F] */

#define WDTCR   _SFR_MEM8(0x60)
#define WDP0    0
#define WDP1    1
#define WDP2    2
#define WDE     3
#define WDCE    4

#define CLKPR   _SFR_MEM8(0x61)
#define CLKPS0  0
#define CLKPS1  1
#define CLKPS2  2
#define CLKPS3  3
#define CLKPCE  7

/* Reserved [0x62..0x63] */

#define PRR     _SFR_MEM8(0x64)
#define PRADC       0
#define PRUSART0    1
#define PRSPI       2
#define PRTIM1      3
#define PRLCD       4

/* Reserved [0x65] */

#define OSCCAL  _SFR_MEM8(0x66)

/* Reserved [0x67..0x68] */

#define EICRA   _SFR_MEM8(0x69)
#define ISC00   0
#define ISC01   1

/* Reserved [0x6A] */

#define PCMSK0  _SFR_MEM8(0x6B)
#define PCINT0  0
#define PCINT1  1
#define PCINT2  2
#define PCINT3  3
#define PCINT4  4
#define PCINT5  5
#define PCINT6  6
#define PCINT7  7

#define PCMSK1  _SFR_MEM8(0x6C)
#define PCINT8  0
#define PCINT9  1
#define PCINT10 2
#define PCINT11 3
#define PCINT12 4
#define PCINT13 5
#define PCINT14 6
#define PCINT15 7

#define PCMSK2  _SFR_MEM8(0x6D)
#define PCINT16 0
#define PCINT17 1
#define PCINT18 2
#define PCINT19 3
#define PCINT20 4
#define PCINT21 5
#define PCINT22 6
#define PCINT23 7

#define TIMSK0  _SFR_MEM8(0x6E)
#define TOIE0   0
#define OCIE0A  1

#define TIMSK1  _SFR_MEM8(0x6F)
#define TOIE1   0
#define OCIE1A  1
#define OCIE1B  2
#define ICIE1   5

#define TIMSK2  _SFR_MEM8(0x70)
#define TOIE2   0
#define OCIE2A  1

/* Reserved [0x71..0x72] */

#define PCMSK3  _SFR_MEM8(0x73)
#define PCINT24 0
#define PCINT25 1
#define PCINT26 2
#define PCINT27 3
#define PCINT28 4
#define PCINT29 5
#define PCINT30 6

/* Reserved [0x74..0x77] */

/* Combine ADCL and ADCH */
#ifndef __ASSEMBLER__
#define ADC     _SFR_MEM16(0x78)
#endif
#define ADCW    _SFR_MEM16(0x78)
#define ADCL    _SFR_MEM8(0x78)
#define ADCH    _SFR_MEM8(0x79)

#define ADCSRA  _SFR_MEM8(0x7A)
#define ADPS0   0
#define ADPS1   1
#define ADPS2   2
#define ADIE    3
#define ADIF    4
#define ADATE   5
#define ADSC    6
#define ADEN    7

#define ADCSRB  _SFR_MEM8(0x7B)
#define ADTS0   0
#define ADTS1   1
#define ADTS2   2
#define ACME    6

#define ADMUX   _SFR_MEM8(0x7C)
#define MUX0    0
#define MUX1    1
#define MUX2    2
#define MUX3    3
#define MUX4    4
#define ADLAR   5
#define REFS0   6
#define REFS1   7

/* Reserved [0x7D] */

#define DIDR0   _SFR_MEM8(0x7E)
#define ADC0D   0
#define ADC1D   1
#define ADC2D   2
#define ADC3D   3
#define ADC4D   4
#define ADC5D   5
#define ADC6D   6
#define ADC7D   7

#define DIDR1   _SFR_MEM8(0x7F)
#define AIN0D   0
#define AIN1D   1

#define TCCR1A  _SFR_MEM8(0X80)
#define WGM10   0
#define WGM11   1
#define COM1B0  4
#define COM1B1  5
#define COM1A0  6
#define COM1A1  7

#define TCCR1B  _SFR_MEM8(0X81)
#define CS10    0
#define CS11    1
#define CS12    2
#define WGM12   3
#define WGM13   4
#define ICES1   6
#define ICNC1   7

#define TCCR1C  _SFR_MEM8(0x82)
#define FOC1B   6
#define FOC1A   7

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

/* Reserved [0x8C..0xAF] */

#define TCCR2A  _SFR_MEM8(0xB0)
#define CS20    0
#define CS21    1
#define CS22    2
#define WGM21   3
#define COM2A0  4
#define COM2A1  5
#define WGM20   6
#define FOC2A   7

/* Reserved [0xB1] */

#define TCNT2   _SFR_MEM8(0xB2)

#define OCR2A   _SFR_MEM8(0xB3)

/* Reserved [0xB4..0xB5] */

#define ASSR    _SFR_MEM8(0xB6)
#define TCR2UB  0
#define OCR2UB  1
#define TCN2UB  2
#define AS2     3
#define EXCLK   4

/* Reserved [0xB7] */

#define USICR   _SFR_MEM8(0xB8)
#define USITC   0
#define USICLK  1
#define USICS0  2
#define USICS1  3
#define USIWM0  4
#define USIWM1  5
#define USIOIE  6
#define USISIE  7

#define USISR   _SFR_MEM8(0xB9)
#define USICNT0 0
#define USICNT1 1
#define USICNT2 2
#define USICNT3 3
#define USIDC   4
#define USIPF   5
#define USIOIF  6
#define USISIF  7

#define USIDR   _SFR_MEM8(0xBA)

/* Reserved [0xBB..0xBF] */

#define UCSR0A  _SFR_MEM8(0xC0)
#define MPCM0   0
#define U2X0    1
#define UPE0    2
#define DOR0    3
#define FE0     4
#define UDRE0   5
#define TXC0    6
#define RXC0    7

#define UCSR0B  _SFR_MEM8(0XC1)
#define TXB80   0
#define RXB80   1
#define UCSZ02  2
#define TXEN0   3
#define RXEN0   4
#define UDRIE0  5
#define TXCIE0  6
#define RXCIE0  7

#define UCSR0C  _SFR_MEM8(0xC2)
#define UCPOL0  0
#define UCSZ00  1
#define UCSZ01  2
#define USBS0   3
#define UPM00   4
#define UPM01   5
#define UMSEL0  6

/* Reserved [0xC3] */

/* Combine UBRR0L and UBRR0H */
#define UBRR0   _SFR_MEM16(0xC4)

#define UBRR0L  _SFR_MEM8(0xC4)
#define UBRR0H  _SFR_MEM8(0xC5)

#define UDR0    _SFR_MEM8(0XC6)

/* Reserved [0xC7..0xD7] */

#define PINH    _SFR_MEM8(0xD8)
#define PINH7   7
#define PINH6   6
#define PINH5   5
#define PINH4   4
#define PINH3   3
#define PINH2   2
#define PINH1   1
#define PINH0   0

#define DDRH    _SFR_MEM8(0xD9)
#define DDH7    7
#define DDH6    6
#define DDH5    5
#define DDH4    4
#define DDH3    3
#define DDH2    2
#define DDH1    1
#define DDH0    0

#define PORTH   _SFR_MEM8(0xDA)
#define PH7     7
#define PH6     6
#define PH5     5
#define PH4     4
#define PH3     3
#define PH2     2
#define PH1     1
#define PH0     0

#define PINJ    _SFR_MEM8(0xDB)
#define PINJ6   6
#define PINJ5   5
#define PINJ4   4
#define PINJ3   3
#define PINJ2   2
#define PINJ1   1
#define PINJ0   0

#define DDRJ    _SFR_MEM8(0xDC)
#define DDJ6    6
#define DDJ5    5
#define DDJ4    4
#define DDJ3    3
#define DDJ2    2
#define DDJ1    1
#define DDJ0    0

#define PORTJ   _SFR_MEM8(0xDD)
#define PJ6     6
#define PJ5     5
#define PJ4     4
#define PJ3     3
#define PJ2     2
#define PJ1     1
#define PJ0     0

/* Reserved [0xDE..0xE3] */

#define LCDCRA  _SFR_MEM8(0XE4)
#define LCDBL   0
#if defined(__AVR_ATmega3290P__)
#define LCDCCD  1
#define LCDBD   2
#endif
#define LCDIE   3
#define LCDIF   4
#define LCDAB   6
#define LCDEN   7

#define LCDCRB  _SFR_MEM8(0XE5)
#define LCDPM0  0
#define LCDPM1  1
#define LCDPM2  2
#define LCDPM3  3
#define LCDMUX0 4
#define LCDMUX1 5
#define LCD2B   6
#define LCDCS   7

#define LCDFRR  _SFR_MEM8(0XE6)
#define LCDCD0  0
#define LCDCD1  1
#define LCDCD2  2
#define LCDPS0  4
#define LCDPS1  5
#define LCDPS2  6

#define LCDCCR  _SFR_MEM8(0XE7)
#define LCDCC0  0
#define LCDCC1  1
#define LCDCC2  2
#define LCDCC3  3
#if defined(__AVR_ATmega3290P__)
#define LCDMDT  4
#endif
#define LCDDC0  5
#define LCDDC1  6
#define LCDDC2  7

/* Reserved [0xE8..0xEB] */

#define LCDDR00 _SFR_MEM8(0XEC)
#define SEG000  0
#define SEG001  1
#define SEG002  2
#define SEG003  3
#define SEG004  4
#define SEG005  5
#define SEG006  6
#define SEG007  7

#define LCDDR01 _SFR_MEM8(0XED)
#define SEG008  0
#define SEG009  1
#define SEG010  2
#define SEG011  3
#define SEG012  4
#define SEG013  5
#define SEG014  6
#define SEG015  7

#define LCDDR02 _SFR_MEM8(0XEE)
#define SEG016  0
#define SEG017  1
#define SEG018  2
#define SEG019  3
#define SEG020  4
#define SEG021  5
#define SEG022  6
#define SEG023  7

#define LCDDR03 _SFR_MEM8(0XEF)
#define SEG024  0
#define SEG025  1
#define SEG026  2
#define SEG027  3
#define SEG028  4
#define SEG029  5
#define SEG030  6
#define SEG031  7

#define LCDDR04 _SFR_MEM8(0XF0)
#define SEG032  0
#define SEG033  1
#define SEG034  2
#define SEG035  3
#define SEG036  4
#define SEG037  5
#define SEG038  6
#define SEG039  7

#define LCDDR05 _SFR_MEM8(0XF1)
#define SEG100  0
#define SEG101  1
#define SEG102  2
#define SEG103  3
#define SEG104  4
#define SEG105  5
#define SEG106  6
#define SEG107  7

#define LCDDR06 _SFR_MEM8(0XF2)
#define SEG108  0
#define SEG109  1
#define SEG110  2
#define SEG111  3
#define SEG112  4
#define SEG113  5
#define SEG114  6
#define SEG115  7

#define LCDDR07 _SFR_MEM8(0XF3)
#define SEG116  0
#define SEG117  1
#define SEG118  2
#define SEG119  3
#define SEG120  4
#define SEG121  5
#define SEG122  6
#define SEG123  7

#define LCDDR08 _SFR_MEM8(0XF4)
#define SEG124  0
#define SEG125  1
#define SEG126  2
#define SEG127  3
#define SEG128  4
#define SEG129  5
#define SEG130  6
#define SEG131  7

#define LCDDR09 _SFR_MEM8(0XF5)
#define SEG132  0
#define SEG133  1
#define SEG134  2
#define SEG135  3
#define SEG136  4
#define SEG137  5
#define SEG138  6
#define SEG139  7

#define LCDDR10 _SFR_MEM8(0XF6)
#define SEG200  0
#define SEG201  1
#define SEG202  2
#define SEG203  3
#define SEG204  4
#define SEG205  5
#define SEG206  6
#define SEG207  7

#define LCDDR11 _SFR_MEM8(0XF7)
#define SEG208  0
#define SEG209  1
#define SEG210  2
#define SEG211  3
#define SEG212  4
#define SEG213  5
#define SEG214  6
#define SEG215  7

#define LCDDR12 _SFR_MEM8(0XF8)
#define SEG216  0
#define SEG217  1
#define SEG218  2
#define SEG219  3
#define SEG220  4
#define SEG221  5
#define SEG222  6
#define SEG223  7

#define LCDDR13 _SFR_MEM8(0XF9)
#define SEG224  0
#define SEG225  1
#define SEG226  2
#define SEG227  3
#define SEG228  4
#define SEG229  5
#define SEG230  6
#define SEG231  7

#define LCDDR14 _SFR_MEM8(0XFA)
#define SEG232  0
#define SEG233  1
#define SEG234  2
#define SEG235  3
#define SEG236  4
#define SEG237  5
#define SEG238  6
#define SEG239  7

#define LCDDR15 _SFR_MEM8(0XFB)
#define SEG300  0
#define SEG301  1
#define SEG302  2
#define SEG303  3
#define SEG304  4
#define SEG305  5
#define SEG306  6
#define SEG307  7

#define LCDDR16 _SFR_MEM8(0XFC)
#define SEG308  0
#define SEG309  1
#define SEG310  2
#define SEG311  3
#define SEG312  4
#define SEG313  5
#define SEG314  6
#define SEG315  7

#define LCDDR17 _SFR_MEM8(0XFD)
#define SEG316  0
#define SEG217  1
#define SEG318  2
#define SEG319  3
#define SEG320  4
#define SEG321  5
#define SEG322  6
#define SEG323  7

#define LCDDR18 _SFR_MEM8(0XFE)
#define SEG324  0
#define SEG325  1
#define SEG326  2
#define SEG327  3
#define SEG328  4
#define SEG329  5
#define SEG330  6
#define SEG331  7

#define LCDDR19 _SFR_MEM8(0XFF)
#define SEG332  0
#define SEG333  1
#define SEG334  2
#define SEG335  3
#define SEG336  4
#define SEG337  5
#define SEG338  6
#define SEG339  7


/* Interrupt vectors */
/* Vector 0 is the reset vector */
/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* Pin Change Interrupt Request 0 */
#define PCINT0_vect			_VECTOR(2)
#define SIG_PIN_CHANGE0			_VECTOR(2)

/* Pin Change Interrupt Request 1 */
#define PCINT1_vect			_VECTOR(3)
#define SIG_PIN_CHANGE1			_VECTOR(3)

/* Timer/Counter2 Compare Match */
#define TIMER2_COMP_vect		_VECTOR(4)
#define SIG_OUTPUT_COMPARE2		_VECTOR(4)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(5)
#define SIG_OVERFLOW2			_VECTOR(5)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(6)
#define SIG_INPUT_CAPTURE1		_VECTOR(6)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(7)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(7)

/* Timer/Counter Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(8)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(8)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(9)
#define SIG_OVERFLOW1			_VECTOR(9)

/* Timer/Counter0 Compare Match */
#define TIMER0_COMP_vect		_VECTOR(10)
#define SIG_OUTPUT_COMPARE0		_VECTOR(10)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(11)
#define SIG_OVERFLOW0			_VECTOR(11)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(12)
#define SIG_SPI				_VECTOR(12)

/* USART, Rx Complete */
#define USART_RX_vect			_VECTOR(13)
#define SIG_UART_RECV			_VECTOR(13)

/* USART Data register Empty */
#define USART_UDRE_vect			_VECTOR(14)
#define SIG_UART_DATA			_VECTOR(14)

/* USART0, Tx Complete */
#define USART0_TX_vect			_VECTOR(15)
#define SIG_UART_TRANS			_VECTOR(15)

/* USI Start Condition */
#define USI_START_vect			_VECTOR(16)
#define SIG_USI_START			_VECTOR(16)

/* USI Overflow */
#define USI_OVERFLOW_vect		_VECTOR(17)
#define SIG_USI_OVERFLOW		_VECTOR(17)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(18)
#define SIG_COMPARATOR			_VECTOR(18)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(19)
#define SIG_ADC				_VECTOR(19)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(20)
#define SIG_EEPROM_READY		_VECTOR(20)

/* Store Program Memory Read */
#define SPM_READY_vect			_VECTOR(21)
#define SIG_SPM_READY			_VECTOR(21)

/* LCD Start of Frame */
#define LCD_vect			_VECTOR(22)
#define SIG_LCD				_VECTOR(22)

/* Pin Change Interrupt Request 2 */
#define PCINT2_vect			_VECTOR(23)
#define SIG_PIN_CHANGE2			_VECTOR(23)

/* Pin Change Interrupt Request 3 */
#define PCINT3_vect			_VECTOR(24)
#define SIG_PIN_CHANGE3			_VECTOR(24)

#define _VECTORS_SIZE 100


/* Constants */
#define SPM_PAGESIZE    128
#define RAMEND          0x8FF
#define XRAMEND         0x8FF
#define E2END           0x3FF
#define E2PAGESIZE      4
#define FLASHEND        0x7FFF


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
#define FUSE_RSTDISBL    (unsigned char)~_BV(0)
#define FUSE_BODLEVEL0   (unsigned char)~_BV(1)
#define FUSE_BODLEVEL1   (unsigned char)~_BV(2)
#define EFUSE_DEFAULT (0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x95
#define SIGNATURE_2 0x04


#endif /* _AVR_IOM3290_H_ */
