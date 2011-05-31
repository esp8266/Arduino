/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
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

/** \addtogroup spi_pdc_module SPI PDC driver
 *  \ingroup spi_at45_module
 * The Spi driver is a low level spi driver which performs SPI device Initializes,
 * spi transfer and receive. It can be used by upper SPI driver such as AT45
 * driver and AT26 driver.
 *
 * \section Usage
 * <ul>
 * <li> Initializes a SPI instance and the corresponding SPI hardware,
 *    Configure SPI in Master Mode using SPID_Configure().</li>
 * <li> Configures the SPI characteristics (such as Clock Polarity, Phase,
 *    transfers delay and Baud Rate) for the device corresponding to the
 *    chip select using SPID_ConfigureCS().</li>
 * <li> Starts a SPI master transfer using SPID_SendCommand().
 *    The transfer is performed using the PDC channels. </li>
 *    <li> It enable the SPI clock.</li>
 *    <li> Set the corresponding peripheral chip select.</li>
 *    <li> Initialize the two SPI PDC buffers.</li>
 *       <li> Initialize SPI_TPR and SPI_TCR with SPI command data and size
 *         to send command data first.</li>
 *       <li> Initialize SPI_RPR and SPI_RCR with SPI command data and size
 *         as dummy value.</li>
 *       <li> Initialize SPI_TNPR and SPI_TNCR with rest of the data to be
 *        transfered.(if the data specified in cmd structure)</li>
 *       <li> Initialize SPI_RNPR and SPI_RNCR with rest of the data to be
 *         received.(if the data specified in cmd structure)</li>
 *    <li> Initialize the callback function if specified.</li>
 *    <li> Enable transmitter and receiver.</li>
 *    <li> Example for sending a command to the dataflash through the SPI.</li>
 * \code
 *      /// Build command to be sent.
 *      ...
 *      // Send Command and data through the SPI
 *      if (SPID_SendCommand(pAt45->pSpid, pCommand)) {
 *          return AT45_ERROR_SPI;
 *      }
 * \endcode
 * <li> The SPI_Handler() must be called by the SPI Interrupt Service Routine
 *    with the corresponding Spi instance. It is invokes to check for pending
 *    interrupts. </li>
 *    <li> Example for initializing SPI interrupt handler in upper application.</li>
 * \code
 *        AIC_ConfigureIT(AT91C_ID_SPI, 0, SPI_Handler);
 * \endcode
 * </ul>
 * Related files :\n
 * \ref spi_pdc.c\n
 * \ref spi_pdc.h.\n
*/
/*@{*/
/*@}*/


/**
 * \file
 *
 * Implementation of SPI PDC driver.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Initializes the Spid structure and the corresponding SPI hardware.
 *
 * \param pSpid  Pointer to a Spid instance.
 * \param pSpiHw Associated SPI peripheral.
 * \param spiId SPI peripheral identifier.
 * \return 0.
 */
extern uint32_t SPID_Configure( Spid* pSpid, Spi* pSpiHw, uint8_t spiId )
{
    /* Initialize the SPI structure*/
    pSpid->pSpiHw = pSpiHw ;
    pSpid->spiId  = spiId ;
    pSpid->semaphore = 1 ;
    pSpid->pCurrentCommand = 0 ;

    /* Enable the SPI clock*/
    PMC_EnablePeripheral( pSpid->spiId ) ;

    /* Configure SPI in Master Mode with No CS selected !!! */
    SPI_Configure( pSpiHw, pSpid->spiId, SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_MR_PCS_Msk ) ;

    /* Enable the SPI */
    SPI_Enable( pSpiHw ) ;

    /* Disable the SPI clock */
    PMC_DisablePeripheral( pSpid->spiId ) ;

    return 0 ;
}

/**
 * \brief Configures the parameters for the device corresponding to the cs.
 *
 * \param pSpid  Pointer to a Spid instance.
 * \param cs  number corresponding to the SPI chip select.
 * \param csr  SPI_CSR value to setup.
 */
extern void SPID_ConfigureCS( Spid* pSpid, uint32_t dwCS, uint32_t dwCSR )
{
    SPI_ConfigureNPCS( pSpid->pSpiHw, dwCS, dwCSR ) ;
}

/**
 * \brief Starts a SPI master transfer. This is a non blocking function. It will
 * return as soon as the transfer is started.
 *
 * \param pSpid  Pointer to a Spid instance.
 * \param pCommand Pointer to the SPI command to execute.
 * \return 0 if the transfer has been started successfully; otherwise returns
 * SPID_ERROR_LOCK is the driver is in use, or SPID_ERROR if the command is not
 * valid.
 */
extern uint32_t SPID_SendCommand( Spid* pSpid, SpidCmd* pCommand )
{
    Spi* pSpiHw = pSpid->pSpiHw ;
    uint32_t dwSpiMr ;

    /* Try to get the dataflash semaphore */
    if ( pSpid->semaphore == 0 )
    {
        return SPID_ERROR_LOCK ;
    }
    pSpid->semaphore-- ;

    /* Enable the SPI clock */
    PMC_EnablePeripheral( pSpid->spiId ) ;

    /* Disable transmitter and receiver*/
    SPI_PdcDisableRx( pSpiHw ) ;
    SPI_PdcDisableTx( pSpiHw ) ;

    /* Write to the MR register*/
    dwSpiMr = pSpiHw->SPI_MR ;
    dwSpiMr |= SPI_MR_PCS_Msk ;
    dwSpiMr &= ~((1 << pCommand->spiCs) << 16 ) ;
    pSpiHw->SPI_MR=dwSpiMr ;

    /* Initialize the two SPI PDC buffer*/
    SPI_PdcSetRx( pSpiHw, pCommand->pCmd, pCommand->cmdSize, pCommand->pData, pCommand->dataSize ) ;
    SPI_PdcSetTx( pSpiHw, pCommand->pCmd, pCommand->cmdSize, pCommand->pData, pCommand->dataSize ) ;

    /* Initialize the callback*/
    pSpid->pCurrentCommand = pCommand ;

    /* Enable transmitter and receiver*/
    SPI_PdcEnableRx( pSpiHw ) ;
    SPI_PdcEnableTx( pSpiHw ) ;

    /* Enable buffer complete interrupt*/
    SPI_EnableIt( pSpiHw, SPI_IER_RXBUFF ) ;

    return 0 ;
}

/**
 * \brief The SPI_Handler must be called by the SPI Interrupt Service Routine with the
 * corresponding Spi instance.
 *
 * \note The SPI_Handler will unlock the Spi semaphore and invoke the upper application
 * callback.
 * \param pSpid  Pointer to a Spid instance.
 */
extern void SPID_Handler( Spid* pSpid )
{
    SpidCmd *pSpidCmd = pSpid->pCurrentCommand ;
    Spi *pSpiHw = pSpid->pSpiHw ;
    volatile uint32_t spiSr ;

    /* Read the status register*/
    spiSr = pSpiHw->SPI_SR ;
    if ( spiSr & SPI_SR_RXBUFF )
    {
        /* Disable transmitter and receiver */
        SPI_PdcDisableRx( pSpiHw ) ;
        SPI_PdcDisableTx( pSpiHw ) ;

        /* Disable the SPI clock*/
        PMC_DisablePeripheral( pSpid->spiId ) ;

        /* Disable buffer complete interrupt */
        SPI_DisableIt( pSpiHw, SPI_IDR_RXBUFF ) ;

        /* Release the dataflash semaphore*/
        pSpid->semaphore++ ;

        /* Invoke the callback associated with the current command*/
        if ( pSpidCmd && pSpidCmd->callback )
        {
            pSpidCmd->callback( 0, pSpidCmd->pArgument ) ;
        }

        /* Nothing must be done after. A new DF operation may have been started
           in the callback function.*/
    }
}

/**
 * \brief Returns 1 if the SPI driver is currently busy executing a command; otherwise
 * returns 0.
 * \param pSpid  Pointer to a Spid instance.
 */
extern uint32_t SPID_IsBusy( const Spid* pSpid )
{
    if ( pSpid->semaphore == 0 )
    {
        return 1 ;
    }
    else
    {
        return 0 ;
    }
}
