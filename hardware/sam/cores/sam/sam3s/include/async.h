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
 * Provide a routine for asynchronous transfer.
 *
 */

#ifndef _ASYNC_
#define _ASYNC_


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 *        Definition
 *----------------------------------------------------------------------------*/
/** Transfer is still pending.*/
#define ASYNC_STATUS_PENDING        0xFF

#ifdef __cplusplus
 extern "C" {
#endif

/*----------------------------------------------------------------------------
 *        Type
 *----------------------------------------------------------------------------*/
/** \brief Asynchronous transfer descriptor. */
typedef struct _Async
{
    /** Asynchronous transfer status.*/
    volatile uint8_t status ;
    /** Callback function to invoke when transfer completes or fails.*/
    void *callback ;
    /** Driver storage area; do not use.*/
    uint32_t pStorage[4] ;
} Async ;

/*----------------------------------------------------------------------------
 *        Global functions
 *----------------------------------------------------------------------------*/
extern uint32_t ASYNC_IsFinished( Async* pAsync ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ASYNC_ */

