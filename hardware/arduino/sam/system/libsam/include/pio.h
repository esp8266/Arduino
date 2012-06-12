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

#ifndef _PIO_
#define _PIO_

/*
 *         Headers
 */

#include "../chip.h"

#include <stdint.h>

/*
 *         Global Definitions
 */
typedef enum _EPioType
{
  PIO_NOT_A_PIN, /* Not under control of a peripheral. */
  PIO_PERIPH_A, /* The pin is controlled by the associated signal of peripheral A. */
  PIO_PERIPH_B, /* The pin is controlled by the associated signal of peripheral B. */
#if (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_)
  PIO_PERIPH_C, /* The pin is controlled by the associated signal of peripheral C. */
  PIO_PERIPH_D, /* The pin is controlled by the associated signal of peripheral D. */
#endif /* (defined _SAM3S_) || (defined _SAM3S8_) || (defined _SAM3N_) */
  PIO_INPUT, /* The pin is an input. */
  PIO_OUTPUT_0, /* The pin is an output and has a default level of 0. */
  PIO_OUTPUT_1 /* The pin is an output and has a default level of 1. */
} EPioType ;


/*  Default pin configuration (no attribute). */
#define PIO_DEFAULT                 (0u << 0)
/*  The internal pin pull-up is active. */
#define PIO_PULLUP                  (1u << 0)
/*  The internal glitch filter is active. */
#define PIO_DEGLITCH                (1u << 1)
/*  The pin is open-drain. */
#define PIO_OPENDRAIN               (1u << 2)

/*  The internal debouncing filter is active. */
#define PIO_DEBOUNCE                (1u << 3)

/*  Enable additional interrupt modes. */
#define PIO_IT_AIME                 (1u << 4)

/*  Interrupt High Level/Rising Edge detection is active. */
#define PIO_IT_RE_OR_HL             (1u << 5)
/*  Interrupt Edge detection is active. */
#define PIO_IT_EDGE                 (1u << 6)

/*  Low level interrupt is active */
#define PIO_IT_LOW_LEVEL            (0               | 0 | PIO_IT_AIME)
/*  High level interrupt is active */
#define PIO_IT_HIGH_LEVEL           (PIO_IT_RE_OR_HL | 0 | PIO_IT_AIME)
/*  Falling edge interrupt is active */
#define PIO_IT_FALL_EDGE            (0               | PIO_IT_EDGE | PIO_IT_AIME)
/*  Rising edge interrupt is active */
#define PIO_IT_RISE_EDGE            (PIO_IT_RE_OR_HL | PIO_IT_EDGE | PIO_IT_AIME)

#ifdef __cplusplus
 extern "C" {
#endif

/*
 *  The #attribute# field is a bitmask that can either be set to PIO_DEFAULt,
 *  or combine (using bitwise OR '|') any number of the following constants:
 *     - PIO_PULLUP
 *     - PIO_DEGLITCH
 *     - PIO_DEBOUNCE
 *     - PIO_OPENDRAIN
 *     - PIO_IT_LOW_LEVEL
 *     - PIO_IT_HIGH_LEVEL
 *     - PIO_IT_FALL_EDGE
 *     - PIO_IT_RISE_EDGE
 */


/*
 *         Global Functions
 */
extern void PIO_DisableInterrupt( Pio* pPio, const uint32_t dwMask ) ;
extern void PIO_PullUp( Pio* pPio, const uint32_t dwMask, const uint32_t dwPullUpEnable ) ;
extern void PIO_SetDebounceFilter( Pio* pPio, const uint32_t dwMask, const uint32_t dwCuttOff ) ;

extern void PIO_Set( Pio* pPio, const uint32_t dwMask ) ;
extern void PIO_Clear( Pio* pPio, const uint32_t dwMask ) ;
extern uint32_t PIO_Get( Pio* pPio, const EPioType dwType, const uint32_t dwMask ) ;

extern void PIO_SetPeripheral( Pio* pPio, const EPioType dwType, const uint32_t dwMask ) ;
extern void PIO_SetInput( Pio* pPio, uint32_t dwMask, uint32_t dwAttribute ) ;
extern void PIO_SetOutput( Pio* pPio, uint32_t dwMask, uint32_t dwDefaultValue,
                                      uint32_t dwMultiDriveEnable, uint32_t dwPullUpEnable ) ;

extern uint32_t PIO_Configure( Pio* pPio, const EPioType dwType, const uint32_t dwMask, const uint32_t dwAttribute ) ;

extern uint32_t PIO_GetOutputDataStatus( const Pio* pPio, const uint32_t dwMask ) ;


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _PIO_ */

