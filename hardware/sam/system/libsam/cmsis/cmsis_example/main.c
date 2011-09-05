#include <stdlib.h>
#include <compiler.h>
#include <board.h>
#include "conf_board.h"

volatile uint32_t msTicks = 0;                        /* counts 1ms timeTicks */

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
__INLINE static void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*----------------------------------------------------------------------------
  configer LED pins
 *----------------------------------------------------------------------------*/
__INLINE static void LED_Config(void)
{
  LED0_PIO->PIO_PER    =  LED0_MASK;          /* Setup Pin PA19 for LED */
  LED0_PIO->PIO_OER    =  LED0_MASK;
  LED0_PIO->PIO_PUDR   =  LED0_MASK;
}

/*----------------------------------------------------------------------------
  Switch on LEDs
 *----------------------------------------------------------------------------*/
__INLINE static void LED_On(uint32_t led)
{
  LED0_PIO->PIO_CODR = led;              /* Turn On  LED */
}


/*----------------------------------------------------------------------------
  Switch off LEDs
 *----------------------------------------------------------------------------*/
__INLINE static void LED_Off(uint32_t led)
{
  LED0_PIO->PIO_SODR = led;              /* Turn Off LED */
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void)
{
	SystemInit();
  
	WDT->WDT_MR = WDT_MR_WDDIS;
  
	if (SysTick_Config(SystemCoreClock / 1000)) { /* Setup SysTick Timer for 1 msec interrupts  */
    	while (1);                                  /* Capture error */
	}
  
	LED_Config();                             
 
  while(1) {
    LED_On (LED0_MASK);                              /* Turn on the LED. */
    Delay (1000);                                /* delay  100 Msec */
    LED_Off (LED0_MASK);                             /* Turn off the LED. */
    Delay (1000);                                /* delay  100 Msec */
  }
}
