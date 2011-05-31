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

/**
 * \file
 *
 * Implementation of SPI PDC driver.
 *
 */

#ifndef _SPI_PDC_
#define _SPI_PDC_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** An unspecified error has occured.*/
#define SPID_ERROR          1

/** SPI driver is currently in use.*/
#define SPID_ERROR_LOCK     2

/*----------------------------------------------------------------------------
 *        Macros
 *----------------------------------------------------------------------------*/

/** Calculates the value of the SCBR field of the Chip Select Register given MCK and SPCK.*/
#define SPID_CSR_SCBR(mck, spck)    (SPI_CSR_SCBR(((mck) / (spck))) )

/** Calculates the value of the DLYBS field of the Chip Select Register given delay in ns and MCK.*/
#define SPID_CSR_DLYBS(mck, delay)  ( SPI_CSR_DLYBS(((((delay) * ((mck) / 1000000)) / 1000) + 1)) )

/** Calculates the value of the DLYBCT field of the Chip Select Register given delay in ns and MCK.*/
#define SPID_CSR_DLYBCT(mck, delay) ( SPI_CSR_DLYBCT((((delay) / 32 * ((mck) / 1000000)) / 1000) + 1) )

#ifdef __cplusplus
 extern "C" {
#endif

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/** SPI transfer complete callback. */
typedef void (*SpidCallback)( uint8_t, void* ) ;

/** \brief Spi Transfer Request prepared by the application upper layer.
 *
 * This structure is sent to the SPI_SendCommand function to start the transfer.
 * At the end of the transfer, the callback is invoked by the interrupt handler.
 */
typedef struct _SpidCmd
{
    /** Pointer to the command data. */
	uint8_t *pCmd;
    /** Command size in bytes. */
	uint8_t cmdSize;
    /** Pointer to the data to be sent. */
	uint8_t *pData;
    /** Data size in bytes. */
	unsigned short dataSize;
    /** SPI chip select. */
	uint8_t spiCs;
    /** Callback function invoked at the end of transfer. */
	SpidCallback callback;
    /** Callback arguments. */
	void *pArgument;
} SpidCmd ;

/** Constant structure associated with SPI port. This structure prevents
    client applications to have access in the same time. */
typedef struct _Spid
{
    /** Pointer to SPI Hardware registers */
	Spi* pSpiHw ;
    /** SPI Id as defined in the product datasheet */
	char spiId ;
    /** Current SpiCommand being processed */
	SpidCmd *pCurrentCommand ;
    /** Mutual exclusion semaphore. */
	volatile char semaphore ;
} Spid ;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern uint32_t SPID_Configure( Spid* pSpid, Spi* pSpiHw, uint8_t spiId ) ;

extern void SPID_ConfigureCS( Spid* pSpid, uint32_t dwCS, uint32_t dwCsr ) ;
	
extern uint32_t SPID_SendCommand( Spid* pSpid, SpidCmd* pCommand ) ;

extern void SPID_Handler( Spid* pSpid ) ;

extern uint32_t SPID_IsBusy( const Spid* pSpid ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _SPI_PDC_ */

