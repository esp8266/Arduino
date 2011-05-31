// nonstandard construct and destroy functions -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
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
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
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

/** @file stl_construct.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _STL_CONSTRUCT_H
#define _STL_CONSTRUCT_H 1

#include <new>
#include <bits/move.h>

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   * Constructs an object in existing memory by invoking an allocated
   * object's constructor with an initializer.
   */
  template<typename _T1, typename _T2>
    inline void
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    // Allow perfect forwarding
    _Construct(_T1* __p, _T2&& __value)
#else
    _Construct(_T1* __p, const _T2& __value)
#endif
    {
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_]allocator::construct
      ::new(static_cast<void*>(__p)) _T1(_GLIBCXX_FORWARD(_T2, __value));
    }

  /**
   * Destroy the object pointed to by a pointer type.
   */
  template<typename _Tp>
    inline void
    _Destroy(_Tp* __pointer)
    { __pointer->~_Tp(); }

  template<bool>
    struct _Destroy_aux
    {
      template<typename _ForwardIterator>
        static void
        __destroy(_ForwardIterator __first, _ForwardIterator __last)
	{
	  for (; __first != __last; ++__first)
	    std::_Destroy(&*__first);
	}
    };

  template<>
    struct _Destroy_aux<true>
    {
      template<typename _ForwardIterator>
        static void
        __destroy(_ForwardIterator, _ForwardIterator) { }
    };

  /**
   * Destroy a range of objects.  If the value_type of the object has
   * a trivial destructor, the compiler should optimize all of this
   * away, otherwise the objects' destructors must be invoked.
   */
  template<typename _ForwardIterator>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
                       _Value_type;
      std::_Destroy_aux<__has_trivial_destructor(_Value_type)>::
	__destroy(__first, __last);
    }

  /**
   * Destroy a range of objects using the supplied allocator.  For
   * nondefault allocators we do not optimize away invocation of 
   * destroy() even if _Tp has a trivial destructor.
   */

  template <typename _Tp> class allocator;

  template<typename _ForwardIterator, typename _Allocator>
    void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last,
	     _Allocator& __alloc)
    {
      for (; __first != __last; ++__first)
	__alloc.destroy(&*__first);
    }

  template<typename _ForwardIterator, typename _Tp>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last,
	     allocator<_Tp>&)
    {
      _Destroy(__first, __last);
    }

_GLIBCXX_END_NAMESPACE

#endif /* _STL_CONSTRUCT_H */

