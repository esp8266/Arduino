/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
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
    pComponent->pPIO_Rs=NULL ;
    pComponent->dwPin_Rs=0u ;

    pComponent->pPIO_EN=NULL ;
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
extern uint32_t SN65HVD234_SetRs( SSN65HVD234_Data* pComponent, Pio* pPIO_Rs, uint32_t dwPin_Rs )
{
    pComponent->pPIO_Rs=pPIO_Rs ;
    pComponent->dwPin_Rs=dwPin_Rs ;

    PIO_SetOutput( pPIO_Rs, dwPin_Rs, 0, 0, 1 ) ;

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
extern uint32_t SN65HVD234_SetEN( SSN65HVD234_Data* pComponent, Pio* pPIO_EN, uint32_t dwPin_EN )
{
    pComponent->pPIO_EN=pPIO_EN ;
    pComponent->dwPin_EN=dwPin_EN ;

    PIO_SetOutput( pPIO_EN, dwPin_EN, 0, 0, 1 ) ;

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
    pComponent->pPIO_EN->PIO_SODR=pComponent->dwPin_EN ;

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
    pComponent->pPIO_EN->PIO_CODR=pComponent->dwPin_EN ;

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
    pComponent->pPIO_Rs->PIO_SODR=pComponent->dwPin_Rs ;

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
    pComponent->pPIO_Rs->PIO_CODR=pComponent->dwPin_Rs ;

    return 0u ;
}

