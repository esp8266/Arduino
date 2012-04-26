/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _SAM3U2C_
#define _SAM3U2C_

/** \addtogroup SAM3U2C_definitions SAM3U2C definitions
  This file defines all structures and symbols for SAM3U2C:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - PIO definitions
*/
/*@{*/

#ifdef __cplusplus
 extern "C" {
#endif

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include <stdint.h>
#ifndef __cplusplus
typedef volatile const uint32_t RoReg; /**< Read only 32-bit register (volatile const unsigned int) */
#else
typedef volatile       uint32_t RoReg; /**< Read only 32-bit register (volatile const unsigned int) */
#endif
typedef volatile       uint32_t WoReg; /**< Write only 32-bit register (volatile unsigned int) */
typedef volatile       uint32_t RwReg; /**< Read-Write 32-bit register (volatile unsigned int) */
#endif

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR SAM3U2C */
/* ************************************************************************** */
/** \addtogroup SAM3U2C_cmsis CMSIS Definitions */
/*@{*/

/**< Interrupt Number Definition */
typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ******************************/
  NonMaskableInt_IRQn   = -14, /**<  2 Non Maskable Interrupt                */
  MemoryManagement_IRQn = -12, /**<  4 Cortex-M3 Memory Management Interrupt */
  BusFault_IRQn         = -11, /**<  5 Cortex-M3 Bus Fault Interrupt         */
  UsageFault_IRQn       = -10, /**<  6 Cortex-M3 Usage Fault Interrupt       */
  SVCall_IRQn           = -5,  /**< 11 Cortex-M3 SV Call Interrupt           */
  DebugMonitor_IRQn     = -4,  /**< 12 Cortex-M3 Debug Monitor Interrupt     */
  PendSV_IRQn           = -2,  /**< 14 Cortex-M3 Pend SV Interrupt           */
  SysTick_IRQn          = -1,  /**< 15 Cortex-M3 System Tick Interrupt       */
/******  SAM3U2C specific Interrupt Numbers *********************************/

  SUPC_IRQn            =  0, /**<  0 SAM3U2C Supply Controller (SUPC) */
  RSTC_IRQn            =  1, /**<  1 SAM3U2C Reset Controller (RSTC) */
  RTC_IRQn             =  2, /**<  2 SAM3U2C Real Time Clock (RTC) */
  RTT_IRQn             =  3, /**<  3 SAM3U2C Real Time Timer (RTT) */
  WDT_IRQn             =  4, /**<  4 SAM3U2C Watchdog Timer (WDT) */
  PMC_IRQn             =  5, /**<  5 SAM3U2C Power Management Controller (PMC) */
  EFC0_IRQn            =  6, /**<  6 SAM3U2C Enhanced Embedded Flash Controller 0 (EFC0) */
  EFC1_IRQn            =  7, /**<  7 SAM3U2C Enhanced Embedded Flash Controller 1 (EFC1) */
  UART_IRQn            =  8, /**<  8 SAM3U2C Universal Asynchronous Receiver Transmitter (UART) */
  SMC_IRQn             =  9, /**<  9 SAM3U2C Static Memory Controller (SMC) */
  PIOA_IRQn            = 10, /**< 10 SAM3U2C Parallel I/O Controller A, (PIOA) */
  PIOB_IRQn            = 11, /**< 11 SAM3U2C Parallel I/O Controller B (PIOB) */
  USART0_IRQn          = 13, /**< 13 SAM3U2C USART 0 (USART0) */
  USART1_IRQn          = 14, /**< 14 SAM3U2C USART 1 (USART1) */
  USART2_IRQn          = 15, /**< 15 SAM3U2C USART 2 (USART2) */
  HSMCI_IRQn           = 17, /**< 17 SAM3U2C High Speed Multimedia Card Interface (HSMCI) */
  TWI0_IRQn            = 18, /**< 18 SAM3U2C Two-Wire Interface 0 (TWI0) */
  TWI1_IRQn            = 19, /**< 19 SAM3U2C Two-Wire Interface 1 (TWI1) */
  SPI_IRQn             = 20, /**< 20 SAM3U2C Serial Peripheral Interface (SPI) */
  SSC_IRQn             = 21, /**< 21 SAM3U2C Synchronous Serial Controller (SSC) */
  TC0_IRQn             = 22, /**< 22 SAM3U2C Timer Counter 0 (TC0) */
  TC1_IRQn             = 23, /**< 23 SAM3U2C Timer Counter 1 (TC1) */
  TC2_IRQn             = 24, /**< 24 SAM3U2C Timer Counter 2 (TC2) */
  PWM_IRQn             = 25, /**< 25 SAM3U2C Pulse Width Modulation Controller (PWM) */
  ADC12B_IRQn          = 26, /**< 26 SAM3U2C 12-bit ADC Controller (ADC12B) */
  ADC_IRQn             = 27, /**< 27 SAM3U2C 10-bit ADC Controller (ADC) */
  DMAC_IRQn            = 28, /**< 28 SAM3U2C DMA Controller (DMAC) */
  UDPHS_IRQn           = 29, /**< 29 SAM3U2C USB Device High Speed (UDPHS) */

  PERIPH_COUNT_IRQn    = 30  /**< Number of peripheral IDs */
} IRQn_Type;

typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;

  /* Cortex-M handlers */
  void* pfnReset_Handler;
  void* pfnNMI_Handler;
  void* pfnHardFault_Handler;
  void* pfnMemManage_Handler;
  void* pfnBusFault_Handler;
  void* pfnUsageFault_Handler;
  void* pfnReserved1_Handler;
  void* pfnReserved2_Handler;
  void* pfnReserved3_Handler;
  void* pfnReserved4_Handler;
  void* pfnSVC_Handler;
  void* pfnDebugMon_Handler;
  void* pfnReserved5_Handler;
  void* pfnPendSV_Handler;
  void* pfnSysTick_Handler;

  /* Peripheral handlers */
  void* pfnSUPC_Handler;   /*  0 Supply Controller */
  void* pfnRSTC_Handler;   /*  1 Reset Controller */
  void* pfnRTC_Handler;    /*  2 Real Time Clock */
  void* pfnRTT_Handler;    /*  3 Real Time Timer */
  void* pfnWDT_Handler;    /*  4 Watchdog Timer */
  void* pfnPMC_Handler;    /*  5 Power Management Controller */
  void* pfnEFC0_Handler;   /*  6 Enhanced Embedded Flash Controller 0 */
  void* pfnEFC1_Handler;   /*  7 Enhanced Embedded Flash Controller 1 */
  void* pfnUART_Handler;   /*  8 Universal Asynchronous Receiver Transmitter */
  void* pfnSMC_Handler;    /*  9 Static Memory Controller */
  void* pfnPIOA_Handler;   /* 10 Parallel I/O Controller A, */
  void* pfnPIOB_Handler;   /* 11 Parallel I/O Controller B */
  void* pvReserved12;
  void* pfnUSART0_Handler; /* 13 USART 0 */
  void* pfnUSART1_Handler; /* 14 USART 1 */
  void* pfnUSART2_Handler; /* 15 USART 2 */
  void* pvReserved16;
  void* pfnHSMCI_Handler;  /* 17 High Speed Multimedia Card Interface */
  void* pfnTWI0_Handler;   /* 18 Two-Wire Interface 0 */
  void* pfnTWI1_Handler;   /* 19 Two-Wire Interface 1 */
  void* pfnSPI_Handler;    /* 20 Serial Peripheral Interface */
  void* pfnSSC_Handler;    /* 21 Synchronous Serial Controller */
  void* pfnTC0_Handler;    /* 22 Timer Counter 0 */
  void* pfnTC1_Handler;    /* 23 Timer Counter 1 */
  void* pfnTC2_Handler;    /* 24 Timer Counter 2 */
  void* pfnPWM_Handler;    /* 25 Pulse Width Modulation Controller */
  void* pfnADC12B_Handler; /* 26 12-bit ADC Controller */
  void* pfnADC_Handler;    /* 27 10-bit ADC Controller */
  void* pfnDMAC_Handler;   /* 28 DMA Controller */
  void* pfnUDPHS_Handler;  /* 29 USB Device High Speed */
} DeviceVectors;

/* Cortex-M3 core handlers */
void Reset_Handler      ( void );
void NMI_Handler        ( void );
void HardFault_Handler  ( void );
void MemManage_Handler  ( void );
void BusFault_Handler   ( void );
void UsageFault_Handler ( void );
void SVC_Handler        ( void );
void DebugMon_Handler   ( void );
void PendSV_Handler     ( void );
void SysTick_Handler    ( void );

/* Peripherals handlers */
void ADC_Handler        ( void );
void ADC12B_Handler     ( void );
void DMAC_Handler       ( void );
void EFC0_Handler       ( void );
void EFC1_Handler       ( void );
void HSMCI_Handler      ( void );
void PIOA_Handler       ( void );
void PIOB_Handler       ( void );
void PMC_Handler        ( void );
void PWM_Handler        ( void );
void RSTC_Handler       ( void );
void RTC_Handler        ( void );
void RTT_Handler        ( void );
void SMC_Handler        ( void );
void SPI_Handler        ( void );
void SSC_Handler        ( void );
void SUPC_Handler       ( void );
void TC0_Handler        ( void );
void TC1_Handler        ( void );
void TC2_Handler        ( void );
void TWI0_Handler       ( void );
void TWI1_Handler       ( void );
void UART_Handler       ( void );
void UDPHS_Handler      ( void );
void USART0_Handler     ( void );
void USART1_Handler     ( void );
void USART2_Handler     ( void );
void WDT_Handler        ( void );

/**
 * \brief Configuration of the Cortex-M3 Processor and Core Peripherals
 */

#define __CM3_REV              0x0200 /**< SAM3U2C core revision number ([15:8] revision number, [7:0] patch number) */
#define __MPU_PRESENT          1      /**< SAM3U2C does provide a MPU */
#define __NVIC_PRIO_BITS       4      /**< SAM3U2C uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0      /**< Set to 1 if different SysTick Config is used */

/*
 * \brief CMSIS includes
 */

#include <core_cm3.h>
#if !defined DONT_USE_CMSIS_INIT
#include "system_sam3u.h"
#endif /* DONT_USE_CMSIS_INIT */

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAM3U2C */
/* ************************************************************************** */
/** \addtogroup SAM3U2C_api Peripheral Software API */
/*@{*/

#include "component/component_adc.h"
#include "component/component_adc12b.h"
#include "component/component_chipid.h"
#include "component/component_dmac.h"
#include "component/component_efc.h"
#include "component/component_gpbr.h"
#include "component/component_hsmci.h"
#include "component/component_matrix.h"
#include "component/component_pdc.h"
#include "component/component_pio.h"
#include "component/component_pmc.h"
#include "component/component_pwm.h"
#include "component/component_rstc.h"
#include "component/component_rtc.h"
#include "component/component_rtt.h"
#include "component/component_smc.h"
#include "component/component_spi.h"
#include "component/component_ssc.h"
#include "component/component_supc.h"
#include "component/component_tc.h"
#include "component/component_twi.h"
#include "component/component_uart.h"
#include "component/component_udphs.h"
#include "component/component_usart.h"
#include "component/component_wdt.h"
/*@}*/

/* ************************************************************************** */
/*   REGISTER ACCESS DEFINITIONS FOR SAM3U2C */
/* ************************************************************************** */
/** \addtogroup SAM3U2C_reg Registers Access Definitions */
/*@{*/

#include "instance/instance_hsmci.h"
#include "instance/instance_ssc.h"
#include "instance/instance_spi.h"
#include "instance/instance_tc0.h"
#include "instance/instance_twi0.h"
#include "instance/instance_twi1.h"
#include "instance/instance_pwm.h"
#include "instance/instance_usart0.h"
#include "instance/instance_usart1.h"
#include "instance/instance_usart2.h"
#include "instance/instance_udphs.h"
#include "instance/instance_adc12b.h"
#include "instance/instance_adc.h"
#include "instance/instance_dmac.h"
#include "instance/instance_smc.h"
#include "instance/instance_matrix.h"
#include "instance/instance_pmc.h"
#include "instance/instance_uart.h"
#include "instance/instance_chipid.h"
#include "instance/instance_efc0.h"
#include "instance/instance_efc1.h"
#include "instance/instance_pioa.h"
#include "instance/instance_piob.h"
#include "instance/instance_rstc.h"
#include "instance/instance_supc.h"
#include "instance/instance_rtt.h"
#include "instance/instance_wdt.h"
#include "instance/instance_rtc.h"
#include "instance/instance_gpbr.h"
/*@}*/

/* ************************************************************************** */
/*   PERIPHERAL ID DEFINITIONS FOR SAM3U2C */
/* ************************************************************************** */
/** \addtogroup SAM3U2C_id Peripheral Ids Definitions */
/*@{*/

#define ID_SUPC   ( 0) /**< \brief Supply Controller (SUPC) */
#define ID_RSTC   ( 1) /**< \brief Reset Controller (RSTC) */
#define ID_RTC    ( 2) /**< \brief Real Time Clock (RTC) */
#define ID_RTT    ( 3) /**< \brief Real Time Timer (RTT) */
#define ID_WDT    ( 4) /**< \brief Watchdog Timer (WDT) */
#define ID_PMC    ( 5) /**< \brief Power Management Controller (PMC) */
#define ID_EFC0   ( 6) /**< \brief Enhanced Embedded Flash Controller 0 (EFC0) */
#define ID_EFC1   ( 7) /**< \brief Enhanced Embedded Flash Controller 1 (EFC1) */
#define ID_UART   ( 8) /**< \brief Universal Asynchronous Receiver Transmitter (UART) */
#define ID_SMC    ( 9) /**< \brief Static Memory Controller (SMC) */
#define ID_PIOA   (10) /**< \brief Parallel I/O Controller A, (PIOA) */
#define ID_PIOB   (11) /**< \brief Parallel I/O Controller B (PIOB) */
#define ID_USART0 (13) /**< \brief USART 0 (USART0) */
#define ID_USART1 (14) /**< \brief USART 1 (USART1) */
#define ID_USART2 (15) /**< \brief USART 2 (USART2) */
#define ID_HSMCI  (17) /**< \brief High Speed Multimedia Card Interface (HSMCI) */
#define ID_TWI0   (18) /**< \brief Two-Wire Interface 0 (TWI0) */
#define ID_TWI1   (19) /**< \brief Two-Wire Interface 1 (TWI1) */
#define ID_SPI    (20) /**< \brief Serial Peripheral Interface (SPI) */
#define ID_SSC    (21) /**< \brief Synchronous Serial Controller (SSC) */
#define ID_TC0    (22) /**< \brief Timer Counter 0 (TC0) */
#define ID_TC1    (23) /**< \brief Timer Counter 1 (TC1) */
#define ID_TC2    (24) /**< \brief Timer Counter 2 (TC2) */
#define ID_PWM    (25) /**< \brief Pulse Width Modulation Controller (PWM) */
#define ID_ADC12B (26) /**< \brief 12-bit ADC Controller (ADC12B) */
#define ID_ADC    (27) /**< \brief 10-bit ADC Controller (ADC) */
#define ID_DMAC   (28) /**< \brief DMA Controller (DMAC) */
#define ID_UDPHS  (29) /**< \brief USB Device High Speed (UDPHS) */

#define ID_PERIPH_COUNT (30) /**< \brief Number of peripheral IDs */
/*@}*/

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAM3U2C */
/* ************************************************************************** */
/** \addtogroup SAM3U2C_base Peripheral Base Address Definitions */
/*@{*/

#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define HSMCI      (0x40000000U) /**< \brief (HSMCI     ) Base Address */
#define SSC        (0x40004000U) /**< \brief (SSC       ) Base Address */
#define SPI        (0x40008000U) /**< \brief (SPI       ) Base Address */
#define TC0        (0x40080000U) /**< \brief (TC0       ) Base Address */
#define TWI0       (0x40084000U) /**< \brief (TWI0      ) Base Address */
#define PDC_TWI0   (0x40084100U) /**< \brief (PDC_TWI0  ) Base Address */
#define TWI1       (0x40088000U) /**< \brief (TWI1      ) Base Address */
#define PDC_TWI1   (0x40088100U) /**< \brief (PDC_TWI1  ) Base Address */
#define PWM        (0x4008C000U) /**< \brief (PWM       ) Base Address */
#define PDC_PWM    (0x4008C100U) /**< \brief (PDC_PWM   ) Base Address */
#define USART0     (0x40090000U) /**< \brief (USART0    ) Base Address */
#define PDC_USART0 (0x40090100U) /**< \brief (PDC_USART0) Base Address */
#define USART1     (0x40094000U) /**< \brief (USART1    ) Base Address */
#define PDC_USART1 (0x40094100U) /**< \brief (PDC_USART1) Base Address */
#define USART2     (0x40098000U) /**< \brief (USART2    ) Base Address */
#define PDC_USART2 (0x40098100U) /**< \brief (PDC_USART2) Base Address */
#define UDPHS      (0x400A4000U) /**< \brief (UDPHS     ) Base Address */
#define ADC12B     (0x400A8000U) /**< \brief (ADC12B    ) Base Address */
#define PDC_ADC12B (0x400A8100U) /**< \brief (PDC_ADC12B) Base Address */
#define ADC        (0x400AC000U) /**< \brief (ADC       ) Base Address */
#define PDC_ADC    (0x400AC100U) /**< \brief (PDC_ADC   ) Base Address */
#define DMAC       (0x400B0000U) /**< \brief (DMAC      ) Base Address */
#define SMC        (0x400E0000U) /**< \brief (SMC       ) Base Address */
#define MATRIX     (0x400E0200U) /**< \brief (MATRIX    ) Base Address */
#define PMC        (0x400E0400U) /**< \brief (PMC       ) Base Address */
#define UART       (0x400E0600U) /**< \brief (UART      ) Base Address */
#define PDC_UART   (0x400E0700U) /**< \brief (PDC_UART  ) Base Address */
#define CHIPID     (0x400E0740U) /**< \brief (CHIPID    ) Base Address */
#define EFC0       (0x400E0800U) /**< \brief (EFC0      ) Base Address */
#define EFC1       (0x400E0A00U) /**< \brief (EFC1      ) Base Address */
#define PIOA       (0x400E0C00U) /**< \brief (PIOA      ) Base Address */
#define PIOB       (0x400E0E00U) /**< \brief (PIOB      ) Base Address */
#define RSTC       (0x400E1200U) /**< \brief (RSTC      ) Base Address */
#define SUPC       (0x400E1210U) /**< \brief (SUPC      ) Base Address */
#define RTT        (0x400E1230U) /**< \brief (RTT       ) Base Address */
#define WDT        (0x400E1250U) /**< \brief (WDT       ) Base Address */
#define RTC        (0x400E1260U) /**< \brief (RTC       ) Base Address */
#define GPBR       (0x400E1290U) /**< \brief (GPBR      ) Base Address */
#else
#define HSMCI      ((Hsmci  *)0x40000000U) /**< \brief (HSMCI     ) Base Address */
#define SSC        ((Ssc    *)0x40004000U) /**< \brief (SSC       ) Base Address */
#define SPI        ((Spi    *)0x40008000U) /**< \brief (SPI       ) Base Address */
#define TC0        ((Tc     *)0x40080000U) /**< \brief (TC0       ) Base Address */
#define TWI0       ((Twi    *)0x40084000U) /**< \brief (TWI0      ) Base Address */
#define PDC_TWI0   ((Pdc    *)0x40084100U) /**< \brief (PDC_TWI0  ) Base Address */
#define TWI1       ((Twi    *)0x40088000U) /**< \brief (TWI1      ) Base Address */
#define PDC_TWI1   ((Pdc    *)0x40088100U) /**< \brief (PDC_TWI1  ) Base Address */
#define PWM        ((Pwm    *)0x4008C000U) /**< \brief (PWM       ) Base Address */
#define PDC_PWM    ((Pdc    *)0x4008C100U) /**< \brief (PDC_PWM   ) Base Address */
#define USART0     ((Usart  *)0x40090000U) /**< \brief (USART0    ) Base Address */
#define PDC_USART0 ((Pdc    *)0x40090100U) /**< \brief (PDC_USART0) Base Address */
#define USART1     ((Usart  *)0x40094000U) /**< \brief (USART1    ) Base Address */
#define PDC_USART1 ((Pdc    *)0x40094100U) /**< \brief (PDC_USART1) Base Address */
#define USART2     ((Usart  *)0x40098000U) /**< \brief (USART2    ) Base Address */
#define PDC_USART2 ((Pdc    *)0x40098100U) /**< \brief (PDC_USART2) Base Address */
#define UDPHS      ((Udphs  *)0x400A4000U) /**< \brief (UDPHS     ) Base Address */
#define ADC12B     ((Adc12b *)0x400A8000U) /**< \brief (ADC12B    ) Base Address */
#define PDC_ADC12B ((Pdc    *)0x400A8100U) /**< \brief (PDC_ADC12B) Base Address */
#define ADC        ((Adc    *)0x400AC000U) /**< \brief (ADC       ) Base Address */
#define PDC_ADC    ((Pdc    *)0x400AC100U) /**< \brief (PDC_ADC   ) Base Address */
#define DMAC       ((Dmac   *)0x400B0000U) /**< \brief (DMAC      ) Base Address */
#define SMC        ((Smc    *)0x400E0000U) /**< \brief (SMC       ) Base Address */
#define MATRIX     ((Matrix *)0x400E0200U) /**< \brief (MATRIX    ) Base Address */
#define PMC        ((Pmc    *)0x400E0400U) /**< \brief (PMC       ) Base Address */
#define UART       ((Uart   *)0x400E0600U) /**< \brief (UART      ) Base Address */
#define PDC_UART   ((Pdc    *)0x400E0700U) /**< \brief (PDC_UART  ) Base Address */
#define CHIPID     ((Chipid *)0x400E0740U) /**< \brief (CHIPID    ) Base Address */
#define EFC0       ((Efc    *)0x400E0800U) /**< \brief (EFC0      ) Base Address */
#define EFC1       ((Efc    *)0x400E0A00U) /**< \brief (EFC1      ) Base Address */
#define PIOA       ((Pio    *)0x400E0C00U) /**< \brief (PIOA      ) Base Address */
#define PIOB       ((Pio    *)0x400E0E00U) /**< \brief (PIOB      ) Base Address */
#define RSTC       ((Rstc   *)0x400E1200U) /**< \brief (RSTC      ) Base Address */
#define SUPC       ((Supc   *)0x400E1210U) /**< \brief (SUPC      ) Base Address */
#define RTT        ((Rtt    *)0x400E1230U) /**< \brief (RTT       ) Base Address */
#define WDT        ((Wdt    *)0x400E1250U) /**< \brief (WDT       ) Base Address */
#define RTC        ((Rtc    *)0x400E1260U) /**< \brief (RTC       ) Base Address */
#define GPBR       ((Gpbr   *)0x400E1290U) /**< \brief (GPBR      ) Base Address */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/*@}*/

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAM3U2C */
/* ************************************************************************** */
/** \addtogroup SAM3U2C_pio Peripheral Pio Definitions */
/*@{*/

#include "pio/pio_sam3u2c.h"
/*@}*/

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAM3U2C */
/* ************************************************************************** */

#define IFLASH0_SIZE             (0x20000u)
#define IFLASH0_PAGE_SIZE        (256u)
#define IFLASH0_LOCK_REGION_SIZE (8192u)
#define IFLASH0_NB_OF_PAGES      (512u)
#define IRAM0_SIZE               (0x4000u)
#define IRAM1_SIZE               (0x4000u)
#define NFCRAM_SIZE              (0x1000u)
#define IFLASH_SIZE              (IFLASH0_SIZE)
#define IRAM_SIZE                (IRAM0_SIZE+IRAM1_SIZE)

#define IFLASH0_ADDR   (0x00080000u) /**< Internal Flash 0 base address */
#define IROM_ADDR      (0x00180000u) /**< Internal ROM base address */
#define IRAM0_ADDR     (0x20000000u) /**< Internal RAM 0 base address */
#define IRAM1_ADDR     (0x20080000u) /**< Internal RAM 1 base address */
#define NFC_RAM_ADDR   (0x20100000u) /**< NAND Flash Controller RAM base address */
#define UDPHS_RAM_ADDR (0x20180000u) /**< USB High Speed Device Port RAM base address */

/* ************************************************************************** */
/*   ELECTRICAL DEFINITIONS FOR SAM3U2C */
/* ************************************************************************** */

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN           (20000UL)
#define CHIP_FREQ_SLCK_RC               (32000UL)
#define CHIP_FREQ_SLCK_RC_MAX           (44000UL)
#define CHIP_FREQ_MAINCK_RC_4MHZ        (4000000UL)
#define CHIP_FREQ_MAINCK_RC_8MHZ        (8000000UL)
#define CHIP_FREQ_MAINCK_RC_12MHZ       (12000000UL)
#define CHIP_FREQ_CPU_MAX               (96000000UL)
#define CHIP_FREQ_XTAL_32K              (32768UL)
#define CHIP_FREQ_XTAL_12M              (12000000UL)

/* Embedded Flash Write Wait State */
#define CHIP_FLASH_WRITE_WAIT_STATE     (6U)

/* Embedded Flash Read Wait State (VDDCORE set at 1.65V) */
#define CHIP_FREQ_FWS_0                 (24000000UL) /**< \brief Maximum operating frequency when FWS is 0 */
#define CHIP_FREQ_FWS_1                 (40000000UL) /**< \brief Maximum operating frequency when FWS is 1 */
#define CHIP_FREQ_FWS_2                 (72000000UL) /**< \brief Maximum operating frequency when FWS is 2 */
#define CHIP_FREQ_FWS_3                 (84000000UL) /**< \brief Maximum operating frequency when FWS is 3 */


#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* _SAM3U2C_ */
