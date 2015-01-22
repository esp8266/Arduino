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

#include "sam4s.h"

typedef void (*intfunc) (void);
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

void __iar_program_start(void);
int __low_level_init(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M3 core handlers */
#pragma weak NMI_Handler=Dummy_Handler
#pragma weak HardFault_Handler=Dummy_Handler
#pragma weak MemManage_Handler=Dummy_Handler
#pragma weak BusFault_Handler=Dummy_Handler
#pragma weak UsageFault_Handler=Dummy_Handler
#pragma weak SVC_Handler=Dummy_Handler
#pragma weak DebugMon_Handler=Dummy_Handler
#pragma weak PendSV_Handler=Dummy_Handler
#pragma weak SysTick_Handler=Dummy_Handler

/* Peripherals handlers */
#pragma weak SUPC_Handler=Dummy_Handler
#pragma weak RSTC_Handler=Dummy_Handler
#pragma weak RTC_Handler=Dummy_Handler
#pragma weak RTT_Handler=Dummy_Handler
#pragma weak WDT_Handler=Dummy_Handler
#pragma weak PMC_Handler=Dummy_Handler
#pragma weak EFC_Handler=Dummy_Handler
#pragma weak UART0_Handler=Dummy_Handler
#pragma weak UART1_Handler=Dummy_Handler
#ifdef _SAM4S_SMC_INSTANCE_
#pragma weak SMC_Handler=Dummy_Handler
#endif /* _SAM4S_SMC_INSTANCE_ */
#pragma weak PIOA_Handler=Dummy_Handler
#pragma weak PIOB_Handler=Dummy_Handler
#ifdef _SAM4S_PIOC_INSTANCE_
#pragma weak PIOC_Handler=Dummy_Handler
#endif /* _SAM4S_PIOC_INSTANCE_ */
#pragma weak USART0_Handler=Dummy_Handler
#ifdef _SAM4S_USART1_INSTANCE_
#pragma weak USART1_Handler=Dummy_Handler
#endif /* _SAM4S_USART1_INSTANCE_ */
#ifdef _SAM4S_HSMCI_INSTANCE_
#pragma weak HSMCI_Handler=Dummy_Handler
#endif /* _SAM4S_HSMCI_INSTANCE_ */
#pragma weak TWI0_Handler=Dummy_Handler
#pragma weak TWI1_Handler=Dummy_Handler
#pragma weak SPI_Handler=Dummy_Handler
#pragma weak SSC_Handler=Dummy_Handler
#pragma weak TC0_Handler=Dummy_Handler
#pragma weak TC1_Handler=Dummy_Handler
#pragma weak TC2_Handler=Dummy_Handler
#ifdef _SAM4S_TC1_INSTANCE_
#pragma weak TC3_Handler=Dummy_Handler
#pragma weak TC4_Handler=Dummy_Handler
#pragma weak TC5_Handler=Dummy_Handler
#endif /* _SAM4S_TC1_INSTANCE_ */
#pragma weak ADC_Handler=Dummy_Handler
#pragma weak DACC_Handler=Dummy_Handler
#pragma weak PWM_Handler=Dummy_Handler
#pragma weak CRCCU_Handler=Dummy_Handler
#pragma weak ACC_Handler=Dummy_Handler
#pragma weak UDP_Handler=Dummy_Handler

/* Exception Table */

/* The name "__vector_table" has special meaning for C-SPY: */
/* it is where the SP start value is found, and the NVIC vector */
/* table register (VTOR) is initialized to this address if != 0 */

#pragma section = ".intvec"
#pragma location = ".intvec"
const DeviceVectors __vector_table[] = {
	(void*) (&__cstack_end__),
	(void*) Reset_Handler,

	(void*) NMI_Handler,
	(void*) HardFault_Handler,
	(void*) MemManage_Handler,
	(void*) BusFault_Handler,
	(void*) UsageFault_Handler,
	(void*) (0UL),           /* Reserved */
	(void*) (0UL),           /* Reserved */
	(void*) (0UL),           /* Reserved */
	(void*) (0UL),           /* Reserved */
	(void*) SVC_Handler,
	(void*) DebugMon_Handler,
	(void*) (0UL),           /* Reserved */
	(void*) PendSV_Handler,
	(void*) SysTick_Handler,

	/* Configurable interrupts */
	(void*) SUPC_Handler,    /* 0  Supply Controller */
	(void*) RSTC_Handler,    /* 1  Reset Controller */
	(void*) RTC_Handler,     /* 2  Real Time Clock */
	(void*) RTT_Handler,     /* 3  Real Time Timer */
	(void*) WDT_Handler,     /* 4  Watchdog Timer */
	(void*) PMC_Handler,     /* 5  PMC */
	(void*) EFC_Handler,     /* 6  EEFC */
	(void*) (0UL),           /* 7  Reserved */
	(void*) UART0_Handler,   /* 8  UART0 */
	(void*) UART1_Handler,   /* 9  UART1 */
#ifdef _SAM4S_SMC_INSTANCE_
	(void*) SMC_Handler,     /* 10 SMC */
#else
	(void*) (0UL),           /* 10 Reserved */
#endif /* _SAM4S_SMC_INSTANCE_ */
	(void*) PIOA_Handler,    /* 11 Parallel IO Controller A */
	(void*) PIOB_Handler,    /* 12 Parallel IO Controller B */
#ifdef _SAM4S_PIOC_INSTANCE_
	(void*) PIOC_Handler,    /* 13 Parallel IO Controller C */
#else
	(void*) (0UL),           /* 13 Reserved */
#endif /* _SAM4S_PIOC_INSTANCE_ */
	(void*) USART0_Handler,  /* 14 USART 0 */
#ifdef _SAM4S_USART1_INSTANCE_
	(void*) USART1_Handler,  /* 15 USART 1 */
#else
	(void*) (0UL),           /* 15 Reserved */
#endif /* _SAM4S_USART1_INSTANCE_ */
	(void*) (0UL),           /* 16 Reserved */
	(void*) (0UL),           /* 17 Reserved */
#ifdef _SAM4S_HSMCI_INSTANCE_
	(void*) HSMCI_Handler,   /* 18 MCI */
#else
	(void*) (0UL),           /* 18 Reserved */
#endif /* _SAM4S_HSMCI_INSTANCE_ */
	(void*) TWI0_Handler,    /* 19 TWI 0 */
	(void*) TWI1_Handler,    /* 20 TWI 1 */
	(void*) SPI_Handler,     /* 21 SPI */
	(void*) SSC_Handler,     /* 22 SSC */
	(void*) TC0_Handler,     /* 23 Timer Counter 0 */
	(void*) TC1_Handler,     /* 24 Timer Counter 1 */
	(void*) TC2_Handler,     /* 25 Timer Counter 2 */
#ifdef _SAM4S_TC1_INSTANCE_
	(void*) TC3_Handler,     /* 26 Timer Counter 3 */
	(void*) TC4_Handler,     /* 27 Timer Counter 4 */
	(void*) TC5_Handler,     /* 28 Timer Counter 5 */
#else
	(void*) (0UL),           /* 26 Reserved */
	(void*) (0UL),           /* 27 Reserved */
	(void*) (0UL),           /* 28 Reserved */
#endif /* _SAM4S_TC1_INSTANCE_ */
	(void*) ADC_Handler,     /* 29 ADC controller */
	(void*) DACC_Handler,    /* 30 DAC controller */
	(void*) PWM_Handler,     /* 31 PWM */
	(void*) CRCCU_Handler,   /* 32 CRC Calculation Unit */
	(void*) ACC_Handler,     /* 33 Analog Comparator */
	(void*) UDP_Handler      /* 34 USB Device Port */
};

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
int __low_level_init(void)
{
	uint32_t *pSrc = __section_begin(".intvec");

	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

	if (((uint32_t) pSrc >= IRAM_ADDR) && ((uint32_t) pSrc < (uint32_t) IRAM_ADDR + (uint32_t) IRAM_SIZE)) {
		SCB->VTOR |= (1UL) << SCB_VTOR_TBLBASE_Pos;
	}

	return 1; /* if return 0, the data sections will not be initialized */
}

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
void Reset_Handler(void)
{
	__iar_program_start();
}
