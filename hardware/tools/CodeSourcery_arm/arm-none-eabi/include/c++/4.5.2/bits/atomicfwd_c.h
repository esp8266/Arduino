// -*- C++ -*- header.

// Copyright (C) 2008, 2009
// Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file bits/atomicfwd_c.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

// "C" only bits.

#define _ATOMIC_MEMBER_ ((__a)->_M_i)

  // POD base classes for atomic intgral types.
  struct __atomic_bool_base
  {
    bool _M_i;
  };

  struct __atomic_char_base
  {
    char _M_i;
  };

  struct __atomic_schar_base
  {
    signed char _M_i;
  };

  struct __atomic_uchar_base
  {
    unsigned char _M_i;
  };

  struct __atomic_short_base
  {
    short _M_i;
  };

  struct __atomic_ushort_base
  {
    unsigned short _M_i;
  };

  struct __atomic_int_base
  {
    int _M_i;
  };

  struct __atomic_uint_base
  {
    unsigned int _M_i;
  };

  struct __atomic_long_base
  {
    long _M_i;
  };

  struct __atomic_ulong_base
  {
    unsigned long _M_i;
  };

  struct __atomic_llong_base
  {
    long long _M_i;
  };

  struct __atomic_ullong_base
  {
    unsigned long long	_M_i;
  };

  struct __atomic_wchar_t_base
  {
    wchar_t _M_i;
  };

  typedef struct __atomic_flag_base 		atomic_flag;
  typedef struct __atomic_address_base 		atomic_address;
  typedef struct __atomic_bool_base		atomic_bool;
  typedef struct __atomic_char_base 		atomic_char;
  typedef struct __atomic_schar_base 		atomic_schar;
  typedef struct __atomic_uchar_base 		atomic_uchar;
  typedef struct __atomic_short_base 		atomic_short;
  typedef struct __atomic_ushort_base 		atomic_ushort;
  typedef struct __atomic_int_base 		atomic_int;
  typedef struct __atomic_uint_base 		atomic_uint;
  typedef struct __atomic_long_base 		atomic_long;
  typedef struct __atomic_ulong_base 		atomic_ulong;
  typedef struct __atomic_llong_base 		atomic_llong;
  typedef struct __atomic_ullong_base 		atomic_ullong;
  typedef struct __atomic_wchar_t_base 		atomic_wchar_t;
  typedef struct __atomic_short_base 		atomic_char16_t;
  typedef struct __atomic_int_base 		atomic_char32_t;

#define atomic_is_lock_free(__a)					\
  false

#define atomic_load_explicit(__a, __x)					\
  _ATOMIC_LOAD_(__a, __x)

#define atomic_load(__a)						\
  atomic_load_explicit(__a, memory_order_seq_cst)

#define atomic_store_explicit(__a, __m, __x)				\
  _ATOMIC_STORE_(__a, __m, __x)

#define atomic_store(__a, __m)						\
  atomic_store_explicit(__a, __m, memory_order_seq_cst)

#define atomic_exchange_explicit(__a, __m, __x)				\
  _ATOMIC_MODIFY_(__a, =, __m, __x)

#define atomic_exchange(__a, __m)					\
  atomic_exchange_explicit(__a, __m, memory_order_seq_cst)

#define atomic_compare_exchange_explicit(__a, __e, __m, __x, __y)	\
  _ATOMIC_CMPEXCHNG_(__a, __e, __m, __x)

#define atomic_compare_exchange(__a, __e, __m)				\
  _ATOMIC_CMPEXCHNG_(__a, __e, __m, memory_order_seq_cst)

#define atomic_fetch_add_explicit(__a, __m, __x)			\
  _ATOMIC_MODIFY_(__a, +=, __m, __x)

#define atomic_fetch_add(__a, __m)					\
  atomic_fetch_add_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_sub_explicit(__a, __m, __x)			\
  _ATOMIC_MODIFY_(__a, -=, __m, __x)

#define atomic_fetch_sub(__a, __m)					\
  atomic_fetch_sub_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_and_explicit(__a, __m, __x)			\
  _ATOMIC_MODIFY_(__a, &=, __m, __x)

#define atomic_fetch_and(__a, __m)					\
  atomic_fetch_and_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_or_explicit(__a, __m, __x) 			\
  _ATOMIC_MODIFY_(__a, |=, __m, __x)

#define atomic_fetch_or(__a, __m)					\
  atomic_fetch_or_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_xor_explicit(__a, __m, __x)			\
  _ATOMIC_MODIFY_(__a, ^=, __m, __x)

#define atomic_fetch_xor(__a, __m)					\
  atomic_fetch_xor_explicit(__a, __m, memory_order_seq_cst)
