/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

/* Stack Configuration */
#define STACK_SIZE       0x900     /** Stack size (in DWords) */
__attribute__ ((aligned(8),section(".stack")))
uint32_t pdwStack[STACK_SIZE] ;

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;


/*----------------------------------------------------------------------------
 *        Prototypes
 *----------------------------------------------------------------------------*/

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
extern int main( void ) ;
/** \endcond */
void Reset_Handler( void ) ;
extern void __libc_init_array( void ) ;

/* Default empty handler */
void Dummy_Handler( void ) ;

/* Cortex-M3 core handlers */
extern void NMI_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void HardFault_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void MemManage_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void BusFault_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void UsageFault_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void SVC_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void DebugMon_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void PendSV_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void SysTick_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;

/* Peripherals handlers */
extern void ACC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void ADC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void CRCCU_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void DAC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void EEFC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void MCI_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void PIOA_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void PIOB_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void PIOC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void PMC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void PWM_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void RSTC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void RTC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void RTT_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void SMC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void SPI_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void SSC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void SUPC_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TC0_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TC1_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TC2_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TC3_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TC4_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TC5_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TWI0_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void TWI1_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void UART0_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void UART1_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void USART0_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void USART1_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void USBD_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
extern void WDT_IrqHandler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;

/*------------------------------------------------------------------------------
 *         Exception Table
 *------------------------------------------------------------------------------*/

void* vector_table[] __attribute__ ((section(".vectors"))) = {
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    (IntFunc)(&pdwStack[STACK_SIZE-1]),
    Reset_Handler,

    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0, 0, 0, 0,         /* Reserved */
    SVC_Handler,
    DebugMon_Handler,
    0,                  /* Reserved  */
    PendSV_Handler,
    SysTick_Handler,

    /* Configurable interrupts  */
    SUPC_IrqHandler,    /* 0  Supply Controller */
    RSTC_IrqHandler,    /* 1  Reset Controller */
    RTC_IrqHandler,     /* 2  Real Time Clock */
    RTT_IrqHandler,     /* 3  Real Time Timer */
    WDT_IrqHandler,     /* 4  Watchdog Timer */
    PMC_IrqHandler,     /* 5  PMC */
    EEFC_IrqHandler,    /* 6  EEFC */
    Dummy_Handler,      /* 7  Reserved */
    UART0_IrqHandler,   /* 8  UART0 */
    UART1_IrqHandler,   /* 9  UART1 */
    SMC_IrqHandler,     /* 10 SMC */
    PIOA_IrqHandler,    /* 11 Parallel IO Controller A */
    PIOB_IrqHandler,    /* 12 Parallel IO Controller B */
    PIOC_IrqHandler,    /* 13 Parallel IO Controller C */
    USART0_IrqHandler,  /* 14 USART 0 */
    USART1_IrqHandler,  /* 15 USART 1 */
    Dummy_Handler,      /* 16 Reserved */
    Dummy_Handler,      /* 17 Reserved */
    MCI_IrqHandler,     /* 18 MCI */
    TWI0_IrqHandler,    /* 19 TWI 0 */
    TWI1_IrqHandler,    /* 20 TWI 1 */
    SPI_IrqHandler,     /* 21 SPI */
    SSC_IrqHandler,     /* 22 SSC */
    TC0_IrqHandler,     /* 23 Timer Counter 0 */
    TC1_IrqHandler,     /* 24 Timer Counter 1 */
    TC2_IrqHandler,     /* 25 Timer Counter 2 */
    TC3_IrqHandler,     /* 26 Timer Counter 3 */
    TC4_IrqHandler,     /* 27 Timer Counter 4 */
    TC5_IrqHandler,     /* 28 Timer Counter 5 */
    ADC_IrqHandler,     /* 29 ADC controller */
    DAC_IrqHandler,     /* 30 DAC controller */
    PWM_IrqHandler,     /* 31 PWM */
    CRCCU_IrqHandler,   /* 32 CRC Calculation Unit */
    ACC_IrqHandler,     /* 33 Analog Comparator */
    USBD_IrqHandler,    /* 34 USB Device Port */
    Dummy_Handler       /* 35 not used */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler( void )
{
    uint32_t *pSrc, *pDest ;

    /* Low level Initialize */
    LowLevelInit() ;

    /* Initialize the relocate segment */
    pSrc = &_etext ;
    pDest = &_srelocate ;

    if ( pSrc != pDest )
    {
        for ( ; pDest < &_erelocate ; )
        {
            *pDest++ = *pSrc++ ;
        }
    }

    /* Clear the zero segment */
    for ( pDest = &_szero ; pDest < &_ezero ; )
    {
        *pDest++ = 0;
    }

    /* Set the vector table base address */
    pSrc = (uint32_t *)&_sfixed;
    SCB->VTOR = ( (uint32_t)pSrc & SCB_VTOR_TBLOFF_Msk ) ;

    if ( ((uint32_t)pSrc >= IRAM_ADDR) && ((uint32_t)pSrc < IRAM_ADDR+IRAM_SIZE) )
    {
	    SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos ;
    }

    /* Initialize the C library */
    __libc_init_array() ;

    /* Branch to main function */
    main() ;

    /* Infinite loop */
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for not used irq.
 */
void Dummy_Handler( void )
{
  while ( 1 ) ;
}
