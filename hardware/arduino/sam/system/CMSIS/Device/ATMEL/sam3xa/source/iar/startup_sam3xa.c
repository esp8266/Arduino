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

#include "sam3xa.h"

/* Initialize segments */
extern uint32_t __cstack_start__;
extern uint32_t __cstack_end__;


extern void __iar_program_start(void);
extern int __low_level_init(void);

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
#pragma weak EFC0_Handler=Dummy_Handler
#pragma weak EFC1_Handler=Dummy_Handler
#pragma weak UART_Handler=Dummy_Handler
#ifdef _SAM3XA_SMC_INSTANCE_
#pragma weak SMC_Handler=Dummy_Handler
#endif /* _SAM3XA_SMC_INSTANCE_ */
#ifdef _SAM3XA_SDRAMC_INSTANCE_
#pragma weak SDRAMC_Handler=Dummy_Handler
#endif /* _SAM3XA_SDRAMC_INSTANCE_ */
#pragma weak PIOA_Handler=Dummy_Handler
#pragma weak PIOB_Handler=Dummy_Handler
#ifdef _SAM3XA_PIOC_INSTANCE_
#pragma weak PIOC_Handler=Dummy_Handler
#endif /* _SAM3XA_PIOC_INSTANCE_ */
#ifdef _SAM3XA_PIOD_INSTANCE_
#pragma weak PIOD_Handler=Dummy_Handler
#endif /* _SAM3XA_PIOD_INSTANCE_ */
#ifdef _SAM3XA_PIOE_INSTANCE_
#pragma weak PIOE_Handler=Dummy_Handler
#endif /* _SAM3XA_PIOE_INSTANCE_ */
#ifdef _SAM3XA_PIOF_INSTANCE_
#pragma weak PIOF_Handler=Dummy_Handler
#endif /* _SAM3XA_PIOF_INSTANCE_ */
#pragma weak USART0_Handler=Dummy_Handler
#pragma weak USART1_Handler=Dummy_Handler
#pragma weak USART2_Handler=Dummy_Handler
#ifdef _SAM3XA_USART3_INSTANCE_
#pragma weak USART3_Handler=Dummy_Handler
#endif /* _SAM3XA_USART3_INSTANCE_ */
#pragma weak HSMCI_Handler=Dummy_Handler
#pragma weak TWI0_Handler=Dummy_Handler
#pragma weak TWI1_Handler=Dummy_Handler
#pragma weak SPI0_Handler=Dummy_Handler
#ifdef _SAM3XA_SPI1_INSTANCE_
#pragma weak SPI1_Handler=Dummy_Handler
#endif /* _SAM3XA_SPI1_INSTANCE_ */
#pragma weak SSC_Handler=Dummy_Handler
#pragma weak TC0_Handler=Dummy_Handler
#pragma weak TC1_Handler=Dummy_Handler
#pragma weak TC2_Handler=Dummy_Handler
#pragma weak TC3_Handler=Dummy_Handler
#pragma weak TC4_Handler=Dummy_Handler
#pragma weak TC5_Handler=Dummy_Handler
#ifdef _SAM3XA_TC2_INSTANCE_
#pragma weak TC6_Handler=Dummy_Handler
#pragma weak TC7_Handler=Dummy_Handler
#pragma weak TC8_Handler=Dummy_Handler
#endif /* _SAM3XA_TC2_INSTANCE_ */
#pragma weak PWM_Handler=Dummy_Handler
#pragma weak ADC_Handler=Dummy_Handler
#pragma weak DACC_Handler=Dummy_Handler
#pragma weak DMAC_Handler=Dummy_Handler
#pragma weak UOTGHS_Handler=Dummy_Handler
#pragma weak TRNG_Handler=Dummy_Handler
#ifdef _SAM3XA_EMAC_INSTANCE_
#pragma weak EMAC_Handler=Dummy_Handler
#endif /* _SAM3XA_EMAC_INSTANCE_ */
#pragma weak CAN0_Handler=Dummy_Handler
#pragma weak CAN1_Handler=Dummy_Handler

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
	(void*) SUPC_Handler,    /*  0  Supply Controller */
	(void*) RSTC_Handler,    /*  1  Reset Controller */
	(void*) RTC_Handler,     /*  2  Real Time Clock */
	(void*) RTT_Handler,     /*  3  Real Time Timer */
	(void*) WDT_Handler,     /*  4  Watchdog Timer */
	(void*) PMC_Handler,     /*  5  PMC */
	(void*) EFC0_Handler,    /*  6  EFC 0 */
	(void*) EFC1_Handler,    /*  7  EFC 1 */
	(void*) UART_Handler,    /*  8  UART */
#ifdef _SAM3XA_SMC_INSTANCE_
	(void*) SMC_Handler,     /*  9  SMC */
#else
	(void*) (0UL),           /*  9 Reserved */
#endif /* _SAM3XA_SMC_INSTANCE_ */
#ifdef _SAM3XA_SDRAMC_INSTANCE_
	(void*) SDRAMC_Handler,  /* 10  SDRAMC */
#else
	(void*) (0UL),           /* 10 Reserved */
#endif /* _SAM3XA_SDRAMC_INSTANCE_ */
	(void*) PIOA_Handler,    /* 11 Parallel IO Controller A */
	(void*) PIOB_Handler,    /* 12 Parallel IO Controller B */
#ifdef _SAM3XA_PIOC_INSTANCE_
	(void*) PIOC_Handler,    /* 13 Parallel IO Controller C */
#else
	(void*) (0UL),           /* 13 Reserved */
#endif /* _SAM3XA_PIOC_INSTANCE_ */
#ifdef _SAM3XA_PIOD_INSTANCE_
	(void*) PIOD_Handler,    /* 14 Parallel IO Controller D */
#else
	(void*) (0UL),           /* 14 Reserved */
#endif /* _SAM3XA_PIOD_INSTANCE_ */
#ifdef _SAM3XA_PIOE_INSTANCE_
	(void*) PIOE_Handler,    /* 15 Parallel IO Controller E */
#else
	(void*) (0UL),           /* 15 Reserved */
#endif /* _SAM3XA_PIOE_INSTANCE_ */
#ifdef _SAM3XA_PIOF_INSTANCE_
	(void*) PIOF_Handler,    /* 16 Parallel IO Controller F */
#else
	(void*) (0UL),           /* 16 Reserved */
#endif /* _SAM3XA_PIOF_INSTANCE_ */
	(void*) USART0_Handler,  /* 17 USART 0 */
	(void*) USART1_Handler,  /* 18 USART 1 */
	(void*) USART2_Handler,  /* 19 USART 2 */
#ifdef _SAM3XA_USART3_INSTANCE_
	(void*) USART3_Handler,  /* 20 USART 3 */
#else
	(void*) (0UL),           /* 20 Reserved */
#endif /* _SAM3XA_USART3_INSTANCE_ */
	(void*) HSMCI_Handler,   /* 21 MCI */
	(void*) TWI0_Handler,    /* 22 TWI 0 */
	(void*) TWI1_Handler,    /* 23 TWI 1 */
	(void*) SPI0_Handler,    /* 24 SPI 0 */
#ifdef _SAM3XA_SPI1_INSTANCE_
	(void*) SPI1_Handler,    /* 25 SPI 1 */
#else
	(void*) (0UL),           /* 25 Reserved */
#endif /* _SAM3XA_SPI1_INSTANCE_ */
	(void*) SSC_Handler,     /* 26 SSC */
	(void*) TC0_Handler,     /* 27 Timer Counter 0 */
	(void*) TC1_Handler,     /* 28 Timer Counter 1 */
	(void*) TC2_Handler,     /* 29 Timer Counter 2 */
	(void*) TC3_Handler,     /* 30 Timer Counter 3 */
	(void*) TC4_Handler,     /* 31 Timer Counter 4 */
	(void*) TC5_Handler,     /* 32 Timer Counter 5 */
#ifdef _SAM3XA_TC2_INSTANCE_
	(void*) TC6_Handler,     /* 33 Timer Counter 6 */
	(void*) TC7_Handler,     /* 34 Timer Counter 7 */
	(void*) TC8_Handler,     /* 35 Timer Counter 8 */
#else
	(void*) (0UL),           /* 33 Reserved */
	(void*) (0UL),           /* 34 Reserved */
	(void*) (0UL),           /* 35 Reserved */
#endif /* _SAM3XA_TC2_INSTANCE_ */
	(void*) PWM_Handler,     /* 36 PWM */
	(void*) ADC_Handler,     /* 37 ADC controller */
	(void*) DACC_Handler,    /* 38 DAC controller */
	(void*) DMAC_Handler,    /* 39 DMA Controller */
	(void*) UOTGHS_Handler,  /* 40 USB OTG High Speed */
	(void*) TRNG_Handler,    /* 41 True Random Number Generator */
#ifdef _SAM3XA_EMAC_INSTANCE_
	(void*) EMAC_Handler,    /* 42 Ethernet MAC */
#else
	(void*) (0UL),           /* 42 Reserved */
#endif /* _SAM3XA_EMAC_INSTANCE_ */
	(void*) CAN0_Handler,    /* 43 CAN Controller 0 */
	(void*) CAN1_Handler     /* 44 CAN Controller 1 */
};

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
int __low_level_init(void)
{
	uint32_t *pSrc = __section_begin(".intvec");

	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

	if (((uint32_t) pSrc >= IRAM0_ADDR) && ((uint32_t) pSrc < NFC_RAM_ADDR)) {
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

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
	while (1) {
	}
}

