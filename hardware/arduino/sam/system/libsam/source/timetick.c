/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
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
 *  \file
 *  Implement simple system tick usage.
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

/*----------------------------------------------------------------------------
 *         Local variables
 *----------------------------------------------------------------------------*/

/** Tick Counter united by ms */
static volatile uint32_t _dwTickCount=0 ;

/*----------------------------------------------------------------------------
 *         Exported Functions
 *----------------------------------------------------------------------------*/

/**
 *  \brief Handler for Sytem Tick interrupt.
 */
extern void TimeTick_Increment( void )
{
    _dwTickCount++ ;
}

/**
 *  \brief Configures the SAM3 SysTick & reset tickCount.
 *  Systick interrupt handler will generates 1ms interrupt and increase a
 *  tickCount.
 *  \param dwNew_MCK  Current master clock.
 */
extern uint32_t TimeTick_Configure( uint32_t dwNew_MCK )
{
    _dwTickCount = 0 ;

    return SysTick_Config( dwNew_MCK/1000 ) ;
}

/**
 *  \brief Get current Tick Count, in ms.
 */
extern uint32_t GetTickCount( void )
{
    return _dwTickCount ;
}

/**
 *  \brief Sync Wait for several ms
 */
extern void Wait( volatile uint32_t dwMs )
{
    uint32_t dwStart ;
    uint32_t dwCurrent ;

    dwStart = _dwTickCount ;
    do
    {
        dwCurrent = _dwTickCount ;
    } while ( dwCurrent - dwStart < dwMs ) ;
}

/**
 *  \brief Sync Sleep for several ms
 */
extern void Sleep( volatile uint32_t dwMs )
{
    uint32_t dwStart ;
    uint32_t dwCurrent ;

    dwStart = _dwTickCount ;

    do
    {
        dwCurrent = _dwTickCount ;

        if ( dwCurrent - dwStart > dwMs )
        {
            break ;
        }

        __WFI() ;
    } while( 1 ) ;
}

