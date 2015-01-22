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

#include "sam3u.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M3 core handlers */
void NMI_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MemManage_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BusFault_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UsageFault_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DebugMon_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void SUPC_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RSTC_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTT_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PMC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EFC0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EFC1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SMC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOA_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOB_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef _SAM3U_PIOC_INSTANCE_
void PIOC_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif /* _SAM3U_PIOC_INSTANCE_ */
void USART0_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART1_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART2_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef _SAM3U_USART3_INSTANCE_
void USART3_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif /* _SAM3U_USART3_INSTANCE_ */
void HSMCI_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWI0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWI1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SPI_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SSC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PWM_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC12B_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DMAC_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UDPHS_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {

	/* Configure Initial Stack Pointer, using linker-generated symbols */
	(void*) (&_estack),
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
	(void*) EFC0_Handler,    /* 6  EFC 0 */
	(void*) EFC1_Handler,    /* 7  EFC 1 */
	(void*) UART_Handler,    /* 8  UART */
	(void*) SMC_Handler,     /* 9  SMC */
	(void*) PIOA_Handler,    /* 10 Parallel IO Controller A */
	(void*) PIOB_Handler,    /* 11 Parallel IO Controller B */
#ifdef _SAM3U_PIOC_INSTANCE_
	(void*) PIOC_Handler,    /* 12 Parallel IO Controller C */
#else
	(void*) (0UL),           /* 12 Reserved */
#endif /* _SAM3U_PIOC_INSTANCE_ */
	(void*) USART0_Handler,  /* 13 USART 0 */
	(void*) USART1_Handler,  /* 14 USART 1 */
	(void*) USART2_Handler,  /* 15 USART 2 */
#ifdef _SAM3U_USART3_INSTANCE_
	(void*) USART3_Handler,  /* 16 USART 3 */
#else
	(void*) (0UL),           /* 16 Reserved */
#endif /* _SAM3U_USART3_INSTANCE_ */
	(void*) HSMCI_Handler,   /* 17 MCI */
	(void*) TWI0_Handler,    /* 18 TWI 0 */
	(void*) TWI1_Handler,    /* 19 TWI 1 */
	(void*) SPI_Handler,     /* 20 SPI */
	(void*) SSC_Handler,     /* 21 SSC */
	(void*) TC0_Handler,     /* 22 Timer Counter 0 */
	(void*) TC1_Handler,     /* 23 Timer Counter 1 */
	(void*) TC2_Handler,     /* 24 Timer Counter 2 */
	(void*) PWM_Handler,     /* 25 PWM */
	(void*) ADC12B_Handler,  /* 26 ADC12B controller */
	(void*) ADC_Handler,     /* 27 ADC controller */
	(void*) DMAC_Handler,    /* 28 DMA controller */
	(void*) UDPHS_Handler    /* 29 USB High Speed Port */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
	uint32_t *pSrc, *pDest;

	/* Initialize the relocate segment */
	pSrc = &_etext;
	pDest = &_srelocate;

	if (pSrc != pDest) {
		for (; pDest < &_erelocate;) {
			*pDest++ = *pSrc++;
		}
	}

	/* Clear the zero segment */
	for (pDest = &_szero; pDest < &_ezero;) {
		*pDest++ = 0;
	}

	/* Set the vector table base address */
	pSrc = (uint32_t *) & _sfixed;
	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

	if (((uint32_t) pSrc >= IRAM0_ADDR) && ((uint32_t) pSrc < IRAM0_ADDR + IRAM_SIZE)) {
		SCB->VTOR |= (1UL) << SCB_VTOR_TBLBASE_Pos;
	}

	/* Initialize the C library */
	__libc_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
	while (1) {
	}
}

