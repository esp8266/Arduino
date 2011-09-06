/*
 %atmel_license%
*/

/**
 * \file
 *
 * \par Purpose
 *
 * Interface for Real Time Timer (RTT) controller.
 *
 * \par Usage
 *
 * -# Changes the prescaler value of the given RTT and restarts it
 *    using \ref RTT_SetPrescaler().
 * -# Get current value of the RTT using \ref RTT_GetTime().
 * -# Enables the specified RTT interrupt using \ref RTT_EnableIT().
 * -# Get the status register value of the given RTT using \ref RTT_GetStatus().
 * -# Configures the RTT to generate an alarm at the given time
 *    using \ref RTT_SetAlarm().
 */

#ifndef _RTT_
#define _RTT_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "../chip.h"

#include <stdint.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus
 extern "C" {
#endif

extern void RTT_SetPrescaler( Rtt* pRtt, uint16_t wPrescaler ) ;

extern uint32_t RTT_GetTime( Rtt* pRtt ) ;

extern void RTT_EnableIT( Rtt* pRtt, uint32_t dwSources ) ;

extern uint32_t RTT_GetStatus( Rtt *pRtt ) ;

extern void RTT_SetAlarm( Rtt *pRtt, uint32_t dwTime ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef RTT_H */

