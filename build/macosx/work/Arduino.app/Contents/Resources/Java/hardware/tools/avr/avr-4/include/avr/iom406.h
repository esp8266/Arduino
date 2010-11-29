/* Copyright (c) 2006, Pieter Conradie
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

/* $Id: iom406.h,v 1.3.2.5 2008/10/17 23:27:49 arcanum Exp $ */

/* avr/iom406.h - definitions for ATmega406 */

#ifndef _AVR_IOM406_H_
#define _AVR_IOM406_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iom406.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

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

/* Reserved [0x06..0x07] */

#define PORTC   _SFR_IO8(0x08)
#define PC0     0

#define PIND    _SFR_IO8(0x09)
#define PIND1   1
#define PIND0   0

#define DDRD    _SFR_IO8(0x0A)
#define DDD1    1
#define DDD0    0

#define PORTD   _SFR_IO8(0x0B)
#define PD1     1
#define PD0     0

/* Reserved [0x0C..0x14] */

/* Timer/Counter0 Interrupt Flag Register */
#define TIFR0   _SFR_IO8(0x15)
#define OCF0B   2
#define OCF0A   1
#define TOV0    0

/* Timer/Counter1 Interrupt Flag Register */
#define TIFR1   _SFR_IO8(0x16)
#define OCF1A   1
#define TOV1    0

/* Reserved [0x17..0x1A] */

/* Pin Change Interrupt Control Register */
#define PCIFR   _SFR_IO8(0x1B)
#define PCIF1   1
#define PCIF0   0

/* External Interrupt Flag Register */
#define EIFR    _SFR_IO8(0x1C)
#define INTF3   3
#define INTF2   2
#define INTF1   1
#define INTF0   0

/* External Interrupt MaSK register */
#define EIMSK   _SFR_IO8(0x1D)
#define INT3    3
#define INT2    2
#define INT1    1
#define INT0    0

/* General Purpose I/O Register 0 */
#define GPIOR0  _SFR_IO8(0x1E)

/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1F)
#define EEPM1   5
#define EEPM0   4
#define EERIE   3
#define EEMPE   2
#define EEPE    1
#define EERE    0

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x20)

/* EEPROM Address Register */
#define EEAR	_SFR_IO16(0x21)
#define EEARL	_SFR_IO8(0x21)
#define EEARH	_SFR_IO8(0x22)

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

/* General Timer/Counter Control Register */
#define GTCCR	_SFR_IO8(0x23)
#define TSM     7
#define PSRSYNC 0

/* Timer/Counter Control Register A */
#define TCCR0A  _SFR_IO8(0x24)
#define COM0A1  7
#define COM0A0  6
#define COM0B1  5
#define COM0B0  4
#define WGM01   1
#define WGM00   0

/* Timer/Counter Control Register B */
#define TCCR0B  _SFR_IO8(0x25)
#define FOC0A   7
#define FOC0B   6
#define WGM02   3
#define CS02    2
#define CS01    1
#define CS00    0

/* Timer/Counter 0 */
#define TCNT0   _SFR_IO8(0x26)

/* Output Compare Register A */
#define OCR0A   _SFR_IO8(0x27)

/* Output Compare Register B */
#define OCR0B   _SFR_IO8(0x28)

/* Reserved [0x29] */

/* General Purpose I/O Register 1 */
#define GPIOR1  _SFR_IO8(0x2A)

/* General Purpose I/O Register 2 */
#define GPIOR2  _SFR_IO8(0x2B)

/* Reserved [0x2C..0x30] */

/* On-chip Debug Register */
#define OCDR    _SFR_IO8(0x31)

/* Reserved [0x32] */

/* Sleep Mode Control Register */
#define SMCR    _SFR_IO8(0x33)
#define SM2     3
#define SM1     2
#define SM0     1
#define SE      0

/* MCU Status Register */
#define MCUSR   _SFR_IO8(0x34)
#define JTRF    4
#define WDRF    3
#define BODRF   2
#define EXTRF   1
#define PORF    0

/* MCU general Control Register */
#define MCUCR   _SFR_IO8(0x35)
#define JTD     7
#define PUD     4
#define IVSEL   1
#define IVCE    0

/* Reserved [0x36] */

/* Store Program Memory Control and Status Register */
#define SPMCSR  _SFR_IO8(0x37)
#define SPMIE   7
#define RWWSB   6
#define SIGRD   5
#define RWWSRE  4
#define BLBSET  3
#define PGWRT   2
#define PGERS   1
#define SPMEN   0

/* Reserved [0x36..0x3C] */

/* 0x3D..0x3E SP */

/* 0x3F SREG */

/* Extended I/O registers */

/* Watchdog Timer Control Register */
#define WDTCSR  _SFR_MEM8(0x60)
#define WDIF    7
#define WDIE    6
#define WDP3    5
#define WDCE    4
#define WDE     3
#define WDP2    2
#define WDP1    1
#define WDP0    0

/* Reserved [0x61] */

/* Wake-up Timer Control and Status Register */
#define WUTCSR  _SFR_MEM8(0x62)
#define WUTIF   7
#define WUTIE   6
#define WUTCF   5
#define WUTR    4
#define WUTE    3
#define WUTP2   2
#define WUTP1   1
#define WUTP0   0

/* Reserved [0x63] */

/* Power Reduction Register 0 */
#define PRR0    _SFR_MEM8(0x64)
#define PRTWI   3
#define PRTIM1  2
#define PRTIM0  1
#define PRVADC  0

/* Reserved [0x65] */

/* Fast Oscillator Calibration Register */
#define FOSCCAL _SFR_MEM8(0x66)

/* Reserved [0x67] */

/* Pin Change Interrupt Control Register */
#define PCICR   _SFR_MEM8(0x68)
#define PCIE1   1
#define PCIE0   0

/* External Interrupt Control Register A */
#define EICRA   _SFR_MEM8(0x69)
#define ISC31   7
#define ISC30   6
#define ISC21   5
#define ISC20   4
#define ISC11   3
#define ISC10   2
#define ISC01   1
#define ISC00   0

/* Reserved [0x6A] */

/* Pin Change Mask Register 0 */
#define PCMSK0  _SFR_MEM8(0x6B)
#define PCINT7  7
#define PCINT6  6
#define PCINT5  5
#define PCINT4  4
#define PCINT3  3
#define PCINT2  2
#define PCINT1  1
#define PCINT0  0

/* Pin Change Mask Register 1 */
#define PCMSK1  _SFR_MEM8(0x6C)
#define PCINT15 7 
#define PCINT14 6 
#define PCINT13 5 
#define PCINT12 4 
#define PCINT11 3 
#define PCINT10 2 
#define PCINT9  1 
#define PCINT8  0

/* Reserved [0x6D] */

/* Timer/Counter Interrupt MaSK register 0 */
#define TIMSK0  _SFR_MEM8(0x6E)
#define OCIE0B  2
#define OCIE0A  1
#define TOIE0   0

/* Timer/Counter Interrupt MaSK register 1 */
#define TIMSK1  _SFR_MEM8(0x6F)
#define OCIE1A  1
#define TOIE1   0

/* Reserved [0x70..0x77] */

/* V-ADC Data Register */
#define VADC	_SFR_MEM16(0x78)
#define VADCL	_SFR_MEM8(0x78)
#define VADCH	_SFR_MEM8(0x79)

/* V-ADC Control and Status Register */
#define VADCSR	_SFR_MEM8(0x7A)
#define VADEN   3
#define VADSC   2
#define VADCCIF 1
#define VADCCIE 0

/* Reserved [0x7B] */

/* V-ADC Multiplexer Selection Register */
#define VADMUX	_SFR_MEM8(0x7C)
#define VADMUX3 3
#define VADMUX2 2
#define VADMUX1 1
#define VADMUX0 0

/* Reserved [0x7D] */

/* Digital Input Disable Register 0 */
#define DIDR0	_SFR_MEM8(0x7E)
#define VADC3D  3
#define VADC2D  2
#define VADC1D  1
#define VADC0D  0

/* Reserved [0x82..0x83] */

/* Timer/Counter 1 Control and Status Register */
#define TCCR1B  _SFR_MEM8(0x81)
#define CTC1    3
#define CS12    2
#define CS11    1
#define CS10    0

/* Reserved [0x82..0x83] */

/* Timer/Counter 1 */
#define TCNT1   _SFR_MEM16(0x84)
#define TCNT1L  _SFR_MEM8(0x84)
#define TCNT1H  _SFR_MEM8(0x85)

/* Reserved [0x86..0x87] */

/* Timer/Counter1 Output Compare Register A */
#define OCR1A   _SFR_MEM16(0x88)
#define OCR1AL  _SFR_MEM8(0x88)
#define OCR1AH  _SFR_MEM8(0x89)

/* Reserved [0x8A..0xB7] */

/* 2-wire Serial Interface Bit Rate Register */
#define TWBR    _SFR_MEM8(0xB8)

/* 2-wire Serial Interface Status Register */
#define TWSR    _SFR_MEM8(0xB9)
#define TWS7    7
#define TWS6    6
#define TWS5    5
#define TWS4    4
#define TWS3    3
#define TWPS1   1
#define TWPS0   0

/* 2-wire Serial Interface Address Register */
#define TWAR    _SFR_MEM8(0xBA)
#define TWA6    7
#define TWA5    6
#define TWA4    5
#define TWA3    4
#define TWA2    3
#define TWA1    2
#define TWA0    1
#define TWGCE   0

/* 2-wire Serial Interface Data Register */
#define TWDR    _SFR_MEM8(0xBB)

/* 2-wire Serial Interface Control Register */
#define TWCR    _SFR_MEM8(0xBC)
#define TWINT   7
#define TWEA    6
#define TWSTA   5
#define TWSTO   4
#define TWWC    3
#define TWEN    2
#define TWIE    0

/* 2-wire Serial (Slave) Address Mask Register */
#define TWAMR   _SFR_MEM8(0xBD)
#define TWAM6   7
#define TWAM5   6
#define TWAM4   5
#define TWAM3   4
#define TWAM2   3
#define TWAM1   2
#define TWAM0   1

/* 2-wire Serial Bus Control and Status Register */
#define TWBCSR  _SFR_MEM8(0xBE)
#define TWBCIF  7
#define TWBCIE  6
#define TWBDT1  2
#define TWBDT0  1
#define TWBCIP  0

/* Reserved [0xBF] */

/* Clock Control Status Register */
#define CCSR    _SFR_MEM8(0xC0)
#define XOE     1
#define ACS     0

/* Reserved [0xC1..0xCF] */

/* Bandgap Calibration C Register */
#define BGCCR   _SFR_MEM8(0xD0)
#define BGEN    7
#define BGCC5   5
#define BGCC4   4
#define BGCC3   3
#define BGCC2   2
#define BGCC1   1
#define BGCC0   0

/* Bandgap Calibration R Register */
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

/* CC-ADC Control and Status Register A */
#define CADCSRA _SFR_MEM8(0xE4)
#define CADEN   7
#define CADUB   5
#define CADAS1  4
#define CADAS0  3
#define CADSI1  2
#define CADSI0  1
#define CADSE   0

/* CC-ADC Control and Status Register B */
#define CADCSRB _SFR_MEM8(0xE5)
#define CADACIE 6
#define CADRCIE 5
#define CADICIE 4
#define CADACIF 2
#define CADRCIF 1
#define CADICIF 0

/* CC-ADC Regular Charge Current */
#define CADRCC  _SFR_MEM8(0xE6)

/* CC-ADC Regular Discharge Current */
#define CADRDC  _SFR_MEM8(0xE7)

/* CC-ADC Instantaneous Current */
#define CADIC   _SFR_MEM16(0xE8)
#define CADICL  _SFR_MEM8(0xE8)
#define CADICH  _SFR_MEM8(0xE9)

/* Reserved [0xEA..0xEF] */

/* FET Control and Status Register */
#define FCSR    _SFR_MEM8(0xF0)
#define PWMOC   5
#define PWMOPC  4
#define CPS     3
#define DFE     2
#define CFE     1
#define PFD     0

/* Cell Balancing Control Register */
#define CBCR    _SFR_MEM8(0xF1)
#define CBE4    3
#define CBE3    2
#define CBE2    1
#define CBE1    0

/* Battery Protection Interrupt Register */
#define BPIR    _SFR_MEM8(0xF2)
#define DUVIF   7
#define COCIF   6
#define DOCIF   5
#define SCIF    4
#define DUVIE   3
#define COCIE   2
#define DOCIE   1
#define SCIE    0

/* Battery Protection Deep Under Voltage Register */
#define BPDUV   _SFR_MEM8(0xF3)
#define DUVT1   5
#define DUVT0   4
#define DUDL3   3
#define DUDL2   2
#define DUDL1   1
#define DUDL0   0

/* Battery Protection Short-circuit Detection Level Register */
#define BPSCD   _SFR_MEM8(0xF4)
#define SCDL3   3
#define SCDL2   2
#define SCDL1   1
#define SCDL0   0

/* Battery Protection Over-current Detection Level Register */
#define BPOCD   _SFR_MEM8(0xF5)
#define DCDL3   7
#define DCDL2   6
#define DCDL1   5
#define DCDL0   4
#define CCDL3   3
#define CCDL2   2
#define CCDL1   1
#define CCDL0   0

/* Current Battery Protection Timing Register */
#define CBPTR   _SFR_MEM8(0xF6)
#define SCPT3   7
#define SCPT2   6
#define SCPT1   5
#define SCPT0   4
#define OCPT3   3
#define OCPT2   2
#define OCPT1   1
#define OCPT0   0

/* Battery Protection Control Register */
#define BPCR    _SFR_MEM8(0xF7)
#define DUVD    3
#define SCD     2
#define DCD     1
#define CCD     0

/* Battery Protection Parameter Lock Register */
#define BPPLR   _SFR_MEM8(0xF8)
#define BPPLE   1
#define BPPL    0

/* Reserved [0xF9..0xFF] */

/* Interrupt vectors */
/* Battery Protection Interrupt */
#define BPINT_vect			_VECTOR(1)


/* External Interrupt Request 0 */
#define INT0_vect			_VECTOR(2)


/* External Interrupt Request 1 */
#define INT1_vect			_VECTOR(3)


/* External Interrupt Request 2 */
#define INT2_vect			_VECTOR(4)


/* External Interrupt Request 3 */
#define INT3_vect			_VECTOR(5)


/* Pin Change Interrupt 0 */
#define PCINT0_vect			_VECTOR(6)


/* Pin Change Interrupt 1 */
#define PCINT1_vect			_VECTOR(7)


/* Watchdog Timeout Interrupt */
#define WDT_vect			_VECTOR(8)


/* Wakeup timer overflow */
#define WAKE_UP_vect			_VECTOR(9)


/* Timer/Counter 1 Compare Match */
#define TIM1_COMP_vect			_VECTOR(10)


/* Timer/Counter 1 Overflow */
#define TIM1_OVF_vect			_VECTOR(11)


/* Timer/Counter0 Compare A Match */
#define TIM0_COMPA_vect			_VECTOR(12)


/* Timer/Counter0 Compare B Match */
#define TIM0_COMPB_vect			_VECTOR(13)


/* Timer/Counter0 Overflow */
#define TIM0_OVF_vect			_VECTOR(14)


/* Two-Wire Bus Connect/Disconnect */
#define TWI_BUS_CD_vect			_VECTOR(15)


/* Two-Wire Serial Interface */
#define TWI_vect			_VECTOR(16)


/* Voltage ADC Conversion Complete */
#define VADC_vect			_VECTOR(17)


/* Coulomb Counter ADC Conversion Complete */
#define CCADC_CONV_vect			_VECTOR(18)

/* Coloumb Counter ADC Regular Current */
#define CCADC_REG_CUR_vect		_VECTOR(19)


/* Coloumb Counter ADC Accumulator */
#define CCADC_ACC_vect			_VECTOR(20)


/* EEPROM Ready */
#define EE_READY_vect			_VECTOR(21)


/* Store Program Memory Ready */
#define SPM_READY_vect			_VECTOR(22)

#define _VECTORS_SIZE 92

/* Constants */
#define SPM_PAGESIZE 128
#define RAMEND       0x8FF
#define XRAMEND      0x8FF
#define E2END        0x1FF
#define E2PAGESIZE   4
#define FLASHEND     0x9FFF


/* Fuses */

#define FUSE_MEMORY_SIZE 2

/* Low Fuse Byte */
#define FUSE_CKSEL   (unsigned char)~_BV(0)
#define FUSE_SUT0    (unsigned char)~_BV(1)
#define FUSE_SUT1    (unsigned char)~_BV(2)
#define FUSE_BOOTRST (unsigned char)~_BV(3)
#define FUSE_BOOTSZ0 (unsigned char)~_BV(4)
#define FUSE_BOOTSZ1 (unsigned char)~_BV(5)
#define FUSE_EESAVE  (unsigned char)~_BV(6)
#define FUSE_WDTON   (unsigned char)~_BV(7)
#define LFUSE_DEFAULT (FUSE_SUT0 & FUSE_BOOTSZ0 & FUSE_BOOTSZ1)

/* High Fuse Byte */
#define FUSE_JTAGEN      (unsigned char)~_BV(0)
#define FUSE_OCDEN       (unsigned char)~_BV(1)
#define HFUSE_DEFAULT (FUSE_JTAGEN)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x95
#define SIGNATURE_2 0x07


#endif /* _AVR_IOM406_H_ */
