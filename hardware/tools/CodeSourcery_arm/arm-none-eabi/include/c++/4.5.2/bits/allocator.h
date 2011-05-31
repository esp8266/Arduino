// Allocators -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010
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

/*
 * Copyright (c) 1996-1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file allocator.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H 1

// Define the base class to std::allocator.
#include <bits/c++allocator.h>

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   * @defgroup allocators Allocators
   * @ingroup memory
   *
   * Classes encapsulating memory operations.
   */

  template<typename _Tp>
    class allocator;

  /// allocator<void> specialization.
  template<>
    class allocator<void>
    {
    public:
      typedef size_t      size_type;
      typedef ptrdiff_t   difference_type;
      typedef void*       pointer;
      typedef const void* const_pointer;
      typedef void        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef allocator<_Tp1> other; };
    };

  /**
   * @brief  The @a standard allocator, as per [20.4].
   * @ingroup allocators
   *
   *  Further details:
   *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt04ch11.html
   */
  template<typename _Tp>
    class allocator: public __glibcxx_base_allocator<_Tp>
    {
   public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef allocator<_Tp1> other; };

      allocator() throw() { }

      allocator(const allocator& __a) throw()
      : __glibcxx_base_allocator<_Tp>(__a) { }

      template<typename _Tp1>
        allocator(const allocator<_Tp1>&) throw() { }

      ~allocator() throw() { }

      // Inherit everything else.
    };

  template<typename _T1, typename _T2>
    inline bool
    operator==(const allocator<_T1>&, const allocator<_T2>&)
    { return true; }

  template<typename _Tp>
    inline bool
    operator==(const allocator<_Tp>&, const allocator<_Tp>&)
    { return true; }

  template<typename _T1, typename _T2>
    inline bool
    operator!=(const allocator<_T1>&, const allocator<_T2>&)
    { return false; }

  template<typename _Tp>
    inline bool
    operator!=(const allocator<_Tp>&, const allocator<_Tp>&)
    { return false; }

  // Inhibit implicit instantiations for required instantiations,
  // which are defined via explicit instantiations elsewhere.
  // NB: This syntax is a GNU extension.
#if _GLIBCXX_EXTERN_TEMPLATE
  extern template class allocator<char>;
  extern template class allocator<wchar_t>;
#endif

  // Undefine.
#undef __glibcxx_base_allocator

  // To implement Option 3 of DR 431.
  template<typename _Alloc, bool = __is_empty(_Alloc)>
    struct __alloc_swap
    { static void _S_do_it(_Alloc&, _Alloc&) { } };

  template<typename _Alloc>
    struct __alloc_swap<_Alloc, false>
    {
      static void
      _S_do_it(_Alloc& __one, _Alloc& __two)
      {
	// Precondition: swappable allocators.
	if (__one != __two)
	  swap(__one, __two);
      }
    };

  // Optimize for stateless allocators.
  template<typename _Alloc, bool = __is_empty(_Alloc)>
    struct __alloc_neq
    {
      static bool
      _S_do_it(const _Alloc&, const _Alloc&)
      { return false; }
    };

  template<typename _Alloc>
    struct __alloc_neq<_Alloc, false>
    {
      static bool
      _S_do_it(const _Alloc& __one, const _Alloc& __two)
      { return __one != __two; }
    };

#ifdef __GXX_EXPERIMENTAL_CXX0X__
   // A very basic implementation for now.  In general we have to wait for
   // the availability of the infrastructure described in N2983:  we should
   // try when either T has a move constructor which cannot throw or T is
   // CopyContructible.
   // NB: This code doesn't properly belong here, we should find a more
   // suited place common to std::vector and std::deque.
   template<typename _Tp,
	    bool = __has_trivial_copy(typename _Tp::value_type)>
     struct __shrink_to_fit
     { static void _S_do_it(_Tp&) { } };

   template<typename _Tp>
     struct __shrink_to_fit<_Tp, true>
     {
       static void
       _S_do_it(_Tp& __v)
       {
	 __try
	   { _Tp(__v).swap(__v); }
	 __catch(...) { }
       }
     };
#endif

_GLIBCXX_END_NAMESPACE

#endif
