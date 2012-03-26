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

/**
 * \brief CMSIS Example
 * \mainpage CMSIS Example
 *
 * \section Purpose
 *
 * The cmsis example will help new users get familiar with
 * basic cmsis utilities of SAM3 and SAM4 microcontrollers.
 *
 * The example will execute the following tests:
 * - The application will flash the LED per second. The second 
 * is calculated by the standard system tick interface of cmsis.
 *
 * \section Requirements
 *
 * This package can be used with SAM3 and SAM4 evaluation kits.
 *
 * \section Description
 * The cmsis example will use the system tick of the Cortex-M.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board. Please
 *    refer to the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6224.pdf">
 *    SAM-BA User Guide</a>, the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *    GNU-Based Software Development</a> application note or the
 *    <a href="ftp://ftp.iar.se/WWWfiles/arm/Guides/EWARM_UserGuide.ENU.pdf">
 *    IAR EWARM User Guide</a>, depending on the solutions that users choose.
 *
 */

#include "sam.h"

#include "conf_board.h"

__INLINE static void delay_ms(uint32_t dw_dly_ticks);
__INLINE static void led_config(void);
__INLINE static void led_on(uint32_t dw_led);
__INLINE static void led_off(uint32_t dw_led);

/* Systick Counter */
static volatile uint32_t dw_ms_ticks = 0U;

/**
 * \brief SysTick_Handler.
 */
void SysTick_Handler(void)
{
	/* Increment counter necessary in delay(). */
	dw_ms_ticks++;
}

/**
 * \brief Delay number of tick Systicks (happens every 1 ms).
 */
__INLINE static void delay_ms(uint32_t dw_dly_ticks)
{
	uint32_t dw_cur_ticks;

	dw_cur_ticks = dw_ms_ticks;
	while ((dw_ms_ticks - dw_cur_ticks) < dw_dly_ticks) {
	}
}

 /**
  * \brief Configure LED pins.
  */
__INLINE static void led_config(void)
{
	/* Set up LED pins. */
	LED0_PIO->PIO_PER = LED0_MASK;
	LED0_PIO->PIO_OER = LED0_MASK;
	LED0_PIO->PIO_PUDR = LED0_MASK;
}

/**
 * \brief Switch on LED.
 */
__INLINE static void led_on(uint32_t dw_led)
{
	/* Turn On LED. */
	LED0_PIO->PIO_CODR = dw_led;
}

/**
 * \brief Switch off LED.
 */
__INLINE static void led_off(uint32_t dw_led)
{
	/* Turn Off LED. */
	LED0_PIO->PIO_SODR = dw_led;
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initilize the SAM3 system */
	SystemInit();

	WDT->WDT_MR = WDT_MR_WDDIS;

	/* Set up SysTick Timer for 1 msec interrupts. */
	if (SysTick_Config(SystemCoreClock / (uint32_t) 1000)) {
		/* Capture error. */
		while (1) {
		}
	}

	led_config();

	/* Flash the LED. */
	while (1) {
		/* Turn on the LED. */
		led_on(LED0_MASK);
		/* Delay  1000 Msec. */
		delay_ms((uint32_t) 1000);

		/* Turn off the LED. */
		led_off(LED0_MASK);
		/* Delay  1000 Msec. */
		delay_ms((uint32_t) 1000);
	}
}
