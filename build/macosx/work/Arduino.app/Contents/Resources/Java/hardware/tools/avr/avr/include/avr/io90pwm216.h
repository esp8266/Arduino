/* Copyright (c) 2007, Atmel Corporation
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

/* $Id: io90pwm216.h,v 1.7.2.6 2008/10/17 23:27:45 arcanum Exp $ */

/* avr/io90pwm216.h - definitions for AT90PWM216 */

#ifndef _AVR_IO90PWM216_H_
#define _AVR_IO90PWM216_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "io90pwm216.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 

/* I/O registers */

/* Port B Input Pins Address */
#define PINB    _SFR_IO8(0x03)
#define PINB0   0
#define PINB1   1
#define PINB2   2
#define PINB3   3
#define PINB4   4
#define PINB5   5
#define PINB6   6
#define PINB7   7

/* Port B Data Direction Register */
#define DDRB    _SFR_IO8(0x04)
#define DDB0    0
#define DDB1    1
#define DDB2    2
#define DDB3    3
#define DDB4    4
#define DDB5    5
#define DDB6    6
#define DDB7    7

/* Port B Data Register */
#define PORTB   _SFR_IO8(0x05)
#define PB0     0
#define PB1     1
#define PB2     2
#define PB3     3
#define PB4     4
#define PB5     5
#define PB6     6
#define PB7     7

/* Port C Input Pins Address */
#define PINC    _SFR_IO8(0x06)
#define PINC0   0
#define PINC1   1
#define PINC2   2
#define PINC3   3
#define PINC4   4
#define PINC5   5
#define PINC6   6
#define PINC7   7

/* Port C Data Direction Register */
#define DDRC    _SFR_IO8(0x07)
#define DDC0    0
#define DDC1    1
#define DDC2    2
#define DDC3    3
#define DDC4    4
#define DDC5    5
#define DDC6    6
#define DDC7    7

/* Port C Data Register */
#define PORTC   _SFR_IO8(0x08)
#define PC0     0
#define PC1     1
#define PC2     2
#define PC3     3
#define PC4     4
#define PC5     5
#define PC6     6
#define PC7     7

/* Port D Input Pins Address */
#define PIND    _SFR_IO8(0x09)
#define PIND0   0
#define PIND1   1
#define PIND2   2
#define PIND3   3
#define PIND4   4
#define PIND5   5
#define PIND6   6
#define PIND7   7

/* Port D Data Direction Register */
#define DDRD    _SFR_IO8(0x0A)
#define DDD0    0
#define DDD1    1
#define DDD2    2
#define DDD3    3
#define DDD4    4
#define DDD5    5
#define DDD6    6
#define DDD7    7

/* Port D Data Register */
#define PORTD   _SFR_IO8(0x0B)
#define PD0     0
#define PD1     1
#define PD2     2
#define PD3     3
#define PD4     4
#define PD5     5
#define PD6     6
#define PD7     7

/* Port E Input Pins Address */
#define PINE    _SFR_IO8(0x0C)
#define PINE0   0
#define PINE1   1
#define PINE2   2

/* Port E Data Direction Register */
#define DDRE    _SFR_IO8(0x0D)
#define DDE0    0
#define DDE1    1
#define DDE2    2

/* Port E Data Register */
#define PORTE   _SFR_IO8(0x0E)
#define PE0     0
#define PE1     1
#define PE2     2

/* Timer/Counter 0 Interrupt Flag Register */
#define TIFR0   _SFR_IO8(0x15)
#define TOV0    0   /* Overflow Flag */
#define OCF0A   1   /* Output Compare Flag 0A */
#define OCF0B   2   /* Output Compare Flag 0B */

/* Timer/Counter1 Interrupt Flag Register */
#define TIFR1   _SFR_IO8(0x16)
#define TOV1    0   /* Overflow Flag */
#define OCF1A   1   /* Output Compare Flag 1A*/
#define OCF1B   2   /* Output Compare Flag 1B*/
#define ICF1    5   /* Input Capture Flag 1 */

/* General Purpose I/O Register 1 */
#define GPIOR1  _SFR_IO8(0x19)
#define GPIOR10 0
#define GPIOR11 1
#define GPIOR12 2
#define GPIOR13 3
#define GPIOR14 4
#define GPIOR15 5
#define GPIOR16 6
#define GPIOR17 7

/* General Purpose I/O Register 2 */
#define GPIOR2  _SFR_IO8(0x1A)
#define GPIOR20 0
#define GPIOR21 1
#define GPIOR22 2
#define GPIOR23 3
#define GPIOR24 4
#define GPIOR25 5
#define GPIOR26 6
#define GPIOR27 7

/* General Purpose I/O Register 3 */
#define GPIOR3  _SFR_IO8(0x1B)
#define GPIOR30 0
#define GPIOR31 1
#define GPIOR32 2
#define GPIOR33 3
#define GPIOR34 4
#define GPIOR35 5
#define GPIOR36 6
#define GPIOR37 7

/* External Interrupt Flag Register */
#define EIFR    _SFR_IO8(0x1C)
#define INTF0   0
#define INTF1   1
#define INTF2   2
#define INTF3   3

/* External Interrupt Mask Register */
#define EIMSK   _SFR_IO8(0x1D)
#define INT0    0   /* External Interrupt Request 0 Enable */
#define INT1    1   /* External Interrupt Request 1 Enable */
#define INT2    2   /* External Interrupt Request 2 Enable */
#define INT3    3   /* External Interrupt Request 3 Enable */

/* General Purpose I/O Register 0 */
#define GPIOR0  _SFR_IO8(0x1E)
#define GPIOR00 0
#define GPIOR01 1
#define GPIOR02 2
#define GPIOR03 3
#define GPIOR04 4
#define GPIOR05 5
#define GPIOR06 6
#define GPIOR07 7

/* EEPROM Control Register */
#define EECR    _SFR_IO8(0x1F)
#define EERE    0   /* EEPROM Read Enable */
#define EEWE    1   /* EEPROM Write Enable */
#define EEMWE   2   /* EEPROM Master Write Enable */
#define EERIE   3   /* EEPROM Ready Interrupt Enable */

/* EEPROM Data Register */
#define EEDR    _SFR_IO8(0x20)
#define EEDR0   0
#define EEDR1   1
#define EEDR2   2
#define EEDR3   3
#define EEDR4   4
#define EEDR5   5
#define EEDR6   6
#define EEDR7   7

/* The EEPROM Address Registers */
#define EEAR    _SFR_IO16(0x21)
#define EEARL   _SFR_IO8(0x21)
#define EEAR0   0
#define EEAR1   1
#define EEAR2   2
#define EEAR3   3
#define EEAR4   4
#define EEAR5   5
#define EEAR6   6
#define EEAR7   7
#define EEARH   _SFR_IO8(0x22)
#define EEAR8   0
#define EEAR9   1
#define EEAR10  2
#define EEAR11  3

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

/* General Timer/Counter Control Register */
#define GTCCR   _SFR_IO8(0x23)
#define PSR10   0   /* Prescaler Reset Timer/Counter1 and Timer/Counter0 */
#define ICPSEL1 6   /* Timer1 Input Capture Selection Bit */
#define TSM     7   /* Timer/Counter Synchronization Mode */

/* Timer/Counter Control Register A */
#define TCCR0A  _SFR_IO8(0x24)
#define WGM00   0   /* Waveform Generation Mode */
#define WGM01   1   /* Waveform Generation Mode */
#define COM0B0  4   /* Compare Output Mode, Fast PWm */
#define COM0B1  5   /* Compare Output Mode, Fast PWm */
#define COM0A0  6   /* Compare Output Mode, Phase Correct PWM Mode */
#define COM0A1  7   /* Compare Output Mode, Phase Correct PWM Mode */

/* Timer/Counter Control Register B */
#define TCCR0B  _SFR_IO8(0x25)
#define CS00    0   /* Clock Select */
#define CS01    1   /* Clock Select */
#define CS02    2   /* Clock Select */
#define WGM02   3   /* Waveform Generation Mode */
#define FOC0B   6   /* Force Output Compare B */
#define FOC0A   7   /* Force Output Compare A */

/* Timer/Counter0 Register */
#define TCNT0   _SFR_IO8(0x26)
#define TCNT00  0
#define TCNT01  1
#define TCNT02  2
#define TCNT03  3
#define TCNT04  4
#define TCNT05  5
#define TCNT06  6
#define TCNT07  7

/* Timer/Counter0 Output Compare Register A */
#define OCR0A   _SFR_IO8(0x27)
#define OCR0A0  0
#define OCR0A1  1
#define OCR0A2  2
#define OCR0A3  3
#define OCR0A4  4
#define OCR0A5  5
#define OCR0A6  6
#define OCR0A7  7

/* Timer/Counter0 Output Compare Register B */
#define OCR0B   _SFR_IO8(0x28)
#define OCR0B0  0
#define OCR0B1  1
#define OCR0B2  2
#define OCR0B3  3
#define OCR0B4  4
#define OCR0B5  5
#define OCR0B6  6
#define OCR0B7  7

/* PLL Control and Status Register */
#define PLLCSR  _SFR_IO8(0x29)
#define PLOCK   0   /* PLL Lock Detector */
#define PLLE    1   /* PLL Enable */
#define PLLF    2   /* PLL Factor */

/* SPI Control Register */
#define SPCR    _SFR_IO8(0x2C)
#define SPR0    0   /* SPI Clock Rate Select 0 */
#define SPR1    1   /* SPI Clock Rate Select 1 */
#define CPHA    2   /* Clock Phase */
#define CPOL    3   /* Clock polarity */
#define MSTR    4   /* Master/Slave Select */
#define DORD    5   /* Data Order */
#define SPE     6   /* SPI Enable */
#define SPIE    7   /* SPI Interrupt Enable */

/* SPI Status Register */
#define SPSR    _SFR_IO8(0x2D)
#define SPI2X   0   /* Double SPI Speed Bit */
#define WCOL    6   /* Write Collision Flag */
#define SPIF    7   /* SPI Interrupt Flag */

/* SPI Data Register */
#define SPDR    _SFR_IO8(0x2E)
#define SPD0    0
#define SPD1    1
#define SPD2    2
#define SPD3    3
#define SPD4    4
#define SPD5    5
#define SPD6    6
#define SPD7    7

/* Analog Comparator Status Register */
#define ACSR    _SFR_IO8(0x30)
#define AC0O    0   /* Analog Comparator 0 Output Bit */
#define AC1O    1   /* Analog Comparator 1 Output Bit */
#define AC2O    2   /* Analog Comparator 2 Output Bit */
#define AC0IF   4   /* Analog Comparator 0 Interrupt Flag Bit */
#define AC1IF   5   /* Analog Comparator 1 Interrupt Flag Bit */
#define AC2IF   6   /* Analog Comparator 2 Interrupt Flag Bit */
#define ACCKDIV 7   /* Analog Comparator Clock Divider */

/* Sleep Mode Control Register */
#define SMCR    _SFR_IO8(0x33)
#define SE      0   /* Sleep Enable */
#define SM0     1   /* Sleep Mode Select bit0 */
#define SM1     2   /* Sleep Mode Select bit1 */
#define SM2     3   /* Sleep Mode Select bit2 */

/* MCU Status Register */
#define MCUSR   _SFR_IO8(0x34)
#define PORF    0   /* Power-on reset flag */
#define EXTRF   1   /* External Reset Flag */
#define BORF    2   /* Brown-out Reset Flag */
#define WDRF    3   /* Watchdog Reset Flag */

/* MCU Control Register */
#define MCUCR   _SFR_IO8(0x35)
#define IVCE    0   /* Interrupt Vector Change Enable */
#define IVSEL   1   /* Interrupt Vector Select */
#define PUD     4   /* Pull-up disable */
#define SPIPS   7   /* SPI Pin Select */

/* Store Program Memory Control Register */
#define SPMCSR  _SFR_IO8(0x37)
#define SPMEN   0   /* Store Program Memory Enable */
#define PGERS   1   /* Page Erase */
#define PGWRT   2   /* Page Write */
#define BLBSET  3   /* Boot Lock Bit Set */
#define RWWSRE  4   /* Read While Write section read enable */
#define RWWSB   6   /* Read While Write Section Busy */
#define SPMIE   7   /* SPM Interrupt Enable */

/* Watchdog Timer Control Register */
#define WDTCSR  _SFR_MEM8(0x60)
#define WDP0    0   /* Watchdog Timer Prescaler bit0 */
#define WDP1    1   /* Watchdog Timer Prescaler bit1 */
#define WDP2    2   /* Watchdog Timer Prescaler bit2 */
#define WDE     3   /* Watchdog Enable */
#define WDCE    4   /* Watchdog Change Enable */
#define WDP3    5   /* Watchdog Timer Prescaler bit3 */
#define WDIE    6   /* Watchdog Timeout Interrupt Enable */
#define WDIF    7   /* Watchdog Timeout Interrupt Flag */

/* Clock Prescaler Register */
#define CLKPR   _SFR_MEM8(0x61)
#define CLKPS0  0   /* Clock Prescaler Select bit0 */
#define CLKPS1  1   /* Clock Prescaler Select bit1 */
#define CLKPS2  2   /* Clock Prescaler Select bit2 */
#define CLKPS3  3   /* Clock Prescaler Select bit3 */
#define CLKPCE  7   /* Clock Prescaler Change Enable */

/* Power Reduction Register */
#define PRR     _SFR_MEM8(0x64)
#define PRADC   0   /* Power Reduction ADC */
#define PRUSART 1   /* Power Reduction USART */
#define PRSPI   2   /* Power Reduction Serial Peripheral Interface */
#define PRTIM0  3   /* Power Reduction Timer/Counter0 */
#define PRTIM1  4   /* Power Reduction Timer/Counter1 */
#define PRPSC0  5   /* Power Reduction PSC0 */
#define PRPSC1  6   /* Power Reduction PSC1 */
#define PRPSC2  7   /* Power Reduction PSC2 */

/* Oscillator Calibration Value */
#define OSCCAL  _SFR_MEM8(0x66)
#define CAL0    0
#define CAL1    1
#define CAL2    2
#define CAL3    3
#define CAL4    4
#define CAL5    5
#define CAL6    6

/* External Interrupt Control Register A */
#define EICRA   _SFR_MEM8(0x69)
#define ISC00   0
#define ISC01   1
#define ISC10   2
#define ISC11   3
#define ISC20   4
#define ISC21   5
#define ISC30   6
#define ISC31   7

/* Timer/Counter0 Interrupt Mask Register */
#define TIMSK0  _SFR_MEM8(0x6E)
#define TOIE0   0   /* Overflow Interrupt Enable */
#define OCIE0A  1   /* Output Compare Match A Interrupt Enable */
#define OCIE0B  2   /* Output Compare Match B Interrupt Enable */

/* Timer/Counter1 Interrupt Mask Register */
#define TIMSK1  _SFR_MEM8(0x6F)
#define TOIE1   0   /* Overflow Interrupt Enable */
#define OCIE1A  1   /* Output Compare Match A Interrupt Enable */
#define OCIE1B  2   /* Output Compare Match B Interrupt Enable */
#define ICIE1   5   /* Input Capture Interrupt Enable */

/* Amplifier 0 Control and Status register */
#define AMP0CSR _SFR_MEM8(0x76)
#define AMP0TS0 0
#define AMP0TS1 1
#define AMP0G0  4
#define AMP0G1  5
#define AMP0IS  6
#define AMP0EN  7

/* Amplifier 1 Control and Status register */
#define AMP1CSR _SFR_MEM8(0x77)
#define AMP1TS0 0
#define AMP1TS1 1
#define AMP1G0  4
#define AMP1G1  5
#define AMP1IS  6
#define AMP1EN  7

/* ADC Result Data Register */
#ifndef __ASSEMBLER__
#define ADC     _SFR_MEM16(0x78)
#endif
#define ADCW    _SFR_MEM16(0x78)
#define ADCL    _SFR_MEM8(0x78)
#define ADCH    _SFR_MEM8(0x79)

/* ADC Control and Status Register A */
#define ADCSRA  _SFR_MEM8(0x7A)
#define ADPS0   0   /* ADC Prescaler Select bit0 */
#define ADPS1   1   /* ADC Prescaler Select bit1 */
#define ADPS2   2   /* ADC Prescaler Select bit2 */
#define ADIE    3   /* ADC Interrupt Enable */
#define ADIF    4   /* ADC Interrupt Flag */
#define ADATE   5   /* ADC Auto Trigger Enable */
#define ADSC    6   /* ADC Start Conversion */
#define ADEN    7   /* ADC Enable */

/* ADC Control and Status Register B */
#define ADCSRB  _SFR_MEM8(0x7B)
#define ADTS0   0   /* ADC Auto Trigger Source 0 */
#define ADTS1   1   /* ADC Auto Trigger Source 1 */
#define ADTS2   2   /* ADC Auto Trigger Source 2 */
#define ADTS3   3   /* ADC Auto Trigger Source 3 */
#define ADHSM   7   /* ADC High Speed Mode */

/* ADC multiplexer Selection Register */
#define ADMUX   _SFR_MEM8(0x7C)
#define MUX0    0   /* Analog Channel and Gain Selection bit0 */
#define MUX1    1   /* Analog Channel and Gain Selection bit1 */
#define MUX2    2   /* Analog Channel and Gain Selection bit2 */
#define MUX3    3   /* Analog Channel and Gain Selection bit3 */
#define ADLAR   5   /* Left Adjust Result */
#define REFS0   6   /* Reference Selection bit0 */
#define REFS1   7   /* Reference Selection bit1 */

/* Digital Input Disable Register 0 */
#define DIDR0   _SFR_MEM8(0x7E)
#define ADC0D   0   /* ADC0 Digital input Disable */
#define ADC1D   1   /* ADC1 Digital input Disable */
#define ADC2D   2   /* ADC2 Digital input Disable */
#define ADC3D   3   /* ADC3 Digital input Disable */
#define ADC4D   4   /* ADC4 Digital input Disable */
#define ADC5D   5   /* ADC5 Digital input Disable */
#define ADC6D   6   /* ADC6 Digital input Disable */
#define ADC7D   7   /* ADC7 Digital input Disable */

/* Digital Input Disable Register 1 */
#define DIDR1   _SFR_MEM8(0x7F)
#define ADC8D   0   /* ADC8 Digital input Disable */
#define ADC9D   1   /* ADC9 Digital input Disable */
#define ADC10D  2   /* ADC10 Digital input Disable */
#define AMP0ND  3
#define AMP0PD  4
#define ACMP0D  5

/* Timer/Counter1 Control Register A */
#define TCCR1A  _SFR_MEM8(0x80)
#define WGM10   0   /* Waveform Generation Mode */
#define WGM11   1   /* Waveform Generation Mode */
#define COM1B0  4   /* Compare Output Mode 1B, bit 0 */
#define COM1B1  5   /* Compare Output Mode 1B, bit 1 */
#define COM1A0  6   /* Comparet Ouput Mode 1A, bit 0 */
#define COM1A1  7   /* Comparet Ouput Mode 1A, bit 1 */

/* Timer/Counter1 Control Register B */
#define TCCR1B  _SFR_MEM8(0x81)
#define CS10    0   /* Prescaler source of Timer/Counter 1 */
#define CS11    1   /* Prescaler source of Timer/Counter 1 */
#define CS12    2   /* Prescaler source of Timer/Counter 1 */
#define WGM12   3   /* Waveform Generation Mode */
#define WGM13   4   /* Waveform Generation Mode */
#define ICES1   6   /* Input Capture 1 Edge Select */
#define ICNC1   7   /* Input Capture 1 Noise Canceler */

/* Timer/Counter1 Control Register C */
#define TCCR1C  _SFR_MEM8(0x82)
#define FOC1B   6   /* Force Output Compare for Channel B */
#define FOC1A   7   /* Force Output Compare for Channel A */

/* Timer/Counter1 */
#define TCNT1   _SFR_MEM16(0x84)
#define TCNT1L  _SFR_MEM8(0x84)
#define TCNT10  0
#define TCNT11  1
#define TCNT12  2
#define TCNT13  3
#define TCNT14  4
#define TCNT15  5
#define TCNT16  6
#define TCNT17  7
#define TCNT1H  _SFR_MEM8(0x85)
#define TCNT18  0
#define TCNT19  1
#define TCNT110 2
#define TCNT111 3
#define TCNT112 4
#define TCNT113 5
#define TCNT114 6
#define TCNT115 7

/* Input Capture Register 1 */
#define ICR1    _SFR_MEM16(0x86)
#define ICR1L   _SFR_MEM8(0x86)
#define ICR17   7
#define ICR16   6
#define ICR15   5
#define ICR14   4
#define ICR13   3
#define ICR12   2
#define ICR11   1
#define ICR10   0
#define ICR1H   _SFR_MEM8(0x87)
#define ICR115  7
#define ICR114  6
#define ICR113  5
#define ICR112  4
#define ICR111  3
#define ICR110  2
#define ICR19   1
#define ICR18   0

/* Output Compare Register 1 A */
#define OCR1A   _SFR_MEM16(0x88)
#define OCR1AL  _SFR_MEM8(0x88)
#define OCR1A0  0
#define OCR1A1  1
#define OCR1A2  2
#define OCR1A3  3
#define OCR1A4  4
#define OCR1A5  5
#define OCR1A6  6
#define OCR1A7  7
#define OCR1AH  _SFR_MEM8(0x89)
#define OCR1A8  0
#define OCR1A9  1
#define OCR1A10 2
#define OCR1A11 3
#define OCR1A12 4
#define OCR1A13 5
#define OCR1A14 6
#define OCR1A15 7

/* Output Compare Register 1 B */
#define OCR1B   _SFR_MEM16(0x8A)
#define OCR1BL  _SFR_MEM8(0x8A)
#define OCR1B0  0
#define OCR1B1  1
#define OCR1B2  2
#define OCR1B3  3
#define OCR1B4  4
#define OCR1B5  5
#define OCR1B6  6
#define OCR1B7  7
#define OCR1BH  _SFR_MEM8(0x8B)
#define OCR1B8  0
#define OCR1B9  1
#define OCR1B10 2
#define OCR1B11 3
#define OCR1B12 4
#define OCR1B13 5
#define OCR1B14 6
#define OCR1B15 7

/* PSC0 Interrupt Flag Register */
#define PIFR0   _SFR_MEM8(0xA0)
#define PEOP0   0   /* End Of PSC0 Interrupt */
#define PRN00   1   /* PSC0 Ramp Number bit0 */
#define PRN01   2   /* PSC0 Ramp Number bit1 */
#define PEV0A   3   /* PSC0 External Event A Interrupt */
#define PEV0B   4   /* PSC0 External Event B Interrupt */
#define PSEI0   5   /* PSC0 Synchro Error Interrupt */
#define POAC0A  6   /* PSC0 Output A Activity */
#define POAC0B  7   /* PSC0 Output B Activity */

/* PSC0 Interrupt Mask Register */
#define PIM0    _SFR_MEM8(0xA1)
#define PEOPE0  0   /* PSC0 End Of Cycle Interrupt Enable */
#define PEVE0A  3   /* PSC0 External Event A Interrupt Enable */
#define PEVE0B  4   /* PSC0 External Event B Interrupt Enable */
#define PSEIE0  5   /* PSC0 Synchro Error Interrupt Enable */

/* PSC1 Interrupt Flag Register */
#define PIFR1   _SFR_MEM8(0xA2)

/* PSC1 Interrupt Mask Register */
#define PIM1    _SFR_MEM8(0xA3)

/* PSC2 Interrupt Flag Register */
#define PIFR2   _SFR_MEM8(0xA4)
#define PEOP2   0   /* End Of PSC2 Interrupt */
#define PRN20   1   /* PSC2 Ramp Number bit0 */
#define PRN21   2   /* PSC2 Ramp Number bit1 */
#define PEV2A   3   /* PSC2 External Event A Interrupt */
#define PEV2B   4   /* PSC2 External Event B Interrupt */
#define PSEI2   5   /* PSC2 Synchro Error Interrupt */
#define POAC2A  6   /* PSC2 Output A Activity */
#define POAC2B  7   /* PSC2 Output B Activity */

/* PSC2 Interrupt Mask Register */
#define PIM2    _SFR_MEM8(0xA5)
#define PEOPE2  0   /* PSC2 End Of Cycle Interrupt Enable */
#define PEVE2A  3   /* PSC2 External Event A Interrupt Enable */
#define PEVE2B  4   /* PSC2 External Event B Interrupt Enable */
#define PSEIE2  5   /* PSC2 Synchro Error Interrupt Enable */

/* Digital to Analog Conversion Control Register */
#define DACON   _SFR_MEM8(0xAA)
#define DAEN    0   /* Digital to Analog Enable bit */
#define DAOE    1   /* Digital to Analog Output Enable bit */
#define DALA    2   /* Digital to Analog Left Adjust */
#define DATS0   4   /* DAC Trigger Selection bit0 */
#define DATS1   5   /* DAC Trigger Selection bit1 */
#define DATS2   6   /* DAC Trigger Selection bit2 */
#define DAATE   7   /* DAC Auto Trigger Enable bit */

/* Digital to Analog Converter input Register */
#define DAC     _SFR_MEM16(0xAB)
#define DACL    _SFR_MEM8(0xAB)
#define DACH    _SFR_MEM8(0xAC)

/* Analog Comparator 0 Control Register */
#define AC0CON  _SFR_MEM8(0xAD)
#define AC0M0   0   /* Analog Comparator 0 Multiplexer register bit0 */
#define AC0M1   1   /* Analog Comparator 0 Multiplexer register bit1 */
#define AC0M2   2   /* Analog Comparator 0 Multiplexer register bit2 */
#define AC0IS0  4   /* Analog Comparator 0 Interrupt Select bit0 */
#define AC0IS1  5   /* Analog Comparator 0 Interrupt Select bit1 */
#define AC0IE   6   /* Analog Comparator 0 Interrupt Enable bit */
#define AC0EN   7   /* Analog Comparator 0 Enable Bit */

/* Analog Comparator 1 Control Register */
#define AC1CON  _SFR_MEM8(0xAE)
#define AC1M0   0   /* Analog Comparator 1 Multiplexer register bit0 */
#define AC1M1   1   /* Analog Comparator 1 Multiplexer register bit1 */
#define AC1M2   2   /* Analog Comparator 1 Multiplexer register bit2 */
#define AC1ICE  3   /* Analog Comparator 1 Interrupt Capture Enable bit */
#define AC1IS0  4   /* Analog Comparator 1 Interrupt Select bit0 */
#define AC1IS1  5   /* Analog Comparator 1 Interrupt Select bit1 */
#define AC1IE   6   /* Analog Comparator 1 Interrupt Enable bit */
#define AC1EN   7   /* Analog Comparator 1 Enable Bit */

/* Analog Comparator 2 Control Register */
#define AC2CON  _SFR_MEM8(0xAF)
#define AC2M0   0   /* Analog Comparator 2 Multiplexer register bit0 */
#define AC2M1   1   /* Analog Comparator 2 Multiplexer register bit1 */
#define AC2M2   2   /* Analog Comparator 2 Multiplexer register bit2 */
#define AC2IS0  4   /* Analog Comparator 2 Interrupt Select bit0 */
#define AC2IS1  5   /* Analog Comparator 2 Interrupt Select bit1 */
#define AC2IE   6   /* Analog Comparator 2 Interrupt Enable bit */
#define AC2EN   7   /* Analog Comparator 2 Enable Bit */

/* USART Control and Status Register A */
#define UCSRA   _SFR_MEM8(0xC0)
#define MPCM    0   /* Multi-processor Communication Mode */
#define U2X     1   /* Double the USART Transmission Speed */
#define UPE     2   /* USART Parity Error */
#define DOR     3   /* Data OverRun */
#define FE      4   /* Frame Error */
#define UDRE    5   /* USART Data Register Empty */
#define TXC     6   /* USART Transmit Complete */
#define RXC     7   /* USART Receive Complete */

/* USART Control and Status Register B */
#define UCSRB   _SFR_MEM8(0xC1)
#define TXB8    0   /* Transmit Data Bit 8 */
#define RXB8    1   /* Receive Data Bit 8 */
#define UCSZ2   2   /* Character Size */
#define TXEN    3   /* Transmitter Enable */
#define RXEN    4   /* Receiver Enable */
#define UDRIE   5   /* USART Data Register Empty Interrupt Enable */
#define TXCIE   6   /* TX Complete Interrupt Enable */
#define RXCIE   7   /* RX Complete Interrupt Enable */

/* USART Control and Status Register C */
#define UCSRC   _SFR_MEM8(0xC2)
#define UCPOL   0   /* Clock Polarity */
#define UCSZ0   1   /* Character Size bit0 */
#define UCSZ1   2   /* Character Size bit1 */
#define USBS    3   /* Stop Bit Select */
#define UPM0    4   /* Parity Mode bit0 */
#define UPM1    5   /* Parity Mode bit1 */
#define UMSEL   6   /* USART Mode Select */

/* USART Baud Rate Register */
#define UBRR    _SFR_MEM16(0xC4)
#define UBRRL   _SFR_MEM8(0xC4)
#define UBRRH   _SFR_MEM8(0xC5)

/* USART I/O Data Register */
#define UDR     _SFR_MEM8(0xC6)

/* EUSART Control and Status Register A */
#define EUCSRA  _SFR_MEM8(0xC8)
#define URxS0   0   /* EUSART Receive Character Size bit0 */
#define URxS1   1   /* EUSART Receive Character Size bit1 */
#define URxS2   2   /* EUSART Receive Character Size bit2 */
#define URxS3   3   /* EUSART Receive Character Size bit3 */
#define UTxS0   4   /* EUSART Transmit Character Size bit0 */
#define UTxS1   5   /* EUSART Transmit Character Size bit1 */
#define UTxS2   6   /* EUSART Transmit Character Size bit2 */
#define UTxS3   7   /* EUSART Transmit Character Size bit3 */

/* EUSART Control and Status Register B */
#define EUCSRB  _SFR_MEM8(0xC9)
#define BODR    0   /* Bit Order */
#define EMCH    1   /* Manchester mode */
#define EUSBS   3   /* EUSBS Enable Bit */
#define EUSART  4   /* EUSART Enable Bit */

/* EUSART Control and Status Register C */
#define EUCSRC  _SFR_MEM8(0xCA)
#define STP0    0   /* Stop bits values bit0 */
#define STP1    1   /* Stop bits values bit1 */
#define F1617   2
#define FEM     3   /* Frame Error Manchester */

/* Manchester receiver Baud Rate Registers */
#define MUBRR   _SFR_MEM16(0xCC)
#define MUBRRL  _SFR_MEM8(0xCC)
#define MUBRRH  _SFR_MEM8(0xCD)

/* EUSART I/O Data Register */
#define EUDR    _SFR_MEM8(0xCE)

/* PSC 0 Synchro and Output Configuration */
#define PSOC0   _SFR_MEM8(0xD0)
#define POEN0A  0   /* PSC 0 OUT Part A Output Enable */
#define POEN0B  2   /* PSC 0 OUT Part B Output Enable */
#define PSYNC00 4   /* Synchronization Out for ADC Selection bit0 */
#define PSYNC01 5   /* Synchronization Out for ADC Selection bit1 */

/* Output Compare SA Registers */
#define OCR0SA  _SFR_MEM16(0xD2)
#define OCR0SAL _SFR_MEM8(0xD2)
#define OCR0SAH _SFR_MEM8(0xD3)

/* Output Compare RA Registers */
#define OCR0RA  _SFR_MEM16(0xD4)
#define OCR0RAL _SFR_MEM8(0xD4)
#define OCR0RAH _SFR_MEM8(0xD5)

/* Output Compare SB Registers */
#define OCR0SB  _SFR_MEM16(0xD6)
#define OCR0SBL _SFR_MEM8(0xD6)
#define OCR0SBH _SFR_MEM8(0xD7)

/* Output Compare RB Registers */
#define OCR0RB  _SFR_MEM16(0xD8)
#define OCR0RBL _SFR_MEM8(0xD8)
#define OCR0RBH _SFR_MEM8(0xD9)

/* PSC 0 Configuration Register */
#define PCNF0   _SFR_MEM8(0xDA)
#define PCLKSEL0 1  /* PSC 0 Input Clock Select */
#define POP0     2  /* PSC 0 Output Polarity */
#define PMODE00  3  /* PSC 0 Mode bit0 */
#define PMODE01  4  /* PSC 0 Mode bit1 */
#define PLOCK0   5  /* PSC 0 Lock */
#define PALOCK0  6  /* PSC 0 Autolock */
#define PFIFTY0  7  /* PSC 0 Fifty */

/* PSC 0 Control Register */
#define PCTL0   _SFR_MEM8(0xDB)
#define PRUN0   0   /* PSC 0 Run */
#define PCCYC0  1   /* PSC 0 Complete Cycle */
#define PARUN0  2   /* PSC 0 Autorun */
#define PAOC0A  3   /* PSC 0 Asynchronous Output Control A */
#define PAOC0B  4   /* PSC 0 Asynchronous Output Control B */
#define PBFM0   5   /* Balance Flank Width Modulation */
#define PPRE00  6   /* PSC 0 Prescaler Select bit0 */
#define PPRE01  7   /* PSC 0 Prescaler Select bit1 */

/* PSC 0 Input A Control Register */
#define PFRC0A  _SFR_MEM8(0xDC)
#define PRFM0A0 0   /* PSC 0 Fault Mode bit0 */
#define PRFM0A1 1   /* PSC 0 Fault Mode bit1 */
#define PRFM0A2 2   /* PSC 0 Fault Mode bit2 */
#define PRFM0A3 3   /* PSC 0 Fault Mode bit3 */
#define PFLTE0A 4   /* PSC 0 Filter Enable on Input Part A */
#define PELEV0A 5   /* PSC 0 Edge Level Selector of Input Part A */
#define PISEL0A 6   /* PSC 0 Input Select for Part A */
#define PCAE0A  7   /* PSC 0 Capture Enable Input Part A */

/* PSC 0 Input B Control Register */
#define PFRC0B  _SFR_MEM8(0xDD)
#define PRFM0B0 0   /* PSC 0 Fault Mode bit0 */
#define PRFM0B1 1   /* PSC 0 Fault Mode bit1 */
#define PRFM0B2 2   /* PSC 0 Fault Mode bit2 */
#define PRFM0B3 3   /* PSC 0 Fault Mode bit3 */
#define PFLTE0B 4   /* PSC 0 Filter Enable on Input Part B */
#define PELEV0B 5   /* PSC 0 Edge Level Selector of Input Part B */
#define PISEL0B 6   /* PSC 0 Input Select for Part B */
#define PCAE0B  7   /* PSC 0 Capture Enable Input Part B */

/* PSC 0 Input Capture Registers */
#define PICR0   _SFR_MEM16(0xDE)
#define PICR0L  _SFR_MEM8(0xDE)
#define PICR0H  _SFR_MEM8(0xDF)
#define PCST0   7   /* PSC Capture Software Trig bit */

/* PSC 1 Synchro and Output Configuration */
#define PSOC1   _SFR_MEM8(0xE0)

/* Output Compare SA Registers */
#define OCR1SA  _SFR_MEM16(0xE2)
#define OCR1SAL _SFR_MEM8(0xE2)
#define OCR1SAH _SFR_MEM8(0xE3)

/* Output Compare RA Registers */
#define OCR1RA  _SFR_MEM16(0xE4)
#define OCR1RAL _SFR_MEM8(0xE4)
#define OCR1RAH _SFR_MEM8(0xE5)

/* Output Compare SB Registers */
#define OCR1SB  _SFR_MEM16(0xE6)
#define OCR1SBL _SFR_MEM8(0xE6)
#define OCR1SBH _SFR_MEM8(0xE7)

/* Output Compare RB Registers */
#define OCR1RB  _SFR_MEM16(0xE8)
#define OCR1RBL _SFR_MEM8(0xE8)
#define OCR1RBH _SFR_MEM8(0xE9)

/* PSC 1 Configuration Register */
#define PCNF1   _SFR_MEM8(0xEA)

/* PSC 1 Control Register */
#define PCTL1   _SFR_MEM8(0xEB)

/* PSC 1 Input A Control Register */
#define PFRC1A  _SFR_MEM8(0xEC)

/* PSC 1 Input B Control Register */
#define PFRC1B  _SFR_MEM8(0xED)

/* PSC 1 Input Capture Registers */
#define PICR1   _SFR_MEM16(0xEE)
#define PICR1L  _SFR_MEM8(0xEE)
#define PICR1H  _SFR_MEM8(0xEF)

/* PSC 2 Synchro and Output Configuration */
#define PSOC2   _SFR_MEM8(0xF0)
#define POEN2A  0   /* PSC 2 OUT Part A Output Enable */
#define POEN2C  1   /* PSCOUT22 Output Enable */
#define POEN2B  2   /* PSC 2 OUT Part B Output Enable */
#define POEN2D  3   /* PSCOUT23 Output Enable */
#define PSYNC20 4   /* Synchronization Out for ADC Selection bit0 */
#define PSYNC21 5   /* Synchronization Out for ADC Selection bit1 */
#define POS22   6   /* PSCOUT22 Selection */
#define POS23   7   /* PSCOUT23 Selection */

/* PSC 2 Output Matrix */
#define POM2    _SFR_MEM8(0xF1)
#define POMV2A0 0   /* Output Matrix Output A Ramp 0 */
#define POMV2A1 1   /* Output Matrix Output A Ramp 1 */
#define POMV2A2 2   /* Output Matrix Output A Ramp 2 */
#define POMV2A3 3   /* Output Matrix Output A Ramp 3 */
#define POMV2B0 4   /* Output Matrix Output B Ramp 0 */
#define POMV2B1 5   /* Output Matrix Output B Ramp 1 */
#define POMV2B2 6   /* Output Matrix Output B Ramp 2 */
#define POMV2B3 7   /* Output Matrix Output B Ramp 3 */

/* Output Compare SA Registers */
#define OCR2SA  _SFR_MEM16(0xF2)
#define OCR2SAL _SFR_MEM8(0xF2)
#define OCR2SAH _SFR_MEM8(0xF3)

/* Output Compare RA Registers */
#define OCR2RA  _SFR_MEM16(0xF4)
#define OCR2RAL _SFR_MEM8(0xF4)
#define OCR2RAH _SFR_MEM8(0xF5)

/* Output Compare SB Registers */
#define OCR2SB  _SFR_MEM16(0xF6)
#define OCR2SBL _SFR_MEM8(0xF6)
#define OCR2SBH _SFR_MEM8(0xF7)

/* Output Compare RB Registers */
#define OCR2RB  _SFR_MEM16(0xF8)
#define OCR2RBL _SFR_MEM8(0xF8)
#define OCR2RBH _SFR_MEM8(0xF9)

/* PSC 2 Configuration Register */
#define PCNF2   _SFR_MEM8(0xFA)
#define POME2    0  /* PSC 2 Output Matrix Enable */
#define PCLKSEL2 1  /* PSC 2 Input Clock Select */
#define POP2     2  /* PSC 2 Output Polarity */
#define PMODE20  3  /* PSC 2 Mode bit0 */
#define PMODE21  4  /* PSC 2 Mode bit1 */
#define PLOCK2   5  /* PSC 2 Lock */
#define PALOCK2  6  /* PSC 2 Autolock */
#define PFIFTY2  7  /* PSC 2 Fifty */

/* PSC 2 Control Register */
#define PCTL2   _SFR_MEM8(0xFB)
#define PRUN2   0   /* PSC 2 Run */
#define PCCYC2  1   /* PSC 2 Complete Cycle */
#define PARUN2  2   /* PSC 2 Autorun */
#define PAOC2A  3   /* PSC 2 Asynchronous Output Control A */
#define PAOC2B  4   /* PSC 2 Asynchronous Output Control B */
#define PBFM2   5   /* Balance Flank Width Modulation */
#define PPRE20  6   /* PSC 2 Prescaler Select bit0 */
#define PPRE21  7   /* PSC 2 Prescaler Select bit1 */

/* PSC 2 Input A Control Register */
#define PFRC2A  _SFR_MEM8(0xFC)
#define PRFM2A0 0   /* PSC 2 Fault Mode bit0 */
#define PRFM2A1 1   /* PSC 2 Fault Mode bit1 */
#define PRFM2A2 2   /* PSC 2 Fault Mode bit2 */
#define PRFM2A3 3   /* PSC 2 Fault Mode bit3 */
#define PFLTE2A 4   /* PSC 2 Filter Enable on Input Part A */
#define PELEV2A 5   /* PSC 2 Edge Level Selector of Input Part A */
#define PISEL2A 6   /* PSC 2 Input Select for Part A */
#define PCAE2A  7   /* PSC 2 Capture Enable Input Part A */

/* PSC 2 Input B Control Register */
#define PFRC2B  _SFR_MEM8(0xFD)
#define PRFM2B0 0   /* PSC 2 Fault Mode bit0 */
#define PRFM2B1 1   /* PSC 2 Fault Mode bit1 */
#define PRFM2B2 2   /* PSC 2 Fault Mode bit2 */
#define PRFM2B3 3   /* PSC 2 Fault Mode bit3 */
#define PFLTE2B 4   /* PSC 2 Filter Enable on Input Part B */
#define PELEV2B 5   /* PSC 2 Edge Level Selector of Input Part B */
#define PISEL2B 6   /* PSC 2 Input Select for Part B */
#define PCAE2B  7   /* PSC 2 Capture Enable Input Part B */

/* PSC 2 Input Capture Registers */
#define PICR2   _SFR_MEM16(0xFE)
#define PICR2L  _SFR_MEM8(0xFE)
#define PICR2H  _SFR_MEM8(0xFF)
#define PCST2   7   /* PSC Capture Software Trig bit */


/* Interrupt Vectors */
/* Interrupt 0 is the reset vector. */

/* PSC2 Capture Event */
#define PSC2_CAPT_vect     _VECTOR(1)

/* PSC2 End Cycle */
#define PSC2_EC_vect       _VECTOR(2)

/* PSC1 Capture Event */
#define PSC1_CAPT_vect     _VECTOR(3)

/* PSC1 End Cycle */
#define PSC1_EC_vect       _VECTOR(4)

/* PSC0 Capture Event */
#define PSC0_CAPT_vect     _VECTOR(5)

/* PSC0 End Cycle */
#define PSC0_EC_vect       _VECTOR(6)

/* Analog Comparator 0 */
#define ANALOG_COMP_0_vect _VECTOR(7)

/* Analog Comparator 1 */
#define ANALOG_COMP_1_vect _VECTOR(8)

/* Analog Comparator 2 */
#define ANALOG_COMP_2_vect _VECTOR(9)

/* External Interrupt Request 0 */
#define INT0_vect          _VECTOR(10)

/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect   _VECTOR(11)

/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect  _VECTOR(12)

/* Timer/Counter Compare Match B */
#define TIMER1_COMPB_vect  _VECTOR(13)

/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect    _VECTOR(15)

/* Timer/Counter0 Compare Match A */
#define TIMER0_COMP_A_vect _VECTOR(16)

/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect    _VECTOR(17)

/* ADC Conversion Complete */
#define ADC_vect           _VECTOR(18)

/* External Interrupt Request 1 */
#define INT1_vect          _VECTOR(19)

/* SPI Serial Transfer Complete */
#define SPI_STC_vect       _VECTOR(20)

/* USART, Rx Complete */
#define USART_RX_vect      _VECTOR(21)

/* USART Data Register Empty */
#define USART_UDRE_vect    _VECTOR(22)

/* USART, Tx Complete */
#define USART_TX_vect      _VECTOR(23)

/* External Interrupt Request 2 */
#define INT2_vect          _VECTOR(24)

/* Watchdog Timeout Interrupt */
#define WDT_vect           _VECTOR(25)

/* EEPROM Ready */
#define EE_READY_vect      _VECTOR(26)

/* Timer Counter 0 Compare Match B */
#define TIMER0_COMPB_vect  _VECTOR(27)

/* External Interrupt Request 3 */
#define INT3_vect          _VECTOR(28)

/* Store Program Memory Read */
#define SPM_READY_vect     _VECTOR(31)

#define _VECTORS_SIZE   (4 * 32)

/* Constants */

#define RAMEND         0x4FF
#define XRAMSIZE       0
#define XRAMEND        (RAMEND + XRAMSIZE)
#define E2END          0x1FF
#define E2PAGESIZE     4
#define FLASHEND       0x3FFF
#define SPM_PAGESIZE   128


/* Fuse Information */

#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0  (unsigned char)~_BV(0)  /* Select Clock Source */
#define FUSE_CKSEL1  (unsigned char)~_BV(1)  /* Select Clock Source */
#define FUSE_CKSEL2  (unsigned char)~_BV(2)  /* Select Clock Source */
#define FUSE_CKSEL3  (unsigned char)~_BV(3)  /* Select Clock Source */
#define FUSE_SUT0    (unsigned char)~_BV(4)  /* Select start-up time */
#define FUSE_SUT1    (unsigned char)~_BV(5)  /* Select start-up time */
#define FUSE_CKOUT   (unsigned char)~_BV(6)  /* Oscillator output option */
#define FUSE_CKDIV8  (unsigned char)~_BV(7)  /* Divide clock by 8 */
#define LFUSE_DEFAULT (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0 & FUSE_CKDIV8)

/* High Fuse Byte */
#define FUSE_BODLEVEL0 (unsigned char)~_BV(0)
#define FUSE_BODLEVEL1 (unsigned char)~_BV(1)
#define FUSE_BODLEVEL2 (unsigned char)~_BV(2)
#define FUSE_EESAVE    (unsigned char)~_BV(3)  /* EEPROM memory is preserved through chip erase */
#define FUSE_WDTON     (unsigned char)~_BV(4)  /* Watchdog timer always on */
#define FUSE_SPIEN     (unsigned char)~_BV(5)  /* Enable Serial programming and Data Downloading */
#define FUSE_DWEN      (unsigned char)~_BV(6)  /* debugWIRE Enable */
#define FUSE_RSTDISBL  (unsigned char)~_BV(7)  /* External Reset Diasble */
#define HFUSE_DEFAULT (FUSE_SPIEN)

/* Extended Fuse Byte */
#define FUSE_BOOTRST  (unsigned char)~_BV(0)  /* Select Reset Vector */
#define FUSE_BOOTSZ0  (unsigned char)~_BV(1)  /* Select Boot Size */
#define FUSE_BOOTSZ1  (unsigned char)~_BV(2)  /* Select Boot Size */
#define FUSE_PSCRV    (unsigned char)~_BV(4)
#define FUSE_PSC0RB   (unsigned char)~_BV(5)
#define FUSE_PSC1RB   (unsigned char)~_BV(6)
#define FUSE_PSC2RB   (unsigned char)~_BV(7)
#define EFUSE_DEFAULT (FUSE_BOOTSZ0 & FUSE_BOOTSZ1)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 


/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x94
#define SIGNATURE_2 0x83


#endif /* _AVR_IO90PWM216_H_ */
