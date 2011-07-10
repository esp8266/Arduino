/**************************************************************************//**
 * @file     system_ARMCM4.c
 * @brief    CMSIS Cortex-M4 Device System Source File
 *           for CM4 Device Series
 * @version  V1.04
 * @date     06. December 2010
 *
 * @note
 * Copyright (C) 2010 ARM Limited. All rights reserved.
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

#include "ARMCM4.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __HSI             ( 8000000UL)
#define __XTAL            (12000000UL)    /* Oscillator frequency             */

#define __SYSTEM_CLOCK    (6*__XTAL)


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __SYSTEM_CLOCK;/*!< System Clock Frequency (Core Clock)*/


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
  SystemCoreClock = __SYSTEM_CLOCK;
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
  #if __FPU_PRESENT == 1
    SCB->CPACR |= (0xF << 20);               /* cp10 and cp11 set in CPACR    */
  #endif

  SystemCoreClock = __SYSTEM_CLOCK;

#ifdef __USE_GPIO
  GPIO0->DATA[0].WORD = 0;
  GPIO0->IE = 0;
  GPIO0->DIR = 0xff83;
  
  GPIO1->DATA[0].WORD = 0;
  GPIO1->IE = 0;
  GPIO1->DIR = 0;
  
  GPIO2->DATA[0].WORD = 0;
  GPIO2->IE = 0;
  GPIO2->DIR = 0;
#endif
}
