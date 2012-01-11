/*! \file *********************************************************************
 *
 * \brief This file contains the default exception handlers.
 *
 * $asf_license$
 *
 * \par Purpose
 *
 * This file provides basic support for Cortex-M processor based 
 * microcontrollers.
 *
 * \note
 * The exception handler has weak aliases.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

#include "exceptions.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

#ifdef __GNUC__
/* Cortex-M3 core handlers */
//void Reset_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
void NMI_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));
//void HardFault_Handler( void ) __attribute__ ((weak, alias ("Dummy_Handler"))) ;
void MemManage_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void BusFault_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void UsageFault_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void SVC_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void DebugMon_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler(void) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void SUPC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void RSTC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void RTT_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void PMC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void EFC0_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void EFC1_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void UART_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void SMC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOA_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOB_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void USART0_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void USART1_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void USART2_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void USART3_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void MCI_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void TWI0_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void TWI1_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void SPI_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void SSC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void PWM_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC12B_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void DMAC_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
void UDPHS_IrqHandler(void) __attribute__ ((weak, alias("Dummy_Handler")));
#endif /* __GNUC__ */

#ifdef __ICCARM__
/* Cortex-M3 core handlers */
//#pragma weak Reset_Handler=Dummy_Handler
#pragma weak NMI_Handler=Dummy_Handler
//#pragma weak HardFault_Handler=Dummy_Handler
#pragma weak MemManage_Handler=Dummy_Handler
#pragma weak BusFault_Handler=Dummy_Handler
#pragma weak UsageFault_Handler=Dummy_Handler
#pragma weak SVC_Handler=Dummy_Handler
#pragma weak DebugMon_Handler=Dummy_Handler
#pragma weak PendSV_Handler=Dummy_Handler
#pragma weak SysTick_Handler=Dummy_Handler

/* Peripherals handlers */
#pragma weak SUPC_IrqHandler=Dummy_Handler
#pragma weak RSTC_IrqHandler=Dummy_Handler
#pragma weak RTC_IrqHandler=Dummy_Handler
#pragma weak RTT_IrqHandler=Dummy_Handler
#pragma weak WDT_IrqHandler=Dummy_Handler
#pragma weak PMC_IrqHandler=Dummy_Handler
#pragma weak EFC0_IrqHandler=Dummy_Handler
#pragma weak EFC1_IrqHandler=Dummy_Handler
#pragma weak UART_IrqHandler=Dummy_Handler
#pragma weak SMC_IrqHandler=Dummy_Handler
#pragma weak PIOA_IrqHandler=Dummy_Handler
#pragma weak PIOB_IrqHandler=Dummy_Handler
#pragma weak PIOC_IrqHandler=Dummy_Handler
#pragma weak USART0_IrqHandler=Dummy_Handler
#pragma weak USART1_IrqHandler=Dummy_Handler
#pragma weak USART2_IrqHandler=Dummy_Handler
#pragma weak USART3_IrqHandler=Dummy_Handler
#pragma weak MCI_IrqHandler=Dummy_Handler
#pragma weak TWI0_IrqHandler=Dummy_Handler
#pragma weak TWI1_IrqHandler=Dummy_Handler
#pragma weak SPI_IrqHandler=Dummy_Handler
#pragma weak SSC_IrqHandler=Dummy_Handler
#pragma weak TC0_IrqHandler=Dummy_Handler
#pragma weak TC1_IrqHandler=Dummy_Handler
#pragma weak TC2_IrqHandler=Dummy_Handler
#pragma weak PWM_IrqHandler=Dummy_Handler
#pragma weak ADC12B_IrqHandler=Dummy_Handler
#pragma weak ADC_IrqHandler=Dummy_Handler
#pragma weak DMAC_IrqHandler=Dummy_Handler
#pragma weak UDPHS_IrqHandler=Dummy_Handler
#endif /* __ICCARM__ */

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
	while (1) {
	}
}

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
