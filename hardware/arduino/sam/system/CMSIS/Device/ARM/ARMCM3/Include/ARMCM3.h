/**************************************************************************//**
 * @file     ARMCM3.h
 * @brief    CMSIS Cortex-M3 Core Peripheral Access Layer Header File
 *           for CM3 Device Series
 * @version  V1.05
 * @date     26. July 2011
 *
 * @note
 * Copyright (C) 2010-2011 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef ARMCM3_H
#define ARMCM3_H

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn         = -14,    /*!<  2 Cortex-M3 Non Maskable Interrupt                  */
  HardFault_IRQn              = -13,    /*!<  3 Cortex-M3 Hard Fault Interrupt                    */
  MemoryManagement_IRQn       = -12,    /*!<  4 Cortex-M3 Memory Management Interrupt             */
  BusFault_IRQn               = -11,    /*!<  5 Cortex-M3 Bus Fault Interrupt                     */
  UsageFault_IRQn             = -10,    /*!<  6 Cortex-M3 Usage Fault Interrupt                   */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

/******  ARMCM3 specific Interrupt Numbers ********************************************************/
  GPIO_IRQn                   = 0       /*!< GPIO Interrupt                                       */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M3 Processor and Core Peripherals */
#define __CM3_REV                 0x0201    /*!< Core Revision r2p1                               */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          3         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */


#include <core_cm3.h>                       /* Cortex-M3 processor and core peripherals           */
#include "system_ARMCM3.h"                  /* System Header                                      */


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

/*--------------------- General Purpose Input and Ouptut ---------------------*/
typedef union
{
  __IO uint32_t WORD;  
  __IO uint8_t  BYTE[4];
} GPIO_Data_TypeDef;

typedef struct
{
  GPIO_Data_TypeDef DATA [256];
  __O uint32_t  DIR;
  uint32_t      RESERVED[3];
  __O uint32_t  IE;
} ARM_GPIO_TypeDef;


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Peripheral and SRAM base address */
#define ARM_SRAM_BASE             ((     uint32_t)0x20000000UL)
#define ARM_PERIPH_BASE           ((     uint32_t)0x40000000UL)

/* Peripheral memory map */
#define ARM_GPIO_BASE              ARM_PERIPH_BASE

#define ARM_GPIO0_BASE            (ARM_GPIO_BASE)
#define ARM_GPIO1_BASE            (ARM_GPIO_BASE       + 0x0800UL)
#define ARM_GPIO2_BASE            (ARM_GPIO_BASE       + 0x1000UL)


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define ARM_GPIO0                ((ARM_GPIO_TypeDef *) ARM_GPIO0_BASE)
#define ARM_GPIO1                ((ARM_GPIO_TypeDef *) ARM_GPIO1_BASE)
#define ARM_GPIO2                ((ARM_GPIO_TypeDef *) ARM_GPIO2_BASE)


#endif  /* ARMCM3_H */
