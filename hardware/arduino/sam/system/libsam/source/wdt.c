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
 * \file
 *
 * Implementation of Watchdog Timer (WDT) controller.
 *
 */

/** \addtogroup wdt_module Working with WDT
 * The WDT driver provides the interface to configure and use the WDT
 * peripheral.
 *
 * The WDT can be used to prevent system lock-up if the software becomes
 * trapped in a deadlock. It can generate a general reset or a processor
 * reset only. It is clocked by slow clock divided by 128.
 *
 * The WDT is running at reset with 16 seconds watchdog period (slow clock at 32.768 kHz)
 * and external reset generation enabled. The user must either disable it or
 * reprogram it to meet the application requires.
 *
 * To use the WDT, the user could follow these few steps:
 * <ul>
 * <li>Enable watchdog with given mode using \ref WDT_Enable().
 * <li>Restart the watchdog using \ref WDT_Restart() within the watchdog period.
 * </ul>
 *
 * For more accurate information, please look at the WDT section of the
 * Datasheet.
 *
 * \note
 * The Watchdog Mode Register (WDT_MR) can be written only once.\n
 *
 * Related files :\n
 * \ref wdt.c\n
 * \ref wdt.h.\n
 */
/*@{*/
/*@}*/

/*---------------------------------------------------------------------------
 *        Headers
 *---------------------------------------------------------------------------*/

#include "chip.h"

#include <stdint.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Enable watchdog with given mode.
 *
 * \note The Watchdog Mode Register (WDT_MR) can be written only once.
 * Only a processor reset resets it.
 *
 * \param dwMode   WDT mode to be set
 */
extern void WDT_Enable( Wdt* pWDT, uint32_t dwMode )
{
    pWDT->WDT_MR = dwMode ;
}

/**
 * \brief Disable watchdog.
 *
 * \note The Watchdog Mode Register (WDT_MR) can be written only once.
 * Only a processor reset resets it.
 */
extern void WDT_Disable( Wdt* pWDT )
{
    pWDT->WDT_MR = WDT_MR_WDDIS;
}

/**
 * \brief Watchdog restart.
 */
extern void WDT_Restart( Wdt* pWDT )
{
    pWDT->WDT_CR = 0xA5000001;
}

/**
 * \brief Watchdog get status.
 */
extern uint32_t WDT_GetStatus( Wdt* pWDT )
{
    return (pWDT->WDT_SR & 0x3) ;
}

/**
 * \brief Watchdog get period.
 *
 * \param dwMs   desired watchdog period in millisecond.
 */
extern uint32_t WDT_GetPeriod( uint32_t dwMs )
{
    if ( (dwMs < 4) || (dwMs > 16000) )
    {
        return 0 ;
    }
    return ((dwMs << 8) / 1000) ;
}
