/* Copyright (c) 2002, Steinar Haugen
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

/* $Id: iom8535.h,v 1.11.2.5 2008/10/17 23:27:50 arcanum Exp $ */

/* avr/iom8535.h - definitions for ATmega8535 */

#ifndef _AVR_IOM8535_H_
#define _AVR_IOM8535_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom8535.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* TWI stands for "Two Wire Interface" or "TWI Was I2C(tm)" */
#define TWBR    _SFR_IO8(0x00)
#define TWSR    _SFR_IO8(0x01)
#define TWAR    _SFR_IO8(0x02)
#define TWDR    _SFR_IO8(0x03)

/* ADC Data register */
#ifndef __ASSEMBLER__
#define ADC     _SFR_IO16(0x04)
#endif
#define ADCW    _SFR_IO16(0x04)
#define ADCL    _SFR_IO8(0x04)
#define ADCH    _SFR_IO8(0x05)

/* ADC Control and Status Register */
#define ADCSRA  _SFR_IO8(0x06)

/* ADC MUX */
#define ADMUX   _SFR_IO8(0x07)

/* Analog Comparator Control and Status Register */
#define ACSR    _SFR_IO8(0x08)

/* USART Baud Rate Register */
#define UBRRL   _SFR_IO8(0x09)

/* USART Control and Status Register B */
#define UCSRB   _SFR_IO8(0x0A)

/* USART Control and Status Register A */
#define UCSRA   _SFR_IO8(0x0B)

/* USART I/O Data Register */
#define UDR     _SFR_IO8(0x0C)

/* SPI Control Register */
#define SPCR    _SFR_IO8(0x0D)

/* SPI Status Register */
#define SPSR    _SFR_IO8(0x0E)

/* SPI I/O Data Register */
#define SPDR    _SFR_IO8(0x0F)

/* Input Pins, Port D */
#define PIND    _SFR_IO8(0x10)

/* Data Direction Register, Port D */
#define DDRD    _SFR_IO8(0x11)

/* Data Register, Port D */
#define PORTD   _SFR_IO8(0x12)

/* Input Pins, Port C */
#define PINC    _SFR_IO8(0x13)

/* Data Direction Register, Port C */
#define DDRC    _SFR_IO8(0x14)

/* Data Register, Port C */
#define PORTC   _SFR_IO8(0x15)

/* Input Pins, Port B */
#define PINB    _SFR_IO8(0x16)

/* Data Direction Register, Port B */
#define DDRB    _SFR_IO8(0x17)

/* Data Register, Port B */
#define PORTB   _SFR_IO8(0x18)

/* Input Pins, Port A */
#define PINA    _SFR_IO8(0x19)

/* Data Direction Register, Port A */
#define DDRA    _SFR_IO8(0x1A)

/* Data Register, Port A */
#define PORTA   _SFR_IO8(0x1B)

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO16(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEARH	_SFR_IO8(0x1F)

/* USART Baud Rate Register HI         */
/* USART Control and Status Register C */
#define UBRRH   _SFR_IO8(0x20)
#define UCSRC   UBRRH

/* Watchdog Timer Control Register */
#define WDTCR   _SFR_IO8(0x21)

/* Asynchronous mode Status Register */
#define ASSR    _SFR_IO8(0x22)

/* Timer/Counter2 Output Compare Register */
#define OCR2    _SFR_IO8(0x23)

/* Timer/Counter 2 */
#define TCNT2   _SFR_IO8(0x24)

/* Timer/Counter 2 Control Register */
#define TCCR2   _SFR_IO8(0x25)

/* T/C 1 Input Capture Register */
#define ICR1    _SFR_IO16(0x26)
#define ICR1L   _SFR_IO8(0x26)
#define ICR1H   _SFR_IO8(0x27)

/* Timer/Counter1 Output Compare Register B */
#define OCR1B   _SFR_IO16(0x28)
#define OCR1BL  _SFR_IO8(0x28)
#define OCR1BH  _SFR_IO8(0x29)

/* Timer/Counter1 Output Compare Register A */
#define OCR1A   _SFR_IO16(0x2A)
#define OCR1AL  _SFR_IO8(0x2A)
#define OCR1AH  _SFR_IO8(0x2B)

/* Timer/Counter 1 */
#define TCNT1   _SFR_IO16(0x2C)
#define TCNT1L  _SFR_IO8(0x2C)
#define TCNT1H  _SFR_IO8(0x2D)

/* Timer/Counter 1 Control and Status Register */
#define TCCR1B  _SFR_IO8(0x2E)

/* Timer/Counter 1 Control Register */
#define TCCR1A  _SFR_IO8(0x2F)

/* Special Function IO Register */
#define SFIOR   _SFR_IO8(0x30)

/* Oscillator Calibration Register */
#define OSCCAL  _SFR_IO8(0x31)

/* Timer/Counter 0 */
#define TCNT0   _SFR_IO8(0x32)

/* Timer/Counter 0 Control Register */
#define TCCR0   _SFR_IO8(0x33)

/* MCU Control and Status Register */
#define MCUCSR  _SFR_IO8(0x34)

/* MCU Control Register */
#define MCUCR   _SFR_IO8(0x35)

/* TWI Control Register */
#define TWCR    _SFR_IO8(0x36)

/* Store Program Memory Control Register */
#define SPMCR   _SFR_IO8(0x37)

/* Timer/Counter Interrupt Flag register */
#define TIFR    _SFR_IO8(0x38)

/* Timer/Counter Interrupt MaSK register */
#define TIMSK   _SFR_IO8(0x39)

/* General Interrupt Flag Register */
#define GIFR    _SFR_IO8(0x3A)

/* General Interrupt MaSK register */
#define GICR    _SFR_IO8(0x3B)

/* Timer/Counter 0 Output Compare Register */
#define OCR0    _SFR_IO8(0x3C)

/* 0x3D..0x3E SP */

/* 0x3F SREG */

/* Interrupt vectors */

/* External Interrupt 0 */
#define INT0_vect			_VECTOR(1)
#define SIG_INTERRUPT0			_VECTOR(1)

/* External Interrupt 1 */
#define INT1_vect			_VECTOR(2)
#define SIG_INTERRUPT1			_VECTOR(2)

/* Timer/Counter2 Compare Match */
#define TIMER2_COMP_vect		_VECTOR(3)
#define SIG_OUTPUT_COMPARE2		_VECTOR(3)

/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(4)
#define SIG_OVERFLOW2			_VECTOR(4)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect		_VECTOR(5)
#define SIG_INPUT_CAPTURE1		_VECTOR(5)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(6)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(6)

/* Timer/Counter1 Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(7)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(7)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(8)
#define SIG_OVERFLOW1			_VECTOR(8)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(9)
#define SIG_OVERFLOW0			_VECTOR(9)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect			_VECTOR(10)
#define SIG_SPI				_VECTOR(10)

/* USART, RX Complete */
#define USART_RX_vect			_VECTOR(11)
#define SIG_UART_RECV			_VECTOR(11)

/* USART Data Register Empty */
#define USART_UDRE_vect			_VECTOR(12)
#define SIG_UART_DATA			_VECTOR(12)

/* USART, TX Complete */
#define USART_TX_vect			_VECTOR(13)
#define SIG_UART_TRANS			_VECTOR(13)

/* ADC Conversion Complete */
#define ADC_vect			_VECTOR(14)
#define SIG_ADC				_VECTOR(14)

/* EEPROM Ready */
#define EE_RDY_vect			_VECTOR(15)
#define SIG_EEPROM_READY		_VECTOR(15)

/* Analog Comparator */
#define ANA_COMP_vect			_VECTOR(16)
#define SIG_COMPARATOR			_VECTOR(16)

/* Two-wire Serial Interface */
#define TWI_vect			_VECTOR(17)
#define SIG_2WIRE_SERIAL		_VECTOR(17)

/* External Interrupt Request 2 */
#define INT2_vect			_VECTOR(18)
#define SIG_INTERRUPT2			_VECTOR(18)

/* TimerCounter0 Compare Match */
#define TIMER0_COMP_vect		_VECTOR(19)
#define SIG_OUTPUT_COMPARE0		_VECTOR(19)

/* Store Program Memory Read */
#define SPM_RDY_vect			_VECTOR(20)
#define SIG_SPM_READY			_VECTOR(20)

#define _VECTORS_SIZE 42

/*
   The Register Bit names are represented by their bit number (0-7).
*/

/* General Interrupt Control Register */
#define    INT1         7
#define    INT0         6
#define    INT2         5
#define    IVSEL        1
#define    IVCE         0

/* General Interrupt Flag Register */
#define    INTF1        7
#define    INTF0        6
#define    INTF2        5

/* Timer/Counter Interrupt MaSK register */
#define    OCIE2        7
#define    TOIE2        6
#define    TICIE1       5
#define    OCIE1A       4
#define    OCIE1B       3
#define    TOIE1        2
#define    OCIE0        1
#define    TOIE0        0

/* Timer/Counter Interrupt Flag register */
#define    OCF2         7
#define    TOV2         6
#define    ICF1         5
#define    OCF1A        4
#define    OCF1B        3
#define    TOV1         2
#define    OCF0         1
#define    TOV0         0

/* Store Program Memory Control Register */
#define    SPMIE        7
#define    RWWSB        6
#define    RWWSRE       4
#define    BLBSET       3
#define    PGWRT        2
#define    PGERS        1
#define    SPMEN        0

/* TWI Control Register */
#define    TWINT        7
#define    TWEA         6
#define    TWSTA        5
#define    TWSTO        4
#define    TWWC         3
#define    TWEN         2
#define    TWIE         0

/* MCU Control Register */
#define    SM2          7
#define    SE           6
#define    SM1          5
#define    SM0          4
#define    ISC11        3
#define    ISC10        2
#define    ISC01        1
#define    ISC00        0

/* MCU Control and Status Register */
#define    ISC2         6
#define    WDRF         3
#define    BORF         2
#define    EXTRF        1
#define    PORF         0

/* Timer/Counter 0 Control Register */
#define    FOC0         7
#define    WGM00        6
#define    COM01        5
#define    COM00        4
#define    WGM01        3
#define    CS02         2
#define    CS01         1
#define    CS00         0

/* 
   The ADHSM bit has been removed from all documentation, 
   as being not needed at all since the comparator has proven 
   to be fast enough even without feeding it more power.
*/

/* Special Function IO Register */
#define    ADTS2        7
#define    ADTS1        6
#define    ADTS0        5
#define    ACME         3
#define    PUD          2
#define    PSR2         1
#define    PSR10        0

/* Timer/Counter 1 Control Register */
#define    COM1A1       7
#define    COM1A0       6
#define    COM1B1       5
#define    COM1B0       4
#define    FOC1A        3
#define    FOC1B        2
#define    WGM11        1
#define    WGM10        0

/* Timer/Counter 1 Control and Status Register */
#define    ICNC1        7
#define    ICES1        6
#define    WGM13        4
#define    WGM12        3
#define    CS12         2
#define    CS11         1
#define    CS10         0

/* Timer/Counter 2 Control Register */
#define    FOC2         7
#define    WGM20        6
#define    COM21        5
#define    COM20        4
#define    WGM21        3
#define    CS22         2
#define    CS21         1
#define    CS20         0

/* Asynchronous mode Status Register */
#define    AS2          3
#define    TCN2UB       2
#define    OCR2UB       1
#define    TCR2UB       0

/* Watchdog Timer Control Register */
#define    WDCE         4
#define    WDE          3
#define    WDP2         2
#define    WDP1         1
#define    WDP0         0

/* USART Control and Status Register C */
#define    URSEL        7
#define    UMSEL        6
#define    UPM1         5
#define    UPM0         4
#define    USBS         3
#define    UCSZ1        2
#define    UCSZ0        1
#define    UCPOL        0

/* Data Register, Port A */
#define    PA7          7
#define    PA6          6
#define    PA5          5
#define    PA4          4
#define    PA3          3
#define    PA2          2
#define    PA1          1
#define    PA0          0

/* Data Direction Register, Port A */
#define    DDA7         7
#define    DDA6         6
#define    DDA5         5
#define    DDA4         4
#define    DDA3         3
#define    DDA2         2
#define    DDA1         1
#define    DDA0         0

/* Input Pins, Port A */
#define    PINA7        7
#define    PINA6        6
#define    PINA5        5
#define    PINA4        4
#define    PINA3        3
#define    PINA2        2
#define    PINA1        1
#define    PINA0        0

/* Data Register, Port B */
#define    PB7          7
#define    PB6          6
#define    PB5          5
#define    PB4          4
#define    PB3          3
#define    PB2          2
#define    PB1          1
#define    PB0          0

/* Data Direction Register, Port B */
#define    DDB7         7
#define    DDB6         6
#define    DDB5         5
#define    DDB4         4
#define    DDB3         3
#define    DDB2         2
#define    DDB1         1
#define    DDB0         0

/* Input Pins, Port B */
#define    PINB7        7
#define    PINB6        6
#define    PINB5        5
#define    PINB4        4
#define    PINB3        3
#define    PINB2        2
#define    PINB1        1
#define    PINB0        0

/* Data Register, Port C */
#define    PC7          7
#define    PC6          6
#define    PC5          5
#define    PC4          4
#define    PC3          3
#define    PC2          2
#define    PC1          1
#define    PC0          0

/* Data Direction Register, Port C */
#define    DDC7         7
#define    DDC6         6
#define    DDC5         5
#define    DDC4         4
#define    DDC3         3
#define    DDC2         2
#define    DDC1         1
#define    DDC0         0

/* Input Pins, Port C */
#define    PINC7        7
#define    PINC6        6
#define    PINC5        5
#define    PINC4        4
#define    PINC3        3
#define    PINC2        2
#define    PINC1        1
#define    PINC0        0

/* Data Register, Port D */
#define    PD7          7
#define    PD6          6
#define    PD5          5
#define    PD4          4
#define    PD3          3
#define    PD2          2
#define    PD1          1
#define    PD0          0

/* Data Direction Register, Port D */
#define    DDD7         7
#define    DDD6         6
#define    DDD5         5
#define    DDD4         4
#define    DDD3         3
#define    DDD2         2
#define    DDD1         1
#define    DDD0         0

/* Input Pins, Port D */
#define    PIND7        7
#define    PIND6        6
#define    PIND5        5
#define    PIND4        4
#define    PIND3        3
#define    PIND2        2
#define    PIND1        1
#define    PIND0        0

/* SPI Status Register */
#define    SPIF         7
#define    WCOL         6
#define    SPI2X        0

/* SPI Control Register */
#define    SPIE         7
#define    SPE          6
#define    DORD         5
#define    MSTR         4
#define    CPOL         3
#define    CPHA         2
#define    SPR1         1
#define    SPR0         0

/* USART Control and Status Register A */
#define    RXC          7
#define    TXC          6
#define    UDRE         5
#define    FE           4
#define    DOR          3
#define    PE           2
#define    U2X          1
#define    MPCM         0

/* USART Control and Status Register B */
#define    RXCIE        7
#define    TXCIE        6
#define    UDRIE        5
#define    RXEN         4
#define    TXEN         3
#define    UCSZ2        2
#define    RXB8         1
#define    TXB8         0

/* Analog Comparator Control and Status Register */
#define    ACD          7
#define    ACBG         6
#define    ACO          5
#define    ACI          4
#define    ACIE         3
#define    ACIC         2
#define    ACIS1        1
#define    ACIS0        0

/* ADC Multiplexer Selection Register */
#define    REFS1        7
#define    REFS0        6
#define    ADLAR        5
#define    MUX4         4
#define    MUX3         3
#define    MUX2         2
#define    MUX1         1
#define    MUX0         0

/* ADC Control and Status Register */
#define    ADEN         7
#define    ADSC         6
#define    ADATE        5
#define    ADIF         4
#define    ADIE         3
#define    ADPS2        2
#define    ADPS1        1
#define    ADPS0        0

/* TWI (Slave) Address Register */
#define    TWGCE        0

/* TWI Status Register */
#define    TWS7         7
#define    TWS6         6
#define    TWS5         5
#define    TWS4         4
#define    TWS3         3
#define    TWPS1        1
#define    TWPS0        0

/* EEPROM Control Register */
#define    EERIE        3
#define    EEMWE        2
#define    EEWE         1
#define    EERE         0

/* Constants */
#define SPM_PAGESIZE 64
#define RAMEND       0x25F    /* Last On-Chip SRAM Location */
#define XRAMEND      0x25F
#define E2END        0x1FF
#define E2PAGESIZE   4
#define FLASHEND     0x1FFF


/* Fuses */

#define FUSE_MEMORY_SIZE 2

/* Low Fuse Byte */
#define FUSE_CKSEL0      (unsigned char)~_BV(0)
#define FUSE_CKSEL1      (unsigned char)~_BV(1)
#define FUSE_CKSEL2      (unsigned char)~_BV(2)
#define FUSE_CKSEL3      (unsigned char)~_BV(3)
#define FUSE_SUT0        (unsigned char)~_BV(4)
#define FUSE_SUT1        (unsigned char)~_BV(5)
#define FUSE_BODEN       (unsigned char)~_BV(6)
#define FUSE_BODLEVEL    (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_CKSEL1 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0 & FUSE_SUT1)

/* High Fuse Byte */
#define FUSE_BOOTRST     (unsigned char)~_BV(0)
#define FUSE_BOOTSZ0     (unsigned char)~_BV(1)
#define FUSE_BOOTSZ1     (unsigned char)~_BV(2)
#define FUSE_EESAVE      (unsigned char)~_BV(3)
#define FUSE_CKOPT       (unsigned char)~_BV(4)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_WDTON       (unsigned char)~_BV(6)
#define FUSE_S8535C      (unsigned char)~_BV(7)
#define HFUSE_DEFAULT (FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_SPIEN)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x93
#define SIGNATURE_2 0x08


#endif /* _AVR_IOM8535_H_ */
