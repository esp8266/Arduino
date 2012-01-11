/*! \file *********************************************************************
 *
 * \brief Startup file for SAM3S.
 *
 * $asf_license$
 *
 * This file defines common SAM series.
 *
 * - Compiler:           IAR EWARM
 * - Supported devices:  All SAM3S devices can be used.
 * - AppNote:
 *
 ******************************************************************************/

#include "../exceptions.h"
#include "sam3.h"

typedef void (*intfunc) (void);
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

void __iar_program_start(void);
int __low_level_init(void);

/* Exception Table */
#pragma language=extended
#pragma segment="CSTACK"

/* The name "__vector_table" has special meaning for C-SPY: */
/* it is where the SP start value is found, and the NVIC vector */
/* table register (VTOR) is initialized to this address if != 0 */

#pragma section = ".intvec"
#pragma location = ".intvec"
const intvec_elem __vector_table[] = {
	{.__ptr = __sfe("CSTACK")},
	{Reset_Handler},

	{NMI_Handler},
	{HardFault_Handler},
	{MemManage_Handler},
	{BusFault_Handler},
	{UsageFault_Handler},
	{0}, {0}, {0}, {0},   /*  Reserved */
	{SVC_Handler},
	{DebugMon_Handler},
	{0},                   /*  Reserved */
	{PendSV_Handler},
	{SysTick_Handler},

	/* Configurable interrupts */
	{SUPC_IrqHandler},     /* 0  Supply Controller */
	{RSTC_IrqHandler},     /* 1  Reset Controller */
	{RTC_IrqHandler},      /* 2  Real Time Clock */
	{RTT_IrqHandler},      /* 3  Real Time Timer */
	{WDT_IrqHandler},      /* 4  Watchdog Timer */
	{PMC_IrqHandler},      /* 5  PMC */
	{EEFC_IrqHandler},     /* 6  EEFC */
	{Dummy_Handler},       /* 7  Reserved */
	{UART0_IrqHandler},    /* 8  UART0 */
	{UART1_IrqHandler},    /* 9  UART1 */
	{SMC_IrqHandler},      /* 10 SMC */
	{PIOA_IrqHandler},     /* 11 Parallel IO Controller A */
	{PIOB_IrqHandler},     /* 12 Parallel IO Controller B */
	{PIOC_IrqHandler},     /* 13 Parallel IO Controller C */
	{USART0_IrqHandler},   /* 14 USART 0 */
	{USART1_IrqHandler},   /* 15 USART 1 */
	{Dummy_Handler},       /* 16 Reserved */
	{Dummy_Handler},       /* 17 Reserved */
	{MCI_IrqHandler},      /* 18 MCI */
	{TWI0_IrqHandler},     /* 19 TWI 0 */
	{TWI1_IrqHandler},     /* 20 TWI 1 */
	{SPI_IrqHandler},      /* 21 SPI */
	{SSC_IrqHandler},      /* 22 SSC */
	{TC0_IrqHandler},      /* 23 Timer Counter 0 */
	{TC1_IrqHandler},      /* 24 Timer Counter 1 */
	{TC2_IrqHandler},      /* 25 Timer Counter 2 */
	{TC3_IrqHandler},      /* 26 Timer Counter 3 */
	{TC4_IrqHandler},      /* 27 Timer Counter 4 */
	{TC5_IrqHandler},      /* 28 Timer Counter 5 */
	{ADC_IrqHandler},      /* 29 ADC controller */
	{DAC_IrqHandler},      /* 30 DAC controller */
	{PWM_IrqHandler},      /* 31 PWM */
	{CRCCU_IrqHandler},	   /* 32 CRC Calculation Unit */
	{ACC_IrqHandler},      /* 33 Analog Comparator */
	{USBD_IrqHandler},     /* 34 USB Device Port */
	{Dummy_Handler}        /* 35 not used */
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
		SCB->VTOR |= (uint32_t) (1 << SCB_VTOR_TBLBASE_Pos);
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
