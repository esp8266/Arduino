// Allocator that wraps "C" malloc -*- C++ -*-

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

/** @file ext/malloc_allocator.h
 *  This file is a GNU extension to the Standard C++ Library.
 */

#ifndef _MALLOC_ALLOCATOR_H
#define _MALLOC_ALLOCATOR_H 1

#include <cstdlib>
#include <new>
#include <bits/functexcept.h>
#include <bits/move.h>

_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)

  using std::size_t;
  using std::ptrdiff_t;

  /**
   *  @brief  An allocator that uses malloc.
   *  @ingroup allocators
   *
   *  This is precisely the allocator defined in the C++ Standard. 
   *    - all allocation calls malloc
   *    - all deallocation calls free
   */
  template<typename _Tp>
    class malloc_allocator
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
        { typedef malloc_allocator<_Tp1> other; };

      malloc_allocator() throw() { }

      malloc_allocator(const malloc_allocator&) throw() { }

      template<typename _Tp1>
        malloc_allocator(const malloc_allocator<_Tp1>&) throw() { }

      ~malloc_allocator() throw() { }

      pointer
      address(reference __x) const { return &__x; }

      const_pointer
      address(const_reference __x) const { return &__x; }

      // NB: __n is permitted to be 0.  The C++ standard says nothing
      // about what the return value is when __n == 0.
      pointer
      allocate(size_type __n, const void* = 0)
      {
	if (__n > this->max_size())
	  std::__throw_bad_alloc();

	pointer __ret = static_cast<_Tp*>(std::malloc(__n * sizeof(_Tp)));
	if (!__ret)
	  std::__throw_bad_alloc();
	return __ret;
      }

      // __p is not permitted to be a null pointer.
      void
      deallocate(pointer __p, size_type)
      { std::free(static_cast<void*>(__p)); }

      size_type
      max_size() const throw() 
      { return size_t(-1) / sizeof(_Tp); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_] allocator::construct
      void 
      construct(pointer __p, const _Tp& __val) 
      { ::new((void *)__p) value_type(__val); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      template<typename... _Args>
        void
        construct(pointer __p, _Args&&... __args) 
	{ ::new((void *)__p) _Tp(std::forward<_Args>(__args)...); }
#endif

      void 
      destroy(pointer __p) { __p->~_Tp(); }
    };

  template<typename _Tp>
    inline bool
    operator==(const malloc_allocator<_Tp>&, const malloc_allocator<_Tp>&)
    { return true; }
  
  template<typename _Tp>
    inline bool
    operator!=(const malloc_allocator<_Tp>&, const malloc_allocator<_Tp>&)
    { return false; }

_GLIBCXX_END_NAMESPACE

#endif
