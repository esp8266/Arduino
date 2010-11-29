/* Copyright (c) 2002,2004,2005 Marek Michalkiewicz
   Copyright (c) 2005, Carlos Lamas
   Copyright (c) 2005,2007 Joerg Wunsch
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

/* $Id: stdint.h,v 1.10 2007/01/23 15:32:48 joerg_wunsch Exp $ */

/*
 * ISO/IEC 9899:1999  7.18 Integer types <stdint.h>
 */

#ifndef __STDINT_H_
#define __STDINT_H_

/** \file */
/** \defgroup avr_stdint <stdint.h>: Standard Integer Types
    \code #include <stdint.h> \endcode

    Use [u]intN_t if you need exactly N bits.

    Since these typedefs are mandated by the C99 standard, they are preferred
    over rolling your own typedefs.  */

/*
 * __USING_MINT8 is defined to 1 if the -mint8 option is in effect.
 */
#if __INT_MAX__ == 127
# define __USING_MINT8 1
#else
# define __USING_MINT8 0
#endif

/* Integer types */

#if defined(__DOXYGEN__)

/* doxygen gets confused by the __attribute__ stuff */

/** \name Exact-width integer types
    Integer types having exactly the specified width */

/*@{*/

/** \ingroup avr_stdint
    8-bit signed type. */

typedef signed char int8_t;

/** \ingroup avr_stdint
    8-bit unsigned type. */

typedef unsigned char uint8_t;

/** \ingroup avr_stdint
    16-bit signed type. */

typedef signed int int16_t;

/** \ingroup avr_stdint
    16-bit unsigned type. */

typedef unsigned int uint16_t;

/** \ingroup avr_stdint
    32-bit signed type. */

typedef signed long int int32_t;

/** \ingroup avr_stdint
    32-bit unsigned type. */

typedef unsigned long int uint32_t;

/** \ingroup avr_stdint
    64-bit signed type.
    \note This type is not available when the compiler
    option -mint8 is in effect. */

typedef signed long long int int64_t;

/** \ingroup avr_stdint
    64-bit unsigned type.
    \note This type is not available when the compiler
    option -mint8 is in effect. */

typedef unsigned long long int uint64_t;

/*@}*/

#else /* !defined(__DOXYGEN__) */

/* actual implementation goes here */

typedef int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));
#if !__USING_MINT8
typedef int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
#endif

#endif /* defined(__DOXYGEN__) */

/** \name Integer types capable of holding object pointers
    These allow you to declare variables of the same size as a pointer. */

/*@{*/

/** \ingroup avr_stdint
    Signed pointer compatible type. */

typedef int16_t intptr_t;

/** \ingroup avr_stdint
    Unsigned pointer compatible type. */

typedef uint16_t uintptr_t;

/*@}*/

/** \name Minimum-width integer types
   Integer types having at least the specified width */

/*@{*/

/** \ingroup avr_stdint
    signed int with at least 8 bits. */

typedef int8_t   int_least8_t;

/** \ingroup avr_stdint
    unsigned int with at least 8 bits. */

typedef uint8_t  uint_least8_t;

/** \ingroup avr_stdint
    signed int with at least 16 bits. */

typedef int16_t  int_least16_t;

/** \ingroup avr_stdint
    unsigned int with at least 16 bits. */

typedef uint16_t uint_least16_t;

/** \ingroup avr_stdint
    signed int with at least 32 bits. */

typedef int32_t  int_least32_t;

/** \ingroup avr_stdint
    unsigned int with at least 32 bits. */

typedef uint32_t uint_least32_t;

#if !__USING_MINT8 || defined(__DOXYGEN__)
/** \ingroup avr_stdint
    signed int with at least 64 bits.
    \note This type is not available when the compiler
    option -mint8 is in effect. */

typedef int64_t  int_least64_t;

/** \ingroup avr_stdint
    unsigned int with at least 64 bits.
    \note This type is not available when the compiler
    option -mint8 is in effect. */

typedef uint64_t uint_least64_t;
#endif

/*@}*/


/** \name Fastest minimum-width integer types
   Integer types being usually fastest having at least the specified width */

/*@{*/

/** \ingroup avr_stdint
    fastest signed int with at least 8 bits. */

typedef int8_t int_fast8_t;

/** \ingroup avr_stdint
    fastest unsigned int with at least 8 bits. */

typedef uint8_t uint_fast8_t;

/** \ingroup avr_stdint
    fastest signed int with at least 16 bits. */

typedef int16_t int_fast16_t;

/** \ingroup avr_stdint
    fastest unsigned int with at least 16 bits. */

typedef uint16_t uint_fast16_t;

/** \ingroup avr_stdint
    fastest signed int with at least 32 bits. */

typedef int32_t int_fast32_t;

/** \ingroup avr_stdint
    fastest unsigned int with at least 32 bits. */

typedef uint32_t uint_fast32_t;

#if !__USING_MINT8 || defined(__DOXYGEN__)
/** \ingroup avr_stdint
    fastest signed int with at least 64 bits.
    \note This type is not available when the compiler
    option -mint8 is in effect. */

typedef int64_t int_fast64_t;

/** \ingroup avr_stdint
    fastest unsigned int with at least 64 bits.
    \note This type is not available when the compiler
    option -mint8 is in effect. */

typedef uint64_t uint_fast64_t;
#endif

/*@}*/


/** \name Greatest-width integer types
   Types designating integer data capable of representing any value of
   any integer type in the corresponding signed or unsigned category */

/*@{*/

#if __USING_MINT8
typedef int32_t intmax_t;

typedef uint32_t uintmax_t;
#else  /* !__USING_MINT8 */
/** \ingroup avr_stdint
    largest signed int available. */

typedef int64_t intmax_t;

/** \ingroup avr_stdint
    largest unsigned int available. */

typedef uint64_t uintmax_t;
#endif /* __USING_MINT8 */

/*@}*/

/* Helping macro */
#ifndef __CONCAT
#define __CONCATenate(left, right) left ## right
#define __CONCAT(left, right) __CONCATenate(left, right)
#endif

#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

/** \name Limits of specified-width integer types
   C++ implementations should define these macros only when
   __STDC_LIMIT_MACROS is defined before <stdint.h> is included */

/*@{*/

/** \ingroup avr_stdint
    largest positive value an int8_t can hold. */

#define INT8_MAX 0x7f

/** \ingroup avr_stdint
    smallest negative value an int8_t can hold. */

#define INT8_MIN (-INT8_MAX - 1)

/** \ingroup avr_stdint
    largest value an uint8_t can hold. */

#define UINT8_MAX (__CONCAT(INT8_MAX, U) * 2U + 1U)

#if __USING_MINT8

#define INT16_MAX 0x7fffL
#define INT16_MIN (-INT16_MAX - 1L)
#define UINT16_MAX (__CONCAT(INT16_MAX, U) * 2UL + 1UL)

#define INT32_MAX 0x7fffffffLL
#define INT32_MIN (-INT32_MAX - 1LL)
#define UINT32_MAX (__CONCAT(INT32_MAX, U) * 2ULL + 1ULL)

#else /* !__USING_MINT8 */

/** \ingroup avr_stdint
    largest positive value an int16_t can hold. */

#define INT16_MAX 0x7fff

/** \ingroup avr_stdint
    smallest negative value an int16_t can hold. */

#define INT16_MIN (-INT16_MAX - 1)

/** \ingroup avr_stdint
    largest value an uint16_t can hold. */

#define UINT16_MAX (__CONCAT(INT16_MAX, U) * 2U + 1U)

/** \ingroup avr_stdint
    largest positive value an int32_t can hold. */

#define INT32_MAX 0x7fffffffL

/** \ingroup avr_stdint
    smallest negative value an int32_t can hold. */

#define INT32_MIN (-INT32_MAX - 1L)

/** \ingroup avr_stdint
    largest value an uint32_t can hold. */

#define UINT32_MAX (__CONCAT(INT32_MAX, U) * 2UL + 1UL)

#endif /* __USING_MINT8 */

/** \ingroup avr_stdint
    largest positive value an int64_t can hold. */

#define INT64_MAX 0x7fffffffffffffffLL

/** \ingroup avr_stdint
    smallest negative value an int64_t can hold. */

#define INT64_MIN (-INT64_MAX - 1LL)

/** \ingroup avr_stdint
    largest value an uint64_t can hold. */

#define UINT64_MAX (__CONCAT(INT64_MAX, U) * 2ULL + 1ULL)

/*@}*/

/** \name Limits of minimum-width integer types */
/*@{*/

/** \ingroup avr_stdint
    largest positive value an int_least8_t can hold. */

#define INT_LEAST8_MAX INT8_MAX

/** \ingroup avr_stdint
    smallest negative value an int_least8_t can hold. */

#define INT_LEAST8_MIN INT8_MIN

/** \ingroup avr_stdint
    largest value an uint_least8_t can hold. */

#define UINT_LEAST8_MAX UINT8_MAX

/** \ingroup avr_stdint
    largest positive value an int_least16_t can hold. */

#define INT_LEAST16_MAX INT16_MAX

/** \ingroup avr_stdint
    smallest negative value an int_least16_t can hold. */

#define INT_LEAST16_MIN INT16_MIN

/** \ingroup avr_stdint
    largest value an uint_least16_t can hold. */

#define UINT_LEAST16_MAX UINT16_MAX

/** \ingroup avr_stdint
    largest positive value an int_least32_t can hold. */

#define INT_LEAST32_MAX INT32_MAX

/** \ingroup avr_stdint
    smallest negative value an int_least32_t can hold. */

#define INT_LEAST32_MIN INT32_MIN

/** \ingroup avr_stdint
    largest value an uint_least32_t can hold. */

#define UINT_LEAST32_MAX UINT32_MAX

/** \ingroup avr_stdint
    largest positive value an int_least64_t can hold. */

#define INT_LEAST64_MAX INT64_MAX

/** \ingroup avr_stdint
    smallest negative value an int_least64_t can hold. */

#define INT_LEAST64_MIN INT64_MIN

/** \ingroup avr_stdint
    largest value an uint_least64_t can hold. */

#define UINT_LEAST64_MAX UINT64_MAX

/*@}*/

/** \name Limits of fastest minimum-width integer types */

/*@{*/

/** \ingroup avr_stdint
    largest positive value an int_fast8_t can hold. */

#define INT_FAST8_MAX INT8_MAX

/** \ingroup avr_stdint
    smallest negative value an int_fast8_t can hold. */

#define INT_FAST8_MIN INT8_MIN

/** \ingroup avr_stdint
    largest value an uint_fast8_t can hold. */

#define UINT_FAST8_MAX UINT8_MAX

/** \ingroup avr_stdint
    largest positive value an int_fast16_t can hold. */

#define INT_FAST16_MAX INT16_MAX

/** \ingroup avr_stdint
    smallest negative value an int_fast16_t can hold. */

#define INT_FAST16_MIN INT16_MIN

/** \ingroup avr_stdint
    largest value an uint_fast16_t can hold. */

#define UINT_FAST16_MAX UINT16_MAX

/** \ingroup avr_stdint
    largest positive value an int_fast32_t can hold. */

#define INT_FAST32_MAX INT32_MAX

/** \ingroup avr_stdint
    smallest negative value an int_fast32_t can hold. */

#define INT_FAST32_MIN INT32_MIN

/** \ingroup avr_stdint
    largest value an uint_fast32_t can hold. */

#define UINT_FAST32_MAX UINT32_MAX

/** \ingroup avr_stdint
    largest positive value an int_fast64_t can hold. */

#define INT_FAST64_MAX INT64_MAX

/** \ingroup avr_stdint
    smallest negative value an int_fast64_t can hold. */

#define INT_FAST64_MIN INT64_MIN

/** \ingroup avr_stdint
    largest value an uint_fast64_t can hold. */

#define UINT_FAST64_MAX UINT64_MAX

/*@}*/

/** \name Limits of integer types capable of holding object pointers */

/*@{*/

/** \ingroup avr_stdint
    largest positive value an intptr_t can hold. */

#define INTPTR_MAX INT16_MAX

/** \ingroup avr_stdint
    smallest negative value an intptr_t can hold. */

#define INTPTR_MIN INT16_MIN

/** \ingroup avr_stdint
    largest value an uintptr_t can hold. */

#define UINTPTR_MAX UINT16_MAX

/*@}*/

/** \name Limits of greatest-width integer types */

/*@{*/

/** \ingroup avr_stdint
    largest positive value an intmax_t can hold. */

#define INTMAX_MAX INT64_MAX

/** \ingroup avr_stdint
    smallest negative value an intmax_t can hold. */

#define INTMAX_MIN INT64_MIN

/** \ingroup avr_stdint
    largest value an uintmax_t can hold. */

#define UINTMAX_MAX UINT64_MAX

/*@}*/

/** \name Limits of other integer types
    C++ implementations should define these macros only when
    __STDC_LIMIT_MACROS is defined before <stdint.h> is included */

/*@{*/

/** \ingroup avr_stdint
    largest positive value a ptrdiff_t can hold. */

#define PTRDIFF_MAX INT16_MAX

/** \ingroup avr_stdint
    smallest negative value a ptrdiff_t can hold. */

#define PTRDIFF_MIN INT16_MIN


/* Limits of sig_atomic_t */
/* signal.h is currently not implemented (not avr/signal.h) */

/** \ingroup avr_stdint
    largest positive value a sig_atomic_t can hold. */

#define SIG_ATOMIC_MAX INT8_MAX

/** \ingroup avr_stdint
    smallest negative value a sig_atomic_t can hold. */

#define SIG_ATOMIC_MIN INT8_MIN


/** \ingroup avr_stdint
    largest value a size_t can hold. */

#define SIZE_MAX (__CONCAT(INT16_MAX, U))


/* Limits of wchar_t */
/* wchar.h is currently not implemented */
/* #define WCHAR_MAX */
/* #define WCHAR_MIN */


/* Limits of wint_t */
/* wchar.h is currently not implemented */
/* #define WINT_MAX */
/* #define WINT_MIN */


#endif /* !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS) */

#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

/** \name Macros for integer constants
    C++ implementations should define these macros only when
    __STDC_CONSTANT_MACROS is defined before <stdint.h> is included.

    These definitions are valid for integer constants without suffix and
    for macros defined as integer constant without suffix */

/** \ingroup avr_stdint
    define a constant of type int8_t */

#define INT8_C(value) ((int8_t) value)

/** \ingroup avr_stdint
    define a constant of type uint8_t */

#define UINT8_C(value) ((uint8_t) __CONCAT(value, U))

#if __USING_MINT8

#define INT16_C(value) __CONCAT(value, L)
#define UINT16_C(value) __CONCAT(value, UL)

#define INT32_C(value) ((int32_t) __CONCAT(value, LL))
#define UINT32_C(value) ((uint32_t) __CONCAT(value, ULL))

#else /* !__USING_MINT8 */

/** \ingroup avr_stdint
    define a constant of type int16_t */

#define INT16_C(value) value

/** \ingroup avr_stdint
    define a constant of type uint16_t */

#define UINT16_C(value) __CONCAT(value, U)

/** \ingroup avr_stdint
    define a constant of type int32_t */

#define INT32_C(value) __CONCAT(value, L)

/** \ingroup avr_stdint
    define a constant of type uint32_t */

#define UINT32_C(value) __CONCAT(value, UL)

#endif /* __USING_MINT8 */

/** \ingroup avr_stdint
    define a constant of type int64_t */

#define INT64_C(value) __CONCAT(value, LL)

/** \ingroup avr_stdint
    define a constant of type uint64_t */

#define UINT64_C(value) __CONCAT(value, ULL)

/** \ingroup avr_stdint
    define a constant of type intmax_t */

#define INTMAX_C(value) __CONCAT(value, LL)

/** \ingroup avr_stdint
    define a constant of type uintmax_t */

#define UINTMAX_C(value) __CONCAT(value, ULL)

/*@}*/

#endif /* !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS) */


#endif /* _STDINT_H_ */
