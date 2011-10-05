/* %arduino_license% */

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t millis( void )
{
// todo: ensure no interrupts
	return GetTickCount() ;
}

uint32_t micros( void )
{
  uint32_t dwTicks ;

  __disable_irq() ;
  dwTicks=SysTick->VAL ;
  __enable_irq() ;

  return (GetTickCount()*1000) + ((SysTick->LOAD + 1 - dwTicks)/(SystemCoreClock/1000000)) ;
}

void delay( uint32_t dwMs )
{
	Wait( dwMs ) ;
}

void delayMicroseconds( uint32_t dwUs )
{
  uint32_t dwStartMicros=micros() ;

	while ( (micros() - dwStartMicros) < dwUs )
	{
		//	do nothing
	}
}

/*
 * Cortex-M3 Systick IT handler
 */
extern void SysTick_Handler( void )
{
  // Increment tick count each ms
  TimeTick_Increment() ;
}

#if defined ( __ICCARM__ ) /* IAR Ewarm 5.41+ */
extern signed int putchar( signed int c ) ;
/**
 * \brief
 *
 * \param c  Character to output.
 *
 * \return The character that was output.
 */
extern WEAK signed int putchar( signed int c )
{
    return c ;
}
#endif /* __ICCARM__ */

#ifdef __cplusplus
}
#endif
