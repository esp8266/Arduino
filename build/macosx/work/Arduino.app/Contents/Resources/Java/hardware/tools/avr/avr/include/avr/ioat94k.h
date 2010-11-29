/* Copyright (c) 2002, Marek Michalkiewicz
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

/* $Id: ioat94k.h,v 1.9 2004/11/01 22:23:56 arcanum Exp $ */

/* avr/ioat94k.h - definitions for AT94K series FPSLIC(tm) */

#ifndef _AVR_IOAT94K_H_
#define _AVR_IOAT94K_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "ioat94k.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* UART1 Baud Rate Register */
#define UBRR1	_SFR_IO8(0x00)

/* UART1 Control and Status Registers */
#define UCSR1B	_SFR_IO8(0x01)
#define UCSR1A	_SFR_IO8(0x02)

/* UART1 I/O Data Register */
#define UDR1	_SFR_IO8(0x03)

/* 0x04 reserved */

/* Input Pins, Port E */
#define PINE	_SFR_IO8(0x05)

/* Data Direction Register, Port E */
#define DDRE	_SFR_IO8(0x06)

/* Data Register, Port E */
#define PORTE	_SFR_IO8(0x07)

/* On Chip Debug Register (reserved) */
#define OCDR    _SFR_IO8(0x08)

/* UART0 Baud Rate Register */
#define UBRR0	_SFR_IO8(0x09)

/* UART0 Control and Status Registers */
#define UCSR0B	_SFR_IO8(0x0A)
#define UCSR0A	_SFR_IO8(0x0B)

/* UART0 I/O Data Register */
#define UDR0	_SFR_IO8(0x0C)

/* 0x0D..0x0F reserved */

/* Input Pins, Port D */
#define PIND	_SFR_IO8(0x10)

/* Data Direction Register, Port D */
#define DDRD	_SFR_IO8(0x11)

/* Data Register, Port D */
#define PORTD	_SFR_IO8(0x12)

/* FPGA I/O Select Control Register */
#define FISCR	_SFR_IO8(0x13)

/* FPGA I/O Select Registers A, B, C, D */
#define FISUA	_SFR_IO8(0x14)
#define FISUB	_SFR_IO8(0x15)
#define FISUC	_SFR_IO8(0x16)
#define FISUD	_SFR_IO8(0x17)

/* FPGA Cache Logic(R) registers (top secret, under NDA) */
#define FPGAX	_SFR_IO8(0x18)
#define FPGAY	_SFR_IO8(0x19)
#define FPGAZ	_SFR_IO8(0x1A)
#define FPGAD	_SFR_IO8(0x1B)

/* TWI stands for "Two Wire Interface" or "TWI Was I2C(tm)" */

/* 2-wire Serial Bit Rate Register */
#define TWBR	_SFR_IO8(0x1C)

/* 2-wire Serial Status Register */
#define TWSR	_SFR_IO8(0x1D)

/* 2-wire Serial (Slave) Address Register */
#define TWAR	_SFR_IO8(0x1E)

/* 2-wire Serial Data Register */
#define TWDR	_SFR_IO8(0x1F)

/* UART Baud Register High */
#define UBRRH	_SFR_IO8(0x20)
#define UBRRHI	UBRRH           /* New name in datasheet (1138F-FPSLI-06/02) */

/* Watchdog Timer Control Register */
#define WDTCR	_SFR_IO8(0x21)

/* Timer/Counter2 Output Compare Register */
#define OCR2	_SFR_IO8(0x22)

/* Timer/Counter2 (8-bit) */
#define TCNT2	_SFR_IO8(0x23)

/* Timer/Counter1 Input Capture Register */
#define ICR1	_SFR_IO16(0x24)
#define ICR1L	_SFR_IO8(0x24)
#define ICR1H	_SFR_IO8(0x25)

/* Asynchronous mode StatuS Register */
#define ASSR	_SFR_IO8(0x26)

/* Timer/Counter2 Control Register */
#define TCCR2	_SFR_IO8(0x27)

/* Timer/Counter1 Output Compare RegisterB */
#define OCR1B	_SFR_IO16(0x28)
#define OCR1BL	_SFR_IO8(0x28)
#define OCR1BH	_SFR_IO8(0x29)

/* Timer/Counter1 Output Compare RegisterA */
#define OCR1A	_SFR_IO16(0x2A)
#define OCR1AL	_SFR_IO8(0x2A)
#define OCR1AH	_SFR_IO8(0x2B)

/* Timer/Counter1 */
#define TCNT1	_SFR_IO16(0x2C)
#define TCNT1L	_SFR_IO8(0x2C)
#define TCNT1H	_SFR_IO8(0x2D)

/* Timer/Counter1 Control Register B */
#define TCCR1B	_SFR_IO8(0x2E)

/* Timer/Counter1 Control Register A */
#define TCCR1A	_SFR_IO8(0x2F)

/* Special Function IO Register */
#define SFIOR	_SFR_IO8(0x30)

/* Timer/Counter0 Output Compare Register */
#define OCR0	_SFR_IO8(0x31)

/* Timer/Counter0 (8-bit) */
#define TCNT0	_SFR_IO8(0x32)

/* Timer/Counter0 Control Register */
#define TCCR0	_SFR_IO8(0x33)

/* 0x34 reserved */

/* MCU Control/Status Register */
#define MCUR	_SFR_IO8(0x35)

/* 2-wire Serial Control Register */
#define TWCR	_SFR_IO8(0x36)

/* 0x37 reserved */

/* Timer/Counter Interrupt Flag Register */
#define TIFR	_SFR_IO8(0x38)

/* Timer/Counter Interrupt MaSK Register */
#define TIMSK	_SFR_IO8(0x39)

/* Software Control Register */
#define SFTCR	_SFR_IO8(0x3A)

/* External Interrupt Mask/Flag Register */
#define EIMF	_SFR_IO8(0x3B)

/* 0x3C reserved */

/* 0x3D..0x3E SP */

/* 0x3F SREG */

/* Interrupt vectors */

#define SIG_FPGA_INTERRUPT0     _VECTOR(1)   /* FPGA_INT0  */        
#define SIG_INTERRUPT0          _VECTOR(2)   /* EXT_INT0   */
#define SIG_FPGA_INTERRUPT1     _VECTOR(3)   /* FPGA_INT1  */        
#define SIG_INTERRUPT1          _VECTOR(4)   /* EXT_INT1   */
#define SIG_FPGA_INTERRUPT2     _VECTOR(5)   /* FPGA_INT2  */        
#define SIG_INTERRUPT2          _VECTOR(6)   /* EXT_INT2   */
#define SIG_FPGA_INTERRUPT3     _VECTOR(7)   /* FPGA_INT3  */        
#define SIG_INTERRUPT3          _VECTOR(8)   /* EXT_INT3   */
#define SIG_OUTPUT_COMPARE2     _VECTOR(9)   /* TIM2_COMP  */        
#define SIG_OVERFLOW2           _VECTOR(10)  /* TIM2_OVF   */        
#define SIG_INPUT_CAPTURE1      _VECTOR(11)  /* TIM1_CAPT  */        
#define SIG_OUTPUT_COMPARE1A    _VECTOR(12)  /* TIM1_COMPA */        
#define SIG_OUTPUT_COMPARE1B    _VECTOR(13)  /* TIM1_COMPB */        
#define SIG_OVERFLOW1           _VECTOR(14)  /* TIM1_OVF   */        
#define SIG_OUTPUT_COMPARE0     _VECTOR(15)  /* TIM0_COMP  */        
#define SIG_OVERFLOW0           _VECTOR(16)  /* TIM0_OVF   */        
#define SIG_FPGA_INTERRUPT4     _VECTOR(17)  /* FPGA_INT4  */        
#define SIG_FPGA_INTERRUPT5     _VECTOR(18)  /* FPGA_INT5  */        
#define SIG_FPGA_INTERRUPT6     _VECTOR(19)  /* FPGA_INT6  */        
#define SIG_FPGA_INTERRUPT7     _VECTOR(20)  /* FPGA_INT7  */        
#define SIG_UART0_RECV          _VECTOR(21)  /* UART0_RXC  */
#define SIG_UART0_DATA          _VECTOR(22)  /* UART0_DRE  */
#define SIG_UART0_TRANS         _VECTOR(23)  /* UART0_TXC  */
#define SIG_FPGA_INTERRUPT8     _VECTOR(24)  /* FPGA_INT8  */        
#define SIG_FPGA_INTERRUPT9     _VECTOR(25)  /* FPGA_INT9  */        
#define SIG_FPGA_INTERRUPT10    _VECTOR(26)  /* FPGA_INT10 */        
#define SIG_FPGA_INTERRUPT11    _VECTOR(27)  /* FPGA_INT11 */        
#define SIG_UART1_RECV          _VECTOR(28)  /* UART1_RXC  */
#define SIG_UART1_DATA          _VECTOR(29)  /* UART1_DRE  */
#define SIG_UART1_TRANS         _VECTOR(30)  /* UART1_TXC  */
#define SIG_FPGA_INTERRUPT12    _VECTOR(31)  /* FPGA_INT12 */
#define SIG_FPGA_INTERRUPT13    _VECTOR(32)  /* FPGA_INT13 */
#define SIG_FPGA_INTERRUPT14    _VECTOR(33)  /* FPGA_INT14 */
#define SIG_FPGA_INTERRUPT15    _VECTOR(34)  /* FPGA_INT15 */
#define SIG_2WIRE_SERIAL        _VECTOR(35)  /* TWS_INT    */

#define _VECTORS_SIZE 144

/* Bit numbers (SFRs alphabetically sorted) */

/* ASSR */
#define AS2           3
#define TCN2UB        2
#define OCR2UB        1
#define TCR2UB        0

/* DDRD */
#define DDD7          7
#define DDD6          6
#define DDD5          5
#define DDD4          4
#define DDD3          3
#define DDD2          2
#define DDD1          1
#define DDD0          0

/* DDRE */
#define DDE7          7
#define DDE6          6
#define DDE5          5
#define DDE4          4
#define DDE3          3
#define DDE2          2
#define DDE1          1
#define DDE0          0

/* EIMF */
#define INTF3         7
#define INTF2         6
#define INTF1         5
#define INTF0         4
#define INT3          3
#define INT2          2
#define INT1          1
#define INT0          0

/* FISCR */
#define FIADR         7
#define XFIS1         1
#define XFIS0         0

/* FISUA */
#define FIF3          7
#define FIF2          6
#define FIF1          5
#define FIF0          4
#define FINT3         3
#define FINT2         2
#define FINT1         1
#define FINT0         0

/* FISUB */
#define FIF7          7
#define FIF6          6
#define FIF5          5
#define FIF4          4
#define FINT7         3
#define FINT6         2
#define FINT5         1
#define FINT4         0

/* FISUC */
#define FIF11         7
#define FIF10         6
#define FIF9          5
#define FIF8          4
#define FINT11        3
#define FINT10        2
#define FINT9         1
#define FINT8         0

/* FISUD */
#define FIF15         7
#define FIF14         6
#define FIF13         5
#define FIF12         4
#define FINT15        3
#define FINT14        2
#define FINT13        1
#define FINT12        0

/* MCUR */
#define JTRF          7
#define JTD           6
#define SE            5
#define SM1           4
#define SM0           3
#define PORF          2
#define WDRF          1
#define EXTRF         0

/* OCDR (reserved) */
#define IDRD          7

/* PIND */
#define PIND7         7
#define PIND6         6
#define PIND5         5
#define PIND4         4
#define PIND3         3
#define PIND2         2
#define PIND1         1
#define PIND0         0

/* PINE */
#define PINE7         7
#define PINE6         6
#define PINE5         5
#define PINE4         4
#define PINE3         3
#define PINE2         2
#define PINE1         1
#define PINE0         0

/* PORTD */
#define PD7        7
#define PD6        6
#define PD5        5
#define PD4        4
#define PD3        3
#define PD2        2
#define PD1        1
#define PD0        0

/* PORTE */
/*
   PE7 = IC1  / INT3 (alternate)
   PE6 = OC1A / INT2 (alternate)
   PE5 = OC1B / INT1 (alternate)
   PE4 = ET11 / INT0 (alternate)
   PE3 = OC2  / RX1  (alternate)
   PE2 =      / TX1  (alternate)
   PE1 = OC0  / RX0  (alternate)
   PE0 = ET0  / TX0  (alternate)
 */
#define PE7        7
#define PE6        6
#define PE5        5
#define PE4        4
#define PE3        3
#define PE2        2
#define PE1        1
#define PE0        0

/* SFIOR */
#define PSR2          1
#define PSR10         0

/* SFTCR */
#define FMXOR         3
#define WDTS          2
#define DBG           1
#define SRST          0

/* TCCR0 */
#define FOC0          7
#define PWM0          6
#define COM01         5
#define COM00         4
#define CTC0          3
#define CS02          2
#define CS01          1
#define CS00          0

/* TCCR1A */
#define COM1A1        7
#define COM1A0        6
#define COM1B1        5
#define COM1B0        4
#define FOC1A         3
#define FOC1B         2
#define PWM11         1
#define PWM10         0

/* TCCR1B */
#define ICNC1         7
#define ICES1         6
#define ICPE          5
#define CTC1          3
#define CS12          2
#define CS11          1
#define CS10          0

/* TCCR2 */
#define FOC2          7
#define PWM2          6
#define COM21         5
#define COM20         4
#define CTC2          3
#define CS22          2
#define CS21          1
#define CS20          0

/* TIFR */
#define TOV1          7
#define OCF1A         6
#define OCF1B         5
#define TOV2          4
#define ICF1          3
#define OCF2          2
#define TOV0          1
#define OCF0          0

/* TIMSK */
#define TOIE1         7
#define OCIE1A        6
#define OCIE1B        5
#define TOIE2         4
#define TICIE1        3
#define OCIE2         2
#define TOIE0         1
#define OCIE0         0

/* TWAR */
/* #define TWA           1 */ /* TWA is bits 7:1 */
#define TWGCE         0

/* TWCR */
#define TWINT         7
#define TWEA          6
#define TWSTA         5
#define TWSTO         4
#define TWWC          3
#define TWEN          2
#define TWIE          0

/* TWSR */
#define TWS7          7
#define TWS6          6
#define TWS5          5
#define TWS4          4
#define TWS3          3

/* UBRRHI
   Bits 11..8 of UART1 are bits 7..4 of UBRRHI.
   Bits 11..8 of UART0 are bits 3..0 of UBRRHI. */
/* #define UBRRHI1       4 */
/* #define UBRRHI0       0 */

/* UCSR0A */
#define RXC0          7
#define TXC0          6
#define UDRE0         5
#define FE0           4
#define OR0           3
#define U2X0          1
#define MPCM0         0

/* UCSR0B */
#define RXCIE0        7
#define TXCIE0        6
#define UDRIE0        5
#define RXEN0         4
#define TXEN0         3
#define CHR90         2
#define RXB80         1
#define TXB80         0

/* UCSR1A */
#define RXC1          7
#define TXC1          6
#define UDRE1         5
#define FE1           4
#define OR1           3
#define U2X1          1
#define MPCM1         0

/* UCSR1B */
#define RXCIE1        7
#define TXCIE1        6
#define UDRIE1        5
#define RXEN1         4
#define TXEN1         3
#define CHR91         2
#define RXB81         1
#define TXB81         0

/* WDTCR */
#define WDTOE         4
#define WDE           3
#define WDP2          2
#define WDP1          1
#define WDP0          0

/*
   Last memory addresses - depending on configuration, it is possible
   to have 20K-32K of program memory and 4K-16K of data memory
   (all in the same 36K total of SRAM, loaded from external EEPROM).
 */

#ifndef RAMEND
#define RAMEND 0x0FFF
#endif

#ifndef XRAMEND
#define XRAMEND RAMEND
#endif

#define E2END 0

#ifndef FLASHEND
#define FLASHEND 0x7FFF
#endif

#endif /* _AVR_IOAT94K_H_ */
