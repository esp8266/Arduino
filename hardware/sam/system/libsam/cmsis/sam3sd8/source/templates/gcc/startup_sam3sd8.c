/*! \file *********************************************************************
 *
 * \brief Startup file for SAM3S8/SAM3SD.
 *
 * $asf_license$
 *
 * This file defines common SAM series.
 *
 * - Compiler:           ARMGCC
 * - Supported devices:  All SAM3S8/SAM3SD devices can be used.
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
	SUPC_IrqHandler,    /* 0  Supply Controller */
	RSTC_IrqHandler,    /* 1  Reset Controller */
	RTC_IrqHandler,     /* 2  Real Time Clock */
	RTT_IrqHandler,     /* 3  Real Time Timer */
	WDT_IrqHandler,     /* 4  Watchdog Timer */
	PMC_IrqHandler,     /* 5  PMC */
	EEFC_IrqHandler,	/* 6  EEFC */
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

	if (((uint32_t) pSrc >= IRAM_ADDR) && ((uint32_t) pSrc < IRAM_ADDR + IRAM_SIZE)) {
		SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos;
	}

	/* Initialize the C library */
	__libc_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1);
}
