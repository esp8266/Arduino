// Allocators -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2009 Free Software Foundation, Inc.
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

/** @file ext/debug_allocator.h
 *  This file is a GNU extension to the Standard C++ Library.
 *  You should only include this header if you are using GCC 3 or later.
 */

#ifndef _DEBUG_ALLOCATOR_H
#define _DEBUG_ALLOCATOR_H 1

#include <stdexcept>

_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)

  using std::size_t;

  /**
   *  @brief  A meta-allocator with debugging bits, as per [20.4].
   *  @ingroup allocators
   *
   *  This is precisely the allocator defined in the C++ Standard. 
   *    - all allocation calls operator new
   *    - all deallocation calls operator delete
   */
  template<typename _Alloc>
    class debug_allocator
    {
    public:
      typedef typename _Alloc::size_type       	size_type;
      typedef typename _Alloc::difference_type	difference_type;
      typedef typename _Alloc::pointer       	pointer;
      typedef typename _Alloc::const_pointer    const_pointer;
      typedef typename _Alloc::reference       	reference;
      typedef typename _Alloc::const_reference  const_reference;
      typedef typename _Alloc::value_type       value_type;

    private:
      // _M_extra is the number of objects that correspond to the
      // extra space where debug information is stored.
      size_type 		_M_extra;
      
      _Alloc			_M_allocator;

    public:
      debug_allocator()
      {
	const size_t __obj_size = sizeof(value_type);
	_M_extra = (sizeof(size_type) + __obj_size - 1) / __obj_size; 
      }
      
      pointer
      allocate(size_type __n)
      {
        pointer __res = _M_allocator.allocate(__n + _M_extra);      
	size_type* __ps = reinterpret_cast<size_type*>(__res);
	*__ps = __n;
        return __res + _M_extra;
      }

      pointer
      allocate(size_type __n, const void* __hint)
      {
        pointer __res = _M_allocator.allocate(__n + _M_extra, __hint);
	size_type* __ps = reinterpret_cast<size_type*>(__res);
	*__ps = __n;
        return __res + _M_extra;
      }

      void
      deallocate(pointer __p, size_type __n)
      {
	if (__p)
	  {
	    pointer __real_p = __p - _M_extra;
	    if (*reinterpret_cast<size_type*>(__real_p) != __n)
	      {
		throw std::runtime_error("debug_allocator::deallocate"
					 " wrong size");
	      }
	    _M_allocator.deallocate(__real_p, __n + _M_extra);
	  }
	else
	  throw std::runtime_error("debug_allocator::deallocate null pointer");
      }
    };

_GLIBCXX_END_NAMESPACE

#endif
