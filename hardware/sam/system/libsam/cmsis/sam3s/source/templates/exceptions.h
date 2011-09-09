/*! \file *********************************************************************
 *
 * \brief This file contains the interface for default exception handlers.
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

/* $asf_license$ */

#ifndef _EXCEPTIONS_
#define _EXCEPTIONS_

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/* Function prototype for exception table items (interrupt handler). */
typedef void( *IntFunc )( void ) ;

/* Default empty handler */
extern void Dummy_Handler( void ) ;

/* Cortex-M3 core handlers */
extern void Reset_Handler( void ) ;
extern void NMI_Handler( void ) ;
extern void HardFault_Handler( void ) ;
extern void MemManage_Handler( void ) ;
extern void BusFault_Handler( void ) ;
extern void UsageFault_Handler( void ) ;
extern void SVC_Handler( void ) ;
extern void DebugMon_Handler( void ) ;
extern void PendSV_Handler( void ) ;
extern void SysTick_Handler( void ) ;

/* Peripherals handlers */
extern void ACC_IrqHandler( void ) ;
extern void ADC_IrqHandler( void ) ;
extern void CRCCU_IrqHandler( void ) ;
extern void DAC_IrqHandler( void ) ;
extern void EEFC_IrqHandler( void ) ;
extern void MCI_IrqHandler( void ) ;
extern void PIOA_IrqHandler( void ) ;
extern void PIOB_IrqHandler( void ) ;
extern void PIOC_IrqHandler( void ) ;
extern void PMC_IrqHandler( void ) ;
extern void PWM_IrqHandler( void ) ;
extern void RSTC_IrqHandler( void ) ;
extern void RTC_IrqHandler( void ) ;
extern void RTT_IrqHandler( void ) ;
extern void SMC_IrqHandler( void ) ;
extern void SPI_IrqHandler( void ) ;
extern void SSC_IrqHandler( void ) ;
extern void SUPC_IrqHandler( void ) ;
extern void TC0_IrqHandler( void ) ;
extern void TC1_IrqHandler( void ) ;
extern void TC2_IrqHandler( void ) ;
extern void TC3_IrqHandler( void ) ;
extern void TC4_IrqHandler( void ) ;
extern void TC5_IrqHandler( void ) ;
extern void TWI0_IrqHandler( void ) ;
extern void TWI1_IrqHandler( void ) ;
extern void UART0_IrqHandler( void ) ;
extern void UART1_IrqHandler( void ) ;
extern void USART0_IrqHandler( void ) ;
extern void USART1_IrqHandler( void ) ;
extern void USBD_IrqHandler( void ) ;
extern void WDT_IrqHandler( void ) ;

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* _EXCEPTIONS_ */
