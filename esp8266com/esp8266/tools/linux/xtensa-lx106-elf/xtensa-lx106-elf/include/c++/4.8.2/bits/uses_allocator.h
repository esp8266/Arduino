// Uses-allocator Construction -*- C++ -*-

// Copyright (C) 2010-2013 Free Software Foundation, Inc.
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

#ifndef _USES_ALLOCATOR_H
#define _USES_ALLOCATOR_H 1

#if __cplusplus < 201103L
# include <bits/c++0x_warning.h>
#else

#include <type_traits>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /// [allocator.tag]
  struct allocator_arg_t { };

  constexpr allocator_arg_t allocator_arg = allocator_arg_t();

_GLIBCXX_HAS_NESTED_TYPE(allocator_type)

  template<typename _Tp, typename _Alloc,
	   bool = __has_allocator_type<_Tp>::value>
    struct __uses_allocator_helper
    : public false_type { };

  template<typename _Tp, typename _Alloc>
    struct __uses_allocator_helper<_Tp, _Alloc, true>
    : public integral_constant<bool, is_convertible<_Alloc,
				     typename _Tp::allocator_type>::value>
    { };

  /// [allocator.uses.trait]
  template<typename _Tp, typename _Alloc>
    struct uses_allocator
    : public integral_constant<bool,
			       __uses_allocator_helper<_Tp, _Alloc>::value>
    { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_allocator_arg
    : is_constructible<_Tp, _Alloc, _Args...>
    { static_assert( uses_allocator<_Tp, _Alloc>::value, "uses allocator" ); };

  struct __uses_alloc_base { };
  struct __uses_alloc0 : __uses_alloc_base
  { struct _Anything { _Anything(...) { } } _M_a; };
  template<typename _Alloc>
    struct __uses_alloc1 : __uses_alloc_base { const _Alloc* _M_a; };
  template<typename _Alloc>
    struct __uses_alloc2 : __uses_alloc_base { const _Alloc* _M_a; };

  template<bool, typename _Alloc, typename... _Args>
    struct __uses_alloc;

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc<true, _Tp, _Alloc, _Args...>
    : conditional<
        is_constructible<_Tp, allocator_arg_t, _Alloc, _Args...>::value,
        __uses_alloc1<_Alloc>,
       	__uses_alloc2<_Alloc>>::type
    { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc<false, _Tp, _Alloc, _Args...>
    : __uses_alloc0 { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc_impl
    : __uses_alloc<uses_allocator<_Tp, _Alloc>::value, _Tp,  _Alloc, _Args...>
    { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    __uses_alloc_impl<_Tp, _Alloc, _Args...>
    __use_alloc(const _Alloc& __a)
    {
      __uses_alloc_impl<_Tp, _Alloc, _Args...> __ret;
      __ret._M_a = &__a;
      return __ret;
    }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif
#endif
