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

#ifndef _BITBANDING_
#define _BITBANDING_

/*----------------------------------------------------------------------------
 *  \file bitbanding.h
 *  Include Defines & macros for bit-banding.
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *         Header files
 *----------------------------------------------------------------------------*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 *         Global Macros
 *----------------------------------------------------------------------------*/

/**
 *  \brief Check if the address is in bit banding sram region.
 *
 *  \note The address should be in area of 0x2000000 ~ 0x200FFFFF
 *
 *  \param x The address to check.
 */
#define IS_BITBAND_SRAM_ADDR(x)       \
    ( ((uint32_t)(x)) >= 0x20000000 && \
      ((uint32_t)(x)) < (0x20000000+0x100000) )

/**
 *  \brief Check if the address is in bit banding peripheral region
 *
 *  \note The address should be in area of 0x4000000 ~ 0x400FFFFF
 *  \param x The address to check
 */
#define IS_BITBAND_PERIPH_ADDR(x)       \
    ( ((uint32_t)(x)) >= 0x40000000 && \
      ((uint32_t)(x)) < (0x40000000+0x100000) )

/**
 *  \brief Calculate bit band alias address.
 * 
 *  Calculate the bit band alias address and return a pointer address to word.
 *
 *  \param addr The byte address of bitbanding bit.
 *  \param bit  The bit position of bitbanding bit.
 *  \callergraph
 */
#define BITBAND_ALIAS_ADDRESS(addr, bit) \
    ((volatile uint32_t*)((((uint32_t)(addr) & 0xF0000000) + 0x02000000) \
                          +((((uint32_t)(addr)&0xFFFFF)*32)\
                          +(  (uint32_t)(bit)*4))))

/**
 *  \brief Bit write through bit banding.
 *
 *  \param addr32 32-bit aligned byte address where the bit exists.
 *  \param bit    Bit position.
 *  \param val    The value that the bit is set to.
 *  \callergraph
 */
#define WRITE_BITBANDING(addr32, bit, val) do {\
        *BITBAND_ALIAS_ADDRESS(addr32,bit) = (val); \
    } while (0);

/**
 *  \brief Toggle bit through bit banding
 *
 *  \param addr32 32-bit aligned byte address where the bit exists.
 *  \param bit    Bit position.
 */
#define TOGGLE_BITBANDING(addr32, bit) do {\
        volatile uint32_t * p = \
            BITBAND_ALIAS_ADDRESS(addr32,bit); \
        if (*p) *p = 0; \
        else    *p = 1; \
    }while(0);

#endif /* #ifndef _BITBANDING_ */
