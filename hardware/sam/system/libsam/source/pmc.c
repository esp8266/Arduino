/*
 %atmel_license%
*/

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

#include <assert.h>

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
    assert( dwId < 35 ) ;

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
    assert( dwId < 35 ) ;

    if ( dwId < 32 )
    {
        if ( (PMC->PMC_PCSR0 & ((uint32_t)1 << dwId)) != ((uint32_t)1 << dwId) )
        {
//            TRACE_DEBUG("PMC_DisablePeripheral: clock of peripheral" " %u is not enabled\n\r", dwId ) ;
        }
        else
        {
            PMC->PMC_PCDR0 = 1 << dwId ;
        }
    }
    else
    {
        dwId -= 32 ;
        if ( (PMC->PMC_PCSR1 & ((uint32_t)1 << dwId)) != ((uint32_t)1 << dwId) )
        {
//            TRACE_DEBUG( "PMC_DisablePeripheral: clock of peripheral" " %u is not enabled\n\r", dwId + 32 ) ;
        }
        else
        {
            PMC->PMC_PCDR1 = 1 << dwId ;
        }
    }
}

/**
 * \brief Enable all the periph clock via PMC.
 */
extern void PMC_EnableAllPeripherals( void )
{
    PMC->PMC_PCER0 = MASK_STATUS0 ;
    while ( (PMC->PMC_PCSR0 & MASK_STATUS0) != MASK_STATUS0 ) ;

    PMC->PMC_PCER1 = MASK_STATUS1 ;
    while ( (PMC->PMC_PCSR1 & MASK_STATUS1) != MASK_STATUS1 ) ;

//    TRACE_DEBUG( "Enable all periph clocks\n\r" ) ;
}

/**
 * \brief Disable all the periph clock via PMC.
 */
extern void PMC_DisableAllPeripherals( void )
{
    PMC->PMC_PCDR0 = MASK_STATUS0 ;
    while ( (PMC->PMC_PCSR0 & MASK_STATUS0) != 0 ) ;

    PMC->PMC_PCDR1 = MASK_STATUS1 ;
    while ( (PMC->PMC_PCSR1 & MASK_STATUS1) != 0 ) ;

//    TRACE_DEBUG( "Disable all periph clocks\n\r" ) ;
}

/**
 * \brief Get Periph Status for the given peripheral ID.
 *
 * \param id  Peripheral ID (ID_xxx).
 */
extern uint32_t PMC_IsPeriphEnabled( uint32_t dwId )
{
    assert( dwId < 35 ) ;

    if ( dwId < 32 )
    {
        return ( PMC->PMC_PCSR0 & (1 << dwId) ) ;
    }
    else {
        return ( PMC->PMC_PCSR1 & (1 << (dwId - 32)) ) ;
    }
}
