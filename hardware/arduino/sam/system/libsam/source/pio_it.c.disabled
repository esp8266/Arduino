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

#include <assert.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

/* Maximum number of interrupt sources that can be defined. This
 * constant can be increased, but the current value is the smallest possible
 * that will be compatible with all existing projects. */
#define MAX_INTERRUPT_SOURCES       7

/*----------------------------------------------------------------------------
 *        Local types
 *----------------------------------------------------------------------------*/

/**
 * Describes a PIO interrupt source, including the PIO instance triggering the
 * interrupt and the associated interrupt handler.
 */
typedef struct _InterruptSource
{
    /* Pointer to the source pin instance. */
    const Pin *pPin ;

    /* Interrupt handler. */
    void (*handler)( const Pin* ) ;
} InterruptSource ;

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/* List of interrupt sources. */
static InterruptSource _aIntSources[MAX_INTERRUPT_SOURCES] ;

/* Number of currently defined interrupt sources. */
static uint32_t _dwNumSources = 0;

/*----------------------------------------------------------------------------
 *        Local Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Handles all interrupts on the given PIO controller.
 * \param id  PIO controller ID.
 * \param pPio  PIO controller base address.
 */
extern void PioInterruptHandler( uint32_t id, Pio *pPio )
{
    uint32_t status;
    uint32_t i;

    /* Read PIO controller status */
    status = pPio->PIO_ISR;
    status &= pPio->PIO_IMR;

    /* Check pending events */
    if ( status != 0 )
    {
        TRACE_DEBUG( "PIO interrupt on PIO controller #%d\n\r", id ) ;

        /* Find triggering source */
        i = 0;
        while ( status != 0 )
        {
            /* There cannot be an unconfigured source enabled. */
            assert(i < _dwNumSources);

            /* Source is configured on the same controller */
            if (_aIntSources[i].pPin->id == id)
            {
                /* Source has PIOs whose statuses have changed */
                if ( (status & _aIntSources[i].pPin->mask) != 0 )
                {
                    TRACE_DEBUG( "Interrupt source #%d triggered\n\r", i ) ;

                    _aIntSources[i].handler(_aIntSources[i].pPin);
                    status &= ~(_aIntSources[i].pPin->mask);
                }
            }
            i++;
        }
    }
}

/*----------------------------------------------------------------------------
 *        Global Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Parallel IO Controller A interrupt handler
 * \Redefined PIOA interrupt handler for NVIC interrupt table.
 */
extern void PIOA_IrqHandler( void )
{
    if ( PIOA->PIO_PCISR != 0 )
    {
        PIO_CaptureHandler() ;
    }

    PioInterruptHandler( ID_PIOA, PIOA ) ;
}

/**
 * \brief Parallel IO Controller B interrupt handler
 * \Redefined PIOB interrupt handler for NVIC interrupt table.
 */
extern void PIOB_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOB, PIOB ) ;
}

/**
 * \brief Parallel IO Controller C interrupt handler
 * \Redefined PIOC interrupt handler for NVIC interrupt table.
 */
extern void PIOC_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOC, PIOC ) ;
}

/**
 * \brief Initializes the PIO interrupt management logic
 *
 * The desired priority of PIO interrupts must be provided.
 * Calling this function multiple times result in the reset of currently
 * configured interrupts.
 *
 * \param priority  PIO controller interrupts priority.
 */
extern void PIO_InitializeInterrupts( uint32_t dwPriority )
{
    TRACE_DEBUG( "PIO_Initialize()\n\r" ) ;

    /* Reset sources */
    _dwNumSources = 0 ;

    /* Configure PIO interrupt sources */
    TRACE_DEBUG( "PIO_Initialize: Configuring PIOA\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOA ) ;
    PIOA->PIO_ISR ;
    PIOA->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOA_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOA_IRQn ) ;
    NVIC_SetPriority( PIOA_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOA_IRQn ) ;

    TRACE_DEBUG( "PIO_Initialize: Configuring PIOB\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOB ) ;
    PIOB->PIO_ISR ;
    PIOB->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOB_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOB_IRQn ) ;
    NVIC_SetPriority( PIOB_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOB_IRQn ) ;

    TRACE_DEBUG( "PIO_Initialize: Configuring PIOC\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOC ) ;
    PIOC->PIO_ISR ;
    PIOC->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOC_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOC_IRQn ) ;
    NVIC_SetPriority( PIOC_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOC_IRQn ) ;
}

/**
 * Configures a PIO or a group of PIO to generate an interrupt on status
 * change. The provided interrupt handler will be called with the triggering
 * pin as its parameter (enabling different pin instances to share the same
 * handler).
 * \param pPin  Pointer to a Pin instance.
 * \param handler  Interrupt handler function pointer.
 */
extern void PIO_ConfigureIt( const Pin *pPin, void (*handler)( const Pin* ) )
{
    Pio* pio ;
    InterruptSource* pSource ;

    TRACE_DEBUG( "PIO_ConfigureIt()\n\r" ) ;

    assert( pPin ) ;
    pio = pPin->pio ;
    assert( _dwNumSources < MAX_INTERRUPT_SOURCES ) ;

    /* Define new source */
    TRACE_DEBUG( "PIO_ConfigureIt: Defining new source #%d.\n\r",  _dwNumSources ) ;

    pSource = &(_aIntSources[_dwNumSources]) ;
    pSource->pPin = pPin ;
    pSource->handler = handler ;
    _dwNumSources++ ;

    /* PIO3 with additional interrupt support
     * Configure additional interrupt mode registers */
    if ( pPin->attribute & PIO_IT_AIME )
    {
        // enable additional interrupt mode
        pio->PIO_AIMER       = pPin->mask ;

        // if bit field of selected pin is 1, set as Rising Edge/High level detection event
        if ( pPin->attribute & PIO_IT_RE_OR_HL )
        {
            pio->PIO_REHLSR    = pPin->mask ;
        }
        else
        {
            pio->PIO_FELLSR     = pPin->mask;
        }

        /* if bit field of selected pin is 1, set as edge detection source */
        if (pPin->attribute & PIO_IT_EDGE)
            pio->PIO_ESR     = pPin->mask;
        else
            pio->PIO_LSR     = pPin->mask;
    }
    else
    {
        /* disable additional interrupt mode */
        pio->PIO_AIMDR       = pPin->mask;
    }
}

/**
 * Enables the given interrupt source if it has been configured. The status
 * register of the corresponding PIO controller is cleared prior to enabling
 * the interrupt.
 * \param pPin  Interrupt source to enable.
 */
extern void PIO_EnableIt( const Pio* pPio, const uint32_t dwMask )
{
    pPio->PIO_ISR ;
    pPio->PIO_IER = dwMask ;
}

/**
 * Disables a given interrupt source, with no added side effects.
 *
 * \param pPin  Interrupt source to disable.
 */
extern void PIO_DisableIt( const Pio* pPio, const uint32_t dwMask )
{
    pPio->PIO_IDR = dwMask ;
}

