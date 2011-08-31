/* %ATMEL_LICENCE% */

#ifndef _SAM3X4E_
#define _SAM3X4E_

/** \addtogroup SAM3X4E_definitions SAM3X4E definitions
  This file defines all structures and symbols for SAM3X4E:
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
/*   CMSIS DEFINITIONS FOR SAM3X4E */
/* ************************************************************************** */
/** \addtogroup SAM3X4E_cmsis CMSIS Definitions */
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
/******  SAM3X4E specific Interrupt Numbers *********************************/
  
  SUPC_IRQn            =  0, /**<  0 SAM3X4E Supply Controller (SUPC) */
  RSTC_IRQn            =  1, /**<  1 SAM3X4E Reset Controller (RSTC) */
  RTC_IRQn             =  2, /**<  2 SAM3X4E Real Time Clock (RTC) */
  RTT_IRQn             =  3, /**<  3 SAM3X4E Real Time Timer (RTT) */
  WDT_IRQn             =  4, /**<  4 SAM3X4E Watchdog Timer (WDT) */
  PMC_IRQn             =  5, /**<  5 SAM3X4E Power Management Controller (PMC) */
  EFC0_IRQn            =  6, /**<  6 SAM3X4E Enhanced Flash Controller 0 (EFC0) */
  EFC1_IRQn            =  7, /**<  7 SAM3X4E Enhanced Flash Controller 1 (EFC1) */
  UART_IRQn            =  8, /**<  8 SAM3X4E Universal Asynchronous Receiver Transceiver (UART) */
  SMC_IRQn             =  9, /**<  9 SAM3X4E Static Memory Controller (SMC) */
  SDRAMC_IRQn          = 10, /**< 10 SAM3X4E Synchronous Dynamic RAM Controller (SDRAMC) */
  PIOA_IRQn            = 11, /**< 11 SAM3X4E Parallel I/O Controller A, (PIOA) */
  PIOB_IRQn            = 12, /**< 12 SAM3X4E Parallel I/O Controller B (PIOB) */
  PIOC_IRQn            = 13, /**< 13 SAM3X4E Parallel I/O Controller C (PIOC) */
  PIOD_IRQn            = 14, /**< 14 SAM3X4E Parallel I/O Controller D (PIOD) */
  PIOE_IRQn            = 15, /**< 15 SAM3X4E Parallel I/O Controller E (PIOE) */
  PIOF_IRQn            = 16, /**< 16 SAM3X4E Parallel I/O Controller F (PIOF) */
  USART0_IRQn          = 17, /**< 17 SAM3X4E USART 0 (USART0) */
  USART1_IRQn          = 18, /**< 18 SAM3X4E USART 1 (USART1) */
  USART2_IRQn          = 19, /**< 19 SAM3X4E USART 2 (USART2) */
  USART3_IRQn          = 20, /**< 20 SAM3X4E USART 3 (USART3) */
  HSMCI_IRQn           = 21, /**< 21 SAM3X4E Multimedia Card Interface (HSMCI) */
  TWI0_IRQn            = 22, /**< 22 SAM3X4E Two-Wire Interface 0 (TWI0) */
  TWI1_IRQn            = 23, /**< 23 SAM3X4E Two-Wire Interface 1 (TWI1) */
  SPI0_IRQn            = 24, /**< 24 SAM3X4E Serial Peripheral Interface (SPI0) */
  SPI1_IRQn            = 25, /**< 25 SAM3X4E Serial Peripheral Interface (SPI1) */
  SSC_IRQn             = 26, /**< 26 SAM3X4E Synchronous Serial Controller (SSC) */
  TC0_IRQn             = 27, /**< 27 SAM3X4E Timer Counter 0 (TC0) */
  TC1_IRQn             = 28, /**< 28 SAM3X4E Timer Counter 1 (TC1) */
  TC2_IRQn             = 29, /**< 29 SAM3X4E Timer Counter 2 (TC2) */
  TC3_IRQn             = 30, /**< 30 SAM3X4E Timer Counter 3 (TC3) */
  TC4_IRQn             = 31, /**< 31 SAM3X4E Timer Counter 4 (TC4) */
  TC5_IRQn             = 32, /**< 32 SAM3X4E Timer Counter 5 (TC5) */
  TC6_IRQn             = 33, /**< 33 SAM3X4E Timer Counter 6 (TC6) */
  TC7_IRQn             = 34, /**< 34 SAM3X4E Timer Counter 7 (TC7) */
  TC8_IRQn             = 35, /**< 35 SAM3X4E Timer Counter 8 (TC8) */
  PWM_IRQn             = 36, /**< 36 SAM3X4E Pulse Width Modulation Controller (PWM) */
  ADC_IRQn             = 37, /**< 37 SAM3X4E ADC Controller (ADC) */
  DACC_IRQn            = 38, /**< 38 SAM3X4E DAC Controller (DACC) */
  DMAC_IRQn            = 39, /**< 39 SAM3X4E DMA Controller (DMAC) */
  UOTGHS_IRQn          = 40, /**< 40 SAM3X4E USB OTG High Speed (UOTGHS) */
  TRNG_IRQn            = 41, /**< 41 SAM3X4E True Random Number Generator (TRNG) */
  EMAC_IRQn            = 42, /**< 42 SAM3X4E Ethernet MAC (EMAC) */
  CAN0_IRQn            = 43, /**< 43 SAM3X4E CAN Controller 0 (CAN0) */
  CAN1_IRQn            = 44  /**< 44 SAM3X4E CAN Controller 1 (CAN1) */
} IRQn_Type;

/**
 * \brief Configuration of the Cortex-M3 Processor and Core Peripherals 
 */

#define __MPU_PRESENT          1 /**< SAM3X4E does provide a MPU */
#define __NVIC_PRIO_BITS       4 /**< SAM3X4E uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0 /**< Set to 1 if different SysTick Config is used */

/*
 * \brief CMSIS includes
 */

#include <core_cm3.h>

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAM3X4E */
/* ************************************************************************** */
/** \addtogroup SAM3X4E_api Peripheral Software API */
/*@{*/

#include "component/ADC.h"
#include "component/CAN.h"
#include "component/CHIPID.h"
#include "component/DACC.h"
#include "component/DMAC.h"
#include "component/EFC.h"
#include "component/EMAC.h"
#include "component/GPBR.h"
#include "component/HSMCI.h"
#include "component/MATRIX.h"
#include "component/PDC.h"
#include "component/PIO.h"
#include "component/PMC.h"
#include "component/PWM.h"
#include "component/RSTC.h"
#include "component/RTC.h"
#include "component/RTT.h"
#include "component/SMC.h"
#include "component/SPI.h"
#include "component/SSC.h"
#include "component/SUPC.h"
#include "component/TC.h"
#include "component/TRNG.h"
#include "component/TWI.h"
#include "component/UART.h"
#include "component/UOTGHS.h"
#include "component/USART.h"
#include "component/WDT.h"
/*@}*/

/* ************************************************************************** */
/*   REGISTER ACCESS DEFINITIONS FOR SAM3X4E */
/* ************************************************************************** */
/** \addtogroup SAM3X4E_reg Registers Access Definitions */
/*@{*/

#include "instance/HSMCI.h"
#include "instance/SSC.h"
#include "instance/SPI0.h"
#include "instance/TC0.h"
#include "instance/TC1.h"
#include "instance/TC2.h"
#include "instance/TWI0.h"
#include "instance/TWI1.h"
#include "instance/PWM.h"
#include "instance/USART0.h"
#include "instance/USART1.h"
#include "instance/USART2.h"
#include "instance/USART3.h"
#include "instance/UOTGHS.h"
#include "instance/EMAC.h"
#include "instance/CAN0.h"
#include "instance/CAN1.h"
#include "instance/TRNG.h"
#include "instance/ADC.h"
#include "instance/DMAC.h"
#include "instance/DACC.h"
#include "instance/SMC.h"
#include "instance/MATRIX.h"
#include "instance/PMC.h"
#include "instance/UART.h"
#include "instance/CHIPID.h"
#include "instance/EFC0.h"
#include "instance/EFC1.h"
#include "instance/PIOA.h"
#include "instance/PIOB.h"
#include "instance/PIOC.h"
#include "instance/PIOD.h"
#include "instance/RSTC.h"
#include "instance/SUPC.h"
#include "instance/RTT.h"
#include "instance/WDT.h"
#include "instance/RTC.h"
#include "instance/GPBR.h"
/*@}*/

/* ************************************************************************** */
/*   PERIPHERAL ID DEFINITIONS FOR SAM3X4E */
/* ************************************************************************** */
/** \addtogroup SAM3X4E_id Peripheral Ids Definitions */
/*@{*/

#define ID_SUPC   ( 0) /**< \brief Supply Controller (SUPC) */
#define ID_RSTC   ( 1) /**< \brief Reset Controller (RSTC) */
#define ID_RTC    ( 2) /**< \brief Real Time Clock (RTC) */
#define ID_RTT    ( 3) /**< \brief Real Time Timer (RTT) */
#define ID_WDT    ( 4) /**< \brief Watchdog Timer (WDT) */
#define ID_PMC    ( 5) /**< \brief Power Management Controller (PMC) */
#define ID_EFC0   ( 6) /**< \brief Enhanced Flash Controller 0 (EFC0) */
#define ID_EFC1   ( 7) /**< \brief Enhanced Flash Controller 1 (EFC1) */
#define ID_UART   ( 8) /**< \brief Universal Asynchronous Receiver Transceiver (UART) */
#define ID_SMC    ( 9) /**< \brief Static Memory Controller (SMC) */
#define ID_SDRAMC (10) /**< \brief Synchronous Dynamic RAM Controller (SDRAMC) */
#define ID_PIOA   (11) /**< \brief Parallel I/O Controller A, (PIOA) */
#define ID_PIOB   (12) /**< \brief Parallel I/O Controller B (PIOB) */
#define ID_PIOC   (13) /**< \brief Parallel I/O Controller C (PIOC) */
#define ID_PIOD   (14) /**< \brief Parallel I/O Controller D (PIOD) */
#define ID_PIOE   (15) /**< \brief Parallel I/O Controller E (PIOE) */
#define ID_PIOF   (16) /**< \brief Parallel I/O Controller F (PIOF) */
#define ID_USART0 (17) /**< \brief USART 0 (USART0) */
#define ID_USART1 (18) /**< \brief USART 1 (USART1) */
#define ID_USART2 (19) /**< \brief USART 2 (USART2) */
#define ID_USART3 (20) /**< \brief USART 3 (USART3) */
#define ID_HSMCI  (21) /**< \brief Multimedia Card Interface (HSMCI) */
#define ID_TWI0   (22) /**< \brief Two-Wire Interface 0 (TWI0) */
#define ID_TWI1   (23) /**< \brief Two-Wire Interface 1 (TWI1) */
#define ID_SPI0   (24) /**< \brief Serial Peripheral Interface (SPI0) */
#define ID_SPI1   (25) /**< \brief Serial Peripheral Interface (SPI1) */
#define ID_SSC    (26) /**< \brief Synchronous Serial Controller (SSC) */
#define ID_TC0    (27) /**< \brief Timer Counter 0 (TC0) */
#define ID_TC1    (28) /**< \brief Timer Counter 1 (TC1) */
#define ID_TC2    (29) /**< \brief Timer Counter 2 (TC2) */
#define ID_TC3    (30) /**< \brief Timer Counter 3 (TC3) */
#define ID_TC4    (31) /**< \brief Timer Counter 4 (TC4) */
#define ID_TC5    (32) /**< \brief Timer Counter 5 (TC5) */
#define ID_TC6    (33) /**< \brief Timer Counter 6 (TC6) */
#define ID_TC7    (34) /**< \brief Timer Counter 7 (TC7) */
#define ID_TC8    (35) /**< \brief Timer Counter 8 (TC8) */
#define ID_PWM    (36) /**< \brief Pulse Width Modulation Controller (PWM) */
#define ID_ADC    (37) /**< \brief ADC Controller (ADC) */
#define ID_DACC   (38) /**< \brief DAC Controller (DACC) */
#define ID_DMAC   (39) /**< \brief DMA Controller (DMAC) */
#define ID_UOTGHS (40) /**< \brief USB OTG High Speed (UOTGHS) */
#define ID_TRNG   (41) /**< \brief True Random Number Generator (TRNG) */
#define ID_EMAC   (42) /**< \brief Ethernet MAC (EMAC) */
#define ID_CAN0   (43) /**< \brief CAN Controller 0 (CAN0) */
#define ID_CAN1   (44) /**< \brief CAN Controller 1 (CAN1) */
/*@}*/

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAM3X4E */
/* ************************************************************************** */
/** \addtogroup SAM3X4E_base Peripheral Base Address Definitions */
/*@{*/

#define HSMCI      CAST(Hsmci     , 0x40000000U) /**< \brief (HSMCI     ) Base Address */
#define SSC        CAST(Ssc       , 0x40004000U) /**< \brief (SSC       ) Base Address */
#define SPI0       CAST(Spi       , 0x40008000U) /**< \brief (SPI0      ) Base Address */
#define TC0        CAST(Tc        , 0x40080000U) /**< \brief (TC0       ) Base Address */
#define TC1        CAST(Tc        , 0x40084000U) /**< \brief (TC1       ) Base Address */
#define TC2        CAST(Tc        , 0x40088000U) /**< \brief (TC2       ) Base Address */
#define TWI0       CAST(Twi       , 0x4008C000U) /**< \brief (TWI0      ) Base Address */
#define PDC_TWI0   CAST(Pdc       , 0x4008C100U) /**< \brief (PDC_TWI0  ) Base Address */
#define TWI1       CAST(Twi       , 0x40090000U) /**< \brief (TWI1      ) Base Address */
#define PDC_TWI1   CAST(Pdc       , 0x40090100U) /**< \brief (PDC_TWI1  ) Base Address */
#define PWM        CAST(Pwm       , 0x40094000U) /**< \brief (PWM       ) Base Address */
#define PDC_PWM    CAST(Pdc       , 0x40094100U) /**< \brief (PDC_PWM   ) Base Address */
#define USART0     CAST(Usart     , 0x40098000U) /**< \brief (USART0    ) Base Address */
#define PDC_USART0 CAST(Pdc       , 0x40098100U) /**< \brief (PDC_USART0) Base Address */
#define USART1     CAST(Usart     , 0x4009C000U) /**< \brief (USART1    ) Base Address */
#define PDC_USART1 CAST(Pdc       , 0x4009C100U) /**< \brief (PDC_USART1) Base Address */
#define USART2     CAST(Usart     , 0x400A0000U) /**< \brief (USART2    ) Base Address */
#define PDC_USART2 CAST(Pdc       , 0x400A0100U) /**< \brief (PDC_USART2) Base Address */
#define USART3     CAST(Usart     , 0x400A4000U) /**< \brief (USART3    ) Base Address */
#define PDC_USART3 CAST(Pdc       , 0x400A4100U) /**< \brief (PDC_USART3) Base Address */
#define UOTGHS     CAST(Uotghs    , 0x400AC000U) /**< \brief (UOTGHS    ) Base Address */
#define EMAC       CAST(Emac      , 0x400B0000U) /**< \brief (EMAC      ) Base Address */
#define CAN0       CAST(Can       , 0x400B4000U) /**< \brief (CAN0      ) Base Address */
#define CAN1       CAST(Can       , 0x400B8000U) /**< \brief (CAN1      ) Base Address */
#define TRNG       CAST(Trng      , 0x400BC000U) /**< \brief (TRNG      ) Base Address */
#define ADC        CAST(Adc       , 0x400C0000U) /**< \brief (ADC       ) Base Address */
#define PDC_ADC    CAST(Pdc       , 0x400C0100U) /**< \brief (PDC_ADC   ) Base Address */
#define DMAC       CAST(Dmac      , 0x400C4000U) /**< \brief (DMAC      ) Base Address */
#define DACC       CAST(Dacc      , 0x400C8000U) /**< \brief (DACC      ) Base Address */
#define PDC_DACC   CAST(Pdc       , 0x400C8100U) /**< \brief (PDC_DACC  ) Base Address */
#define SMC        CAST(Smc       , 0x400E0000U) /**< \brief (SMC       ) Base Address */
#define MATRIX     CAST(Matrix    , 0x400E0400U) /**< \brief (MATRIX    ) Base Address */
#define PMC        CAST(Pmc       , 0x400E0600U) /**< \brief (PMC       ) Base Address */
#define UART       CAST(Uart      , 0x400E0800U) /**< \brief (UART      ) Base Address */
#define PDC_UART   CAST(Pdc       , 0x400E0900U) /**< \brief (PDC_UART  ) Base Address */
#define CHIPID     CAST(Chipid    , 0x400E0940U) /**< \brief (CHIPID    ) Base Address */
#define EFC0       CAST(Efc       , 0x400E0A00U) /**< \brief (EFC0      ) Base Address */
#define EFC1       CAST(Efc       , 0x400E0C00U) /**< \brief (EFC1      ) Base Address */
#define PIOA       CAST(Pio       , 0x400E0E00U) /**< \brief (PIOA      ) Base Address */
#define PIOB       CAST(Pio       , 0x400E1000U) /**< \brief (PIOB      ) Base Address */
#define PIOC       CAST(Pio       , 0x400E1200U) /**< \brief (PIOC      ) Base Address */
#define PIOD       CAST(Pio       , 0x400E1400U) /**< \brief (PIOD      ) Base Address */
#define RSTC       CAST(Rstc      , 0x400E1A00U) /**< \brief (RSTC      ) Base Address */
#define SUPC       CAST(Supc      , 0x400E1A10U) /**< \brief (SUPC      ) Base Address */
#define RTT        CAST(Rtt       , 0x400E1A30U) /**< \brief (RTT       ) Base Address */
#define WDT        CAST(Wdt       , 0x400E1A50U) /**< \brief (WDT       ) Base Address */
#define RTC        CAST(Rtc       , 0x400E1A60U) /**< \brief (RTC       ) Base Address */
#define GPBR       CAST(Gpbr      , 0x400E1A90U) /**< \brief (GPBR      ) Base Address */
/*@}*/

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAM3X4E */
/* ************************************************************************** */
/** \addtogroup SAM3X4E_pio Peripheral Pio Definitions */
/*@{*/

#include "pio/SAM3X4E.h"
/*@}*/

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAM3X4E */
/* ************************************************************************** */

#define IFLASH0_SIZE             0x20000
#define IFLASH0_PAGE_SIZE        256
#define IFLASH0_LOCK_REGION_SIZE 16384
#define IFLASH0_NB_OF_PAGES      512
#define IFLASH1_SIZE             0x20000
#define IFLASH1_PAGE_SIZE        256
#define IFLASH1_LOCK_REGION_SIZE 16384
#define IFLASH1_NB_OF_PAGES      512
#define IRAM0_SIZE               0x8000
#define IRAM1_SIZE               0x8000
#define NFCRAM_SIZE              0x1000
#define IFLASH_SIZE              0x40000
#define IRAM_SIZE                0x10000

#define IFLASH0_ADDR    (0x00080000u) /**< Internal Flash 0 base address */
#if defined IFLASH0_SIZE
#define IFLASH1_ADDR    (IFLASH0_ADDR+IFLASH0_SIZE) /**< Internal Flash 1 base address */
#endif
#define IROM_ADDR       (0x00100000u) /**< Internal ROM base address */
#define IRAM0_ADDR      (0x20000000u) /**< Internal RAM 0 base address */
#define IRAM1_ADDR      (0x20080000u) /**< Internal RAM 1 base address */
#define NFC_RAM_ADDR    (0x20100000u) /**< NAND Flash Controller RAM base address */
#define UOTGHS_RAM_ADDR (0x20180000u) /**< USB On-The-Go Interface RAM base address */
#define EBI_CS0_ADDR    (0x60000000u) /**< EBI Chip Select 0 base address */
#define EBI_CS1_ADDR    (0x61000000u) /**< EBI Chip Select 1 base address */
#define EBI_CS2_ADDR    (0x62000000u) /**< EBI Chip Select 2 base address */
#define EBI_CS3_ADDR    (0x63000000u) /**< EBI Chip Select 3 base address */
#define EBI_CS4_ADDR    (0x64000000u) /**< EBI Chip Select 4 base address */
#define EBI_CS5_ADDR    (0x65000000u) /**< EBI Chip Select 5 base address */
#define EBI_CS6_ADDR    (0x66000000u) /**< EBI Chip Select 6 base address */
#define EBI_CS7_ADDR    (0x67000000u) /**< EBI Chip Select 7 base address */

#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* _SAM3X4E_ */
