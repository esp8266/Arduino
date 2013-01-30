/*
  Copyright (c) 2013 Arduino.  All right reserved.

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

/**
  * \file
  *
  * Implementation of the SN65HVD234 drivers.
  *
  */

#include "sn65hvd234.h"

#include <string.h>

/**
 * \brief Initialize SN65HVD234 component data
 *
 * \param pComponent    pointer on SSN65HVD234_Data
 *
 * \return              0 if OK
 */
extern uint32_t SN65HVD234_Init( SSN65HVD234_Data* pComponent )
{
    pComponent->dwPin_Rs=0u ;
    pComponent->dwPin_EN=0u ;

    return 0u ;
}

/**
 * \brief Initialize Rs pin of transceiver
 *
 * \param pComponent    pointer on SSN65HVD234_Data
 * \param pPIO_Rs       pointer on PIOx base for transceiver Rs pin
 * \param dwPin_Rs      PIO pin index for transceiver Rs pin
 *
 * \return              0 if OK
 */
extern uint32_t SN65HVD234_SetRs( SSN65HVD234_Data* pComponent, uint32_t dwPin_Rs )
{
    pComponent->dwPin_Rs=dwPin_Rs ;

    pinMode( dwPin_Rs, OUTPUT ) ;

    return 0u ;
}

/**
 * \brief Initialize EN pin of transceiver
 *
 * \param pComponent    pointer on SSN65HVD234_Data
 * \param pPIO_EN       pointer on PIOx base for transceiver EN pin
 * \param dwPin_EN      PIO pin index for transceiver EN pin
 *
 * \return              0 if OK
 */
extern uint32_t SN65HVD234_SetEN( SSN65HVD234_Data* pComponent, uint32_t dwPin_EN )
{
    pComponent->dwPin_EN=dwPin_EN ;

    pinMode( dwPin_EN, OUTPUT ) ;

    return 0u ;
}

/**
 * \brief Enable transceiver
 *
 * \param pComponent    pointer on SSN65HVD234_Data
 *
 * \return              0 if OK
 */
extern uint32_t SN65HVD234_Enable( SSN65HVD234_Data* pComponent )
{
    // Raise EN of SN65HVD234 to High Level (Vcc)
    digitalWrite( pComponent->dwPin_EN, HIGH ) ;

    return 0u ;
}

/**
 * \brief Disable transceiver
 *
 * \param pComponent    pointer on SSN65HVD234_Data
 *
 * \return              0 if OK
 */
extern uint32_t SN65HVD234_Disable( SSN65HVD234_Data* pComponent )
{
    // Lower EN of SN65HVD234 to Low Level (0.0v)
    digitalWrite( pComponent->dwPin_EN, LOW ) ;

    return 0u ;
}

/**
 * \brief Turn component into lowpower mode
 *
 * \param pComponent    pointer on SSN65HVD234_Data
 *
 * \return              0 if OK
 */
extern uint32_t SN65HVD234_EnableLowPower( SSN65HVD234_Data* pComponent )
{
    // Raise Rs of SN65HVD234 to more than 0.75v
    digitalWrite( pComponent->dwPin_Rs, HIGH ) ;

    // Now, SN65HVD234 is only listening

    return 0u ;
}

/**
 * \brief Restore Normal mode by leaving lowpower mode
 *
 * \param pComponent    pointer on SSN65HVD234_Data
 *
 * \return              0 if OK
 */
extern uint32_t SN65HVD234_DisableLowPower( SSN65HVD234_Data* pComponent )
{
    // Lower Rs of SN65HVD234 to 0.0v < 0.33v
    digitalWrite( pComponent->dwPin_Rs, LOW ) ;

    return 0u ;
}

