/*! \file *********************************************************************
 *
 * \brief This file contains the interface for default exception handlers.
 *
 * $asf_license$
 *
 * \par Purpose
 *
 * This file provides basic support for Cortex-M processor based 
 * microcontrollers.
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/* Function prototype for exception table items (interrupt handler). */
typedef void (*IntFunc) (void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M3 core handlers */
void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/* Peripherals handlers */
void SUPC_IrqHandler(void);
void RSTC_IrqHandler(void);
void RTC_IrqHandler(void);
void RTT_IrqHandler(void);
void WDT_IrqHandler(void);
void PMC_IrqHandler(void);
void EFC0_IrqHandler(void);
void EFC1_IrqHandler(void);
void UART_IrqHandler(void);
void SMC_IrqHandler(void);
void PIOA_IrqHandler(void);
void PIOB_IrqHandler(void);
void PIOC_IrqHandler(void);
void USART0_IrqHandler(void);
void USART1_IrqHandler(void);
void USART2_IrqHandler(void);
void USART3_IrqHandler(void);
void MCI_IrqHandler(void);
void TWI0_IrqHandler(void);
void TWI1_IrqHandler(void);
void SPI_IrqHandler(void);
void SSC_IrqHandler(void);
void TC0_IrqHandler(void);
void TC1_IrqHandler(void);
void TC2_IrqHandler(void);
void PWM_IrqHandler(void);
void ADC12B_IrqHandler(void);
void ADC_IrqHandler(void);
void DMAC_IrqHandler(void);
void UDPHS_IrqHandler(void);

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* EXCEPTIONS_H_INCLUDED */
