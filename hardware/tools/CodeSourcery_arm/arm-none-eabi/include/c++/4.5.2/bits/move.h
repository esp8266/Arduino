// Move, forward and identity for C++0x + swap -*- C++ -*-

// Copyright (C) 2007, 2008, 2009 Free Software Foundation, Inc.
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

/** @file move.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _MOVE_H
#define _MOVE_H 1

#include <bits/c++config.h>
#include <cstddef>
#include <bits/concept_check.h>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <type_traits> // Brings in std::declval too.

_GLIBCXX_BEGIN_NAMESPACE(std)

  /// identity
  template<typename _Tp>
    struct identity
    {
      typedef _Tp type;
    };

  /// forward (as per N2835)
  /// Forward lvalues as rvalues.
  template<typename _Tp>
    inline typename enable_if<!is_lvalue_reference<_Tp>::value, _Tp&&>::type
    forward(typename std::identity<_Tp>::type& __t)
    { return static_cast<_Tp&&>(__t); }

  /// Forward rvalues as rvalues.
  template<typename _Tp>
    inline typename enable_if<!is_lvalue_reference<_Tp>::value, _Tp&&>::type
    forward(typename std::identity<_Tp>::type&& __t)
    { return static_cast<_Tp&&>(__t); }

  // Forward lvalues as lvalues.
  template<typename _Tp>
    inline typename enable_if<is_lvalue_reference<_Tp>::value, _Tp>::type
    forward(typename std::identity<_Tp>::type __t)
    { return __t; }

  // Prevent forwarding rvalues as const lvalues.
  template<typename _Tp>
    inline typename enable_if<is_lvalue_reference<_Tp>::value, _Tp>::type
    forward(typename std::remove_reference<_Tp>::type&& __t) = delete;

  /**
   *  @brief Move a value.
   *  @ingroup mutating_algorithms
   *  @param  __t  A thing of arbitrary type.
   *  @return Same, moved.
  */
  template<typename _Tp>
    inline typename std::remove_reference<_Tp>::type&&
    move(_Tp&& __t)
    { return static_cast<typename std::remove_reference<_Tp>::type&&>(__t); }

  /// declval, from type_traits.

_GLIBCXX_END_NAMESPACE

#define _GLIBCXX_MOVE(_Tp) std::move(_Tp)
#define _GLIBCXX_FORWARD(_Tp, __val) std::forward<_Tp>(__val)
#else
#define _GLIBCXX_MOVE(_Tp) (_Tp)
#define _GLIBCXX_FORWARD(_Tp, __val) (__val)
#endif

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   *  @brief Swaps two values.
   *  @ingroup mutating_algorithms
   *  @param  __a  A thing of arbitrary type.
   *  @param  __b  Another thing of arbitrary type.
   *  @return   Nothing.
  */
  template<typename _Tp>
    inline void
    swap(_Tp& __a, _Tp& __b)
    {
      // concept requirements
      __glibcxx_function_requires(_SGIAssignableConcept<_Tp>)

      _Tp __tmp = _GLIBCXX_MOVE(__a);
      __a = _GLIBCXX_MOVE(__b);
      __b = _GLIBCXX_MOVE(__tmp);
    }

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // DR 809. std::swap should be overloaded for array types.
  template<typename _Tp, size_t _Nm>
    inline void
    swap(_Tp (&__a)[_Nm], _Tp (&__b)[_Nm])
    {
      for (size_t __n = 0; __n < _Nm; ++__n)
	swap(__a[__n], __b[__n]);
    }

_GLIBCXX_END_NAMESPACE

#endif /* _MOVE_H */
