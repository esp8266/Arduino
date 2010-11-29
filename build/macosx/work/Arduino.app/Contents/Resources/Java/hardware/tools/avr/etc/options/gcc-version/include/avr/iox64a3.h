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
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id: iox64a3.h,v 1.1.2.4 2008/11/03 04:13:18 arcanum Exp $ */

/* avr/iox64a3.h - definitions for ATxmega64A3 */

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iox64a3.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif 


#ifndef _AVR_ATxmega64A3_H_
#define _AVR_ATxmega64A3_H_ 1


/* Ungrouped common registers */
#define GPIO0  _SFR_MEM8(0x0000)  /* General Purpose IO Register 0 */
#define GPIO1  _SFR_MEM8(0x0001)  /* General Purpose IO Register 1 */
#define GPIO2  _SFR_MEM8(0x0002)  /* General Purpose IO Register 2 */
#define GPIO3  _SFR_MEM8(0x0003)  /* General Purpose IO Register 3 */
#define GPIO4  _SFR_MEM8(0x0004)  /* General Purpose IO Register 4 */
#define GPIO5  _SFR_MEM8(0x0005)  /* General Purpose IO Register 5 */
#define GPIO6  _SFR_MEM8(0x0006)  /* General Purpose IO Register 6 */
#define GPIO7  _SFR_MEM8(0x0007)  /* General Purpose IO Register 7 */
#define GPIO8  _SFR_MEM8(0x0008)  /* General Purpose IO Register 8 */
#define GPIO9  _SFR_MEM8(0x0009)  /* General Purpose IO Register 9 */
#define GPIOA  _SFR_MEM8(0x000A)  /* General Purpose IO Register 10 */
#define GPIOB  _SFR_MEM8(0x000B)  /* General Purpose IO Register 11 */
#define GPIOC  _SFR_MEM8(0x000C)  /* General Purpose IO Register 12 */
#define GPIOD  _SFR_MEM8(0x000D)  /* General Purpose IO Register 13 */
#define GPIOE  _SFR_MEM8(0x000E)  /* General Purpose IO Register 14 */
#define GPIOF  _SFR_MEM8(0x000F)  /* General Purpose IO Register 15 */

#define CCP  _SFR_MEM8(0x0034)  /* Configuration Change Protection */
#define RAMPD  _SFR_MEM8(0x0038)  /* Ramp D */
#define RAMPX  _SFR_MEM8(0x0039)  /* Ramp X */
#define RAMPY  _SFR_MEM8(0x003A)  /* Ramp Y */
#define RAMPZ  _SFR_MEM8(0x003B)  /* Ramp Z */
#define EIND  _SFR_MEM8(0x003C)  /* Extended Indirect Jump */
#define SPL  _SFR_MEM8(0x003D)  /* Stack Pointer Low */
#define SPH  _SFR_MEM8(0x003E)  /* Stack Pointer High */
#define SREG  _SFR_MEM8(0x003F)  /* Status Register */


/* C Language Only */
#if !defined (__ASSEMBLER__)

#include <stdint.h>

typedef volatile uint8_t register8_t;
typedef volatile uint16_t register16_t;
typedef volatile uint32_t register32_t;


#ifdef _WORDREGISTER
#undef _WORDREGISTER
#endif
#define _WORDREGISTER(regname)   \
    union \
    { \
        register16_t regname; \
        struct \
        { \
            register8_t regname ## L; \
            register8_t regname ## H; \
        }; \
    }

#ifdef _DWORDREGISTER
#undef _DWORDREGISTER
#endif
#define _DWORDREGISTER(regname)  \
    union \
    { \
        register32_t regname; \
        struct \
        { \
            register8_t regname ## 0; \
            register8_t regname ## 1; \
            register8_t regname ## 2; \
            register8_t regname ## 3; \
        }; \
    }


/*
==========================================================================
IO Module Structures
==========================================================================
*/


/*
--------------------------------------------------------------------------
XOCD - On-Chip Debug System
--------------------------------------------------------------------------
*/

/* On-Chip Debug System */
typedef struct OCD_struct
{
    register8_t OCDR0;  /* OCD Register 0 */
    register8_t OCDR1;  /* OCD Register 1 */
} OCD_t;


/* CCP signatures */
typedef enum CCP_enum
{
    CCP_SPM_gc = (0x9D<<0),  /* SPM Instruction Protection */
    CCP_IOREG_gc = (0xD8<<0),  /* IO Register Protection */
} CCP_t;


/*
--------------------------------------------------------------------------
CLK - Clock System
--------------------------------------------------------------------------
*/

/* Clock System */
typedef struct CLK_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t PSCTRL;  /* Prescaler Control Register */
    register8_t LOCK;  /* Lock register */
    register8_t RTCCTRL;  /* RTC Control Register */
} CLK_t;

/*
--------------------------------------------------------------------------
CLK - Clock System
--------------------------------------------------------------------------
*/

/* Power Reduction */
typedef struct PR_struct
{
    register8_t PR;  /* General Power Reduction */
    register8_t PRPA;  /* Power Reduction Port A */
    register8_t PRPB;  /* Power Reduction Port B */
    register8_t PRPC;  /* Power Reduction Port C */
    register8_t PRPD;  /* Power Reduction Port D */
    register8_t PRPE;  /* Power Reduction Port E */
    register8_t PRPF;  /* Power Reduction Port F */
} PR_t;

/* System Clock Selection */
typedef enum CLK_SCLKSEL_enum
{
    CLK_SCLKSEL_RC2M_gc = (0x00<<0),  /* Internal 2MHz RC Oscillator */
    CLK_SCLKSEL_RC32M_gc = (0x01<<0),  /* Internal 32MHz RC Oscillator */
    CLK_SCLKSEL_RC32K_gc = (0x02<<0),  /* Internal 32kHz RC Oscillator */
    CLK_SCLKSEL_XOSC_gc = (0x03<<0),  /* External Crystal Oscillator or Clock */
    CLK_SCLKSEL_PLL_gc = (0x04<<0),  /* Phase Locked Loop */
} CLK_SCLKSEL_t;

/* Prescaler A Division Factor */
typedef enum CLK_PSADIV_enum
{
    CLK_PSADIV_1_gc = (0x00<<2),  /* Divide by 1 */
    CLK_PSADIV_2_gc = (0x01<<2),  /* Divide by 2 */
    CLK_PSADIV_4_gc = (0x03<<2),  /* Divide by 4 */
    CLK_PSADIV_8_gc = (0x05<<2),  /* Divide by 8 */
    CLK_PSADIV_16_gc = (0x07<<2),  /* Divide by 16 */
    CLK_PSADIV_32_gc = (0x09<<2),  /* Divide by 32 */
    CLK_PSADIV_64_gc = (0x0B<<2),  /* Divide by 64 */
    CLK_PSADIV_128_gc = (0x0D<<2),  /* Divide by 128 */
    CLK_PSADIV_256_gc = (0x0F<<2),  /* Divide by 256 */
    CLK_PSADIV_512_gc = (0x11<<2),  /* Divide by 512 */
} CLK_PSADIV_t;

/* Prescaler B and C Division Factor */
typedef enum CLK_PSBCDIV_enum
{
    CLK_PSBCDIV_1_1_gc = (0x00<<0),  /* Divide B by 1 and C by 1 */
    CLK_PSBCDIV_1_2_gc = (0x01<<0),  /* Divide B by 1 and C by 2 */
    CLK_PSBCDIV_4_1_gc = (0x02<<0),  /* Divide B by 4 and C by 1 */
    CLK_PSBCDIV_2_2_gc = (0x03<<0),  /* Divide B by 2 and C by 2 */
} CLK_PSBCDIV_t;

/* RTC Clock Source */
typedef enum CLK_RTCSRC_enum
{
    CLK_RTCSRC_ULP_gc = (0x00<<1),  /* 1kHz from internal 32kHz ULP */
    CLK_RTCSRC_TOSC_gc = (0x01<<1),  /* 1kHz from 32kHz crystal oscillator on TOSC */
    CLK_RTCSRC_RCOSC_gc = (0x02<<1),  /* 1kHz from internal 32kHz RC oscillator */
    CLK_RTCSRC_TOSC32_gc = (0x05<<1),  /* 32kHz from 32kHz crystal oscillator on TOSC */
} CLK_RTCSRC_t;


/*
--------------------------------------------------------------------------
SLEEP - Sleep Controller
--------------------------------------------------------------------------
*/

/* Sleep Controller */
typedef struct SLEEP_struct
{
    register8_t CTRL;  /* Control Register */
} SLEEP_t;

/* Sleep Mode */
typedef enum SLEEP_SMODE_enum
{
    SLEEP_SMODE_IDLE_gc = (0x00<<1),  /* Idle mode */
    SLEEP_SMODE_PDOWN_gc = (0x02<<1),  /* Power-down Mode */
    SLEEP_SMODE_PSAVE_gc = (0x03<<1),  /* Power-save Mode */
    SLEEP_SMODE_STDBY_gc = (0x06<<1),  /* Standby Mode */
    SLEEP_SMODE_ESTDBY_gc = (0x07<<1),  /* Extended Standby Mode */
} SLEEP_SMODE_t;


/*
--------------------------------------------------------------------------
OSC - Oscillator
--------------------------------------------------------------------------
*/

/* Oscillator */
typedef struct OSC_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t STATUS;  /* Status Register */
    register8_t XOSCCTRL;  /* External Oscillator Control Register */
    register8_t XOSCFAIL;  /* External Oscillator Failure Detection Register */
    register8_t RC32KCAL;  /* 32kHz Internal Oscillator Calibration Register */
    register8_t PLLCTRL;  /* PLL Control REgister */
    register8_t DFLLCTRL;  /* DFLL Control Register */
} OSC_t;

/* Oscillator Frequency Range */
typedef enum OSC_FRQRANGE_enum
{
    OSC_FRQRANGE_04TO2_gc = (0x00<<6),  /* 0.4 - 2 MHz */
    OSC_FRQRANGE_2TO9_gc = (0x01<<6),  /* 2 - 9 MHz */
    OSC_FRQRANGE_9TO12_gc = (0x02<<6),  /* 9 - 12 MHz */
    OSC_FRQRANGE_12TO16_gc = (0x03<<6),  /* 12 - 16 MHz */
} OSC_FRQRANGE_t;

/* External Oscillator Selection and Startup Time */
typedef enum OSC_XOSCSEL_enum
{
    OSC_XOSCSEL_EXTCLK_gc = (0x00<<0),  /* External Clock - 6 CLK */
    OSC_XOSCSEL_32KHz_gc = (0x02<<0),  /* 32kHz TOSC - 32K CLK */
    OSC_XOSCSEL_XTAL_256CLK_gc = (0x03<<0),  /* 0.4-16MHz XTAL - 256 CLK */
    OSC_XOSCSEL_XTAL_1KCLK_gc = (0x07<<0),  /* 0.4-16MHz XTAL - 1K CLK */
    OSC_XOSCSEL_XTAL_16KCLK_gc = (0x0B<<0),  /* 0.4-16MHz XTAL - 16K CLK */
} OSC_XOSCSEL_t;

/* PLL Clock Source */
typedef enum OSC_PLLSRC_enum
{
    OSC_PLLSRC_RC2M_gc = (0x00<<6),  /* Internal 2MHz RC Oscillator */
    OSC_PLLSRC_RC32M_gc = (0x02<<6),  /* Internal 32MHz RC Oscillator */
    OSC_PLLSRC_XOSC_gc = (0x03<<6),  /* External Oscillator */
} OSC_PLLSRC_t;


/*
--------------------------------------------------------------------------
DFLL - DFLL
--------------------------------------------------------------------------
*/

/* DFLL */
typedef struct DFLL_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t reserved_0x01;
    register8_t CALA;  /* Calibration Register A */
    register8_t CALB;  /* Calibration Register B */
    register8_t OSCCNT0;  /* Oscillator Counter Register 0 */
    register8_t OSCCNT1;  /* Oscillator Counter Register 1 */
    register8_t OSCCNT2;  /* Oscillator Counter Register 2 */
    register8_t reserved_0x07;
} DFLL_t;


/*
--------------------------------------------------------------------------
RST - Reset
--------------------------------------------------------------------------
*/

/* Reset */
typedef struct RST_struct
{
    register8_t STATUS;  /* Status Register */
    register8_t CTRL;  /* Control Register */
} RST_t;


/*
--------------------------------------------------------------------------
WDT - Watch-Dog Timer
--------------------------------------------------------------------------
*/

/* Watch-Dog Timer */
typedef struct WDT_struct
{
    register8_t CTRL;  /* Control */
    register8_t WINCTRL;  /* Windowed Mode Control */
    register8_t STATUS;  /* Status */
} WDT_t;

/* Period setting */
typedef enum WDT_PER_enum
{
    WDT_PER_8CLK_gc = (0x00<<2),  /* 8 cycles (8ms @ 3.3V) */
    WDT_PER_16CLK_gc = (0x01<<2),  /* 16 cycles (16ms @ 3.3V) */
    WDT_PER_32CLK_gc = (0x02<<2),  /* 32 cycles (32ms @ 3.3V) */
    WDT_PER_64CLK_gc = (0x03<<2),  /* 64 cycles (64ms @ 3.3V) */
    WDT_PER_128CLK_gc = (0x04<<2),  /* 128 cycles (0.125s @ 3.3V) */
    WDT_PER_256CLK_gc = (0x05<<2),  /* 256 cycles (0.25s @ 3.3V) */
    WDT_PER_512CLK_gc = (0x06<<2),  /* 512 cycles (0.5s @ 3.3V) */
    WDT_PER_1KCLK_gc = (0x07<<2),  /* 1K cycles (1s @ 3.3V) */
    WDT_PER_2KCLK_gc = (0x08<<2),  /* 2K cycles (2s @ 3.3V) */
    WDT_PER_4KCLK_gc = (0x09<<2),  /* 4K cycles (4s @ 3.3V) */
    WDT_PER_8KCLK_gc = (0x0A<<2),  /* 8K cycles (8s @ 3.3V) */
} WDT_PER_t;

/* Closed window period */
typedef enum WDT_WPER_enum
{
    WDT_WPER_8CLK_gc = (0x00<<2),  /* 8 cycles (8ms @ 3.3V) */
    WDT_WPER_16CLK_gc = (0x01<<2),  /* 16 cycles (16ms @ 3.3V) */
    WDT_WPER_32CLK_gc = (0x02<<2),  /* 32 cycles (32ms @ 3.3V) */
    WDT_WPER_64CLK_gc = (0x03<<2),  /* 64 cycles (64ms @ 3.3V) */
    WDT_WPER_128CLK_gc = (0x04<<2),  /* 128 cycles (0.125s @ 3.3V) */
    WDT_WPER_256CLK_gc = (0x05<<2),  /* 256 cycles (0.25s @ 3.3V) */
    WDT_WPER_512CLK_gc = (0x06<<2),  /* 512 cycles (0.5s @ 3.3V) */
    WDT_WPER_1KCLK_gc = (0x07<<2),  /* 1K cycles (1s @ 3.3V) */
    WDT_WPER_2KCLK_gc = (0x08<<2),  /* 2K cycles (2s @ 3.3V) */
    WDT_WPER_4KCLK_gc = (0x09<<2),  /* 4K cycles (4s @ 3.3V) */
    WDT_WPER_8KCLK_gc = (0x0A<<2),  /* 8K cycles (8s @ 3.3V) */
} WDT_WPER_t;


/*
--------------------------------------------------------------------------
MCU - MCU Control
--------------------------------------------------------------------------
*/

/* MCU Control */
typedef struct MCU_struct
{
    register8_t DEVID0;  /* Device ID byte 0 */
    register8_t DEVID1;  /* Device ID byte 1 */
    register8_t DEVID2;  /* Device ID byte 2 */
    register8_t REVID;  /* Revision ID */
    register8_t JTAGUID;  /* JTAG User ID */
    register8_t reserved_0x05;
    register8_t MCUCR;  /* MCU Control */
    register8_t reserved_0x07;
    register8_t EVSYSLOCK;  /* Event System Lock */
    register8_t AWEXLOCK;  /* AWEX Lock */
    register8_t reserved_0x0A;
    register8_t reserved_0x0B;
} MCU_t;


/*
--------------------------------------------------------------------------
PMIC - Programmable Multi-level Interrupt Controller
--------------------------------------------------------------------------
*/

/* Programmable Multi-level Interrupt Controller */
typedef struct PMIC_struct
{
    register8_t STATUS;  /* Status Register */
    register8_t INTPRI;  /* Interrupt Priority */
    register8_t CTRL;  /* Control Register */
} PMIC_t;


/*
--------------------------------------------------------------------------
DMA - DMA Controller
--------------------------------------------------------------------------
*/

/* DMA Channel */
typedef struct DMA_CH_struct
{
    register8_t CTRLA;  /* Channel Control */
    register8_t CTRLB;  /* Channel Control */
    register8_t ADDRCTRL;  /* Address Control */
    register8_t TRIGSRC;  /* Channel Trigger Source */
    _WORDREGISTER(TRFCNT);  /* Channel Block Transfer Count */
    register8_t REPCNT;  /* Channel Repeat Count */
    register8_t reserved_0x07;
    register8_t SRCADDR0;  /* Channel Source Address 0 */
    register8_t SRCADDR1;  /* Channel Source Address 1 */
    register8_t SRCADDR2;  /* Channel Source Address 2 */
    register8_t reserved_0x0B;
    register8_t DESTADDR0;  /* Channel Destination Address 0 */
    register8_t DESTADDR1;  /* Channel Destination Address 1 */
    register8_t DESTADDR2;  /* Channel Destination Address 2 */
    register8_t reserved_0x0F;
} DMA_CH_t;

/*
--------------------------------------------------------------------------
DMA - DMA Controller
--------------------------------------------------------------------------
*/

/* DMA Controller */
typedef struct DMA_struct
{
    register8_t CTRL;  /* Control */
    register8_t reserved_0x01;
    register8_t reserved_0x02;
    register8_t INTFLAGS;  /* Transfer Interrupt Status */
    register8_t STATUS;  /* Status */
    register8_t reserved_0x05;
    _WORDREGISTER(TEMP);  /* Temporary Register For 16/24-bit Access */
    register8_t reserved_0x08;
    register8_t reserved_0x09;
    register8_t reserved_0x0A;
    register8_t reserved_0x0B;
    register8_t reserved_0x0C;
    register8_t reserved_0x0D;
    register8_t reserved_0x0E;
    register8_t reserved_0x0F;
    DMA_CH_t CH0;  /* DMA Channel 0 */
    DMA_CH_t CH1;  /* DMA Channel 1 */
    DMA_CH_t CH2;  /* DMA Channel 2 */
    DMA_CH_t CH3;  /* DMA Channel 3 */
} DMA_t;

/* Burst mode */
typedef enum DMA_CH_BURSTLEN_enum
{
    DMA_CH_BURSTLEN_1BYTE_gc = (0x00<<0),  /* 1-byte burst mode */
    DMA_CH_BURSTLEN_2BYTE_gc = (0x01<<0),  /* 2-byte burst mode */
    DMA_CH_BURSTLEN_4BYTE_gc = (0x02<<0),  /* 4-byte burst mode */
    DMA_CH_BURSTLEN_8BYTE_gc = (0x03<<0),  /* 8-byte burst mode */
} DMA_CH_BURSTLEN_t;

/* Source address reload mode */
typedef enum DMA_CH_SRCRELOAD_enum
{
    DMA_CH_SRCRELOAD_NONE_gc = (0x00<<6),  /* No reload */
    DMA_CH_SRCRELOAD_BLOCK_gc = (0x01<<6),  /* Reload at end of block */
    DMA_CH_SRCRELOAD_BURST_gc = (0x02<<6),  /* Reload at end of burst */
    DMA_CH_SRCRELOAD_TRANSACTION_gc = (0x03<<6),  /* Reload at end of transaction */
} DMA_CH_SRCRELOAD_t;

/* Source addressing mode */
typedef enum DMA_CH_SRCDIR_enum
{
    DMA_CH_SRCDIR_FIXED_gc = (0x00<<4),  /* Fixed */
    DMA_CH_SRCDIR_INC_gc = (0x01<<4),  /* Increment */
    DMA_CH_SRCDIR_DEC_gc = (0x02<<4),  /* Decrement */
} DMA_CH_SRCDIR_t;

/* Destination adress reload mode */
typedef enum DMA_CH_DESTRELOAD_enum
{
    DMA_CH_DESTRELOAD_NONE_gc = (0x00<<2),  /* No reload */
    DMA_CH_DESTRELOAD_BLOCK_gc = (0x01<<2),  /* Reload at end of block */
    DMA_CH_DESTRELOAD_BURST_gc = (0x02<<2),  /* Reload at end of burst */
    DMA_CH_DESTRELOAD_TRANSACTION_gc = (0x03<<2),  /* Reload at end of transaction */
} DMA_CH_DESTRELOAD_t;

/* Destination adressing mode */
typedef enum DMA_CH_DESTDIR_enum
{
    DMA_CH_DESTDIR_FIXED_gc = (0x00<<0),  /* Fixed */
    DMA_CH_DESTDIR_INC_gc = (0x01<<0),  /* Increment */
    DMA_CH_DESTDIR_DEC_gc = (0x02<<0),  /* Decrement */
} DMA_CH_DESTDIR_t;

/* Transfer trigger source */
typedef enum DMA_CH_TRIGSRC_enum
{
    DMA_CH_TRIGSRC_OFF_gc = (0x00<<0),  /* Off software triggers only */
    DMA_CH_TRIGSRC_EVSYS_CH0_gc = (0x01<<0),  /* Event System Channel 0 */
    DMA_CH_TRIGSRC_EVSYS_CH1_gc = (0x02<<0),  /* Event System Channel 1 */
    DMA_CH_TRIGSRC_EVSYS_CH2_gc = (0x03<<0),  /* Event System Channel 2 */
    DMA_CH_TRIGSRC_ADCA_CH0_gc = (0x10<<0),  /* ADCA Channel 0 */
    DMA_CH_TRIGSRC_ADCA_CH1_gc = (0x11<<0),  /* ADCA Channel 1 */
    DMA_CH_TRIGSRC_ADCA_CH2_gc = (0x12<<0),  /* ADCA Channel 2 */
    DMA_CH_TRIGSRC_ADCA_CH3_gc = (0x13<<0),  /* ADCA Channel 3 */
    DMA_CH_TRIGSRC_ADCA_CH4_gc = (0x14<<0),  /* ADCA Channel 0,1,2,3 combined */
    DMA_CH_TRIGSRC_DACA_CH0_gc = (0x15<<0),  /* DACA Channel 0 */
    DMA_CH_TRIGSRC_DACA_CH1_gc = (0x16<<0),  /* DACA Channel 1 */
    DMA_CH_TRIGSRC_ADCB_CH0_gc = (0x20<<0),  /* ADCB Channel 0 */
    DMA_CH_TRIGSRC_ADCB_CH1_gc = (0x21<<0),  /* ADCB Channel 1 */
    DMA_CH_TRIGSRC_ADCB_CH2_gc = (0x22<<0),  /* ADCB Channel 2 */
    DMA_CH_TRIGSRC_ADCB_CH3_gc = (0x23<<0),  /* ADCB Channel 3 */
    DMA_CH_TRIGSRC_ADCB_CH4_gc = (0x24<<0),  /* ADCB Channel 0,1,2,3 combined */
    DMA_CH_TRIGSRC_DACB_CH0_gc = (0x25<<0),  /* DACB Channel 0 */
    DMA_CH_TRIGSRC_DACB_CH1_gc = (0x26<<0),  /* DACB Channel 1 */
    DMA_CH_TRIGSRC_TCC0_OVF_gc = (0x40<<0),  /* Timer/Counter C0 Overflow */
    DMA_CH_TRIGSRC_TCC0_ERR_gc = (0x41<<0),  /* Timer/Counter C0 Error */
    DMA_CH_TRIGSRC_TCC0_CCA_gc = (0x42<<0),  /* Timer/Counter C0 Compare or Capture A */
    DMA_CH_TRIGSRC_TCC0_CCB_gc = (0x43<<0),  /* Timer/Counter C0 Compare or Capture B */
    DMA_CH_TRIGSRC_TCC0_CCC_gc = (0x44<<0),  /* Timer/Counter C0 Compare or Capture C */
    DMA_CH_TRIGSRC_TCC0_CCD_gc = (0x45<<0),  /* Timer/Counter C0 Compare or Capture D */
    DMA_CH_TRIGSRC_TCC1_OVF_gc = (0x46<<0),  /* Timer/Counter C1 Overflow */
    DMA_CH_TRIGSRC_TCC1_ERR_gc = (0x47<<0),  /* Timer/Counter C1 Error */
    DMA_CH_TRIGSRC_TCC1_CCA_gc = (0x48<<0),  /* Timer/Counter C1 Compare or Capture A */
    DMA_CH_TRIGSRC_TCC1_CCB_gc = (0x49<<0),  /* Timer/Counter C1 Compare or Capture B */
    DMA_CH_TRIGSRC_SPIC_gc = (0x4A<<0),  /* SPI C Transfer Complete */
    DMA_CH_TRIGSRC_USARTC0_RXC_gc = (0x4B<<0),  /* USART C0 Receive Complete */
    DMA_CH_TRIGSRC_USARTC0_DRE_gc = (0x4C<<0),  /* USART C0 Data Register Empty */
    DMA_CH_TRIGSRC_USARTC1_RXC_gc = (0x4E<<0),  /* USART C1 Receive Complete */
    DMA_CH_TRIGSRC_USARTC1_DRE_gc = (0x4F<<0),  /* USART C1 Data Register Empty */
    DMA_CH_TRIGSRC_TCD0_OVF_gc = (0x60<<0),  /* Timer/Counter D0 Overflow */
    DMA_CH_TRIGSRC_TCD0_ERR_gc = (0x61<<0),  /* Timer/Counter D0 Error */
    DMA_CH_TRIGSRC_TCD0_CCA_gc = (0x62<<0),  /* Timer/Counter D0 Compare or Capture A */
    DMA_CH_TRIGSRC_TCD0_CCB_gc = (0x63<<0),  /* Timer/Counter D0 Compare or Capture B */
    DMA_CH_TRIGSRC_TCD0_CCC_gc = (0x64<<0),  /* Timer/Counter D0 Compare or Capture C */
    DMA_CH_TRIGSRC_TCD0_CCD_gc = (0x65<<0),  /* Timer/Counter D0 Compare or Capture D */
    DMA_CH_TRIGSRC_TCD1_OVF_gc = (0x66<<0),  /* Timer/Counter D1 Overflow */
    DMA_CH_TRIGSRC_TCD1_ERR_gc = (0x67<<0),  /* Timer/Counter D1 Error */
    DMA_CH_TRIGSRC_TCD1_CCA_gc = (0x68<<0),  /* Timer/Counter D1 Compare or Capture A */
    DMA_CH_TRIGSRC_TCD1_CCB_gc = (0x69<<0),  /* Timer/Counter D1 Compare or Capture B */
    DMA_CH_TRIGSRC_SPID_gc = (0x6A<<0),  /* SPI D Transfer Complete */
    DMA_CH_TRIGSRC_USARTD0_RXC_gc = (0x6B<<0),  /* USART D0 Receive Complete */
    DMA_CH_TRIGSRC_USARTD0_DRE_gc = (0x6C<<0),  /* USART D0 Data Register Empty */
    DMA_CH_TRIGSRC_USARTD1_RXC_gc = (0x6E<<0),  /* USART D1 Receive Complete */
    DMA_CH_TRIGSRC_USARTD1_DRE_gc = (0x6F<<0),  /* USART D1 Data Register Empty */
    DMA_CH_TRIGSRC_TCE0_OVF_gc = (0x80<<0),  /* Timer/Counter E0 Overflow */
    DMA_CH_TRIGSRC_TCE0_ERR_gc = (0x81<<0),  /* Timer/Counter E0 Error */
    DMA_CH_TRIGSRC_TCE0_CCA_gc = (0x82<<0),  /* Timer/Counter E0 Compare or Capture A */
    DMA_CH_TRIGSRC_TCE0_CCB_gc = (0x83<<0),  /* Timer/Counter E0 Compare or Capture B */
    DMA_CH_TRIGSRC_TCE0_CCC_gc = (0x84<<0),  /* Timer/Counter E0 Compare or Capture C */
    DMA_CH_TRIGSRC_TCE0_CCD_gc = (0x85<<0),  /* Timer/Counter E0 Compare or Capture D */
    DMA_CH_TRIGSRC_TCE1_OVF_gc = (0x86<<0),  /* Timer/Counter E1 Overflow */
    DMA_CH_TRIGSRC_TCE1_ERR_gc = (0x87<<0),  /* Timer/Counter E1 Error */
    DMA_CH_TRIGSRC_TCE1_CCA_gc = (0x88<<0),  /* Timer/Counter E1 Compare or Capture A */
    DMA_CH_TRIGSRC_TCE1_CCB_gc = (0x89<<0),  /* Timer/Counter E1 Compare or Capture B */
    DMA_CH_TRIGSRC_SPIE_gc = (0x8A<<0),  /* SPI E Transfer Complete */
    DMA_CH_TRIGSRC_USARTE0_RXC_gc = (0x8B<<0),  /* USART E0 Receive Complete */
    DMA_CH_TRIGSRC_USARTE0_DRE_gc = (0x8C<<0),  /* USART E0 Data Register Empty */
    DMA_CH_TRIGSRC_USARTE1_RXC_gc = (0x8E<<0),  /* USART E1 Receive Complete */
    DMA_CH_TRIGSRC_USARTE1_DRE_gc = (0x8F<<0),  /* USART E1 Data Register Empty */
    DMA_CH_TRIGSRC_TCF0_OVF_gc = (0xA0<<0),  /* Timer/Counter F0 Overflow */
    DMA_CH_TRIGSRC_TCF0_ERR_gc = (0xA1<<0),  /* Timer/Counter F0 Error */
    DMA_CH_TRIGSRC_TCF0_CCA_gc = (0xA2<<0),  /* Timer/Counter F0 Compare or Capture A */
    DMA_CH_TRIGSRC_TCF0_CCB_gc = (0xA3<<0),  /* Timer/Counter F0 Compare or Capture B */
    DMA_CH_TRIGSRC_TCF0_CCC_gc = (0xA4<<0),  /* Timer/Counter F0 Compare or Capture C */
    DMA_CH_TRIGSRC_TCF0_CCD_gc = (0xA5<<0),  /* Timer/Counter F0 Compare or Capture D */
    DMA_CH_TRIGSRC_TCF1_OVF_gc = (0xA6<<0),  /* Timer/Counter F1 Overflow */
    DMA_CH_TRIGSRC_TCF1_ERR_gc = (0xA7<<0),  /* Timer/Counter F1 Error */
    DMA_CH_TRIGSRC_TCF1_CCA_gc = (0xA8<<0),  /* Timer/Counter F1 Compare or Capture A */
    DMA_CH_TRIGSRC_TCF1_CCB_gc = (0xA9<<0),  /* Timer/Counter F1 Compare or Capture B */
    DMA_CH_TRIGSRC_SPIF_gc = (0xAA<<0),  /* SPI F Transfer Complete */
    DMA_CH_TRIGSRC_USARTF0_RXC_gc = (0xAB<<0),  /* USART F0 Receive Complete */
    DMA_CH_TRIGSRC_USARTF0_DRE_gc = (0xAC<<0),  /* USART F0 Data Register Empty */
    DMA_CH_TRIGSRC_USARTF1_RXC_gc = (0xAE<<0),  /* USART F1 Receive Complete */
    DMA_CH_TRIGSRC_USARTF1_DRE_gc = (0xAF<<0),  /* USART F1 Data Register Empty */
} DMA_CH_TRIGSRC_t;

/* Double buffering mode */
typedef enum DMA_DBUFMODE_enum
{
    DMA_DBUFMODE_DISABLED_gc = (0x00<<2),  /* Double buffering disabled */
    DMA_DBUFMODE_CH01_gc = (0x01<<2),  /* Double buffering enabled on channel 0/1 */
    DMA_DBUFMODE_CH23_gc = (0x02<<2),  /* Double buffering enabled on channel 2/3 */
    DMA_DBUFMODE_CH01CH23_gc = (0x03<<2),  /* Double buffering enabled on ch. 0/1 and ch. 2/3 */
} DMA_DBUFMODE_t;

/* Priority mode */
typedef enum DMA_PRIMODE_enum
{
    DMA_PRIMODE_RR0123_gc = (0x00<<0),  /* Round Robin */
    DMA_PRIMODE_CH0RR123_gc = (0x01<<0),  /* Channel 0 > Round Robin on channel 1/2/3 */
    DMA_PRIMODE_CH01RR23_gc = (0x02<<0),  /* Channel 0 > channel 1 > Round Robin on channel 2/3 */
    DMA_PRIMODE_CH0123_gc = (0x03<<0),  /* Channel 0 > channel 1 > channel 2 > channel 3 */
} DMA_PRIMODE_t;

/* Interrupt level */
typedef enum DMA_CH_ERRINTLVL_enum
{
    DMA_CH_ERRINTLVL_OFF_gc = (0x00<<2),  /* Interrupt disabled */
    DMA_CH_ERRINTLVL_LO_gc = (0x01<<2),  /* Low level */
    DMA_CH_ERRINTLVL_MED_gc = (0x02<<2),  /* Medium level */
    DMA_CH_ERRINTLVL_HI_gc = (0x03<<2),  /* High level */
} DMA_CH_ERRINTLVL_t;

/* Interrupt level */
typedef enum DMA_CH_TRNINTLVL_enum
{
    DMA_CH_TRNINTLVL_OFF_gc = (0x00<<0),  /* Interrupt disabled */
    DMA_CH_TRNINTLVL_LO_gc = (0x01<<0),  /* Low level */
    DMA_CH_TRNINTLVL_MED_gc = (0x02<<0),  /* Medium level */
    DMA_CH_TRNINTLVL_HI_gc = (0x03<<0),  /* High level */
} DMA_CH_TRNINTLVL_t;


/*
--------------------------------------------------------------------------
EVSYS - Event System
--------------------------------------------------------------------------
*/

/* Event System */
typedef struct EVSYS_struct
{
    register8_t CH0MUX;  /* Event Channel 0 Multiplexer */
    register8_t CH1MUX;  /* Event Channel 1 Multiplexer */
    register8_t CH2MUX;  /* Event Channel 2 Multiplexer */
    register8_t CH3MUX;  /* Event Channel 3 Multiplexer */
    register8_t CH4MUX;  /* Event Channel 4 Multiplexer */
    register8_t CH5MUX;  /* Event Channel 5 Multiplexer */
    register8_t CH6MUX;  /* Event Channel 6 Multiplexer */
    register8_t CH7MUX;  /* Event Channel 7 Multiplexer */
    register8_t CH0CTRL;  /* Channel 0 Control Register */
    register8_t CH1CTRL;  /* Channel 1 Control Register */
    register8_t CH2CTRL;  /* Channel 2 Control Register */
    register8_t CH3CTRL;  /* Channel 3 Control Register */
    register8_t CH4CTRL;  /* Channel 4 Control Register */
    register8_t CH5CTRL;  /* Channel 5 Control Register */
    register8_t CH6CTRL;  /* Channel 6 Control Register */
    register8_t CH7CTRL;  /* Channel 7 Control Register */
    register8_t STROBE;  /* Event Strobe */
    register8_t DATA;  /* Event Data */
} EVSYS_t;

/* Quadrature Decoder Index Recognition Mode */
typedef enum EVSYS_QDIRM_enum
{
    EVSYS_QDIRM_00_gc = (0x00<<5),  /* QDPH0 = 0, QDPH90 = 0 */
    EVSYS_QDIRM_01_gc = (0x01<<5),  /* QDPH0 = 0, QDPH90 = 1 */
    EVSYS_QDIRM_10_gc = (0x02<<5),  /* QDPH0 = 1, QDPH90 = 0 */
    EVSYS_QDIRM_11_gc = (0x03<<5),  /* QDPH0 = 1, QDPH90 = 1 */
} EVSYS_QDIRM_t;

/* Digital filter coefficient */
typedef enum EVSYS_DIGFILT_enum
{
    EVSYS_DIGFILT_1SAMPLE_gc = (0x00<<0),  /* 1 SAMPLE */
    EVSYS_DIGFILT_2SAMPLES_gc = (0x01<<0),  /* 2 SAMPLES */
    EVSYS_DIGFILT_3SAMPLES_gc = (0x02<<0),  /* 3 SAMPLES */
    EVSYS_DIGFILT_4SAMPLES_gc = (0x03<<0),  /* 4 SAMPLES */
    EVSYS_DIGFILT_5SAMPLES_gc = (0x04<<0),  /* 5 SAMPLES */
    EVSYS_DIGFILT_6SAMPLES_gc = (0x05<<0),  /* 6 SAMPLES */
    EVSYS_DIGFILT_7SAMPLES_gc = (0x06<<0),  /* 7 SAMPLES */
    EVSYS_DIGFILT_8SAMPLES_gc = (0x07<<0),  /* 8 SAMPLES */
} EVSYS_DIGFILT_t;

/* Event Channel multiplexer input selection */
typedef enum EVSYS_CHMUX_enum
{
    EVSYS_CHMUX_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_CHMUX_RTC_OVF_gc = (0x08<<0),  /* RTC Overflow */
    EVSYS_CHMUX_RTC_CMP_gc = (0x09<<0),  /* RTC Compare Match */
    EVSYS_CHMUX_ACA_CH0_gc = (0x10<<0),  /* Analog Comparator A Channel 0 */
    EVSYS_CHMUX_ACA_CH1_gc = (0x11<<0),  /* Analog Comparator A Channel 1 */
    EVSYS_CHMUX_ACA_WIN_gc = (0x12<<0),  /* Analog Comparator A Window */
    EVSYS_CHMUX_ACB_CH0_gc = (0x13<<0),  /* Analog Comparator B Channel 0 */
    EVSYS_CHMUX_ACB_CH1_gc = (0x14<<0),  /* Analog Comparator B Channel 1 */
    EVSYS_CHMUX_ACB_WIN_gc = (0x15<<0),  /* Analog Comparator B Window */
    EVSYS_CHMUX_ADCA_CH0_gc = (0x20<<0),  /* ADC A Channel 0 */
    EVSYS_CHMUX_ADCA_CH1_gc = (0x21<<0),  /* ADC A Channel 1 */
    EVSYS_CHMUX_ADCA_CH2_gc = (0x22<<0),  /* ADC A Channel 2 */
    EVSYS_CHMUX_ADCA_CH3_gc = (0x23<<0),  /* ADC A Channel 3 */
    EVSYS_CHMUX_ADCB_CH0_gc = (0x24<<0),  /* ADC B Channel 0 */
    EVSYS_CHMUX_ADCB_CH1_gc = (0x25<<0),  /* ADC B Channel 1 */
    EVSYS_CHMUX_ADCB_CH2_gc = (0x26<<0),  /* ADC B Channel 2 */
    EVSYS_CHMUX_ADCB_CH3_gc = (0x27<<0),  /* ADC B Channel 3 */
    EVSYS_CHMUX_PORTA_PIN0_gc = (0x50<<0),  /* Port A, Pin0 */
    EVSYS_CHMUX_PORTA_PIN1_gc = (0x51<<0),  /* Port A, Pin1 */
    EVSYS_CHMUX_PORTA_PIN2_gc = (0x52<<0),  /* Port A, Pin2 */
    EVSYS_CHMUX_PORTA_PIN3_gc = (0x53<<0),  /* Port A, Pin3 */
    EVSYS_CHMUX_PORTA_PIN4_gc = (0x54<<0),  /* Port A, Pin4 */
    EVSYS_CHMUX_PORTA_PIN5_gc = (0x55<<0),  /* Port A, Pin5 */
    EVSYS_CHMUX_PORTA_PIN6_gc = (0x56<<0),  /* Port A, Pin6 */
    EVSYS_CHMUX_PORTA_PIN7_gc = (0x57<<0),  /* Port A, Pin7 */
    EVSYS_CHMUX_PORTB_PIN0_gc = (0x58<<0),  /* Port B, Pin0 */
    EVSYS_CHMUX_PORTB_PIN1_gc = (0x59<<0),  /* Port B, Pin1 */
    EVSYS_CHMUX_PORTB_PIN2_gc = (0x5A<<0),  /* Port B, Pin2 */
    EVSYS_CHMUX_PORTB_PIN3_gc = (0x5B<<0),  /* Port B, Pin3 */
    EVSYS_CHMUX_PORTB_PIN4_gc = (0x5C<<0),  /* Port B, Pin4 */
    EVSYS_CHMUX_PORTB_PIN5_gc = (0x5D<<0),  /* Port B, Pin5 */
    EVSYS_CHMUX_PORTB_PIN6_gc = (0x5E<<0),  /* Port B, Pin6 */
    EVSYS_CHMUX_PORTB_PIN7_gc = (0x5F<<0),  /* Port B, Pin7 */
    EVSYS_CHMUX_PORTC_PIN0_gc = (0x60<<0),  /* Port C, Pin0 */
    EVSYS_CHMUX_PORTC_PIN1_gc = (0x61<<0),  /* Port C, Pin1 */
    EVSYS_CHMUX_PORTC_PIN2_gc = (0x62<<0),  /* Port C, Pin2 */
    EVSYS_CHMUX_PORTC_PIN3_gc = (0x63<<0),  /* Port C, Pin3 */
    EVSYS_CHMUX_PORTC_PIN4_gc = (0x64<<0),  /* Port C, Pin4 */
    EVSYS_CHMUX_PORTC_PIN5_gc = (0x65<<0),  /* Port C, Pin5 */
    EVSYS_CHMUX_PORTC_PIN6_gc = (0x66<<0),  /* Port C, Pin6 */
    EVSYS_CHMUX_PORTC_PIN7_gc = (0x67<<0),  /* Port C, Pin7 */
    EVSYS_CHMUX_PORTD_PIN0_gc = (0x68<<0),  /* Port D, Pin0 */
    EVSYS_CHMUX_PORTD_PIN1_gc = (0x69<<0),  /* Port D, Pin1 */
    EVSYS_CHMUX_PORTD_PIN2_gc = (0x6A<<0),  /* Port D, Pin2 */
    EVSYS_CHMUX_PORTD_PIN3_gc = (0x6B<<0),  /* Port D, Pin3 */
    EVSYS_CHMUX_PORTD_PIN4_gc = (0x6C<<0),  /* Port D, Pin4 */
    EVSYS_CHMUX_PORTD_PIN5_gc = (0x6D<<0),  /* Port D, Pin5 */
    EVSYS_CHMUX_PORTD_PIN6_gc = (0x6E<<0),  /* Port D, Pin6 */
    EVSYS_CHMUX_PORTD_PIN7_gc = (0x6F<<0),  /* Port D, Pin7 */
    EVSYS_CHMUX_PORTE_PIN0_gc = (0x70<<0),  /* Port E, Pin0 */
    EVSYS_CHMUX_PORTE_PIN1_gc = (0x71<<0),  /* Port E, Pin1 */
    EVSYS_CHMUX_PORTE_PIN2_gc = (0x72<<0),  /* Port E, Pin2 */
    EVSYS_CHMUX_PORTE_PIN3_gc = (0x73<<0),  /* Port E, Pin3 */
    EVSYS_CHMUX_PORTE_PIN4_gc = (0x74<<0),  /* Port E, Pin4 */
    EVSYS_CHMUX_PORTE_PIN5_gc = (0x75<<0),  /* Port E, Pin5 */
    EVSYS_CHMUX_PORTE_PIN6_gc = (0x76<<0),  /* Port E, Pin6 */
    EVSYS_CHMUX_PORTE_PIN7_gc = (0x77<<0),  /* Port E, Pin7 */
    EVSYS_CHMUX_PORTF_PIN0_gc = (0x78<<0),  /* Port F, Pin0 */
    EVSYS_CHMUX_PORTF_PIN1_gc = (0x79<<0),  /* Port F, Pin1 */
    EVSYS_CHMUX_PORTF_PIN2_gc = (0x7A<<0),  /* Port F, Pin2 */
    EVSYS_CHMUX_PORTF_PIN3_gc = (0x7B<<0),  /* Port F, Pin3 */
    EVSYS_CHMUX_PORTF_PIN4_gc = (0x7C<<0),  /* Port F, Pin4 */
    EVSYS_CHMUX_PORTF_PIN5_gc = (0x7D<<0),  /* Port F, Pin5 */
    EVSYS_CHMUX_PORTF_PIN6_gc = (0x7E<<0),  /* Port F, Pin6 */
    EVSYS_CHMUX_PORTF_PIN7_gc = (0x7F<<0),  /* Port F, Pin7 */
    EVSYS_CHMUX_PRESCALER_1_gc = (0x80<<0),  /* Prescaler, divide by 1 */
    EVSYS_CHMUX_PRESCALER_2_gc = (0x81<<0),  /* Prescaler, divide by 2 */
    EVSYS_CHMUX_PRESCALER_4_gc = (0x82<<0),  /* Prescaler, divide by 4 */
    EVSYS_CHMUX_PRESCALER_8_gc = (0x83<<0),  /* Prescaler, divide by 8 */
    EVSYS_CHMUX_PRESCALER_16_gc = (0x84<<0),  /* Prescaler, divide by 16 */
    EVSYS_CHMUX_PRESCALER_32_gc = (0x85<<0),  /* Prescaler, divide by 32 */
    EVSYS_CHMUX_PRESCALER_64_gc = (0x86<<0),  /* Prescaler, divide by 64 */
    EVSYS_CHMUX_PRESCALER_128_gc = (0x87<<0),  /* Prescaler, divide by 128 */
    EVSYS_CHMUX_PRESCALER_256_gc = (0x88<<0),  /* Prescaler, divide by 256 */
    EVSYS_CHMUX_PRESCALER_512_gc = (0x89<<0),  /* Prescaler, divide by 512 */
    EVSYS_CHMUX_PRESCALER_1024_gc = (0x8A<<0),  /* Prescaler, divide by 1024 */
    EVSYS_CHMUX_PRESCALER_2048_gc = (0x8B<<0),  /* Prescaler, divide by 2048 */
    EVSYS_CHMUX_PRESCALER_4096_gc = (0x8C<<0),  /* Prescaler, divide by 4096 */
    EVSYS_CHMUX_PRESCALER_8192_gc = (0x8D<<0),  /* Prescaler, divide by 8192 */
    EVSYS_CHMUX_PRESCALER_16384_gc = (0x8E<<0),  /* Prescaler, divide by 16384 */
    EVSYS_CHMUX_PRESCALER_32768_gc = (0x8F<<0),  /* Prescaler, divide by 32768 */
    EVSYS_CHMUX_TCC0_OVF_gc = (0xC0<<0),  /* Timer/Counter C0 Overflow */
    EVSYS_CHMUX_TCC0_ERR_gc = (0xC1<<0),  /* Timer/Counter C0 Error */
    EVSYS_CHMUX_TCC0_CCA_gc = (0xC4<<0),  /* Timer/Counter C0 Compare or Capture A */
    EVSYS_CHMUX_TCC0_CCB_gc = (0xC5<<0),  /* Timer/Counter C0 Compare or Capture B */
    EVSYS_CHMUX_TCC0_CCC_gc = (0xC6<<0),  /* Timer/Counter C0 Compare or Capture C */
    EVSYS_CHMUX_TCC0_CCD_gc = (0xC7<<0),  /* Timer/Counter C0 Compare or Capture D */
    EVSYS_CHMUX_TCC1_OVF_gc = (0xC8<<0),  /* Timer/Counter C1 Overflow */
    EVSYS_CHMUX_TCC1_ERR_gc = (0xC9<<0),  /* Timer/Counter C1 Error */
    EVSYS_CHMUX_TCC1_CCA_gc = (0xCC<<0),  /* Timer/Counter C1 Compare or Capture A */
    EVSYS_CHMUX_TCC1_CCB_gc = (0xCD<<0),  /* Timer/Counter C1 Compare or Capture B */
    EVSYS_CHMUX_TCD0_OVF_gc = (0xD0<<0),  /* Timer/Counter D0 Overflow */
    EVSYS_CHMUX_TCD0_ERR_gc = (0xD1<<0),  /* Timer/Counter D0 Error */
    EVSYS_CHMUX_TCD0_CCA_gc = (0xD4<<0),  /* Timer/Counter D0 Compare or Capture A */
    EVSYS_CHMUX_TCD0_CCB_gc = (0xD5<<0),  /* Timer/Counter D0 Compare or Capture B */
    EVSYS_CHMUX_TCD0_CCC_gc = (0xD6<<0),  /* Timer/Counter D0 Compare or Capture C */
    EVSYS_CHMUX_TCD0_CCD_gc = (0xD7<<0),  /* Timer/Counter D0 Compare or Capture D */
    EVSYS_CHMUX_TCD1_OVF_gc = (0xD8<<0),  /* Timer/Counter D1 Overflow */
    EVSYS_CHMUX_TCD1_ERR_gc = (0xD9<<0),  /* Timer/Counter D1 Error */
    EVSYS_CHMUX_TCD1_CCA_gc = (0xDC<<0),  /* Timer/Counter D1 Compare or Capture A */
    EVSYS_CHMUX_TCD1_CCB_gc = (0xDD<<0),  /* Timer/Counter D1 Compare or Capture B */
    EVSYS_CHMUX_TCE0_OVF_gc = (0xE0<<0),  /* Timer/Counter E0 Overflow */
    EVSYS_CHMUX_TCE0_ERR_gc = (0xE1<<0),  /* Timer/Counter E0 Error */
    EVSYS_CHMUX_TCE0_CCA_gc = (0xE4<<0),  /* Timer/Counter E0 Compare or Capture A */
    EVSYS_CHMUX_TCE0_CCB_gc = (0xE5<<0),  /* Timer/Counter E0 Compare or Capture B */
    EVSYS_CHMUX_TCE0_CCC_gc = (0xE6<<0),  /* Timer/Counter E0 Compare or Capture C */
    EVSYS_CHMUX_TCE0_CCD_gc = (0xE7<<0),  /* Timer/Counter E0 Compare or Capture D */
    EVSYS_CHMUX_TCE1_OVF_gc = (0xE8<<0),  /* Timer/Counter E1 Overflow */
    EVSYS_CHMUX_TCE1_ERR_gc = (0xE9<<0),  /* Timer/Counter E1 Error */
    EVSYS_CHMUX_TCE1_CCA_gc = (0xEC<<0),  /* Timer/Counter E1 Compare or Capture A */
    EVSYS_CHMUX_TCE1_CCB_gc = (0xED<<0),  /* Timer/Counter E1 Compare or Capture B */
    EVSYS_CHMUX_TCF0_OVF_gc = (0xF0<<0),  /* Timer/Counter F0 Overflow */
    EVSYS_CHMUX_TCF0_ERR_gc = (0xF1<<0),  /* Timer/Counter F0 Error */
    EVSYS_CHMUX_TCF0_CCA_gc = (0xF4<<0),  /* Timer/Counter F0 Compare or Capture A */
    EVSYS_CHMUX_TCF0_CCB_gc = (0xF5<<0),  /* Timer/Counter F0 Compare or Capture B */
    EVSYS_CHMUX_TCF0_CCC_gc = (0xF6<<0),  /* Timer/Counter F0 Compare or Capture C */
    EVSYS_CHMUX_TCF0_CCD_gc = (0xF7<<0),  /* Timer/Counter F0 Compare or Capture D */
    EVSYS_CHMUX_TCF1_OVF_gc = (0xF8<<0),  /* Timer/Counter F1 Overflow */
    EVSYS_CHMUX_TCF1_ERR_gc = (0xF9<<0),  /* Timer/Counter F1 Error */
    EVSYS_CHMUX_TCF1_CCA_gc = (0xFC<<0),  /* Timer/Counter F1 Compare or Capture A */
    EVSYS_CHMUX_TCF1_CCB_gc = (0xFD<<0),  /* Timer/Counter F1 Compare or Capture B */
} EVSYS_CHMUX_t;


/*
--------------------------------------------------------------------------
NVM - Non Volatile Memory Controller
--------------------------------------------------------------------------
*/

/* Non-volatile Memory Controller */
typedef struct NVM_struct
{
    register8_t ADDR0;  /* Address Register 0 */
    register8_t ADDR1;  /* Address Register 1 */
    register8_t ADDR2;  /* Address Register 2 */
    register8_t reserved_0x03;
    register8_t DATA0;  /* Data Register 0 */
    register8_t DATA1;  /* Data Register 1 */
    register8_t DATA2;  /* Data Register 2 */
    register8_t reserved_0x07;
    register8_t reserved_0x08;
    register8_t reserved_0x09;
    register8_t CMD;  /* Command */
    register8_t CTRLA;  /* Control Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t reserved_0x0E;
    register8_t STATUS;  /* Status */
    register8_t LOCKBITS;  /* Lock Bits */
} NVM_t;

/*
--------------------------------------------------------------------------
NVM - Non Volatile Memory Controller
--------------------------------------------------------------------------
*/

/* Lock Bits */
typedef struct NVM_LOCKBITS_struct
{
    register8_t LOCKBITS;  /* Lock Bits */
} NVM_LOCKBITS_t;

/*
--------------------------------------------------------------------------
NVM - Non Volatile Memory Controller
--------------------------------------------------------------------------
*/

/* Fuses */
typedef struct NVM_FUSES_struct
{
    register8_t FUSEBYTE0;  /* JTAG User ID */
    register8_t FUSEBYTE1;  /* Watchdog Configuration */
    register8_t FUSEBYTE2;  /* Reset Configuration */
    register8_t reserved_0x03;
    register8_t FUSEBYTE4;  /* Start-up Configuration */
    register8_t FUSEBYTE5;  /* EESAVE and BOD Level */
} NVM_FUSES_t;

/*
--------------------------------------------------------------------------
NVM - Non Volatile Memory Controller
--------------------------------------------------------------------------
*/

/* Production Signatures */
typedef struct NVM_PROD_SIGNATURES_struct
{
    register8_t RCOSC2M;  /* RCOSC 2MHz Calibration Value */
    register8_t reserved_0x01;
    register8_t RCOSC32K;  /* RCOSC 32kHz Calibration Value */
    register8_t RCOSC32M;  /* RCOSC 32MHz Calibration Value */
    register8_t reserved_0x04;
    register8_t reserved_0x05;
    register8_t reserved_0x06;
    register8_t reserved_0x07;
    register8_t LOTNUM0;  /* Lot Number Byte 0, ASCII */
    register8_t LOTNUM1;  /* Lot Number Byte 1, ASCII */
    register8_t LOTNUM2;  /* Lot Number Byte 2, ASCII */
    register8_t LOTNUM3;  /* Lot Number Byte 3, ASCII */
    register8_t LOTNUM4;  /* Lot Number Byte 4, ASCII */
    register8_t LOTNUM5;  /* Lot Number Byte 5, ASCII */
    register8_t reserved_0x0E;
    register8_t reserved_0x0F;
    register8_t WAFNUM;  /* Wafer Number */
    register8_t reserved_0x11;
    register8_t COORDX0;  /* Wafer Coordinate X Byte 0 */
    register8_t COORDX1;  /* Wafer Coordinate X Byte 1 */
    register8_t COORDY0;  /* Wafer Coordinate Y Byte 0 */
    register8_t COORDY1;  /* Wafer Coordinate Y Byte 1 */
    register8_t reserved_0x16;
    register8_t reserved_0x17;
    register8_t reserved_0x18;
    register8_t reserved_0x19;
    register8_t reserved_0x1A;
    register8_t reserved_0x1B;
    register8_t reserved_0x1C;
    register8_t reserved_0x1D;
    register8_t reserved_0x1E;
    register8_t reserved_0x1F;
    register8_t ADCACAL0;  /* ADCA Calibration Byte 0 */
    register8_t ADCACAL1;  /* ADCA Calibration Byte 1 */
    register8_t reserved_0x22;
    register8_t reserved_0x23;
    register8_t ADCBCAL0;  /* ADCB Calibration Byte 0 */
    register8_t ADCBCAL1;  /* ADCB Calibration Byte 1 */
    register8_t reserved_0x26;
    register8_t reserved_0x27;
    register8_t reserved_0x28;
    register8_t reserved_0x29;
    register8_t reserved_0x2A;
    register8_t reserved_0x2B;
    register8_t reserved_0x2C;
    register8_t reserved_0x2D;
    register8_t TEMPSENSE0;  /* Temperature Sensor Calibration Byte 0 */
    register8_t TEMPSENSE1;  /* Temperature Sensor Calibration Byte 0 */
    register8_t DACAOFFCAL;  /* DACA Calibration Byte 0 */
    register8_t DACACAINCAL;  /* DACA Calibration Byte 1 */
    register8_t DACBOFFCAL;  /* DACB Calibration Byte 0 */
    register8_t DACBGAINCAL;  /* DACB Calibration Byte 1 */
    register8_t reserved_0x34;
    register8_t reserved_0x35;
    register8_t reserved_0x36;
    register8_t reserved_0x37;
    register8_t reserved_0x38;
    register8_t reserved_0x39;
    register8_t reserved_0x3A;
    register8_t reserved_0x3B;
    register8_t reserved_0x3C;
    register8_t reserved_0x3D;
    register8_t reserved_0x3E;
} NVM_PROD_SIGNATURES_t;

/* NVM Command */
typedef enum NVM_CMD_enum
{
    NVM_CMD_NO_OPERATION_gc = (0x00<<0),  /* Noop/Ordinary LPM */
    NVM_CMD_READ_CALIB_ROW_gc = (0x02<<0),  /* Read calibration row */
    NVM_CMD_READ_USER_SIG_ROW_gc = (0x01<<0),  /* Read user signature row */
    NVM_CMD_READ_EEPROM_gc = (0x06<<0),  /* Read EEPROM */
    NVM_CMD_READ_FUSES_gc = (0x07<<0),  /* Read fuse byte */
    NVM_CMD_WRITE_LOCK_BITS_gc = (0x08<<0),  /* Write lock bits */
    NVM_CMD_ERASE_USER_SIG_ROW_gc = (0x18<<0),  /* Erase user signature row */
    NVM_CMD_WRITE_USER_SIG_ROW_gc = (0x1A<<0),  /* Write user signature row */
    NVM_CMD_ERASE_APP_gc = (0x20<<0),  /* Erase Application Section */
    NVM_CMD_ERASE_APP_PAGE_gc = (0x22<<0),  /* Erase Application Section page */
    NVM_CMD_LOAD_FLASH_BUFFER_gc = (0x23<<0),  /* Load Flash page buffer */
    NVM_CMD_WRITE_APP_PAGE_gc = (0x24<<0),  /* Write Application Section page */
    NVM_CMD_ERASE_WRITE_APP_PAGE_gc = (0x25<<0),  /* Erase-and-write Application Section page */
    NVM_CMD_ERASE_FLASH_BUFFER_gc = (0x26<<0),  /* Erase/flush Flash page buffer */
    NVM_CMD_ERASE_BOOT_PAGE_gc = (0x2A<<0),  /* Erase Boot Section page */
    NVM_CMD_WRITE_BOOT_PAGE_gc = (0x2C<<0),  /* Write Boot Section page */
    NVM_CMD_ERASE_WRITE_BOOT_PAGE_gc = (0x2D<<0),  /* Erase-and-write Boot Section page */
    NVM_CMD_ERASE_EEPROM_gc = (0x30<<0),  /* Erase EEPROM */
    NVM_CMD_ERASE_EEPROM_PAGE_gc = (0x32<<0),  /* Erase EEPROM page */
    NVM_CMD_LOAD_EEPROM_BUFFER_gc = (0x33<<0),  /* Load EEPROM page buffer */
    NVM_CMD_WRITE_EEPROM_PAGE_gc = (0x34<<0),  /* Write EEPROM page */
    NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc = (0x35<<0),  /* Erase-and-write EEPROM page */
    NVM_CMD_ERASE_EEPROM_BUFFER_gc = (0x36<<0),  /* Erase/flush EEPROM page buffer */
    NVM_CMD_APP_CRC_gc = (0x38<<0),  /* Generate Application section CRC */
    NVM_CMD_BOOT_CRC_gc = (0x39<<0),  /* Generate Boot Section CRC */
    NVM_CMD_FLASH_RANGE_CRC_gc = (0x3A<<0),  /* Generate Flash Range CRC */
} NVM_CMD_t;

/* SPM ready interrupt level */
typedef enum NVM_SPMLVL_enum
{
    NVM_SPMLVL_OFF_gc = (0x00<<2),  /* Interrupt disabled */
    NVM_SPMLVL_LO_gc = (0x01<<2),  /* Low level */
    NVM_SPMLVL_MED_gc = (0x02<<2),  /* Medium level */
    NVM_SPMLVL_HI_gc = (0x03<<2),  /* High level */
} NVM_SPMLVL_t;

/* EEPROM ready interrupt level */
typedef enum NVM_EELVL_enum
{
    NVM_EELVL_OFF_gc = (0x00<<0),  /* Interrupt disabled */
    NVM_EELVL_LO_gc = (0x01<<0),  /* Low level */
    NVM_EELVL_MED_gc = (0x02<<0),  /* Medium level */
    NVM_EELVL_HI_gc = (0x03<<0),  /* High level */
} NVM_EELVL_t;

/* Boot lock bits - boot setcion */
typedef enum NVM_BLBB_enum
{
    NVM_BLBB_NOLOCK_gc = (0x03<<6),  /* No locks */
    NVM_BLBB_WLOCK_gc = (0x02<<6),  /* Write not allowed */
    NVM_BLBB_RLOCK_gc = (0x01<<6),  /* Read not allowed */
    NVM_BLBB_RWLOCK_gc = (0x00<<6),  /* Read and write not allowed */
} NVM_BLBB_t;

/* Boot lock bits - application section */
typedef enum NVM_BLBA_enum
{
    NVM_BLBA_NOLOCK_gc = (0x03<<4),  /* No locks */
    NVM_BLBA_WLOCK_gc = (0x02<<4),  /* Write not allowed */
    NVM_BLBA_RLOCK_gc = (0x01<<4),  /* Read not allowed */
    NVM_BLBA_RWLOCK_gc = (0x00<<4),  /* Read and write not allowed */
} NVM_BLBA_t;

/* Boot lock bits - application table section */
typedef enum NVM_BLBAT_enum
{
    NVM_BLBAT_NOLOCK_gc = (0x03<<2),  /* No locks */
    NVM_BLBAT_WLOCK_gc = (0x02<<2),  /* Write not allowed */
    NVM_BLBAT_RLOCK_gc = (0x01<<2),  /* Read not allowed */
    NVM_BLBAT_RWLOCK_gc = (0x00<<2),  /* Read and write not allowed */
} NVM_BLBAT_t;

/* Lock bits */
typedef enum NVM_LB_enum
{
    NVM_LB_NOLOCK_gc = (0x03<<0),  /* No locks */
    NVM_LB_WLOCK_gc = (0x02<<0),  /* Write not allowed */
    NVM_LB_RWLOCK_gc = (0x00<<0),  /* Read and write not allowed */
} NVM_LB_t;

/* Boot Loader Section Reset Vector */
typedef enum BOOTRST_enum
{
    BOOTRST_BOOTLDR_gc = (0x00<<6),  /* Boot Loader Reset */
    BOOTRST_APPLICATION_gc = (0x01<<6),  /* Application Reset */
} BOOTRST_t;

/* BOD operation */
typedef enum BOD_enum
{
    BOD_INSAMPLEDMODE_gc = (0x01<<2),  /* BOD enabled in sampled mode */
    BOD_CONTINOUSLY_gc = (0x02<<2),  /* BOD enabled continuously */
    BOD_DISABLED_gc = (0x03<<2),  /* BOD Disabled */
} BOD_t;

/* Watchdog (Window) Timeout Period */
typedef enum WD_enum
{
    WD_8CLK_gc = (0x00<<4),  /* 8 cycles (8ms @ 3.3V) */
    WD_16CLK_gc = (0x01<<4),  /* 16 cycles (16ms @ 3.3V) */
    WD_32CLK_gc = (0x02<<4),  /* 32 cycles (32ms @ 3.3V) */
    WD_64CLK_gc = (0x03<<4),  /* 64 cycles (64ms @ 3.3V) */
    WD_128CLK_gc = (0x04<<4),  /* 128 cycles (0.125s @ 3.3V) */
    WD_256CLK_gc = (0x05<<4),  /* 256 cycles (0.25s @ 3.3V) */
    WD_512CLK_gc = (0x06<<4),  /* 512 cycles (0.5s @ 3.3V) */
    WD_1KCLK_gc = (0x07<<4),  /* 1K cycles (1s @ 3.3V) */
    WD_2KCLK_gc = (0x08<<4),  /* 2K cycles (2s @ 3.3V) */
    WD_4KCLK_gc = (0x09<<4),  /* 4K cycles (4s @ 3.3V) */
    WD_8KCLK_gc = (0x0A<<4),  /* 8K cycles (8s @ 3.3V) */
} WD_t;

/* Start-up Time */
typedef enum SUT_enum
{
    SUT_0MS_gc = (0x03<<2),  /* 0 ms */
    SUT_4MS_gc = (0x01<<2),  /* 4 ms */
    SUT_64MS_gc = (0x00<<2),  /* 64 ms */
} SUT_t;

/* Brown Out Detection Voltage Level */
typedef enum BODLVL_enum
{
    BODLVL_1V6_gc = (0x07<<0),  /* 1.6 V */
    BODLVL_1V8_gc = (0x06<<0),  /* 1.8 V */
    BODLVL_2V0_gc = (0x05<<0),  /* 2.0 V */
    BODLVL_2V2_gc = (0x04<<0),  /* 2.2 V */
    BODLVL_2V4_gc = (0x03<<0),  /* 2.4 V */
    BODLVL_2V7_gc = (0x02<<0),  /* 2.7 V */
    BODLVL_2V9_gc = (0x01<<0),  /* 2.9 V */
    BODLVL_3V2_gc = (0x00<<0),  /* 3.2 V */
} BODLVL_t;


/*
--------------------------------------------------------------------------
AC - Analog Comparator
--------------------------------------------------------------------------
*/

/* Analog Comparator */
typedef struct AC_struct
{
    register8_t AC0CTRL;  /* Comparator 0 Control */
    register8_t AC1CTRL;  /* Comparator 1 Control */
    register8_t AC0MUXCTRL;  /* Comparator 0 MUX Control */
    register8_t AC1MUXCTRL;  /* Comparator 1 MUX Control */
    register8_t CTRLA;  /* Control Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t WINCTRL;  /* Window Mode Control */
    register8_t STATUS;  /* Status */
} AC_t;

/* Interrupt mode */
typedef enum AC_INTMODE_enum
{
    AC_INTMODE_BOTHEDGES_gc = (0x00<<6),  /* Interrupt on both edges */
    AC_INTMODE_FALLING_gc = (0x02<<6),  /* Interrupt on falling edge */
    AC_INTMODE_RISING_gc = (0x03<<6),  /* Interrupt on rising edge */
} AC_INTMODE_t;

/* Interrupt level */
typedef enum AC_INTLVL_enum
{
    AC_INTLVL_OFF_gc = (0x00<<4),  /* Interrupt disabled */
    AC_INTLVL_LO_gc = (0x01<<4),  /* Low level */
    AC_INTLVL_MED_gc = (0x02<<4),  /* Medium level */
    AC_INTLVL_HI_gc = (0x03<<4),  /* High level */
} AC_INTLVL_t;

/* Hysteresis mode selection */
typedef enum AC_HYSMODE_enum
{
    AC_HYSMODE_NO_gc = (0x00<<1),  /* No hysteresis */
    AC_HYSMODE_SMALL_gc = (0x01<<1),  /* Small hysteresis */
    AC_HYSMODE_LARGE_gc = (0x02<<1),  /* Large hysteresis */
} AC_HYSMODE_t;

/* Positive input multiplexer selection */
typedef enum AC_MUXPOS_enum
{
    AC_MUXPOS_PIN0_gc = (0x00<<3),  /* Pin 0 */
    AC_MUXPOS_PIN1_gc = (0x01<<3),  /* Pin 1 */
    AC_MUXPOS_PIN2_gc = (0x02<<3),  /* Pin 2 */
    AC_MUXPOS_PIN3_gc = (0x03<<3),  /* Pin 3 */
    AC_MUXPOS_PIN4_gc = (0x04<<3),  /* Pin 4 */
    AC_MUXPOS_PIN5_gc = (0x05<<3),  /* Pin 5 */
    AC_MUXPOS_PIN6_gc = (0x06<<3),  /* Pin 6 */
    AC_MUXPOS_DAC_gc = (0x07<<3),  /* DAC output */
} AC_MUXPOS_t;

/* Negative input multiplexer selection */
typedef enum AC_MUXNEG_enum
{
    AC_MUXNEG_PIN0_gc = (0x00<<0),  /* Pin 0 */
    AC_MUXNEG_PIN1_gc = (0x01<<0),  /* Pin 1 */
    AC_MUXNEG_PIN3_gc = (0x02<<0),  /* Pin 3 */
    AC_MUXNEG_PIN5_gc = (0x03<<0),  /* Pin 5 */
    AC_MUXNEG_PIN7_gc = (0x04<<0),  /* Pin 7 */
    AC_MUXNEG_DAC_gc = (0x05<<0),  /* DAC output */
    AC_MUXNEG_BANDGAP_gc = (0x06<<0),  /* Bandgap Reference */
    AC_MUXNEG_SCALER_gc = (0x07<<0),  /* Internal voltage scaler */
} AC_MUXNEG_t;

/* Windows interrupt mode */
typedef enum AC_WINTMODE_enum
{
    AC_WINTMODE_ABOVE_gc = (0x00<<2),  /* Interrupt on above window */
    AC_WINTMODE_INSIDE_gc = (0x01<<2),  /* Interrupt on inside window */
    AC_WINTMODE_BELOW_gc = (0x02<<2),  /* Interrupt on below window */
    AC_WINTMODE_OUTSIDE_gc = (0x03<<2),  /* Interrupt on outside window */
} AC_WINTMODE_t;

/* Window interrupt level */
typedef enum AC_WINTLVL_enum
{
    AC_WINTLVL_OFF_gc = (0x00<<0),  /* Interrupt disabled */
    AC_WINTLVL_LO_gc = (0x01<<0),  /* Low priority */
    AC_WINTLVL_MED_gc = (0x02<<0),  /* Medium priority */
    AC_WINTLVL_HI_gc = (0x03<<0),  /* High priority */
} AC_WINTLVL_t;

/* Window mode state */
typedef enum AC_WSTATE_enum
{
    AC_WSTATE_ABOVE_gc = (0x00<<6),  /* Signal above window */
    AC_WSTATE_INSIDE_gc = (0x01<<6),  /* Signal inside window */
    AC_WSTATE_BELOW_gc = (0x02<<6),  /* Signal below window */
} AC_WSTATE_t;


/*
--------------------------------------------------------------------------
ADC - Analog/Digital Converter
--------------------------------------------------------------------------
*/

/* ADC Channel */
typedef struct ADC_CH_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t MUXCTRL;  /* MUX Control */
    register8_t INTCTRL;  /* Channel Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    _WORDREGISTER(RES);  /* Channel Result */
    register8_t reserved_0x6;
    register8_t reserved_0x7;
} ADC_CH_t;

/*
--------------------------------------------------------------------------
ADC - Analog/Digital Converter
--------------------------------------------------------------------------
*/

/* Analog-to-Digital Converter */
typedef struct ADC_struct
{
    register8_t CTRLA;  /* Control Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t REFCTRL;  /* Reference Control */
    register8_t EVCTRL;  /* Event Control */
    register8_t PRESCALER;  /* Clock Prescaler */
    register8_t CALCTRL;  /* Calibration Control Register */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t reserved_0x07;
    register8_t reserved_0x08;
    register8_t reserved_0x09;
    register8_t reserved_0x0A;
    register8_t reserved_0x0B;
    register8_t CALIB;  /* Calibration Value */
    register8_t reserved_0x0D;
    register8_t reserved_0x0E;
    register8_t reserved_0x0F;
    _WORDREGISTER(CH0RES);  /* Channel 0 Result */
    _WORDREGISTER(CH1RES);  /* Channel 1 Result */
    _WORDREGISTER(CH2RES);  /* Channel 2 Result */
    _WORDREGISTER(CH3RES);  /* Channel 3 Result */
    _WORDREGISTER(CMP);  /* Compare Value */
    register8_t reserved_0x1A;
    register8_t reserved_0x1B;
    register8_t reserved_0x1C;
    register8_t reserved_0x1D;
    register8_t reserved_0x1E;
    register8_t reserved_0x1F;
    ADC_CH_t CH0;  /* ADC Channel 0 */
    ADC_CH_t CH1;  /* ADC Channel 1 */
    ADC_CH_t CH2;  /* ADC Channel 2 */
    ADC_CH_t CH3;  /* ADC Channel 3 */
} ADC_t;

/* Positive input multiplexer selection */
typedef enum ADC_CH_MUXPOS_enum
{
    ADC_CH_MUXPOS_PIN0_gc = (0x00<<3),  /* Input pin 0 */
    ADC_CH_MUXPOS_PIN1_gc = (0x01<<3),  /* Input pin 1 */
    ADC_CH_MUXPOS_PIN2_gc = (0x02<<3),  /* Input pin 2 */
    ADC_CH_MUXPOS_PIN3_gc = (0x03<<3),  /* Input pin 3 */
    ADC_CH_MUXPOS_PIN4_gc = (0x04<<3),  /* Input pin 4 */
    ADC_CH_MUXPOS_PIN5_gc = (0x05<<3),  /* Input pin 5 */
    ADC_CH_MUXPOS_PIN6_gc = (0x06<<3),  /* Input pin 6 */
    ADC_CH_MUXPOS_PIN7_gc = (0x07<<3),  /* Input pin 7 */
} ADC_CH_MUXPOS_t;

/* Internal input multiplexer selections */
typedef enum ADC_CH_MUXINT_enum
{
    ADC_CH_MUXINT_TEMP_gc = (0x00<<3),  /* Temperature Reference */
    ADC_CH_MUXINT_BANDGAP_gc = (0x01<<3),  /* Bandgap Reference */
    ADC_CH_MUXINT_SCALEDVCC_gc = (0x02<<3),  /* 1/10 scaled VCC */
    ADC_CH_MUXINT_DAC_gc = (0x03<<3),  /* DAC output */
} ADC_CH_MUXINT_t;

/* Negative input multiplexer selection */
typedef enum ADC_CH_MUXNEG_enum
{
    ADC_CH_MUXNEG_PIN0_gc = (0x00<<0),  /* Input pin 0 */
    ADC_CH_MUXNEG_PIN1_gc = (0x01<<0),  /* Input pin 1 */
    ADC_CH_MUXNEG_PIN2_gc = (0x02<<0),  /* Input pin 2 */
    ADC_CH_MUXNEG_PIN3_gc = (0x03<<0),  /* Input pin 3 */
    ADC_CH_MUXNEG_PIN4_gc = (0x04<<0),  /* Input pin 4 */
    ADC_CH_MUXNEG_PIN5_gc = (0x05<<0),  /* Input pin 5 */
    ADC_CH_MUXNEG_PIN6_gc = (0x06<<0),  /* Input pin 6 */
    ADC_CH_MUXNEG_PIN7_gc = (0x07<<0),  /* Input pin 7 */
} ADC_CH_MUXNEG_t;

/* Input mode */
typedef enum ADC_CH_INPUTMODE_enum
{
    ADC_CH_INPUTMODE_INTERNAL_gc = (0x00<<0),  /* Internal inputs, no gain */
    ADC_CH_INPUTMODE_SINGLEENDED_gc = (0x01<<0),  /* Single-ended input, no gain */
    ADC_CH_INPUTMODE_DIFF_gc = (0x02<<0),  /* Differential input, no gain */
    ADC_CH_INPUTMODE_DIFFWGAIN_gc = (0x03<<0),  /* Differential input, with gain */
} ADC_CH_INPUTMODE_t;

/* Gain factor */
typedef enum ADC_CH_GAIN_enum
{
    ADC_CH_GAIN_1X_gc = (0x00<<2),  /* 1x gain */
    ADC_CH_GAIN_2X_gc = (0x01<<2),  /* 2x gain */
    ADC_CH_GAIN_4X_gc = (0x02<<2),  /* 4x gain */
    ADC_CH_GAIN_8X_gc = (0x03<<2),  /* 8x gain */
    ADC_CH_GAIN_16X_gc = (0x04<<2),  /* 16x gain */
    ADC_CH_GAIN_32X_gc = (0x05<<2),  /* 32x gain */
    ADC_CH_GAIN_64X_gc = (0x06<<2),  /* 64x gain */
} ADC_CH_GAIN_t;

/* Conversion result resolution */
typedef enum ADC_RESOLUTION_enum
{
    ADC_RESOLUTION_12BIT_gc = (0x00<<1),  /* 12-bit right-adjusted result */
    ADC_RESOLUTION_8BIT_gc = (0x02<<1),  /* 8-bit right-adjusted result */
    ADC_RESOLUTION_LEFT12BIT_gc = (0x03<<1),  /* 12-bit left-adjusted result */
} ADC_RESOLUTION_t;

/* Voltage reference selection */
typedef enum ADC_REFSEL_enum
{
    ADC_REFSEL_INT1V_gc = (0x00<<4),  /* Internal 1V */
    ADC_REFSEL_VCC_gc = (0x01<<4),  /* Internal VCC / 1.6V */
    ADC_REFSEL_AREFA_gc = (0x02<<4),  /* External reference on PORT A */
    ADC_REFSEL_AREFB_gc = (0x03<<4),  /* External reference on PORT B */
} ADC_REFSEL_t;

/* Channel sweep selection */
typedef enum ADC_SWEEP_enum
{
    ADC_SWEEP_0_gc = (0x00<<6),  /* ADC Channel 0 */
    ADC_SWEEP_01_gc = (0x01<<6),  /* ADC Channel 0,1 */
    ADC_SWEEP_012_gc = (0x02<<6),  /* ADC Channel 0,1,2 */
    ADC_SWEEP_0123_gc = (0x03<<6),  /* ADC Channel 0,1,2,3 */
} ADC_SWEEP_t;

/* Event channel input selection */
typedef enum ADC_EVSEL_enum
{
    ADC_EVSEL_0123_gc = (0x00<<3),  /* Event Channel 0,1,2,3 */
    ADC_EVSEL_1234_gc = (0x01<<3),  /* Event Channel 1,2,3,4 */
    ADC_EVSEL_2345_gc = (0x02<<3),  /* Event Channel 2,3,4,5 */
    ADC_EVSEL_3456_gc = (0x03<<3),  /* Event Channel 3,4,5,6 */
    ADC_EVSEL_4567_gc = (0x04<<3),  /* Event Channel 4,5,6,7 */
    ADC_EVSEL_567_gc = (0x05<<3),  /* Event Channel 5,6,7 */
    ADC_EVSEL_67_gc = (0x06<<3),  /* Event Channel 6,7 */
    ADC_EVSEL_7_gc = (0x07<<3),  /* Event Channel 7 */
} ADC_EVSEL_t;

/* Event action selection */
typedef enum ADC_EVACT_enum
{
    ADC_EVACT_NONE_gc = (0x00<<0),  /* No event action */
    ADC_EVACT_CH0_gc = (0x01<<0),  /* First event triggers channel 0 */
    ADC_EVACT_CH01_gc = (0x02<<0),  /* First two events trigger channel 0,1 */
    ADC_EVACT_CH012_gc = (0x03<<0),  /* First three events trigger channel 0,1,2 */
    ADC_EVACT_CH0123_gc = (0x04<<0),  /* Events trigger channel 0,1,2,3 */
    ADC_EVACT_SWEEP_gc = (0x05<<0),  /* First event triggers sweep */
    ADC_EVACT_SYNCHSWEEP_gc = (0x06<<0),  /* First event triggers synchronized sweep */
} ADC_EVACT_t;

/* Interupt mode */
typedef enum ADC_CH_INTMODE_enum
{
    ADC_CH_INTMODE_COMPLETE_gc = (0x00<<2),  /* Interrupt on conversion complete */
    ADC_CH_INTMODE_BELOW_gc = (0x01<<2),  /* Interrupt on result below compare value */
    ADC_CH_INTMODE_ABOVE_gc = (0x03<<2),  /* Interrupt on result above compare value */
} ADC_CH_INTMODE_t;

/* Interrupt level */
typedef enum ADC_CH_INTLVL_enum
{
    ADC_CH_INTLVL_OFF_gc = (0x00<<0),  /* Interrupt disabled */
    ADC_CH_INTLVL_LO_gc = (0x01<<0),  /* Low level */
    ADC_CH_INTLVL_MED_gc = (0x02<<0),  /* Medium level */
    ADC_CH_INTLVL_HI_gc = (0x03<<0),  /* High level */
} ADC_CH_INTLVL_t;

/* DMA request selection */
typedef enum ADC_DMASEL_enum
{
    ADC_DMASEL_OFF_gc = (0x00<<5),  /* Combined DMA request OFF */
    ADC_DMASEL_CH01_gc = (0x01<<5),  /* ADC Channel 0 or 1 */
    ADC_DMASEL_CH012_gc = (0x02<<5),  /* ADC Channel 0 or 1 or 2 */
    ADC_DMASEL_CH0123_gc = (0x03<<5),  /* ADC Channel 0 or 1 or 2 or 3 */
} ADC_DMASEL_t;

/* Clock prescaler */
typedef enum ADC_PRESCALER_enum
{
    ADC_PRESCALER_DIV4_gc = (0x00<<0),  /* Divide clock by 4 */
    ADC_PRESCALER_DIV8_gc = (0x01<<0),  /* Divide clock by 8 */
    ADC_PRESCALER_DIV16_gc = (0x02<<0),  /* Divide clock by 16 */
    ADC_PRESCALER_DIV32_gc = (0x03<<0),  /* Divide clock by 32 */
    ADC_PRESCALER_DIV64_gc = (0x04<<0),  /* Divide clock by 64 */
    ADC_PRESCALER_DIV128_gc = (0x05<<0),  /* Divide clock by 128 */
    ADC_PRESCALER_DIV256_gc = (0x06<<0),  /* Divide clock by 256 */
    ADC_PRESCALER_DIV512_gc = (0x07<<0),  /* Divide clock by 512 */
} ADC_PRESCALER_t;


/*
--------------------------------------------------------------------------
DAC - Digital/Analog Converter
--------------------------------------------------------------------------
*/

/* Digital-to-Analog Converter */
typedef struct DAC_struct
{
    register8_t CTRLA;  /* Control Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t CTRLC;  /* Control Register C */
    register8_t EVCTRL;  /* Event Input Control */
    register8_t TIMCTRL;  /* Timing Control */
    register8_t STATUS;  /* Status */
    register8_t reserved_0x06;
    register8_t reserved_0x07;
    register8_t GAINCAL;  /* Gain Calibration */
    register8_t OFFSETCAL;  /* Offset Calibration */
    register8_t reserved_0x0A;
    register8_t reserved_0x0B;
    register8_t reserved_0x0C;
    register8_t reserved_0x0D;
    register8_t reserved_0x0E;
    register8_t reserved_0x0F;
    register8_t reserved_0x10;
    register8_t reserved_0x11;
    register8_t reserved_0x12;
    register8_t reserved_0x13;
    register8_t reserved_0x14;
    register8_t reserved_0x15;
    register8_t reserved_0x16;
    register8_t reserved_0x17;
    _WORDREGISTER(CH0DATA);  /* Channel 0 Data */
    _WORDREGISTER(CH1DATA);  /* Channel 1 Data */
} DAC_t;

/* Output channel selection */
typedef enum DAC_CHSEL_enum
{
    DAC_CHSEL_SINGLE_gc = (0x00<<5),  /* Single channel operation (Channel A only) */
    DAC_CHSEL_DUAL_gc = (0x02<<5),  /* Dual channel operation (S/H on both channels) */
} DAC_CHSEL_t;

/* Reference voltage selection */
typedef enum DAC_REFSEL_enum
{
    DAC_REFSEL_INT1V_gc = (0x00<<3),  /* Internal 1V  */
    DAC_REFSEL_AVCC_gc = (0x01<<3),  /* Analog supply voltage */
    DAC_REFSEL_AREFA_gc = (0x02<<3),  /* External reference on AREF on PORTA */
    DAC_REFSEL_AREFB_gc = (0x03<<3),  /* External reference on AREF on PORTB */
} DAC_REFSEL_t;

/* Event channel selection */
typedef enum DAC_EVSEL_enum
{
    DAC_EVSEL_0_gc = (0x00<<0),  /* Event Channel 0 */
    DAC_EVSEL_1_gc = (0x01<<0),  /* Event Channel 1 */
    DAC_EVSEL_2_gc = (0x02<<0),  /* Event Channel 2 */
    DAC_EVSEL_3_gc = (0x03<<0),  /* Event Channel 3 */
    DAC_EVSEL_4_gc = (0x04<<0),  /* Event Channel 4 */
    DAC_EVSEL_5_gc = (0x05<<0),  /* Event Channel 5 */
    DAC_EVSEL_6_gc = (0x06<<0),  /* Event Channel 6 */
    DAC_EVSEL_7_gc = (0x07<<0),  /* Event Channel 7 */
} DAC_EVSEL_t;

/* Conversion interval */
typedef enum DAC_CONINTVAL_enum
{
    DAC_CONINTVAL_1CLK_gc = (0x00<<4),  /* 1 CLK / 2 CLK in S/H mode */
    DAC_CONINTVAL_2CLK_gc = (0x01<<4),  /* 2 CLK / 3 CLK in S/H mode */
    DAC_CONINTVAL_4CLK_gc = (0x02<<4),  /* 4 CLK / 6 CLK in S/H mode */
    DAC_CONINTVAL_8CLK_gc = (0x03<<4),  /* 8 CLK / 12 CLK in S/H mode */
    DAC_CONINTVAL_16CLK_gc = (0x04<<4),  /* 16 CLK / 24 CLK in S/H mode */
    DAC_CONINTVAL_32CLK_gc = (0x05<<4),  /* 32 CLK / 48 CLK in S/H mode */
    DAC_CONINTVAL_64CLK_gc = (0x06<<4),  /* 64 CLK / 96 CLK in S/H mode */
    DAC_CONINTVAL_128CLK_gc = (0x07<<4),  /* 128 CLK / 192 CLK in S/H mode */
} DAC_CONINTVAL_t;

/* Refresh rate */
typedef enum DAC_REFRESH_enum
{
    DAC_REFRESH_16CLK_gc = (0x00<<0),  /* 16 CLK */
    DAC_REFRESH_32CLK_gc = (0x01<<0),  /* 32 CLK */
    DAC_REFRESH_64CLK_gc = (0x02<<0),  /* 64 CLK */
    DAC_REFRESH_128CLK_gc = (0x03<<0),  /* 128 CLK */
    DAC_REFRESH_256CLK_gc = (0x04<<0),  /* 256 CLK */
    DAC_REFRESH_512CLK_gc = (0x05<<0),  /* 512 CLK */
    DAC_REFRESH_1024CLK_gc = (0x06<<0),  /* 1024 CLK */
    DAC_REFRESH_2048CLK_gc = (0x07<<0),  /* 2048 CLK */
    DAC_REFRESH_4086CLK_gc = (0x08<<0),  /* 4096 CLK */
    DAC_REFRESH_8192CLK_gc = (0x09<<0),  /* 8192 CLK */
    DAC_REFRESH_16384CLK_gc = (0x0A<<0),  /* 16384 CLK */
    DAC_REFRESH_32768CLK_gc = (0x0B<<0),  /* 32768 CLK */
    DAC_REFRESH_65536CLK_gc = (0x0C<<0),  /* 65536 CLK */
    DAC_REFRESH_OFF_gc = (0x0F<<0),  /* Auto refresh OFF */
} DAC_REFRESH_t;


/*
--------------------------------------------------------------------------
RTC - Real-Time Clounter
--------------------------------------------------------------------------
*/

/* Real-Time Counter */
typedef struct RTC_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t STATUS;  /* Status Register */
    register8_t INTCTRL;  /* Interrupt Control Register */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t TEMP;  /* Temporary register */
    register8_t reserved_0x05;
    register8_t reserved_0x06;
    register8_t reserved_0x07;
    _WORDREGISTER(CNT);  /* Count Register */
    _WORDREGISTER(PER);  /* Period Register */
    _WORDREGISTER(COMP);  /* Compare Register */
} RTC_t;

/* Prescaler Factor */
typedef enum RTC_PRESCALER_enum
{
    RTC_PRESCALER_OFF_gc = (0x00<<0),  /* RTC Off */
    RTC_PRESCALER_DIV1_gc = (0x01<<0),  /* RTC Clock */
    RTC_PRESCALER_DIV2_gc = (0x02<<0),  /* RTC Clock / 2 */
    RTC_PRESCALER_DIV8_gc = (0x03<<0),  /* RTC Clock / 8 */
    RTC_PRESCALER_DIV16_gc = (0x04<<0),  /* RTC Clock / 16 */
    RTC_PRESCALER_DIV64_gc = (0x05<<0),  /* RTC Clock / 64 */
    RTC_PRESCALER_DIV256_gc = (0x06<<0),  /* RTC Clock / 256 */
    RTC_PRESCALER_DIV1024_gc = (0x07<<0),  /* RTC Clock / 1024 */
} RTC_PRESCALER_t;

/* Compare Interrupt level */
typedef enum RTC_COMPINTLVL_enum
{
    RTC_COMPINTLVL_OFF_gc = (0x00<<2),  /* Interrupt Disabled */
    RTC_COMPINTLVL_LO_gc = (0x01<<2),  /* Low Level */
    RTC_COMPINTLVL_MED_gc = (0x02<<2),  /* Medium Level */
    RTC_COMPINTLVL_HI_gc = (0x03<<2),  /* High Level */
} RTC_COMPINTLVL_t;

/* Overflow Interrupt level */
typedef enum RTC_OVFINTLVL_enum
{
    RTC_OVFINTLVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    RTC_OVFINTLVL_LO_gc = (0x01<<0),  /* Low Level */
    RTC_OVFINTLVL_MED_gc = (0x02<<0),  /* Medium Level */
    RTC_OVFINTLVL_HI_gc = (0x03<<0),  /* High Level */
} RTC_OVFINTLVL_t;


/*
--------------------------------------------------------------------------
EBI - External Bus Interface
--------------------------------------------------------------------------
*/

/* EBI Chip Select Module */
typedef struct EBI_CS_struct
{
    register8_t CTRLA;  /* Chip Select Control Register A */
    register8_t CTRLB;  /* Chip Select Control Register B */
    _WORDREGISTER(BASEADDR);  /* Chip Select Base Address */
} EBI_CS_t;

/*
--------------------------------------------------------------------------
EBI - External Bus Interface
--------------------------------------------------------------------------
*/

/* External Bus Interface */
typedef struct EBI_struct
{
    register8_t CTRL;  /* Control */
    register8_t SDRAMCTRLA;  /* SDRAM Control Register A */
    register8_t reserved_0x02;
    register8_t reserved_0x03;
    _WORDREGISTER(REFRESH);  /* SDRAM Refresh Period */
    _WORDREGISTER(INITDLY);  /* SDRAM Initialization Delay */
    register8_t SDRAMCTRLB;  /* SDRAM Control Register B */
    register8_t SDRAMCTRLC;  /* SDRAM Control Register C */
    register8_t reserved_0x0A;
    register8_t reserved_0x0B;
    register8_t reserved_0x0C;
    register8_t reserved_0x0D;
    register8_t reserved_0x0E;
    register8_t reserved_0x0F;
    EBI_CS_t CS0;  /* Chip Select 0 */
    EBI_CS_t CS1;  /* Chip Select 1 */
    EBI_CS_t CS2;  /* Chip Select 2 */
    EBI_CS_t CS3;  /* Chip Select 3 */
} EBI_t;

/* Chip Select adress space */
typedef enum EBI_CS_ASPACE_enum
{
    EBI_CS_ASPACE_256B_gc = (0x00<<2),  /* 256 bytes */
    EBI_CS_ASPACE_512B_gc = (0x01<<2),  /* 512 bytes */
    EBI_CS_ASPACE_1KB_gc = (0x02<<2),  /* 1K bytes */
    EBI_CS_ASPACE_2KB_gc = (0x03<<2),  /* 2K bytes */
    EBI_CS_ASPACE_4KB_gc = (0x04<<2),  /* 4K bytes */
    EBI_CS_ASPACE_8KB_gc = (0x05<<2),  /* 8K bytes */
    EBI_CS_ASPACE_16KB_gc = (0x06<<2),  /* 16K bytes */
    EBI_CS_ASPACE_32KB_gc = (0x07<<2),  /* 32K bytes */
    EBI_CS_ASPACE_64KB_gc = (0x08<<2),  /* 64K bytes */
    EBI_CS_ASPACE_128KB_gc = (0x09<<2),  /* 128K bytes */
    EBI_CS_ASPACE_256KB_gc = (0x0A<<2),  /* 256K bytes */
    EBI_CS_ASPACE_512KB_gc = (0x0B<<2),  /* 512K bytes */
    EBI_CS_ASPACE_1MB_gc = (0x0C<<2),  /* 1M bytes */
    EBI_CS_ASPACE_2MB_gc = (0x0D<<2),  /* 2M bytes */
    EBI_CS_ASPACE_4MB_gc = (0x0E<<2),  /* 4M bytes */
    EBI_CS_ASPACE_8MB_gc = (0x0F<<2),  /* 8M bytes */
    EBI_CS_ASPACE_16M_gc = (0x10<<2),  /* 16M bytes */
} EBI_CS_ASPACE_t;

/*  */
typedef enum EBI_CS_SRWS_enum
{
    EBI_CS_SRWS_0CLK_gc = (0x00<<0),  /* 0 cycles */
    EBI_CS_SRWS_1CLK_gc = (0x01<<0),  /* 1 cycle */
    EBI_CS_SRWS_2CLK_gc = (0x02<<0),  /* 2 cycles */
    EBI_CS_SRWS_3CLK_gc = (0x03<<0),  /* 3 cycles */
    EBI_CS_SRWS_4CLK_gc = (0x04<<0),  /* 4 cycles */
    EBI_CS_SRWS_5CLK_gc = (0x05<<0),  /* 5 cycle */
    EBI_CS_SRWS_6CLK_gc = (0x06<<0),  /* 6 cycles */
    EBI_CS_SRWS_7CLK_gc = (0x07<<0),  /* 7 cycles */
} EBI_CS_SRWS_t;

/* Chip Select address mode */
typedef enum EBI_CS_MODE_enum
{
    EBI_CS_MODE_DISABLED_gc = (0x00<<0),  /* Chip Select Disabled */
    EBI_CS_MODE_SRAM_gc = (0x01<<0),  /* Chip Select in SRAM mode */
    EBI_CS_MODE_LPC_gc = (0x02<<0),  /* Chip Select in SRAM LPC mode */
    EBI_CS_MODE_SDRAM_gc = (0x03<<0),  /* Chip Select in SDRAM mode */
} EBI_CS_MODE_t;

/* Chip Select SDRAM mode */
typedef enum EBI_CS_SDMODE_enum
{
    EBI_CS_SDMODE_NORMAL_gc = (0x00<<0),  /* Normal mode */
    EBI_CS_SDMODE_LOAD_gc = (0x01<<0),  /* Load Mode Register command mode */
} EBI_CS_SDMODE_t;

/*  */
typedef enum EBI_SDDATAW_enum
{
    EBI_SDDATAW_4BIT_gc = (0x00<<6),  /* 4-bit data bus */
    EBI_SDDATAW_8BIT_gc = (0x01<<6),  /* 8-bit data bus */
} EBI_SDDATAW_t;

/*  */
typedef enum EBI_LPCMODE_enum
{
    EBI_LPCMODE_ALE1_gc = (0x00<<4),  /* Data muxed with addr byte 0 */
    EBI_LPCMODE_ALE12_gc = (0x02<<4),  /* Data muxed with addr byte 0 and 1 */
} EBI_LPCMODE_t;

/*  */
typedef enum EBI_SRMODE_enum
{
    EBI_SRMODE_ALE1_gc = (0x00<<2),  /* Addr byte 0 muxed with 1 */
    EBI_SRMODE_ALE2_gc = (0x01<<2),  /* Addr byte 0 muxed with 2 */
    EBI_SRMODE_ALE12_gc = (0x02<<2),  /* Addr byte 0 muxed with 1 and 2 */
    EBI_SRMODE_NOALE_gc = (0x03<<2),  /* No addr muxing */
} EBI_SRMODE_t;

/*  */
typedef enum EBI_IFMODE_enum
{
    EBI_IFMODE_DISABLED_gc = (0x00<<0),  /* EBI Disabled */
    EBI_IFMODE_3PORT_gc = (0x01<<0),  /* 3-port mode */
    EBI_IFMODE_4PORT_gc = (0x02<<0),  /* 4-port mode */
    EBI_IFMODE_2PORT_gc = (0x03<<0),  /* 2-port mode */
} EBI_IFMODE_t;

/*  */
typedef enum EBI_SDCOL_enum
{
    EBI_SDCOL_8BIT_gc = (0x00<<0),  /* 8 column bits */
    EBI_SDCOL_9BIT_gc = (0x01<<0),  /* 9 column bits */
    EBI_SDCOL_10BIT_gc = (0x02<<0),  /* 10 column bits */
    EBI_SDCOL_11BIT_gc = (0x03<<0),  /* 11 column bits */
} EBI_SDCOL_t;

/*  */
typedef enum EBI_MRDLY_enum
{
    EBI_MRDLY_0CLK_gc = (0x00<<6),  /* 0 cycles */
    EBI_MRDLY_1CLK_gc = (0x01<<6),  /* 1 cycle */
    EBI_MRDLY_2CLK_gc = (0x02<<6),  /* 2 cycles */
    EBI_MRDLY_3CLK_gc = (0x03<<6),  /* 3 cycles */
} EBI_MRDLY_t;

/*  */
typedef enum EBI_ROWCYCDLY_enum
{
    EBI_ROWCYCDLY_0CLK_gc = (0x00<<3),  /* 0 cycles */
    EBI_ROWCYCDLY_1CLK_gc = (0x01<<3),  /* 1 cycle */
    EBI_ROWCYCDLY_2CLK_gc = (0x02<<3),  /* 2 cycles */
    EBI_ROWCYCDLY_3CLK_gc = (0x03<<3),  /* 3 cycles */
    EBI_ROWCYCDLY_4CLK_gc = (0x04<<3),  /* 4 cycles */
    EBI_ROWCYCDLY_5CLK_gc = (0x05<<3),  /* 5 cycle */
    EBI_ROWCYCDLY_6CLK_gc = (0x06<<3),  /* 6 cycles */
    EBI_ROWCYCDLY_7CLK_gc = (0x07<<3),  /* 7 cycles */
} EBI_ROWCYCDLY_t;

/*  */
typedef enum EBI_RPDLY_enum
{
    EBI_RPDLY_0CLK_gc = (0x00<<0),  /* 0 cycles */
    EBI_RPDLY_1CLK_gc = (0x01<<0),  /* 1 cycle */
    EBI_RPDLY_2CLK_gc = (0x02<<0),  /* 2 cycles */
    EBI_RPDLY_3CLK_gc = (0x03<<0),  /* 3 cycles */
    EBI_RPDLY_4CLK_gc = (0x04<<0),  /* 4 cycles */
    EBI_RPDLY_5CLK_gc = (0x05<<0),  /* 5 cycle */
    EBI_RPDLY_6CLK_gc = (0x06<<0),  /* 6 cycles */
    EBI_RPDLY_7CLK_gc = (0x07<<0),  /* 7 cycles */
} EBI_RPDLY_t;

/*  */
typedef enum EBI_WRDLY_enum
{
    EBI_WRDLY_0CLK_gc = (0x00<<6),  /* 0 cycles */
    EBI_WRDLY_1CLK_gc = (0x01<<6),  /* 1 cycle */
    EBI_WRDLY_2CLK_gc = (0x02<<6),  /* 2 cycles */
    EBI_WRDLY_3CLK_gc = (0x03<<6),  /* 3 cycles */
} EBI_WRDLY_t;

/*  */
typedef enum EBI_ESRDLY_enum
{
    EBI_ESRDLY_0CLK_gc = (0x00<<3),  /* 0 cycles */
    EBI_ESRDLY_1CLK_gc = (0x01<<3),  /* 1 cycle */
    EBI_ESRDLY_2CLK_gc = (0x02<<3),  /* 2 cycles */
    EBI_ESRDLY_3CLK_gc = (0x03<<3),  /* 3 cycles */
    EBI_ESRDLY_4CLK_gc = (0x04<<3),  /* 4 cycles */
    EBI_ESRDLY_5CLK_gc = (0x05<<3),  /* 5 cycle */
    EBI_ESRDLY_6CLK_gc = (0x06<<3),  /* 6 cycles */
    EBI_ESRDLY_7CLK_gc = (0x07<<3),  /* 7 cycles */
} EBI_ESRDLY_t;

/*  */
typedef enum EBI_ROWCOLDLY_enum
{
    EBI_ROWCOLDLY_0CLK_gc = (0x00<<0),  /* 0 cycles */
    EBI_ROWCOLDLY_1CLK_gc = (0x01<<0),  /* 1 cycle */
    EBI_ROWCOLDLY_2CLK_gc = (0x02<<0),  /* 2 cycles */
    EBI_ROWCOLDLY_3CLK_gc = (0x03<<0),  /* 3 cycles */
    EBI_ROWCOLDLY_4CLK_gc = (0x04<<0),  /* 4 cycles */
    EBI_ROWCOLDLY_5CLK_gc = (0x05<<0),  /* 5 cycle */
    EBI_ROWCOLDLY_6CLK_gc = (0x06<<0),  /* 6 cycles */
    EBI_ROWCOLDLY_7CLK_gc = (0x07<<0),  /* 7 cycles */
} EBI_ROWCOLDLY_t;


/*
--------------------------------------------------------------------------
TWI - Two-Wire Interface
--------------------------------------------------------------------------
*/

/*  */
typedef struct TWI_MASTER_struct
{
    register8_t CTRLA;  /* Control Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t CTRLC;  /* Control Register C */
    register8_t STATUS;  /* Status Register */
    register8_t BAUD;  /* Baurd Rate Control Register */
    register8_t ADDR;  /* Address Register */
    register8_t DATA;  /* Data Register */
} TWI_MASTER_t;

/*
--------------------------------------------------------------------------
TWI - Two-Wire Interface
--------------------------------------------------------------------------
*/

/*  */
typedef struct TWI_SLAVE_struct
{
    register8_t CTRLA;  /* Control Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t STATUS;  /* Status Register */
    register8_t ADDR;  /* Address Register */
    register8_t DATA;  /* Data Register */
} TWI_SLAVE_t;

/*
--------------------------------------------------------------------------
TWI - Two-Wire Interface
--------------------------------------------------------------------------
*/

/* Two-Wire Interface */
typedef struct TWI_struct
{
    register8_t CTRL;  /* TWI Common Control Register */
    TWI_MASTER_t MASTER;  /* TWI master module */
    TWI_SLAVE_t SLAVE;  /* TWI slave module */
} TWI_t;

/* Master Interrupt Level */
typedef enum TWI_MASTER_INTLVL_enum
{
    TWI_MASTER_INTLVL_OFF_gc = (0x00<<6),  /* Interrupt Disabled */
    TWI_MASTER_INTLVL_LO_gc = (0x01<<6),  /* Low Level */
    TWI_MASTER_INTLVL_MED_gc = (0x02<<6),  /* Medium Level */
    TWI_MASTER_INTLVL_HI_gc = (0x03<<6),  /* High Level */
} TWI_MASTER_INTLVL_t;

/* Inactive Timeout */
typedef enum TWI_MASTER_TIMEOUT_enum
{
    TWI_MASTER_TIMEOUT_DISABLED_gc = (0x00<<2),  /* Bus Timeout Disabled */
    TWI_MASTER_TIMEOUT_50US_gc = (0x01<<2),  /* 50 Microseconds */
    TWI_MASTER_TIMEOUT_100US_gc = (0x02<<2),  /* 100 Microseconds */
    TWI_MASTER_TIMEOUT_200US_gc = (0x03<<2),  /* 200 Microseconds */
} TWI_MASTER_TIMEOUT_t;

/* Master Command */
typedef enum TWI_MASTER_CMD_enum
{
    TWI_MASTER_CMD_NOACT_gc = (0x00<<0),  /* No Action */
    TWI_MASTER_CMD_REPSTART_gc = (0x01<<0),  /* Issue Repeated Start Condition */
    TWI_MASTER_CMD_RECVTRANS_gc = (0x02<<0),  /* Receive or Transmit Data */
    TWI_MASTER_CMD_STOP_gc = (0x03<<0),  /* Issue Stop Condition */
} TWI_MASTER_CMD_t;

/* Master Bus State */
typedef enum TWI_MASTER_BUSSTATE_enum
{
    TWI_MASTER_BUSSTATE_UNKNOWN_gc = (0x00<<0),  /* Unknown Bus State */
    TWI_MASTER_BUSSTATE_IDLE_gc = (0x01<<0),  /* Bus is Idle */
    TWI_MASTER_BUSSTATE_OWNER_gc = (0x02<<0),  /* This Module Controls The Bus */
    TWI_MASTER_BUSSTATE_BUSY_gc = (0x03<<0),  /* The Bus is Busy */
} TWI_MASTER_BUSSTATE_t;

/* Slave Interrupt Level */
typedef enum TWI_SLAVE_INTLVL_enum
{
    TWI_SLAVE_INTLVL_OFF_gc = (0x00<<6),  /* Interrupt Disabled */
    TWI_SLAVE_INTLVL_LO_gc = (0x01<<6),  /* Low Level */
    TWI_SLAVE_INTLVL_MED_gc = (0x02<<6),  /* Medium Level */
    TWI_SLAVE_INTLVL_HI_gc = (0x03<<6),  /* High Level */
} TWI_SLAVE_INTLVL_t;

/* Slave Command */
typedef enum TWI_SLAVE_CMD_enum
{
    TWI_SLAVE_CMD_NOACT_gc = (0x00<<0),  /* No Action */
    TWI_SLAVE_CMD_COMPTRANS_gc = (0x02<<0),  /* Used To Complete a Transaction */
    TWI_SLAVE_CMD_RESPONSE_gc = (0x03<<0),  /* Used in Response to Address/Data Interrupt */
} TWI_SLAVE_CMD_t;


/*
--------------------------------------------------------------------------
PORT - Port Configuration
--------------------------------------------------------------------------
*/

/* I/O port Configuration */
typedef struct PORTCFG_struct
{
    register8_t MPCMASK;  /* Multi-pin Configuration Mask */
    register8_t reserved_0x01;
    register8_t VPCTRLA;  /* Virtual Port Control Register A */
    register8_t VPCTRLB;  /* Virtual Port Control Register B */
    register8_t CLKEVOUT;  /* Clock and Event Out Register */
} PORTCFG_t;

/*
--------------------------------------------------------------------------
PORT - Port Configuration
--------------------------------------------------------------------------
*/

/* Virtual Port */
typedef struct VPORT_struct
{
    register8_t DIR;  /* I/O Port Data Direction */
    register8_t OUT;  /* I/O Port Output */
    register8_t IN;  /* I/O Port Input */
    register8_t INTFLAGS;  /* Interrupt Flag Register */
} VPORT_t;

/*
--------------------------------------------------------------------------
PORT - Port Configuration
--------------------------------------------------------------------------
*/

/* I/O Ports */
typedef struct PORT_struct
{
    register8_t DIR;  /* I/O Port Data Direction */
    register8_t DIRSET;  /* I/O Port Data Direction Set */
    register8_t DIRCLR;  /* I/O Port Data Direction Clear */
    register8_t DIRTGL;  /* I/O Port Data Direction Toggle */
    register8_t OUT;  /* I/O Port Output */
    register8_t OUTSET;  /* I/O Port Output Set */
    register8_t OUTCLR;  /* I/O Port Output Clear */
    register8_t OUTTGL;  /* I/O Port Output Toggle */
    register8_t IN;  /* I/O port Input */
    register8_t INTCTRL;  /* Interrupt Control Register */
    register8_t INT0MASK;  /* Port Interrupt 0 Mask */
    register8_t INT1MASK;  /* Port Interrupt 1 Mask */
    register8_t INTFLAGS;  /* Interrupt Flag Register */
    register8_t reserved_0x0D;
    register8_t reserved_0x0E;
    register8_t reserved_0x0F;
    register8_t PIN0CTRL;  /* Pin 0 Control Register */
    register8_t PIN1CTRL;  /* Pin 1 Control Register */
    register8_t PIN2CTRL;  /* Pin 2 Control Register */
    register8_t PIN3CTRL;  /* Pin 3 Control Register */
    register8_t PIN4CTRL;  /* Pin 4 Control Register */
    register8_t PIN5CTRL;  /* Pin 5 Control Register */
    register8_t PIN6CTRL;  /* Pin 6 Control Register */
    register8_t PIN7CTRL;  /* Pin 7 Control Register */
} PORT_t;

/* Virtual Port 0 Mapping */
typedef enum PORTCFG_VP0MAP_enum
{
    PORTCFG_VP0MAP_PORTA_gc = (0x00<<0),  /* Mapped To PORTA */
    PORTCFG_VP0MAP_PORTB_gc = (0x01<<0),  /* Mapped To PORTB */
    PORTCFG_VP0MAP_PORTC_gc = (0x02<<0),  /* Mapped To PORTC */
    PORTCFG_VP0MAP_PORTD_gc = (0x03<<0),  /* Mapped To PORTD */
    PORTCFG_VP0MAP_PORTE_gc = (0x04<<0),  /* Mapped To PORTE */
    PORTCFG_VP0MAP_PORTF_gc = (0x05<<0),  /* Mapped To PORTF */
    PORTCFG_VP0MAP_PORTG_gc = (0x06<<0),  /* Mapped To PORTG */
    PORTCFG_VP0MAP_PORTH_gc = (0x07<<0),  /* Mapped To PORTH */
    PORTCFG_VP0MAP_PORTJ_gc = (0x08<<0),  /* Mapped To PORTJ */
    PORTCFG_VP0MAP_PORTK_gc = (0x09<<0),  /* Mapped To PORTK */
    PORTCFG_VP0MAP_PORTL_gc = (0x0A<<0),  /* Mapped To PORTL */
    PORTCFG_VP0MAP_PORTM_gc = (0x0B<<0),  /* Mapped To PORTM */
    PORTCFG_VP0MAP_PORTN_gc = (0x0C<<0),  /* Mapped To PORTN */
    PORTCFG_VP0MAP_PORTP_gc = (0x0D<<0),  /* Mapped To PORTP */
    PORTCFG_VP0MAP_PORTQ_gc = (0x0E<<0),  /* Mapped To PORTQ */
    PORTCFG_VP0MAP_PORTR_gc = (0x0F<<0),  /* Mapped To PORTR */
} PORTCFG_VP0MAP_t;

/* Virtual Port 1 Mapping */
typedef enum PORTCFG_VP1MAP_enum
{
    PORTCFG_VP1MAP_PORTA_gc = (0x00<<4),  /* Mapped To PORTA */
    PORTCFG_VP1MAP_PORTB_gc = (0x01<<4),  /* Mapped To PORTB */
    PORTCFG_VP1MAP_PORTC_gc = (0x02<<4),  /* Mapped To PORTC */
    PORTCFG_VP1MAP_PORTD_gc = (0x03<<4),  /* Mapped To PORTD */
    PORTCFG_VP1MAP_PORTE_gc = (0x04<<4),  /* Mapped To PORTE */
    PORTCFG_VP1MAP_PORTF_gc = (0x05<<4),  /* Mapped To PORTF */
    PORTCFG_VP1MAP_PORTG_gc = (0x06<<4),  /* Mapped To PORTG */
    PORTCFG_VP1MAP_PORTH_gc = (0x07<<4),  /* Mapped To PORTH */
    PORTCFG_VP1MAP_PORTJ_gc = (0x08<<4),  /* Mapped To PORTJ */
    PORTCFG_VP1MAP_PORTK_gc = (0x09<<4),  /* Mapped To PORTK */
    PORTCFG_VP1MAP_PORTL_gc = (0x0A<<4),  /* Mapped To PORTL */
    PORTCFG_VP1MAP_PORTM_gc = (0x0B<<4),  /* Mapped To PORTM */
    PORTCFG_VP1MAP_PORTN_gc = (0x0C<<4),  /* Mapped To PORTN */
    PORTCFG_VP1MAP_PORTP_gc = (0x0D<<4),  /* Mapped To PORTP */
    PORTCFG_VP1MAP_PORTQ_gc = (0x0E<<4),  /* Mapped To PORTQ */
    PORTCFG_VP1MAP_PORTR_gc = (0x0F<<4),  /* Mapped To PORTR */
} PORTCFG_VP1MAP_t;

/* Virtual Port 2 Mapping */
typedef enum PORTCFG_VP2MAP_enum
{
    PORTCFG_VP2MAP_PORTA_gc = (0x00<<0),  /* Mapped To PORTA */
    PORTCFG_VP2MAP_PORTB_gc = (0x01<<0),  /* Mapped To PORTB */
    PORTCFG_VP2MAP_PORTC_gc = (0x02<<0),  /* Mapped To PORTC */
    PORTCFG_VP2MAP_PORTD_gc = (0x03<<0),  /* Mapped To PORTD */
    PORTCFG_VP2MAP_PORTE_gc = (0x04<<0),  /* Mapped To PORTE */
    PORTCFG_VP2MAP_PORTF_gc = (0x05<<0),  /* Mapped To PORTF */
    PORTCFG_VP2MAP_PORTG_gc = (0x06<<0),  /* Mapped To PORTG */
    PORTCFG_VP2MAP_PORTH_gc = (0x07<<0),  /* Mapped To PORTH */
    PORTCFG_VP2MAP_PORTJ_gc = (0x08<<0),  /* Mapped To PORTJ */
    PORTCFG_VP2MAP_PORTK_gc = (0x09<<0),  /* Mapped To PORTK */
    PORTCFG_VP2MAP_PORTL_gc = (0x0A<<0),  /* Mapped To PORTL */
    PORTCFG_VP2MAP_PORTM_gc = (0x0B<<0),  /* Mapped To PORTM */
    PORTCFG_VP2MAP_PORTN_gc = (0x0C<<0),  /* Mapped To PORTN */
    PORTCFG_VP2MAP_PORTP_gc = (0x0D<<0),  /* Mapped To PORTP */
    PORTCFG_VP2MAP_PORTQ_gc = (0x0E<<0),  /* Mapped To PORTQ */
    PORTCFG_VP2MAP_PORTR_gc = (0x0F<<0),  /* Mapped To PORTR */
} PORTCFG_VP2MAP_t;

/* Virtual Port 3 Mapping */
typedef enum PORTCFG_VP3MAP_enum
{
    PORTCFG_VP3MAP_PORTA_gc = (0x00<<4),  /* Mapped To PORTA */
    PORTCFG_VP3MAP_PORTB_gc = (0x01<<4),  /* Mapped To PORTB */
    PORTCFG_VP3MAP_PORTC_gc = (0x02<<4),  /* Mapped To PORTC */
    PORTCFG_VP3MAP_PORTD_gc = (0x03<<4),  /* Mapped To PORTD */
    PORTCFG_VP3MAP_PORTE_gc = (0x04<<4),  /* Mapped To PORTE */
    PORTCFG_VP3MAP_PORTF_gc = (0x05<<4),  /* Mapped To PORTF */
    PORTCFG_VP3MAP_PORTG_gc = (0x06<<4),  /* Mapped To PORTG */
    PORTCFG_VP3MAP_PORTH_gc = (0x07<<4),  /* Mapped To PORTH */
    PORTCFG_VP3MAP_PORTJ_gc = (0x08<<4),  /* Mapped To PORTJ */
    PORTCFG_VP3MAP_PORTK_gc = (0x09<<4),  /* Mapped To PORTK */
    PORTCFG_VP3MAP_PORTL_gc = (0x0A<<4),  /* Mapped To PORTL */
    PORTCFG_VP3MAP_PORTM_gc = (0x0B<<4),  /* Mapped To PORTM */
    PORTCFG_VP3MAP_PORTN_gc = (0x0C<<4),  /* Mapped To PORTN */
    PORTCFG_VP3MAP_PORTP_gc = (0x0D<<4),  /* Mapped To PORTP */
    PORTCFG_VP3MAP_PORTQ_gc = (0x0E<<4),  /* Mapped To PORTQ */
    PORTCFG_VP3MAP_PORTR_gc = (0x0F<<4),  /* Mapped To PORTR */
} PORTCFG_VP3MAP_t;

/* Clock Output Port */
typedef enum PORTCFG_CLKOUT_enum
{
    PORTCFG_CLKOUT_OFF_gc = (0x00<<0),  /* Clock Output Disabled */
    PORTCFG_CLKOUT_PC7_gc = (0x01<<0),  /* Clock Output on Port C pin 7 */
    PORTCFG_CLKOUT_PD7_gc = (0x02<<0),  /* Clock Output on Port D pin 7 */
    PORTCFG_CLKOUT_PE7_gc = (0x03<<0),  /* Clock Output on Port E pin 7 */
} PORTCFG_CLKOUT_t;

/* Event Output Port */
typedef enum PORTCFG_EVOUT_enum
{
    PORTCFG_EVOUT_OFF_gc = (0x00<<4),  /* Event Output Disabled */
    PORTCFG_EVOUT_PC7_gc = (0x01<<4),  /* Event Channel 7 Output on Port C pin 7 */
    PORTCFG_EVOUT_PD7_gc = (0x02<<4),  /* Event Channel 7 Output on Port D pin 7 */
    PORTCFG_EVOUT_PE7_gc = (0x03<<4),  /* Event Channel 7 Output on Port E pin 7 */
} PORTCFG_EVOUT_t;

/* Port Interrupt 0 Level */
typedef enum PORT_INT0LVL_enum
{
    PORT_INT0LVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    PORT_INT0LVL_LO_gc = (0x01<<0),  /* Low Level */
    PORT_INT0LVL_MED_gc = (0x02<<0),  /* Medium Level */
    PORT_INT0LVL_HI_gc = (0x03<<0),  /* High Level */
} PORT_INT0LVL_t;

/* Port Interrupt 1 Level */
typedef enum PORT_INT1LVL_enum
{
    PORT_INT1LVL_OFF_gc = (0x00<<2),  /* Interrupt Disabled */
    PORT_INT1LVL_LO_gc = (0x01<<2),  /* Low Level */
    PORT_INT1LVL_MED_gc = (0x02<<2),  /* Medium Level */
    PORT_INT1LVL_HI_gc = (0x03<<2),  /* High Level */
} PORT_INT1LVL_t;

/* Output/Pull Configuration */
typedef enum PORT_OPC_enum
{
    PORT_OPC_TOTEM_gc = (0x00<<3),  /* Totempole */
    PORT_OPC_BUSKEEPER_gc = (0x01<<3),  /* Totempole w/ Bus keeper on Input and Output */
    PORT_OPC_PULLDOWN_gc = (0x02<<3),  /* Totempole w/ Pull-down on Input */
    PORT_OPC_PULLUP_gc = (0x03<<3),  /* Totempole w/ Pull-up on Input */
    PORT_OPC_WIREDOR_gc = (0x04<<3),  /* Wired OR */
    PORT_OPC_WIREDAND_gc = (0x05<<3),  /* Wired AND */
    PORT_OPC_WIREDORPULL_gc = (0x06<<3),  /* Wired OR w/ Pull-down */
    PORT_OPC_WIREDANDPULL_gc = (0x07<<3),  /* Wired AND w/ Pull-up */
} PORT_OPC_t;

/* Input/Sense Configuration */
typedef enum PORT_ISC_enum
{
    PORT_ISC_BOTHEDGES_gc = (0x00<<0),  /* Sense Both Edges */
    PORT_ISC_RISING_gc = (0x01<<0),  /* Sense Rising Edge */
    PORT_ISC_FALLING_gc = (0x02<<0),  /* Sense Falling Edge */
    PORT_ISC_LEVEL_gc = (0x03<<0),  /* Sense Level (Transparent For Events) */
    PORT_ISC_INPUT_DISABLE_gc = (0x07<<0),  /* Disable Digital Input Buffer */
} PORT_ISC_t;


/*
--------------------------------------------------------------------------
TC - 16-bit Timer/Counter With PWM
--------------------------------------------------------------------------
*/

/* 16-bit Timer/Counter 0 */
typedef struct TC0_struct
{
    register8_t CTRLA;  /* Control  Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t CTRLC;  /* Control register C */
    register8_t CTRLD;  /* Control Register D */
    register8_t CTRLE;  /* Control Register E */
    register8_t reserved_0x05;
    register8_t INTCTRLA;  /* Interrupt Control Register A */
    register8_t INTCTRLB;  /* Interrupt Control Register B */
    register8_t CTRLFCLR;  /* Control Register F Clear */
    register8_t CTRLFSET;  /* Control Register F Set */
    register8_t CTRLGCLR;  /* Control Register G Clear */
    register8_t CTRLGSET;  /* Control Register G Set */
    register8_t INTFLAGS;  /* Interrupt Flag Register */
    register8_t reserved_0x0D;
    register8_t reserved_0x0E;
    register8_t TEMP;  /* Temporary Register For 16-bit Access */
    register8_t reserved_0x10;
    register8_t reserved_0x11;
    register8_t reserved_0x12;
    register8_t reserved_0x13;
    register8_t reserved_0x14;
    register8_t reserved_0x15;
    register8_t reserved_0x16;
    register8_t reserved_0x17;
    register8_t reserved_0x18;
    register8_t reserved_0x19;
    register8_t reserved_0x1A;
    register8_t reserved_0x1B;
    register8_t reserved_0x1C;
    register8_t reserved_0x1D;
    register8_t reserved_0x1E;
    register8_t reserved_0x1F;
    _WORDREGISTER(CNT);  /* Count */
    register8_t reserved_0x22;
    register8_t reserved_0x23;
    register8_t reserved_0x24;
    register8_t reserved_0x25;
    _WORDREGISTER(PER);  /* Period */
    _WORDREGISTER(CCA);  /* Compare or Capture A */
    _WORDREGISTER(CCB);  /* Compare or Capture B */
    _WORDREGISTER(CCC);  /* Compare or Capture C */
    _WORDREGISTER(CCD);  /* Compare or Capture D */
    register8_t reserved_0x30;
    register8_t reserved_0x31;
    register8_t reserved_0x32;
    register8_t reserved_0x33;
    register8_t reserved_0x34;
    register8_t reserved_0x35;
    _WORDREGISTER(PERBUF);  /* Period Buffer */
    _WORDREGISTER(CCABUF);  /* Compare Or Capture A Buffer */
    _WORDREGISTER(CCBBUF);  /* Compare Or Capture B Buffer */
    _WORDREGISTER(CCCBUF);  /* Compare Or Capture C Buffer */
    _WORDREGISTER(CCDBUF);  /* Compare Or Capture D Buffer */
} TC0_t;

/*
--------------------------------------------------------------------------
TC - 16-bit Timer/Counter With PWM
--------------------------------------------------------------------------
*/

/* 16-bit Timer/Counter 1 */
typedef struct TC1_struct
{
    register8_t CTRLA;  /* Control  Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t CTRLC;  /* Control register C */
    register8_t CTRLD;  /* Control Register D */
    register8_t CTRLE;  /* Control Register E */
    register8_t reserved_0x05;
    register8_t INTCTRLA;  /* Interrupt Control Register A */
    register8_t INTCTRLB;  /* Interrupt Control Register B */
    register8_t CTRLFCLR;  /* Control Register F Clear */
    register8_t CTRLFSET;  /* Control Register F Set */
    register8_t CTRLGCLR;  /* Control Register G Clear */
    register8_t CTRLGSET;  /* Control Register G Set */
    register8_t INTFLAGS;  /* Interrupt Flag Register */
    register8_t reserved_0x0D;
    register8_t reserved_0x0E;
    register8_t TEMP;  /* Temporary Register For 16-bit Access */
    register8_t reserved_0x10;
    register8_t reserved_0x11;
    register8_t reserved_0x12;
    register8_t reserved_0x13;
    register8_t reserved_0x14;
    register8_t reserved_0x15;
    register8_t reserved_0x16;
    register8_t reserved_0x17;
    register8_t reserved_0x18;
    register8_t reserved_0x19;
    register8_t reserved_0x1A;
    register8_t reserved_0x1B;
    register8_t reserved_0x1C;
    register8_t reserved_0x1D;
    register8_t reserved_0x1E;
    register8_t reserved_0x1F;
    _WORDREGISTER(CNT);  /* Count */
    register8_t reserved_0x22;
    register8_t reserved_0x23;
    register8_t reserved_0x24;
    register8_t reserved_0x25;
    _WORDREGISTER(PER);  /* Period */
    _WORDREGISTER(CCA);  /* Compare or Capture A */
    _WORDREGISTER(CCB);  /* Compare or Capture B */
    register8_t reserved_0x2C;
    register8_t reserved_0x2D;
    register8_t reserved_0x2E;
    register8_t reserved_0x2F;
    register8_t reserved_0x30;
    register8_t reserved_0x31;
    register8_t reserved_0x32;
    register8_t reserved_0x33;
    register8_t reserved_0x34;
    register8_t reserved_0x35;
    _WORDREGISTER(PERBUF);  /* Period Buffer */
    _WORDREGISTER(CCABUF);  /* Compare Or Capture A Buffer */
    _WORDREGISTER(CCBBUF);  /* Compare Or Capture B Buffer */
} TC1_t;

/*
--------------------------------------------------------------------------
TC - 16-bit Timer/Counter With PWM
--------------------------------------------------------------------------
*/

/* Advanced Waveform Extension */
typedef struct AWEX_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t reserved_0x01;
    register8_t FDEVMASK;  /* Fault Detection Event Mask */
    register8_t FDCTRL;  /* Fault Detection Control Register */
    register8_t STATUS;  /* Status Register */
    register8_t reserved_0x05;
    register8_t DTBOTH;  /* Dead Time Both Sides */
    register8_t DTBOTHBUF;  /* Dead Time Both Sides Buffer */
    register8_t DTLS;  /* Dead Time Low Side */
    register8_t DTHS;  /* Dead Time High Side */
    register8_t DTLSBUF;  /* Dead Time Low Side Buffer */
    register8_t DTHSBUF;  /* Dead Time High Side Buffer */
    register8_t OUTOVEN;  /* Output Override Enable */
} AWEX_t;

/*
--------------------------------------------------------------------------
TC - 16-bit Timer/Counter With PWM
--------------------------------------------------------------------------
*/

/* High-Resolution Extension */
typedef struct HIRES_struct
{
    register8_t CTRL;  /* Control Register */
} HIRES_t;

/* Clock Selection */
typedef enum TC_CLKSEL_enum
{
    TC_CLKSEL_OFF_gc = (0x00<<0),  /* Timer Off */
    TC_CLKSEL_DIV1_gc = (0x01<<0),  /* System Clock */
    TC_CLKSEL_DIV2_gc = (0x02<<0),  /* System Clock / 2 */
    TC_CLKSEL_DIV4_gc = (0x03<<0),  /* System Clock / 4 */
    TC_CLKSEL_DIV8_gc = (0x04<<0),  /* System Clock / 8 */
    TC_CLKSEL_DIV64_gc = (0x05<<0),  /* System Clock / 64 */
    TC_CLKSEL_DIV256_gc = (0x06<<0),  /* System Clock / 256 */
    TC_CLKSEL_DIV1024_gc = (0x07<<0),  /* System Clock / 1024 */
    TC_CLKSEL_EVCH0_gc = (0x08<<0),  /* Event Channel 0 */
    TC_CLKSEL_EVCH1_gc = (0x09<<0),  /* Event Channel 1 */
    TC_CLKSEL_EVCH2_gc = (0x0A<<0),  /* Event Channel 2 */
    TC_CLKSEL_EVCH3_gc = (0x0B<<0),  /* Event Channel 3 */
    TC_CLKSEL_EVCH4_gc = (0x0C<<0),  /* Event Channel 4 */
    TC_CLKSEL_EVCH5_gc = (0x0D<<0),  /* Event Channel 5 */
    TC_CLKSEL_EVCH6_gc = (0x0E<<0),  /* Event Channel 6 */
    TC_CLKSEL_EVCH7_gc = (0x0F<<0),  /* Event Channel 7 */
} TC_CLKSEL_t;

/* Waveform Generation Mode */
typedef enum TC_WGMODE_enum
{
    TC_WGMODE_NORMAL_gc = (0x00<<0),  /* Normal Mode */
    TC_WGMODE_FRQ_gc = (0x01<<0),  /* Frequency Generation Mode */
    TC_WGMODE_SS_gc = (0x03<<0),  /* Single Slope */
    TC_WGMODE_DS_T_gc = (0x05<<0),  /* Dual Slope, Update on TOP */
    TC_WGMODE_DS_TB_gc = (0x06<<0),  /* Dual Slope, Update on TOP and BOTTOM */
    TC_WGMODE_DS_B_gc = (0x07<<0),  /* Dual Slope, Update on BOTTOM */
} TC_WGMODE_t;

/* Event Action */
typedef enum TC_EVACT_enum
{
    TC_EVACT_OFF_gc = (0x00<<5),  /* No Event Action */
    TC_EVACT_CAPT_gc = (0x01<<5),  /* Input Capture */
    TC_EVACT_UPDOWN_gc = (0x02<<5),  /* Externally Controlled Up/Down Count */
    TC_EVACT_QDEC_gc = (0x03<<5),  /* Quadrature Decode */
    TC_EVACT_RESTART_gc = (0x04<<5),  /* Restart */
    TC_EVACT_FRW_gc = (0x05<<5),  /* Frequency Capture */
    TC_EVACT_PW_gc = (0x06<<5),  /* Pulse-width Capture */
} TC_EVACT_t;

/* Event Selection */
typedef enum TC_EVSEL_enum
{
    TC_EVSEL_OFF_gc = (0x00<<0),  /* No Event Source */
    TC_EVSEL_CH0_gc = (0x08<<0),  /* Event Channel 0 */
    TC_EVSEL_CH1_gc = (0x09<<0),  /* Event Channel 1 */
    TC_EVSEL_CH2_gc = (0x0A<<0),  /* Event Channel 2 */
    TC_EVSEL_CH3_gc = (0x0B<<0),  /* Event Channel 3 */
    TC_EVSEL_CH4_gc = (0x0C<<0),  /* Event Channel 4 */
    TC_EVSEL_CH5_gc = (0x0D<<0),  /* Event Channel 5 */
    TC_EVSEL_CH6_gc = (0x0E<<0),  /* Event Channel 6 */
    TC_EVSEL_CH7_gc = (0x0F<<0),  /* Event Channel 7 */
} TC_EVSEL_t;

/* Error Interrupt Level */
typedef enum TC_ERRINTLVL_enum
{
    TC_ERRINTLVL_OFF_gc = (0x00<<2),  /* Interrupt Disabled */
    TC_ERRINTLVL_LO_gc = (0x01<<2),  /* Low Level */
    TC_ERRINTLVL_MED_gc = (0x02<<2),  /* Medium Level */
    TC_ERRINTLVL_HI_gc = (0x03<<2),  /* High Level */
} TC_ERRINTLVL_t;

/* Overflow Interrupt Level */
typedef enum TC_OVFINTLVL_enum
{
    TC_OVFINTLVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    TC_OVFINTLVL_LO_gc = (0x01<<0),  /* Low Level */
    TC_OVFINTLVL_MED_gc = (0x02<<0),  /* Medium Level */
    TC_OVFINTLVL_HI_gc = (0x03<<0),  /* High Level */
} TC_OVFINTLVL_t;

/* Compare or Capture D Interrupt Level */
typedef enum TC_CCDINTLVL_enum
{
    TC_CCDINTLVL_OFF_gc = (0x00<<6),  /* Interrupt Disabled */
    TC_CCDINTLVL_LO_gc = (0x01<<6),  /* Low Level */
    TC_CCDINTLVL_MED_gc = (0x02<<6),  /* Medium Level */
    TC_CCDINTLVL_HI_gc = (0x03<<6),  /* High Level */
} TC_CCDINTLVL_t;

/* Compare or Capture C Interrupt Level */
typedef enum TC_CCCINTLVL_enum
{
    TC_CCCINTLVL_OFF_gc = (0x00<<4),  /* Interrupt Disabled */
    TC_CCCINTLVL_LO_gc = (0x01<<4),  /* Low Level */
    TC_CCCINTLVL_MED_gc = (0x02<<4),  /* Medium Level */
    TC_CCCINTLVL_HI_gc = (0x03<<4),  /* High Level */
} TC_CCCINTLVL_t;

/* Compare or Capture B Interrupt Level */
typedef enum TC_CCBINTLVL_enum
{
    TC_CCBINTLVL_OFF_gc = (0x00<<2),  /* Interrupt Disabled */
    TC_CCBINTLVL_LO_gc = (0x01<<2),  /* Low Level */
    TC_CCBINTLVL_MED_gc = (0x02<<2),  /* Medium Level */
    TC_CCBINTLVL_HI_gc = (0x03<<2),  /* High Level */
} TC_CCBINTLVL_t;

/* Compare or Capture A Interrupt Level */
typedef enum TC_CCAINTLVL_enum
{
    TC_CCAINTLVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    TC_CCAINTLVL_LO_gc = (0x01<<0),  /* Low Level */
    TC_CCAINTLVL_MED_gc = (0x02<<0),  /* Medium Level */
    TC_CCAINTLVL_HI_gc = (0x03<<0),  /* High Level */
} TC_CCAINTLVL_t;

/* Timer/Counter Command */
typedef enum TC_CMD_enum
{
    TC_CMD_NONE_gc = (0x00<<2),  /* No Command */
    TC_CMD_UPDATE_gc = (0x01<<2),  /* Force Update */
    TC_CMD_RESTART_gc = (0x02<<2),  /* Force Restart */
    TC_CMD_RESET_gc = (0x03<<2),  /* Force Hard Reset */
} TC_CMD_t;

/* Fault Detect Action */
typedef enum AWEX_FDACT_enum
{
    AWEX_FDACT_NONE_gc = (0x00<<0),  /* No Fault Protection */
    AWEX_FDACT_CLEAROE_gc = (0x01<<0),  /* Clear Output Enable Bits */
    AWEX_FDACT_CLEARDIR_gc = (0x03<<0),  /* Clear I/O Port Direction Bits */
} AWEX_FDACT_t;

/* High Resolution Enable */
typedef enum HIRES_HREN_enum
{
    HIRES_HREN_NONE_gc = (0x00<<0),  /* No Fault Protection */
    HIRES_HREN_TC0_gc = (0x01<<0),  /* Enable High Resolution on Timer/Counter 0 */
    HIRES_HREN_TC1_gc = (0x02<<0),  /* Enable High Resolution on Timer/Counter 1 */
    HIRES_HREN_BOTH_gc = (0x03<<0),  /* Enable High Resolution both Timer/Counters */
} HIRES_HREN_t;


/*
--------------------------------------------------------------------------
USART - Universal Asynchronous Receiver-Transmitter
--------------------------------------------------------------------------
*/

/* Universal Synchronous/Asynchronous Receiver/Transmitter */
typedef struct USART_struct
{
    register8_t DATA;  /* Data Register */
    register8_t STATUS;  /* Status Register */
    register8_t reserved_0x02;
    register8_t CTRLA;  /* Control Register A */
    register8_t CTRLB;  /* Control Register B */
    register8_t CTRLC;  /* Control Register C */
    register8_t BAUDCTRLA;  /* Baud Rate Control Register A */
    register8_t BAUDCTRLB;  /* Baud Rate Control Register B */
} USART_t;

/* Receive Complete Interrupt level */
typedef enum USART_RXCINTLVL_enum
{
    USART_RXCINTLVL_OFF_gc = (0x00<<4),  /* Interrupt Disabled */
    USART_RXCINTLVL_LO_gc = (0x01<<4),  /* Low Level */
    USART_RXCINTLVL_MED_gc = (0x02<<4),  /* Medium Level */
    USART_RXCINTLVL_HI_gc = (0x03<<4),  /* High Level */
} USART_RXCINTLVL_t;

/* Transmit Complete Interrupt level */
typedef enum USART_TXCINTLVL_enum
{
    USART_TXCINTLVL_OFF_gc = (0x00<<2),  /* Interrupt Disabled */
    USART_TXCINTLVL_LO_gc = (0x01<<2),  /* Low Level */
    USART_TXCINTLVL_MED_gc = (0x02<<2),  /* Medium Level */
    USART_TXCINTLVL_HI_gc = (0x03<<2),  /* High Level */
} USART_TXCINTLVL_t;

/* Data Register Empty Interrupt level */
typedef enum USART_DREINTLVL_enum
{
    USART_DREINTLVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    USART_DREINTLVL_LO_gc = (0x01<<0),  /* Low Level */
    USART_DREINTLVL_MED_gc = (0x02<<0),  /* Medium Level */
    USART_DREINTLVL_HI_gc = (0x03<<0),  /* High Level */
} USART_DREINTLVL_t;

/* Character Size */
typedef enum USART_CHSIZE_enum
{
    USART_CHSIZE_5BIT_gc = (0x00<<0),  /* Character size: 5 bit */
    USART_CHSIZE_6BIT_gc = (0x01<<0),  /* Character size: 6 bit */
    USART_CHSIZE_7BIT_gc = (0x02<<0),  /* Character size: 7 bit */
    USART_CHSIZE_8BIT_gc = (0x03<<0),  /* Character size: 8 bit */
    USART_CHSIZE_9BIT_gc = (0x07<<0),  /* Character size: 9 bit */
} USART_CHSIZE_t;

/* Communication Mode */
typedef enum USART_CMODE_enum
{
    USART_CMODE_ASYNCHRONOUS_gc = (0x00<<6),  /* Asynchronous Mode */
    USART_CMODE_SYNCHRONOUS_gc = (0x01<<6),  /* Synchronous Mode */
    USART_CMODE_IRDA_gc = (0x02<<6),  /* IrDA Mode */
    USART_CMODE_MSPI_gc = (0x03<<6),  /* Master SPI Mode */
} USART_CMODE_t;

/* Parity Mode */
typedef enum USART_PMODE_enum
{
    USART_PMODE_DISABLED_gc = (0x00<<4),  /* No Parity */
    USART_PMODE_EVEN_gc = (0x02<<4),  /* Even Parity */
    USART_PMODE_ODD_gc = (0x03<<4),  /* Odd Parity */
} USART_PMODE_t;


/*
--------------------------------------------------------------------------
SPI - Serial Peripheral Interface
--------------------------------------------------------------------------
*/

/* Serial Peripheral Interface */
typedef struct SPI_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t INTCTRL;  /* Interrupt Control Register */
    register8_t STATUS;  /* Status Register */
    register8_t DATA;  /* Data Register */
} SPI_t;

/* SPI Mode */
typedef enum SPI_MODE_enum
{
    SPI_MODE_0_gc = (0x00<<2),  /* SPI Mode 0 */
    SPI_MODE_1_gc = (0x01<<2),  /* SPI Mode 1 */
    SPI_MODE_2_gc = (0x02<<2),  /* SPI Mode 2 */
    SPI_MODE_3_gc = (0x03<<2),  /* SPI Mode 3 */
} SPI_MODE_t;

/* Prescaler setting */
typedef enum SPI_PRESCALER_enum
{
    SPI_PRESCALER_DIV4_gc = (0x00<<0),  /* System Clock / 4 */
    SPI_PRESCALER_DIV16_gc = (0x01<<0),  /* System Clock / 16 */
    SPI_PRESCALER_DIV64_gc = (0x02<<0),  /* System Clock / 64 */
    SPI_PRESCALER_DIV128_gc = (0x03<<0),  /* System Clock / 128 */
} SPI_PRESCALER_t;

/* Interrupt level */
typedef enum SPI_INTLVL_enum
{
    SPI_INTLVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    SPI_INTLVL_LO_gc = (0x01<<0),  /* Low Level */
    SPI_INTLVL_MED_gc = (0x02<<0),  /* Medium Level */
    SPI_INTLVL_HI_gc = (0x03<<0),  /* High Level */
} SPI_INTLVL_t;


/*
--------------------------------------------------------------------------
IRCOM - IR Communication Module
--------------------------------------------------------------------------
*/

/* IR Communication Module */
typedef struct IRCOM_struct
{
    register8_t CTRL;  /* Control Register */
    register8_t TXPLCTRL;  /* IrDA Transmitter Pulse Length Control Register */
    register8_t RXPLCTRL;  /* IrDA Receiver Pulse Length Control Register */
} IRCOM_t;

/* Event channel selection */
typedef enum IRDA_EVSEL_enum
{
    IRDA_EVSEL_OFF_gc = (0x00<<0),  /* No Event Source */
    IRDA_EVSEL_0_gc = (0x08<<0),  /* Event Channel 0 */
    IRDA_EVSEL_1_gc = (0x09<<0),  /* Event Channel 1 */
    IRDA_EVSEL_2_gc = (0x0A<<0),  /* Event Channel 2 */
    IRDA_EVSEL_3_gc = (0x0B<<0),  /* Event Channel 3 */
    IRDA_EVSEL_4_gc = (0x0C<<0),  /* Event Channel 4 */
    IRDA_EVSEL_5_gc = (0x0D<<0),  /* Event Channel 5 */
    IRDA_EVSEL_6_gc = (0x0E<<0),  /* Event Channel 6 */
    IRDA_EVSEL_7_gc = (0x0F<<0),  /* Event Channel 7 */
} IRDA_EVSEL_t;


/*
--------------------------------------------------------------------------
AES - AES Module
--------------------------------------------------------------------------
*/

/* AES Module */
typedef struct AES_struct
{
    register8_t CTRL;  /* AES Control Register */
    register8_t STATUS;  /* AES Status Register */
    register8_t STATE;  /* AES State Register */
    register8_t KEY;  /* AES Key Register */
    register8_t INTCTRL;  /* AES Interrupt Control Register */
} AES_t;

/* Interrupt level */
typedef enum AES_INTLVL_enum
{
    AES_INTLVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    AES_INTLVL_LO_gc = (0x01<<0),  /* Low Level */
    AES_INTLVL_MED_gc = (0x02<<0),  /* Medium Level */
    AES_INTLVL_HI_gc = (0x03<<0),  /* High Level */
} AES_INTLVL_t;



/*
==========================================================================
IO Module Instances. Mapped to memory.
==========================================================================
*/

#define GPIO    (*(GPIO_t *) 0x0000)  /* General Purpose IO Registers */
#define VPORT0    (*(VPORT_t *) 0x0010)  /* Virtual Port 0 */
#define VPORT1    (*(VPORT_t *) 0x0014)  /* Virtual Port 1 */
#define VPORT2    (*(VPORT_t *) 0x0018)  /* Virtual Port 2 */
#define VPORT3    (*(VPORT_t *) 0x001C)  /* Virtual Port 3 */
#define OCD    (*(OCD_t *) 0x002E)  /* On-Chip Debug System */
#define CPU    (*(CPU_t *) 0x0030)  /* CPU Registers */
#define CLK    (*(CLK_t *) 0x0040)  /* Clock System */
#define SLEEP    (*(SLEEP_t *) 0x0048)  /* Sleep Controller */
#define OSC    (*(OSC_t *) 0x0050)  /* Oscillator Control */
#define DFLLRC32M    (*(DFLL_t *) 0x0060)  /* DFLL for 32MHz RC Oscillator */
#define DFLLRC2M    (*(DFLL_t *) 0x0068)  /* DFLL for 2MHz RC Oscillator */
#define PR    (*(PR_t *) 0x0070)  /* Power Reduction */
#define RST    (*(RST_t *) 0x0078)  /* Reset Controller */
#define WDT    (*(WDT_t *) 0x0080)  /* Watch-Dog Timer */
#define MCU    (*(MCU_t *) 0x0090)  /* MCU Control */
#define PMIC    (*(PMIC_t *) 0x00A0)  /* Programmable Interrupt Controller */
#define PORTCFG    (*(PORTCFG_t *) 0x00B0)  /* Port Configuration */
#define AES    (*(AES_t *) 0x00C0)  /* AES Crypto Module */
#define DMA    (*(DMA_t *) 0x0100)  /* DMA Controller */
#define EVSYS    (*(EVSYS_t *) 0x0180)  /* Event System */
#define NVM    (*(NVM_t *) 0x01C0)  /* Non Volatile Memory Controller */
#define ADCA    (*(ADC_t *) 0x0200)  /* Analog to Digital Converter A */
#define ADCB    (*(ADC_t *) 0x0240)  /* Analog to Digital Converter B */
#define DACB    (*(DAC_t *) 0x0320)  /* Digital to Analog Converter B */
#define ACA    (*(AC_t *) 0x0380)  /* Analog Comparator A */
#define ACB    (*(AC_t *) 0x0390)  /* Analog Comparator B */
#define RTC    (*(RTC_t *) 0x0400)  /* Real-Time Counter */
#define TWIC    (*(TWI_t *) 0x0480)  /* Two-Wire Interface C */
#define TWIE    (*(TWI_t *) 0x04A0)  /* Two-Wire Interface E */
#define PORTA    (*(PORT_t *) 0x0600)  /* Port A */
#define PORTB    (*(PORT_t *) 0x0620)  /* Port B */
#define PORTC    (*(PORT_t *) 0x0640)  /* Port C */
#define PORTD    (*(PORT_t *) 0x0660)  /* Port D */
#define PORTE    (*(PORT_t *) 0x0680)  /* Port E */
#define PORTF    (*(PORT_t *) 0x06A0)  /* Port F */
#define PORTR    (*(PORT_t *) 0x07E0)  /* Port R */
#define TCC0    (*(TC0_t *) 0x0800)  /* Timer/Counter C0 */
#define TCC1    (*(TC1_t *) 0x0840)  /* Timer/Counter C1 */
#define AWEXC    (*(AWEX_t *) 0x0880)  /* Advanced Waveform Extension C */
#define HIRESC    (*(HIRES_t *) 0x0890)  /* High-Resolution Extension C */
#define USARTC0    (*(USART_t *) 0x08A0)  /* Universal Asynchronous Receiver-Transmitter C0 */
#define USARTC1    (*(USART_t *) 0x08B0)  /* Universal Asynchronous Receiver-Transmitter C1 */
#define SPIC    (*(SPI_t *) 0x08C0)  /* Serial Peripheral Interface C */
#define IRCOM    (*(IRCOM_t *) 0x08F8)  /* IR Communication Module */
#define TCD0    (*(TC0_t *) 0x0900)  /* Timer/Counter D0 */
#define TCD1    (*(TC1_t *) 0x0940)  /* Timer/Counter D1 */
#define HIRESD    (*(HIRES_t *) 0x0990)  /* High-Resolution Extension D */
#define USARTD0    (*(USART_t *) 0x09A0)  /* Universal Asynchronous Receiver-Transmitter D0 */
#define USARTD1    (*(USART_t *) 0x09B0)  /* Universal Asynchronous Receiver-Transmitter D1 */
#define SPID    (*(SPI_t *) 0x09C0)  /* Serial Peripheral Interface D */
#define TCE0    (*(TC0_t *) 0x0A00)  /* Timer/Counter E0 */
#define TCE1    (*(TC1_t *) 0x0A40)  /* Timer/Counter E1 */
#define AWEXE    (*(AWEX_t *) 0x0A80)  /* Advanced Waveform Extension E */
#define HIRESE    (*(HIRES_t *) 0x0A90)  /* High-Resolution Extension E */
#define USARTE0    (*(USART_t *) 0x0AA0)  /* Universal Asynchronous Receiver-Transmitter E0 */
#define USARTE1    (*(USART_t *) 0x0AB0)  /* Universal Asynchronous Receiver-Transmitter E1 */
#define SPIE    (*(SPI_t *) 0x0AC0)  /* Serial Peripheral Interface E */
#define TCF0    (*(TC0_t *) 0x0B00)  /* Timer/Counter F0 */
#define HIRESF    (*(HIRES_t *) 0x0B90)  /* High-Resolution Extension F */
#define USARTF0    (*(USART_t *) 0x0BA0)  /* Universal Asynchronous Receiver-Transmitter F0 */
#define USARTF1    (*(USART_t *) 0x0BB0)  /* Universal Asynchronous Receiver-Transmitter F1 */
#define SPIF    (*(SPI_t *) 0x0BC0)  /* Serial Peripheral Interface F */


#endif /* !defined (__ASSEMBLER__) */


/* ========== Flattened fully qualified IO register names ========== */

/* GPIO - General Purpose IO Registers */
#define GPIO_GPIO0  _SFR_MEM8(0x0000)
#define GPIO_GPIO1  _SFR_MEM8(0x0001)
#define GPIO_GPIO2  _SFR_MEM8(0x0002)
#define GPIO_GPIO3  _SFR_MEM8(0x0003)
#define GPIO_GPIO4  _SFR_MEM8(0x0004)
#define GPIO_GPIO5  _SFR_MEM8(0x0005)
#define GPIO_GPIO6  _SFR_MEM8(0x0006)
#define GPIO_GPIO7  _SFR_MEM8(0x0007)
#define GPIO_GPIO8  _SFR_MEM8(0x0008)
#define GPIO_GPIO9  _SFR_MEM8(0x0009)
#define GPIO_GPIOA  _SFR_MEM8(0x000A)
#define GPIO_GPIOB  _SFR_MEM8(0x000B)
#define GPIO_GPIOC  _SFR_MEM8(0x000C)
#define GPIO_GPIOD  _SFR_MEM8(0x000D)
#define GPIO_GPIOE  _SFR_MEM8(0x000E)
#define GPIO_GPIOF  _SFR_MEM8(0x000F)

/* VPORT0 - Virtual Port 0 */
#define VPORT0_DIR  _SFR_MEM8(0x0010)
#define VPORT0_OUT  _SFR_MEM8(0x0011)
#define VPORT0_IN  _SFR_MEM8(0x0012)
#define VPORT0_INTFLAGS  _SFR_MEM8(0x0013)

/* VPORT1 - Virtual Port 1 */
#define VPORT1_DIR  _SFR_MEM8(0x0014)
#define VPORT1_OUT  _SFR_MEM8(0x0015)
#define VPORT1_IN  _SFR_MEM8(0x0016)
#define VPORT1_INTFLAGS  _SFR_MEM8(0x0017)

/* VPORT2 - Virtual Port 2 */
#define VPORT2_DIR  _SFR_MEM8(0x0018)
#define VPORT2_OUT  _SFR_MEM8(0x0019)
#define VPORT2_IN  _SFR_MEM8(0x001A)
#define VPORT2_INTFLAGS  _SFR_MEM8(0x001B)

/* VPORT3 - Virtual Port 3 */
#define VPORT3_DIR  _SFR_MEM8(0x001C)
#define VPORT3_OUT  _SFR_MEM8(0x001D)
#define VPORT3_IN  _SFR_MEM8(0x001E)
#define VPORT3_INTFLAGS  _SFR_MEM8(0x001F)

/* OCD - On-Chip Debug System */
#define OCD_OCDR0  _SFR_MEM8(0x002E)
#define OCD_OCDR1  _SFR_MEM8(0x002F)

/* CPU - CPU Registers */
#define CPU_CCP  _SFR_MEM8(0x0034)
#define CPU_RAMPD  _SFR_MEM8(0x0038)
#define CPU_RAMPX  _SFR_MEM8(0x0039)
#define CPU_RAMPY  _SFR_MEM8(0x003A)
#define CPU_RAMPZ  _SFR_MEM8(0x003B)
#define CPU_EIND  _SFR_MEM8(0x003C)
#define CPU_SPL  _SFR_MEM8(0x003D)
#define CPU_SPH  _SFR_MEM8(0x003E)
#define CPU_SREG  _SFR_MEM8(0x003F)

/* CLK - Clock System */
#define CLK_CTRL  _SFR_MEM8(0x0040)
#define CLK_PSCTRL  _SFR_MEM8(0x0041)
#define CLK_LOCK  _SFR_MEM8(0x0042)
#define CLK_RTCCTRL  _SFR_MEM8(0x0043)

/* SLEEP - Sleep Controller */
#define SLEEP_CTRL  _SFR_MEM8(0x0048)

/* OSC - Oscillator Control */
#define OSC_CTRL  _SFR_MEM8(0x0050)
#define OSC_STATUS  _SFR_MEM8(0x0051)
#define OSC_XOSCCTRL  _SFR_MEM8(0x0052)
#define OSC_XOSCFAIL  _SFR_MEM8(0x0053)
#define OSC_RC32KCAL  _SFR_MEM8(0x0054)
#define OSC_PLLCTRL  _SFR_MEM8(0x0055)
#define OSC_DFLLCTRL  _SFR_MEM8(0x0056)

/* DFLLRC32M - DFLL for 32MHz RC Oscillator */
#define DFLLRC32M_CTRL  _SFR_MEM8(0x0060)
#define DFLLRC32M_CALA  _SFR_MEM8(0x0062)
#define DFLLRC32M_CALB  _SFR_MEM8(0x0063)
#define DFLLRC32M_OSCCNT0  _SFR_MEM8(0x0064)
#define DFLLRC32M_OSCCNT1  _SFR_MEM8(0x0065)
#define DFLLRC32M_OSCCNT2  _SFR_MEM8(0x0066)

/* DFLLRC2M - DFLL for 2MHz RC Oscillator */
#define DFLLRC2M_CTRL  _SFR_MEM8(0x0068)
#define DFLLRC2M_CALA  _SFR_MEM8(0x006A)
#define DFLLRC2M_CALB  _SFR_MEM8(0x006B)
#define DFLLRC2M_OSCCNT0  _SFR_MEM8(0x006C)
#define DFLLRC2M_OSCCNT1  _SFR_MEM8(0x006D)
#define DFLLRC2M_OSCCNT2  _SFR_MEM8(0x006E)

/* PR - Power Reduction */
#define PR_PR  _SFR_MEM8(0x0070)
#define PR_PRPA  _SFR_MEM8(0x0071)
#define PR_PRPB  _SFR_MEM8(0x0072)
#define PR_PRPC  _SFR_MEM8(0x0073)
#define PR_PRPD  _SFR_MEM8(0x0074)
#define PR_PRPE  _SFR_MEM8(0x0075)
#define PR_PRPF  _SFR_MEM8(0x0076)

/* RST - Reset Controller */
#define RST_STATUS  _SFR_MEM8(0x0078)
#define RST_CTRL  _SFR_MEM8(0x0079)

/* WDT - Watch-Dog Timer */
#define WDT_CTRL  _SFR_MEM8(0x0080)
#define WDT_WINCTRL  _SFR_MEM8(0x0081)
#define WDT_STATUS  _SFR_MEM8(0x0082)

/* MCU - MCU Control */
#define MCU_DEVID0  _SFR_MEM8(0x0090)
#define MCU_DEVID1  _SFR_MEM8(0x0091)
#define MCU_DEVID2  _SFR_MEM8(0x0092)
#define MCU_REVID  _SFR_MEM8(0x0093)
#define MCU_JTAGUID  _SFR_MEM8(0x0094)
#define MCU_MCUCR  _SFR_MEM8(0x0096)
#define MCU_EVSYSLOCK  _SFR_MEM8(0x0098)
#define MCU_AWEXLOCK  _SFR_MEM8(0x0099)

/* PMIC - Programmable Interrupt Controller */
#define PMIC_STATUS  _SFR_MEM8(0x00A0)
#define PMIC_INTPRI  _SFR_MEM8(0x00A1)
#define PMIC_CTRL  _SFR_MEM8(0x00A2)

/* PORTCFG - Port Configuration */
#define PORTCFG_MPCMASK  _SFR_MEM8(0x00B0)
#define PORTCFG_VPCTRLA  _SFR_MEM8(0x00B2)
#define PORTCFG_VPCTRLB  _SFR_MEM8(0x00B3)
#define PORTCFG_CLKEVOUT  _SFR_MEM8(0x00B4)

/* AES - AES Crypto Module */
#define AES_CTRL  _SFR_MEM8(0x00C0)
#define AES_STATUS  _SFR_MEM8(0x00C1)
#define AES_STATE  _SFR_MEM8(0x00C2)
#define AES_KEY  _SFR_MEM8(0x00C3)
#define AES_INTCTRL  _SFR_MEM8(0x00C4)

/* DMA - DMA Controller */
#define DMA_CTRL  _SFR_MEM8(0x0100)
#define DMA_INTFLAGS  _SFR_MEM8(0x0103)
#define DMA_STATUS  _SFR_MEM8(0x0104)
#define DMA_TEMP  _SFR_MEM16(0x0106)
#define DMA_CH0_CTRLA  _SFR_MEM8(0x0110)
#define DMA_CH0_CTRLB  _SFR_MEM8(0x0111)
#define DMA_CH0_ADDRCTRL  _SFR_MEM8(0x0112)
#define DMA_CH0_TRIGSRC  _SFR_MEM8(0x0113)
#define DMA_CH0_TRFCNT  _SFR_MEM16(0x0114)
#define DMA_CH0_REPCNT  _SFR_MEM8(0x0116)
#define DMA_CH0_SRCADDR0  _SFR_MEM8(0x0118)
#define DMA_CH0_SRCADDR1  _SFR_MEM8(0x0119)
#define DMA_CH0_SRCADDR2  _SFR_MEM8(0x011A)
#define DMA_CH0_DESTADDR0  _SFR_MEM8(0x011C)
#define DMA_CH0_DESTADDR1  _SFR_MEM8(0x011D)
#define DMA_CH0_DESTADDR2  _SFR_MEM8(0x011E)
#define DMA_CH1_CTRLA  _SFR_MEM8(0x0120)
#define DMA_CH1_CTRLB  _SFR_MEM8(0x0121)
#define DMA_CH1_ADDRCTRL  _SFR_MEM8(0x0122)
#define DMA_CH1_TRIGSRC  _SFR_MEM8(0x0123)
#define DMA_CH1_TRFCNT  _SFR_MEM16(0x0124)
#define DMA_CH1_REPCNT  _SFR_MEM8(0x0126)
#define DMA_CH1_SRCADDR0  _SFR_MEM8(0x0128)
#define DMA_CH1_SRCADDR1  _SFR_MEM8(0x0129)
#define DMA_CH1_SRCADDR2  _SFR_MEM8(0x012A)
#define DMA_CH1_DESTADDR0  _SFR_MEM8(0x012C)
#define DMA_CH1_DESTADDR1  _SFR_MEM8(0x012D)
#define DMA_CH1_DESTADDR2  _SFR_MEM8(0x012E)
#define DMA_CH2_CTRLA  _SFR_MEM8(0x0130)
#define DMA_CH2_CTRLB  _SFR_MEM8(0x0131)
#define DMA_CH2_ADDRCTRL  _SFR_MEM8(0x0132)
#define DMA_CH2_TRIGSRC  _SFR_MEM8(0x0133)
#define DMA_CH2_TRFCNT  _SFR_MEM16(0x0134)
#define DMA_CH2_REPCNT  _SFR_MEM8(0x0136)
#define DMA_CH2_SRCADDR0  _SFR_MEM8(0x0138)
#define DMA_CH2_SRCADDR1  _SFR_MEM8(0x0139)
#define DMA_CH2_SRCADDR2  _SFR_MEM8(0x013A)
#define DMA_CH2_DESTADDR0  _SFR_MEM8(0x013C)
#define DMA_CH2_DESTADDR1  _SFR_MEM8(0x013D)
#define DMA_CH2_DESTADDR2  _SFR_MEM8(0x013E)
#define DMA_CH3_CTRLA  _SFR_MEM8(0x0140)
#define DMA_CH3_CTRLB  _SFR_MEM8(0x0141)
#define DMA_CH3_ADDRCTRL  _SFR_MEM8(0x0142)
#define DMA_CH3_TRIGSRC  _SFR_MEM8(0x0143)
#define DMA_CH3_TRFCNT  _SFR_MEM16(0x0144)
#define DMA_CH3_REPCNT  _SFR_MEM8(0x0146)
#define DMA_CH3_SRCADDR0  _SFR_MEM8(0x0148)
#define DMA_CH3_SRCADDR1  _SFR_MEM8(0x0149)
#define DMA_CH3_SRCADDR2  _SFR_MEM8(0x014A)
#define DMA_CH3_DESTADDR0  _SFR_MEM8(0x014C)
#define DMA_CH3_DESTADDR1  _SFR_MEM8(0x014D)
#define DMA_CH3_DESTADDR2  _SFR_MEM8(0x014E)

/* EVSYS - Event System */
#define EVSYS_CH0MUX  _SFR_MEM8(0x0180)
#define EVSYS_CH1MUX  _SFR_MEM8(0x0181)
#define EVSYS_CH2MUX  _SFR_MEM8(0x0182)
#define EVSYS_CH3MUX  _SFR_MEM8(0x0183)
#define EVSYS_CH4MUX  _SFR_MEM8(0x0184)
#define EVSYS_CH5MUX  _SFR_MEM8(0x0185)
#define EVSYS_CH6MUX  _SFR_MEM8(0x0186)
#define EVSYS_CH7MUX  _SFR_MEM8(0x0187)
#define EVSYS_CH0CTRL  _SFR_MEM8(0x0188)
#define EVSYS_CH1CTRL  _SFR_MEM8(0x0189)
#define EVSYS_CH2CTRL  _SFR_MEM8(0x018A)
#define EVSYS_CH3CTRL  _SFR_MEM8(0x018B)
#define EVSYS_CH4CTRL  _SFR_MEM8(0x018C)
#define EVSYS_CH5CTRL  _SFR_MEM8(0x018D)
#define EVSYS_CH6CTRL  _SFR_MEM8(0x018E)
#define EVSYS_CH7CTRL  _SFR_MEM8(0x018F)
#define EVSYS_STROBE  _SFR_MEM8(0x0190)
#define EVSYS_DATA  _SFR_MEM8(0x0191)

/* NVM - Non Volatile Memory Controller */
#define NVM_ADDR0  _SFR_MEM8(0x01C0)
#define NVM_ADDR1  _SFR_MEM8(0x01C1)
#define NVM_ADDR2  _SFR_MEM8(0x01C2)
#define NVM_DATA0  _SFR_MEM8(0x01C4)
#define NVM_DATA1  _SFR_MEM8(0x01C5)
#define NVM_DATA2  _SFR_MEM8(0x01C6)
#define NVM_CMD  _SFR_MEM8(0x01CA)
#define NVM_CTRLA  _SFR_MEM8(0x01CB)
#define NVM_CTRLB  _SFR_MEM8(0x01CC)
#define NVM_INTCTRL  _SFR_MEM8(0x01CD)
#define NVM_STATUS  _SFR_MEM8(0x01CF)
#define NVM_LOCKBITS  _SFR_MEM8(0x01D0)

/* ADCA - Analog to Digital Converter A */
#define ADCA_CTRLA  _SFR_MEM8(0x0200)
#define ADCA_CTRLB  _SFR_MEM8(0x0201)
#define ADCA_REFCTRL  _SFR_MEM8(0x0202)
#define ADCA_EVCTRL  _SFR_MEM8(0x0203)
#define ADCA_PRESCALER  _SFR_MEM8(0x0204)
#define ADCA_CALCTRL  _SFR_MEM8(0x0205)
#define ADCA_INTFLAGS  _SFR_MEM8(0x0206)
#define ADCA_CALIB  _SFR_MEM8(0x020C)
#define ADCA_CH0RES  _SFR_MEM16(0x0210)
#define ADCA_CH1RES  _SFR_MEM16(0x0212)
#define ADCA_CH2RES  _SFR_MEM16(0x0214)
#define ADCA_CH3RES  _SFR_MEM16(0x0216)
#define ADCA_CMP  _SFR_MEM16(0x0218)
#define ADCA_CH0_CTRL  _SFR_MEM8(0x0220)
#define ADCA_CH0_MUXCTRL  _SFR_MEM8(0x0221)
#define ADCA_CH0_INTCTRL  _SFR_MEM8(0x0222)
#define ADCA_CH0_INTFLAGS  _SFR_MEM8(0x0223)
#define ADCA_CH0_RES  _SFR_MEM16(0x0224)
#define ADCA_CH1_CTRL  _SFR_MEM8(0x0228)
#define ADCA_CH1_MUXCTRL  _SFR_MEM8(0x0229)
#define ADCA_CH1_INTCTRL  _SFR_MEM8(0x022A)
#define ADCA_CH1_INTFLAGS  _SFR_MEM8(0x022B)
#define ADCA_CH1_RES  _SFR_MEM16(0x022C)
#define ADCA_CH2_CTRL  _SFR_MEM8(0x0230)
#define ADCA_CH2_MUXCTRL  _SFR_MEM8(0x0231)
#define ADCA_CH2_INTCTRL  _SFR_MEM8(0x0232)
#define ADCA_CH2_INTFLAGS  _SFR_MEM8(0x0233)
#define ADCA_CH2_RES  _SFR_MEM16(0x0234)
#define ADCA_CH3_CTRL  _SFR_MEM8(0x0238)
#define ADCA_CH3_MUXCTRL  _SFR_MEM8(0x0239)
#define ADCA_CH3_INTCTRL  _SFR_MEM8(0x023A)
#define ADCA_CH3_INTFLAGS  _SFR_MEM8(0x023B)
#define ADCA_CH3_RES  _SFR_MEM16(0x023C)

/* ADCB - Analog to Digital Converter B */
#define ADCB_CTRLA  _SFR_MEM8(0x0240)
#define ADCB_CTRLB  _SFR_MEM8(0x0241)
#define ADCB_REFCTRL  _SFR_MEM8(0x0242)
#define ADCB_EVCTRL  _SFR_MEM8(0x0243)
#define ADCB_PRESCALER  _SFR_MEM8(0x0244)
#define ADCB_CALCTRL  _SFR_MEM8(0x0245)
#define ADCB_INTFLAGS  _SFR_MEM8(0x0246)
#define ADCB_CALIB  _SFR_MEM8(0x024C)
#define ADCB_CH0RES  _SFR_MEM16(0x0250)
#define ADCB_CH1RES  _SFR_MEM16(0x0252)
#define ADCB_CH2RES  _SFR_MEM16(0x0254)
#define ADCB_CH3RES  _SFR_MEM16(0x0256)
#define ADCB_CMP  _SFR_MEM16(0x0258)
#define ADCB_CH0_CTRL  _SFR_MEM8(0x0260)
#define ADCB_CH0_MUXCTRL  _SFR_MEM8(0x0261)
#define ADCB_CH0_INTCTRL  _SFR_MEM8(0x0262)
#define ADCB_CH0_INTFLAGS  _SFR_MEM8(0x0263)
#define ADCB_CH0_RES  _SFR_MEM16(0x0264)
#define ADCB_CH1_CTRL  _SFR_MEM8(0x0268)
#define ADCB_CH1_MUXCTRL  _SFR_MEM8(0x0269)
#define ADCB_CH1_INTCTRL  _SFR_MEM8(0x026A)
#define ADCB_CH1_INTFLAGS  _SFR_MEM8(0x026B)
#define ADCB_CH1_RES  _SFR_MEM16(0x026C)
#define ADCB_CH2_CTRL  _SFR_MEM8(0x0270)
#define ADCB_CH2_MUXCTRL  _SFR_MEM8(0x0271)
#define ADCB_CH2_INTCTRL  _SFR_MEM8(0x0272)
#define ADCB_CH2_INTFLAGS  _SFR_MEM8(0x0273)
#define ADCB_CH2_RES  _SFR_MEM16(0x0274)
#define ADCB_CH3_CTRL  _SFR_MEM8(0x0278)
#define ADCB_CH3_MUXCTRL  _SFR_MEM8(0x0279)
#define ADCB_CH3_INTCTRL  _SFR_MEM8(0x027A)
#define ADCB_CH3_INTFLAGS  _SFR_MEM8(0x027B)
#define ADCB_CH3_RES  _SFR_MEM16(0x027C)

/* DACB - Digital to Analog Converter B */
#define DACB_CTRLA  _SFR_MEM8(0x0320)
#define DACB_CTRLB  _SFR_MEM8(0x0321)
#define DACB_CTRLC  _SFR_MEM8(0x0322)
#define DACB_EVCTRL  _SFR_MEM8(0x0323)
#define DACB_TIMCTRL  _SFR_MEM8(0x0324)
#define DACB_STATUS  _SFR_MEM8(0x0325)
#define DACB_GAINCAL  _SFR_MEM8(0x0328)
#define DACB_OFFSETCAL  _SFR_MEM8(0x0329)
#define DACB_CH0DATA  _SFR_MEM16(0x0338)
#define DACB_CH1DATA  _SFR_MEM16(0x033A)

/* ACA - Analog Comparator A */
#define ACA_AC0CTRL  _SFR_MEM8(0x0380)
#define ACA_AC1CTRL  _SFR_MEM8(0x0381)
#define ACA_AC0MUXCTRL  _SFR_MEM8(0x0382)
#define ACA_AC1MUXCTRL  _SFR_MEM8(0x0383)
#define ACA_CTRLA  _SFR_MEM8(0x0384)
#define ACA_CTRLB  _SFR_MEM8(0x0385)
#define ACA_WINCTRL  _SFR_MEM8(0x0386)
#define ACA_STATUS  _SFR_MEM8(0x0387)

/* ACB - Analog Comparator B */
#define ACB_AC0CTRL  _SFR_MEM8(0x0390)
#define ACB_AC1CTRL  _SFR_MEM8(0x0391)
#define ACB_AC0MUXCTRL  _SFR_MEM8(0x0392)
#define ACB_AC1MUXCTRL  _SFR_MEM8(0x0393)
#define ACB_CTRLA  _SFR_MEM8(0x0394)
#define ACB_CTRLB  _SFR_MEM8(0x0395)
#define ACB_WINCTRL  _SFR_MEM8(0x0396)
#define ACB_STATUS  _SFR_MEM8(0x0397)

/* RTC - Real-Time Counter */
#define RTC_CTRL  _SFR_MEM8(0x0400)
#define RTC_STATUS  _SFR_MEM8(0x0401)
#define RTC_INTCTRL  _SFR_MEM8(0x0402)
#define RTC_INTFLAGS  _SFR_MEM8(0x0403)
#define RTC_TEMP  _SFR_MEM8(0x0404)
#define RTC_CNT  _SFR_MEM16(0x0408)
#define RTC_PER  _SFR_MEM16(0x040A)
#define RTC_COMP  _SFR_MEM16(0x040C)

/* TWIC - Two-Wire Interface C */
#define TWIC_CTRL  _SFR_MEM8(0x0480)
#define TWIC_MASTER_CTRLA  _SFR_MEM8(0x0481)
#define TWIC_MASTER_CTRLB  _SFR_MEM8(0x0482)
#define TWIC_MASTER_CTRLC  _SFR_MEM8(0x0483)
#define TWIC_MASTER_STATUS  _SFR_MEM8(0x0484)
#define TWIC_MASTER_BAUD  _SFR_MEM8(0x0485)
#define TWIC_MASTER_ADDR  _SFR_MEM8(0x0486)
#define TWIC_MASTER_DATA  _SFR_MEM8(0x0487)
#define TWIC_SLAVE_CTRLA  _SFR_MEM8(0x0488)
#define TWIC_SLAVE_CTRLB  _SFR_MEM8(0x0489)
#define TWIC_SLAVE_STATUS  _SFR_MEM8(0x048A)
#define TWIC_SLAVE_ADDR  _SFR_MEM8(0x048B)
#define TWIC_SLAVE_DATA  _SFR_MEM8(0x048C)

/* TWIE - Two-Wire Interface E */
#define TWIE_CTRL  _SFR_MEM8(0x04A0)
#define TWIE_MASTER_CTRLA  _SFR_MEM8(0x04A1)
#define TWIE_MASTER_CTRLB  _SFR_MEM8(0x04A2)
#define TWIE_MASTER_CTRLC  _SFR_MEM8(0x04A3)
#define TWIE_MASTER_STATUS  _SFR_MEM8(0x04A4)
#define TWIE_MASTER_BAUD  _SFR_MEM8(0x04A5)
#define TWIE_MASTER_ADDR  _SFR_MEM8(0x04A6)
#define TWIE_MASTER_DATA  _SFR_MEM8(0x04A7)
#define TWIE_SLAVE_CTRLA  _SFR_MEM8(0x04A8)
#define TWIE_SLAVE_CTRLB  _SFR_MEM8(0x04A9)
#define TWIE_SLAVE_STATUS  _SFR_MEM8(0x04AA)
#define TWIE_SLAVE_ADDR  _SFR_MEM8(0x04AB)
#define TWIE_SLAVE_DATA  _SFR_MEM8(0x04AC)

/* PORTA - Port A */
#define PORTA_DIR  _SFR_MEM8(0x0600)
#define PORTA_DIRSET  _SFR_MEM8(0x0601)
#define PORTA_DIRCLR  _SFR_MEM8(0x0602)
#define PORTA_DIRTGL  _SFR_MEM8(0x0603)
#define PORTA_OUT  _SFR_MEM8(0x0604)
#define PORTA_OUTSET  _SFR_MEM8(0x0605)
#define PORTA_OUTCLR  _SFR_MEM8(0x0606)
#define PORTA_OUTTGL  _SFR_MEM8(0x0607)
#define PORTA_IN  _SFR_MEM8(0x0608)
#define PORTA_INTCTRL  _SFR_MEM8(0x0609)
#define PORTA_INT0MASK  _SFR_MEM8(0x060A)
#define PORTA_INT1MASK  _SFR_MEM8(0x060B)
#define PORTA_INTFLAGS  _SFR_MEM8(0x060C)
#define PORTA_PIN0CTRL  _SFR_MEM8(0x0610)
#define PORTA_PIN1CTRL  _SFR_MEM8(0x0611)
#define PORTA_PIN2CTRL  _SFR_MEM8(0x0612)
#define PORTA_PIN3CTRL  _SFR_MEM8(0x0613)
#define PORTA_PIN4CTRL  _SFR_MEM8(0x0614)
#define PORTA_PIN5CTRL  _SFR_MEM8(0x0615)
#define PORTA_PIN6CTRL  _SFR_MEM8(0x0616)
#define PORTA_PIN7CTRL  _SFR_MEM8(0x0617)

/* PORTB - Port B */
#define PORTB_DIR  _SFR_MEM8(0x0620)
#define PORTB_DIRSET  _SFR_MEM8(0x0621)
#define PORTB_DIRCLR  _SFR_MEM8(0x0622)
#define PORTB_DIRTGL  _SFR_MEM8(0x0623)
#define PORTB_OUT  _SFR_MEM8(0x0624)
#define PORTB_OUTSET  _SFR_MEM8(0x0625)
#define PORTB_OUTCLR  _SFR_MEM8(0x0626)
#define PORTB_OUTTGL  _SFR_MEM8(0x0627)
#define PORTB_IN  _SFR_MEM8(0x0628)
#define PORTB_INTCTRL  _SFR_MEM8(0x0629)
#define PORTB_INT0MASK  _SFR_MEM8(0x062A)
#define PORTB_INT1MASK  _SFR_MEM8(0x062B)
#define PORTB_INTFLAGS  _SFR_MEM8(0x062C)
#define PORTB_PIN0CTRL  _SFR_MEM8(0x0630)
#define PORTB_PIN1CTRL  _SFR_MEM8(0x0631)
#define PORTB_PIN2CTRL  _SFR_MEM8(0x0632)
#define PORTB_PIN3CTRL  _SFR_MEM8(0x0633)
#define PORTB_PIN4CTRL  _SFR_MEM8(0x0634)
#define PORTB_PIN5CTRL  _SFR_MEM8(0x0635)
#define PORTB_PIN6CTRL  _SFR_MEM8(0x0636)
#define PORTB_PIN7CTRL  _SFR_MEM8(0x0637)

/* PORTC - Port C */
#define PORTC_DIR  _SFR_MEM8(0x0640)
#define PORTC_DIRSET  _SFR_MEM8(0x0641)
#define PORTC_DIRCLR  _SFR_MEM8(0x0642)
#define PORTC_DIRTGL  _SFR_MEM8(0x0643)
#define PORTC_OUT  _SFR_MEM8(0x0644)
#define PORTC_OUTSET  _SFR_MEM8(0x0645)
#define PORTC_OUTCLR  _SFR_MEM8(0x0646)
#define PORTC_OUTTGL  _SFR_MEM8(0x0647)
#define PORTC_IN  _SFR_MEM8(0x0648)
#define PORTC_INTCTRL  _SFR_MEM8(0x0649)
#define PORTC_INT0MASK  _SFR_MEM8(0x064A)
#define PORTC_INT1MASK  _SFR_MEM8(0x064B)
#define PORTC_INTFLAGS  _SFR_MEM8(0x064C)
#define PORTC_PIN0CTRL  _SFR_MEM8(0x0650)
#define PORTC_PIN1CTRL  _SFR_MEM8(0x0651)
#define PORTC_PIN2CTRL  _SFR_MEM8(0x0652)
#define PORTC_PIN3CTRL  _SFR_MEM8(0x0653)
#define PORTC_PIN4CTRL  _SFR_MEM8(0x0654)
#define PORTC_PIN5CTRL  _SFR_MEM8(0x0655)
#define PORTC_PIN6CTRL  _SFR_MEM8(0x0656)
#define PORTC_PIN7CTRL  _SFR_MEM8(0x0657)

/* PORTD - Port D */
#define PORTD_DIR  _SFR_MEM8(0x0660)
#define PORTD_DIRSET  _SFR_MEM8(0x0661)
#define PORTD_DIRCLR  _SFR_MEM8(0x0662)
#define PORTD_DIRTGL  _SFR_MEM8(0x0663)
#define PORTD_OUT  _SFR_MEM8(0x0664)
#define PORTD_OUTSET  _SFR_MEM8(0x0665)
#define PORTD_OUTCLR  _SFR_MEM8(0x0666)
#define PORTD_OUTTGL  _SFR_MEM8(0x0667)
#define PORTD_IN  _SFR_MEM8(0x0668)
#define PORTD_INTCTRL  _SFR_MEM8(0x0669)
#define PORTD_INT0MASK  _SFR_MEM8(0x066A)
#define PORTD_INT1MASK  _SFR_MEM8(0x066B)
#define PORTD_INTFLAGS  _SFR_MEM8(0x066C)
#define PORTD_PIN0CTRL  _SFR_MEM8(0x0670)
#define PORTD_PIN1CTRL  _SFR_MEM8(0x0671)
#define PORTD_PIN2CTRL  _SFR_MEM8(0x0672)
#define PORTD_PIN3CTRL  _SFR_MEM8(0x0673)
#define PORTD_PIN4CTRL  _SFR_MEM8(0x0674)
#define PORTD_PIN5CTRL  _SFR_MEM8(0x0675)
#define PORTD_PIN6CTRL  _SFR_MEM8(0x0676)
#define PORTD_PIN7CTRL  _SFR_MEM8(0x0677)

/* PORTE - Port E */
#define PORTE_DIR  _SFR_MEM8(0x0680)
#define PORTE_DIRSET  _SFR_MEM8(0x0681)
#define PORTE_DIRCLR  _SFR_MEM8(0x0682)
#define PORTE_DIRTGL  _SFR_MEM8(0x0683)
#define PORTE_OUT  _SFR_MEM8(0x0684)
#define PORTE_OUTSET  _SFR_MEM8(0x0685)
#define PORTE_OUTCLR  _SFR_MEM8(0x0686)
#define PORTE_OUTTGL  _SFR_MEM8(0x0687)
#define PORTE_IN  _SFR_MEM8(0x0688)
#define PORTE_INTCTRL  _SFR_MEM8(0x0689)
#define PORTE_INT0MASK  _SFR_MEM8(0x068A)
#define PORTE_INT1MASK  _SFR_MEM8(0x068B)
#define PORTE_INTFLAGS  _SFR_MEM8(0x068C)
#define PORTE_PIN0CTRL  _SFR_MEM8(0x0690)
#define PORTE_PIN1CTRL  _SFR_MEM8(0x0691)
#define PORTE_PIN2CTRL  _SFR_MEM8(0x0692)
#define PORTE_PIN3CTRL  _SFR_MEM8(0x0693)
#define PORTE_PIN4CTRL  _SFR_MEM8(0x0694)
#define PORTE_PIN5CTRL  _SFR_MEM8(0x0695)
#define PORTE_PIN6CTRL  _SFR_MEM8(0x0696)
#define PORTE_PIN7CTRL  _SFR_MEM8(0x0697)

/* PORTF - Port F */
#define PORTF_DIR  _SFR_MEM8(0x06A0)
#define PORTF_DIRSET  _SFR_MEM8(0x06A1)
#define PORTF_DIRCLR  _SFR_MEM8(0x06A2)
#define PORTF_DIRTGL  _SFR_MEM8(0x06A3)
#define PORTF_OUT  _SFR_MEM8(0x06A4)
#define PORTF_OUTSET  _SFR_MEM8(0x06A5)
#define PORTF_OUTCLR  _SFR_MEM8(0x06A6)
#define PORTF_OUTTGL  _SFR_MEM8(0x06A7)
#define PORTF_IN  _SFR_MEM8(0x06A8)
#define PORTF_INTCTRL  _SFR_MEM8(0x06A9)
#define PORTF_INT0MASK  _SFR_MEM8(0x06AA)
#define PORTF_INT1MASK  _SFR_MEM8(0x06AB)
#define PORTF_INTFLAGS  _SFR_MEM8(0x06AC)
#define PORTF_PIN0CTRL  _SFR_MEM8(0x06B0)
#define PORTF_PIN1CTRL  _SFR_MEM8(0x06B1)
#define PORTF_PIN2CTRL  _SFR_MEM8(0x06B2)
#define PORTF_PIN3CTRL  _SFR_MEM8(0x06B3)
#define PORTF_PIN4CTRL  _SFR_MEM8(0x06B4)
#define PORTF_PIN5CTRL  _SFR_MEM8(0x06B5)
#define PORTF_PIN6CTRL  _SFR_MEM8(0x06B6)
#define PORTF_PIN7CTRL  _SFR_MEM8(0x06B7)

/* PORTR - Port R */
#define PORTR_DIR  _SFR_MEM8(0x07E0)
#define PORTR_DIRSET  _SFR_MEM8(0x07E1)
#define PORTR_DIRCLR  _SFR_MEM8(0x07E2)
#define PORTR_DIRTGL  _SFR_MEM8(0x07E3)
#define PORTR_OUT  _SFR_MEM8(0x07E4)
#define PORTR_OUTSET  _SFR_MEM8(0x07E5)
#define PORTR_OUTCLR  _SFR_MEM8(0x07E6)
#define PORTR_OUTTGL  _SFR_MEM8(0x07E7)
#define PORTR_IN  _SFR_MEM8(0x07E8)
#define PORTR_INTCTRL  _SFR_MEM8(0x07E9)
#define PORTR_INT0MASK  _SFR_MEM8(0x07EA)
#define PORTR_INT1MASK  _SFR_MEM8(0x07EB)
#define PORTR_INTFLAGS  _SFR_MEM8(0x07EC)
#define PORTR_PIN0CTRL  _SFR_MEM8(0x07F0)
#define PORTR_PIN1CTRL  _SFR_MEM8(0x07F1)
#define PORTR_PIN2CTRL  _SFR_MEM8(0x07F2)
#define PORTR_PIN3CTRL  _SFR_MEM8(0x07F3)
#define PORTR_PIN4CTRL  _SFR_MEM8(0x07F4)
#define PORTR_PIN5CTRL  _SFR_MEM8(0x07F5)
#define PORTR_PIN6CTRL  _SFR_MEM8(0x07F6)
#define PORTR_PIN7CTRL  _SFR_MEM8(0x07F7)

/* TCC0 - Timer/Counter C0 */
#define TCC0_CTRLA  _SFR_MEM8(0x0800)
#define TCC0_CTRLB  _SFR_MEM8(0x0801)
#define TCC0_CTRLC  _SFR_MEM8(0x0802)
#define TCC0_CTRLD  _SFR_MEM8(0x0803)
#define TCC0_CTRLE  _SFR_MEM8(0x0804)
#define TCC0_INTCTRLA  _SFR_MEM8(0x0806)
#define TCC0_INTCTRLB  _SFR_MEM8(0x0807)
#define TCC0_CTRLFCLR  _SFR_MEM8(0x0808)
#define TCC0_CTRLFSET  _SFR_MEM8(0x0809)
#define TCC0_CTRLGCLR  _SFR_MEM8(0x080A)
#define TCC0_CTRLGSET  _SFR_MEM8(0x080B)
#define TCC0_INTFLAGS  _SFR_MEM8(0x080C)
#define TCC0_TEMP  _SFR_MEM8(0x080F)
#define TCC0_CNT  _SFR_MEM16(0x0820)
#define TCC0_PER  _SFR_MEM16(0x0826)
#define TCC0_CCA  _SFR_MEM16(0x0828)
#define TCC0_CCB  _SFR_MEM16(0x082A)
#define TCC0_CCC  _SFR_MEM16(0x082C)
#define TCC0_CCD  _SFR_MEM16(0x082E)
#define TCC0_PERBUF  _SFR_MEM16(0x0836)
#define TCC0_CCABUF  _SFR_MEM16(0x0838)
#define TCC0_CCBBUF  _SFR_MEM16(0x083A)
#define TCC0_CCCBUF  _SFR_MEM16(0x083C)
#define TCC0_CCDBUF  _SFR_MEM16(0x083E)

/* TCC1 - Timer/Counter C1 */
#define TCC1_CTRLA  _SFR_MEM8(0x0840)
#define TCC1_CTRLB  _SFR_MEM8(0x0841)
#define TCC1_CTRLC  _SFR_MEM8(0x0842)
#define TCC1_CTRLD  _SFR_MEM8(0x0843)
#define TCC1_CTRLE  _SFR_MEM8(0x0844)
#define TCC1_INTCTRLA  _SFR_MEM8(0x0846)
#define TCC1_INTCTRLB  _SFR_MEM8(0x0847)
#define TCC1_CTRLFCLR  _SFR_MEM8(0x0848)
#define TCC1_CTRLFSET  _SFR_MEM8(0x0849)
#define TCC1_CTRLGCLR  _SFR_MEM8(0x084A)
#define TCC1_CTRLGSET  _SFR_MEM8(0x084B)
#define TCC1_INTFLAGS  _SFR_MEM8(0x084C)
#define TCC1_TEMP  _SFR_MEM8(0x084F)
#define TCC1_CNT  _SFR_MEM16(0x0860)
#define TCC1_PER  _SFR_MEM16(0x0866)
#define TCC1_CCA  _SFR_MEM16(0x0868)
#define TCC1_CCB  _SFR_MEM16(0x086A)
#define TCC1_PERBUF  _SFR_MEM16(0x0876)
#define TCC1_CCABUF  _SFR_MEM16(0x0878)
#define TCC1_CCBBUF  _SFR_MEM16(0x087A)

/* AWEXC - Advanced Waveform Extension C */
#define AWEXC_CTRL  _SFR_MEM8(0x0880)
#define AWEXC_FDEVMASK  _SFR_MEM8(0x0882)
#define AWEXC_FDCTRL  _SFR_MEM8(0x0883)
#define AWEXC_STATUS  _SFR_MEM8(0x0884)
#define AWEXC_DTBOTH  _SFR_MEM8(0x0886)
#define AWEXC_DTBOTHBUF  _SFR_MEM8(0x0887)
#define AWEXC_DTLS  _SFR_MEM8(0x0888)
#define AWEXC_DTHS  _SFR_MEM8(0x0889)
#define AWEXC_DTLSBUF  _SFR_MEM8(0x088A)
#define AWEXC_DTHSBUF  _SFR_MEM8(0x088B)
#define AWEXC_OUTOVEN  _SFR_MEM8(0x088C)

/* HIRESC - High-Resolution Extension C */
#define HIRESC_CTRL  _SFR_MEM8(0x0890)

/* USARTC0 - Universal Asynchronous Receiver-Transmitter C0 */
#define USARTC0_DATA  _SFR_MEM8(0x08A0)
#define USARTC0_STATUS  _SFR_MEM8(0x08A1)
#define USARTC0_CTRLA  _SFR_MEM8(0x08A3)
#define USARTC0_CTRLB  _SFR_MEM8(0x08A4)
#define USARTC0_CTRLC  _SFR_MEM8(0x08A5)
#define USARTC0_BAUDCTRLA  _SFR_MEM8(0x08A6)
#define USARTC0_BAUDCTRLB  _SFR_MEM8(0x08A7)

/* USARTC1 - Universal Asynchronous Receiver-Transmitter C1 */
#define USARTC1_DATA  _SFR_MEM8(0x08B0)
#define USARTC1_STATUS  _SFR_MEM8(0x08B1)
#define USARTC1_CTRLA  _SFR_MEM8(0x08B3)
#define USARTC1_CTRLB  _SFR_MEM8(0x08B4)
#define USARTC1_CTRLC  _SFR_MEM8(0x08B5)
#define USARTC1_BAUDCTRLA  _SFR_MEM8(0x08B6)
#define USARTC1_BAUDCTRLB  _SFR_MEM8(0x08B7)

/* SPIC - Serial Peripheral Interface C */
#define SPIC_CTRL  _SFR_MEM8(0x08C0)
#define SPIC_INTCTRL  _SFR_MEM8(0x08C1)
#define SPIC_STATUS  _SFR_MEM8(0x08C2)
#define SPIC_DATA  _SFR_MEM8(0x08C3)

/* IRCOM - IR Communication Module */
#define IRCOM_CTRL  _SFR_MEM8(0x08F8)
#define IRCOM_TXPLCTRL  _SFR_MEM8(0x08F9)
#define IRCOM_RXPLCTRL  _SFR_MEM8(0x08FA)

/* TCD0 - Timer/Counter D0 */
#define TCD0_CTRLA  _SFR_MEM8(0x0900)
#define TCD0_CTRLB  _SFR_MEM8(0x0901)
#define TCD0_CTRLC  _SFR_MEM8(0x0902)
#define TCD0_CTRLD  _SFR_MEM8(0x0903)
#define TCD0_CTRLE  _SFR_MEM8(0x0904)
#define TCD0_INTCTRLA  _SFR_MEM8(0x0906)
#define TCD0_INTCTRLB  _SFR_MEM8(0x0907)
#define TCD0_CTRLFCLR  _SFR_MEM8(0x0908)
#define TCD0_CTRLFSET  _SFR_MEM8(0x0909)
#define TCD0_CTRLGCLR  _SFR_MEM8(0x090A)
#define TCD0_CTRLGSET  _SFR_MEM8(0x090B)
#define TCD0_INTFLAGS  _SFR_MEM8(0x090C)
#define TCD0_TEMP  _SFR_MEM8(0x090F)
#define TCD0_CNT  _SFR_MEM16(0x0920)
#define TCD0_PER  _SFR_MEM16(0x0926)
#define TCD0_CCA  _SFR_MEM16(0x0928)
#define TCD0_CCB  _SFR_MEM16(0x092A)
#define TCD0_CCC  _SFR_MEM16(0x092C)
#define TCD0_CCD  _SFR_MEM16(0x092E)
#define TCD0_PERBUF  _SFR_MEM16(0x0936)
#define TCD0_CCABUF  _SFR_MEM16(0x0938)
#define TCD0_CCBBUF  _SFR_MEM16(0x093A)
#define TCD0_CCCBUF  _SFR_MEM16(0x093C)
#define TCD0_CCDBUF  _SFR_MEM16(0x093E)

/* TCD1 - Timer/Counter D1 */
#define TCD1_CTRLA  _SFR_MEM8(0x0940)
#define TCD1_CTRLB  _SFR_MEM8(0x0941)
#define TCD1_CTRLC  _SFR_MEM8(0x0942)
#define TCD1_CTRLD  _SFR_MEM8(0x0943)
#define TCD1_CTRLE  _SFR_MEM8(0x0944)
#define TCD1_INTCTRLA  _SFR_MEM8(0x0946)
#define TCD1_INTCTRLB  _SFR_MEM8(0x0947)
#define TCD1_CTRLFCLR  _SFR_MEM8(0x0948)
#define TCD1_CTRLFSET  _SFR_MEM8(0x0949)
#define TCD1_CTRLGCLR  _SFR_MEM8(0x094A)
#define TCD1_CTRLGSET  _SFR_MEM8(0x094B)
#define TCD1_INTFLAGS  _SFR_MEM8(0x094C)
#define TCD1_TEMP  _SFR_MEM8(0x094F)
#define TCD1_CNT  _SFR_MEM16(0x0960)
#define TCD1_PER  _SFR_MEM16(0x0966)
#define TCD1_CCA  _SFR_MEM16(0x0968)
#define TCD1_CCB  _SFR_MEM16(0x096A)
#define TCD1_PERBUF  _SFR_MEM16(0x0976)
#define TCD1_CCABUF  _SFR_MEM16(0x0978)
#define TCD1_CCBBUF  _SFR_MEM16(0x097A)

/* HIRESD - High-Resolution Extension D */
#define HIRESD_CTRL  _SFR_MEM8(0x0990)

/* USARTD0 - Universal Asynchronous Receiver-Transmitter D0 */
#define USARTD0_DATA  _SFR_MEM8(0x09A0)
#define USARTD0_STATUS  _SFR_MEM8(0x09A1)
#define USARTD0_CTRLA  _SFR_MEM8(0x09A3)
#define USARTD0_CTRLB  _SFR_MEM8(0x09A4)
#define USARTD0_CTRLC  _SFR_MEM8(0x09A5)
#define USARTD0_BAUDCTRLA  _SFR_MEM8(0x09A6)
#define USARTD0_BAUDCTRLB  _SFR_MEM8(0x09A7)

/* USARTD1 - Universal Asynchronous Receiver-Transmitter D1 */
#define USARTD1_DATA  _SFR_MEM8(0x09B0)
#define USARTD1_STATUS  _SFR_MEM8(0x09B1)
#define USARTD1_CTRLA  _SFR_MEM8(0x09B3)
#define USARTD1_CTRLB  _SFR_MEM8(0x09B4)
#define USARTD1_CTRLC  _SFR_MEM8(0x09B5)
#define USARTD1_BAUDCTRLA  _SFR_MEM8(0x09B6)
#define USARTD1_BAUDCTRLB  _SFR_MEM8(0x09B7)

/* SPID - Serial Peripheral Interface D */
#define SPID_CTRL  _SFR_MEM8(0x09C0)
#define SPID_INTCTRL  _SFR_MEM8(0x09C1)
#define SPID_STATUS  _SFR_MEM8(0x09C2)
#define SPID_DATA  _SFR_MEM8(0x09C3)

/* TCE0 - Timer/Counter E0 */
#define TCE0_CTRLA  _SFR_MEM8(0x0A00)
#define TCE0_CTRLB  _SFR_MEM8(0x0A01)
#define TCE0_CTRLC  _SFR_MEM8(0x0A02)
#define TCE0_CTRLD  _SFR_MEM8(0x0A03)
#define TCE0_CTRLE  _SFR_MEM8(0x0A04)
#define TCE0_INTCTRLA  _SFR_MEM8(0x0A06)
#define TCE0_INTCTRLB  _SFR_MEM8(0x0A07)
#define TCE0_CTRLFCLR  _SFR_MEM8(0x0A08)
#define TCE0_CTRLFSET  _SFR_MEM8(0x0A09)
#define TCE0_CTRLGCLR  _SFR_MEM8(0x0A0A)
#define TCE0_CTRLGSET  _SFR_MEM8(0x0A0B)
#define TCE0_INTFLAGS  _SFR_MEM8(0x0A0C)
#define TCE0_TEMP  _SFR_MEM8(0x0A0F)
#define TCE0_CNT  _SFR_MEM16(0x0A20)
#define TCE0_PER  _SFR_MEM16(0x0A26)
#define TCE0_CCA  _SFR_MEM16(0x0A28)
#define TCE0_CCB  _SFR_MEM16(0x0A2A)
#define TCE0_CCC  _SFR_MEM16(0x0A2C)
#define TCE0_CCD  _SFR_MEM16(0x0A2E)
#define TCE0_PERBUF  _SFR_MEM16(0x0A36)
#define TCE0_CCABUF  _SFR_MEM16(0x0A38)
#define TCE0_CCBBUF  _SFR_MEM16(0x0A3A)
#define TCE0_CCCBUF  _SFR_MEM16(0x0A3C)
#define TCE0_CCDBUF  _SFR_MEM16(0x0A3E)

/* TCE1 - Timer/Counter E1 */
#define TCE1_CTRLA  _SFR_MEM8(0x0A40)
#define TCE1_CTRLB  _SFR_MEM8(0x0A41)
#define TCE1_CTRLC  _SFR_MEM8(0x0A42)
#define TCE1_CTRLD  _SFR_MEM8(0x0A43)
#define TCE1_CTRLE  _SFR_MEM8(0x0A44)
#define TCE1_INTCTRLA  _SFR_MEM8(0x0A46)
#define TCE1_INTCTRLB  _SFR_MEM8(0x0A47)
#define TCE1_CTRLFCLR  _SFR_MEM8(0x0A48)
#define TCE1_CTRLFSET  _SFR_MEM8(0x0A49)
#define TCE1_CTRLGCLR  _SFR_MEM8(0x0A4A)
#define TCE1_CTRLGSET  _SFR_MEM8(0x0A4B)
#define TCE1_INTFLAGS  _SFR_MEM8(0x0A4C)
#define TCE1_TEMP  _SFR_MEM8(0x0A4F)
#define TCE1_CNT  _SFR_MEM16(0x0A60)
#define TCE1_PER  _SFR_MEM16(0x0A66)
#define TCE1_CCA  _SFR_MEM16(0x0A68)
#define TCE1_CCB  _SFR_MEM16(0x0A6A)
#define TCE1_PERBUF  _SFR_MEM16(0x0A76)
#define TCE1_CCABUF  _SFR_MEM16(0x0A78)
#define TCE1_CCBBUF  _SFR_MEM16(0x0A7A)

/* AWEXE - Advanced Waveform Extension E */
#define AWEXE_CTRL  _SFR_MEM8(0x0A80)
#define AWEXE_FDEVMASK  _SFR_MEM8(0x0A82)
#define AWEXE_FDCTRL  _SFR_MEM8(0x0A83)
#define AWEXE_STATUS  _SFR_MEM8(0x0A84)
#define AWEXE_DTBOTH  _SFR_MEM8(0x0A86)
#define AWEXE_DTBOTHBUF  _SFR_MEM8(0x0A87)
#define AWEXE_DTLS  _SFR_MEM8(0x0A88)
#define AWEXE_DTHS  _SFR_MEM8(0x0A89)
#define AWEXE_DTLSBUF  _SFR_MEM8(0x0A8A)
#define AWEXE_DTHSBUF  _SFR_MEM8(0x0A8B)
#define AWEXE_OUTOVEN  _SFR_MEM8(0x0A8C)

/* HIRESE - High-Resolution Extension E */
#define HIRESE_CTRL  _SFR_MEM8(0x0A90)

/* USARTE0 - Universal Asynchronous Receiver-Transmitter E0 */
#define USARTE0_DATA  _SFR_MEM8(0x0AA0)
#define USARTE0_STATUS  _SFR_MEM8(0x0AA1)
#define USARTE0_CTRLA  _SFR_MEM8(0x0AA3)
#define USARTE0_CTRLB  _SFR_MEM8(0x0AA4)
#define USARTE0_CTRLC  _SFR_MEM8(0x0AA5)
#define USARTE0_BAUDCTRLA  _SFR_MEM8(0x0AA6)
#define USARTE0_BAUDCTRLB  _SFR_MEM8(0x0AA7)

/* USARTE1 - Universal Asynchronous Receiver-Transmitter E1 */
#define USARTE1_DATA  _SFR_MEM8(0x0AB0)
#define USARTE1_STATUS  _SFR_MEM8(0x0AB1)
#define USARTE1_CTRLA  _SFR_MEM8(0x0AB3)
#define USARTE1_CTRLB  _SFR_MEM8(0x0AB4)
#define USARTE1_CTRLC  _SFR_MEM8(0x0AB5)
#define USARTE1_BAUDCTRLA  _SFR_MEM8(0x0AB6)
#define USARTE1_BAUDCTRLB  _SFR_MEM8(0x0AB7)

/* SPIE - Serial Peripheral Interface E */
#define SPIE_CTRL  _SFR_MEM8(0x0AC0)
#define SPIE_INTCTRL  _SFR_MEM8(0x0AC1)
#define SPIE_STATUS  _SFR_MEM8(0x0AC2)
#define SPIE_DATA  _SFR_MEM8(0x0AC3)

/* TCF0 - Timer/Counter F0 */
#define TCF0_CTRLA  _SFR_MEM8(0x0B00)
#define TCF0_CTRLB  _SFR_MEM8(0x0B01)
#define TCF0_CTRLC  _SFR_MEM8(0x0B02)
#define TCF0_CTRLD  _SFR_MEM8(0x0B03)
#define TCF0_CTRLE  _SFR_MEM8(0x0B04)
#define TCF0_INTCTRLA  _SFR_MEM8(0x0B06)
#define TCF0_INTCTRLB  _SFR_MEM8(0x0B07)
#define TCF0_CTRLFCLR  _SFR_MEM8(0x0B08)
#define TCF0_CTRLFSET  _SFR_MEM8(0x0B09)
#define TCF0_CTRLGCLR  _SFR_MEM8(0x0B0A)
#define TCF0_CTRLGSET  _SFR_MEM8(0x0B0B)
#define TCF0_INTFLAGS  _SFR_MEM8(0x0B0C)
#define TCF0_TEMP  _SFR_MEM8(0x0B0F)
#define TCF0_CNT  _SFR_MEM16(0x0B20)
#define TCF0_PER  _SFR_MEM16(0x0B26)
#define TCF0_CCA  _SFR_MEM16(0x0B28)
#define TCF0_CCB  _SFR_MEM16(0x0B2A)
#define TCF0_CCC  _SFR_MEM16(0x0B2C)
#define TCF0_CCD  _SFR_MEM16(0x0B2E)
#define TCF0_PERBUF  _SFR_MEM16(0x0B36)
#define TCF0_CCABUF  _SFR_MEM16(0x0B38)
#define TCF0_CCBBUF  _SFR_MEM16(0x0B3A)
#define TCF0_CCCBUF  _SFR_MEM16(0x0B3C)
#define TCF0_CCDBUF  _SFR_MEM16(0x0B3E)

/* HIRESF - High-Resolution Extension F */
#define HIRESF_CTRL  _SFR_MEM8(0x0B90)

/* USARTF0 - Universal Asynchronous Receiver-Transmitter F0 */
#define USARTF0_DATA  _SFR_MEM8(0x0BA0)
#define USARTF0_STATUS  _SFR_MEM8(0x0BA1)
#define USARTF0_CTRLA  _SFR_MEM8(0x0BA3)
#define USARTF0_CTRLB  _SFR_MEM8(0x0BA4)
#define USARTF0_CTRLC  _SFR_MEM8(0x0BA5)
#define USARTF0_BAUDCTRLA  _SFR_MEM8(0x0BA6)
#define USARTF0_BAUDCTRLB  _SFR_MEM8(0x0BA7)

/* USARTF1 - Universal Asynchronous Receiver-Transmitter F1 */
#define USARTF1_DATA  _SFR_MEM8(0x0BB0)
#define USARTF1_STATUS  _SFR_MEM8(0x0BB1)
#define USARTF1_CTRLA  _SFR_MEM8(0x0BB3)
#define USARTF1_CTRLB  _SFR_MEM8(0x0BB4)
#define USARTF1_CTRLC  _SFR_MEM8(0x0BB5)
#define USARTF1_BAUDCTRLA  _SFR_MEM8(0x0BB6)
#define USARTF1_BAUDCTRLB  _SFR_MEM8(0x0BB7)

/* SPIF - Serial Peripheral Interface F */
#define SPIF_CTRL  _SFR_MEM8(0x0BC0)
#define SPIF_INTCTRL  _SFR_MEM8(0x0BC1)
#define SPIF_STATUS  _SFR_MEM8(0x0BC2)
#define SPIF_DATA  _SFR_MEM8(0x0BC3)



/*================== Bitfield Definitions ================== */

/* XOCD - On-Chip Debug System */
/* OCD.OCDR1  bit masks and bit positions */
#define OCD_OCDRD_bm  0x01  /* OCDR Dirty bit mask. */
#define OCD_OCDRD_bp  0  /* OCDR Dirty bit position. */


/* CPU - CPU */
/* CPU.CCP  bit masks and bit positions */
#define CPU_CCP_gm  0xFF  /* CCP signature group mask. */
#define CPU_CCP_gp  0  /* CCP signature group position. */
#define CPU_CCP0_bm  (1<<0)  /* CCP signature bit 0 mask. */
#define CPU_CCP0_bp  0  /* CCP signature bit 0 position. */
#define CPU_CCP1_bm  (1<<1)  /* CCP signature bit 1 mask. */
#define CPU_CCP1_bp  1  /* CCP signature bit 1 position. */
#define CPU_CCP2_bm  (1<<2)  /* CCP signature bit 2 mask. */
#define CPU_CCP2_bp  2  /* CCP signature bit 2 position. */
#define CPU_CCP3_bm  (1<<3)  /* CCP signature bit 3 mask. */
#define CPU_CCP3_bp  3  /* CCP signature bit 3 position. */
#define CPU_CCP4_bm  (1<<4)  /* CCP signature bit 4 mask. */
#define CPU_CCP4_bp  4  /* CCP signature bit 4 position. */
#define CPU_CCP5_bm  (1<<5)  /* CCP signature bit 5 mask. */
#define CPU_CCP5_bp  5  /* CCP signature bit 5 position. */
#define CPU_CCP6_bm  (1<<6)  /* CCP signature bit 6 mask. */
#define CPU_CCP6_bp  6  /* CCP signature bit 6 position. */
#define CPU_CCP7_bm  (1<<7)  /* CCP signature bit 7 mask. */
#define CPU_CCP7_bp  7  /* CCP signature bit 7 position. */


/* CPU.SREG  bit masks and bit positions */
#define CPU_I_bm  0x80  /* Global Interrupt Enable Flag bit mask. */
#define CPU_I_bp  7  /* Global Interrupt Enable Flag bit position. */

#define CPU_T_bm  0x40  /* Transfer Bit bit mask. */
#define CPU_T_bp  6  /* Transfer Bit bit position. */

#define CPU_H_bm  0x20  /* Half Carry Flag bit mask. */
#define CPU_H_bp  5  /* Half Carry Flag bit position. */

#define CPU_S_bm  0x10  /* N Exclusive Or V Flag bit mask. */
#define CPU_S_bp  4  /* N Exclusive Or V Flag bit position. */

#define CPU_V_bm  0x08  /* Two's Complement Overflow Flag bit mask. */
#define CPU_V_bp  3  /* Two's Complement Overflow Flag bit position. */

#define CPU_N_bm  0x04  /* Negative Flag bit mask. */
#define CPU_N_bp  2  /* Negative Flag bit position. */

#define CPU_Z_bm  0x02  /* Zero Flag bit mask. */
#define CPU_Z_bp  1  /* Zero Flag bit position. */

#define CPU_C_bm  0x01  /* Carry Flag bit mask. */
#define CPU_C_bp  0  /* Carry Flag bit position. */


/* CLK - Clock System */
/* CLK.CTRL  bit masks and bit positions */
#define CLK_SCLKSEL_gm  0x07  /* System Clock Selection group mask. */
#define CLK_SCLKSEL_gp  0  /* System Clock Selection group position. */
#define CLK_SCLKSEL0_bm  (1<<0)  /* System Clock Selection bit 0 mask. */
#define CLK_SCLKSEL0_bp  0  /* System Clock Selection bit 0 position. */
#define CLK_SCLKSEL1_bm  (1<<1)  /* System Clock Selection bit 1 mask. */
#define CLK_SCLKSEL1_bp  1  /* System Clock Selection bit 1 position. */
#define CLK_SCLKSEL2_bm  (1<<2)  /* System Clock Selection bit 2 mask. */
#define CLK_SCLKSEL2_bp  2  /* System Clock Selection bit 2 position. */


/* CLK.PSCTRL  bit masks and bit positions */
#define CLK_PSADIV_gm  0x7C  /* Prescaler A Division Factor group mask. */
#define CLK_PSADIV_gp  2  /* Prescaler A Division Factor group position. */
#define CLK_PSADIV0_bm  (1<<2)  /* Prescaler A Division Factor bit 0 mask. */
#define CLK_PSADIV0_bp  2  /* Prescaler A Division Factor bit 0 position. */
#define CLK_PSADIV1_bm  (1<<3)  /* Prescaler A Division Factor bit 1 mask. */
#define CLK_PSADIV1_bp  3  /* Prescaler A Division Factor bit 1 position. */
#define CLK_PSADIV2_bm  (1<<4)  /* Prescaler A Division Factor bit 2 mask. */
#define CLK_PSADIV2_bp  4  /* Prescaler A Division Factor bit 2 position. */
#define CLK_PSADIV3_bm  (1<<5)  /* Prescaler A Division Factor bit 3 mask. */
#define CLK_PSADIV3_bp  5  /* Prescaler A Division Factor bit 3 position. */
#define CLK_PSADIV4_bm  (1<<6)  /* Prescaler A Division Factor bit 4 mask. */
#define CLK_PSADIV4_bp  6  /* Prescaler A Division Factor bit 4 position. */

#define CLK_PSBCDIV_gm  0x03  /* Prescaler B and C Division factor group mask. */
#define CLK_PSBCDIV_gp  0  /* Prescaler B and C Division factor group position. */
#define CLK_PSBCDIV0_bm  (1<<0)  /* Prescaler B and C Division factor bit 0 mask. */
#define CLK_PSBCDIV0_bp  0  /* Prescaler B and C Division factor bit 0 position. */
#define CLK_PSBCDIV1_bm  (1<<1)  /* Prescaler B and C Division factor bit 1 mask. */
#define CLK_PSBCDIV1_bp  1  /* Prescaler B and C Division factor bit 1 position. */


/* CLK.LOCK  bit masks and bit positions */
#define CLK_LOCK_bm  0x01  /* Clock System Lock bit mask. */
#define CLK_LOCK_bp  0  /* Clock System Lock bit position. */


/* CLK.RTCCTRL  bit masks and bit positions */
#define CLK_RTCSRC_gm  0x0E  /* Clock Source group mask. */
#define CLK_RTCSRC_gp  1  /* Clock Source group position. */
#define CLK_RTCSRC0_bm  (1<<1)  /* Clock Source bit 0 mask. */
#define CLK_RTCSRC0_bp  1  /* Clock Source bit 0 position. */
#define CLK_RTCSRC1_bm  (1<<2)  /* Clock Source bit 1 mask. */
#define CLK_RTCSRC1_bp  2  /* Clock Source bit 1 position. */
#define CLK_RTCSRC2_bm  (1<<3)  /* Clock Source bit 2 mask. */
#define CLK_RTCSRC2_bp  3  /* Clock Source bit 2 position. */

#define CLK_RTCEN_bm  0x01  /* RTC Clock Source Enable bit mask. */
#define CLK_RTCEN_bp  0  /* RTC Clock Source Enable bit position. */


/* PR.PR  bit masks and bit positions */
#define PR_EBI_bm  0x08  /* External Bus Interface bit mask. */
#define PR_EBI_bp  3  /* External Bus Interface bit position. */

#define PR_RTC_bm  0x04  /* Real-time Counter bit mask. */
#define PR_RTC_bp  2  /* Real-time Counter bit position. */

#define PR_EVSYS_bm  0x02  /* Event System bit mask. */
#define PR_EVSYS_bp  1  /* Event System bit position. */

#define PR_DMA_bm  0x01  /* DMA-Controller bit mask. */
#define PR_DMA_bp  0  /* DMA-Controller bit position. */


/* PR.PRPA  bit masks and bit positions */
#define PR_DAC_bm  0x04  /* Port A DAC bit mask. */
#define PR_DAC_bp  2  /* Port A DAC bit position. */

#define PR_ADC_bm  0x02  /* Port A ADC bit mask. */
#define PR_ADC_bp  1  /* Port A ADC bit position. */

#define PR_AC_bm  0x01  /* Port A Analog Comparator bit mask. */
#define PR_AC_bp  0  /* Port A Analog Comparator bit position. */


/* PR.PRPB  bit masks and bit positions */
/* PR_DAC_bm  Predefined. */
/* PR_DAC_bp  Predefined. */

/* PR_ADC_bm  Predefined. */
/* PR_ADC_bp  Predefined. */

/* PR_AC_bm  Predefined. */
/* PR_AC_bp  Predefined. */


/* PR.PRPC  bit masks and bit positions */
#define PR_TWI_bm  0x40  /* Port C Two-wire Interface bit mask. */
#define PR_TWI_bp  6  /* Port C Two-wire Interface bit position. */

#define PR_USART1_bm  0x20  /* Port C USART1 bit mask. */
#define PR_USART1_bp  5  /* Port C USART1 bit position. */

#define PR_USART0_bm  0x10  /* Port C USART0 bit mask. */
#define PR_USART0_bp  4  /* Port C USART0 bit position. */

#define PR_SPI_bm  0x08  /* Port C SPI bit mask. */
#define PR_SPI_bp  3  /* Port C SPI bit position. */

#define PR_HIRES_bm  0x04  /* Port C AWEX bit mask. */
#define PR_HIRES_bp  2  /* Port C AWEX bit position. */

#define PR_TC1_bm  0x02  /* Port C Timer/Counter1 bit mask. */
#define PR_TC1_bp  1  /* Port C Timer/Counter1 bit position. */

#define PR_TC0_bm  0x01  /* Port C Timer/Counter0 bit mask. */
#define PR_TC0_bp  0  /* Port C Timer/Counter0 bit position. */


/* PR.PRPD  bit masks and bit positions */
/* PR_TWI_bm  Predefined. */
/* PR_TWI_bp  Predefined. */

/* PR_USART1_bm  Predefined. */
/* PR_USART1_bp  Predefined. */

/* PR_USART0_bm  Predefined. */
/* PR_USART0_bp  Predefined. */

/* PR_SPI_bm  Predefined. */
/* PR_SPI_bp  Predefined. */

/* PR_HIRES_bm  Predefined. */
/* PR_HIRES_bp  Predefined. */

/* PR_TC1_bm  Predefined. */
/* PR_TC1_bp  Predefined. */

/* PR_TC0_bm  Predefined. */
/* PR_TC0_bp  Predefined. */


/* PR.PRPE  bit masks and bit positions */
/* PR_TWI_bm  Predefined. */
/* PR_TWI_bp  Predefined. */

/* PR_USART1_bm  Predefined. */
/* PR_USART1_bp  Predefined. */

/* PR_USART0_bm  Predefined. */
/* PR_USART0_bp  Predefined. */

/* PR_SPI_bm  Predefined. */
/* PR_SPI_bp  Predefined. */

/* PR_HIRES_bm  Predefined. */
/* PR_HIRES_bp  Predefined. */

/* PR_TC1_bm  Predefined. */
/* PR_TC1_bp  Predefined. */

/* PR_TC0_bm  Predefined. */
/* PR_TC0_bp  Predefined. */


/* PR.PRPF  bit masks and bit positions */
/* PR_TWI_bm  Predefined. */
/* PR_TWI_bp  Predefined. */

/* PR_USART1_bm  Predefined. */
/* PR_USART1_bp  Predefined. */

/* PR_USART0_bm  Predefined. */
/* PR_USART0_bp  Predefined. */

/* PR_SPI_bm  Predefined. */
/* PR_SPI_bp  Predefined. */

/* PR_HIRES_bm  Predefined. */
/* PR_HIRES_bp  Predefined. */

/* PR_TC1_bm  Predefined. */
/* PR_TC1_bp  Predefined. */

/* PR_TC0_bm  Predefined. */
/* PR_TC0_bp  Predefined. */


/* SLEEP - Sleep Controller */
/* SLEEP.CTRL  bit masks and bit positions */
#define SLEEP_SMODE_gm  0x0E  /* Sleep Mode group mask. */
#define SLEEP_SMODE_gp  1  /* Sleep Mode group position. */
#define SLEEP_SMODE0_bm  (1<<1)  /* Sleep Mode bit 0 mask. */
#define SLEEP_SMODE0_bp  1  /* Sleep Mode bit 0 position. */
#define SLEEP_SMODE1_bm  (1<<2)  /* Sleep Mode bit 1 mask. */
#define SLEEP_SMODE1_bp  2  /* Sleep Mode bit 1 position. */
#define SLEEP_SMODE2_bm  (1<<3)  /* Sleep Mode bit 2 mask. */
#define SLEEP_SMODE2_bp  3  /* Sleep Mode bit 2 position. */

#define SLEEP_SEN_bm  0x01  /* Sleep Enable bit mask. */
#define SLEEP_SEN_bp  0  /* Sleep Enable bit position. */


/* OSC - Oscillator */
/* OSC.CTRL  bit masks and bit positions */
#define OSC_PLLEN_bm  0x10  /* PLL Enable bit mask. */
#define OSC_PLLEN_bp  4  /* PLL Enable bit position. */

#define OSC_XOSCEN_bm  0x08  /* External Oscillator Enable bit mask. */
#define OSC_XOSCEN_bp  3  /* External Oscillator Enable bit position. */

#define OSC_RC32KEN_bm  0x04  /* Internal 32kHz RC Oscillator Enable bit mask. */
#define OSC_RC32KEN_bp  2  /* Internal 32kHz RC Oscillator Enable bit position. */

#define OSC_RC32MEN_bm  0x02  /* Internal 32MHz RC Oscillator Enable bit mask. */
#define OSC_RC32MEN_bp  1  /* Internal 32MHz RC Oscillator Enable bit position. */

#define OSC_RC2MEN_bm  0x01  /* Internal 2MHz RC Oscillator Enable bit mask. */
#define OSC_RC2MEN_bp  0  /* Internal 2MHz RC Oscillator Enable bit position. */


/* OSC.STATUS  bit masks and bit positions */
#define OSC_PLLRDY_bm  0x10  /* PLL Ready bit mask. */
#define OSC_PLLRDY_bp  4  /* PLL Ready bit position. */

#define OSC_XOSCRDY_bm  0x08  /* External Oscillator Ready bit mask. */
#define OSC_XOSCRDY_bp  3  /* External Oscillator Ready bit position. */

#define OSC_RC32KRDY_bm  0x04  /* Internal 32kHz RC Oscillator Ready bit mask. */
#define OSC_RC32KRDY_bp  2  /* Internal 32kHz RC Oscillator Ready bit position. */

#define OSC_RC32MRDY_bm  0x02  /* Internal 32MHz RC Oscillator Ready bit mask. */
#define OSC_RC32MRDY_bp  1  /* Internal 32MHz RC Oscillator Ready bit position. */

#define OSC_RC2MRDY_bm  0x01  /* Internal 2MHz RC Oscillator Ready bit mask. */
#define OSC_RC2MRDY_bp  0  /* Internal 2MHz RC Oscillator Ready bit position. */


/* OSC.XOSCCTRL  bit masks and bit positions */
#define OSC_FRQRANGE_gm  0xC0  /* Frequency Range group mask. */
#define OSC_FRQRANGE_gp  6  /* Frequency Range group position. */
#define OSC_FRQRANGE0_bm  (1<<6)  /* Frequency Range bit 0 mask. */
#define OSC_FRQRANGE0_bp  6  /* Frequency Range bit 0 position. */
#define OSC_FRQRANGE1_bm  (1<<7)  /* Frequency Range bit 1 mask. */
#define OSC_FRQRANGE1_bp  7  /* Frequency Range bit 1 position. */

#define OSC_X32KLPM_bm  0x20  /* 32kHz XTAL OSC Low-power Mode bit mask. */
#define OSC_X32KLPM_bp  5  /* 32kHz XTAL OSC Low-power Mode bit position. */

#define OSC_XOSCSEL_gm  0x0F  /* External Oscillator Selection and Startup Time group mask. */
#define OSC_XOSCSEL_gp  0  /* External Oscillator Selection and Startup Time group position. */
#define OSC_XOSCSEL0_bm  (1<<0)  /* External Oscillator Selection and Startup Time bit 0 mask. */
#define OSC_XOSCSEL0_bp  0  /* External Oscillator Selection and Startup Time bit 0 position. */
#define OSC_XOSCSEL1_bm  (1<<1)  /* External Oscillator Selection and Startup Time bit 1 mask. */
#define OSC_XOSCSEL1_bp  1  /* External Oscillator Selection and Startup Time bit 1 position. */
#define OSC_XOSCSEL2_bm  (1<<2)  /* External Oscillator Selection and Startup Time bit 2 mask. */
#define OSC_XOSCSEL2_bp  2  /* External Oscillator Selection and Startup Time bit 2 position. */
#define OSC_XOSCSEL3_bm  (1<<3)  /* External Oscillator Selection and Startup Time bit 3 mask. */
#define OSC_XOSCSEL3_bp  3  /* External Oscillator Selection and Startup Time bit 3 position. */


/* OSC.XOSCFAIL  bit masks and bit positions */
#define OSC_XOSCFDIF_bm  0x02  /* Failure Detection Interrupt Flag bit mask. */
#define OSC_XOSCFDIF_bp  1  /* Failure Detection Interrupt Flag bit position. */

#define OSC_XOSCFDEN_bm  0x01  /* Failure Detection Enable bit mask. */
#define OSC_XOSCFDEN_bp  0  /* Failure Detection Enable bit position. */


/* OSC.PLLCTRL  bit masks and bit positions */
#define OSC_PLLSRC_gm  0xC0  /* Clock Source group mask. */
#define OSC_PLLSRC_gp  6  /* Clock Source group position. */
#define OSC_PLLSRC0_bm  (1<<6)  /* Clock Source bit 0 mask. */
#define OSC_PLLSRC0_bp  6  /* Clock Source bit 0 position. */
#define OSC_PLLSRC1_bm  (1<<7)  /* Clock Source bit 1 mask. */
#define OSC_PLLSRC1_bp  7  /* Clock Source bit 1 position. */

#define OSC_PLLFAC_gm  0x1F  /* Multiplication Factor group mask. */
#define OSC_PLLFAC_gp  0  /* Multiplication Factor group position. */
#define OSC_PLLFAC0_bm  (1<<0)  /* Multiplication Factor bit 0 mask. */
#define OSC_PLLFAC0_bp  0  /* Multiplication Factor bit 0 position. */
#define OSC_PLLFAC1_bm  (1<<1)  /* Multiplication Factor bit 1 mask. */
#define OSC_PLLFAC1_bp  1  /* Multiplication Factor bit 1 position. */
#define OSC_PLLFAC2_bm  (1<<2)  /* Multiplication Factor bit 2 mask. */
#define OSC_PLLFAC2_bp  2  /* Multiplication Factor bit 2 position. */
#define OSC_PLLFAC3_bm  (1<<3)  /* Multiplication Factor bit 3 mask. */
#define OSC_PLLFAC3_bp  3  /* Multiplication Factor bit 3 position. */
#define OSC_PLLFAC4_bm  (1<<4)  /* Multiplication Factor bit 4 mask. */
#define OSC_PLLFAC4_bp  4  /* Multiplication Factor bit 4 position. */


/* OSC.DFLLCTRL  bit masks and bit positions */
#define OSC_RC32MCREF_bm  0x02  /* 32MHz Calibration Reference bit mask. */
#define OSC_RC32MCREF_bp  1  /* 32MHz Calibration Reference bit position. */

#define OSC_RC2MCREF_bm  0x01  /* 2MHz Calibration Reference bit mask. */
#define OSC_RC2MCREF_bp  0  /* 2MHz Calibration Reference bit position. */


/* DFLL - DFLL */
/* DFLL.CTRL  bit masks and bit positions */
#define DFLL_ENABLE_bm  0x01  /* DFLL Enable bit mask. */
#define DFLL_ENABLE_bp  0  /* DFLL Enable bit position. */


/* DFLL.CALA  bit masks and bit positions */
#define DFLL_CALL_gm  0x7F  /* DFLL Calibration bits [6:0] group mask. */
#define DFLL_CALL_gp  0  /* DFLL Calibration bits [6:0] group position. */
#define DFLL_CALL0_bm  (1<<0)  /* DFLL Calibration bits [6:0] bit 0 mask. */
#define DFLL_CALL0_bp  0  /* DFLL Calibration bits [6:0] bit 0 position. */
#define DFLL_CALL1_bm  (1<<1)  /* DFLL Calibration bits [6:0] bit 1 mask. */
#define DFLL_CALL1_bp  1  /* DFLL Calibration bits [6:0] bit 1 position. */
#define DFLL_CALL2_bm  (1<<2)  /* DFLL Calibration bits [6:0] bit 2 mask. */
#define DFLL_CALL2_bp  2  /* DFLL Calibration bits [6:0] bit 2 position. */
#define DFLL_CALL3_bm  (1<<3)  /* DFLL Calibration bits [6:0] bit 3 mask. */
#define DFLL_CALL3_bp  3  /* DFLL Calibration bits [6:0] bit 3 position. */
#define DFLL_CALL4_bm  (1<<4)  /* DFLL Calibration bits [6:0] bit 4 mask. */
#define DFLL_CALL4_bp  4  /* DFLL Calibration bits [6:0] bit 4 position. */
#define DFLL_CALL5_bm  (1<<5)  /* DFLL Calibration bits [6:0] bit 5 mask. */
#define DFLL_CALL5_bp  5  /* DFLL Calibration bits [6:0] bit 5 position. */
#define DFLL_CALL6_bm  (1<<6)  /* DFLL Calibration bits [6:0] bit 6 mask. */
#define DFLL_CALL6_bp  6  /* DFLL Calibration bits [6:0] bit 6 position. */


/* DFLL.CALB  bit masks and bit positions */
#define DFLL_CALH_gm  0x3F  /* DFLL Calibration bits [12:7] group mask. */
#define DFLL_CALH_gp  0  /* DFLL Calibration bits [12:7] group position. */
#define DFLL_CALH0_bm  (1<<0)  /* DFLL Calibration bits [12:7] bit 0 mask. */
#define DFLL_CALH0_bp  0  /* DFLL Calibration bits [12:7] bit 0 position. */
#define DFLL_CALH1_bm  (1<<1)  /* DFLL Calibration bits [12:7] bit 1 mask. */
#define DFLL_CALH1_bp  1  /* DFLL Calibration bits [12:7] bit 1 position. */
#define DFLL_CALH2_bm  (1<<2)  /* DFLL Calibration bits [12:7] bit 2 mask. */
#define DFLL_CALH2_bp  2  /* DFLL Calibration bits [12:7] bit 2 position. */
#define DFLL_CALH3_bm  (1<<3)  /* DFLL Calibration bits [12:7] bit 3 mask. */
#define DFLL_CALH3_bp  3  /* DFLL Calibration bits [12:7] bit 3 position. */
#define DFLL_CALH4_bm  (1<<4)  /* DFLL Calibration bits [12:7] bit 4 mask. */
#define DFLL_CALH4_bp  4  /* DFLL Calibration bits [12:7] bit 4 position. */
#define DFLL_CALH5_bm  (1<<5)  /* DFLL Calibration bits [12:7] bit 5 mask. */
#define DFLL_CALH5_bp  5  /* DFLL Calibration bits [12:7] bit 5 position. */


/* RST - Reset */
/* RST.STATUS  bit masks and bit positions */
#define RST_SDRF_bm  0x40  /* Spike Detection Reset Flag bit mask. */
#define RST_SDRF_bp  6  /* Spike Detection Reset Flag bit position. */

#define RST_SRF_bm  0x20  /* Software Reset Flag bit mask. */
#define RST_SRF_bp  5  /* Software Reset Flag bit position. */

#define RST_PDIRF_bm  0x10  /* Programming and Debug Interface Interface Reset Flag bit mask. */
#define RST_PDIRF_bp  4  /* Programming and Debug Interface Interface Reset Flag bit position. */

#define RST_WDRF_bm  0x08  /* Watchdog Reset Flag bit mask. */
#define RST_WDRF_bp  3  /* Watchdog Reset Flag bit position. */

#define RST_BORF_bm  0x04  /* Brown-out Reset Flag bit mask. */
#define RST_BORF_bp  2  /* Brown-out Reset Flag bit position. */

#define RST_EXTRF_bm  0x02  /* External Reset Flag bit mask. */
#define RST_EXTRF_bp  1  /* External Reset Flag bit position. */

#define RST_PORF_bm  0x01  /* Power-on Reset Flag bit mask. */
#define RST_PORF_bp  0  /* Power-on Reset Flag bit position. */


/* RST.CTRL  bit masks and bit positions */
#define RST_SWRST_bm  0x01  /* Software Reset bit mask. */
#define RST_SWRST_bp  0  /* Software Reset bit position. */


/* WDT - Watch-Dog Timer */
/* WDT.CTRL  bit masks and bit positions */
#define WDT_PER_gm  0x3C  /* Period group mask. */
#define WDT_PER_gp  2  /* Period group position. */
#define WDT_PER0_bm  (1<<2)  /* Period bit 0 mask. */
#define WDT_PER0_bp  2  /* Period bit 0 position. */
#define WDT_PER1_bm  (1<<3)  /* Period bit 1 mask. */
#define WDT_PER1_bp  3  /* Period bit 1 position. */
#define WDT_PER2_bm  (1<<4)  /* Period bit 2 mask. */
#define WDT_PER2_bp  4  /* Period bit 2 position. */
#define WDT_PER3_bm  (1<<5)  /* Period bit 3 mask. */
#define WDT_PER3_bp  5  /* Period bit 3 position. */

#define WDT_ENABLE_bm  0x02  /* Enable bit mask. */
#define WDT_ENABLE_bp  1  /* Enable bit position. */

#define WDT_CEN_bm  0x01  /* Change Enable bit mask. */
#define WDT_CEN_bp  0  /* Change Enable bit position. */


/* WDT.WINCTRL  bit masks and bit positions */
#define WDT_WPER_gm  0x3C  /* Windowed Mode Period group mask. */
#define WDT_WPER_gp  2  /* Windowed Mode Period group position. */
#define WDT_WPER0_bm  (1<<2)  /* Windowed Mode Period bit 0 mask. */
#define WDT_WPER0_bp  2  /* Windowed Mode Period bit 0 position. */
#define WDT_WPER1_bm  (1<<3)  /* Windowed Mode Period bit 1 mask. */
#define WDT_WPER1_bp  3  /* Windowed Mode Period bit 1 position. */
#define WDT_WPER2_bm  (1<<4)  /* Windowed Mode Period bit 2 mask. */
#define WDT_WPER2_bp  4  /* Windowed Mode Period bit 2 position. */
#define WDT_WPER3_bm  (1<<5)  /* Windowed Mode Period bit 3 mask. */
#define WDT_WPER3_bp  5  /* Windowed Mode Period bit 3 position. */

#define WDT_WEN_bm  0x02  /* Windowed Mode Enable bit mask. */
#define WDT_WEN_bp  1  /* Windowed Mode Enable bit position. */

#define WDT_WCEN_bm  0x01  /* Windowed Mode Change Enable bit mask. */
#define WDT_WCEN_bp  0  /* Windowed Mode Change Enable bit position. */


/* WDT.STATUS  bit masks and bit positions */
#define WDT_SYNCBUSY_bm  0x01  /* Syncronization busy bit mask. */
#define WDT_SYNCBUSY_bp  0  /* Syncronization busy bit position. */


/* MCU - MCU Control */
/* MCU.MCUCR  bit masks and bit positions */
#define MCU_JTAGD_bm  0x01  /* JTAG Disable bit mask. */
#define MCU_JTAGD_bp  0  /* JTAG Disable bit position. */


/* MCU.EVSYSLOCK  bit masks and bit positions */
#define MCU_EVSYS1LOCK_bm  0x10  /* Event Channel 4-7 Lock bit mask. */
#define MCU_EVSYS1LOCK_bp  4  /* Event Channel 4-7 Lock bit position. */

#define MCU_EVSYS0LOCK_bm  0x01  /* Event Channel 0-3 Lock bit mask. */
#define MCU_EVSYS0LOCK_bp  0  /* Event Channel 0-3 Lock bit position. */


/* MCU.AWEXLOCK  bit masks and bit positions */
#define MCU_AWEXELOCK_bm  0x04  /* AWeX on T/C E0 Lock bit mask. */
#define MCU_AWEXELOCK_bp  2  /* AWeX on T/C E0 Lock bit position. */

#define MCU_AWEXCLOCK_bm  0x01  /* AWeX on T/C C0 Lock bit mask. */
#define MCU_AWEXCLOCK_bp  0  /* AWeX on T/C C0 Lock bit position. */


/* PMIC - Programmable Multi-level Interrupt Controller */
/* PMIC.STATUS  bit masks and bit positions */
#define PMIC_NMIEX_bm  0x80  /* Non-maskable Interrupt Executing bit mask. */
#define PMIC_NMIEX_bp  7  /* Non-maskable Interrupt Executing bit position. */

#define PMIC_HILVLEX_bm  0x04  /* High Level Interrupt Executing bit mask. */
#define PMIC_HILVLEX_bp  2  /* High Level Interrupt Executing bit position. */

#define PMIC_MEDLVLEX_bm  0x02  /* Medium Level Interrupt Executing bit mask. */
#define PMIC_MEDLVLEX_bp  1  /* Medium Level Interrupt Executing bit position. */

#define PMIC_LOLVLEX_bm  0x01  /* Low Level Interrupt Executing bit mask. */
#define PMIC_LOLVLEX_bp  0  /* Low Level Interrupt Executing bit position. */


/* PMIC.CTRL  bit masks and bit positions */
#define PMIC_RREN_bm  0x80  /* Round-Robin Priority Enable bit mask. */
#define PMIC_RREN_bp  7  /* Round-Robin Priority Enable bit position. */

#define PMIC_IVSEL_bm  0x40  /* Interrupt Vector Select bit mask. */
#define PMIC_IVSEL_bp  6  /* Interrupt Vector Select bit position. */

#define PMIC_HILVLEN_bm  0x04  /* High Level Enable bit mask. */
#define PMIC_HILVLEN_bp  2  /* High Level Enable bit position. */

#define PMIC_MEDLVLEN_bm  0x02  /* Medium Level Enable bit mask. */
#define PMIC_MEDLVLEN_bp  1  /* Medium Level Enable bit position. */

#define PMIC_LOLVLEN_bm  0x01  /* Low Level Enable bit mask. */
#define PMIC_LOLVLEN_bp  0  /* Low Level Enable bit position. */


/* DMA - DMA Controller */
/* DMA_CH.CTRLA  bit masks and bit positions */
#define DMA_CH_ENABLE_bm  0x80  /* Channel Enable bit mask. */
#define DMA_CH_ENABLE_bp  7  /* Channel Enable bit position. */

#define DMA_CH_RESET_bm  0x40  /* Channel Software Reset bit mask. */
#define DMA_CH_RESET_bp  6  /* Channel Software Reset bit position. */

#define DMA_CH_REPEAT_bm  0x20  /* Channel Repeat Mode bit mask. */
#define DMA_CH_REPEAT_bp  5  /* Channel Repeat Mode bit position. */

#define DMA_CH_TRFREQ_bm  0x10  /* Channel Transfer Request bit mask. */
#define DMA_CH_TRFREQ_bp  4  /* Channel Transfer Request bit position. */

#define DMA_CH_SINGLE_bm  0x04  /* Channel Single Shot Data Transfer bit mask. */
#define DMA_CH_SINGLE_bp  2  /* Channel Single Shot Data Transfer bit position. */

#define DMA_CH_BURSTLEN_gm  0x03  /* Channel Transfer Mode group mask. */
#define DMA_CH_BURSTLEN_gp  0  /* Channel Transfer Mode group position. */
#define DMA_CH_BURSTLEN0_bm  (1<<0)  /* Channel Transfer Mode bit 0 mask. */
#define DMA_CH_BURSTLEN0_bp  0  /* Channel Transfer Mode bit 0 position. */
#define DMA_CH_BURSTLEN1_bm  (1<<1)  /* Channel Transfer Mode bit 1 mask. */
#define DMA_CH_BURSTLEN1_bp  1  /* Channel Transfer Mode bit 1 position. */


/* DMA_CH.CTRLB  bit masks and bit positions */
#define DMA_CH_CHBUSY_bm  0x80  /* Block Transfer Busy bit mask. */
#define DMA_CH_CHBUSY_bp  7  /* Block Transfer Busy bit position. */

#define DMA_CH_CHPEND_bm  0x40  /* Block Transfer Pending bit mask. */
#define DMA_CH_CHPEND_bp  6  /* Block Transfer Pending bit position. */

#define DMA_CH_ERRIF_bm  0x20  /* Block Transfer Error Interrupt Flag bit mask. */
#define DMA_CH_ERRIF_bp  5  /* Block Transfer Error Interrupt Flag bit position. */

#define DMA_CH_TRNIF_bm  0x10  /* Transaction Complete Interrup Flag bit mask. */
#define DMA_CH_TRNIF_bp  4  /* Transaction Complete Interrup Flag bit position. */

#define DMA_CH_ERRINTLVL_gm  0x0C  /* Transfer Error Interrupt Level group mask. */
#define DMA_CH_ERRINTLVL_gp  2  /* Transfer Error Interrupt Level group position. */
#define DMA_CH_ERRINTLVL0_bm  (1<<2)  /* Transfer Error Interrupt Level bit 0 mask. */
#define DMA_CH_ERRINTLVL0_bp  2  /* Transfer Error Interrupt Level bit 0 position. */
#define DMA_CH_ERRINTLVL1_bm  (1<<3)  /* Transfer Error Interrupt Level bit 1 mask. */
#define DMA_CH_ERRINTLVL1_bp  3  /* Transfer Error Interrupt Level bit 1 position. */

#define DMA_CH_TRNINTLVL_gm  0x03  /* Transaction Complete Interrupt Level group mask. */
#define DMA_CH_TRNINTLVL_gp  0  /* Transaction Complete Interrupt Level group position. */
#define DMA_CH_TRNINTLVL0_bm  (1<<0)  /* Transaction Complete Interrupt Level bit 0 mask. */
#define DMA_CH_TRNINTLVL0_bp  0  /* Transaction Complete Interrupt Level bit 0 position. */
#define DMA_CH_TRNINTLVL1_bm  (1<<1)  /* Transaction Complete Interrupt Level bit 1 mask. */
#define DMA_CH_TRNINTLVL1_bp  1  /* Transaction Complete Interrupt Level bit 1 position. */


/* DMA_CH.ADDRCTRL  bit masks and bit positions */
#define DMA_CH_SRCRELOAD_gm  0xC0  /* Channel Source Address Reload group mask. */
#define DMA_CH_SRCRELOAD_gp  6  /* Channel Source Address Reload group position. */
#define DMA_CH_SRCRELOAD0_bm  (1<<6)  /* Channel Source Address Reload bit 0 mask. */
#define DMA_CH_SRCRELOAD0_bp  6  /* Channel Source Address Reload bit 0 position. */
#define DMA_CH_SRCRELOAD1_bm  (1<<7)  /* Channel Source Address Reload bit 1 mask. */
#define DMA_CH_SRCRELOAD1_bp  7  /* Channel Source Address Reload bit 1 position. */

#define DMA_CH_SRCDIR_gm  0x30  /* Channel Source Address Mode group mask. */
#define DMA_CH_SRCDIR_gp  4  /* Channel Source Address Mode group position. */
#define DMA_CH_SRCDIR0_bm  (1<<4)  /* Channel Source Address Mode bit 0 mask. */
#define DMA_CH_SRCDIR0_bp  4  /* Channel Source Address Mode bit 0 position. */
#define DMA_CH_SRCDIR1_bm  (1<<5)  /* Channel Source Address Mode bit 1 mask. */
#define DMA_CH_SRCDIR1_bp  5  /* Channel Source Address Mode bit 1 position. */

#define DMA_CH_DESTRELOAD_gm  0x0C  /* Channel Destination Address Reload group mask. */
#define DMA_CH_DESTRELOAD_gp  2  /* Channel Destination Address Reload group position. */
#define DMA_CH_DESTRELOAD0_bm  (1<<2)  /* Channel Destination Address Reload bit 0 mask. */
#define DMA_CH_DESTRELOAD0_bp  2  /* Channel Destination Address Reload bit 0 position. */
#define DMA_CH_DESTRELOAD1_bm  (1<<3)  /* Channel Destination Address Reload bit 1 mask. */
#define DMA_CH_DESTRELOAD1_bp  3  /* Channel Destination Address Reload bit 1 position. */

#define DMA_CH_DESTDIR_gm  0x03  /* Channel Destination Address Mode group mask. */
#define DMA_CH_DESTDIR_gp  0  /* Channel Destination Address Mode group position. */
#define DMA_CH_DESTDIR0_bm  (1<<0)  /* Channel Destination Address Mode bit 0 mask. */
#define DMA_CH_DESTDIR0_bp  0  /* Channel Destination Address Mode bit 0 position. */
#define DMA_CH_DESTDIR1_bm  (1<<1)  /* Channel Destination Address Mode bit 1 mask. */
#define DMA_CH_DESTDIR1_bp  1  /* Channel Destination Address Mode bit 1 position. */


/* DMA_CH.TRIGSRC  bit masks and bit positions */
#define DMA_CH_TRIGSRC_gm  0xFF  /* Channel Trigger Source group mask. */
#define DMA_CH_TRIGSRC_gp  0  /* Channel Trigger Source group position. */
#define DMA_CH_TRIGSRC0_bm  (1<<0)  /* Channel Trigger Source bit 0 mask. */
#define DMA_CH_TRIGSRC0_bp  0  /* Channel Trigger Source bit 0 position. */
#define DMA_CH_TRIGSRC1_bm  (1<<1)  /* Channel Trigger Source bit 1 mask. */
#define DMA_CH_TRIGSRC1_bp  1  /* Channel Trigger Source bit 1 position. */
#define DMA_CH_TRIGSRC2_bm  (1<<2)  /* Channel Trigger Source bit 2 mask. */
#define DMA_CH_TRIGSRC2_bp  2  /* Channel Trigger Source bit 2 position. */
#define DMA_CH_TRIGSRC3_bm  (1<<3)  /* Channel Trigger Source bit 3 mask. */
#define DMA_CH_TRIGSRC3_bp  3  /* Channel Trigger Source bit 3 position. */
#define DMA_CH_TRIGSRC4_bm  (1<<4)  /* Channel Trigger Source bit 4 mask. */
#define DMA_CH_TRIGSRC4_bp  4  /* Channel Trigger Source bit 4 position. */
#define DMA_CH_TRIGSRC5_bm  (1<<5)  /* Channel Trigger Source bit 5 mask. */
#define DMA_CH_TRIGSRC5_bp  5  /* Channel Trigger Source bit 5 position. */
#define DMA_CH_TRIGSRC6_bm  (1<<6)  /* Channel Trigger Source bit 6 mask. */
#define DMA_CH_TRIGSRC6_bp  6  /* Channel Trigger Source bit 6 position. */
#define DMA_CH_TRIGSRC7_bm  (1<<7)  /* Channel Trigger Source bit 7 mask. */
#define DMA_CH_TRIGSRC7_bp  7  /* Channel Trigger Source bit 7 position. */


/* DMA.CTRL  bit masks and bit positions */
#define DMA_ENABLE_bm  0x80  /* Enable bit mask. */
#define DMA_ENABLE_bp  7  /* Enable bit position. */

#define DMA_RESET_bm  0x40  /* Software Reset bit mask. */
#define DMA_RESET_bp  6  /* Software Reset bit position. */

#define DMA_DBUFMODE_gm  0x0C  /* Double Buffering Mode group mask. */
#define DMA_DBUFMODE_gp  2  /* Double Buffering Mode group position. */
#define DMA_DBUFMODE0_bm  (1<<2)  /* Double Buffering Mode bit 0 mask. */
#define DMA_DBUFMODE0_bp  2  /* Double Buffering Mode bit 0 position. */
#define DMA_DBUFMODE1_bm  (1<<3)  /* Double Buffering Mode bit 1 mask. */
#define DMA_DBUFMODE1_bp  3  /* Double Buffering Mode bit 1 position. */

#define DMA_PRIMODE_gm  0x03  /* Channel Priority Mode group mask. */
#define DMA_PRIMODE_gp  0  /* Channel Priority Mode group position. */
#define DMA_PRIMODE0_bm  (1<<0)  /* Channel Priority Mode bit 0 mask. */
#define DMA_PRIMODE0_bp  0  /* Channel Priority Mode bit 0 position. */
#define DMA_PRIMODE1_bm  (1<<1)  /* Channel Priority Mode bit 1 mask. */
#define DMA_PRIMODE1_bp  1  /* Channel Priority Mode bit 1 position. */


/* DMA.INTFLAGS  bit masks and bit positions */
#define DMA_CH3ERRIF_bm  0x80  /* Channel 3 Block Transfer Error Interrupt Flag bit mask. */
#define DMA_CH3ERRIF_bp  7  /* Channel 3 Block Transfer Error Interrupt Flag bit position. */

#define DMA_CH2ERRIF_bm  0x40  /* Channel 2 Block Transfer Error Interrupt Flag bit mask. */
#define DMA_CH2ERRIF_bp  6  /* Channel 2 Block Transfer Error Interrupt Flag bit position. */

#define DMA_CH1ERRIF_bm  0x20  /* Channel 1 Block Transfer Error Interrupt Flag bit mask. */
#define DMA_CH1ERRIF_bp  5  /* Channel 1 Block Transfer Error Interrupt Flag bit position. */

#define DMA_CH0ERRIF_bm  0x10  /* Channel 0 Block Transfer Error Interrupt Flag bit mask. */
#define DMA_CH0ERRIF_bp  4  /* Channel 0 Block Transfer Error Interrupt Flag bit position. */

#define DMA_CH3TRNIF_bm  0x08  /* Channel 3 Transaction Complete Interrupt Flag bit mask. */
#define DMA_CH3TRNIF_bp  3  /* Channel 3 Transaction Complete Interrupt Flag bit position. */

#define DMA_CH2TRNIF_bm  0x04  /* Channel 2 Transaction Complete Interrupt Flag bit mask. */
#define DMA_CH2TRNIF_bp  2  /* Channel 2 Transaction Complete Interrupt Flag bit position. */

#define DMA_CH1TRNIF_bm  0x02  /* Channel 1 Transaction Complete Interrupt Flag bit mask. */
#define DMA_CH1TRNIF_bp  1  /* Channel 1 Transaction Complete Interrupt Flag bit position. */

#define DMA_CH0TRNIF_bm  0x01  /* Channel 0 Transaction Complete Interrupt Flag bit mask. */
#define DMA_CH0TRNIF_bp  0  /* Channel 0 Transaction Complete Interrupt Flag bit position. */


/* DMA.STATUS  bit masks and bit positions */
#define DMA_CH3BUSY_bm  0x80  /* Channel 3 Block Transfer Busy bit mask. */
#define DMA_CH3BUSY_bp  7  /* Channel 3 Block Transfer Busy bit position. */

#define DMA_CH2BUSY_bm  0x40  /* Channel 2 Block Transfer Busy bit mask. */
#define DMA_CH2BUSY_bp  6  /* Channel 2 Block Transfer Busy bit position. */

#define DMA_CH1BUSY_bm  0x20  /* Channel 1 Block Transfer Busy bit mask. */
#define DMA_CH1BUSY_bp  5  /* Channel 1 Block Transfer Busy bit position. */

#define DMA_CH0BUSY_bm  0x10  /* Channel 0 Block Transfer Busy bit mask. */
#define DMA_CH0BUSY_bp  4  /* Channel 0 Block Transfer Busy bit position. */

#define DMA_CH3PEND_bm  0x08  /* Channel 3 Block Transfer Pending bit mask. */
#define DMA_CH3PEND_bp  3  /* Channel 3 Block Transfer Pending bit position. */

#define DMA_CH2PEND_bm  0x04  /* Channel 2 Block Transfer Pending bit mask. */
#define DMA_CH2PEND_bp  2  /* Channel 2 Block Transfer Pending bit position. */

#define DMA_CH1PEND_bm  0x02  /* Channel 1 Block Transfer Pending bit mask. */
#define DMA_CH1PEND_bp  1  /* Channel 1 Block Transfer Pending bit position. */

#define DMA_CH0PEND_bm  0x01  /* Channel 0 Block Transfer Pending bit mask. */
#define DMA_CH0PEND_bp  0  /* Channel 0 Block Transfer Pending bit position. */


/* EVSYS - Event System */
/* EVSYS.CH0MUX  bit masks and bit positions */
#define EVSYS_CHMUX_gm  0xFF  /* Event Channel 0 Multiplexer group mask. */
#define EVSYS_CHMUX_gp  0  /* Event Channel 0 Multiplexer group position. */
#define EVSYS_CHMUX0_bm  (1<<0)  /* Event Channel 0 Multiplexer bit 0 mask. */
#define EVSYS_CHMUX0_bp  0  /* Event Channel 0 Multiplexer bit 0 position. */
#define EVSYS_CHMUX1_bm  (1<<1)  /* Event Channel 0 Multiplexer bit 1 mask. */
#define EVSYS_CHMUX1_bp  1  /* Event Channel 0 Multiplexer bit 1 position. */
#define EVSYS_CHMUX2_bm  (1<<2)  /* Event Channel 0 Multiplexer bit 2 mask. */
#define EVSYS_CHMUX2_bp  2  /* Event Channel 0 Multiplexer bit 2 position. */
#define EVSYS_CHMUX3_bm  (1<<3)  /* Event Channel 0 Multiplexer bit 3 mask. */
#define EVSYS_CHMUX3_bp  3  /* Event Channel 0 Multiplexer bit 3 position. */
#define EVSYS_CHMUX4_bm  (1<<4)  /* Event Channel 0 Multiplexer bit 4 mask. */
#define EVSYS_CHMUX4_bp  4  /* Event Channel 0 Multiplexer bit 4 position. */
#define EVSYS_CHMUX5_bm  (1<<5)  /* Event Channel 0 Multiplexer bit 5 mask. */
#define EVSYS_CHMUX5_bp  5  /* Event Channel 0 Multiplexer bit 5 position. */
#define EVSYS_CHMUX6_bm  (1<<6)  /* Event Channel 0 Multiplexer bit 6 mask. */
#define EVSYS_CHMUX6_bp  6  /* Event Channel 0 Multiplexer bit 6 position. */
#define EVSYS_CHMUX7_bm  (1<<7)  /* Event Channel 0 Multiplexer bit 7 mask. */
#define EVSYS_CHMUX7_bp  7  /* Event Channel 0 Multiplexer bit 7 position. */


/* EVSYS.CH1MUX  bit masks and bit positions */
/* EVSYS_CHMUX_gm  Predefined. */
/* EVSYS_CHMUX_gp  Predefined. */
/* EVSYS_CHMUX0_bm  Predefined. */
/* EVSYS_CHMUX0_bp  Predefined. */
/* EVSYS_CHMUX1_bm  Predefined. */
/* EVSYS_CHMUX1_bp  Predefined. */
/* EVSYS_CHMUX2_bm  Predefined. */
/* EVSYS_CHMUX2_bp  Predefined. */
/* EVSYS_CHMUX3_bm  Predefined. */
/* EVSYS_CHMUX3_bp  Predefined. */
/* EVSYS_CHMUX4_bm  Predefined. */
/* EVSYS_CHMUX4_bp  Predefined. */
/* EVSYS_CHMUX5_bm  Predefined. */
/* EVSYS_CHMUX5_bp  Predefined. */
/* EVSYS_CHMUX6_bm  Predefined. */
/* EVSYS_CHMUX6_bp  Predefined. */
/* EVSYS_CHMUX7_bm  Predefined. */
/* EVSYS_CHMUX7_bp  Predefined. */


/* EVSYS.CH2MUX  bit masks and bit positions */
/* EVSYS_CHMUX_gm  Predefined. */
/* EVSYS_CHMUX_gp  Predefined. */
/* EVSYS_CHMUX0_bm  Predefined. */
/* EVSYS_CHMUX0_bp  Predefined. */
/* EVSYS_CHMUX1_bm  Predefined. */
/* EVSYS_CHMUX1_bp  Predefined. */
/* EVSYS_CHMUX2_bm  Predefined. */
/* EVSYS_CHMUX2_bp  Predefined. */
/* EVSYS_CHMUX3_bm  Predefined. */
/* EVSYS_CHMUX3_bp  Predefined. */
/* EVSYS_CHMUX4_bm  Predefined. */
/* EVSYS_CHMUX4_bp  Predefined. */
/* EVSYS_CHMUX5_bm  Predefined. */
/* EVSYS_CHMUX5_bp  Predefined. */
/* EVSYS_CHMUX6_bm  Predefined. */
/* EVSYS_CHMUX6_bp  Predefined. */
/* EVSYS_CHMUX7_bm  Predefined. */
/* EVSYS_CHMUX7_bp  Predefined. */


/* EVSYS.CH3MUX  bit masks and bit positions */
/* EVSYS_CHMUX_gm  Predefined. */
/* EVSYS_CHMUX_gp  Predefined. */
/* EVSYS_CHMUX0_bm  Predefined. */
/* EVSYS_CHMUX0_bp  Predefined. */
/* EVSYS_CHMUX1_bm  Predefined. */
/* EVSYS_CHMUX1_bp  Predefined. */
/* EVSYS_CHMUX2_bm  Predefined. */
/* EVSYS_CHMUX2_bp  Predefined. */
/* EVSYS_CHMUX3_bm  Predefined. */
/* EVSYS_CHMUX3_bp  Predefined. */
/* EVSYS_CHMUX4_bm  Predefined. */
/* EVSYS_CHMUX4_bp  Predefined. */
/* EVSYS_CHMUX5_bm  Predefined. */
/* EVSYS_CHMUX5_bp  Predefined. */
/* EVSYS_CHMUX6_bm  Predefined. */
/* EVSYS_CHMUX6_bp  Predefined. */
/* EVSYS_CHMUX7_bm  Predefined. */
/* EVSYS_CHMUX7_bp  Predefined. */


/* EVSYS.CH4MUX  bit masks and bit positions */
/* EVSYS_CHMUX_gm  Predefined. */
/* EVSYS_CHMUX_gp  Predefined. */
/* EVSYS_CHMUX0_bm  Predefined. */
/* EVSYS_CHMUX0_bp  Predefined. */
/* EVSYS_CHMUX1_bm  Predefined. */
/* EVSYS_CHMUX1_bp  Predefined. */
/* EVSYS_CHMUX2_bm  Predefined. */
/* EVSYS_CHMUX2_bp  Predefined. */
/* EVSYS_CHMUX3_bm  Predefined. */
/* EVSYS_CHMUX3_bp  Predefined. */
/* EVSYS_CHMUX4_bm  Predefined. */
/* EVSYS_CHMUX4_bp  Predefined. */
/* EVSYS_CHMUX5_bm  Predefined. */
/* EVSYS_CHMUX5_bp  Predefined. */
/* EVSYS_CHMUX6_bm  Predefined. */
/* EVSYS_CHMUX6_bp  Predefined. */
/* EVSYS_CHMUX7_bm  Predefined. */
/* EVSYS_CHMUX7_bp  Predefined. */


/* EVSYS.CH5MUX  bit masks and bit positions */
/* EVSYS_CHMUX_gm  Predefined. */
/* EVSYS_CHMUX_gp  Predefined. */
/* EVSYS_CHMUX0_bm  Predefined. */
/* EVSYS_CHMUX0_bp  Predefined. */
/* EVSYS_CHMUX1_bm  Predefined. */
/* EVSYS_CHMUX1_bp  Predefined. */
/* EVSYS_CHMUX2_bm  Predefined. */
/* EVSYS_CHMUX2_bp  Predefined. */
/* EVSYS_CHMUX3_bm  Predefined. */
/* EVSYS_CHMUX3_bp  Predefined. */
/* EVSYS_CHMUX4_bm  Predefined. */
/* EVSYS_CHMUX4_bp  Predefined. */
/* EVSYS_CHMUX5_bm  Predefined. */
/* EVSYS_CHMUX5_bp  Predefined. */
/* EVSYS_CHMUX6_bm  Predefined. */
/* EVSYS_CHMUX6_bp  Predefined. */
/* EVSYS_CHMUX7_bm  Predefined. */
/* EVSYS_CHMUX7_bp  Predefined. */


/* EVSYS.CH6MUX  bit masks and bit positions */
/* EVSYS_CHMUX_gm  Predefined. */
/* EVSYS_CHMUX_gp  Predefined. */
/* EVSYS_CHMUX0_bm  Predefined. */
/* EVSYS_CHMUX0_bp  Predefined. */
/* EVSYS_CHMUX1_bm  Predefined. */
/* EVSYS_CHMUX1_bp  Predefined. */
/* EVSYS_CHMUX2_bm  Predefined. */
/* EVSYS_CHMUX2_bp  Predefined. */
/* EVSYS_CHMUX3_bm  Predefined. */
/* EVSYS_CHMUX3_bp  Predefined. */
/* EVSYS_CHMUX4_bm  Predefined. */
/* EVSYS_CHMUX4_bp  Predefined. */
/* EVSYS_CHMUX5_bm  Predefined. */
/* EVSYS_CHMUX5_bp  Predefined. */
/* EVSYS_CHMUX6_bm  Predefined. */
/* EVSYS_CHMUX6_bp  Predefined. */
/* EVSYS_CHMUX7_bm  Predefined. */
/* EVSYS_CHMUX7_bp  Predefined. */


/* EVSYS.CH7MUX  bit masks and bit positions */
/* EVSYS_CHMUX_gm  Predefined. */
/* EVSYS_CHMUX_gp  Predefined. */
/* EVSYS_CHMUX0_bm  Predefined. */
/* EVSYS_CHMUX0_bp  Predefined. */
/* EVSYS_CHMUX1_bm  Predefined. */
/* EVSYS_CHMUX1_bp  Predefined. */
/* EVSYS_CHMUX2_bm  Predefined. */
/* EVSYS_CHMUX2_bp  Predefined. */
/* EVSYS_CHMUX3_bm  Predefined. */
/* EVSYS_CHMUX3_bp  Predefined. */
/* EVSYS_CHMUX4_bm  Predefined. */
/* EVSYS_CHMUX4_bp  Predefined. */
/* EVSYS_CHMUX5_bm  Predefined. */
/* EVSYS_CHMUX5_bp  Predefined. */
/* EVSYS_CHMUX6_bm  Predefined. */
/* EVSYS_CHMUX6_bp  Predefined. */
/* EVSYS_CHMUX7_bm  Predefined. */
/* EVSYS_CHMUX7_bp  Predefined. */


/* EVSYS.CH0CTRL  bit masks and bit positions */
#define EVSYS_QDIRM_gm  0x60  /* Quadrature Decoder Index Recognition Mode group mask. */
#define EVSYS_QDIRM_gp  5  /* Quadrature Decoder Index Recognition Mode group position. */
#define EVSYS_QDIRM0_bm  (1<<5)  /* Quadrature Decoder Index Recognition Mode bit 0 mask. */
#define EVSYS_QDIRM0_bp  5  /* Quadrature Decoder Index Recognition Mode bit 0 position. */
#define EVSYS_QDIRM1_bm  (1<<6)  /* Quadrature Decoder Index Recognition Mode bit 1 mask. */
#define EVSYS_QDIRM1_bp  6  /* Quadrature Decoder Index Recognition Mode bit 1 position. */

#define EVSYS_QDIEN_bm  0x10  /* Quadrature Decoder Index Enable bit mask. */
#define EVSYS_QDIEN_bp  4  /* Quadrature Decoder Index Enable bit position. */

#define EVSYS_QDEN_bm  0x08  /* Quadrature Decoder Enable bit mask. */
#define EVSYS_QDEN_bp  3  /* Quadrature Decoder Enable bit position. */

#define EVSYS_DIGFILT_gm  0x07  /* Digital Filter group mask. */
#define EVSYS_DIGFILT_gp  0  /* Digital Filter group position. */
#define EVSYS_DIGFILT0_bm  (1<<0)  /* Digital Filter bit 0 mask. */
#define EVSYS_DIGFILT0_bp  0  /* Digital Filter bit 0 position. */
#define EVSYS_DIGFILT1_bm  (1<<1)  /* Digital Filter bit 1 mask. */
#define EVSYS_DIGFILT1_bp  1  /* Digital Filter bit 1 position. */
#define EVSYS_DIGFILT2_bm  (1<<2)  /* Digital Filter bit 2 mask. */
#define EVSYS_DIGFILT2_bp  2  /* Digital Filter bit 2 position. */


/* EVSYS.CH1CTRL  bit masks and bit positions */
/* EVSYS_DIGFILT_gm  Predefined. */
/* EVSYS_DIGFILT_gp  Predefined. */
/* EVSYS_DIGFILT0_bm  Predefined. */
/* EVSYS_DIGFILT0_bp  Predefined. */
/* EVSYS_DIGFILT1_bm  Predefined. */
/* EVSYS_DIGFILT1_bp  Predefined. */
/* EVSYS_DIGFILT2_bm  Predefined. */
/* EVSYS_DIGFILT2_bp  Predefined. */


/* EVSYS.CH2CTRL  bit masks and bit positions */
/* EVSYS_QDIRM_gm  Predefined. */
/* EVSYS_QDIRM_gp  Predefined. */
/* EVSYS_QDIRM0_bm  Predefined. */
/* EVSYS_QDIRM0_bp  Predefined. */
/* EVSYS_QDIRM1_bm  Predefined. */
/* EVSYS_QDIRM1_bp  Predefined. */

/* EVSYS_QDIEN_bm  Predefined. */
/* EVSYS_QDIEN_bp  Predefined. */

/* EVSYS_QDEN_bm  Predefined. */
/* EVSYS_QDEN_bp  Predefined. */

/* EVSYS_DIGFILT_gm  Predefined. */
/* EVSYS_DIGFILT_gp  Predefined. */
/* EVSYS_DIGFILT0_bm  Predefined. */
/* EVSYS_DIGFILT0_bp  Predefined. */
/* EVSYS_DIGFILT1_bm  Predefined. */
/* EVSYS_DIGFILT1_bp  Predefined. */
/* EVSYS_DIGFILT2_bm  Predefined. */
/* EVSYS_DIGFILT2_bp  Predefined. */


/* EVSYS.CH3CTRL  bit masks and bit positions */
/* EVSYS_DIGFILT_gm  Predefined. */
/* EVSYS_DIGFILT_gp  Predefined. */
/* EVSYS_DIGFILT0_bm  Predefined. */
/* EVSYS_DIGFILT0_bp  Predefined. */
/* EVSYS_DIGFILT1_bm  Predefined. */
/* EVSYS_DIGFILT1_bp  Predefined. */
/* EVSYS_DIGFILT2_bm  Predefined. */
/* EVSYS_DIGFILT2_bp  Predefined. */


/* EVSYS.CH4CTRL  bit masks and bit positions */
/* EVSYS_QDIRM_gm  Predefined. */
/* EVSYS_QDIRM_gp  Predefined. */
/* EVSYS_QDIRM0_bm  Predefined. */
/* EVSYS_QDIRM0_bp  Predefined. */
/* EVSYS_QDIRM1_bm  Predefined. */
/* EVSYS_QDIRM1_bp  Predefined. */

/* EVSYS_QDIEN_bm  Predefined. */
/* EVSYS_QDIEN_bp  Predefined. */

/* EVSYS_QDEN_bm  Predefined. */
/* EVSYS_QDEN_bp  Predefined. */

/* EVSYS_DIGFILT_gm  Predefined. */
/* EVSYS_DIGFILT_gp  Predefined. */
/* EVSYS_DIGFILT0_bm  Predefined. */
/* EVSYS_DIGFILT0_bp  Predefined. */
/* EVSYS_DIGFILT1_bm  Predefined. */
/* EVSYS_DIGFILT1_bp  Predefined. */
/* EVSYS_DIGFILT2_bm  Predefined. */
/* EVSYS_DIGFILT2_bp  Predefined. */


/* EVSYS.CH5CTRL  bit masks and bit positions */
/* EVSYS_DIGFILT_gm  Predefined. */
/* EVSYS_DIGFILT_gp  Predefined. */
/* EVSYS_DIGFILT0_bm  Predefined. */
/* EVSYS_DIGFILT0_bp  Predefined. */
/* EVSYS_DIGFILT1_bm  Predefined. */
/* EVSYS_DIGFILT1_bp  Predefined. */
/* EVSYS_DIGFILT2_bm  Predefined. */
/* EVSYS_DIGFILT2_bp  Predefined. */


/* EVSYS.CH6CTRL  bit masks and bit positions */
/* EVSYS_DIGFILT_gm  Predefined. */
/* EVSYS_DIGFILT_gp  Predefined. */
/* EVSYS_DIGFILT0_bm  Predefined. */
/* EVSYS_DIGFILT0_bp  Predefined. */
/* EVSYS_DIGFILT1_bm  Predefined. */
/* EVSYS_DIGFILT1_bp  Predefined. */
/* EVSYS_DIGFILT2_bm  Predefined. */
/* EVSYS_DIGFILT2_bp  Predefined. */


/* EVSYS.CH7CTRL  bit masks and bit positions */
/* EVSYS_DIGFILT_gm  Predefined. */
/* EVSYS_DIGFILT_gp  Predefined. */
/* EVSYS_DIGFILT0_bm  Predefined. */
/* EVSYS_DIGFILT0_bp  Predefined. */
/* EVSYS_DIGFILT1_bm  Predefined. */
/* EVSYS_DIGFILT1_bp  Predefined. */
/* EVSYS_DIGFILT2_bm  Predefined. */
/* EVSYS_DIGFILT2_bp  Predefined. */


/* NVM - Non Volatile Memory Controller */
/* NVM.CMD  bit masks and bit positions */
#define NVM_CMD_gm  0xFF  /* Command group mask. */
#define NVM_CMD_gp  0  /* Command group position. */
#define NVM_CMD0_bm  (1<<0)  /* Command bit 0 mask. */
#define NVM_CMD0_bp  0  /* Command bit 0 position. */
#define NVM_CMD1_bm  (1<<1)  /* Command bit 1 mask. */
#define NVM_CMD1_bp  1  /* Command bit 1 position. */
#define NVM_CMD2_bm  (1<<2)  /* Command bit 2 mask. */
#define NVM_CMD2_bp  2  /* Command bit 2 position. */
#define NVM_CMD3_bm  (1<<3)  /* Command bit 3 mask. */
#define NVM_CMD3_bp  3  /* Command bit 3 position. */
#define NVM_CMD4_bm  (1<<4)  /* Command bit 4 mask. */
#define NVM_CMD4_bp  4  /* Command bit 4 position. */
#define NVM_CMD5_bm  (1<<5)  /* Command bit 5 mask. */
#define NVM_CMD5_bp  5  /* Command bit 5 position. */
#define NVM_CMD6_bm  (1<<6)  /* Command bit 6 mask. */
#define NVM_CMD6_bp  6  /* Command bit 6 position. */
#define NVM_CMD7_bm  (1<<7)  /* Command bit 7 mask. */
#define NVM_CMD7_bp  7  /* Command bit 7 position. */


/* NVM.CTRLA  bit masks and bit positions */
#define NVM_CMDEX_bm  0x01  /* Command Execute bit mask. */
#define NVM_CMDEX_bp  0  /* Command Execute bit position. */


/* NVM.CTRLB  bit masks and bit positions */
#define NVM_EEMAPEN_bm  0x08  /* EEPROM Mapping Enable bit mask. */
#define NVM_EEMAPEN_bp  3  /* EEPROM Mapping Enable bit position. */

#define NVM_FPRM_bm  0x04  /* Flash Power Reduction Enable bit mask. */
#define NVM_FPRM_bp  2  /* Flash Power Reduction Enable bit position. */

#define NVM_EPRM_bm  0x02  /* EEPROM Power Reduction Enable bit mask. */
#define NVM_EPRM_bp  1  /* EEPROM Power Reduction Enable bit position. */

#define NVM_SPMLOCK_bm  0x01  /* SPM Lock bit mask. */
#define NVM_SPMLOCK_bp  0  /* SPM Lock bit position. */


/* NVM.INTCTRL  bit masks and bit positions */
#define NVM_SPMLVL_gm  0x0C  /* SPM Interrupt Level group mask. */
#define NVM_SPMLVL_gp  2  /* SPM Interrupt Level group position. */
#define NVM_SPMLVL0_bm  (1<<2)  /* SPM Interrupt Level bit 0 mask. */
#define NVM_SPMLVL0_bp  2  /* SPM Interrupt Level bit 0 position. */
#define NVM_SPMLVL1_bm  (1<<3)  /* SPM Interrupt Level bit 1 mask. */
#define NVM_SPMLVL1_bp  3  /* SPM Interrupt Level bit 1 position. */

#define NVM_EELVL_gm  0x03  /* EEPROM Interrupt Level group mask. */
#define NVM_EELVL_gp  0  /* EEPROM Interrupt Level group position. */
#define NVM_EELVL0_bm  (1<<0)  /* EEPROM Interrupt Level bit 0 mask. */
#define NVM_EELVL0_bp  0  /* EEPROM Interrupt Level bit 0 position. */
#define NVM_EELVL1_bm  (1<<1)  /* EEPROM Interrupt Level bit 1 mask. */
#define NVM_EELVL1_bp  1  /* EEPROM Interrupt Level bit 1 position. */


/* NVM.STATUS  bit masks and bit positions */
#define NVM_NVMBUSY_bm  0x80  /* Non-volatile Memory Busy bit mask. */
#define NVM_NVMBUSY_bp  7  /* Non-volatile Memory Busy bit position. */

#define NVM_FBUSY_bm  0x40  /* Flash Memory Busy bit mask. */
#define NVM_FBUSY_bp  6  /* Flash Memory Busy bit position. */

#define NVM_EELOAD_bm  0x02  /* EEPROM Page Buffer Active Loading bit mask. */
#define NVM_EELOAD_bp  1  /* EEPROM Page Buffer Active Loading bit position. */

#define NVM_FLOAD_bm  0x01  /* Flash Page Buffer Active Loading bit mask. */
#define NVM_FLOAD_bp  0  /* Flash Page Buffer Active Loading bit position. */


/* NVM.LOCKBITS  bit masks and bit positions */
#define NVM_BLBB_gm  0xC0  /* Boot Lock Bits - Boot Section group mask. */
#define NVM_BLBB_gp  6  /* Boot Lock Bits - Boot Section group position. */
#define NVM_BLBB0_bm  (1<<6)  /* Boot Lock Bits - Boot Section bit 0 mask. */
#define NVM_BLBB0_bp  6  /* Boot Lock Bits - Boot Section bit 0 position. */
#define NVM_BLBB1_bm  (1<<7)  /* Boot Lock Bits - Boot Section bit 1 mask. */
#define NVM_BLBB1_bp  7  /* Boot Lock Bits - Boot Section bit 1 position. */

#define NVM_BLBA_gm  0x30  /* Boot Lock Bits - Application Section group mask. */
#define NVM_BLBA_gp  4  /* Boot Lock Bits - Application Section group position. */
#define NVM_BLBA0_bm  (1<<4)  /* Boot Lock Bits - Application Section bit 0 mask. */
#define NVM_BLBA0_bp  4  /* Boot Lock Bits - Application Section bit 0 position. */
#define NVM_BLBA1_bm  (1<<5)  /* Boot Lock Bits - Application Section bit 1 mask. */
#define NVM_BLBA1_bp  5  /* Boot Lock Bits - Application Section bit 1 position. */

#define NVM_BLBAT_gm  0x0C  /* Boot Lock Bits - Application Table group mask. */
#define NVM_BLBAT_gp  2  /* Boot Lock Bits - Application Table group position. */
#define NVM_BLBAT0_bm  (1<<2)  /* Boot Lock Bits - Application Table bit 0 mask. */
#define NVM_BLBAT0_bp  2  /* Boot Lock Bits - Application Table bit 0 position. */
#define NVM_BLBAT1_bm  (1<<3)  /* Boot Lock Bits - Application Table bit 1 mask. */
#define NVM_BLBAT1_bp  3  /* Boot Lock Bits - Application Table bit 1 position. */

#define NVM_LB_gm  0x03  /* Lock Bits group mask. */
#define NVM_LB_gp  0  /* Lock Bits group position. */
#define NVM_LB0_bm  (1<<0)  /* Lock Bits bit 0 mask. */
#define NVM_LB0_bp  0  /* Lock Bits bit 0 position. */
#define NVM_LB1_bm  (1<<1)  /* Lock Bits bit 1 mask. */
#define NVM_LB1_bp  1  /* Lock Bits bit 1 position. */


/* NVM_LOCKBITS.LOCKBITS  bit masks and bit positions */
#define NVM_LOCKBITS_BLBB_gm  0xC0  /* Boot Lock Bits - Boot Section group mask. */
#define NVM_LOCKBITS_BLBB_gp  6  /* Boot Lock Bits - Boot Section group position. */
#define NVM_LOCKBITS_BLBB0_bm  (1<<6)  /* Boot Lock Bits - Boot Section bit 0 mask. */
#define NVM_LOCKBITS_BLBB0_bp  6  /* Boot Lock Bits - Boot Section bit 0 position. */
#define NVM_LOCKBITS_BLBB1_bm  (1<<7)  /* Boot Lock Bits - Boot Section bit 1 mask. */
#define NVM_LOCKBITS_BLBB1_bp  7  /* Boot Lock Bits - Boot Section bit 1 position. */

#define NVM_LOCKBITS_BLBA_gm  0x30  /* Boot Lock Bits - Application Section group mask. */
#define NVM_LOCKBITS_BLBA_gp  4  /* Boot Lock Bits - Application Section group position. */
#define NVM_LOCKBITS_BLBA0_bm  (1<<4)  /* Boot Lock Bits - Application Section bit 0 mask. */
#define NVM_LOCKBITS_BLBA0_bp  4  /* Boot Lock Bits - Application Section bit 0 position. */
#define NVM_LOCKBITS_BLBA1_bm  (1<<5)  /* Boot Lock Bits - Application Section bit 1 mask. */
#define NVM_LOCKBITS_BLBA1_bp  5  /* Boot Lock Bits - Application Section bit 1 position. */

#define NVM_LOCKBITS_BLBAT_gm  0x0C  /* Boot Lock Bits - Application Table group mask. */
#define NVM_LOCKBITS_BLBAT_gp  2  /* Boot Lock Bits - Application Table group position. */
#define NVM_LOCKBITS_BLBAT0_bm  (1<<2)  /* Boot Lock Bits - Application Table bit 0 mask. */
#define NVM_LOCKBITS_BLBAT0_bp  2  /* Boot Lock Bits - Application Table bit 0 position. */
#define NVM_LOCKBITS_BLBAT1_bm  (1<<3)  /* Boot Lock Bits - Application Table bit 1 mask. */
#define NVM_LOCKBITS_BLBAT1_bp  3  /* Boot Lock Bits - Application Table bit 1 position. */

#define NVM_LOCKBITS_LB_gm  0x03  /* Lock Bits group mask. */
#define NVM_LOCKBITS_LB_gp  0  /* Lock Bits group position. */
#define NVM_LOCKBITS_LB0_bm  (1<<0)  /* Lock Bits bit 0 mask. */
#define NVM_LOCKBITS_LB0_bp  0  /* Lock Bits bit 0 position. */
#define NVM_LOCKBITS_LB1_bm  (1<<1)  /* Lock Bits bit 1 mask. */
#define NVM_LOCKBITS_LB1_bp  1  /* Lock Bits bit 1 position. */


/* NVM_FUSES.FUSEBYTE0  bit masks and bit positions */
#define NVM_FUSES_JTAGUSERID_gm  0xFF  /* JTAG User ID group mask. */
#define NVM_FUSES_JTAGUSERID_gp  0  /* JTAG User ID group position. */
#define NVM_FUSES_JTAGUSERID0_bm  (1<<0)  /* JTAG User ID bit 0 mask. */
#define NVM_FUSES_JTAGUSERID0_bp  0  /* JTAG User ID bit 0 position. */
#define NVM_FUSES_JTAGUSERID1_bm  (1<<1)  /* JTAG User ID bit 1 mask. */
#define NVM_FUSES_JTAGUSERID1_bp  1  /* JTAG User ID bit 1 position. */
#define NVM_FUSES_JTAGUSERID2_bm  (1<<2)  /* JTAG User ID bit 2 mask. */
#define NVM_FUSES_JTAGUSERID2_bp  2  /* JTAG User ID bit 2 position. */
#define NVM_FUSES_JTAGUSERID3_bm  (1<<3)  /* JTAG User ID bit 3 mask. */
#define NVM_FUSES_JTAGUSERID3_bp  3  /* JTAG User ID bit 3 position. */
#define NVM_FUSES_JTAGUSERID4_bm  (1<<4)  /* JTAG User ID bit 4 mask. */
#define NVM_FUSES_JTAGUSERID4_bp  4  /* JTAG User ID bit 4 position. */
#define NVM_FUSES_JTAGUSERID5_bm  (1<<5)  /* JTAG User ID bit 5 mask. */
#define NVM_FUSES_JTAGUSERID5_bp  5  /* JTAG User ID bit 5 position. */
#define NVM_FUSES_JTAGUSERID6_bm  (1<<6)  /* JTAG User ID bit 6 mask. */
#define NVM_FUSES_JTAGUSERID6_bp  6  /* JTAG User ID bit 6 position. */
#define NVM_FUSES_JTAGUSERID7_bm  (1<<7)  /* JTAG User ID bit 7 mask. */
#define NVM_FUSES_JTAGUSERID7_bp  7  /* JTAG User ID bit 7 position. */


/* NVM_FUSES.FUSEBYTE1  bit masks and bit positions */
#define NVM_FUSES_WDWP_gm  0xF0  /* Watchdog Window Timeout Period group mask. */
#define NVM_FUSES_WDWP_gp  4  /* Watchdog Window Timeout Period group position. */
#define NVM_FUSES_WDWP0_bm  (1<<4)  /* Watchdog Window Timeout Period bit 0 mask. */
#define NVM_FUSES_WDWP0_bp  4  /* Watchdog Window Timeout Period bit 0 position. */
#define NVM_FUSES_WDWP1_bm  (1<<5)  /* Watchdog Window Timeout Period bit 1 mask. */
#define NVM_FUSES_WDWP1_bp  5  /* Watchdog Window Timeout Period bit 1 position. */
#define NVM_FUSES_WDWP2_bm  (1<<6)  /* Watchdog Window Timeout Period bit 2 mask. */
#define NVM_FUSES_WDWP2_bp  6  /* Watchdog Window Timeout Period bit 2 position. */
#define NVM_FUSES_WDWP3_bm  (1<<7)  /* Watchdog Window Timeout Period bit 3 mask. */
#define NVM_FUSES_WDWP3_bp  7  /* Watchdog Window Timeout Period bit 3 position. */

#define NVM_FUSES_WDP_gm  0x0F  /* Watchdog Timeout Period group mask. */
#define NVM_FUSES_WDP_gp  0  /* Watchdog Timeout Period group position. */
#define NVM_FUSES_WDP0_bm  (1<<0)  /* Watchdog Timeout Period bit 0 mask. */
#define NVM_FUSES_WDP0_bp  0  /* Watchdog Timeout Period bit 0 position. */
#define NVM_FUSES_WDP1_bm  (1<<1)  /* Watchdog Timeout Period bit 1 mask. */
#define NVM_FUSES_WDP1_bp  1  /* Watchdog Timeout Period bit 1 position. */
#define NVM_FUSES_WDP2_bm  (1<<2)  /* Watchdog Timeout Period bit 2 mask. */
#define NVM_FUSES_WDP2_bp  2  /* Watchdog Timeout Period bit 2 position. */
#define NVM_FUSES_WDP3_bm  (1<<3)  /* Watchdog Timeout Period bit 3 mask. */
#define NVM_FUSES_WDP3_bp  3  /* Watchdog Timeout Period bit 3 position. */


/* NVM_FUSES.FUSEBYTE2  bit masks and bit positions */
#define NVM_FUSES_DVSDON_bm  0x80  /* Spike Detector Enable bit mask. */
#define NVM_FUSES_DVSDON_bp  7  /* Spike Detector Enable bit position. */

#define NVM_FUSES_BOOTRST_bm  0x40  /* Boot Loader Section Reset Vector bit mask. */
#define NVM_FUSES_BOOTRST_bp  6  /* Boot Loader Section Reset Vector bit position. */

#define NVM_FUSES_BODACT_gm  0x0C  /* BOD Operation in Active Mode group mask. */
#define NVM_FUSES_BODACT_gp  2  /* BOD Operation in Active Mode group position. */
#define NVM_FUSES_BODACT0_bm  (1<<2)  /* BOD Operation in Active Mode bit 0 mask. */
#define NVM_FUSES_BODACT0_bp  2  /* BOD Operation in Active Mode bit 0 position. */
#define NVM_FUSES_BODACT1_bm  (1<<3)  /* BOD Operation in Active Mode bit 1 mask. */
#define NVM_FUSES_BODACT1_bp  3  /* BOD Operation in Active Mode bit 1 position. */

#define NVM_FUSES_BODPD_gm  0x03  /* BOD Operation in Power-Down Mode group mask. */
#define NVM_FUSES_BODPD_gp  0  /* BOD Operation in Power-Down Mode group position. */
#define NVM_FUSES_BODPD0_bm  (1<<0)  /* BOD Operation in Power-Down Mode bit 0 mask. */
#define NVM_FUSES_BODPD0_bp  0  /* BOD Operation in Power-Down Mode bit 0 position. */
#define NVM_FUSES_BODPD1_bm  (1<<1)  /* BOD Operation in Power-Down Mode bit 1 mask. */
#define NVM_FUSES_BODPD1_bp  1  /* BOD Operation in Power-Down Mode bit 1 position. */


/* NVM_FUSES.FUSEBYTE4  bit masks and bit positions */
#define NVM_FUSES_SUT_gm  0x0C  /* Start-up Time group mask. */
#define NVM_FUSES_SUT_gp  2  /* Start-up Time group position. */
#define NVM_FUSES_SUT0_bm  (1<<2)  /* Start-up Time bit 0 mask. */
#define NVM_FUSES_SUT0_bp  2  /* Start-up Time bit 0 position. */
#define NVM_FUSES_SUT1_bm  (1<<3)  /* Start-up Time bit 1 mask. */
#define NVM_FUSES_SUT1_bp  3  /* Start-up Time bit 1 position. */

#define NVM_FUSES_WDLOCK_bm  0x02  /* Watchdog Timer Lock bit mask. */
#define NVM_FUSES_WDLOCK_bp  1  /* Watchdog Timer Lock bit position. */

#define NVM_FUSES_JTAGEN_bm  0x01  /* JTAG Interface Enable bit mask. */
#define NVM_FUSES_JTAGEN_bp  0  /* JTAG Interface Enable bit position. */


/* NVM_FUSES.FUSEBYTE5  bit masks and bit positions */
#define NVM_FUSES_EESAVE_bm  0x08  /* Preserve EEPROM Through Chip Erase bit mask. */
#define NVM_FUSES_EESAVE_bp  3  /* Preserve EEPROM Through Chip Erase bit position. */

#define NVM_FUSES_BODLVL_gm  0x07  /* Brown Out Detection Voltage Level group mask. */
#define NVM_FUSES_BODLVL_gp  0  /* Brown Out Detection Voltage Level group position. */
#define NVM_FUSES_BODLVL0_bm  (1<<0)  /* Brown Out Detection Voltage Level bit 0 mask. */
#define NVM_FUSES_BODLVL0_bp  0  /* Brown Out Detection Voltage Level bit 0 position. */
#define NVM_FUSES_BODLVL1_bm  (1<<1)  /* Brown Out Detection Voltage Level bit 1 mask. */
#define NVM_FUSES_BODLVL1_bp  1  /* Brown Out Detection Voltage Level bit 1 position. */
#define NVM_FUSES_BODLVL2_bm  (1<<2)  /* Brown Out Detection Voltage Level bit 2 mask. */
#define NVM_FUSES_BODLVL2_bp  2  /* Brown Out Detection Voltage Level bit 2 position. */


/* AC - Analog Comparator */
/* AC.AC0CTRL  bit masks and bit positions */
#define AC_INTMODE_gm  0xC0  /* Interrupt Mode group mask. */
#define AC_INTMODE_gp  6  /* Interrupt Mode group position. */
#define AC_INTMODE0_bm  (1<<6)  /* Interrupt Mode bit 0 mask. */
#define AC_INTMODE0_bp  6  /* Interrupt Mode bit 0 position. */
#define AC_INTMODE1_bm  (1<<7)  /* Interrupt Mode bit 1 mask. */
#define AC_INTMODE1_bp  7  /* Interrupt Mode bit 1 position. */

#define AC_INTLVL_gm  0x30  /* Interrupt Level group mask. */
#define AC_INTLVL_gp  4  /* Interrupt Level group position. */
#define AC_INTLVL0_bm  (1<<4)  /* Interrupt Level bit 0 mask. */
#define AC_INTLVL0_bp  4  /* Interrupt Level bit 0 position. */
#define AC_INTLVL1_bm  (1<<5)  /* Interrupt Level bit 1 mask. */
#define AC_INTLVL1_bp  5  /* Interrupt Level bit 1 position. */

#define AC_HSMODE_bm  0x08  /* High-speed Mode bit mask. */
#define AC_HSMODE_bp  3  /* High-speed Mode bit position. */

#define AC_HYSMODE_gm  0x06  /* Hysteresis Mode group mask. */
#define AC_HYSMODE_gp  1  /* Hysteresis Mode group position. */
#define AC_HYSMODE0_bm  (1<<1)  /* Hysteresis Mode bit 0 mask. */
#define AC_HYSMODE0_bp  1  /* Hysteresis Mode bit 0 position. */
#define AC_HYSMODE1_bm  (1<<2)  /* Hysteresis Mode bit 1 mask. */
#define AC_HYSMODE1_bp  2  /* Hysteresis Mode bit 1 position. */

#define AC_ENABLE_bm  0x01  /* Enable bit mask. */
#define AC_ENABLE_bp  0  /* Enable bit position. */


/* AC.AC1CTRL  bit masks and bit positions */
/* AC_INTMODE_gm  Predefined. */
/* AC_INTMODE_gp  Predefined. */
/* AC_INTMODE0_bm  Predefined. */
/* AC_INTMODE0_bp  Predefined. */
/* AC_INTMODE1_bm  Predefined. */
/* AC_INTMODE1_bp  Predefined. */

/* AC_INTLVL_gm  Predefined. */
/* AC_INTLVL_gp  Predefined. */
/* AC_INTLVL0_bm  Predefined. */
/* AC_INTLVL0_bp  Predefined. */
/* AC_INTLVL1_bm  Predefined. */
/* AC_INTLVL1_bp  Predefined. */

/* AC_HSMODE_bm  Predefined. */
/* AC_HSMODE_bp  Predefined. */

/* AC_HYSMODE_gm  Predefined. */
/* AC_HYSMODE_gp  Predefined. */
/* AC_HYSMODE0_bm  Predefined. */
/* AC_HYSMODE0_bp  Predefined. */
/* AC_HYSMODE1_bm  Predefined. */
/* AC_HYSMODE1_bp  Predefined. */

/* AC_ENABLE_bm  Predefined. */
/* AC_ENABLE_bp  Predefined. */


/* AC.AC0MUXCTRL  bit masks and bit positions */
#define AC_MUXPOS_gm  0x38  /* MUX Positive Input group mask. */
#define AC_MUXPOS_gp  3  /* MUX Positive Input group position. */
#define AC_MUXPOS0_bm  (1<<3)  /* MUX Positive Input bit 0 mask. */
#define AC_MUXPOS0_bp  3  /* MUX Positive Input bit 0 position. */
#define AC_MUXPOS1_bm  (1<<4)  /* MUX Positive Input bit 1 mask. */
#define AC_MUXPOS1_bp  4  /* MUX Positive Input bit 1 position. */
#define AC_MUXPOS2_bm  (1<<5)  /* MUX Positive Input bit 2 mask. */
#define AC_MUXPOS2_bp  5  /* MUX Positive Input bit 2 position. */

#define AC_MUXNEG_gm  0x07  /* MUX Negative Input group mask. */
#define AC_MUXNEG_gp  0  /* MUX Negative Input group position. */
#define AC_MUXNEG0_bm  (1<<0)  /* MUX Negative Input bit 0 mask. */
#define AC_MUXNEG0_bp  0  /* MUX Negative Input bit 0 position. */
#define AC_MUXNEG1_bm  (1<<1)  /* MUX Negative Input bit 1 mask. */
#define AC_MUXNEG1_bp  1  /* MUX Negative Input bit 1 position. */
#define AC_MUXNEG2_bm  (1<<2)  /* MUX Negative Input bit 2 mask. */
#define AC_MUXNEG2_bp  2  /* MUX Negative Input bit 2 position. */


/* AC.AC1MUXCTRL  bit masks and bit positions */
/* AC_MUXPOS_gm  Predefined. */
/* AC_MUXPOS_gp  Predefined. */
/* AC_MUXPOS0_bm  Predefined. */
/* AC_MUXPOS0_bp  Predefined. */
/* AC_MUXPOS1_bm  Predefined. */
/* AC_MUXPOS1_bp  Predefined. */
/* AC_MUXPOS2_bm  Predefined. */
/* AC_MUXPOS2_bp  Predefined. */

/* AC_MUXNEG_gm  Predefined. */
/* AC_MUXNEG_gp  Predefined. */
/* AC_MUXNEG0_bm  Predefined. */
/* AC_MUXNEG0_bp  Predefined. */
/* AC_MUXNEG1_bm  Predefined. */
/* AC_MUXNEG1_bp  Predefined. */
/* AC_MUXNEG2_bm  Predefined. */
/* AC_MUXNEG2_bp  Predefined. */


/* AC.CTRLA  bit masks and bit positions */
#define AC_AC0OUT_bm  0x01  /* Comparator 0 Output Enable bit mask. */
#define AC_AC0OUT_bp  0  /* Comparator 0 Output Enable bit position. */


/* AC.CTRLB  bit masks and bit positions */
#define AC_SCALEFAC_gm  0x3F  /* VCC Voltage Scaler Factor group mask. */
#define AC_SCALEFAC_gp  0  /* VCC Voltage Scaler Factor group position. */
#define AC_SCALEFAC0_bm  (1<<0)  /* VCC Voltage Scaler Factor bit 0 mask. */
#define AC_SCALEFAC0_bp  0  /* VCC Voltage Scaler Factor bit 0 position. */
#define AC_SCALEFAC1_bm  (1<<1)  /* VCC Voltage Scaler Factor bit 1 mask. */
#define AC_SCALEFAC1_bp  1  /* VCC Voltage Scaler Factor bit 1 position. */
#define AC_SCALEFAC2_bm  (1<<2)  /* VCC Voltage Scaler Factor bit 2 mask. */
#define AC_SCALEFAC2_bp  2  /* VCC Voltage Scaler Factor bit 2 position. */
#define AC_SCALEFAC3_bm  (1<<3)  /* VCC Voltage Scaler Factor bit 3 mask. */
#define AC_SCALEFAC3_bp  3  /* VCC Voltage Scaler Factor bit 3 position. */
#define AC_SCALEFAC4_bm  (1<<4)  /* VCC Voltage Scaler Factor bit 4 mask. */
#define AC_SCALEFAC4_bp  4  /* VCC Voltage Scaler Factor bit 4 position. */
#define AC_SCALEFAC5_bm  (1<<5)  /* VCC Voltage Scaler Factor bit 5 mask. */
#define AC_SCALEFAC5_bp  5  /* VCC Voltage Scaler Factor bit 5 position. */


/* AC.WINCTRL  bit masks and bit positions */
#define AC_WEN_bm  0x10  /* Window Mode Enable bit mask. */
#define AC_WEN_bp  4  /* Window Mode Enable bit position. */

#define AC_WINTMODE_gm  0x0C  /* Window Interrupt Mode group mask. */
#define AC_WINTMODE_gp  2  /* Window Interrupt Mode group position. */
#define AC_WINTMODE0_bm  (1<<2)  /* Window Interrupt Mode bit 0 mask. */
#define AC_WINTMODE0_bp  2  /* Window Interrupt Mode bit 0 position. */
#define AC_WINTMODE1_bm  (1<<3)  /* Window Interrupt Mode bit 1 mask. */
#define AC_WINTMODE1_bp  3  /* Window Interrupt Mode bit 1 position. */

#define AC_WINTLVL_gm  0x03  /* Window Interrupt Level group mask. */
#define AC_WINTLVL_gp  0  /* Window Interrupt Level group position. */
#define AC_WINTLVL0_bm  (1<<0)  /* Window Interrupt Level bit 0 mask. */
#define AC_WINTLVL0_bp  0  /* Window Interrupt Level bit 0 position. */
#define AC_WINTLVL1_bm  (1<<1)  /* Window Interrupt Level bit 1 mask. */
#define AC_WINTLVL1_bp  1  /* Window Interrupt Level bit 1 position. */


/* AC.STATUS  bit masks and bit positions */
#define AC_WSTATE_gm  0xC0  /* Window Mode State group mask. */
#define AC_WSTATE_gp  6  /* Window Mode State group position. */
#define AC_WSTATE0_bm  (1<<6)  /* Window Mode State bit 0 mask. */
#define AC_WSTATE0_bp  6  /* Window Mode State bit 0 position. */
#define AC_WSTATE1_bm  (1<<7)  /* Window Mode State bit 1 mask. */
#define AC_WSTATE1_bp  7  /* Window Mode State bit 1 position. */

#define AC_AC1STATE_bm  0x20  /* Comparator 1 State bit mask. */
#define AC_AC1STATE_bp  5  /* Comparator 1 State bit position. */

#define AC_AC0STATE_bm  0x10  /* Comparator 0 State bit mask. */
#define AC_AC0STATE_bp  4  /* Comparator 0 State bit position. */

#define AC_WIF_bm  0x04  /* Window Mode Interrupt Flag bit mask. */
#define AC_WIF_bp  2  /* Window Mode Interrupt Flag bit position. */

#define AC_AC1IF_bm  0x02  /* Comparator 1 Interrupt Flag bit mask. */
#define AC_AC1IF_bp  1  /* Comparator 1 Interrupt Flag bit position. */

#define AC_AC0IF_bm  0x01  /* Comparator 0 Interrupt Flag bit mask. */
#define AC_AC0IF_bp  0  /* Comparator 0 Interrupt Flag bit position. */


/* ADC - Analog/Digital Converter */
/* ADC_CH.CTRL  bit masks and bit positions */
#define ADC_CH_START_bm  0x80  /* Channel Start Conversion bit mask. */
#define ADC_CH_START_bp  7  /* Channel Start Conversion bit position. */

#define ADC_CH_GAINFAC_gm  0x1C  /* Gain Factor group mask. */
#define ADC_CH_GAINFAC_gp  2  /* Gain Factor group position. */
#define ADC_CH_GAINFAC0_bm  (1<<2)  /* Gain Factor bit 0 mask. */
#define ADC_CH_GAINFAC0_bp  2  /* Gain Factor bit 0 position. */
#define ADC_CH_GAINFAC1_bm  (1<<3)  /* Gain Factor bit 1 mask. */
#define ADC_CH_GAINFAC1_bp  3  /* Gain Factor bit 1 position. */
#define ADC_CH_GAINFAC2_bm  (1<<4)  /* Gain Factor bit 2 mask. */
#define ADC_CH_GAINFAC2_bp  4  /* Gain Factor bit 2 position. */

#define ADC_CH_INPUTMODE_gm  0x03  /* Input Mode Select group mask. */
#define ADC_CH_INPUTMODE_gp  0  /* Input Mode Select group position. */
#define ADC_CH_INPUTMODE0_bm  (1<<0)  /* Input Mode Select bit 0 mask. */
#define ADC_CH_INPUTMODE0_bp  0  /* Input Mode Select bit 0 position. */
#define ADC_CH_INPUTMODE1_bm  (1<<1)  /* Input Mode Select bit 1 mask. */
#define ADC_CH_INPUTMODE1_bp  1  /* Input Mode Select bit 1 position. */


/* ADC_CH.MUXCTRL  bit masks and bit positions */
#define ADC_CH_MUXPOS_gm  0x78  /* Positive Input Select group mask. */
#define ADC_CH_MUXPOS_gp  3  /* Positive Input Select group position. */
#define ADC_CH_MUXPOS0_bm  (1<<3)  /* Positive Input Select bit 0 mask. */
#define ADC_CH_MUXPOS0_bp  3  /* Positive Input Select bit 0 position. */
#define ADC_CH_MUXPOS1_bm  (1<<4)  /* Positive Input Select bit 1 mask. */
#define ADC_CH_MUXPOS1_bp  4  /* Positive Input Select bit 1 position. */
#define ADC_CH_MUXPOS2_bm  (1<<5)  /* Positive Input Select bit 2 mask. */
#define ADC_CH_MUXPOS2_bp  5  /* Positive Input Select bit 2 position. */
#define ADC_CH_MUXPOS3_bm  (1<<6)  /* Positive Input Select bit 3 mask. */
#define ADC_CH_MUXPOS3_bp  6  /* Positive Input Select bit 3 position. */

#define ADC_CH_MUXINT_gm  0x78  /* Internal Input Select group mask. */
#define ADC_CH_MUXINT_gp  3  /* Internal Input Select group position. */
#define ADC_CH_MUXINT0_bm  (1<<3)  /* Internal Input Select bit 0 mask. */
#define ADC_CH_MUXINT0_bp  3  /* Internal Input Select bit 0 position. */
#define ADC_CH_MUXINT1_bm  (1<<4)  /* Internal Input Select bit 1 mask. */
#define ADC_CH_MUXINT1_bp  4  /* Internal Input Select bit 1 position. */
#define ADC_CH_MUXINT2_bm  (1<<5)  /* Internal Input Select bit 2 mask. */
#define ADC_CH_MUXINT2_bp  5  /* Internal Input Select bit 2 position. */
#define ADC_CH_MUXINT3_bm  (1<<6)  /* Internal Input Select bit 3 mask. */
#define ADC_CH_MUXINT3_bp  6  /* Internal Input Select bit 3 position. */

#define ADC_CH_MUXNEG_gm  0x03  /* Negative Input Select group mask. */
#define ADC_CH_MUXNEG_gp  0  /* Negative Input Select group position. */
#define ADC_CH_MUXNEG0_bm  (1<<0)  /* Negative Input Select bit 0 mask. */
#define ADC_CH_MUXNEG0_bp  0  /* Negative Input Select bit 0 position. */
#define ADC_CH_MUXNEG1_bm  (1<<1)  /* Negative Input Select bit 1 mask. */
#define ADC_CH_MUXNEG1_bp  1  /* Negative Input Select bit 1 position. */


/* ADC_CH.INTCTRL  bit masks and bit positions */
#define ADC_CH_INTMODE_gm  0x0C  /* Interrupt Mode group mask. */
#define ADC_CH_INTMODE_gp  2  /* Interrupt Mode group position. */
#define ADC_CH_INTMODE0_bm  (1<<2)  /* Interrupt Mode bit 0 mask. */
#define ADC_CH_INTMODE0_bp  2  /* Interrupt Mode bit 0 position. */
#define ADC_CH_INTMODE1_bm  (1<<3)  /* Interrupt Mode bit 1 mask. */
#define ADC_CH_INTMODE1_bp  3  /* Interrupt Mode bit 1 position. */

#define ADC_CH_INTLVL_gm  0x03  /* Interrupt Level group mask. */
#define ADC_CH_INTLVL_gp  0  /* Interrupt Level group position. */
#define ADC_CH_INTLVL0_bm  (1<<0)  /* Interrupt Level bit 0 mask. */
#define ADC_CH_INTLVL0_bp  0  /* Interrupt Level bit 0 position. */
#define ADC_CH_INTLVL1_bm  (1<<1)  /* Interrupt Level bit 1 mask. */
#define ADC_CH_INTLVL1_bp  1  /* Interrupt Level bit 1 position. */


/* ADC_CH.INTFLAGS  bit masks and bit positions */
#define ADC_CH_CHIF_bm  0x01  /* Channel Interrupt Flag bit mask. */
#define ADC_CH_CHIF_bp  0  /* Channel Interrupt Flag bit position. */


/* ADC.CTRLA  bit masks and bit positions */
#define ADC_DMASEL_gm  0xE0  /* DMA Selection group mask. */
#define ADC_DMASEL_gp  5  /* DMA Selection group position. */
#define ADC_DMASEL0_bm  (1<<5)  /* DMA Selection bit 0 mask. */
#define ADC_DMASEL0_bp  5  /* DMA Selection bit 0 position. */
#define ADC_DMASEL1_bm  (1<<6)  /* DMA Selection bit 1 mask. */
#define ADC_DMASEL1_bp  6  /* DMA Selection bit 1 position. */
#define ADC_DMASEL2_bm  (1<<7)  /* DMA Selection bit 2 mask. */
#define ADC_DMASEL2_bp  7  /* DMA Selection bit 2 position. */

#define ADC_CH3START_bm  0x20  /* Channel 3 Start Conversion bit mask. */
#define ADC_CH3START_bp  5  /* Channel 3 Start Conversion bit position. */

#define ADC_CH2START_bm  0x10  /* Channel 2 Start Conversion bit mask. */
#define ADC_CH2START_bp  4  /* Channel 2 Start Conversion bit position. */

#define ADC_CH1START_bm  0x08  /* Channel 1 Start Conversion bit mask. */
#define ADC_CH1START_bp  3  /* Channel 1 Start Conversion bit position. */

#define ADC_CH0START_bm  0x04  /* Channel 0 Start Conversion bit mask. */
#define ADC_CH0START_bp  2  /* Channel 0 Start Conversion bit position. */

#define ADC_FLUSH_bm  0x02  /* Flush Pipeline bit mask. */
#define ADC_FLUSH_bp  1  /* Flush Pipeline bit position. */

#define ADC_ENABLE_bm  0x01  /* Enable ADC bit mask. */
#define ADC_ENABLE_bp  0  /* Enable ADC bit position. */


/* ADC.CTRLB  bit masks and bit positions */
#define ADC_CONMODE_bm  0x10  /* Conversion Mode bit mask. */
#define ADC_CONMODE_bp  4  /* Conversion Mode bit position. */

#define ADC_FREERUN_bm  0x08  /* Free Running Mode Enable bit mask. */
#define ADC_FREERUN_bp  3  /* Free Running Mode Enable bit position. */

#define ADC_RESOLUTION_gm  0x06  /* Result Resolution group mask. */
#define ADC_RESOLUTION_gp  1  /* Result Resolution group position. */
#define ADC_RESOLUTION0_bm  (1<<1)  /* Result Resolution bit 0 mask. */
#define ADC_RESOLUTION0_bp  1  /* Result Resolution bit 0 position. */
#define ADC_RESOLUTION1_bm  (1<<2)  /* Result Resolution bit 1 mask. */
#define ADC_RESOLUTION1_bp  2  /* Result Resolution bit 1 position. */


/* ADC.REFCTRL  bit masks and bit positions */
#define ADC_REFSEL_gm  0x30  /* Reference Selection group mask. */
#define ADC_REFSEL_gp  4  /* Reference Selection group position. */
#define ADC_REFSEL0_bm  (1<<4)  /* Reference Selection bit 0 mask. */
#define ADC_REFSEL0_bp  4  /* Reference Selection bit 0 position. */
#define ADC_REFSEL1_bm  (1<<5)  /* Reference Selection bit 1 mask. */
#define ADC_REFSEL1_bp  5  /* Reference Selection bit 1 position. */

#define ADC_BANDGAP_bm  0x02  /* Bandgap enable bit mask. */
#define ADC_BANDGAP_bp  1  /* Bandgap enable bit position. */

#define ADC_TEMPREF_bm  0x01  /* Temperature Reference Enable bit mask. */
#define ADC_TEMPREF_bp  0  /* Temperature Reference Enable bit position. */


/* ADC.EVCTRL  bit masks and bit positions */
#define ADC_SWEEP_gm  0xC0  /* Channel Sweep Selection group mask. */
#define ADC_SWEEP_gp  6  /* Channel Sweep Selection group position. */
#define ADC_SWEEP0_bm  (1<<6)  /* Channel Sweep Selection bit 0 mask. */
#define ADC_SWEEP0_bp  6  /* Channel Sweep Selection bit 0 position. */
#define ADC_SWEEP1_bm  (1<<7)  /* Channel Sweep Selection bit 1 mask. */
#define ADC_SWEEP1_bp  7  /* Channel Sweep Selection bit 1 position. */

#define ADC_EVSEL_gm  0x38  /* Event Input Select group mask. */
#define ADC_EVSEL_gp  3  /* Event Input Select group position. */
#define ADC_EVSEL0_bm  (1<<3)  /* Event Input Select bit 0 mask. */
#define ADC_EVSEL0_bp  3  /* Event Input Select bit 0 position. */
#define ADC_EVSEL1_bm  (1<<4)  /* Event Input Select bit 1 mask. */
#define ADC_EVSEL1_bp  4  /* Event Input Select bit 1 position. */
#define ADC_EVSEL2_bm  (1<<5)  /* Event Input Select bit 2 mask. */
#define ADC_EVSEL2_bp  5  /* Event Input Select bit 2 position. */

#define ADC_EVACT_gm  0x07  /* Event Action Select group mask. */
#define ADC_EVACT_gp  0  /* Event Action Select group position. */
#define ADC_EVACT0_bm  (1<<0)  /* Event Action Select bit 0 mask. */
#define ADC_EVACT0_bp  0  /* Event Action Select bit 0 position. */
#define ADC_EVACT1_bm  (1<<1)  /* Event Action Select bit 1 mask. */
#define ADC_EVACT1_bp  1  /* Event Action Select bit 1 position. */
#define ADC_EVACT2_bm  (1<<2)  /* Event Action Select bit 2 mask. */
#define ADC_EVACT2_bp  2  /* Event Action Select bit 2 position. */


/* ADC.PRESCALER  bit masks and bit positions */
#define ADC_PRESCALER_gm  0x07  /* Clock Prescaler Selection group mask. */
#define ADC_PRESCALER_gp  0  /* Clock Prescaler Selection group position. */
#define ADC_PRESCALER0_bm  (1<<0)  /* Clock Prescaler Selection bit 0 mask. */
#define ADC_PRESCALER0_bp  0  /* Clock Prescaler Selection bit 0 position. */
#define ADC_PRESCALER1_bm  (1<<1)  /* Clock Prescaler Selection bit 1 mask. */
#define ADC_PRESCALER1_bp  1  /* Clock Prescaler Selection bit 1 position. */
#define ADC_PRESCALER2_bm  (1<<2)  /* Clock Prescaler Selection bit 2 mask. */
#define ADC_PRESCALER2_bp  2  /* Clock Prescaler Selection bit 2 position. */


/* ADC.CALCTRL  bit masks and bit positions */
#define ADC_CAL_bm  0x01  /* ADC Calibration Start bit mask. */
#define ADC_CAL_bp  0  /* ADC Calibration Start bit position. */


/* ADC.INTFLAGS  bit masks and bit positions */
#define ADC_CH3IF_bm  0x08  /* Channel 3 Interrupt Flag bit mask. */
#define ADC_CH3IF_bp  3  /* Channel 3 Interrupt Flag bit position. */

#define ADC_CH2IF_bm  0x04  /* Channel 2 Interrupt Flag bit mask. */
#define ADC_CH2IF_bp  2  /* Channel 2 Interrupt Flag bit position. */

#define ADC_CH1IF_bm  0x02  /* Channel 1 Interrupt Flag bit mask. */
#define ADC_CH1IF_bp  1  /* Channel 1 Interrupt Flag bit position. */

#define ADC_CH0IF_bm  0x01  /* Channel 0 Interrupt Flag bit mask. */
#define ADC_CH0IF_bp  0  /* Channel 0 Interrupt Flag bit position. */


/* DAC - Digital/Analog Converter */
/* DAC.CTRLA  bit masks and bit positions */
#define DAC_IDOEN_bm  0x10  /* Internal Output Enable bit mask. */
#define DAC_IDOEN_bp  4  /* Internal Output Enable bit position. */

#define DAC_CH1EN_bm  0x08  /* Channel 1 Output Enable bit mask. */
#define DAC_CH1EN_bp  3  /* Channel 1 Output Enable bit position. */

#define DAC_CH0EN_bm  0x04  /* Channel 0 Output Enable bit mask. */
#define DAC_CH0EN_bp  2  /* Channel 0 Output Enable bit position. */

#define DAC_LPMODE_bm  0x02  /* Low Power Mode bit mask. */
#define DAC_LPMODE_bp  1  /* Low Power Mode bit position. */

#define DAC_ENABLE_bm  0x01  /* Enable bit mask. */
#define DAC_ENABLE_bp  0  /* Enable bit position. */


/* DAC.CTRLB  bit masks and bit positions */
#define DAC_CHSEL_gm  0x60  /* Channel Select group mask. */
#define DAC_CHSEL_gp  5  /* Channel Select group position. */
#define DAC_CHSEL0_bm  (1<<5)  /* Channel Select bit 0 mask. */
#define DAC_CHSEL0_bp  5  /* Channel Select bit 0 position. */
#define DAC_CHSEL1_bm  (1<<6)  /* Channel Select bit 1 mask. */
#define DAC_CHSEL1_bp  6  /* Channel Select bit 1 position. */

#define DAC_CH1TRIG_bm  0x02  /* Channel 1 Event Trig Enable bit mask. */
#define DAC_CH1TRIG_bp  1  /* Channel 1 Event Trig Enable bit position. */

#define DAC_CH0TRIG_bm  0x01  /* Channel 0 Event Trig Enable bit mask. */
#define DAC_CH0TRIG_bp  0  /* Channel 0 Event Trig Enable bit position. */


/* DAC.CTRLC  bit masks and bit positions */
#define DAC_REFSEL_gm  0x18  /* Reference Select group mask. */
#define DAC_REFSEL_gp  3  /* Reference Select group position. */
#define DAC_REFSEL0_bm  (1<<3)  /* Reference Select bit 0 mask. */
#define DAC_REFSEL0_bp  3  /* Reference Select bit 0 position. */
#define DAC_REFSEL1_bm  (1<<4)  /* Reference Select bit 1 mask. */
#define DAC_REFSEL1_bp  4  /* Reference Select bit 1 position. */

#define DAC_LEFTADJ_bm  0x01  /* Left-adjust Result bit mask. */
#define DAC_LEFTADJ_bp  0  /* Left-adjust Result bit position. */


/* DAC.EVCTRL  bit masks and bit positions */
#define DAC_EVSEL_gm  0x07  /* Event Input Selection group mask. */
#define DAC_EVSEL_gp  0  /* Event Input Selection group position. */
#define DAC_EVSEL0_bm  (1<<0)  /* Event Input Selection bit 0 mask. */
#define DAC_EVSEL0_bp  0  /* Event Input Selection bit 0 position. */
#define DAC_EVSEL1_bm  (1<<1)  /* Event Input Selection bit 1 mask. */
#define DAC_EVSEL1_bp  1  /* Event Input Selection bit 1 position. */
#define DAC_EVSEL2_bm  (1<<2)  /* Event Input Selection bit 2 mask. */
#define DAC_EVSEL2_bp  2  /* Event Input Selection bit 2 position. */


/* DAC.TIMCTRL  bit masks and bit positions */
#define DAC_CONINTVAL_gm  0x70  /* Conversion Intercal group mask. */
#define DAC_CONINTVAL_gp  4  /* Conversion Intercal group position. */
#define DAC_CONINTVAL0_bm  (1<<4)  /* Conversion Intercal bit 0 mask. */
#define DAC_CONINTVAL0_bp  4  /* Conversion Intercal bit 0 position. */
#define DAC_CONINTVAL1_bm  (1<<5)  /* Conversion Intercal bit 1 mask. */
#define DAC_CONINTVAL1_bp  5  /* Conversion Intercal bit 1 position. */
#define DAC_CONINTVAL2_bm  (1<<6)  /* Conversion Intercal bit 2 mask. */
#define DAC_CONINTVAL2_bp  6  /* Conversion Intercal bit 2 position. */

#define DAC_REFRESH_gm  0x0F  /* Refresh Timing Control group mask. */
#define DAC_REFRESH_gp  0  /* Refresh Timing Control group position. */
#define DAC_REFRESH0_bm  (1<<0)  /* Refresh Timing Control bit 0 mask. */
#define DAC_REFRESH0_bp  0  /* Refresh Timing Control bit 0 position. */
#define DAC_REFRESH1_bm  (1<<1)  /* Refresh Timing Control bit 1 mask. */
#define DAC_REFRESH1_bp  1  /* Refresh Timing Control bit 1 position. */
#define DAC_REFRESH2_bm  (1<<2)  /* Refresh Timing Control bit 2 mask. */
#define DAC_REFRESH2_bp  2  /* Refresh Timing Control bit 2 position. */
#define DAC_REFRESH3_bm  (1<<3)  /* Refresh Timing Control bit 3 mask. */
#define DAC_REFRESH3_bp  3  /* Refresh Timing Control bit 3 position. */


/* DAC.STATUS  bit masks and bit positions */
#define DAC_CH1DRE_bm  0x02  /* Channel 1 Data Register Empty bit mask. */
#define DAC_CH1DRE_bp  1  /* Channel 1 Data Register Empty bit position. */

#define DAC_CH0DRE_bm  0x01  /* Channel 0 Data Register Empty bit mask. */
#define DAC_CH0DRE_bp  0  /* Channel 0 Data Register Empty bit position. */


/* RTC - Real-Time Clounter */
/* RTC.CTRL  bit masks and bit positions */
#define RTC_PRESCALER_gm  0x07  /* Prescaling Factor group mask. */
#define RTC_PRESCALER_gp  0  /* Prescaling Factor group position. */
#define RTC_PRESCALER0_bm  (1<<0)  /* Prescaling Factor bit 0 mask. */
#define RTC_PRESCALER0_bp  0  /* Prescaling Factor bit 0 position. */
#define RTC_PRESCALER1_bm  (1<<1)  /* Prescaling Factor bit 1 mask. */
#define RTC_PRESCALER1_bp  1  /* Prescaling Factor bit 1 position. */
#define RTC_PRESCALER2_bm  (1<<2)  /* Prescaling Factor bit 2 mask. */
#define RTC_PRESCALER2_bp  2  /* Prescaling Factor bit 2 position. */


/* RTC.STATUS  bit masks and bit positions */
#define RTC_SYNCBUSY_bm  0x01  /* Synchronization Busy Flag bit mask. */
#define RTC_SYNCBUSY_bp  0  /* Synchronization Busy Flag bit position. */


/* RTC.INTCTRL  bit masks and bit positions */
#define RTC_COMPINTLVL_gm  0x0C  /* Compare Match Interrupt Level group mask. */
#define RTC_COMPINTLVL_gp  2  /* Compare Match Interrupt Level group position. */
#define RTC_COMPINTLVL0_bm  (1<<2)  /* Compare Match Interrupt Level bit 0 mask. */
#define RTC_COMPINTLVL0_bp  2  /* Compare Match Interrupt Level bit 0 position. */
#define RTC_COMPINTLVL1_bm  (1<<3)  /* Compare Match Interrupt Level bit 1 mask. */
#define RTC_COMPINTLVL1_bp  3  /* Compare Match Interrupt Level bit 1 position. */

#define RTC_OVFINTLVL_gm  0x03  /* Overflow Interrupt Level group mask. */
#define RTC_OVFINTLVL_gp  0  /* Overflow Interrupt Level group position. */
#define RTC_OVFINTLVL0_bm  (1<<0)  /* Overflow Interrupt Level bit 0 mask. */
#define RTC_OVFINTLVL0_bp  0  /* Overflow Interrupt Level bit 0 position. */
#define RTC_OVFINTLVL1_bm  (1<<1)  /* Overflow Interrupt Level bit 1 mask. */
#define RTC_OVFINTLVL1_bp  1  /* Overflow Interrupt Level bit 1 position. */


/* RTC.INTFLAGS  bit masks and bit positions */
#define RTC_COMPIF_bm  0x02  /* Compare Match Interrupt Flag bit mask. */
#define RTC_COMPIF_bp  1  /* Compare Match Interrupt Flag bit position. */

#define RTC_OVFIF_bm  0x01  /* Overflow Interrupt Flag bit mask. */
#define RTC_OVFIF_bp  0  /* Overflow Interrupt Flag bit position. */


/* EBI - External Bus Interface */
/* EBI_CS.CTRLA  bit masks and bit positions */
#define EBI_CS_ASPACE_gm  0x7C  /* Address Space group mask. */
#define EBI_CS_ASPACE_gp  2  /* Address Space group position. */
#define EBI_CS_ASPACE0_bm  (1<<2)  /* Address Space bit 0 mask. */
#define EBI_CS_ASPACE0_bp  2  /* Address Space bit 0 position. */
#define EBI_CS_ASPACE1_bm  (1<<3)  /* Address Space bit 1 mask. */
#define EBI_CS_ASPACE1_bp  3  /* Address Space bit 1 position. */
#define EBI_CS_ASPACE2_bm  (1<<4)  /* Address Space bit 2 mask. */
#define EBI_CS_ASPACE2_bp  4  /* Address Space bit 2 position. */
#define EBI_CS_ASPACE3_bm  (1<<5)  /* Address Space bit 3 mask. */
#define EBI_CS_ASPACE3_bp  5  /* Address Space bit 3 position. */
#define EBI_CS_ASPACE4_bm  (1<<6)  /* Address Space bit 4 mask. */
#define EBI_CS_ASPACE4_bp  6  /* Address Space bit 4 position. */

#define EBI_CS_MODE_gm  0x03  /* Memory Mode group mask. */
#define EBI_CS_MODE_gp  0  /* Memory Mode group position. */
#define EBI_CS_MODE0_bm  (1<<0)  /* Memory Mode bit 0 mask. */
#define EBI_CS_MODE0_bp  0  /* Memory Mode bit 0 position. */
#define EBI_CS_MODE1_bm  (1<<1)  /* Memory Mode bit 1 mask. */
#define EBI_CS_MODE1_bp  1  /* Memory Mode bit 1 position. */


/* EBI_CS.CTRLB  bit masks and bit positions */
#define EBI_CS_SRWS_gm  0x07  /* SRAM Wait State Cycles group mask. */
#define EBI_CS_SRWS_gp  0  /* SRAM Wait State Cycles group position. */
#define EBI_CS_SRWS0_bm  (1<<0)  /* SRAM Wait State Cycles bit 0 mask. */
#define EBI_CS_SRWS0_bp  0  /* SRAM Wait State Cycles bit 0 position. */
#define EBI_CS_SRWS1_bm  (1<<1)  /* SRAM Wait State Cycles bit 1 mask. */
#define EBI_CS_SRWS1_bp  1  /* SRAM Wait State Cycles bit 1 position. */
#define EBI_CS_SRWS2_bm  (1<<2)  /* SRAM Wait State Cycles bit 2 mask. */
#define EBI_CS_SRWS2_bp  2  /* SRAM Wait State Cycles bit 2 position. */

#define EBI_CS_SDINITDONE_bm  0x80  /* SDRAM Initialization Done bit mask. */
#define EBI_CS_SDINITDONE_bp  7  /* SDRAM Initialization Done bit position. */

#define EBI_CS_SDSREN_bm  0x04  /* SDRAM Self-refresh Enable bit mask. */
#define EBI_CS_SDSREN_bp  2  /* SDRAM Self-refresh Enable bit position. */

#define EBI_CS_SDMODE_gm  0x03  /* SDRAM Mode group mask. */
#define EBI_CS_SDMODE_gp  0  /* SDRAM Mode group position. */
#define EBI_CS_SDMODE0_bm  (1<<0)  /* SDRAM Mode bit 0 mask. */
#define EBI_CS_SDMODE0_bp  0  /* SDRAM Mode bit 0 position. */
#define EBI_CS_SDMODE1_bm  (1<<1)  /* SDRAM Mode bit 1 mask. */
#define EBI_CS_SDMODE1_bp  1  /* SDRAM Mode bit 1 position. */


/* EBI.CTRL  bit masks and bit positions */
#define EBI_SDDATAW_gm  0xC0  /* SDRAM Data Width Setting group mask. */
#define EBI_SDDATAW_gp  6  /* SDRAM Data Width Setting group position. */
#define EBI_SDDATAW0_bm  (1<<6)  /* SDRAM Data Width Setting bit 0 mask. */
#define EBI_SDDATAW0_bp  6  /* SDRAM Data Width Setting bit 0 position. */
#define EBI_SDDATAW1_bm  (1<<7)  /* SDRAM Data Width Setting bit 1 mask. */
#define EBI_SDDATAW1_bp  7  /* SDRAM Data Width Setting bit 1 position. */

#define EBI_LPCMODE_gm  0x30  /* SRAM LPC Mode group mask. */
#define EBI_LPCMODE_gp  4  /* SRAM LPC Mode group position. */
#define EBI_LPCMODE0_bm  (1<<4)  /* SRAM LPC Mode bit 0 mask. */
#define EBI_LPCMODE0_bp  4  /* SRAM LPC Mode bit 0 position. */
#define EBI_LPCMODE1_bm  (1<<5)  /* SRAM LPC Mode bit 1 mask. */
#define EBI_LPCMODE1_bp  5  /* SRAM LPC Mode bit 1 position. */

#define EBI_SRMODE_gm  0x0C  /* SRAM Mode group mask. */
#define EBI_SRMODE_gp  2  /* SRAM Mode group position. */
#define EBI_SRMODE0_bm  (1<<2)  /* SRAM Mode bit 0 mask. */
#define EBI_SRMODE0_bp  2  /* SRAM Mode bit 0 position. */
#define EBI_SRMODE1_bm  (1<<3)  /* SRAM Mode bit 1 mask. */
#define EBI_SRMODE1_bp  3  /* SRAM Mode bit 1 position. */

#define EBI_IFMODE_gm  0x03  /* Interface Mode group mask. */
#define EBI_IFMODE_gp  0  /* Interface Mode group position. */
#define EBI_IFMODE0_bm  (1<<0)  /* Interface Mode bit 0 mask. */
#define EBI_IFMODE0_bp  0  /* Interface Mode bit 0 position. */
#define EBI_IFMODE1_bm  (1<<1)  /* Interface Mode bit 1 mask. */
#define EBI_IFMODE1_bp  1  /* Interface Mode bit 1 position. */


/* EBI.SDRAMCTRLA  bit masks and bit positions */
#define EBI_SDCAS_bm  0x08  /* SDRAM CAS Latency Setting bit mask. */
#define EBI_SDCAS_bp  3  /* SDRAM CAS Latency Setting bit position. */

#define EBI_SDROW_bm  0x04  /* SDRAM ROW Bits Setting bit mask. */
#define EBI_SDROW_bp  2  /* SDRAM ROW Bits Setting bit position. */

#define EBI_SDCOL_gm  0x03  /* SDRAM Column Bits Setting group mask. */
#define EBI_SDCOL_gp  0  /* SDRAM Column Bits Setting group position. */
#define EBI_SDCOL0_bm  (1<<0)  /* SDRAM Column Bits Setting bit 0 mask. */
#define EBI_SDCOL0_bp  0  /* SDRAM Column Bits Setting bit 0 position. */
#define EBI_SDCOL1_bm  (1<<1)  /* SDRAM Column Bits Setting bit 1 mask. */
#define EBI_SDCOL1_bp  1  /* SDRAM Column Bits Setting bit 1 position. */


/* EBI.SDRAMCTRLB  bit masks and bit positions */
#define EBI_MRDLY_gm  0xC0  /* SDRAM Mode Register Delay group mask. */
#define EBI_MRDLY_gp  6  /* SDRAM Mode Register Delay group position. */
#define EBI_MRDLY0_bm  (1<<6)  /* SDRAM Mode Register Delay bit 0 mask. */
#define EBI_MRDLY0_bp  6  /* SDRAM Mode Register Delay bit 0 position. */
#define EBI_MRDLY1_bm  (1<<7)  /* SDRAM Mode Register Delay bit 1 mask. */
#define EBI_MRDLY1_bp  7  /* SDRAM Mode Register Delay bit 1 position. */

#define EBI_ROWCYCDLY_gm  0x38  /* SDRAM Row Cycle Delay group mask. */
#define EBI_ROWCYCDLY_gp  3  /* SDRAM Row Cycle Delay group position. */
#define EBI_ROWCYCDLY0_bm  (1<<3)  /* SDRAM Row Cycle Delay bit 0 mask. */
#define EBI_ROWCYCDLY0_bp  3  /* SDRAM Row Cycle Delay bit 0 position. */
#define EBI_ROWCYCDLY1_bm  (1<<4)  /* SDRAM Row Cycle Delay bit 1 mask. */
#define EBI_ROWCYCDLY1_bp  4  /* SDRAM Row Cycle Delay bit 1 position. */
#define EBI_ROWCYCDLY2_bm  (1<<5)  /* SDRAM Row Cycle Delay bit 2 mask. */
#define EBI_ROWCYCDLY2_bp  5  /* SDRAM Row Cycle Delay bit 2 position. */

#define EBI_RPDLY_gm  0x07  /* SDRAM Row-to-Precharge Delay group mask. */
#define EBI_RPDLY_gp  0  /* SDRAM Row-to-Precharge Delay group position. */
#define EBI_RPDLY0_bm  (1<<0)  /* SDRAM Row-to-Precharge Delay bit 0 mask. */
#define EBI_RPDLY0_bp  0  /* SDRAM Row-to-Precharge Delay bit 0 position. */
#define EBI_RPDLY1_bm  (1<<1)  /* SDRAM Row-to-Precharge Delay bit 1 mask. */
#define EBI_RPDLY1_bp  1  /* SDRAM Row-to-Precharge Delay bit 1 position. */
#define EBI_RPDLY2_bm  (1<<2)  /* SDRAM Row-to-Precharge Delay bit 2 mask. */
#define EBI_RPDLY2_bp  2  /* SDRAM Row-to-Precharge Delay bit 2 position. */


/* EBI.SDRAMCTRLC  bit masks and bit positions */
#define EBI_WRDLY_gm  0xC0  /* SDRAM Write Recovery Delay group mask. */
#define EBI_WRDLY_gp  6  /* SDRAM Write Recovery Delay group position. */
#define EBI_WRDLY0_bm  (1<<6)  /* SDRAM Write Recovery Delay bit 0 mask. */
#define EBI_WRDLY0_bp  6  /* SDRAM Write Recovery Delay bit 0 position. */
#define EBI_WRDLY1_bm  (1<<7)  /* SDRAM Write Recovery Delay bit 1 mask. */
#define EBI_WRDLY1_bp  7  /* SDRAM Write Recovery Delay bit 1 position. */

#define EBI_ESRDLY_gm  0x38  /* SDRAM Exit-Self-refresh-to-Active Delay group mask. */
#define EBI_ESRDLY_gp  3  /* SDRAM Exit-Self-refresh-to-Active Delay group position. */
#define EBI_ESRDLY0_bm  (1<<3)  /* SDRAM Exit-Self-refresh-to-Active Delay bit 0 mask. */
#define EBI_ESRDLY0_bp  3  /* SDRAM Exit-Self-refresh-to-Active Delay bit 0 position. */
#define EBI_ESRDLY1_bm  (1<<4)  /* SDRAM Exit-Self-refresh-to-Active Delay bit 1 mask. */
#define EBI_ESRDLY1_bp  4  /* SDRAM Exit-Self-refresh-to-Active Delay bit 1 position. */
#define EBI_ESRDLY2_bm  (1<<5)  /* SDRAM Exit-Self-refresh-to-Active Delay bit 2 mask. */
#define EBI_ESRDLY2_bp  5  /* SDRAM Exit-Self-refresh-to-Active Delay bit 2 position. */

#define EBI_ROWCOLDLY_gm  0x07  /* SDRAM Row-to-Column Delay group mask. */
#define EBI_ROWCOLDLY_gp  0  /* SDRAM Row-to-Column Delay group position. */
#define EBI_ROWCOLDLY0_bm  (1<<0)  /* SDRAM Row-to-Column Delay bit 0 mask. */
#define EBI_ROWCOLDLY0_bp  0  /* SDRAM Row-to-Column Delay bit 0 position. */
#define EBI_ROWCOLDLY1_bm  (1<<1)  /* SDRAM Row-to-Column Delay bit 1 mask. */
#define EBI_ROWCOLDLY1_bp  1  /* SDRAM Row-to-Column Delay bit 1 position. */
#define EBI_ROWCOLDLY2_bm  (1<<2)  /* SDRAM Row-to-Column Delay bit 2 mask. */
#define EBI_ROWCOLDLY2_bp  2  /* SDRAM Row-to-Column Delay bit 2 position. */


/* TWI - Two-Wire Interface */
/* TWI_MASTER.CTRLA  bit masks and bit positions */
#define TWI_MASTER_INTLVL_gm  0xC0  /* Interrupt Level group mask. */
#define TWI_MASTER_INTLVL_gp  6  /* Interrupt Level group position. */
#define TWI_MASTER_INTLVL0_bm  (1<<6)  /* Interrupt Level bit 0 mask. */
#define TWI_MASTER_INTLVL0_bp  6  /* Interrupt Level bit 0 position. */
#define TWI_MASTER_INTLVL1_bm  (1<<7)  /* Interrupt Level bit 1 mask. */
#define TWI_MASTER_INTLVL1_bp  7  /* Interrupt Level bit 1 position. */

#define TWI_MASTER_RIEN_bm  0x20  /* Read Interrupt Enable bit mask. */
#define TWI_MASTER_RIEN_bp  5  /* Read Interrupt Enable bit position. */

#define TWI_MASTER_WIEN_bm  0x10  /* Write Interrupt Enable bit mask. */
#define TWI_MASTER_WIEN_bp  4  /* Write Interrupt Enable bit position. */

#define TWI_MASTER_ENABLE_bm  0x08  /* Enable TWI Master bit mask. */
#define TWI_MASTER_ENABLE_bp  3  /* Enable TWI Master bit position. */


/* TWI_MASTER.CTRLB  bit masks and bit positions */
#define TWI_MASTER_TIMEOUT_gm  0x0C  /* Inactive Bus Timeout group mask. */
#define TWI_MASTER_TIMEOUT_gp  2  /* Inactive Bus Timeout group position. */
#define TWI_MASTER_TIMEOUT0_bm  (1<<2)  /* Inactive Bus Timeout bit 0 mask. */
#define TWI_MASTER_TIMEOUT0_bp  2  /* Inactive Bus Timeout bit 0 position. */
#define TWI_MASTER_TIMEOUT1_bm  (1<<3)  /* Inactive Bus Timeout bit 1 mask. */
#define TWI_MASTER_TIMEOUT1_bp  3  /* Inactive Bus Timeout bit 1 position. */

#define TWI_MASTER_QCEN_bm  0x02  /* Quick Command Enable bit mask. */
#define TWI_MASTER_QCEN_bp  1  /* Quick Command Enable bit position. */

#define TWI_MASTER_SMEN_bm  0x01  /* Smart Mode Enable bit mask. */
#define TWI_MASTER_SMEN_bp  0  /* Smart Mode Enable bit position. */


/* TWI_MASTER.CTRLC  bit masks and bit positions */
#define TWI_MASTER_ACKACT_bm  0x04  /* Acknowledge Action bit mask. */
#define TWI_MASTER_ACKACT_bp  2  /* Acknowledge Action bit position. */

#define TWI_MASTER_CMD_gm  0x03  /* Command group mask. */
#define TWI_MASTER_CMD_gp  0  /* Command group position. */
#define TWI_MASTER_CMD0_bm  (1<<0)  /* Command bit 0 mask. */
#define TWI_MASTER_CMD0_bp  0  /* Command bit 0 position. */
#define TWI_MASTER_CMD1_bm  (1<<1)  /* Command bit 1 mask. */
#define TWI_MASTER_CMD1_bp  1  /* Command bit 1 position. */


/* TWI_MASTER.STATUS  bit masks and bit positions */
#define TWI_MASTER_RIF_bm  0x80  /* Read Interrupt Flag bit mask. */
#define TWI_MASTER_RIF_bp  7  /* Read Interrupt Flag bit position. */

#define TWI_MASTER_WIF_bm  0x40  /* Write Interrupt Flag bit mask. */
#define TWI_MASTER_WIF_bp  6  /* Write Interrupt Flag bit position. */

#define TWI_MASTER_CLKHOLD_bm  0x20  /* Clock Hold bit mask. */
#define TWI_MASTER_CLKHOLD_bp  5  /* Clock Hold bit position. */

#define TWI_MASTER_RXACK_bm  0x10  /* Received Acknowledge bit mask. */
#define TWI_MASTER_RXACK_bp  4  /* Received Acknowledge bit position. */

#define TWI_MASTER_ARBLOST_bm  0x08  /* Arbitration Lost bit mask. */
#define TWI_MASTER_ARBLOST_bp  3  /* Arbitration Lost bit position. */

#define TWI_MASTER_BUSERR_bm  0x04  /* Bus Error bit mask. */
#define TWI_MASTER_BUSERR_bp  2  /* Bus Error bit position. */

#define TWI_MASTER_BUSSTATE_gm  0x03  /* Bus State group mask. */
#define TWI_MASTER_BUSSTATE_gp  0  /* Bus State group position. */
#define TWI_MASTER_BUSSTATE0_bm  (1<<0)  /* Bus State bit 0 mask. */
#define TWI_MASTER_BUSSTATE0_bp  0  /* Bus State bit 0 position. */
#define TWI_MASTER_BUSSTATE1_bm  (1<<1)  /* Bus State bit 1 mask. */
#define TWI_MASTER_BUSSTATE1_bp  1  /* Bus State bit 1 position. */


/* TWI_SLAVE.CTRLA  bit masks and bit positions */
#define TWI_SLAVE_INTLVL_gm  0xC0  /* Interrupt Level group mask. */
#define TWI_SLAVE_INTLVL_gp  6  /* Interrupt Level group position. */
#define TWI_SLAVE_INTLVL0_bm  (1<<6)  /* Interrupt Level bit 0 mask. */
#define TWI_SLAVE_INTLVL0_bp  6  /* Interrupt Level bit 0 position. */
#define TWI_SLAVE_INTLVL1_bm  (1<<7)  /* Interrupt Level bit 1 mask. */
#define TWI_SLAVE_INTLVL1_bp  7  /* Interrupt Level bit 1 position. */

#define TWI_SLAVE_DIEN_bm  0x20  /* Data Interrupt Enable bit mask. */
#define TWI_SLAVE_DIEN_bp  5  /* Data Interrupt Enable bit position. */

#define TWI_SLAVE_APIEN_bm  0x10  /* Address/Stop Interrupt Enable bit mask. */
#define TWI_SLAVE_APIEN_bp  4  /* Address/Stop Interrupt Enable bit position. */

#define TWI_SLAVE_ENABLE_bm  0x08  /* Enable TWI Slave bit mask. */
#define TWI_SLAVE_ENABLE_bp  3  /* Enable TWI Slave bit position. */

#define TWI_SLAVE_PIEN_bm  0x04  /* Stop Interrupt Enable bit mask. */
#define TWI_SLAVE_PIEN_bp  2  /* Stop Interrupt Enable bit position. */

#define TWI_SLAVE_PMEN_bm  0x02  /* Promiscuous Mode Enable bit mask. */
#define TWI_SLAVE_PMEN_bp  1  /* Promiscuous Mode Enable bit position. */

#define TWI_SLAVE_SMEN_bm  0x01  /* Smart Mode Enable bit mask. */
#define TWI_SLAVE_SMEN_bp  0  /* Smart Mode Enable bit position. */


/* TWI_SLAVE.CTRLB  bit masks and bit positions */
#define TWI_SLAVE_ACKACT_bm  0x04  /* Acknowledge Action bit mask. */
#define TWI_SLAVE_ACKACT_bp  2  /* Acknowledge Action bit position. */

#define TWI_SLAVE_CMD_gm  0x03  /* Command group mask. */
#define TWI_SLAVE_CMD_gp  0  /* Command group position. */
#define TWI_SLAVE_CMD0_bm  (1<<0)  /* Command bit 0 mask. */
#define TWI_SLAVE_CMD0_bp  0  /* Command bit 0 position. */
#define TWI_SLAVE_CMD1_bm  (1<<1)  /* Command bit 1 mask. */
#define TWI_SLAVE_CMD1_bp  1  /* Command bit 1 position. */


/* TWI_SLAVE.STATUS  bit masks and bit positions */
#define TWI_SLAVE_DIF_bm  0x80  /* Data Interrupt Flag bit mask. */
#define TWI_SLAVE_DIF_bp  7  /* Data Interrupt Flag bit position. */

#define TWI_SLAVE_APIF_bm  0x40  /* Address/Stop Interrupt Flag bit mask. */
#define TWI_SLAVE_APIF_bp  6  /* Address/Stop Interrupt Flag bit position. */

#define TWI_SLAVE_CLKHOLD_bm  0x20  /* Clock Hold bit mask. */
#define TWI_SLAVE_CLKHOLD_bp  5  /* Clock Hold bit position. */

#define TWI_SLAVE_RXACK_bm  0x10  /* Received Acknowledge bit mask. */
#define TWI_SLAVE_RXACK_bp  4  /* Received Acknowledge bit position. */

#define TWI_SLAVE_COLL_bm  0x08  /* Collision bit mask. */
#define TWI_SLAVE_COLL_bp  3  /* Collision bit position. */

#define TWI_SLAVE_BUSERR_bm  0x04  /* Bus Error bit mask. */
#define TWI_SLAVE_BUSERR_bp  2  /* Bus Error bit position. */

#define TWI_SLAVE_DIR_bm  0x02  /* Read/Write Direction bit mask. */
#define TWI_SLAVE_DIR_bp  1  /* Read/Write Direction bit position. */

#define TWI_SLAVE_AP_bm  0x01  /* Slave Address or Stop bit mask. */
#define TWI_SLAVE_AP_bp  0  /* Slave Address or Stop bit position. */


/* TWI.CTRL  bit masks and bit positions */
#define TWI_SDAHOLD_bm  0x02  /* SDA Hold Time Enable bit mask. */
#define TWI_SDAHOLD_bp  1  /* SDA Hold Time Enable bit position. */

#define TWI_EDIEN_bm  0x01  /* External Driver Interface Enable bit mask. */
#define TWI_EDIEN_bp  0  /* External Driver Interface Enable bit position. */


/* PORT - Port Configuration */
/* PORTCFG.VPCTRLA  bit masks and bit positions */
#define PORTCFG_VP1MAP_gm  0xF0  /* Virtual Port 1 Mapping group mask. */
#define PORTCFG_VP1MAP_gp  4  /* Virtual Port 1 Mapping group position. */
#define PORTCFG_VP1MAP0_bm  (1<<4)  /* Virtual Port 1 Mapping bit 0 mask. */
#define PORTCFG_VP1MAP0_bp  4  /* Virtual Port 1 Mapping bit 0 position. */
#define PORTCFG_VP1MAP1_bm  (1<<5)  /* Virtual Port 1 Mapping bit 1 mask. */
#define PORTCFG_VP1MAP1_bp  5  /* Virtual Port 1 Mapping bit 1 position. */
#define PORTCFG_VP1MAP2_bm  (1<<6)  /* Virtual Port 1 Mapping bit 2 mask. */
#define PORTCFG_VP1MAP2_bp  6  /* Virtual Port 1 Mapping bit 2 position. */
#define PORTCFG_VP1MAP3_bm  (1<<7)  /* Virtual Port 1 Mapping bit 3 mask. */
#define PORTCFG_VP1MAP3_bp  7  /* Virtual Port 1 Mapping bit 3 position. */

#define PORTCFG_VP0MAP_gm  0x0F  /* Virtual Port 0 Mapping group mask. */
#define PORTCFG_VP0MAP_gp  0  /* Virtual Port 0 Mapping group position. */
#define PORTCFG_VP0MAP0_bm  (1<<0)  /* Virtual Port 0 Mapping bit 0 mask. */
#define PORTCFG_VP0MAP0_bp  0  /* Virtual Port 0 Mapping bit 0 position. */
#define PORTCFG_VP0MAP1_bm  (1<<1)  /* Virtual Port 0 Mapping bit 1 mask. */
#define PORTCFG_VP0MAP1_bp  1  /* Virtual Port 0 Mapping bit 1 position. */
#define PORTCFG_VP0MAP2_bm  (1<<2)  /* Virtual Port 0 Mapping bit 2 mask. */
#define PORTCFG_VP0MAP2_bp  2  /* Virtual Port 0 Mapping bit 2 position. */
#define PORTCFG_VP0MAP3_bm  (1<<3)  /* Virtual Port 0 Mapping bit 3 mask. */
#define PORTCFG_VP0MAP3_bp  3  /* Virtual Port 0 Mapping bit 3 position. */


/* PORTCFG.VPCTRLB  bit masks and bit positions */
#define PORTCFG_VP3MAP_gm  0xF0  /* Virtual Port 3 Mapping group mask. */
#define PORTCFG_VP3MAP_gp  4  /* Virtual Port 3 Mapping group position. */
#define PORTCFG_VP3MAP0_bm  (1<<4)  /* Virtual Port 3 Mapping bit 0 mask. */
#define PORTCFG_VP3MAP0_bp  4  /* Virtual Port 3 Mapping bit 0 position. */
#define PORTCFG_VP3MAP1_bm  (1<<5)  /* Virtual Port 3 Mapping bit 1 mask. */
#define PORTCFG_VP3MAP1_bp  5  /* Virtual Port 3 Mapping bit 1 position. */
#define PORTCFG_VP3MAP2_bm  (1<<6)  /* Virtual Port 3 Mapping bit 2 mask. */
#define PORTCFG_VP3MAP2_bp  6  /* Virtual Port 3 Mapping bit 2 position. */
#define PORTCFG_VP3MAP3_bm  (1<<7)  /* Virtual Port 3 Mapping bit 3 mask. */
#define PORTCFG_VP3MAP3_bp  7  /* Virtual Port 3 Mapping bit 3 position. */

#define PORTCFG_VP2MAP_gm  0x0F  /* Virtual Port 2 Mapping group mask. */
#define PORTCFG_VP2MAP_gp  0  /* Virtual Port 2 Mapping group position. */
#define PORTCFG_VP2MAP0_bm  (1<<0)  /* Virtual Port 2 Mapping bit 0 mask. */
#define PORTCFG_VP2MAP0_bp  0  /* Virtual Port 2 Mapping bit 0 position. */
#define PORTCFG_VP2MAP1_bm  (1<<1)  /* Virtual Port 2 Mapping bit 1 mask. */
#define PORTCFG_VP2MAP1_bp  1  /* Virtual Port 2 Mapping bit 1 position. */
#define PORTCFG_VP2MAP2_bm  (1<<2)  /* Virtual Port 2 Mapping bit 2 mask. */
#define PORTCFG_VP2MAP2_bp  2  /* Virtual Port 2 Mapping bit 2 position. */
#define PORTCFG_VP2MAP3_bm  (1<<3)  /* Virtual Port 2 Mapping bit 3 mask. */
#define PORTCFG_VP2MAP3_bp  3  /* Virtual Port 2 Mapping bit 3 position. */


/* PORTCFG.CLKEVOUT  bit masks and bit positions */
#define PORTCFG_CLKOUT_gm  0x03  /* Clock Output Port group mask. */
#define PORTCFG_CLKOUT_gp  0  /* Clock Output Port group position. */
#define PORTCFG_CLKOUT0_bm  (1<<0)  /* Clock Output Port bit 0 mask. */
#define PORTCFG_CLKOUT0_bp  0  /* Clock Output Port bit 0 position. */
#define PORTCFG_CLKOUT1_bm  (1<<1)  /* Clock Output Port bit 1 mask. */
#define PORTCFG_CLKOUT1_bp  1  /* Clock Output Port bit 1 position. */

#define PORTCFG_EVOUT_gm  0x30  /* Event Output Port group mask. */
#define PORTCFG_EVOUT_gp  4  /* Event Output Port group position. */
#define PORTCFG_EVOUT0_bm  (1<<4)  /* Event Output Port bit 0 mask. */
#define PORTCFG_EVOUT0_bp  4  /* Event Output Port bit 0 position. */
#define PORTCFG_EVOUT1_bm  (1<<5)  /* Event Output Port bit 1 mask. */
#define PORTCFG_EVOUT1_bp  5  /* Event Output Port bit 1 position. */


/* VPORT.INTFLAGS  bit masks and bit positions */
#define VPORT_INT1IF_bm  0x02  /* Port Interrupt 1 Flag bit mask. */
#define VPORT_INT1IF_bp  1  /* Port Interrupt 1 Flag bit position. */

#define VPORT_INT0IF_bm  0x01  /* Port Interrupt 0 Flag bit mask. */
#define VPORT_INT0IF_bp  0  /* Port Interrupt 0 Flag bit position. */


/* PORT.INTCTRL  bit masks and bit positions */
#define PORT_INT1LVL_gm  0x0C  /* Port Interrupt 1 Level group mask. */
#define PORT_INT1LVL_gp  2  /* Port Interrupt 1 Level group position. */
#define PORT_INT1LVL0_bm  (1<<2)  /* Port Interrupt 1 Level bit 0 mask. */
#define PORT_INT1LVL0_bp  2  /* Port Interrupt 1 Level bit 0 position. */
#define PORT_INT1LVL1_bm  (1<<3)  /* Port Interrupt 1 Level bit 1 mask. */
#define PORT_INT1LVL1_bp  3  /* Port Interrupt 1 Level bit 1 position. */

#define PORT_INT0LVL_gm  0x03  /* Port Interrupt 0 Level group mask. */
#define PORT_INT0LVL_gp  0  /* Port Interrupt 0 Level group position. */
#define PORT_INT0LVL0_bm  (1<<0)  /* Port Interrupt 0 Level bit 0 mask. */
#define PORT_INT0LVL0_bp  0  /* Port Interrupt 0 Level bit 0 position. */
#define PORT_INT0LVL1_bm  (1<<1)  /* Port Interrupt 0 Level bit 1 mask. */
#define PORT_INT0LVL1_bp  1  /* Port Interrupt 0 Level bit 1 position. */


/* PORT.INTFLAGS  bit masks and bit positions */
#define PORT_INT1IF_bm  0x02  /* Port Interrupt 1 Flag bit mask. */
#define PORT_INT1IF_bp  1  /* Port Interrupt 1 Flag bit position. */

#define PORT_INT0IF_bm  0x01  /* Port Interrupt 0 Flag bit mask. */
#define PORT_INT0IF_bp  0  /* Port Interrupt 0 Flag bit position. */


/* PORT.PIN0CTRL  bit masks and bit positions */
#define PORT_SRLEN_bm  0x80  /* Slew Rate Enable bit mask. */
#define PORT_SRLEN_bp  7  /* Slew Rate Enable bit position. */

#define PORT_INVEN_bm  0x40  /* Inverted I/O Enable bit mask. */
#define PORT_INVEN_bp  6  /* Inverted I/O Enable bit position. */

#define PORT_OPC_gm  0x38  /* Output/Pull Configuration group mask. */
#define PORT_OPC_gp  3  /* Output/Pull Configuration group position. */
#define PORT_OPC0_bm  (1<<3)  /* Output/Pull Configuration bit 0 mask. */
#define PORT_OPC0_bp  3  /* Output/Pull Configuration bit 0 position. */
#define PORT_OPC1_bm  (1<<4)  /* Output/Pull Configuration bit 1 mask. */
#define PORT_OPC1_bp  4  /* Output/Pull Configuration bit 1 position. */
#define PORT_OPC2_bm  (1<<5)  /* Output/Pull Configuration bit 2 mask. */
#define PORT_OPC2_bp  5  /* Output/Pull Configuration bit 2 position. */

#define PORT_ISC_gm  0x07  /* Input/Sense Configuration group mask. */
#define PORT_ISC_gp  0  /* Input/Sense Configuration group position. */
#define PORT_ISC0_bm  (1<<0)  /* Input/Sense Configuration bit 0 mask. */
#define PORT_ISC0_bp  0  /* Input/Sense Configuration bit 0 position. */
#define PORT_ISC1_bm  (1<<1)  /* Input/Sense Configuration bit 1 mask. */
#define PORT_ISC1_bp  1  /* Input/Sense Configuration bit 1 position. */
#define PORT_ISC2_bm  (1<<2)  /* Input/Sense Configuration bit 2 mask. */
#define PORT_ISC2_bp  2  /* Input/Sense Configuration bit 2 position. */


/* PORT.PIN1CTRL  bit masks and bit positions */
/* PORT_SRLEN_bm  Predefined. */
/* PORT_SRLEN_bp  Predefined. */

/* PORT_INVEN_bm  Predefined. */
/* PORT_INVEN_bp  Predefined. */

/* PORT_OPC_gm  Predefined. */
/* PORT_OPC_gp  Predefined. */
/* PORT_OPC0_bm  Predefined. */
/* PORT_OPC0_bp  Predefined. */
/* PORT_OPC1_bm  Predefined. */
/* PORT_OPC1_bp  Predefined. */
/* PORT_OPC2_bm  Predefined. */
/* PORT_OPC2_bp  Predefined. */

/* PORT_ISC_gm  Predefined. */
/* PORT_ISC_gp  Predefined. */
/* PORT_ISC0_bm  Predefined. */
/* PORT_ISC0_bp  Predefined. */
/* PORT_ISC1_bm  Predefined. */
/* PORT_ISC1_bp  Predefined. */
/* PORT_ISC2_bm  Predefined. */
/* PORT_ISC2_bp  Predefined. */


/* PORT.PIN2CTRL  bit masks and bit positions */
/* PORT_SRLEN_bm  Predefined. */
/* PORT_SRLEN_bp  Predefined. */

/* PORT_INVEN_bm  Predefined. */
/* PORT_INVEN_bp  Predefined. */

/* PORT_OPC_gm  Predefined. */
/* PORT_OPC_gp  Predefined. */
/* PORT_OPC0_bm  Predefined. */
/* PORT_OPC0_bp  Predefined. */
/* PORT_OPC1_bm  Predefined. */
/* PORT_OPC1_bp  Predefined. */
/* PORT_OPC2_bm  Predefined. */
/* PORT_OPC2_bp  Predefined. */

/* PORT_ISC_gm  Predefined. */
/* PORT_ISC_gp  Predefined. */
/* PORT_ISC0_bm  Predefined. */
/* PORT_ISC0_bp  Predefined. */
/* PORT_ISC1_bm  Predefined. */
/* PORT_ISC1_bp  Predefined. */
/* PORT_ISC2_bm  Predefined. */
/* PORT_ISC2_bp  Predefined. */


/* PORT.PIN3CTRL  bit masks and bit positions */
/* PORT_SRLEN_bm  Predefined. */
/* PORT_SRLEN_bp  Predefined. */

/* PORT_INVEN_bm  Predefined. */
/* PORT_INVEN_bp  Predefined. */

/* PORT_OPC_gm  Predefined. */
/* PORT_OPC_gp  Predefined. */
/* PORT_OPC0_bm  Predefined. */
/* PORT_OPC0_bp  Predefined. */
/* PORT_OPC1_bm  Predefined. */
/* PORT_OPC1_bp  Predefined. */
/* PORT_OPC2_bm  Predefined. */
/* PORT_OPC2_bp  Predefined. */

/* PORT_ISC_gm  Predefined. */
/* PORT_ISC_gp  Predefined. */
/* PORT_ISC0_bm  Predefined. */
/* PORT_ISC0_bp  Predefined. */
/* PORT_ISC1_bm  Predefined. */
/* PORT_ISC1_bp  Predefined. */
/* PORT_ISC2_bm  Predefined. */
/* PORT_ISC2_bp  Predefined. */


/* PORT.PIN4CTRL  bit masks and bit positions */
/* PORT_SRLEN_bm  Predefined. */
/* PORT_SRLEN_bp  Predefined. */

/* PORT_INVEN_bm  Predefined. */
/* PORT_INVEN_bp  Predefined. */

/* PORT_OPC_gm  Predefined. */
/* PORT_OPC_gp  Predefined. */
/* PORT_OPC0_bm  Predefined. */
/* PORT_OPC0_bp  Predefined. */
/* PORT_OPC1_bm  Predefined. */
/* PORT_OPC1_bp  Predefined. */
/* PORT_OPC2_bm  Predefined. */
/* PORT_OPC2_bp  Predefined. */

/* PORT_ISC_gm  Predefined. */
/* PORT_ISC_gp  Predefined. */
/* PORT_ISC0_bm  Predefined. */
/* PORT_ISC0_bp  Predefined. */
/* PORT_ISC1_bm  Predefined. */
/* PORT_ISC1_bp  Predefined. */
/* PORT_ISC2_bm  Predefined. */
/* PORT_ISC2_bp  Predefined. */


/* PORT.PIN5CTRL  bit masks and bit positions */
/* PORT_SRLEN_bm  Predefined. */
/* PORT_SRLEN_bp  Predefined. */

/* PORT_INVEN_bm  Predefined. */
/* PORT_INVEN_bp  Predefined. */

/* PORT_OPC_gm  Predefined. */
/* PORT_OPC_gp  Predefined. */
/* PORT_OPC0_bm  Predefined. */
/* PORT_OPC0_bp  Predefined. */
/* PORT_OPC1_bm  Predefined. */
/* PORT_OPC1_bp  Predefined. */
/* PORT_OPC2_bm  Predefined. */
/* PORT_OPC2_bp  Predefined. */

/* PORT_ISC_gm  Predefined. */
/* PORT_ISC_gp  Predefined. */
/* PORT_ISC0_bm  Predefined. */
/* PORT_ISC0_bp  Predefined. */
/* PORT_ISC1_bm  Predefined. */
/* PORT_ISC1_bp  Predefined. */
/* PORT_ISC2_bm  Predefined. */
/* PORT_ISC2_bp  Predefined. */


/* PORT.PIN6CTRL  bit masks and bit positions */
/* PORT_SRLEN_bm  Predefined. */
/* PORT_SRLEN_bp  Predefined. */

/* PORT_INVEN_bm  Predefined. */
/* PORT_INVEN_bp  Predefined. */

/* PORT_OPC_gm  Predefined. */
/* PORT_OPC_gp  Predefined. */
/* PORT_OPC0_bm  Predefined. */
/* PORT_OPC0_bp  Predefined. */
/* PORT_OPC1_bm  Predefined. */
/* PORT_OPC1_bp  Predefined. */
/* PORT_OPC2_bm  Predefined. */
/* PORT_OPC2_bp  Predefined. */

/* PORT_ISC_gm  Predefined. */
/* PORT_ISC_gp  Predefined. */
/* PORT_ISC0_bm  Predefined. */
/* PORT_ISC0_bp  Predefined. */
/* PORT_ISC1_bm  Predefined. */
/* PORT_ISC1_bp  Predefined. */
/* PORT_ISC2_bm  Predefined. */
/* PORT_ISC2_bp  Predefined. */


/* PORT.PIN7CTRL  bit masks and bit positions */
/* PORT_SRLEN_bm  Predefined. */
/* PORT_SRLEN_bp  Predefined. */

/* PORT_INVEN_bm  Predefined. */
/* PORT_INVEN_bp  Predefined. */

/* PORT_OPC_gm  Predefined. */
/* PORT_OPC_gp  Predefined. */
/* PORT_OPC0_bm  Predefined. */
/* PORT_OPC0_bp  Predefined. */
/* PORT_OPC1_bm  Predefined. */
/* PORT_OPC1_bp  Predefined. */
/* PORT_OPC2_bm  Predefined. */
/* PORT_OPC2_bp  Predefined. */

/* PORT_ISC_gm  Predefined. */
/* PORT_ISC_gp  Predefined. */
/* PORT_ISC0_bm  Predefined. */
/* PORT_ISC0_bp  Predefined. */
/* PORT_ISC1_bm  Predefined. */
/* PORT_ISC1_bp  Predefined. */
/* PORT_ISC2_bm  Predefined. */
/* PORT_ISC2_bp  Predefined. */


/* TC - 16-bit Timer/Counter With PWM */
/* TC0.CTRLA  bit masks and bit positions */
#define TC0_CLKSEL_gm  0x0F  /* Clock Selection group mask. */
#define TC0_CLKSEL_gp  0  /* Clock Selection group position. */
#define TC0_CLKSEL0_bm  (1<<0)  /* Clock Selection bit 0 mask. */
#define TC0_CLKSEL0_bp  0  /* Clock Selection bit 0 position. */
#define TC0_CLKSEL1_bm  (1<<1)  /* Clock Selection bit 1 mask. */
#define TC0_CLKSEL1_bp  1  /* Clock Selection bit 1 position. */
#define TC0_CLKSEL2_bm  (1<<2)  /* Clock Selection bit 2 mask. */
#define TC0_CLKSEL2_bp  2  /* Clock Selection bit 2 position. */
#define TC0_CLKSEL3_bm  (1<<3)  /* Clock Selection bit 3 mask. */
#define TC0_CLKSEL3_bp  3  /* Clock Selection bit 3 position. */


/* TC0.CTRLB  bit masks and bit positions */
#define TC0_CCDEN_bm  0x80  /* Compare or Capture D Enable bit mask. */
#define TC0_CCDEN_bp  7  /* Compare or Capture D Enable bit position. */

#define TC0_CCCEN_bm  0x40  /* Compare or Capture C Enable bit mask. */
#define TC0_CCCEN_bp  6  /* Compare or Capture C Enable bit position. */

#define TC0_CCBEN_bm  0x20  /* Compare or Capture B Enable bit mask. */
#define TC0_CCBEN_bp  5  /* Compare or Capture B Enable bit position. */

#define TC0_CCAEN_bm  0x10  /* Compare or Capture A Enable bit mask. */
#define TC0_CCAEN_bp  4  /* Compare or Capture A Enable bit position. */

#define TC0_WGMODE_gm  0x07  /* Waveform generation mode group mask. */
#define TC0_WGMODE_gp  0  /* Waveform generation mode group position. */
#define TC0_WGMODE0_bm  (1<<0)  /* Waveform generation mode bit 0 mask. */
#define TC0_WGMODE0_bp  0  /* Waveform generation mode bit 0 position. */
#define TC0_WGMODE1_bm  (1<<1)  /* Waveform generation mode bit 1 mask. */
#define TC0_WGMODE1_bp  1  /* Waveform generation mode bit 1 position. */
#define TC0_WGMODE2_bm  (1<<2)  /* Waveform generation mode bit 2 mask. */
#define TC0_WGMODE2_bp  2  /* Waveform generation mode bit 2 position. */


/* TC0.CTRLC  bit masks and bit positions */
#define TC0_CMPD_bm  0x08  /* Compare D Output Value bit mask. */
#define TC0_CMPD_bp  3  /* Compare D Output Value bit position. */

#define TC0_CMPC_bm  0x04  /* Compare C Output Value bit mask. */
#define TC0_CMPC_bp  2  /* Compare C Output Value bit position. */

#define TC0_CMPB_bm  0x02  /* Compare B Output Value bit mask. */
#define TC0_CMPB_bp  1  /* Compare B Output Value bit position. */

#define TC0_CMPA_bm  0x01  /* Compare A Output Value bit mask. */
#define TC0_CMPA_bp  0  /* Compare A Output Value bit position. */


/* TC0.CTRLD  bit masks and bit positions */
#define TC0_EVACT_gm  0xE0  /* Event Action group mask. */
#define TC0_EVACT_gp  5  /* Event Action group position. */
#define TC0_EVACT0_bm  (1<<5)  /* Event Action bit 0 mask. */
#define TC0_EVACT0_bp  5  /* Event Action bit 0 position. */
#define TC0_EVACT1_bm  (1<<6)  /* Event Action bit 1 mask. */
#define TC0_EVACT1_bp  6  /* Event Action bit 1 position. */
#define TC0_EVACT2_bm  (1<<7)  /* Event Action bit 2 mask. */
#define TC0_EVACT2_bp  7  /* Event Action bit 2 position. */

#define TC0_EVDLY_bm  0x10  /* Event Delay bit mask. */
#define TC0_EVDLY_bp  4  /* Event Delay bit position. */

#define TC0_EVSEL_gm  0x0F  /* Event Source Select group mask. */
#define TC0_EVSEL_gp  0  /* Event Source Select group position. */
#define TC0_EVSEL0_bm  (1<<0)  /* Event Source Select bit 0 mask. */
#define TC0_EVSEL0_bp  0  /* Event Source Select bit 0 position. */
#define TC0_EVSEL1_bm  (1<<1)  /* Event Source Select bit 1 mask. */
#define TC0_EVSEL1_bp  1  /* Event Source Select bit 1 position. */
#define TC0_EVSEL2_bm  (1<<2)  /* Event Source Select bit 2 mask. */
#define TC0_EVSEL2_bp  2  /* Event Source Select bit 2 position. */
#define TC0_EVSEL3_bm  (1<<3)  /* Event Source Select bit 3 mask. */
#define TC0_EVSEL3_bp  3  /* Event Source Select bit 3 position. */


/* TC0.CTRLE  bit masks and bit positions */
#define TC0_DTHM_bm  0x02  /* Dead Time Hold Mode bit mask. */
#define TC0_DTHM_bp  1  /* Dead Time Hold Mode bit position. */

#define TC0_BYTEM_bm  0x01  /* Byte Mode bit mask. */
#define TC0_BYTEM_bp  0  /* Byte Mode bit position. */


/* TC0.INTCTRLA  bit masks and bit positions */
#define TC0_ERRINTLVL_gm  0x0C  /* Error Interrupt Level group mask. */
#define TC0_ERRINTLVL_gp  2  /* Error Interrupt Level group position. */
#define TC0_ERRINTLVL0_bm  (1<<2)  /* Error Interrupt Level bit 0 mask. */
#define TC0_ERRINTLVL0_bp  2  /* Error Interrupt Level bit 0 position. */
#define TC0_ERRINTLVL1_bm  (1<<3)  /* Error Interrupt Level bit 1 mask. */
#define TC0_ERRINTLVL1_bp  3  /* Error Interrupt Level bit 1 position. */

#define TC0_OVFINTLVL_gm  0x03  /* Overflow interrupt level group mask. */
#define TC0_OVFINTLVL_gp  0  /* Overflow interrupt level group position. */
#define TC0_OVFINTLVL0_bm  (1<<0)  /* Overflow interrupt level bit 0 mask. */
#define TC0_OVFINTLVL0_bp  0  /* Overflow interrupt level bit 0 position. */
#define TC0_OVFINTLVL1_bm  (1<<1)  /* Overflow interrupt level bit 1 mask. */
#define TC0_OVFINTLVL1_bp  1  /* Overflow interrupt level bit 1 position. */


/* TC0.INTCTRLB  bit masks and bit positions */
#define TC0_CCDINTLVL_gm  0xC0  /* Compare or Capture D Interrupt Level group mask. */
#define TC0_CCDINTLVL_gp  6  /* Compare or Capture D Interrupt Level group position. */
#define TC0_CCDINTLVL0_bm  (1<<6)  /* Compare or Capture D Interrupt Level bit 0 mask. */
#define TC0_CCDINTLVL0_bp  6  /* Compare or Capture D Interrupt Level bit 0 position. */
#define TC0_CCDINTLVL1_bm  (1<<7)  /* Compare or Capture D Interrupt Level bit 1 mask. */
#define TC0_CCDINTLVL1_bp  7  /* Compare or Capture D Interrupt Level bit 1 position. */

#define TC0_CCCINTLVL_gm  0x30  /* Compare or Capture C Interrupt Level group mask. */
#define TC0_CCCINTLVL_gp  4  /* Compare or Capture C Interrupt Level group position. */
#define TC0_CCCINTLVL0_bm  (1<<4)  /* Compare or Capture C Interrupt Level bit 0 mask. */
#define TC0_CCCINTLVL0_bp  4  /* Compare or Capture C Interrupt Level bit 0 position. */
#define TC0_CCCINTLVL1_bm  (1<<5)  /* Compare or Capture C Interrupt Level bit 1 mask. */
#define TC0_CCCINTLVL1_bp  5  /* Compare or Capture C Interrupt Level bit 1 position. */

#define TC0_CCBINTLVL_gm  0x0C  /* Compare or Capture B Interrupt Level group mask. */
#define TC0_CCBINTLVL_gp  2  /* Compare or Capture B Interrupt Level group position. */
#define TC0_CCBINTLVL0_bm  (1<<2)  /* Compare or Capture B Interrupt Level bit 0 mask. */
#define TC0_CCBINTLVL0_bp  2  /* Compare or Capture B Interrupt Level bit 0 position. */
#define TC0_CCBINTLVL1_bm  (1<<3)  /* Compare or Capture B Interrupt Level bit 1 mask. */
#define TC0_CCBINTLVL1_bp  3  /* Compare or Capture B Interrupt Level bit 1 position. */

#define TC0_CCAINTLVL_gm  0x03  /* Compare or Capture A Interrupt Level group mask. */
#define TC0_CCAINTLVL_gp  0  /* Compare or Capture A Interrupt Level group position. */
#define TC0_CCAINTLVL0_bm  (1<<0)  /* Compare or Capture A Interrupt Level bit 0 mask. */
#define TC0_CCAINTLVL0_bp  0  /* Compare or Capture A Interrupt Level bit 0 position. */
#define TC0_CCAINTLVL1_bm  (1<<1)  /* Compare or Capture A Interrupt Level bit 1 mask. */
#define TC0_CCAINTLVL1_bp  1  /* Compare or Capture A Interrupt Level bit 1 position. */


/* TC0.CTRLFCLR  bit masks and bit positions */
#define TC0_CMD_gm  0x0C  /* Command group mask. */
#define TC0_CMD_gp  2  /* Command group position. */
#define TC0_CMD0_bm  (1<<2)  /* Command bit 0 mask. */
#define TC0_CMD0_bp  2  /* Command bit 0 position. */
#define TC0_CMD1_bm  (1<<3)  /* Command bit 1 mask. */
#define TC0_CMD1_bp  3  /* Command bit 1 position. */

#define TC0_LUPD_bm  0x02  /* Lock Update bit mask. */
#define TC0_LUPD_bp  1  /* Lock Update bit position. */

#define TC0_DIR_bm  0x01  /* Direction bit mask. */
#define TC0_DIR_bp  0  /* Direction bit position. */


/* TC0.CTRLFSET  bit masks and bit positions */
/* TC0_CMD_gm  Predefined. */
/* TC0_CMD_gp  Predefined. */
/* TC0_CMD0_bm  Predefined. */
/* TC0_CMD0_bp  Predefined. */
/* TC0_CMD1_bm  Predefined. */
/* TC0_CMD1_bp  Predefined. */

/* TC0_LUPD_bm  Predefined. */
/* TC0_LUPD_bp  Predefined. */

/* TC0_DIR_bm  Predefined. */
/* TC0_DIR_bp  Predefined. */


/* TC0.CTRLGCLR  bit masks and bit positions */
#define TC0_CCDBV_bm  0x10  /* Compare or Capture D Buffer Valid bit mask. */
#define TC0_CCDBV_bp  4  /* Compare or Capture D Buffer Valid bit position. */

#define TC0_CCCBV_bm  0x08  /* Compare or Capture C Buffer Valid bit mask. */
#define TC0_CCCBV_bp  3  /* Compare or Capture C Buffer Valid bit position. */

#define TC0_CCBBV_bm  0x04  /* Compare or Capture B Buffer Valid bit mask. */
#define TC0_CCBBV_bp  2  /* Compare or Capture B Buffer Valid bit position. */

#define TC0_CCABV_bm  0x02  /* Compare or Capture A Buffer Valid bit mask. */
#define TC0_CCABV_bp  1  /* Compare or Capture A Buffer Valid bit position. */

#define TC0_PERBV_bm  0x01  /* Period Buffer Valid bit mask. */
#define TC0_PERBV_bp  0  /* Period Buffer Valid bit position. */


/* TC0.CTRLGSET  bit masks and bit positions */
/* TC0_CCDBV_bm  Predefined. */
/* TC0_CCDBV_bp  Predefined. */

/* TC0_CCCBV_bm  Predefined. */
/* TC0_CCCBV_bp  Predefined. */

/* TC0_CCBBV_bm  Predefined. */
/* TC0_CCBBV_bp  Predefined. */

/* TC0_CCABV_bm  Predefined. */
/* TC0_CCABV_bp  Predefined. */

/* TC0_PERBV_bm  Predefined. */
/* TC0_PERBV_bp  Predefined. */


/* TC0.INTFLAGS  bit masks and bit positions */
#define TC0_CCDIF_bm  0x80  /* Compare or Capture D Interrupt Flag bit mask. */
#define TC0_CCDIF_bp  7  /* Compare or Capture D Interrupt Flag bit position. */

#define TC0_CCCIF_bm  0x40  /* Compare or Capture C Interrupt Flag bit mask. */
#define TC0_CCCIF_bp  6  /* Compare or Capture C Interrupt Flag bit position. */

#define TC0_CCBIF_bm  0x20  /* Compare or Capture B Interrupt Flag bit mask. */
#define TC0_CCBIF_bp  5  /* Compare or Capture B Interrupt Flag bit position. */

#define TC0_CCAIF_bm  0x10  /* Compare or Capture A Interrupt Flag bit mask. */
#define TC0_CCAIF_bp  4  /* Compare or Capture A Interrupt Flag bit position. */

#define TC0_ERRIF_bm  0x02  /* Error Interrupt Flag bit mask. */
#define TC0_ERRIF_bp  1  /* Error Interrupt Flag bit position. */

#define TC0_OVFIF_bm  0x01  /* Overflow Interrupt Flag bit mask. */
#define TC0_OVFIF_bp  0  /* Overflow Interrupt Flag bit position. */


/* TC1.CTRLA  bit masks and bit positions */
#define TC1_CLKSEL_gm  0x0F  /* Clock Selection group mask. */
#define TC1_CLKSEL_gp  0  /* Clock Selection group position. */
#define TC1_CLKSEL0_bm  (1<<0)  /* Clock Selection bit 0 mask. */
#define TC1_CLKSEL0_bp  0  /* Clock Selection bit 0 position. */
#define TC1_CLKSEL1_bm  (1<<1)  /* Clock Selection bit 1 mask. */
#define TC1_CLKSEL1_bp  1  /* Clock Selection bit 1 position. */
#define TC1_CLKSEL2_bm  (1<<2)  /* Clock Selection bit 2 mask. */
#define TC1_CLKSEL2_bp  2  /* Clock Selection bit 2 position. */
#define TC1_CLKSEL3_bm  (1<<3)  /* Clock Selection bit 3 mask. */
#define TC1_CLKSEL3_bp  3  /* Clock Selection bit 3 position. */


/* TC1.CTRLB  bit masks and bit positions */
#define TC1_CCBEN_bm  0x20  /* Compare or Capture B Enable bit mask. */
#define TC1_CCBEN_bp  5  /* Compare or Capture B Enable bit position. */

#define TC1_CCAEN_bm  0x10  /* Compare or Capture A Enable bit mask. */
#define TC1_CCAEN_bp  4  /* Compare or Capture A Enable bit position. */

#define TC1_WGMODE_gm  0x07  /* Waveform generation mode group mask. */
#define TC1_WGMODE_gp  0  /* Waveform generation mode group position. */
#define TC1_WGMODE0_bm  (1<<0)  /* Waveform generation mode bit 0 mask. */
#define TC1_WGMODE0_bp  0  /* Waveform generation mode bit 0 position. */
#define TC1_WGMODE1_bm  (1<<1)  /* Waveform generation mode bit 1 mask. */
#define TC1_WGMODE1_bp  1  /* Waveform generation mode bit 1 position. */
#define TC1_WGMODE2_bm  (1<<2)  /* Waveform generation mode bit 2 mask. */
#define TC1_WGMODE2_bp  2  /* Waveform generation mode bit 2 position. */


/* TC1.CTRLC  bit masks and bit positions */
#define TC1_CMPB_bm  0x02  /* Compare B Output Value bit mask. */
#define TC1_CMPB_bp  1  /* Compare B Output Value bit position. */

#define TC1_CMPA_bm  0x01  /* Compare A Output Value bit mask. */
#define TC1_CMPA_bp  0  /* Compare A Output Value bit position. */


/* TC1.CTRLD  bit masks and bit positions */
#define TC1_EVACT_gm  0xE0  /* Event Action group mask. */
#define TC1_EVACT_gp  5  /* Event Action group position. */
#define TC1_EVACT0_bm  (1<<5)  /* Event Action bit 0 mask. */
#define TC1_EVACT0_bp  5  /* Event Action bit 0 position. */
#define TC1_EVACT1_bm  (1<<6)  /* Event Action bit 1 mask. */
#define TC1_EVACT1_bp  6  /* Event Action bit 1 position. */
#define TC1_EVACT2_bm  (1<<7)  /* Event Action bit 2 mask. */
#define TC1_EVACT2_bp  7  /* Event Action bit 2 position. */

#define TC1_EVDLY_bm  0x10  /* Event Delay bit mask. */
#define TC1_EVDLY_bp  4  /* Event Delay bit position. */

#define TC1_EVSEL_gm  0x0F  /* Event Source Select group mask. */
#define TC1_EVSEL_gp  0  /* Event Source Select group position. */
#define TC1_EVSEL0_bm  (1<<0)  /* Event Source Select bit 0 mask. */
#define TC1_EVSEL0_bp  0  /* Event Source Select bit 0 position. */
#define TC1_EVSEL1_bm  (1<<1)  /* Event Source Select bit 1 mask. */
#define TC1_EVSEL1_bp  1  /* Event Source Select bit 1 position. */
#define TC1_EVSEL2_bm  (1<<2)  /* Event Source Select bit 2 mask. */
#define TC1_EVSEL2_bp  2  /* Event Source Select bit 2 position. */
#define TC1_EVSEL3_bm  (1<<3)  /* Event Source Select bit 3 mask. */
#define TC1_EVSEL3_bp  3  /* Event Source Select bit 3 position. */


/* TC1.CTRLE  bit masks and bit positions */
#define TC1_DTHM_bm  0x02  /* Dead Time Hold Mode bit mask. */
#define TC1_DTHM_bp  1  /* Dead Time Hold Mode bit position. */

#define TC1_BYTEM_bm  0x01  /* Byte Mode bit mask. */
#define TC1_BYTEM_bp  0  /* Byte Mode bit position. */


/* TC1.INTCTRLA  bit masks and bit positions */
#define TC1_ERRINTLVL_gm  0x0C  /* Error Interrupt Level group mask. */
#define TC1_ERRINTLVL_gp  2  /* Error Interrupt Level group position. */
#define TC1_ERRINTLVL0_bm  (1<<2)  /* Error Interrupt Level bit 0 mask. */
#define TC1_ERRINTLVL0_bp  2  /* Error Interrupt Level bit 0 position. */
#define TC1_ERRINTLVL1_bm  (1<<3)  /* Error Interrupt Level bit 1 mask. */
#define TC1_ERRINTLVL1_bp  3  /* Error Interrupt Level bit 1 position. */

#define TC1_OVFINTLVL_gm  0x03  /* Overflow interrupt level group mask. */
#define TC1_OVFINTLVL_gp  0  /* Overflow interrupt level group position. */
#define TC1_OVFINTLVL0_bm  (1<<0)  /* Overflow interrupt level bit 0 mask. */
#define TC1_OVFINTLVL0_bp  0  /* Overflow interrupt level bit 0 position. */
#define TC1_OVFINTLVL1_bm  (1<<1)  /* Overflow interrupt level bit 1 mask. */
#define TC1_OVFINTLVL1_bp  1  /* Overflow interrupt level bit 1 position. */


/* TC1.INTCTRLB  bit masks and bit positions */
#define TC1_CCBINTLVL_gm  0x0C  /* Compare or Capture B Interrupt Level group mask. */
#define TC1_CCBINTLVL_gp  2  /* Compare or Capture B Interrupt Level group position. */
#define TC1_CCBINTLVL0_bm  (1<<2)  /* Compare or Capture B Interrupt Level bit 0 mask. */
#define TC1_CCBINTLVL0_bp  2  /* Compare or Capture B Interrupt Level bit 0 position. */
#define TC1_CCBINTLVL1_bm  (1<<3)  /* Compare or Capture B Interrupt Level bit 1 mask. */
#define TC1_CCBINTLVL1_bp  3  /* Compare or Capture B Interrupt Level bit 1 position. */

#define TC1_CCAINTLVL_gm  0x03  /* Compare or Capture A Interrupt Level group mask. */
#define TC1_CCAINTLVL_gp  0  /* Compare or Capture A Interrupt Level group position. */
#define TC1_CCAINTLVL0_bm  (1<<0)  /* Compare or Capture A Interrupt Level bit 0 mask. */
#define TC1_CCAINTLVL0_bp  0  /* Compare or Capture A Interrupt Level bit 0 position. */
#define TC1_CCAINTLVL1_bm  (1<<1)  /* Compare or Capture A Interrupt Level bit 1 mask. */
#define TC1_CCAINTLVL1_bp  1  /* Compare or Capture A Interrupt Level bit 1 position. */


/* TC1.CTRLFCLR  bit masks and bit positions */
#define TC1_CMD_gm  0x0C  /* Command group mask. */
#define TC1_CMD_gp  2  /* Command group position. */
#define TC1_CMD0_bm  (1<<2)  /* Command bit 0 mask. */
#define TC1_CMD0_bp  2  /* Command bit 0 position. */
#define TC1_CMD1_bm  (1<<3)  /* Command bit 1 mask. */
#define TC1_CMD1_bp  3  /* Command bit 1 position. */

#define TC1_LUPD_bm  0x02  /* Lock Update bit mask. */
#define TC1_LUPD_bp  1  /* Lock Update bit position. */

#define TC1_DIR_bm  0x01  /* Direction bit mask. */
#define TC1_DIR_bp  0  /* Direction bit position. */


/* TC1.CTRLFSET  bit masks and bit positions */
/* TC1_CMD_gm  Predefined. */
/* TC1_CMD_gp  Predefined. */
/* TC1_CMD0_bm  Predefined. */
/* TC1_CMD0_bp  Predefined. */
/* TC1_CMD1_bm  Predefined. */
/* TC1_CMD1_bp  Predefined. */

/* TC1_LUPD_bm  Predefined. */
/* TC1_LUPD_bp  Predefined. */

/* TC1_DIR_bm  Predefined. */
/* TC1_DIR_bp  Predefined. */


/* TC1.CTRLGCLR  bit masks and bit positions */
#define TC1_CCBBV_bm  0x04  /* Compare or Capture B Buffer Valid bit mask. */
#define TC1_CCBBV_bp  2  /* Compare or Capture B Buffer Valid bit position. */

#define TC1_CCABV_bm  0x02  /* Compare or Capture A Buffer Valid bit mask. */
#define TC1_CCABV_bp  1  /* Compare or Capture A Buffer Valid bit position. */

#define TC1_PERBV_bm  0x01  /* Period Buffer Valid bit mask. */
#define TC1_PERBV_bp  0  /* Period Buffer Valid bit position. */


/* TC1.CTRLGSET  bit masks and bit positions */
/* TC1_CCBBV_bm  Predefined. */
/* TC1_CCBBV_bp  Predefined. */

/* TC1_CCABV_bm  Predefined. */
/* TC1_CCABV_bp  Predefined. */

/* TC1_PERBV_bm  Predefined. */
/* TC1_PERBV_bp  Predefined. */


/* TC1.INTFLAGS  bit masks and bit positions */
#define TC1_CCBIF_bm  0x20  /* Compare or Capture B Interrupt Flag bit mask. */
#define TC1_CCBIF_bp  5  /* Compare or Capture B Interrupt Flag bit position. */

#define TC1_CCAIF_bm  0x10  /* Compare or Capture A Interrupt Flag bit mask. */
#define TC1_CCAIF_bp  4  /* Compare or Capture A Interrupt Flag bit position. */

#define TC1_ERRIF_bm  0x02  /* Error Interrupt Flag bit mask. */
#define TC1_ERRIF_bp  1  /* Error Interrupt Flag bit position. */

#define TC1_OVFIF_bm  0x01  /* Overflow Interrupt Flag bit mask. */
#define TC1_OVFIF_bp  0  /* Overflow Interrupt Flag bit position. */


/* AWEX.CTRL  bit masks and bit positions */
#define AWEX_PGM_bm  0x20  /* Pattern Generation Mode bit mask. */
#define AWEX_PGM_bp  5  /* Pattern Generation Mode bit position. */

#define AWEX_CWCM_bm  0x10  /* Common Waveform Channel Mode bit mask. */
#define AWEX_CWCM_bp  4  /* Common Waveform Channel Mode bit position. */

#define AWEX_DTICCDEN_bm  0x08  /* Dead Time Insertion Compare Channel D Enable bit mask. */
#define AWEX_DTICCDEN_bp  3  /* Dead Time Insertion Compare Channel D Enable bit position. */

#define AWEX_DTICCCEN_bm  0x04  /* Dead Time Insertion Compare Channel C Enable bit mask. */
#define AWEX_DTICCCEN_bp  2  /* Dead Time Insertion Compare Channel C Enable bit position. */

#define AWEX_DTICCBEN_bm  0x02  /* Dead Time Insertion Compare Channel B Enable bit mask. */
#define AWEX_DTICCBEN_bp  1  /* Dead Time Insertion Compare Channel B Enable bit position. */

#define AWEX_DTICCAEN_bm  0x01  /* Dead Time Insertion Compare Channel A Enable bit mask. */
#define AWEX_DTICCAEN_bp  0  /* Dead Time Insertion Compare Channel A Enable bit position. */


/* AWEX.FDCTRL  bit masks and bit positions */
#define AWEX_FDDBD_bm  0x10  /* Fault Detect on Disable Break Disable bit mask. */
#define AWEX_FDDBD_bp  4  /* Fault Detect on Disable Break Disable bit position. */

#define AWEX_FDMODE_bm  0x04  /* Fault Detect Mode bit mask. */
#define AWEX_FDMODE_bp  2  /* Fault Detect Mode bit position. */

#define AWEX_FDACT_gm  0x03  /* Fault Detect Action group mask. */
#define AWEX_FDACT_gp  0  /* Fault Detect Action group position. */
#define AWEX_FDACT0_bm  (1<<0)  /* Fault Detect Action bit 0 mask. */
#define AWEX_FDACT0_bp  0  /* Fault Detect Action bit 0 position. */
#define AWEX_FDACT1_bm  (1<<1)  /* Fault Detect Action bit 1 mask. */
#define AWEX_FDACT1_bp  1  /* Fault Detect Action bit 1 position. */


/* AWEX.STATUS  bit masks and bit positions */
#define AWEX_FDF_bm  0x04  /* Fault Detect Flag bit mask. */
#define AWEX_FDF_bp  2  /* Fault Detect Flag bit position. */

#define AWEX_DTHSBUFV_bm  0x02  /* Dead Time High Side Buffer Valid bit mask. */
#define AWEX_DTHSBUFV_bp  1  /* Dead Time High Side Buffer Valid bit position. */

#define AWEX_DTLSBUFV_bm  0x01  /* Dead Time Low Side Buffer Valid bit mask. */
#define AWEX_DTLSBUFV_bp  0  /* Dead Time Low Side Buffer Valid bit position. */


/* HIRES.CTRL  bit masks and bit positions */
#define HIRES_HREN_gm  0x03  /* High Resolution Enable group mask. */
#define HIRES_HREN_gp  0  /* High Resolution Enable group position. */
#define HIRES_HREN0_bm  (1<<0)  /* High Resolution Enable bit 0 mask. */
#define HIRES_HREN0_bp  0  /* High Resolution Enable bit 0 position. */
#define HIRES_HREN1_bm  (1<<1)  /* High Resolution Enable bit 1 mask. */
#define HIRES_HREN1_bp  1  /* High Resolution Enable bit 1 position. */


/* USART - Universal Asynchronous Receiver-Transmitter */
/* USART.STATUS  bit masks and bit positions */
#define USART_RXCIF_bm  0x80  /* Receive Interrupt Flag bit mask. */
#define USART_RXCIF_bp  7  /* Receive Interrupt Flag bit position. */

#define USART_TXCIF_bm  0x40  /* Transmit Interrupt Flag bit mask. */
#define USART_TXCIF_bp  6  /* Transmit Interrupt Flag bit position. */

#define USART_DREIF_bm  0x20  /* Data Register Empty Flag bit mask. */
#define USART_DREIF_bp  5  /* Data Register Empty Flag bit position. */

#define USART_FERR_bm  0x10  /* Frame Error bit mask. */
#define USART_FERR_bp  4  /* Frame Error bit position. */

#define USART_BUFOVF_bm  0x08  /* Buffer Overflow bit mask. */
#define USART_BUFOVF_bp  3  /* Buffer Overflow bit position. */

#define USART_PERR_bm  0x04  /* Parity Error bit mask. */
#define USART_PERR_bp  2  /* Parity Error bit position. */

#define USART_RXB8_bm  0x01  /* Receive Bit 8 bit mask. */
#define USART_RXB8_bp  0  /* Receive Bit 8 bit position. */


/* USART.CTRLA  bit masks and bit positions */
#define USART_RXCINTLVL_gm  0x30  /* Receive Interrupt Level group mask. */
#define USART_RXCINTLVL_gp  4  /* Receive Interrupt Level group position. */
#define USART_RXCINTLVL0_bm  (1<<4)  /* Receive Interrupt Level bit 0 mask. */
#define USART_RXCINTLVL0_bp  4  /* Receive Interrupt Level bit 0 position. */
#define USART_RXCINTLVL1_bm  (1<<5)  /* Receive Interrupt Level bit 1 mask. */
#define USART_RXCINTLVL1_bp  5  /* Receive Interrupt Level bit 1 position. */

#define USART_TXCINTLVL_gm  0x0C  /* Transmit Interrupt Level group mask. */
#define USART_TXCINTLVL_gp  2  /* Transmit Interrupt Level group position. */
#define USART_TXCINTLVL0_bm  (1<<2)  /* Transmit Interrupt Level bit 0 mask. */
#define USART_TXCINTLVL0_bp  2  /* Transmit Interrupt Level bit 0 position. */
#define USART_TXCINTLVL1_bm  (1<<3)  /* Transmit Interrupt Level bit 1 mask. */
#define USART_TXCINTLVL1_bp  3  /* Transmit Interrupt Level bit 1 position. */

#define USART_DREINTLVL_gm  0x03  /* Data Register Empty Interrupt Level group mask. */
#define USART_DREINTLVL_gp  0  /* Data Register Empty Interrupt Level group position. */
#define USART_DREINTLVL0_bm  (1<<0)  /* Data Register Empty Interrupt Level bit 0 mask. */
#define USART_DREINTLVL0_bp  0  /* Data Register Empty Interrupt Level bit 0 position. */
#define USART_DREINTLVL1_bm  (1<<1)  /* Data Register Empty Interrupt Level bit 1 mask. */
#define USART_DREINTLVL1_bp  1  /* Data Register Empty Interrupt Level bit 1 position. */


/* USART.CTRLB  bit masks and bit positions */
#define USART_RXEN_bm  0x10  /* Receiver Enable bit mask. */
#define USART_RXEN_bp  4  /* Receiver Enable bit position. */

#define USART_TXEN_bm  0x08  /* Transmitter Enable bit mask. */
#define USART_TXEN_bp  3  /* Transmitter Enable bit position. */

#define USART_CLK2X_bm  0x04  /* Double transmission speed bit mask. */
#define USART_CLK2X_bp  2  /* Double transmission speed bit position. */

#define USART_MPCM_bm  0x02  /* Multi-processor Communication Mode bit mask. */
#define USART_MPCM_bp  1  /* Multi-processor Communication Mode bit position. */

#define USART_TXB8_bm  0x01  /* Transmit bit 8 bit mask. */
#define USART_TXB8_bp  0  /* Transmit bit 8 bit position. */


/* USART.CTRLC  bit masks and bit positions */
#define USART_CMODE_gm  0xC0  /* Communication Mode group mask. */
#define USART_CMODE_gp  6  /* Communication Mode group position. */
#define USART_CMODE0_bm  (1<<6)  /* Communication Mode bit 0 mask. */
#define USART_CMODE0_bp  6  /* Communication Mode bit 0 position. */
#define USART_CMODE1_bm  (1<<7)  /* Communication Mode bit 1 mask. */
#define USART_CMODE1_bp  7  /* Communication Mode bit 1 position. */

#define USART_PMODE_gm  0x30  /* Parity Mode group mask. */
#define USART_PMODE_gp  4  /* Parity Mode group position. */
#define USART_PMODE0_bm  (1<<4)  /* Parity Mode bit 0 mask. */
#define USART_PMODE0_bp  4  /* Parity Mode bit 0 position. */
#define USART_PMODE1_bm  (1<<5)  /* Parity Mode bit 1 mask. */
#define USART_PMODE1_bp  5  /* Parity Mode bit 1 position. */

#define USART_SBMODE_bm  0x08  /* Stop Bit Mode bit mask. */
#define USART_SBMODE_bp  3  /* Stop Bit Mode bit position. */

#define USART_CHSIZE_gm  0x07  /* Character Size group mask. */
#define USART_CHSIZE_gp  0  /* Character Size group position. */
#define USART_CHSIZE0_bm  (1<<0)  /* Character Size bit 0 mask. */
#define USART_CHSIZE0_bp  0  /* Character Size bit 0 position. */
#define USART_CHSIZE1_bm  (1<<1)  /* Character Size bit 1 mask. */
#define USART_CHSIZE1_bp  1  /* Character Size bit 1 position. */
#define USART_CHSIZE2_bm  (1<<2)  /* Character Size bit 2 mask. */
#define USART_CHSIZE2_bp  2  /* Character Size bit 2 position. */


/* USART.BAUDCTRLA  bit masks and bit positions */
#define USART_BSEL_gm  0xFF  /* Baud Rate Selection Bits [7:0] group mask. */
#define USART_BSEL_gp  0  /* Baud Rate Selection Bits [7:0] group position. */
#define USART_BSEL0_bm  (1<<0)  /* Baud Rate Selection Bits [7:0] bit 0 mask. */
#define USART_BSEL0_bp  0  /* Baud Rate Selection Bits [7:0] bit 0 position. */
#define USART_BSEL1_bm  (1<<1)  /* Baud Rate Selection Bits [7:0] bit 1 mask. */
#define USART_BSEL1_bp  1  /* Baud Rate Selection Bits [7:0] bit 1 position. */
#define USART_BSEL2_bm  (1<<2)  /* Baud Rate Selection Bits [7:0] bit 2 mask. */
#define USART_BSEL2_bp  2  /* Baud Rate Selection Bits [7:0] bit 2 position. */
#define USART_BSEL3_bm  (1<<3)  /* Baud Rate Selection Bits [7:0] bit 3 mask. */
#define USART_BSEL3_bp  3  /* Baud Rate Selection Bits [7:0] bit 3 position. */
#define USART_BSEL4_bm  (1<<4)  /* Baud Rate Selection Bits [7:0] bit 4 mask. */
#define USART_BSEL4_bp  4  /* Baud Rate Selection Bits [7:0] bit 4 position. */
#define USART_BSEL5_bm  (1<<5)  /* Baud Rate Selection Bits [7:0] bit 5 mask. */
#define USART_BSEL5_bp  5  /* Baud Rate Selection Bits [7:0] bit 5 position. */
#define USART_BSEL6_bm  (1<<6)  /* Baud Rate Selection Bits [7:0] bit 6 mask. */
#define USART_BSEL6_bp  6  /* Baud Rate Selection Bits [7:0] bit 6 position. */
#define USART_BSEL7_bm  (1<<7)  /* Baud Rate Selection Bits [7:0] bit 7 mask. */
#define USART_BSEL7_bp  7  /* Baud Rate Selection Bits [7:0] bit 7 position. */


/* USART.BAUDCTRLB  bit masks and bit positions */
#define USART_BSCALE_gm  0xF0  /* Baud Rate Scale group mask. */
#define USART_BSCALE_gp  4  /* Baud Rate Scale group position. */
#define USART_BSCALE0_bm  (1<<4)  /* Baud Rate Scale bit 0 mask. */
#define USART_BSCALE0_bp  4  /* Baud Rate Scale bit 0 position. */
#define USART_BSCALE1_bm  (1<<5)  /* Baud Rate Scale bit 1 mask. */
#define USART_BSCALE1_bp  5  /* Baud Rate Scale bit 1 position. */
#define USART_BSCALE2_bm  (1<<6)  /* Baud Rate Scale bit 2 mask. */
#define USART_BSCALE2_bp  6  /* Baud Rate Scale bit 2 position. */
#define USART_BSCALE3_bm  (1<<7)  /* Baud Rate Scale bit 3 mask. */
#define USART_BSCALE3_bp  7  /* Baud Rate Scale bit 3 position. */

/* USART_BSEL_gm  Predefined. */
/* USART_BSEL_gp  Predefined. */
/* USART_BSEL0_bm  Predefined. */
/* USART_BSEL0_bp  Predefined. */
/* USART_BSEL1_bm  Predefined. */
/* USART_BSEL1_bp  Predefined. */
/* USART_BSEL2_bm  Predefined. */
/* USART_BSEL2_bp  Predefined. */
/* USART_BSEL3_bm  Predefined. */
/* USART_BSEL3_bp  Predefined. */


/* SPI - Serial Peripheral Interface */
/* SPI.CTRL  bit masks and bit positions */
#define SPI_CLK2X_bm  0x80  /* Enable Double Speed bit mask. */
#define SPI_CLK2X_bp  7  /* Enable Double Speed bit position. */

#define SPI_ENABLE_bm  0x40  /* Enable Module bit mask. */
#define SPI_ENABLE_bp  6  /* Enable Module bit position. */

#define SPI_DORD_bm  0x20  /* Data Order Setting bit mask. */
#define SPI_DORD_bp  5  /* Data Order Setting bit position. */

#define SPI_MASTER_bm  0x10  /* Master Operation Enable bit mask. */
#define SPI_MASTER_bp  4  /* Master Operation Enable bit position. */

#define SPI_MODE_gm  0x0C  /* SPI Mode group mask. */
#define SPI_MODE_gp  2  /* SPI Mode group position. */
#define SPI_MODE0_bm  (1<<2)  /* SPI Mode bit 0 mask. */
#define SPI_MODE0_bp  2  /* SPI Mode bit 0 position. */
#define SPI_MODE1_bm  (1<<3)  /* SPI Mode bit 1 mask. */
#define SPI_MODE1_bp  3  /* SPI Mode bit 1 position. */

#define SPI_PRESCALER_gm  0x03  /* Prescaler group mask. */
#define SPI_PRESCALER_gp  0  /* Prescaler group position. */
#define SPI_PRESCALER0_bm  (1<<0)  /* Prescaler bit 0 mask. */
#define SPI_PRESCALER0_bp  0  /* Prescaler bit 0 position. */
#define SPI_PRESCALER1_bm  (1<<1)  /* Prescaler bit 1 mask. */
#define SPI_PRESCALER1_bp  1  /* Prescaler bit 1 position. */


/* SPI.INTCTRL  bit masks and bit positions */
#define SPI_INTLVL_gm  0x03  /* Interrupt level group mask. */
#define SPI_INTLVL_gp  0  /* Interrupt level group position. */
#define SPI_INTLVL0_bm  (1<<0)  /* Interrupt level bit 0 mask. */
#define SPI_INTLVL0_bp  0  /* Interrupt level bit 0 position. */
#define SPI_INTLVL1_bm  (1<<1)  /* Interrupt level bit 1 mask. */
#define SPI_INTLVL1_bp  1  /* Interrupt level bit 1 position. */


/* SPI.STATUS  bit masks and bit positions */
#define SPI_IF_bm  0x80  /* Interrupt Flag bit mask. */
#define SPI_IF_bp  7  /* Interrupt Flag bit position. */

#define SPI_WRCOL_bm  0x40  /* Write Collision bit mask. */
#define SPI_WRCOL_bp  6  /* Write Collision bit position. */


/* IRCOM - IR Communication Module */
/* IRCOM.CTRL  bit masks and bit positions */
#define IRCOM_EVSEL_gm  0x0F  /* Event Channel Select group mask. */
#define IRCOM_EVSEL_gp  0  /* Event Channel Select group position. */
#define IRCOM_EVSEL0_bm  (1<<0)  /* Event Channel Select bit 0 mask. */
#define IRCOM_EVSEL0_bp  0  /* Event Channel Select bit 0 position. */
#define IRCOM_EVSEL1_bm  (1<<1)  /* Event Channel Select bit 1 mask. */
#define IRCOM_EVSEL1_bp  1  /* Event Channel Select bit 1 position. */
#define IRCOM_EVSEL2_bm  (1<<2)  /* Event Channel Select bit 2 mask. */
#define IRCOM_EVSEL2_bp  2  /* Event Channel Select bit 2 position. */
#define IRCOM_EVSEL3_bm  (1<<3)  /* Event Channel Select bit 3 mask. */
#define IRCOM_EVSEL3_bp  3  /* Event Channel Select bit 3 position. */


/* AES - AES Module */
/* AES.CTRL  bit masks and bit positions */
#define AES_START_bm  0x80  /* Start/Run bit mask. */
#define AES_START_bp  7  /* Start/Run bit position. */

#define AES_AUTO_bm  0x40  /* Auto Start Trigger bit mask. */
#define AES_AUTO_bp  6  /* Auto Start Trigger bit position. */

#define AES_RESET_bm  0x20  /* AES Software Reset bit mask. */
#define AES_RESET_bp  5  /* AES Software Reset bit position. */

#define AES_DECRYPT_bm  0x10  /* Decryption / Direction bit mask. */
#define AES_DECRYPT_bp  4  /* Decryption / Direction bit position. */

#define AES_XOR_bm  0x04  /* State XOR Load Enable bit mask. */
#define AES_XOR_bp  2  /* State XOR Load Enable bit position. */


/* AES.STATUS  bit masks and bit positions */
#define AES_ERROR_bm  0x80  /* AES Error bit mask. */
#define AES_ERROR_bp  7  /* AES Error bit position. */

#define AES_SRIF_bm  0x01  /* State Ready Interrupt Flag bit mask. */
#define AES_SRIF_bp  0  /* State Ready Interrupt Flag bit position. */


/* AES.INTCTRL  bit masks and bit positions */
#define AES_INTLVL_gm  0x03  /* Interrupt level group mask. */
#define AES_INTLVL_gp  0  /* Interrupt level group position. */
#define AES_INTLVL0_bm  (1<<0)  /* Interrupt level bit 0 mask. */
#define AES_INTLVL0_bp  0  /* Interrupt level bit 0 position. */
#define AES_INTLVL1_bm  (1<<1)  /* Interrupt level bit 1 mask. */
#define AES_INTLVL1_bp  1  /* Interrupt level bit 1 position. */



// Generic Port Pins

#define PIN0_bm 0x01 
#define PIN0_bp 0
#define PIN1_bm 0x02
#define PIN1_bp 1
#define PIN2_bm 0x04 
#define PIN2_bp 2
#define PIN3_bm 0x08 
#define PIN3_bp 3
#define PIN4_bm 0x10 
#define PIN4_bp 4
#define PIN5_bm 0x20 
#define PIN5_bp 5
#define PIN6_bm 0x40 
#define PIN6_bp 6
#define PIN7_bm 0x80 
#define PIN7_bp 7


/* ========== Interrupt Vector Definitions ========== */
/* Vector 0 is the reset vector */

/* OSC interrupt vectors */
#define OSC_XOSCF_vect_num  1
#define OSC_XOSCF_vect      _VECTOR(1)  /* External Oscillator Failure Interrupt (NMI) */

/* PORTC interrupt vectors */
#define PORTC_INT0_vect_num  2
#define PORTC_INT0_vect      _VECTOR(2)  /* External Interrupt 0 */
#define PORTC_INT1_vect_num  3
#define PORTC_INT1_vect      _VECTOR(3)  /* External Interrupt 1 */

/* PORTR interrupt vectors */
#define PORTR_INT0_vect_num  4
#define PORTR_INT0_vect      _VECTOR(4)  /* External Interrupt 0 */
#define PORTR_INT1_vect_num  5
#define PORTR_INT1_vect      _VECTOR(5)  /* External Interrupt 1 */

/* DMA interrupt vectors */
#define DMA_CH0_vect_num  6
#define DMA_CH0_vect      _VECTOR(6)  /* Channel 0 Interrupt */
#define DMA_CH1_vect_num  7
#define DMA_CH1_vect      _VECTOR(7)  /* Channel 1 Interrupt */
#define DMA_CH2_vect_num  8
#define DMA_CH2_vect      _VECTOR(8)  /* Channel 2 Interrupt */
#define DMA_CH3_vect_num  9
#define DMA_CH3_vect      _VECTOR(9)  /* Channel 3 Interrupt */

/* RTC interrupt vectors */
#define RTC_OVF_vect_num  10
#define RTC_OVF_vect      _VECTOR(10)  /* Overflow Interrupt */
#define RTC_COMP_vect_num  11
#define RTC_COMP_vect      _VECTOR(11)  /* Compare Interrupt */

/* TWIC interrupt vectors */
#define TWIC_TWIS_vect_num  12
#define TWIC_TWIS_vect      _VECTOR(12)  /* TWI Slave Interrupt */
#define TWIC_TWIM_vect_num  13
#define TWIC_TWIM_vect      _VECTOR(13)  /* TWI Master Interrupt */

/* TCC0 interrupt vectors */
#define TCC0_OVF_vect_num  14
#define TCC0_OVF_vect      _VECTOR(14)  /* Overflow Interrupt */
#define TCC0_ERR_vect_num  15
#define TCC0_ERR_vect      _VECTOR(15)  /* Error Interrupt */
#define TCC0_CCA_vect_num  16
#define TCC0_CCA_vect      _VECTOR(16)  /* Compare or Capture A Interrupt */
#define TCC0_CCB_vect_num  17
#define TCC0_CCB_vect      _VECTOR(17)  /* Compare or Capture B Interrupt */
#define TCC0_CCC_vect_num  18
#define TCC0_CCC_vect      _VECTOR(18)  /* Compare or Capture C Interrupt */
#define TCC0_CCD_vect_num  19
#define TCC0_CCD_vect      _VECTOR(19)  /* Compare or Capture D Interrupt */

/* TCC1 interrupt vectors */
#define TCC1_OVF_vect_num  20
#define TCC1_OVF_vect      _VECTOR(20)  /* Overflow Interrupt */
#define TCC1_ERR_vect_num  21
#define TCC1_ERR_vect      _VECTOR(21)  /* Error Interrupt */
#define TCC1_CCA_vect_num  22
#define TCC1_CCA_vect      _VECTOR(22)  /* Compare or Capture A Interrupt */
#define TCC1_CCB_vect_num  23
#define TCC1_CCB_vect      _VECTOR(23)  /* Compare or Capture B Interrupt */

/* SPIC interrupt vectors */
#define SPIC_INT_vect_num  24
#define SPIC_INT_vect      _VECTOR(24)  /* SPI Interrupt */

/* USARTC0 interrupt vectors */
#define USARTC0_RXC_vect_num  25
#define USARTC0_RXC_vect      _VECTOR(25)  /* Reception Complete Interrupt */
#define USARTC0_DRE_vect_num  26
#define USARTC0_DRE_vect      _VECTOR(26)  /* Data Register Empty Interrupt */
#define USARTC0_TXC_vect_num  27
#define USARTC0_TXC_vect      _VECTOR(27)  /* Transmission Complete Interrupt */

/* USARTC1 interrupt vectors */
#define USARTC1_RXC_vect_num  28
#define USARTC1_RXC_vect      _VECTOR(28)  /* Reception Complete Interrupt */
#define USARTC1_DRE_vect_num  29
#define USARTC1_DRE_vect      _VECTOR(29)  /* Data Register Empty Interrupt */
#define USARTC1_TXC_vect_num  30
#define USARTC1_TXC_vect      _VECTOR(30)  /* Transmission Complete Interrupt */

/* AES interrupt vectors */
#define AES_INT_vect_num  31
#define AES_INT_vect      _VECTOR(31)  /* AES Interrupt */

/* NVM interrupt vectors */
#define NVM_EE_vect_num  32
#define NVM_EE_vect      _VECTOR(32)  /* EE Interrupt */
#define NVM_SPM_vect_num  33
#define NVM_SPM_vect      _VECTOR(33)  /* SPM Interrupt */

/* PORTB interrupt vectors */
#define PORTB_INT0_vect_num  34
#define PORTB_INT0_vect      _VECTOR(34)  /* External Interrupt 0 */
#define PORTB_INT1_vect_num  35
#define PORTB_INT1_vect      _VECTOR(35)  /* External Interrupt 1 */

/* ACB interrupt vectors */
#define ACB_AC0_vect_num  36
#define ACB_AC0_vect      _VECTOR(36)  /* AC0 Interrupt */
#define ACB_AC1_vect_num  37
#define ACB_AC1_vect      _VECTOR(37)  /* AC1 Interrupt */
#define ACB_ACW_vect_num  38
#define ACB_ACW_vect      _VECTOR(38)  /* ACW Window Mode Interrupt */

/* ADCB interrupt vectors */
#define ADCB_CH0_vect_num  39
#define ADCB_CH0_vect      _VECTOR(39)  /* Interrupt 0 */
#define ADCB_CH1_vect_num  40
#define ADCB_CH1_vect      _VECTOR(40)  /* Interrupt 1 */
#define ADCB_CH2_vect_num  41
#define ADCB_CH2_vect      _VECTOR(41)  /* Interrupt 2 */
#define ADCB_CH3_vect_num  42
#define ADCB_CH3_vect      _VECTOR(42)  /* Interrupt 3 */

/* PORTE interrupt vectors */
#define PORTE_INT0_vect_num  43
#define PORTE_INT0_vect      _VECTOR(43)  /* External Interrupt 0 */
#define PORTE_INT1_vect_num  44
#define PORTE_INT1_vect      _VECTOR(44)  /* External Interrupt 1 */

/* TWIE interrupt vectors */
#define TWIE_TWIS_vect_num  45
#define TWIE_TWIS_vect      _VECTOR(45)  /* TWI Slave Interrupt */
#define TWIE_TWIM_vect_num  46
#define TWIE_TWIM_vect      _VECTOR(46)  /* TWI Master Interrupt */

/* TCE0 interrupt vectors */
#define TCE0_OVF_vect_num  47
#define TCE0_OVF_vect      _VECTOR(47)  /* Overflow Interrupt */
#define TCE0_ERR_vect_num  48
#define TCE0_ERR_vect      _VECTOR(48)  /* Error Interrupt */
#define TCE0_CCA_vect_num  49
#define TCE0_CCA_vect      _VECTOR(49)  /* Compare or Capture A Interrupt */
#define TCE0_CCB_vect_num  50
#define TCE0_CCB_vect      _VECTOR(50)  /* Compare or Capture B Interrupt */
#define TCE0_CCC_vect_num  51
#define TCE0_CCC_vect      _VECTOR(51)  /* Compare or Capture C Interrupt */
#define TCE0_CCD_vect_num  52
#define TCE0_CCD_vect      _VECTOR(52)  /* Compare or Capture D Interrupt */

/* TCE1 interrupt vectors */
#define TCE1_OVF_vect_num  53
#define TCE1_OVF_vect      _VECTOR(53)  /* Overflow Interrupt */
#define TCE1_ERR_vect_num  54
#define TCE1_ERR_vect      _VECTOR(54)  /* Error Interrupt */
#define TCE1_CCA_vect_num  55
#define TCE1_CCA_vect      _VECTOR(55)  /* Compare or Capture A Interrupt */
#define TCE1_CCB_vect_num  56
#define TCE1_CCB_vect      _VECTOR(56)  /* Compare or Capture B Interrupt */

/* SPIE interrupt vectors */
#define SPIE_INT_vect_num  57
#define SPIE_INT_vect      _VECTOR(57)  /* SPI Interrupt */

/* USARTE0 interrupt vectors */
#define USARTE0_RXC_vect_num  58
#define USARTE0_RXC_vect      _VECTOR(58)  /* Reception Complete Interrupt */
#define USARTE0_DRE_vect_num  59
#define USARTE0_DRE_vect      _VECTOR(59)  /* Data Register Empty Interrupt */
#define USARTE0_TXC_vect_num  60
#define USARTE0_TXC_vect      _VECTOR(60)  /* Transmission Complete Interrupt */

/* USARTE1 interrupt vectors */
#define USARTE1_RXC_vect_num  61
#define USARTE1_RXC_vect      _VECTOR(61)  /* Reception Complete Interrupt */
#define USARTE1_DRE_vect_num  62
#define USARTE1_DRE_vect      _VECTOR(62)  /* Data Register Empty Interrupt */
#define USARTE1_TXC_vect_num  63
#define USARTE1_TXC_vect      _VECTOR(63)  /* Transmission Complete Interrupt */

/* PORTD interrupt vectors */
#define PORTD_INT0_vect_num  64
#define PORTD_INT0_vect      _VECTOR(64)  /* External Interrupt 0 */
#define PORTD_INT1_vect_num  65
#define PORTD_INT1_vect      _VECTOR(65)  /* External Interrupt 1 */

/* PORTA interrupt vectors */
#define PORTA_INT0_vect_num  66
#define PORTA_INT0_vect      _VECTOR(66)  /* External Interrupt 0 */
#define PORTA_INT1_vect_num  67
#define PORTA_INT1_vect      _VECTOR(67)  /* External Interrupt 1 */

/* ACA interrupt vectors */
#define ACA_AC0_vect_num  68
#define ACA_AC0_vect      _VECTOR(68)  /* AC0 Interrupt */
#define ACA_AC1_vect_num  69
#define ACA_AC1_vect      _VECTOR(69)  /* AC1 Interrupt */
#define ACA_ACW_vect_num  70
#define ACA_ACW_vect      _VECTOR(70)  /* ACW Window Mode Interrupt */

/* ADCA interrupt vectors */
#define ADCA_CH0_vect_num  71
#define ADCA_CH0_vect      _VECTOR(71)  /* Interrupt 0 */
#define ADCA_CH1_vect_num  72
#define ADCA_CH1_vect      _VECTOR(72)  /* Interrupt 1 */
#define ADCA_CH2_vect_num  73
#define ADCA_CH2_vect      _VECTOR(73)  /* Interrupt 2 */
#define ADCA_CH3_vect_num  74
#define ADCA_CH3_vect      _VECTOR(74)  /* Interrupt 3 */

/* TCD0 interrupt vectors */
#define TCD0_OVF_vect_num  77
#define TCD0_OVF_vect      _VECTOR(77)  /* Overflow Interrupt */
#define TCD0_ERR_vect_num  78
#define TCD0_ERR_vect      _VECTOR(78)  /* Error Interrupt */
#define TCD0_CCA_vect_num  79
#define TCD0_CCA_vect      _VECTOR(79)  /* Compare or Capture A Interrupt */
#define TCD0_CCB_vect_num  80
#define TCD0_CCB_vect      _VECTOR(80)  /* Compare or Capture B Interrupt */
#define TCD0_CCC_vect_num  81
#define TCD0_CCC_vect      _VECTOR(81)  /* Compare or Capture C Interrupt */
#define TCD0_CCD_vect_num  82
#define TCD0_CCD_vect      _VECTOR(82)  /* Compare or Capture D Interrupt */

/* TCD1 interrupt vectors */
#define TCD1_OVF_vect_num  83
#define TCD1_OVF_vect      _VECTOR(83)  /* Overflow Interrupt */
#define TCD1_ERR_vect_num  84
#define TCD1_ERR_vect      _VECTOR(84)  /* Error Interrupt */
#define TCD1_CCA_vect_num  85
#define TCD1_CCA_vect      _VECTOR(85)  /* Compare or Capture A Interrupt */
#define TCD1_CCB_vect_num  86
#define TCD1_CCB_vect      _VECTOR(86)  /* Compare or Capture B Interrupt */

/* SPID interrupt vectors */
#define SPID_INT_vect_num  87
#define SPID_INT_vect      _VECTOR(87)  /* SPI Interrupt */

/* USARTD0 interrupt vectors */
#define USARTD0_RXC_vect_num  88
#define USARTD0_RXC_vect      _VECTOR(88)  /* Reception Complete Interrupt */
#define USARTD0_DRE_vect_num  89
#define USARTD0_DRE_vect      _VECTOR(89)  /* Data Register Empty Interrupt */
#define USARTD0_TXC_vect_num  90
#define USARTD0_TXC_vect      _VECTOR(90)  /* Transmission Complete Interrupt */

/* USARTD1 interrupt vectors */
#define USARTD1_RXC_vect_num  91
#define USARTD1_RXC_vect      _VECTOR(91)  /* Reception Complete Interrupt */
#define USARTD1_DRE_vect_num  92
#define USARTD1_DRE_vect      _VECTOR(92)  /* Data Register Empty Interrupt */
#define USARTD1_TXC_vect_num  93
#define USARTD1_TXC_vect      _VECTOR(93)  /* Transmission Complete Interrupt */

/* PORTF interrupt vectors */
#define PORTF_INT0_vect_num  104
#define PORTF_INT0_vect      _VECTOR(104)  /* External Interrupt 0 */
#define PORTF_INT1_vect_num  105
#define PORTF_INT1_vect      _VECTOR(105)  /* External Interrupt 1 */

/* TCF0 interrupt vectors */
#define TCF0_OVF_vect_num  108
#define TCF0_OVF_vect      _VECTOR(108)  /* Overflow Interrupt */
#define TCF0_ERR_vect_num  109
#define TCF0_ERR_vect      _VECTOR(109)  /* Error Interrupt */
#define TCF0_CCA_vect_num  110
#define TCF0_CCA_vect      _VECTOR(110)  /* Compare or Capture A Interrupt */
#define TCF0_CCB_vect_num  111
#define TCF0_CCB_vect      _VECTOR(111)  /* Compare or Capture B Interrupt */
#define TCF0_CCC_vect_num  112
#define TCF0_CCC_vect      _VECTOR(112)  /* Compare or Capture C Interrupt */
#define TCF0_CCD_vect_num  113
#define TCF0_CCD_vect      _VECTOR(113)  /* Compare or Capture D Interrupt */

/* USARTF0 interrupt vectors */
#define USARTF0_RXC_vect_num  119
#define USARTF0_RXC_vect      _VECTOR(119)  /* Reception Complete Interrupt */
#define USARTF0_DRE_vect_num  120
#define USARTF0_DRE_vect      _VECTOR(120)  /* Data Register Empty Interrupt */
#define USARTF0_TXC_vect_num  121
#define USARTF0_TXC_vect      _VECTOR(121)  /* Transmission Complete Interrupt */


#define _VECTOR_SIZE 4 /* Size of individual vector. */
#define _VECTORS_SIZE (122 * _VECTOR_SIZE)


/* ========== Constants ========== */

#define PROGMEM_START     (0x0000)
#define PROGMEM_SIZE      (69632)
#define PROGMEM_END       (PROGMEM_START + PROGMEM_SIZE - 1)

#define APP_SECTION_START     (0x0000)
#define APP_SECTION_SIZE      (65536)
#define APP_SECTION_END       (APP_SECTION_START + APP_SECTION_SIZE - 1)

#define APPTABLE_SECTION_START     (0x0F000)
#define APPTABLE_SECTION_SIZE      (4096)
#define APPTABLE_SECTION_END       (APPTABLE_SECTION_START + APPTABLE_SECTION_SIZE - 1)

#define BOOT_SECTION_START     (0x10000)
#define BOOT_SECTION_SIZE      (4096)
#define BOOT_SECTION_END       (BOOT_SECTION_START + BOOT_SECTION_SIZE - 1)

#define DATAMEM_START     (0x0000)
#define DATAMEM_SIZE      (12288)
#define DATAMEM_END       (DATAMEM_START + DATAMEM_SIZE - 1)

#define IO_START     (0x0000)
#define IO_SIZE      (4096)
#define IO_END       (IO_START + IO_SIZE - 1)

#define MAPPED_EEPROM_START     (0x1000)
#define MAPPED_EEPROM_SIZE      (2048)
#define MAPPED_EEPROM_END       (MAPPED_EEPROM_START + MAPPED_EEPROM_SIZE - 1)

#define INTERNAL_SRAM_START     (0x2000)
#define INTERNAL_SRAM_SIZE      (4096)
#define INTERNAL_SRAM_END       (INTERNAL_SRAM_START + INTERNAL_SRAM_SIZE - 1)

#define EEPROM_START     (0x0000)
#define EEPROM_SIZE      (2048)
#define EEPROM_END       (EEPROM_START + EEPROM_SIZE - 1)

#define FUSE_START     (0x0000)
#define FUSE_SIZE      (6)
#define FUSE_END       (FUSE_START + FUSE_SIZE - 1)

#define LOCKBIT_START     (0x0000)
#define LOCKBIT_SIZE      (1)
#define LOCKBIT_END       (LOCKBIT_START + LOCKBIT_SIZE - 1)

#define SIGNATURES_START     (0x0000)
#define SIGNATURES_SIZE      (3)
#define SIGNATURES_END       (SIGNATURES_START + SIGNATURES_SIZE - 1)

#define USER_SIGNATURES_START     (0x0000)
#define USER_SIGNATURES_SIZE      (256)
#define USER_SIGNATURES_END       (USER_SIGNATURES_START + USER_SIGNATURES_SIZE - 1)

#define PROD_SIGNATURES_START     (0x0000)
#define PROD_SIGNATURES_SIZE      (52)
#define PROD_SIGNATURES_END       (PROD_SIGNATURES_START + PROD_SIGNATURES_SIZE - 1)

#define FLASHEND     PROGMEM_END
#define SPM_PAGESIZE PROGMEM_PAGE_SIZE
#define RAMSTART     INTERNAL_SRAM_START
#define RAMSIZE      INTERNAL_SRAM_SIZE
#define RAMEND       INTERNAL_SRAM_END
#define XRAMSTART    EXTERNAL_SRAM_START
#define XRAMSIZE     EXTERNAL_SRAM_SIZE
#define XRAMEND      INTERNAL_SRAM_END
#define E2END        EEPROM_END
#define E2PAGESIZE   EEPROM_PAGE_SIZE


/* ========== Fuses ========== */
#define FUSE_MEMORY_SIZE 6

/* Fuse Byte 0 */
#define FUSE_JTAGUSERID0  ~_BV(0)  /* JTAG User ID Bit 0 */
#define FUSE_JTAGUSERID1  ~_BV(1)  /* JTAG User ID Bit 1 */
#define FUSE_JTAGUSERID2  ~_BV(2)  /* JTAG User ID Bit 2 */
#define FUSE_JTAGUSERID3  ~_BV(3)  /* JTAG User ID Bit 3 */
#define FUSE_JTAGUSERID4  ~_BV(4)  /* JTAG User ID Bit 4 */
#define FUSE_JTAGUSERID5  ~_BV(5)  /* JTAG User ID Bit 5 */
#define FUSE_JTAGUSERID6  ~_BV(6)  /* JTAG User ID Bit 6 */
#define FUSE_JTAGUSERID7  ~_BV(7)  /* JTAG User ID Bit 7 */
#define FUSE0_DEFAULT  (0xFF)

/* Fuse Byte 1 */
#define FUSE_WDP0  ~_BV(0)  /* Watchdog Timeout Period Bit 0 */
#define FUSE_WDP1  ~_BV(1)  /* Watchdog Timeout Period Bit 1 */
#define FUSE_WDP2  ~_BV(2)  /* Watchdog Timeout Period Bit 2 */
#define FUSE_WDP3  ~_BV(3)  /* Watchdog Timeout Period Bit 3 */
#define FUSE_WDWP0  ~_BV(4)  /* Watchdog Window Timeout Period Bit 0 */
#define FUSE_WDWP1  ~_BV(5)  /* Watchdog Window Timeout Period Bit 1 */
#define FUSE_WDWP2  ~_BV(6)  /* Watchdog Window Timeout Period Bit 2 */
#define FUSE_WDWP3  ~_BV(7)  /* Watchdog Window Timeout Period Bit 3 */
#define FUSE1_DEFAULT  (0xFF)

/* Fuse Byte 2 */
#define FUSE_BODPD0  ~_BV(0)  /* BOD Operation in Power-Down Mode Bit 0 */
#define FUSE_BODPD1  ~_BV(1)  /* BOD Operation in Power-Down Mode Bit 1 */
#define FUSE_BODACT0  ~_BV(2)  /* BOD Operation in Active Mode Bit 0 */
#define FUSE_BODACT1  ~_BV(3)  /* BOD Operation in Active Mode Bit 1 */
#define FUSE_BOOTRST  ~_BV(6)  /* Boot Loader Section Reset Vector */
#define FUSE_DVSDON  ~_BV(7)  /* Spike Detector Enable */
#define FUSE2_DEFAULT  (0xFF)

/* Fuse Byte 3 Reserved */

/* Fuse Byte 4 */
#define FUSE_JTAGEN  ~_BV(0)  /* JTAG Interface Enable */
#define FUSE_WDLOCK  ~_BV(1)  /* Watchdog Timer Lock */
#define FUSE_SUT0  ~_BV(2)  /* Start-up Time Bit 0 */
#define FUSE_SUT1  ~_BV(3)  /* Start-up Time Bit 1 */
#define FUSE4_DEFAULT  (0xFF)

/* Fuse Byte 5 */
#define FUSE_BODLVL0  ~_BV(0)  /* Brown Out Detection Voltage Level Bit 0 */
#define FUSE_BODLVL1  ~_BV(1)  /* Brown Out Detection Voltage Level Bit 1 */
#define FUSE_BODLVL2  ~_BV(2)  /* Brown Out Detection Voltage Level Bit 2 */
#define FUSE_EESAVE  ~_BV(3)  /* Preserve EEPROM Through Chip Erase */
#define FUSE5_DEFAULT  (0xFF)


/* ========== Lock Bits ========== */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_APPLICATION_TABLE_BITS_EXIST
#define __BOOT_LOCK_APPLICATION_BITS_EXIST
#define __BOOT_LOCK_BOOT_BITS_EXIST


/* ========== Signature ========== */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x96
#define SIGNATURE_2 0x42


#endif /* _AVR_ATxmega64A3_H_ */

