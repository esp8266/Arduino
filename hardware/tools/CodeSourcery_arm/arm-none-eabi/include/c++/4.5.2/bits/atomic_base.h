// -*- C++ -*- compatibility header.

// Copyright (C) 2008, 2009 Free Software Foundation, Inc.
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

/** @file bits/atomic_base.h
 *  This is a Standard C++ Library header.
 */

#include <bits/c++config.h>
#include <stddef.h>
#include <stdbool.h> // XXX need to define bool w/o stdbool.h in tr1/cstdbool

#ifndef _GLIBCXX_ATOMIC_BASE_H
#define _GLIBCXX_ATOMIC_BASE_H 1

_GLIBCXX_BEGIN_NAMESPACE(std)
_GLIBCXX_BEGIN_EXTERN_C

  /**
   * @defgroup atomics Atomics
   *
   * Components for performing atomic operations.
   * @{
   */

  /// Enumeration for memory_order
  typedef enum memory_order
    {
      memory_order_relaxed,
      memory_order_consume,
      memory_order_acquire,
      memory_order_release,
      memory_order_acq_rel,
      memory_order_seq_cst
    } memory_order;

  // Base for atomic_flag.
  typedef struct __atomic_flag_base
  {
    bool _M_i;
  } __atomic_flag_base;

#define ATOMIC_FLAG_INIT { false }

  /// 29.2 Lock-free Property
#if defined(_GLIBCXX_ATOMIC_BUILTINS_1) && defined(_GLIBCXX_ATOMIC_BUILTINS_2) \
  && defined(_GLIBCXX_ATOMIC_BUILTINS_4) && defined(_GLIBCXX_ATOMIC_BUILTINS_8)
# define _GLIBCXX_ATOMIC_PROPERTY 2
# define _GLIBCXX_ATOMIC_NAMESPACE __atomic2
#elif defined(_GLIBCXX_ATOMIC_BUILTINS_1)
# define _GLIBCXX_ATOMIC_PROPERTY 1
# define _GLIBCXX_ATOMIC_NAMESPACE __atomic1
#else
# define _GLIBCXX_ATOMIC_PROPERTY 0
# define _GLIBCXX_ATOMIC_NAMESPACE __atomic0
#endif

#define ATOMIC_INTEGRAL_LOCK_FREE _GLIBCXX_ATOMIC_PROPERTY
#define ATOMIC_ADDRESS_LOCK_FREE _GLIBCXX_ATOMIC_PROPERTY

  // Switch atomic integral base types based on C or C++.  In
  // addition, for "C" only provide type-generic macros for atomic
  // operations. (As C++ accomplishes the same thing with sets of
  // overloaded functions.
#ifdef __cplusplus
  inline namespace _GLIBCXX_ATOMIC_NAMESPACE { }
# include <bits/atomicfwd_cxx.h>
#else
# include <bits/atomicfwd_c.h>
#endif

  // Typedefs for other atomic integral types.
  typedef atomic_schar 		atomic_int_least8_t;
  typedef atomic_uchar 		atomic_uint_least8_t;
  typedef atomic_short 		atomic_int_least16_t;
  typedef atomic_ushort 	atomic_uint_least16_t;
  typedef atomic_int 		atomic_int_least32_t;
  typedef atomic_uint 		atomic_uint_least32_t;
  typedef atomic_llong 		atomic_int_least64_t;
  typedef atomic_ullong 	atomic_uint_least64_t;

  typedef atomic_schar 		atomic_int_fast8_t;
  typedef atomic_uchar 		atomic_uint_fast8_t;
  typedef atomic_short 		atomic_int_fast16_t;
  typedef atomic_ushort 	atomic_uint_fast16_t;
  typedef atomic_int 		atomic_int_fast32_t;
  typedef atomic_uint 		atomic_uint_fast32_t;
  typedef atomic_llong 		atomic_int_fast64_t;
  typedef atomic_ullong 	atomic_uint_fast64_t;

  typedef atomic_long 		atomic_intptr_t;
  typedef atomic_ulong 		atomic_uintptr_t;

  typedef atomic_long 		atomic_ssize_t;
  typedef atomic_ulong 		atomic_size_t;

  typedef atomic_llong 		atomic_intmax_t;
  typedef atomic_ullong 	atomic_uintmax_t;

  typedef atomic_long 		atomic_ptrdiff_t;

  // Accessor functions for base atomic_flag type.
  bool
  atomic_flag_test_and_set_explicit(__atomic_flag_base*, memory_order)
  _GLIBCXX_NOTHROW;

  inline bool
  atomic_flag_test_and_set(__atomic_flag_base* __a)
  { return atomic_flag_test_and_set_explicit(__a, memory_order_seq_cst); }

  void
  atomic_flag_clear_explicit(__atomic_flag_base*, memory_order)
  _GLIBCXX_NOTHROW;

  inline void
  atomic_flag_clear(__atomic_flag_base* __a)
  { atomic_flag_clear_explicit(__a, memory_order_seq_cst); }

  void
  __atomic_flag_wait_explicit(__atomic_flag_base*, memory_order)
  _GLIBCXX_NOTHROW;

  _GLIBCXX_CONST __atomic_flag_base*
  __atomic_flag_for_address(const void* __z) _GLIBCXX_NOTHROW;

  // Implementation specific defines.
#define _ATOMIC_LOAD_(__a, __x)						\
  ({__typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_;			\
    atomic_flag* __g = __atomic_flag_for_address(__p); 			\
    __atomic_flag_wait_explicit(__g, __x);				\
    __typeof__ _ATOMIC_MEMBER_ __r = *__p;				\
    atomic_flag_clear_explicit(__g, __x);		       		\
    __r; })

#define _ATOMIC_STORE_(__a, __m, __x)					\
  ({__typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_;			\
    __typeof__(__m) __v = (__m);			       		\
    atomic_flag* __g = __atomic_flag_for_address(__p); 			\
    __atomic_flag_wait_explicit(__g, __x);				\
    *__p = __v;								\
    atomic_flag_clear_explicit(__g, __x);		       		\
    __v; })

#define _ATOMIC_MODIFY_(__a, __o, __m, __x)				\
  ({__typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_;			\
    __typeof__(__m) __v = (__m);			       		\
    atomic_flag* __g = __atomic_flag_for_address(__p); 			\
    __atomic_flag_wait_explicit(__g, __x);				\
    __typeof__ _ATOMIC_MEMBER_ __r = *__p;				\
    *__p __o __v;					       		\
    atomic_flag_clear_explicit(__g, __x);		       		\
    __r; })

#define _ATOMIC_CMPEXCHNG_(__a, __e, __m, __x)				\
  ({__typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_;			\
    __typeof__(__e) __q = (__e);			       		\
    __typeof__(__m) __v = (__m);			       		\
    bool __r;						       		\
    atomic_flag* __g = __atomic_flag_for_address(__p); 			\
    __atomic_flag_wait_explicit(__g, __x);				\
    __typeof__ _ATOMIC_MEMBER_ __t__ = *__p;		       		\
    if (__t__ == *__q) { *__p = __v; __r = true; }			\
    else { *__q = __t__; __r = false; }		       			\
    atomic_flag_clear_explicit(__g, __x);		       		\
    __r; })

  // @} group atomics

_GLIBCXX_END_EXTERN_C
_GLIBCXX_END_NAMESPACE

// Inject into global namespace.
#ifdef __cplusplus

#include <atomic>

using std::memory_order;
using std::memory_order_relaxed;
using std::memory_order_consume;
using std::memory_order_acquire;
using std::memory_order_release;
using std::memory_order_acq_rel;
using std::memory_order_seq_cst;
using std::atomic_flag;
using std::atomic_bool;
using std::atomic_char;
using std::atomic_schar;
using std::atomic_uchar;
using std::atomic_short;
using std::atomic_ushort;
using std::atomic_int;
using std::atomic_uint;
using std::atomic_long;
using std::atomic_ulong;
using std::atomic_llong;
using std::atomic_ullong;
using std::atomic_wchar_t;
using std::atomic_char16_t;
using std::atomic_char32_t;
using std::atomic_address;
using std::atomic;
#endif

#endif
