/* %ATMEL_LICENCE% */

#ifndef _SAM3N1B_
#define _SAM3N1B_

/** \addtogroup SAM3N1B_definitions SAM3N1B definitions
  This file defines all structures and symbols for SAM3N1B:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - PIO definitions
*/
/*@{*/

#ifdef __cplusplus
 extern "C" {
#endif 

#ifndef __ASSEMBLY__
#include <stdint.h>
#ifndef __cplusplus
typedef volatile const uint32_t RoReg; /**< Read only 32-bit register (volatile const unsigned int) */
#else
typedef volatile       uint32_t RoReg; /**< Read only 32-bit register (volatile const unsigned int) */
#endif
typedef volatile       uint32_t WoReg; /**< Write only 32-bit register (volatile unsigned int) */
typedef volatile       uint32_t RwReg; /**< Read-Write 32-bit register (volatile unsigned int) */
#define CAST(type, value) (((type)*)(value))
#define REG_ACCESS(type, address) (*((type)*)(address)) /**< C code: Register value */
#else
#define CAST(type, value) (value) 
#define REG_ACCESS(type, address) (address) /**< Assembly code: Register address */
#endif

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR SAM3N1B */
/* ************************************************************************** */
/** \addtogroup SAM3N1B_cmsis CMSIS Definitions */
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
/******  SAM3N1B specific Interrupt Numbers *********************************/
  
  SUPC_IRQn            =  0, /**<  0 SAM3N1B Supply Controller (SUPC) */
  RSTC_IRQn            =  1, /**<  1 SAM3N1B Reset Controller (RSTC) */
  RTC_IRQn             =  2, /**<  2 SAM3N1B Real Time Clock (RTC) */
  RTT_IRQn             =  3, /**<  3 SAM3N1B Real Time Timer (RTT) */
  WDT_IRQn             =  4, /**<  4 SAM3N1B Watchdog Timer (WDT) */
  PMC_IRQn             =  5, /**<  5 SAM3N1B Power Management Controller (PMC) */
  EFC_IRQn             =  6, /**<  6 SAM3N1B Enhanced Flash Controller (EFC) */
  UART0_IRQn           =  8, /**<  8 SAM3N1B UART 0 (UART0) */
  UART1_IRQn           =  9, /**<  9 SAM3N1B UART 1 (UART1) */
  PIOA_IRQn            = 11, /**< 11 SAM3N1B Parallel I/O Controller A (PIOA) */
  PIOB_IRQn            = 12, /**< 12 SAM3N1B Parallel I/O Controller B (PIOB) */
  PIOC_IRQn            = 13, /**< 13 SAM3N1B Parallel I/O Controller C (PIOC) */
  USART0_IRQn          = 14, /**< 14 SAM3N1B USART 0 (USART0) */
  USART1_IRQn          = 15, /**< 15 SAM3N1B USART 1 (USART1) */
  TWI0_IRQn            = 19, /**< 19 SAM3N1B Two Wire Interface 0 (TWI0) */
  TWI1_IRQn            = 20, /**< 20 SAM3N1B Two Wire Interface 1 (TWI1) */
  SPI_IRQn             = 21, /**< 21 SAM3N1B Serial Peripheral Interface (SPI) */
  TC0_IRQn             = 23, /**< 23 SAM3N1B Timer/Counter 0 (TC0) */
  TC1_IRQn             = 24, /**< 24 SAM3N1B Timer/Counter 1 (TC1) */
  TC2_IRQn             = 25, /**< 25 SAM3N1B Timer/Counter 2 (TC2) */
  TC3_IRQn             = 26, /**< 26 SAM3N1B Timer/Counter 3 (TC3) */
  TC4_IRQn             = 27, /**< 27 SAM3N1B Timer/Counter 4 (TC4) */
  TC5_IRQn             = 28, /**< 28 SAM3N1B Timer/Counter 5 (TC5) */
  ADC_IRQn             = 29, /**< 29 SAM3N1B Analog To Digital Converter (ADC) */
  DACC_IRQn            = 30, /**< 30 SAM3N1B Digital To Analog Converter (DACC) */
  PWM_IRQn             = 31  /**< 31 SAM3N1B Pulse Width Modulation (PWM) */
} IRQn_Type;

/**
 * \brief Configuration of the Cortex-M3 Processor and Core Peripherals 
 */

#define __MPU_PRESENT          0 /**< SAM3N1B does not provide a MPU */
#define __NVIC_PRIO_BITS       4 /**< SAM3N1B uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0 /**< Set to 1 if different SysTick Config is used */

/*
 * \brief CMSIS includes
 */

#include <core_cm3.h>

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAM3N1B */
/* ************************************************************************** */
/** \addtogroup SAM3N1B_api Peripheral Software API */
/*@{*/

#include "component/ADC.h"
#include "component/CHIPID.h"
#include "component/DACC.h"
#include "component/EFC.h"
#include "component/GPBR.h"
#include "component/MATRIX.h"
#include "component/PDC.h"
#include "component/PIO.h"
#include "component/PMC.h"
#include "component/PWM.h"
#include "component/RSTC.h"
#include "component/RTC.h"
#include "component/RTT.h"
#include "component/SPI.h"
#include "component/SUPC.h"
#include "component/TC.h"
#include "component/TWI.h"
#include "component/UART.h"
#include "component/USART.h"
#include "component/WDT.h"
/*@}*/

/* ************************************************************************** */
/*   REGISTER ACCESS DEFINITIONS FOR SAM3N1B */
/* ************************************************************************** */
/** \addtogroup SAM3N1B_reg Registers Access Definitions */
/*@{*/

#include "instance/SPI.h"
#include "instance/TC0.h"
#include "instance/TWI0.h"
#include "instance/TWI1.h"
#include "instance/PWM.h"
#include "instance/USART0.h"
#include "instance/USART1.h"
#include "instance/ADC.h"
#include "instance/DACC.h"
#include "instance/MATRIX.h"
#include "instance/PMC.h"
#include "instance/UART0.h"
#include "instance/CHIPID.h"
#include "instance/UART1.h"
#include "instance/EFC.h"
#include "instance/PIOA.h"
#include "instance/PIOB.h"
#include "instance/RSTC.h"
#include "instance/SUPC.h"
#include "instance/RTT.h"
#include "instance/WDT.h"
#include "instance/RTC.h"
#include "instance/GPBR.h"
/*@}*/

/* ************************************************************************** */
/*   PERIPHERAL ID DEFINITIONS FOR SAM3N1B */
/* ************************************************************************** */
/** \addtogroup SAM3N1B_id Peripheral Ids Definitions */
/*@{*/

#define ID_SUPC   ( 0) /**< \brief Supply Controller (SUPC) */
#define ID_RSTC   ( 1) /**< \brief Reset Controller (RSTC) */
#define ID_RTC    ( 2) /**< \brief Real Time Clock (RTC) */
#define ID_RTT    ( 3) /**< \brief Real Time Timer (RTT) */
#define ID_WDT    ( 4) /**< \brief Watchdog Timer (WDT) */
#define ID_PMC    ( 5) /**< \brief Power Management Controller (PMC) */
#define ID_EFC    ( 6) /**< \brief Enhanced Flash Controller (EFC) */
#define ID_UART0  ( 8) /**< \brief UART 0 (UART0) */
#define ID_UART1  ( 9) /**< \brief UART 1 (UART1) */
#define ID_PIOA   (11) /**< \brief Parallel I/O Controller A (PIOA) */
#define ID_PIOB   (12) /**< \brief Parallel I/O Controller B (PIOB) */
#define ID_PIOC   (13) /**< \brief Parallel I/O Controller C (PIOC) */
#define ID_USART0 (14) /**< \brief USART 0 (USART0) */
#define ID_USART1 (15) /**< \brief USART 1 (USART1) */
#define ID_TWI0   (19) /**< \brief Two Wire Interface 0 (TWI0) */
#define ID_TWI1   (20) /**< \brief Two Wire Interface 1 (TWI1) */
#define ID_SPI    (21) /**< \brief Serial Peripheral Interface (SPI) */
#define ID_TC0    (23) /**< \brief Timer/Counter 0 (TC0) */
#define ID_TC1    (24) /**< \brief Timer/Counter 1 (TC1) */
#define ID_TC2    (25) /**< \brief Timer/Counter 2 (TC2) */
#define ID_TC3    (26) /**< \brief Timer/Counter 3 (TC3) */
#define ID_TC4    (27) /**< \brief Timer/Counter 4 (TC4) */
#define ID_TC5    (28) /**< \brief Timer/Counter 5 (TC5) */
#define ID_ADC    (29) /**< \brief Analog To Digital Converter (ADC) */
#define ID_DACC   (30) /**< \brief Digital To Analog Converter (DACC) */
#define ID_PWM    (31) /**< \brief Pulse Width Modulation (PWM) */
/*@}*/

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAM3N1B */
/* ************************************************************************** */
/** \addtogroup SAM3N1B_base Peripheral Base Address Definitions */
/*@{*/

#define SPI        CAST(Spi       , 0x40008000U) /**< \brief (SPI       ) Base Address */
#define PDC_SPI    CAST(Pdc       , 0x40008100U) /**< \brief (PDC_SPI   ) Base Address */
#define TC0        CAST(Tc        , 0x40010000U) /**< \brief (TC0       ) Base Address */
#define TWI0       CAST(Twi       , 0x40018000U) /**< \brief (TWI0      ) Base Address */
#define PDC_TWI0   CAST(Pdc       , 0x40018100U) /**< \brief (PDC_TWI0  ) Base Address */
#define TWI1       CAST(Twi       , 0x4001C000U) /**< \brief (TWI1      ) Base Address */
#define PWM        CAST(Pwm       , 0x40020000U) /**< \brief (PWM       ) Base Address */
#define USART0     CAST(Usart     , 0x40024000U) /**< \brief (USART0    ) Base Address */
#define PDC_USART0 CAST(Pdc       , 0x40024100U) /**< \brief (PDC_USART0) Base Address */
#define USART1     CAST(Usart     , 0x40028000U) /**< \brief (USART1    ) Base Address */
#define ADC        CAST(Adc       , 0x40038000U) /**< \brief (ADC       ) Base Address */
#define PDC_ADC    CAST(Pdc       , 0x40038100U) /**< \brief (PDC_ADC   ) Base Address */
#define DACC       CAST(Dacc      , 0x4003C000U) /**< \brief (DACC      ) Base Address */
#define PDC_DACC   CAST(Pdc       , 0x4003C100U) /**< \brief (PDC_DACC  ) Base Address */
#define MATRIX     CAST(Matrix    , 0x400E0200U) /**< \brief (MATRIX    ) Base Address */
#define PMC        CAST(Pmc       , 0x400E0400U) /**< \brief (PMC       ) Base Address */
#define UART0      CAST(Uart      , 0x400E0600U) /**< \brief (UART0     ) Base Address */
#define PDC_UART0  CAST(Pdc       , 0x400E0700U) /**< \brief (PDC_UART0 ) Base Address */
#define CHIPID     CAST(Chipid    , 0x400E0740U) /**< \brief (CHIPID    ) Base Address */
#define UART1      CAST(Uart      , 0x400E0800U) /**< \brief (UART1     ) Base Address */
#define EFC        CAST(Efc       , 0x400E0A00U) /**< \brief (EFC       ) Base Address */
#define PIOA       CAST(Pio       , 0x400E0E00U) /**< \brief (PIOA      ) Base Address */
#define PIOB       CAST(Pio       , 0x400E1000U) /**< \brief (PIOB      ) Base Address */
#define RSTC       CAST(Rstc      , 0x400E1400U) /**< \brief (RSTC      ) Base Address */
#define SUPC       CAST(Supc      , 0x400E1410U) /**< \brief (SUPC      ) Base Address */
#define RTT        CAST(Rtt       , 0x400E1430U) /**< \brief (RTT       ) Base Address */
#define WDT        CAST(Wdt       , 0x400E1450U) /**< \brief (WDT       ) Base Address */
#define RTC        CAST(Rtc       , 0x400E1460U) /**< \brief (RTC       ) Base Address */
#define GPBR       CAST(Gpbr      , 0x400E1490U) /**< \brief (GPBR      ) Base Address */
/*@}*/

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAM3N1B */
/* ************************************************************************** */
/** \addtogroup SAM3N1B_pio Peripheral Pio Definitions */
/*@{*/

#include "pio/SAM3N1B.h"
/*@}*/

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAM3N1B */
/* ************************************************************************** */

#define IFLASH_SIZE             0x10000
#define IFLASH_PAGE_SIZE        256
#define IFLASH_LOCK_REGION_SIZE 16384
#define IFLASH_NB_OF_PAGES      256
#define IFLASH_NB_OF_LOCK_BITS  4
#define IRAM_SIZE               0x2000

#define IFLASH_ADDR (0x00400000u) /**< Internal Flash base address */
#define IROM_ADDR   (0x00800000u) /**< Internal ROM base address */
#define IRAM_ADDR   (0x20000000u) /**< Internal RAM base address */

#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* _SAM3N1B_ */
