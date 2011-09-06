/*
 %atmel_license%
*/

/** \file cmsis example */

#include <stdlib.h>
#include <compiler.h>
#include <board.h>
#include "conf_board.h"

//! counts 1ms timeTicks
volatile uint32_t dw_ms_ticks = 0;

/**
 * \brief SysTick_Handler.
 */
void SysTick_Handler(void)
{
  // increment counter necessary in delay()
  dw_ms_ticks++;
}

/**
 * \brief delays number of tick Systicks (happens every 1 ms)
 */
__INLINE static void delay_ms(uint32_t dw_dly_ticks)
{
  uint32_t dw_cur_ticks;

  dw_cur_ticks = dw_ms_ticks;
  while ((dw_ms_ticks - dw_cur_ticks) < dw_dly_ticks);
}

 /**
 * \brief configer LED pins
 */
__INLINE static void led_config(void)
{
  // Setup LED Pin
  LED0_PIO->PIO_PER    =  LED0_MASK;
  LED0_PIO->PIO_OER    =  LED0_MASK;
  LED0_PIO->PIO_PUDR   =  LED0_MASK;
}

/**
* \brief Switch on LED
*/
__INLINE static void led_on(uint32_t dw_led)
{
  // Turn On LED
  LED0_PIO->PIO_CODR = dw_led;
}

/**
* \brief Switch off LED
*/ 
__INLINE static void led_off(uint32_t dw_led)
{
  // Turn Off LED
  LED0_PIO->PIO_SODR = dw_led;
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
  SystemInit();

  WDT->WDT_MR = WDT_MR_WDDIS;

  // Setup SysTick Timer for 1 msec interrupts
  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Capture error
    while (1);
  }

  led_config();

  // Flash the LED
  while(1) {
    // Turn on the LED.
    led_on (LED0_MASK);
    // delay  1000 Msec.
    delay_ms (1000);

    // Turn off the LED.
    led_off(LED0_MASK);
    // delay  1000 Msec.
    delay_ms (1000);
  }
}
