/* Copyright (c) 2007 Eric B. Weddington
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id: common.h,v 1.3.4.3 2008/03/24 17:11:06 arcanum Exp $ */


#ifndef _AVR_COMMON_H
#define _AVR_COMMON_H

#include <avr/sfr_defs.h>

/* 
This purpose of this header is to define registers that have not been 
previously defined in the individual device IO header files, and to define 
other symbols that are common across AVR device families.

This file is designed to be included in <avr/io.h> after the individual
device IO header files, and after <avr/sfr_defs.h>

*/

/*------------ Registers Not Previously Defined ------------*/

/* 
These are registers that are not previously defined in the individual
IO header files, OR they are defined here because they are used in parts of
avr-libc even if a device is not selected but a general architecture has
been selected.
*/


/*
Stack pointer register.

AVR architecture 1 has no RAM, thus no stack pointer. 

All other architectures do have a stack pointer.  Some devices have only
less than 256 bytes of possible RAM locations (128 Bytes of SRAM
and no option for external RAM), thus SPH is officially "reserved"
for them.
*/
#if __AVR_ARCH__ >= 100
#  ifndef SPL
#    define SPL _SFR_MEM8(0x3D)
#  endif
#  ifndef SPH
#    define SPH _SFR_MEM8(0x3E)
#  endif
#  ifndef SP
#    define SP _SFR_MEM16(0x3D)
#  endif
#elif __AVR_ARCH__ != 1 
#  ifndef SPL
#    define SPL _SFR_IO8(0x3D)
#  endif
#  if XRAMEND < 0x100 && !defined(__COMPILING_AVR_LIBC__)
#    ifndef SP
#      define SP  _SFR_IO8(0x3D)
#    endif
#  else
#    ifndef SP
#      define SP  _SFR_IO16(0x3D)
#    endif
#    ifndef SPH
#      define SPH _SFR_IO8(0x3E)
#    endif
#  endif /* XRAMEND < 0x100 && !defined(__COMPILING_AVR_LIBC__) */
#endif /* __AVR_ARCH__ != 1 */


/* Status Register */
#ifndef SREG
#  if __AVR_ARCH__ >= 100
#    define SREG _SFR_MEM8(0x3F)
#  else
#    define SREG _SFR_IO8(0x3F)
#  endif
#endif


/* SREG bit definitions */
#ifndef SREG_C
#  define SREG_C  (0)
#endif
#ifndef SREG_Z
#  define SREG_Z  (1)
#endif
#ifndef SREG_N
#  define SREG_N  (2)
#endif
#ifndef SREG_V
#  define SREG_V  (3)
#endif
#ifndef SREG_S
#  define SREG_S  (4)
#endif
#ifndef SREG_H
#  define SREG_H  (5)
#endif
#ifndef SREG_T
#  define SREG_T  (6)
#endif
#ifndef SREG_I
#  define SREG_I  (7)
#endif


#if defined(__COMPILING_AVR_LIBC__)

/* AVR 6 Architecture */
#  if __AVR_ARCH__ == 6
#    ifndef EIND
#      define EIND  _SFR_IO8(0X3C)
#    endif
/* XMEGA Architectures */
#  elif __AVR_ARCH__ >= 100
#    ifndef EIND
#      define EIND  _SFR_MEM8(0x3C)
#    endif
#  endif

/*
Only few devices come without EEPROM.  In order to assemble the
EEPROM library components without defining a specific device, we
keep the EEPROM-related definitions here.
*/

/* EEPROM Control Register */
#  ifndef EECR
#    define EECR   _SFR_IO8(0x1C)
#  endif

/* EEPROM Data Register */
#  ifndef EEDR
#    define EEDR   _SFR_IO8(0x1D)
#  endif

/* EEPROM Address Register */
#  ifndef EEAR
#    define EEAR   _SFR_IO16(0x1E)
#  endif
#  ifndef EEARL
#    define EEARL  _SFR_IO8(0x1E)
#  endif
#  ifndef EEARH
#    define EEARH  _SFR_IO8(0x1F)
#  endif

/* EEPROM Control Register bits */
#  ifndef EERE
#    define EERE   (0)
#  endif
#  ifndef EEWE
#    define EEWE   (1)
#  endif
#  ifndef EEMWE
#    define EEMWE  (2)
#  endif
#  ifndef EERIE
#    define EERIE  (3)
#  endif

#endif /* __COMPILING_AVR_LIBC__ */



/*------------ Common Symbols ------------*/

/* 
Generic definitions for registers that are common across multiple AVR devices
and families.
*/

/* Pointer registers definitions */
#if __AVR_ARCH__ != 1  /* avr1 does not have X and Y pointers */
#  define XL  r26
#  define XH  r27
#  define YL  r28
#  define YH  r29
#endif /* #if __AVR_ARCH__ != 1 */
#define ZL  r30
#define ZH  r31


/* Status Register */
#if defined(SREG)
#  define AVR_STATUS_REG   SREG
#  if __AVR_ARCH__ >= 100
#    define AVR_STATUS_ADDR  _SFR_MEM_ADDR(SREG)
#  else
#    define AVR_STATUS_ADDR  _SFR_IO_ADDR(SREG)
#  endif
#endif

/* Stack Pointer (combined) Register */
#if defined(SP)
#  define AVR_STACK_POINTER_REG   SP
#  if __AVR_ARCH__ >= 100
#    define AVR_STACK_POINTER_ADDR  _SFR_MEM_ADDR(SP)
#  else
#    define AVR_STACK_POINTER_ADDR  _SFR_IO_ADDR(SP)
#  endif
#endif

/* Stack Pointer High Register */
#if defined(SPH)
#  define _HAVE_AVR_STACK_POINTER_HI 1
#  define AVR_STACK_POINTER_HI_REG   SPH
#  if __AVR_ARCH__ >= 100
#    define AVR_STACK_POINTER_HI_ADDR  _SFR_MEM_ADDR(SPH)
#  else
#    define AVR_STACK_POINTER_HI_ADDR  _SFR_IO_ADDR(SPH)
#  endif
#endif

/* Stack Pointer Low Register */
#if defined(SPL)
#  define AVR_STACK_POINTER_LO_REG   SPL
#  if __AVR_ARCH__ >= 100
#    define AVR_STACK_POINTER_LO_ADDR  _SFR_MEM_ADDR(SPL)
#  else
#    define AVR_STACK_POINTER_LO_ADDR  _SFR_IO_ADDR(SPL)
#  endif
#endif

/* RAMPZ Register */
#if defined(RAMPZ)
#  define AVR_RAMPZ_REG   RAMPZ
#  if __AVR_ARCH__ >= 100
#    define AVR_RAMPZ_ADDR  _SFR_MEM_ADDR(RAMPZ)
#  else
#    define AVR_RAMPZ_ADDR  _SFR_IO_ADDR(RAMPZ)
#  endif
#endif

/* Extended Indirect Register */
#if defined(EIND)
#  define AVR_EXTENDED_INDIRECT_REG   EIND
#  if __AVR_ARCH__ >= 100
#    define AVR_EXTENDED_INDIRECT_ADDR  _SFR_MEM_ADDR(EIND)
#  else
#    define AVR_EXTENDED_INDIRECT_ADDR  _SFR_IO_ADDR(EIND)
#  endif
#endif

/*------------ Workaround to old compilers (4.1.2 and earlier)  ------------*/

#ifndef __AVR_HAVE_MOVW__
# if  defined(__AVR_ENHANCED__) && __AVR_ENHANCED__
#  define __AVR_HAVE_MOVW__ 1
# endif
#endif

#ifndef __AVR_HAVE_LPMX__
# if  defined(__AVR_ENHANCED__) && __AVR_ENHANCED__
#  define __AVR_HAVE_LPMX__ 1
# endif
#endif

#ifndef __AVR_HAVE_MUL__
# if  defined(__AVR_ENHANCED__) && __AVR_ENHANCED__
#  define __AVR_HAVE_MUL__ 1
# endif
#endif

#endif /* _AVR_COMMON_H */
