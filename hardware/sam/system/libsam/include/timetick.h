/*
 %atmel_license%
*/

/**
 *  \file
 *
 *  \par Purpose
 *
 *  Methods and definitions for Global time tick and wait functions.
 *
 *  Defines a common and simpliest use of Time Tick, to increase tickCount
 *  every 1ms, the application can get this value through GetTickCount().
 *
 *  \par Usage
 *
 *  -# Configure the System Tick with TimeTick_Configure() when MCK changed
 *     \note
 *     Must be done before any invoke of GetTickCount(), Wait() or Sleep().
 *  -# Uses GetTickCount to get current tick value.
 *  -# Uses Wait to wait several ms.
 *  -# Uses Sleep to enter wait for interrupt mode to wait several ms.
 *
 */

#ifndef _TIMETICK_
#define _TIMETICK_

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/

extern uint32_t TimeTick_Configure( uint32_t dwNew_MCK ) ;

extern void TimeTick_Increment( void ) ;

extern uint32_t GetTickCount( void ) ;

extern void Wait( volatile uint32_t dwMs ) ;

extern void Sleep( volatile uint32_t dwMs ) ;

#endif /* _TIMETICK_ */
