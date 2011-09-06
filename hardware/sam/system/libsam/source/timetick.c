/*
 %atmel_license%
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

