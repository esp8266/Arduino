/*
 %atmel_license%
*/

/**
 * \file
 *
 * \section Purpose
 * Interface for Watchdog Timer (WDT) controller.
 *
 * \section Usage
 * -# Enable watchdog with given mode using \ref WDT_Enable().
 * -# Disable watchdog using \ref WDT_Disable()
 * -# Restart the watchdog using \ref WDT_Restart().
 * -# Get watchdog status using \ref  WDT_GetStatus().
 * -# Caculate watchdog period value using \ref WDT_GetPeriod().
 */

#ifndef _WDT_
#define _WDT_

#include "../chip.h"

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern void WDT_Enable( Wdt* pWDT, uint32_t dwMode ) ;

extern void WDT_Disable( Wdt* pWDT ) ;

extern void WDT_Restart( Wdt* pWDT ) ;

extern uint32_t WDT_GetStatus( Wdt* pWDT ) ;

extern uint32_t WDT_GetPeriod( uint32_t dwMs ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _WDT_ */

