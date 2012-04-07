/* Copyright (c) 2002, 2003, 2004, 2007 Marek Michalkiewicz
   Copyright (c) 2005, 2006 Bjoern Haase
   Copyright (c) 2008 Atmel Corporation
   Copyright (c) 2008 Wouter van Gulik
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

/* $Id: eeprom.h,v 1.21.2.6 2008/08/19 22:10:39 arcanum Exp $ */

#ifndef _AVR_EEPROM_H_
#define _AVR_EEPROM_H_ 1

#include <avr/io.h>
#include <stddef.h>	/* size_t */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	__ATTR_PURE__
# ifdef	 __DOXYGEN__
#  define __ATTR_PURE__
# else
#  define __ATTR_PURE__  __attribute__((__pure__))
# endif
#endif

#if (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) )
uint16_t __eerd_word (const uint16_t *, uint8_t (*)(const uint8_t *))
    __ATTR_PURE__;
uint32_t __eerd_dword (const uint32_t *, uint8_t (*)(const uint8_t *))
    __ATTR_PURE__;
void __eerd_block (void *, const void *, size_t, uint8_t (*)(const uint8_t *));

void __eewr_word (uint16_t *, uint16_t, void (*)(uint8_t *, uint8_t));
void __eewr_dword (uint32_t *, uint32_t, void (*)(uint8_t *, uint8_t));
void __eewr_block (void *, const void *, size_t, void (*)(uint8_t *, uint8_t));
#endif /* (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) ) */

#if !E2END && !defined(__DOXYGEN__)
# ifndef __COMPILING_AVR_LIBC__
#  warning "Device does not have EEPROM available."
# endif
  /* Omit below for chips without EEPROM. */

#else

/** \defgroup avr_eeprom <avr/eeprom.h>: EEPROM handling
    \code #include <avr/eeprom.h> \endcode

    This header file declares the interface to some simple library
    routines suitable for handling the data EEPROM contained in the
    AVR microcontrollers.  The implementation uses a simple polled
    mode interface.  Applications that require interrupt-controlled
    EEPROM access to ensure that no time will be wasted in spinloops
    will have to deploy their own implementation.

    \note All of the read/write functions first make sure the EEPROM
    is ready to be accessed.  Since this may cause long delays if a
    write operation is still pending, time-critical applications
    should first poll the EEPROM e. g. using eeprom_is_ready() before
    attempting any actual I/O.  But this functions are not wait until
    SELFPRGEN in SPMCSR becomes zero.  Do this manually, if your
    softwate contains the Flash burning.

    \note As these functions modify IO registers, they are known to be
    non-reentrant.  If any of these functions are used from both,
    standard and interrupt context, the applications must ensure
    proper protection (e.g. by disabling interrupts before accessing
    them).

    \note All write functions force erase_and_write programming mode.
 */

/** \def EEMEM
    \ingroup avr_eeprom
    Attribute expression causing a variable to be allocated within the
    .eeprom section.	*/
#define EEMEM __attribute__((section(".eeprom")))


/* Register definitions */

/* Check for aliases. */
#if	!defined(EEWE) && defined(EEPE)
# define EEWE EEPE
#endif

#if	!defined(EEMWE) && defined(EEMPE)
# define EEMWE EEMPE
#endif

#if	!defined(EECR) && defined(DEECR)
/* AT86RF401 */
# define EECR  DEECR
# define EEAR  DEEAR
# define EEARL DEEAR
# define EEDR  DEEDR
# define EERE  EER
# define EEWE  EEL
# define EEMWE EEU
#endif


#if	!defined(EECR) || !defined(EEDR) || !defined(EEARL)

# if	 !defined(__EEPROM_REG_LOCATIONS__) \
      && !defined(EEPROM_REG_LOCATIONS_OVERRIDE)
   /* 6-byte string denoting where to find the EEPROM registers in memory
      space.  Adresses denoted in hex syntax with uppercase letters. Used
      by the EEPROM subroutines.
	First two letters:  EECR address.
	Second two letters: EEDR address.
	Last two letters:   EEAR address.
    */
#  error "Unknown EEPROM register(s) location."
# endif

/* If needed, override the locations defined in the IO headers. */
# ifdef  EEPROM_REG_LOCATIONS_OVERRIDE
#  undef  __EEPROM_REG_LOCATIONS__
#  define __EEPROM_REG_LOCATIONS__ EEPROM_REG_LOCATIONS_OVERRIDE
# endif

# define CONCAT1(a, b) CONCAT2(a, b)
# define CONCAT2(a, b) a ## b
# define HEXNR CONCAT1(0x, __EEPROM_REG_LOCATIONS__)

# undef EECR
# define EECR _SFR_IO8((HEXNR >> 16) & 0xFF)

# undef EEDR
# define EEDR _SFR_IO8((HEXNR >> 8) & 0xFF)

# undef EEAR
# define EEAR _SFR_IO8(HEXNR & 0xFF)

# undef EEARH

# undef EEARL
# define EEARL EEAR

#endif


/** \def eeprom_is_ready
    \ingroup avr_eeprom
    \returns 1 if EEPROM is ready for a new read/write operation, 0 if not.
 */
#if	defined(__DOXYGEN__)
# define eeprom_is_ready()
#elif	defined(DEECR)
# define eeprom_is_ready() bit_is_clear(DEECR, BSY)
#else
# define eeprom_is_ready() bit_is_clear(EECR, EEWE)
#endif


/** \def eeprom_busy_wait
    \ingroup avr_eeprom
    Loops until the eeprom is no longer busy.
    \returns Nothing.
 */ 	 
#define eeprom_busy_wait() do {} while (!eeprom_is_ready())


/** \ingroup avr_eeprom
    Read one byte from EEPROM address \a __p.
 */
__ATTR_PURE__ static __inline__ uint8_t eeprom_read_byte (const uint8_t *__p)
{
    do {} while (!eeprom_is_ready ());
#if E2END <= 0xFF
    EEARL = (uint8_t)(uint16_t)__p;
#else
    EEAR = (uint16_t)__p;
#endif
    /* Use inline assembly below as some AVRs have problems with accessing
    EECR with STS instructions. For example, see errata for ATmega64. 
    The code below also assumes that EECR and EEDR are in the I/O space.
    */
    uint8_t __result;
    __asm__ __volatile__
    (
        "/* START EEPROM READ CRITICAL SECTION */ \n\t"
        "sbi %1, %2 \n\t"
        "in %0, %3 \n\t"
        "/* END EEPROM READ CRITICAL SECTION */ \n\t"
        : "=r" (__result)
        : "i" (_SFR_IO_ADDR(EECR)),
          "i" (EERE),
          "i" (_SFR_IO_ADDR(EEDR))
    );
    return __result;
}

/** \ingroup avr_eeprom
    Read one 16-bit word (little endian) from EEPROM address \a __p.
 */
__ATTR_PURE__ static __inline__ uint16_t eeprom_read_word (const uint16_t *__p)
{
#if (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) )
    return __eerd_word (__p, eeprom_read_byte);
#else
    /* If ATmega256x device, do not call function. */
    union
    {
        uint16_t word;
        struct
        {
            uint8_t lo;
            uint8_t hi;
        } byte;
    } x;

    x.byte.lo = eeprom_read_byte ((const uint8_t *)__p);
    x.byte.hi = eeprom_read_byte ((const uint8_t *)__p + 1);
    return x.word;
#endif
}

/** \ingroup avr_eeprom
    Read one 32-bit double word (little endian) from EEPROM address \a __p.
 */
__ATTR_PURE__ static __inline__
uint32_t eeprom_read_dword (const uint32_t *__p)
{
#if (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) )
    return __eerd_dword (__p, eeprom_read_byte);
#else
    /* If ATmega256x device, do not call function. */
    union
    {
        uint32_t dword;
        struct
        {
            uint8_t byte0;
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
        } byte;
    } x;

    x.byte.byte0 = eeprom_read_byte ((const uint8_t *)__p);
    x.byte.byte1 = eeprom_read_byte ((const uint8_t *)__p + 1);
    x.byte.byte2 = eeprom_read_byte ((const uint8_t *)__p + 2);
    x.byte.byte3 = eeprom_read_byte ((const uint8_t *)__p + 3);
    return x.dword;
#endif
}

/** \ingroup avr_eeprom
    Read a block of \a __n bytes from EEPROM address \a __src to SRAM
    \a __dst.
 */
static __inline__ void
eeprom_read_block (void *__dst, const void *__src, size_t __n)
{
#if (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) )
    __eerd_block (__dst, __src, __n, eeprom_read_byte);
#else
    /* If ATmega256x device, do not call function. */
	char	*_myDstPtr;
	char	*_mySrcPtr;
	
	_myDstPtr	=	(char *)__dst;
	_mySrcPtr	=	(char *)__src;
	while (__n--)
    {
    //*	Jul  6,	2010	modifed by Mark Sproul to work with the 2560
	//	*(char *)__dst++	=	eeprom_read_byte((const uint8_t *)__src++);
		*_myDstPtr			=	eeprom_read_byte((const uint8_t *)_mySrcPtr);
		_myDstPtr++;
		_mySrcPtr++;
    }
#endif
}

/** \ingroup avr_eeprom
    Write a byte \a __value to EEPROM address \a __p.
 */
static __inline__ void eeprom_write_byte (uint8_t *__p, uint8_t __value)
{
    do {} while (!eeprom_is_ready ());

#if	defined(EEPM0) && defined(EEPM1)
    EECR = 0;		/* Set programming mode: erase and write.	*/
#elif	defined(EEPM0) || defined(EEPM1)
# warning "Unknown EECR register, eeprom_write_byte() has become outdated."
#endif

#if	E2END <= 0xFF
    EEARL = (unsigned)__p;
#else
    EEAR = (unsigned)__p;
#endif
    EEDR = __value;

    __asm__ __volatile__ (
        "/* START EEPROM WRITE CRITICAL SECTION */\n\t"
        "in	r0, %[__sreg]		\n\t"
        "cli				\n\t"
        "sbi	%[__eecr], %[__eemwe]	\n\t"
        "sbi	%[__eecr], %[__eewe]	\n\t"
        "out	%[__sreg], r0		\n\t"
        "/* END EEPROM WRITE CRITICAL SECTION */"
        :
        : [__eecr]  "i" (_SFR_IO_ADDR(EECR)),
          [__sreg]  "i" (_SFR_IO_ADDR(SREG)),
          [__eemwe] "i" (EEMWE),
          [__eewe]  "i" (EEWE)
        : "r0"
    );
}

/** \ingroup avr_eeprom
    Write a word \a __value to EEPROM address \a __p.
 */
static __inline__ void eeprom_write_word (uint16_t *__p, uint16_t __value)
{
#if (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) )
    __eewr_word (__p, __value, eeprom_write_byte);
#else
    /* If ATmega256x device, do not call function. */
    union
    {
        uint16_t word;
        struct
        {
            uint8_t lo;
            uint8_t hi;
        } byte;
    } x;

    x.word = __value;
    eeprom_write_byte ((uint8_t *)__p, x.byte.lo);
    eeprom_write_byte ((uint8_t *)__p + 1, x.byte.hi);
#endif
}

/** \ingroup avr_eeprom
    Write a 32-bit double word \a __value to EEPROM address \a __p.
 */
static __inline__ void eeprom_write_dword (uint32_t *__p, uint32_t __value)
{
#if (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) )
    __eewr_dword (__p, __value, eeprom_write_byte);
#else
    /* If ATmega256x device, do not call function. */
    union
    {
        uint32_t dword;
        struct
        {
            uint8_t byte0;
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
        } byte;
    } x;

    x.dword = __value;
    eeprom_write_byte ((uint8_t *)__p, x.byte.byte0);
    eeprom_write_byte ((uint8_t *)__p + 1, x.byte.byte1);
    eeprom_write_byte ((uint8_t *)__p + 2, x.byte.byte2);
    eeprom_write_byte ((uint8_t *)__p + 3, x.byte.byte3);
#endif
}

/** \ingroup avr_eeprom
    Write a block of \a __n bytes to EEPROM address \a __dst from \a __src.
    \note The argument order is mismatch with common functions like strcpy().
 */
static __inline__ void
eeprom_write_block (const void *__src, void *__dst, size_t __n)
{
#if (! (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)) )
    __eewr_block (__dst, __src, __n, eeprom_write_byte);
#else
    /* If ATmega256x device, do not call function. */
	uint8_t	*_myDstPtr;
	uint8_t	*_mySrcPtr;

    //*	Jul  6,	2010	modifed by Mark Sproul to work with the 2560
	_myDstPtr	=	(uint8_t *)__dst;
	_mySrcPtr	=	(uint8_t *)__src;

	while (__n--)
	{
	//	eeprom_write_byte((uint8_t *)__dst++, *(uint8_t *)__src++);
		eeprom_write_byte(_myDstPtr++, *_mySrcPtr++);
	}
#endif
}

/** \name IAR C compatibility defines	*/
/*@{*/

/** \def _EEPUT
    \ingroup avr_eeprom
    Write a byte to EEPROM. Compatibility define for IAR C.	*/
#define _EEPUT(addr, val) eeprom_write_byte ((uint8_t *)(addr), (uint8_t)(val))

/** \def _EEGET
    \ingroup avr_eeprom
    Read a byte from EEPROM. Compatibility define for IAR C.	*/
#define _EEGET(var, addr) (var) = eeprom_read_byte ((const uint8_t *)(addr))

/*@}*/

#endif	/* E2END || defined(__DOXYGEN__) */

#ifdef __cplusplus
}
#endif

#endif	/* !_AVR_EEPROM_H */
