// array allocator -*- C++ -*-

// Copyright (C) 2004-2013 Free Software Foundation, Inc.
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

/** @file ext/array_allocator.h
 *  This file is a GNU extension to the Standard C++ Library.
 */

#ifndef _ARRAY_ALLOCATOR_H
#define _ARRAY_ALLOCATOR_H 1

#include <bits/c++config.h>
#include <new>
#include <bits/functexcept.h>
#include <tr1/array>
#include <bits/move.h>
#if __cplusplus >= 201103L
#include <type_traits>
#endif

namespace __gnu_cxx _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

 using std::size_t;
 using std::ptrdiff_t;

  /// Base class.
 template<typename _Tp>
    class array_allocator_base
    {
    public:
      typedef size_t     	size_type;
      typedef ptrdiff_t  	difference_type;
      typedef _Tp*       	pointer;
      typedef const _Tp* 	const_pointer;
      typedef _Tp&       	reference;
      typedef const _Tp&	const_reference;
      typedef _Tp        	value_type;

      pointer
      address(reference __x) const _GLIBCXX_NOEXCEPT
      { return std::__addressof(__x); }

      const_pointer
      address(const_reference __x) const _GLIBCXX_NOEXCEPT
      { return std::__addressof(__x); }

      void
      deallocate(pointer, size_type)
      { 
	// Does nothing.
      }

      size_type
      max_size() const _GLIBCXX_USE_NOEXCEPT 
      { return size_t(-1) / sizeof(_Tp); }

#if __cplusplus >= 201103L
      template<typename _Up, typename... _Args>
        void
        construct(_Up* __p, _Args&&... __args)
	{ ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }

      template<typename _Up>
        void 
        destroy(_Up* __p) { __p->~_Up(); }
#else
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_] allocator::construct
      void 
      construct(pointer __p, const _Tp& __val) 
      { ::new((void *)__p) value_type(__val); }

      void 
      destroy(pointer __p) { __p->~_Tp(); }
#endif
    };  

  /**
   *  @brief  An allocator that uses previously allocated memory.
   *  This memory can be externally, globally, or otherwise allocated.
   *  @ingroup allocators
   */
  template<typename _Tp, typename _Array = std::tr1::array<_Tp, 1> >
    class array_allocator : public array_allocator_base<_Tp>
    {
    public:
      typedef size_t     	size_type;
      typedef ptrdiff_t  	difference_type;
      typedef _Tp*       	pointer;
      typedef const _Tp* 	const_pointer;
      typedef _Tp&       	reference;
      typedef const _Tp& 	const_reference;
      typedef _Tp        	value_type;
      typedef _Array		array_type;

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 2103. std::allocator propagate_on_container_move_assignment
      typedef std::true_type propagate_on_container_move_assignment;
#endif

    private:
      array_type* 	_M_array;
      size_type 	_M_used;

    public:
     template<typename _Tp1, typename _Array1 = _Array>
        struct rebind
        { typedef array_allocator<_Tp1, _Array1> other; };

      array_allocator(array_type* __array = 0) _GLIBCXX_USE_NOEXCEPT 
      : _M_array(__array), _M_used(size_type()) { }

      array_allocator(const array_allocator& __o) _GLIBCXX_USE_NOEXCEPT 
      : _M_array(__o._M_array), _M_used(__o._M_used) { }

      template<typename _Tp1, typename _Array1>
        array_allocator(const array_allocator<_Tp1, _Array1>&)
	_GLIBCXX_USE_NOEXCEPT
	: _M_array(0), _M_used(size_type()) { }

      ~array_allocator() _GLIBCXX_USE_NOEXCEPT { }

      pointer
      allocate(size_type __n, const void* = 0)
      {
	if (_M_array == 0 || _M_used + __n > _M_array->size())
	  std::__throw_bad_alloc();
	pointer __ret = _M_array->begin() + _M_used;
	_M_used += __n;
	return __ret;
      }
    };

  template<typename _Tp, typename _Array>
    inline bool
    operator==(const array_allocator<_Tp, _Array>&,
	       const array_allocator<_Tp, _Array>&)
    { return true; }
  
  template<typename _Tp, typename _Array>
    inline bool
    operator!=(const array_allocator<_Tp, _Array>&, 
	       const array_allocator<_Tp, _Array>&)
    { return false; }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif
