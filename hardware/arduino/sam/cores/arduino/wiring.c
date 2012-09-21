/*
  Copyright (c) 2011 Arduino.  All right reserved.

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

uint32_t millis( void )
{
// todo: ensure no interrupts
	return GetTickCount() ;
}

uint32_t micros( void )
{
    uint32_t ticks ;
    uint32_t count ;

    SysTick->CTRL;
    do {
        ticks = SysTick->VAL;
        count = GetTickCount();
    } while (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk);

    return count * 1000 + (SysTick->LOAD + 1 - ticks) / (SystemCoreClock/1000000) ;
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
 * Cortex-M3 Systick IT handler: MOVED TO MAIN DUE TO WEAK SYMBOL ISSUE NOT RESOLVED
 */
void SysTick_Handler( void )
{
	tickReset();

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
