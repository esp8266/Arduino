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

#ifndef _SAM_DEFS_
#define _SAM_DEFS_

#define part_is_defined(part) (defined(__ ## part ## __))

/* SAM3 family */

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

#define SAM3S8 ( \
    part_is_defined( SAM3S8B ) || \
    part_is_defined( SAM3S8C ) )

#define SAM3SD8 ( \
    part_is_defined( SAM3SD8B ) || \
    part_is_defined( SAM3SD8C ) )

#define SAM3U1 ( \
    part_is_defined( SAM3U1C ) || \
    part_is_defined( SAM3U1E ) )

#define SAM3U2 ( \
    part_is_defined( SAM3U2C ) || \
    part_is_defined( SAM3U2E ) )

#define SAM3U4 ( \
    part_is_defined( SAM3U4C ) || \
    part_is_defined( SAM3U4E ) )

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
	
#define SAM3X4 ( \
    part_is_defined( SAM3X4C ) || \
    part_is_defined( SAM3X4E ) )

#define SAM3X8 ( \
    part_is_defined( SAM3X8C ) || \
    part_is_defined( SAM3X8E ) || \
    part_is_defined( SAM3X8H ) )

#define SAM3A2 ( \
    part_is_defined( SAM3A2C ) )

#define SAM3A4 ( \
    part_is_defined( SAM3A4C ) )

#define SAM3A8 ( \
    part_is_defined( SAM3A8C ) )

/* Entire SAM3S Family */
#define SAM3S (SAM3S1 || SAM3S2 || SAM3S4 || SAM3S8 || SAM3SD8)

/* Entire SAM3U Family */
#define SAM3U (SAM3U1 || SAM3U2 || SAM3U4)

/* Entire SAM3N Family */
#define SAM3N (SAM3N1 || SAM3N2 || SAM3N4)

/* Entire SAM3XA Family */
#define SAM3XA ( SAM3X4 || SAM3X8 || SAM3A2 || SAM3A4 || SAM3A8)

/* SAM9 family */

/* SAM7 family */


/* Global SAM product line */
#define SAM ( SAM3S || SAM3U || SAM3N || SAM3XA )

#include "include/sam3.h"

#endif /* _SAM_DEFS_ */
