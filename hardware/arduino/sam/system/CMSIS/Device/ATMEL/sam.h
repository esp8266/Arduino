/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
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

#ifndef _SAM_INCLUDED_
#define _SAM_INCLUDED_

#define part_is_defined(part) (defined(__ ## part ## __))

/* 
 * ----------------------------------------------------------------------------
 * SAM3 family 
 * ----------------------------------------------------------------------------
 */

/* SAM3N series */
#define SAM3N00 ( \
    part_is_defined( SAM3N00A ) || \
    part_is_defined( SAM3N00B ) )

#define SAM3N0 ( \
    part_is_defined( SAM3N0A ) || \
    part_is_defined( SAM3N0B ) || \
    part_is_defined( SAM3N0C ) )

#define SAM3N1 ( \
    part_is_defined( SAM3N1A ) || \
    part_is_defined( SAM3N1B ) || \
    part_is_defined( SAM3N1C ) )

#define SAM3N2 ( \
    part_is_defined( SAM3N2A ) || \
    part_is_defined( SAM3N2B ) || \
    part_is_defined( SAM3N2C ) )

#define SAM3N4 ( \
    part_is_defined( SAM3N4A ) || \
    part_is_defined( SAM3N4B ) || \
    part_is_defined( SAM3N4C ) )

/* Entire SAM3N series */
#define SAM3N_SERIES (SAM3N00 || SAM3N0 || SAM3N1 || SAM3N2 || SAM3N4)

	
/* SAM3S series */
#define SAM3S1 ( \
    part_is_defined( SAM3S1A ) || \
    part_is_defined( SAM3S1B ) || \
    part_is_defined( SAM3S1C ) )

#define SAM3S2 ( \
    part_is_defined( SAM3S2A ) || \
    part_is_defined( SAM3S2B ) || \
    part_is_defined( SAM3S2C ) )

#define SAM3S4 ( \
    part_is_defined( SAM3S4A ) || \
    part_is_defined( SAM3S4B ) || \
    part_is_defined( SAM3S4C ) )

/* Entire SAM3S series */
#define SAM3S_SERIES (SAM3S1 || SAM3S2 || SAM3S4)

/* SAM3SD8 series */
#define SAM3S8 ( \
    part_is_defined( SAM3S8B ) || \
    part_is_defined( SAM3S8C ) )

#define SAM3SD8 ( \
    part_is_defined( SAM3SD8B ) || \
    part_is_defined( SAM3SD8C ) )

/* Entire SAM3SD8 series */
#define SAM3SD8_SERIES (SAM3S8 || SAM3SD8)

/* SAM3U series */
#define SAM3U1 ( \
    part_is_defined( SAM3U1C ) || \
    part_is_defined( SAM3U1E ) )

#define SAM3U2 ( \
    part_is_defined( SAM3U2C ) || \
    part_is_defined( SAM3U2E ) )

#define SAM3U4 ( \
    part_is_defined( SAM3U4C ) || \
    part_is_defined( SAM3U4E ) )

/* Entire SAM3U series */
#define SAM3U_SERIES (SAM3U1 || SAM3U2 || SAM3U4)

/* SAM3XA series */
#define SAM3X4 ( \
    part_is_defined( SAM3X4C ) || \
    part_is_defined( SAM3X4E ) )

#define SAM3X8 ( \
    part_is_defined( SAM3X8C ) || \
    part_is_defined( SAM3X8E ) || \
    part_is_defined( SAM3X8H ) )

#define SAM3A4 ( \
    part_is_defined( SAM3A4C ) )

#define SAM3A8 ( \
    part_is_defined( SAM3A8C ) )

/* Entire SAM3XA series */
#define SAM3XA_SERIES ( SAM3X4 || SAM3X8 || SAM3A4 || SAM3A8)

/* 
 * ----------------------------------------------------------------------------
 * SAM4 family 
 * ----------------------------------------------------------------------------
 */


/* Entire SAM3 Family */
#define SAM3_SERIES ( SAM3N_SERIES || SAM3S_SERIES || SAM3SD8_SERIES || SAM3U_SERIES || SAM3XA_SERIES )

/* SAM4S series */
#define SAM4S8 ( \
    part_is_defined( SAM4S8B ) || \
    part_is_defined( SAM4S8C ) )

#define SAM4S16 ( \
    part_is_defined( SAM4S16B ) || \
    part_is_defined( SAM4S16C ) )

/* Entire SAM4S series */
#define SAM4S_SERIES ( SAM4S8 || SAM4S16)

/* Entire SAM4 Family */
#define SAM4_SERIES ( SAM4S_SERIES )

/* 
 * ----------------------------------------------------------------------------
 * SAM9 family 
 * ----------------------------------------------------------------------------
 */

/* 
 * ----------------------------------------------------------------------------
 * SAM7 family 
 * ----------------------------------------------------------------------------
 */



/* 
 * ----------------------------------------------------------------------------
 * Whole SAM product line
 * ----------------------------------------------------------------------------
 */
#define SAM ( SAM3_SERIES || SAM4_SERIES )

/* 
 * ----------------------------------------------------------------------------
 * Header inclusion 
 * ----------------------------------------------------------------------------
 */

#if SAM3_SERIES
#include "sam3.h"
#endif /* SAM3 */

#if SAM4_SERIES
#include "sam4.h"
#endif /* SAM4 */

#endif /* _SAM_INCLUDED_ */
