/*
  Copyright (c) 2012 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "Reset.h"

#ifdef __cplusplus
extern "C" {
#endif

static void __halt() {
	// Halts
	while (1)
		;
}

extern void svcHook(void);
extern void pendSVHook(void);
extern int sysTickHook(void);

/* Cortex-M3 core handlers */
void NMI_Handler       (void) __attribute__ ((weak, alias("__halt")));
void HardFault_Handler (void) __attribute__ ((weak, alias("__halt")));
void MemManage_Handler (void) __attribute__ ((weak, alias("__halt")));
void BusFault_Handler  (void) __attribute__ ((weak, alias("__halt")));
void UsageFault_Handler(void) __attribute__ ((weak, alias("__halt")));
void DebugMon_Handler  (void) __attribute__ ((weak, alias("__halt")));
void SVC_Handler       (void) { svcHook(); }
void PendSV_Handler    (void) {	pendSVHook(); }

void SysTick_Handler(void)
{
	if (sysTickHook())
		return;

	tickReset();

	// Increment tick count each ms
	TimeTick_Increment();
}

/* Peripherals handlers */
void SUPC_Handler       (void) __attribute__ ((weak, alias("__halt")));
void RSTC_Handler       (void) __attribute__ ((weak, alias("__halt")));
void RTC_Handler        (void) __attribute__ ((weak, alias("__halt")));
void RTT_Handler        (void) __attribute__ ((weak, alias("__halt")));
void WDT_Handler        (void) __attribute__ ((weak, alias("__halt")));
void PMC_Handler        (void) __attribute__ ((weak, alias("__halt")));
void EFC0_Handler       (void) __attribute__ ((weak, alias("__halt")));
void EFC1_Handler       (void) __attribute__ ((weak, alias("__halt")));
void UART_Handler       (void) __attribute__ ((weak, alias("__halt")));
#ifdef _SAM3XA_SMC_INSTANCE_
void SMC_Handler        (void) __attribute__ ((weak, alias("__halt")));
#endif
#ifdef _SAM3XA_SDRAMC_INSTANCE_
void SDRAMC_Handler     (void) __attribute__ ((weak, alias("__halt")));
#endif
void PIOA_Handler       (void) __attribute__ ((weak, alias("__halt")));
void PIOB_Handler       (void) __attribute__ ((weak, alias("__halt")));
#ifdef _SAM3XA_PIOC_INSTANCE_
void PIOC_Handler       (void) __attribute__ ((weak, alias("__halt")));
#endif
#ifdef _SAM3XA_PIOD_INSTANCE_
void PIOD_Handler       (void) __attribute__ ((weak, alias("__halt")));
#endif
#ifdef _SAM3XA_PIOE_INSTANCE_
void PIOE_Handler       (void) __attribute__ ((weak, alias("__halt")));
#endif
#ifdef _SAM3XA_PIOF_INSTANCE_
void PIOF_Handler       (void) __attribute__ ((weak, alias("__halt")));
#endif
void USART0_Handler     (void) __attribute__ ((weak, alias("__halt")));
void USART1_Handler     (void) __attribute__ ((weak, alias("__halt")));
void USART2_Handler     (void) __attribute__ ((weak, alias("__halt")));
#ifdef _SAM3XA_USART3_INSTANCE_
void USART3_Handler     (void) __attribute__ ((weak, alias("__halt")));
#endif
void HSMCI_Handler      (void) __attribute__ ((weak, alias("__halt")));
void TWI0_Handler       (void) __attribute__ ((weak, alias("__halt")));
void TWI1_Handler       (void) __attribute__ ((weak, alias("__halt")));
void SPI0_Handler       (void) __attribute__ ((weak, alias("__halt")));
#ifdef _SAM3XA_SPI1_INSTANCE_
void SPI1_Handler       (void) __attribute__ ((weak, alias("__halt")));
#endif
void SSC_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC0_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC1_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC2_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC3_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC4_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC5_Handler        (void) __attribute__ ((weak, alias("__halt")));
#ifdef _SAM3XA_TC2_INSTANCE_
void TC6_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC7_Handler        (void) __attribute__ ((weak, alias("__halt")));
void TC8_Handler        (void) __attribute__ ((weak, alias("__halt")));
#endif
void PWM_Handler        (void) __attribute__ ((weak, alias("__halt")));
void ADC_Handler        (void) __attribute__ ((weak, alias("__halt")));
void DACC_Handler       (void) __attribute__ ((weak, alias("__halt")));
void DMAC_Handler       (void) __attribute__ ((weak, alias("__halt")));
void UOTGHS_Handler     (void) __attribute__ ((weak, alias("__halt")));
void TRNG_Handler       (void) __attribute__ ((weak, alias("__halt")));
#ifdef _SAM3XA_EMAC_INSTANCE_
void EMAC_Handler       (void) __attribute__ ((weak, alias("__halt")));
#endif
void CAN0_Handler       (void) __attribute__ ((weak, alias("__halt")));
void CAN1_Handler       (void) __attribute__ ((weak, alias("__halt")));

#ifdef __cplusplus
}
#endif

