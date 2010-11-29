/* Copyright (c) 2007 Anatoly Sokolov 
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

/* $Id: iousbxx2.h,v 1.3 2007/05/12 11:40:46 aesok Exp $ */

/* iousbxx2.h - definitions for AT90USB82 and AT90USB162.  */

#ifndef _AVR_IOUSBXX2_H_
#define _AVR_IOUSBXX2_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iousbxx2.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* Registers and associated bit numbers */

/* Reserved [0x00..0x02] */

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
#define PINC2   2
#define PINC1   1
#define PINC0   0

#define DDRC    _SFR_IO8(0x07)
#define DDC7    7
#define DDC6    6
#define DDC5    5
#define DDC4    4
#define DDC2    2
#define DDC1    1
#define DDC0    0

#define PORTC   _SFR_IO8(0x08)
#define PC7     7
#define PC6     6
#define PC5     5
#define PC4     4
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

/* Reserved [0xC..0x14] */

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

/* Reserved [0x17..0x1A] */

#define PCIFR   _SFR_IO8(0x1B)
#define PCIF1   1
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

#define DWDR    _SFR_IO8(0x31)
#define IDRD    7

/* Reserved [0x32] */

#define SMCR    _SFR_IO8(0x33)
#define SM2     3
#define SM1     2
#define SM0     1
#define SE      0

#define MCUSR   _SFR_IO8(0x34)
#define USBRF   5
#define WDRF    3
#define BORF    2
#define EXTRF   1
#define PORF    0

#define MCUCR   _SFR_IO8(0x35)
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
#define CLKPS3  3
#define CLKPS2  2
#define CLKPS1  1
#define CLKPS0  0

#define WDTCKD  _SFR_MEM8(0x62)
#define WDEWIF  3
#define WDEWIE  2
#define WCLKD1  1
#define WCLKD0  0

#define REGCR   _SFR_MEM8(0x63)
#define REGDIS  0

#define PRR0    _SFR_MEM8(0x64)
#define PRTIM0  5
#define PRTIM1  3
#define PRSPI   2

#define PRR1    _SFR_MEM8(0x65)
#define PRUSB   7
#define PRUSART1 0

#define OSCCAL  _SFR_MEM8(0x66)

/* Reserved [0x67] */

#define PCICR   _SFR_MEM8(0x68)
#define PCIE1   1
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

#define PCMSK1  _SFR_MEM8(0x6C)
#define PCINT12 4
#define PCINT11 3
#define PCINT10 2
#define PCINT9  1
#define PCINT8  0

/* Reserved [0x6D] */

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

/* Reserved [0x70..0x7F] */

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

/* Reserved [0x8E..0xC7] */

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

#define UCSR1D  _SFR_MEM8(0xCB)
#define CTSEN   1
#define RTSEN   0

/* Combine UBRR1L and UBRR1H */
#define UBRR1   _SFR_MEM16(0xCC)

#define UBRR1L  _SFR_MEM8(0xCC)
#define UBRR1H  _SFR_MEM8(0xCD)

#define UDR1    _SFR_MEM8(0XCE)

/* Reserved [0xCF] */

#define CKSEL0  _SFR_MEM8(0XD0)
#define RCSUT1  7
#define RCSUT0  6
#define EXSUT1  5
#define EXSUT0  4
#define RCE     3
#define EXTE    2
#define CLKS    0

#define CKSEL1  _SFR_MEM8(0XD1)
#define RCCKSEL3 7
#define RCCKSEL2 6
#define RCCKSEL1 5
#define RCCKSEL0 4
#define EXCKSEL3 3
#define EXCKSEL2 2
#define EXCKSEL1 1
#define EXCKSEL0 0

#define CKSTA   _SFR_MEM8(0XD2)
#define RCON    1
#define EXTON   0

/* Reserved [0xD3..0xD7] */

#define USBCON  _SFR_MEM8(0XD8)
#define USBE    7
#define FRZCLK  5

/* Reserved [0xD9..0xDA] */

/* Combine UDPADDL and UDPADDH */
#define UDPADD  _SFR_MEM16(0xDB)

#define UDPADDL _SFR_MEM8(0xDB)
#define UDPADDH _SFR_MEM8(0xDC)
#define DPACC   7

/* Reserved [0xDD..0xDF] */

#define UDCON   _SFR_MEM8(0XE0)
#define RSTCPU  2
#define RMWKUP  1
#define DETACH  0

#define UDINT   _SFR_MEM8(0XE1)
#define UPRSMI  6
#define EORSMI  5
#define WAKEUPI 4
#define EORSTI  3
#define SOFI    2
#define SUSPI   0

#define UDIEN   _SFR_MEM8(0XE2)
#define UPRSME  6
#define EORSME  5
#define WAKEUPE 4
#define EORSTE  3
#define SOFE    2
#define SUSPE   0

#define UDADDR  _SFR_MEM8(0XE3)
#define ADDEN   7

/* Combine UDFNUML and UDFNUMH */
#define UDFNUM  _SFR_MEM16(0xE4)

#define UDFNUML _SFR_MEM8(0xE4)
#define UDFNUMH _SFR_MEM8(0xE5)

#define UDMFN   _SFR_MEM8(0XE6)
#define FNCERR  4

/* Reserved [0xE7] */

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
#define EPNUM2  2
#define EPNUM1  1
#define EPNUM0  0

#define UERST   _SFR_MEM8(0XEA)
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

#define UEBCLX  _SFR_MEM8(0xF2)

/* Reserved [0xF3] */

#define UEINT   _SFR_MEM8(0XF4)
#define EPINT4  4
#define EPINT3  3
#define EPINT2  2
#define EPINT1  1
#define EPINT0  0

/* Reserved [0xF5..0xF9] */

#define PS2CON  _SFR_MEM8(0XFA)
#define PS2EN   0

#define UPOE    _SFR_MEM8(0XFB)
#define UPWE1   7
#define UPWE0   6
#define UPDRV1  5
#define UPDRV0  4
#define SCKI    3
#define DATAI   2
#define DPI     1
#define DMI     0

/* Reserved [0xFC..0xFF] */

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

/* Pin Change Interrupt Request 1 */
#define PCINT1_vect			_VECTOR(10)

/* USB General Interrupt Request */
#define USB_GEN_vect			_VECTOR(11)

/* USB Endpoint/Pipe Interrupt Communication Request */
#define USB_COM_vect			_VECTOR(12)

/* Watchdog Time-out Interrupt */
#define WDT_vect			_VECTOR(13)

/* Timer/Counter2 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(14)

/* Timer/Counter2 Compare Match B */
#define TIMER1_COMPA_vect		_VECTOR(15)

/* Timer/Counter2 Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(16)

/* Timer/Counter2 Compare Match C */
#define TIMER1_COMPC_vect		_VECTOR(17)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(18)

/* Timer/Counter0 Compare Match A */
#define TIMER0_COMPA_vect		_VECTOR(19)

/* Timer/Counter0 Compare Match B */
#define TIMER0_COMPB_vect		_VECTOR(20)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(21)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(22)

/* USART1, Rx Complete */
#define USART1_RX_vect			_VECTOR(23)

/* USART1 Data register Empty */
#define USART1_UDRE_vect		_VECTOR(24)

/* USART1, Tx Complete */
#define USART1_TX_vect			_VECTOR(25)

/* Analog Comparator */
#define ANALOG_COMP_vect		_VECTOR(26)

/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(27)

/* Store Program Memory Read */
#define SPM_READY_vect			_VECTOR(28)

#define _VECTORS_SIZE 116

#endif  /* _AVR_IOUSBXX2_H_ */
