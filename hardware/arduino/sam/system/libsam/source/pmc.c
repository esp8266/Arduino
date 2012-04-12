/* ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

//#include <assert.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

#define MASK_STATUS0 0xFFFFFFFC
#define MASK_STATUS1 0xFFFFFFFF

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Enables the clock of a peripheral. The peripheral ID is used
 * to identify which peripheral is targetted.
 *
 * \note The ID must NOT be shifted (i.e. 1 << ID_xxx).
 *
 * \param id  Peripheral ID (ID_xxx).
 */
extern void PMC_EnablePeripheral( uint32_t dwId )
{
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3XA_)
    //assert( dwId < 35 ) ;

    if ( dwId < 32 )
    {
        if ( (PMC->PMC_PCSR0 & ((uint32_t)1 << dwId)) == ((uint32_t)1 << dwId) )
        {
//            TRACE_DEBUG( "PMC_EnablePeripheral: clock of peripheral"  " %u is already enabled\n\r", dwId ) ;
        }
        else
        {
            PMC->PMC_PCER0 = 1 << dwId ;
        }
    }
    else
    {
        dwId -= 32;
        if ((PMC->PMC_PCSR1 & ((uint32_t)1 << dwId)) == ((uint32_t)1 << dwId))
        {
//            TRACE_DEBUG( "PMC_EnablePeripheral: clock of peripheral"  " %u is already enabled\n\r", dwId + 32 ) ;
        }
        else
        {
            PMC->PMC_PCER1 = 1 << dwId ;
        }
    }
#elif (defined _SAM3N_) || (defined _SAM3U_)
    if ( (PMC->PMC_PCSR0 & ((uint32_t)1 << dwId)) != ((uint32_t)1 << dwId) )
    {
        PMC->PMC_PCER0 = 1 << dwId ;
    }
#else
    #error "The specified chip is not supported."
#endif
}

/**
 * \brief Disables the clock of a peripheral. The peripheral ID is used
 * to identify which peripheral is targetted.
 *
 * \note The ID must NOT be shifted (i.e. 1 << ID_xxx).
 *
 * \param id  Peripheral ID (ID_xxx).
 */
extern void PMC_DisablePeripheral( uint32_t dwId )
{
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3XA_)
    //assert( dwId < 35 ) ;

    if ( dwId < 32 )
    {
        if ( (PMC->PMC_PCSR0 & ((uint32_t)1 << dwId)) == ((uint32_t)1 << dwId) )
        {
            PMC->PMC_PCDR0 = 1 << dwId ;
        }
    }
    else
    {
        dwId -= 32 ;
        if ( (PMC->PMC_PCSR1 & ((uint32_t)1 << dwId)) == ((uint32_t)1 << dwId) )
        {
            PMC->PMC_PCDR1 = 1 << dwId ;
        }
    }
#elif (defined _SAM3N_) || (defined _SAM3U_)
    if ( (PMC->PMC_PCSR0 & ((uint32_t)1 << dwId)) == ((uint32_t)1 << dwId) )
    {
        PMC->PMC_PCDR0 = 1 << dwId ;
    }
#else
    #error "The specified chip is not supported."
#endif
}

/**
 * \brief Get the status of the specified peripheral clock.
 *
 * \note The ID must NOT be shifted (i.e. 1 << ID_xxx).
 *
 * \param dwId Peripheral ID (ID_xxx).
 *
 * \retval 0 Clock is active.
 * \retval 1 Clock is inactive.
 * \retval 2 Invalid parameter.
 */
extern uint32_t PMC_IsPeripheralEnabled( uint32_t dwId )
{
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3XA_)
    //assert( dwId < 35 ) ;

    if ( dwId < 32 )
    {
        if ( PMC->PMC_PCSR0 & (1 << dwId) )
        {
            return 0 ;
        }
        else
        {
            return 1 ;
        }
    }
    else
    {
        if ( PMC->PMC_PCSR1 & (1 << (dwId - 32)) )
        {
            return 0 ;
        }
        else
        {
            return 1 ;
        }
    }
#elif (defined _SAM3N_) || (defined _SAM3U_)
    if ( (PMC->PMC_PCSR0 & ((uint32_t)1 << dwId)) )
    {
        return 0 ;
    }
    else
    {
        return 1 ;
    }    
#else
    #error "The specified chip is not supported."
#endif
}
