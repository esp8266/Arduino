/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief NEWLIB_ADDONS miscellaneous macros include file for AVR32.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef __AVR32_NEWLIB_ADDONS_IO_H__
#define __AVR32_NEWLIB_ADDONS_IO_H__

#include <_ansi.h>

_BEGIN_STD_C

typedef char u8;
typedef unsigned int u32;

#define __raw_writeb(v,a)       (*(volatile unsigned char  *)(a) = (v))
#define __raw_writew(v,a)       (*(volatile unsigned short *)(a) = (v))
#define __raw_writel(v,a)       (*(volatile unsigned int   *)(a) = (v))

#define __raw_readb(a)          (*(volatile unsigned char  *)(a))
#define __raw_readw(a)          (*(volatile unsigned short *)(a))
#define __raw_readl(a)          (*(volatile unsigned int   *)(a))

/* As long as I/O is only performed in P4 (or possibly P3), we're safe */
#define writeb(v,a)		__raw_writeb(v,a)
#define writew(v,a)		__raw_writew(v,a)
#define writel(v,a)		__raw_writel(v,a)

#define readb(a)		__raw_readb(a)
#define readw(a)		__raw_readw(a)
#define readl(a)		__raw_readl(a)

/* Memory segments when segmentation is enabled */
#define P0SEG		0x00000000
#define P1SEG		0x80000000
#define P2SEG		0xa0000000
#define P3SEG		0xc0000000
#define P4SEG		0xe0000000

/* Returns the privileged segment base of a given address */
#define PXSEG(a)	(((unsigned long)(a)) & 0xe0000000)

/* Returns the physical address of a PnSEG (n=1,2) address */
#define PHYSADDR(a)	(((unsigned long)(a)) & 0x1fffffff)

/*
 * Map an address to a certain privileged segment
 */
#define P1SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P1SEG))
#define P2SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P2SEG))
#define P3SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P3SEG))
#define P4SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P4SEG))


#define cached(addr) P1SEGADDR(addr)
#define uncached(addr) P2SEGADDR(addr)
#define physaddr(addr) PHYSADDR(addr)

#define BF(field, value) \
  ({ union { \
      struct { \
       unsigned           : 32 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long __val: field ##  _SIZE ; \
      }; \
      unsigned long __ul; \
     } __tmp; \
     __tmp.__ul = 0; \
     __tmp.__val = value; \
     __tmp.__ul;})

#define BF_D(field, value) \
  ({ union { \
      struct { \
       unsigned long long : 64 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long long __val: field ##  _SIZE ; \
      }; \
      unsigned long long __ul; \
     } __tmp; \
     __tmp.__ul = 0; \
     __tmp.__val = value; \
     __tmp.__ul;})

#define BFINS(var, field, value) \
  { union {\
      struct { \
       unsigned           : 32 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long __val: field ##  _SIZE ; \
      }; \
      unsigned long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val = value; \
     var = __tmp.__ul;}

#define BFEXT(var, field) \
  ({ union {\
      struct { \
       unsigned           : 32 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long __val: field ##  _SIZE ; \
      }; \
      unsigned long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val; })

#define BFINS_D(var, field, value) \
  { union {\
      struct { \
       unsigned long long : 64 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long long __val: field ##  _SIZE ; \
      }; \
      unsigned long long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val = value; \
     var = __tmp.__ul;}

#define BFEXT_D(var, field) \
  ({ union {\
      struct { \
       unsigned long long : 64 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long long __val: field ##  _SIZE ; \
      }; \
      unsigned long long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val; })


_END_STD_C

#endif
