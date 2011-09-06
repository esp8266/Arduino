/*
 %atmel_license%
*/

/**
 *  \file
 *
 *  \par Purpose
 *
 *  Configuration and handling of interrupts on PIO status changes. The API
 *  provided here have several advantages over the traditional PIO interrupt
 *  configuration approach:
 *     - It is highly portable
 *     - It automatically demultiplexes interrupts when multiples pins have been
 *       configured on a single PIO controller
 *     - It allows a group of pins to share the same interrupt
 *
 *  However, it also has several minor drawbacks that may prevent from using it
 *  in particular applications:
 *     - It enables the clocks of all PIO controllers
 *     - PIO controllers all share the same interrupt handler, which does the
 *       demultiplexing and can be slower than direct configuration
 *     - It reserves space for a fixed number of interrupts, which can be
 *       increased by modifying the appropriate constant in pio_it.c.
 *
 *  \par Usage
 *
 *  -# Initialize the PIO interrupt mechanism using PIO_InitializeInterrupts()
 *     with the desired priority (0 ... 7).
 *  -# Configure a status change interrupt on one or more pin(s) with
 *     PIO_ConfigureIt().
 *  -# Enable & disable interrupts on pins using PIO_EnableIt() and
 *     PIO_DisableIt().
 */

#ifndef _PIO_IT_
#define _PIO_IT_

/*
 *         Headers
 */

#include "pio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*
 *         Global functions
 */

extern void PIO_InitializeInterrupts( uint32_t dwPriority ) ;

extern void PIO_ConfigureIt( const Pin *pPin, void (*handler)( const Pin* ) ) ;

extern void PIO_EnableIt( const Pio* pPio, const uint32_t dwMask ) ;
extern void PIO_DisableIt( const Pio* pPio, const uint32_t dwMask ) ;

extern void PIO_IT_InterruptHandler( void ) ;

extern void PioInterruptHandler( uint32_t id, Pio *pPio ) ;

extern void PIO_CaptureHandler( void ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _PIO_IT_ */

