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
