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

/* $Id: io76c711.h,v 1.5 2004/11/01 21:19:54 arcanum Exp $ */

/* avr/io76c711.h - definitions for AT76C711 */

#ifndef _AVR_IO76C711_H_
#define _AVR_IO76C711_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "io76c711.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* 0x00-0x0C reserved */

/* SPI */
#define SPCR	_SFR_IO8(0x0D)
#define SPSR	_SFR_IO8(0x0E)
#define SPDR	_SFR_IO8(0x0F)

/* Port D */
#define PIND	_SFR_IO8(0x10)
#define DDRD	_SFR_IO8(0x11)
#define PORTD	_SFR_IO8(0x12)

/* Peripheral Enable Register */
#define PERIPHEN _SFR_IO8(0x13)

/* Clock Control Register */
#define CLK_CNTR _SFR_IO8(0x14)

/* Data Register, Port C */
#define PORTC	_SFR_IO8(0x15)

/* Port B */
#define PINB	_SFR_IO8(0x16)
#define DDRB	_SFR_IO8(0x17)
#define PORTB	_SFR_IO8(0x18)

/* Port A */
#define PINA	_SFR_IO8(0x19)
#define DDRA	_SFR_IO8(0x1A)
#define PORTA	_SFR_IO8(0x1B)

/* 0x1C-0x1F reserved */

#define IRDAMOD	_SFR_IO8(0x20)

#define WDTCR	_SFR_IO8(0x21)

/* 0x22-0x25 reserved */
/* Timer 1 */
#define ICR1	_SFR_IO16(0x26)
#define ICR1L	_SFR_IO8(0x26)
#define ICR1H	_SFR_IO8(0x27)
#define OCR1B	_SFR_IO16(0x28)
#define OCR1BL	_SFR_IO8(0x28)
#define OCR1BH	_SFR_IO8(0x29)
#define OCR1A	_SFR_IO16(0x2A)
#define OCR1AL	_SFR_IO8(0x2A)
#define OCR1AH	_SFR_IO8(0x2B)
#define TCNT1	_SFR_IO16(0x2C)
#define TCNT1L	_SFR_IO8(0x2C)
#define TCNT1H	_SFR_IO8(0x2D)
#define TCCR1B	_SFR_IO8(0x2E)
#define TCCR1A	_SFR_IO8(0x2F)

/* 0x30 reserved */

/* Timer 0 */
#define PRELD	_SFR_IO8(0x31)
#define TCNT0	_SFR_IO8(0x32)
#define TCCR0	_SFR_IO8(0x33)

#define MCUSR	_SFR_IO8(0x34)
#define MCUCR	_SFR_IO8(0x35)

#define TIFR	_SFR_IO8(0x36)
#define TIMSK	_SFR_IO8(0x37)

/* 0x38 reserved */

#define EIMSK	_SFR_IO8(0x39)

/* 0x3A-0x3C reserved */

/* 0x3D..0x3E SP */

/* 0x3F SREG */

/* Interrupt vectors */

#define SIG_SUSPEND_RESUME	_VECTOR(1)
#define SIG_INTERRUPT0		_VECTOR(2)
#define SIG_INPUT_CAPTURE1	_VECTOR(3)
#define SIG_OUTPUT_COMPARE1A	_VECTOR(4)
#define SIG_OUTPUT_COMPARE1B	_VECTOR(5)
#define SIG_OVERFLOW1		_VECTOR(6)
#define SIG_OVERFLOW0		_VECTOR(7)
#define SIG_SPI			_VECTOR(8)
#define SIG_TDMAC		_VECTOR(9)
#define SIG_UART0		_VECTOR(10)
#define SIG_RDMAC		_VECTOR(11)
#define SIG_USB_HW		_VECTOR(12)
#define SIG_UART1		_VECTOR(13)
#define SIG_INTERRUPT1		_VECTOR(14)

#define _VECTORS_SIZE 60

/* Bit numbers */

/* EIMSK */
/* bits 7-4 reserved */
#define POL1	3
#define POL0	2
#define INT1	1
#define INT0	0

/* TIMSK */
#define TOIE1	7
#define OCIE1A	6
#define OCIE1B	5
/* bit 4 reserved */
#define TICIE1	3
/* bit 2 reserved */
#define TOIE0	1
/* bit 0 reserved */

/* TIFR */
#define TOV1	7
#define OCF1A	6
#define OCF1B	5
/* bit 4 reserved */
#define ICF1	3
/* bit 2 reserved */
#define TOV0	1
/* bit 0 reserved */

/* MCUCR */
/* bits 7-6 reserved */
#define SE	5
#define SM1	4
#define SM0	3
/* bits 2-0 reserved */

/* MCUSR */
/* bits 7-2 reserved */
#define EXTRF	1
#define PORF	0

/* TCCR0 */
/* bits 7-6 reserved */
#define COM01	5
#define COM00	4
#define CTC0	3
#define CS02	2
#define CS01	1
#define CS00	0

/* TCCR1A */
#define COM1A1	7
#define COM1A0	6
#define COM1B1	5
#define COM1B0	4
/* bits 3-0 reserved */

/* TCCR1B */
#define ICNC1	7
#define ICES1	6
/* bits 5-4 reserved */
#define CTC1	3
#define CS12	2
#define CS11	1
#define CS10	0

/* WDTCR */
/* bits 7-5 reserved */
#define WDTOE	4
#define WDE	3
#define WDP2	2
#define WDP1	1
#define WDP0	0

/* IRDAMOD */
/* bits 7-3 reserved */
#define POL	2
#define MODE	1
#define EN	0

/* PORTA */
#define PA7	7
#define PA6	6
#define PA5	5
#define PA4	4
#define PA3	3
#define PA2	2
#define PA1	1
#define PA0	0

/* DDRA */
#define DDA7	7
#define DDA6	6
#define DDA5	5
#define DDA4	4
#define DDA3	3
#define DDA2	2
#define DDA1	1
#define DDA0	0

/* PINA */
#define PINA7	7
#define PINA6	6
#define PINA5	5
#define PINA4	4
#define PINA3	3
#define PINA2	2
#define PINA1	1
#define PINA0	0

/*
   PB7 = SCK
   PB6 = MISO
   PB5 = MOSI
   PB4 = SS#
   PB2 = ICP
   PB1 = T1
   PB0 = T0
 */

/* PORTB */
#define PB7	7
#define PB6	6
#define PB5	5
#define PB4	4
#define PB3	3
#define PB2	2
#define PB1	1
#define PB0	0

/* DDRB */
#define DDB7	7
#define DDB6	6
#define DDB5	5
#define DDB4	4
#define DDB3	3
#define DDB2	2
#define DDB1	1
#define DDB0	0

/* PINB */
#define PINB7	7
#define PINB6	6
#define PINB5	5
#define PINB4	4
#define PINB3	3
#define PINB2	2
#define PINB1	1
#define PINB0	0

/* PORTC */
/* bits 7-4 reserved */
#define PC3	 3
#define PC2	 2
#define PC1	 1
#define PC0	 0

/*
   PD7 = INT1 / OC1B
   PD6 = INT0 / OC1A
   PD1 = TXD
   PD0 = RXD
 */

/* PORTD */
#define PD7	 7
#define PD6	 6
#define PD5	 5
#define PD4	 4
#define PD3	 3
#define PD2	 2
#define PD1	 1
#define PD0	 0

/* DDRD */
#define DDD7	7
#define DDD6	6
#define DDD5	5
#define DDD4	4
#define DDD3	3
#define DDD2	2
#define DDD1	1
#define DDD0	0

/* PIND */
#define PIND7	7
#define PIND6	6
#define PIND5	5
#define PIND4	4
#define PIND3	3
#define PIND2	2
#define PIND1	1
#define PIND0	0

/* CLK_CNTR */
/* bits 7-5 reserved */
#define UOSC	4
#define UCK	3
#define IRCK	2
/* bits 1-0 reserved */

/* PERIPHEN */
/* bits 7-3 reserved */
#define IRDA	2
#define UART	1
#define USB	0

/* SPSR */
#define SPIF	7
#define WCOL	6
/* bits 5-0 reserved */

/* SPCR */
#define SPIE	7
#define SPE	6
#define DORD	5
#define MSTR	4
#define CPOL	3
#define CPHA	2
#define SPR1	1
#define SPR0	0

/* Memory mapped registers (XXX - not yet changed to use _SFR_MEM8() macros) */

/* UART */
#define UART0_BASE 0x2020
#define UART1_BASE 0x2030
/* offsets from the base address */
#define US_RHR		0x00
#define US_THR		0x00
#define US_IER		0x01
#define US_FCR		0x02
#define US_PMR		0x03
#define US_MR		0x04
#define US_CSR		0x05
#define US_CR		0x06
#define US_BL		0x07
#define US_BM		0x08
#define US_RTO		0x09
#define US_TTG		0x0A

/* DMA */
#define DMA_BASE 0x2000
/* offsets from the base address */
#define TXTADL		0x01
#define TXPLL		0x03
#define TXPLM		0x04
#define TXTPLL		0x05
#define TXTPLM		0x06
#define RXTADL		0x07
#define RXTADMEN	0x08
#define RSPLL		0x09
#define RXPLM		0x0A
#define RXTPLL		0x0B
#define RXTPLM		0x0C
#define INTCST		0x0D
/* XXX DPORG register mentioned on page 20, but undocumented */

/* XXX Program Memory Control Bit mentioned on page 20, but undocumented */
#define PROGRAM_MEMORY_CONTROL_BIT 0x2040

/* USB */
#define USB_BASE 0x1000
/* offsets from the base address */
#define FRM_NUM_H	0x0FD
#define FRM_NUM_L	0x0FC
#define GLB_STATE	0x0FB
#define SPRSR		0x0FA
#define SPRSIE		0x0F9
#define UISR		0x0F7
#define UIAR		0x0F5
#define FADDR		0x0F2
#define ENDPPGPG	0x0F1
#define ECR0		0x0EF
#define ECR1		0x0EE
#define ECR2		0x0ED
#define ECR3		0x0EC
#define ECR4		0x0EB
#define ECR5		0x0EA
#define ECR6		0x0E9
#define ECR7		0x0E8
#define CSR0		0x0DF
#define CSR1		0x0DE
#define CSR2		0x0DD
#define CSR3		0x0DC
#define CSR4		0x0DB
#define CSR5		0x0DA
#define CSR6		0x0D9
#define CSR7		0x0D8
#define FDR0		0x0CF
#define FDR1		0x0CE
#define FDR2		0x0CD
#define FDR3		0x0CC
#define FDR4		0x0CB
#define FDR5		0x0CA
#define FDR6		0x0C9
#define FDR7		0x0C8
#define FBYTE_CNT0_L	0x0BF
#define FBYTE_CNT1_L	0x0BE
#define FBYTE_CNT2_L	0x0BD
#define FBYTE_CNT3_L	0x0BC
#define FBYTE_CNT4_L	0x0BB
#define FBYTE_CNT5_L	0x0BA
#define FBYTE_CNT6_L	0x0B9
#define FBYTE_CNT7_L	0x0B8
#define FBYTE_CNT0_H	0x0AF
#define FBYTE_CNT1_H	0x0AE
#define FBYTE_CNT2_H	0x0AD
#define FBYTE_CNT3_H	0x0AC
#define FBYTE_CNT4_H	0x0AB
#define FBYTE_CNT5_H	0x0AA
#define FBYTE_CNT6_H	0x0A9
#define FBYTE_CNT7_H	0x0A8
#define SLP_MD_EN	0x100
#define IRQ_EN		0x101
#define IRQ_STAT	0x102
#define SUSP_WUP	0x103
#define PA_EN		0x104
#define USB_DMA_ADL	0x105
#define USB_DMA_ADH	0x106
#define USB_DMA_PLR	0x107
#define USB_DMA_EAD	0x108
#define USB_DMA_PLT	0x109
#define USB_DMA_EN	0x10A

/* Last memory addresses */
#define RAMEND		0x07FF
#define XRAMEND		0x07FF
#define E2END		0
#define FLASHEND	0x3FFF

/*
   AT76C711 data space memory map (ranges not listed are reserved):
   0x0000 - 0x001F - AVR registers
   0x0020 - 0x005F - AVR I/O space
   0x0060 - 0x07FF - AVR data SRAM
   0x1000 - 0x1FFF - USB (not all locations used)
   0x2000 - 0x201F - DMA controller
   0x2020 - 0x202F - UART0
   0x2030 - 0x203F - UART1 (IRDA)
   0x2040          - the mysterious Program Memory Control bit (???)
   0x3000 - 0x37FF - DPRAM
   0x8000 - 0xBFFF - program SRAM (read/write), would be nice if other
                     AVR devices did that as well (no need to use LPM!)
 */
#endif /* _AVR_IO76C711_H_ */
