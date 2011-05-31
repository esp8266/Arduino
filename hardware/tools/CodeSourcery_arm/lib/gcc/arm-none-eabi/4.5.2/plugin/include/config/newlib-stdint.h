/* Definitions for <stdint.h> types on systems using newlib.
   Copyright (C) 2008, 2009 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* newlib uses 32-bit long in certain cases for all non-SPU
   targets.  */
#ifndef STDINT_LONG32
#define STDINT_LONG32 (LONG_TYPE_SIZE == 32)
#endif

#define SIG_ATOMIC_TYPE "int"

/* The newlib logic actually checks for sizes greater than 32 rather
   than equal to 64 for various 64-bit types.  */

#define INT8_TYPE (CHAR_TYPE_SIZE == 8 ? "signed char" : 0)
#define INT16_TYPE (SHORT_TYPE_SIZE == 16 ? "short int" : INT_TYPE_SIZE == 16 ? "int" : CHAR_TYPE_SIZE == 16 ? "signed char" : 0)
#define INT32_TYPE (STDINT_LONG32 ? "long int" : INT_TYPE_SIZE == 32 ? "int" : SHORT_TYPE_SIZE == 32 ? "short int" : CHAR_TYPE_SIZE == 32 ? "signed char" : 0)
#define INT64_TYPE (LONG_TYPE_SIZE == 64 ? "long int" : LONG_LONG_TYPE_SIZE == 64 ? "long long int" : INT_TYPE_SIZE == 64 ? "int" : 0)
#define UINT8_TYPE (CHAR_TYPE_SIZE == 8 ? "unsigned char" : 0)
#define UINT16_TYPE (SHORT_TYPE_SIZE == 16 ? "short unsigned int" : INT_TYPE_SIZE == 16 ? "unsigned int" : CHAR_TYPE_SIZE == 16 ? "unsigned char" : 0)
#define UINT32_TYPE (STDINT_LONG32 ? "long unsigned int" : INT_TYPE_SIZE == 32 ? "unsigned int" : SHORT_TYPE_SIZE == 32 ? "short unsigned int" : CHAR_TYPE_SIZE == 32 ? "unsigned char" : 0)
#define UINT64_TYPE (LONG_TYPE_SIZE == 64 ? "long unsigned int" : LONG_LONG_TYPE_SIZE == 64 ? "long long unsigned int" : INT_TYPE_SIZE == 64 ? "unsigned int" : 0)

#define INT_LEAST8_TYPE (INT8_TYPE ? INT8_TYPE : INT16_TYPE ? INT16_TYPE : INT32_TYPE ? INT32_TYPE : INT64_TYPE ? INT64_TYPE : 0)
#define INT_LEAST16_TYPE (INT16_TYPE ? INT16_TYPE : INT32_TYPE ? INT32_TYPE : INT64_TYPE ? INT64_TYPE : 0)
#define INT_LEAST32_TYPE (INT32_TYPE ? INT32_TYPE : INT64_TYPE ? INT64_TYPE : 0)
#define INT_LEAST64_TYPE INT64_TYPE
#define UINT_LEAST8_TYPE (UINT8_TYPE ? UINT8_TYPE : UINT16_TYPE ? UINT16_TYPE : UINT32_TYPE ? UINT32_TYPE : UINT64_TYPE ? UINT64_TYPE : 0)
#define UINT_LEAST16_TYPE (UINT16_TYPE ? UINT16_TYPE : UINT32_TYPE ? UINT32_TYPE : UINT64_TYPE ? UINT64_TYPE : 0)
#define UINT_LEAST32_TYPE (UINT32_TYPE ? UINT32_TYPE : UINT64_TYPE ? UINT64_TYPE : 0)
#define UINT_LEAST64_TYPE UINT64_TYPE

#define INT_FAST8_TYPE (INT_TYPE_SIZE >= 8 ? "int" : INT_LEAST8_TYPE)
#define INT_FAST16_TYPE (INT_TYPE_SIZE >= 16 ? "int" : INT_LEAST16_TYPE)
#define INT_FAST32_TYPE (INT_TYPE_SIZE >= 32 ? "int" : INT_LEAST32_TYPE)
#define INT_FAST64_TYPE (INT_TYPE_SIZE >= 64 ? "int" : INT_LEAST64_TYPE)
#define UINT_FAST8_TYPE (INT_TYPE_SIZE >= 8 ? "unsigned int" : UINT_LEAST8_TYPE)
#define UINT_FAST16_TYPE (INT_TYPE_SIZE >= 16 ? "unsigned int" : UINT_LEAST16_TYPE)
#define UINT_FAST32_TYPE (INT_TYPE_SIZE >= 32 ? "unsigned int" : UINT_LEAST32_TYPE)
#define UINT_FAST64_TYPE (INT_TYPE_SIZE >= 64 ? "unsigned int" : UINT_LEAST64_TYPE)

/* Newlib uses the unsigned type corresponding to ptrdiff_t for
   uintptr_t; this is the same as size_t for most newlib-using
   targets.  */
#define INTPTR_TYPE PTRDIFF_TYPE
#ifndef UINTPTR_TYPE
#define UINTPTR_TYPE SIZE_TYPE
#endif
