/*! \file *********************************************************************
 *
 * \brief Startup file for SAM3X.
 *
 * $asf_license$
 *
 * This file defines common SAM series.
 *
 * - Compiler:           ARMGCC
 * - Supported devices:  All SAM3X devices can be used.
 * - AppNote:
 *
 ******************************************************************************/

#include "../exceptions.h"
#include "sam3.h"

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

/* Exception Table */
__attribute__ ((section(".vectors")))
IntFunc exception_table[] = {

	/* Configure Initial Stack Pointer, using linker-generated symbols */
	(IntFunc) (&_estack),
	Reset_Handler,

	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0, 0, 0, 0,         /*  Reserved */
	SVC_Handler,
	DebugMon_Handler,
	0,                  /*  Reserved */
	PendSV_Handler,
	SysTick_Handler,

	/* Configurable interrupts */
	SUPC_IrqHandler,    /*  0  Supply Controller */
	RSTC_IrqHandler,    /*  1  Reset Controller */
	RTC_IrqHandler,     /*  2  Real Time Clock */
	RTT_IrqHandler,     /*  3  Real Time Timer */
	WDT_IrqHandler,     /*  4  Watchdog Timer */
	PMC_IrqHandler,     /*  5  PMC */
	EFC0_IrqHandler,    /*  6  EFC 0 */
	EFC1_IrqHandler,    /*  7  EFC 1 */
	UART_IrqHandler,    /*  8  UART */
	SMC_IrqHandler,     /*  9  SMC */
	SDRAMC_IrqHandler,  /* 10  SDRAMC */
	PIOA_IrqHandler,    /* 11 Parallel IO Controller A */
	PIOB_IrqHandler,    /* 12 Parallel IO Controller B */
	PIOC_IrqHandler,    /* 13 Parallel IO Controller C */
	PIOD_IrqHandler,    /* 14 Parallel IO Controller D */
	PIOE_IrqHandler,    /* 15 Parallel IO Controller E */
	PIOF_IrqHandler,    /* 16 Parallel IO Controller F */
	USART0_IrqHandler,  /* 17 USART 0 */
	USART1_IrqHandler,  /* 18 USART 1 */
	USART2_IrqHandler,  /* 19 USART 2 */
	USART3_IrqHandler,  /* 20 USART 3 */
	HSMCI_IrqHandler,   /* 21 MCI */
	TWI0_IrqHandler,    /* 22 TWI 0 */
	TWI1_IrqHandler,    /* 23 TWI 1 */
	SPI0_IrqHandler,    /* 24 SPI 0 */
	SPI1_IrqHandler,    /* 25 SPI 1 */
	SSC_IrqHandler,     /* 26 SSC */
	TC0_IrqHandler,     /* 27 Timer Counter 0 */
	TC1_IrqHandler,     /* 28 Timer Counter 1 */
	TC2_IrqHandler,     /* 29 Timer Counter 2 */
	TC3_IrqHandler,     /* 30 Timer Counter 3 */
	TC4_IrqHandler,     /* 31 Timer Counter 4 */
	TC5_IrqHandler,     /* 32 Timer Counter 5 */
	TC6_IrqHandler,     /* 33 Timer Counter 6 */
	TC7_IrqHandler,     /* 34 Timer Counter 7 */
	TC8_IrqHandler,     /* 35 Timer Counter 8 */
	PWM_IrqHandler,     /* 36 PWM */
	ADC_IrqHandler,     /* 37 ADC controller */
	DACC_IrqHandler,    /* 38 DAC controller */
	DMAC_IrqHandler,    /* 39 DMA Controller */
	UOTGHS_IrqHandler,  /* 40 USB OTG High Speed */
	TRNG_IrqHandler,    /* 41 True Random Number Generator */
	EMAC_IrqHandler,    /* 42 Ethernet MAC */
	CAN0_IrqHandler,    /* 43 CAN Controller 0 */
	CAN1_IrqHandler,    /* 44 CAN Controller 1 */
};

/* TEMPORARY PATCH FOR SCB */
#define SCB_VTOR_TBLBASE_Pos               29                            /*!< SCB VTOR: TBLBASE Position */
#define SCB_VTOR_TBLBASE_Msk               (1UL << SCB_VTOR_TBLBASE_Pos) /*!< SCB VTOR: TBLBASE Mask */

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

	if (((uint32_t) pSrc >= IRAM0_ADDR) && ((uint32_t) pSrc < NFC_RAM_ADDR)) {
		SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos;
	}

	/* Initialize the C library */
	__libc_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1);
}
