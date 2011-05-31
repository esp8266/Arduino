// Pair implementation -*- C++ -*-

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

/** @file stl_pair.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _STL_PAIR_H
#define _STL_PAIR_H 1

#include <bits/move.h> // for std::move / std::forward, std::decay, and
                       // std::swap

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <type_traits>
#endif

_GLIBCXX_BEGIN_NAMESPACE(std)

  /// pair holds two objects of arbitrary type.
  template<class _T1, class _T2>
    struct pair
    {
      typedef _T1 first_type;    ///<  @c first_type is the first bound type
      typedef _T2 second_type;   ///<  @c second_type is the second bound type

      _T1 first;                 ///< @c first is a copy of the first object
      _T2 second;                ///< @c second is a copy of the second object

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 265.  std::pair::pair() effects overly restrictive
      /** The default constructor creates @c first and @c second using their
       *  respective default constructors.  */
      pair()
      : first(), second() { }

      /** Two objects may be passed to a @c pair constructor to be copied.  */
      pair(const _T1& __a, const _T2& __b)
      : first(__a), second(__b) { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      // DR 811.
      template<class _U1, class = typename
	       std::enable_if<std::is_convertible<_U1, _T1>::value>::type>
        pair(_U1&& __x, const _T2& __y)
	: first(std::forward<_U1>(__x)),
	  second(__y) { }

      template<class _U2, class = typename
	       std::enable_if<std::is_convertible<_U2, _T2>::value>::type>
        pair(const _T1& __x, _U2&& __y)
	: first(__x),
	  second(std::forward<_U2>(__y)) { }

      template<class _U1, class _U2, class = typename
	       std::enable_if<std::is_convertible<_U1, _T1>::value
			      && std::is_convertible<_U2, _T2>::value>::type>
        pair(_U1&& __x, _U2&& __y)
	: first(std::forward<_U1>(__x)),
	  second(std::forward<_U2>(__y)) { }
#endif

      /** There is also a templated copy ctor for the @c pair class itself.  */
      template<class _U1, class _U2>
        pair(const pair<_U1, _U2>& __p)
	: first(__p.first),
	  second(__p.second) { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      template<class _U1, class _U2>
        pair(pair<_U1, _U2>&& __p)
	: first(std::forward<_U1>(__p.first)),
	  second(std::forward<_U2>(__p.second)) { }

      pair&
      operator=(pair&& __p)
      { 
	first = std::move(__p.first);
	second = std::move(__p.second);
	return *this;
      }

      template<class _U1, class _U2>
        pair&
        operator=(pair<_U1, _U2>&& __p)
	{
	  first = std::move(__p.first);
	  second = std::move(__p.second);
	  return *this;
	}

      void
      swap(pair& __p)
      {
	using std::swap;
	swap(first, __p.first);
	swap(second, __p.second);	
      }
#endif
    };

  /// Two pairs of the same type are equal iff their members are equal.
  template<class _T1, class _T2>
    inline bool
    operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __x.first == __y.first && __x.second == __y.second; }

  /// <http://gcc.gnu.org/onlinedocs/libstdc++/manual/utilities.html>
  template<class _T1, class _T2>
    inline bool
    operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __x.first < __y.first
	     || (!(__y.first < __x.first) && __x.second < __y.second); }

  /// Uses @c operator== to find the result.
  template<class _T1, class _T2>
    inline bool
    operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__x == __y); }

  /// Uses @c operator< to find the result.
  template<class _T1, class _T2>
    inline bool
    operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __y < __x; }

  /// Uses @c operator< to find the result.
  template<class _T1, class _T2>
    inline bool
    operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__y < __x); }

  /// Uses @c operator< to find the result.
  template<class _T1, class _T2>
    inline bool
    operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__x < __y); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  /// See std::pair::swap().
  // Note:  no std::swap overloads in C++03 mode, this has performance
  //        implications, see, eg, libstdc++/38466.
  template<class _T1, class _T2>
    inline void
    swap(pair<_T1, _T2>& __x, pair<_T1, _T2>& __y)
    { __x.swap(__y); }
#endif

  /**
   *  @brief A convenience wrapper for creating a pair from two objects.
   *  @param  x  The first object.
   *  @param  y  The second object.
   *  @return   A newly-constructed pair<> object of the appropriate type.
   *
   *  The standard requires that the objects be passed by reference-to-const,
   *  but LWG issue #181 says they should be passed by const value.  We follow
   *  the LWG by default.
   */
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 181.  make_pair() unintended behavior
#ifndef __GXX_EXPERIMENTAL_CXX0X__
  template<class _T1, class _T2>
    inline pair<_T1, _T2>
    make_pair(_T1 __x, _T2 __y)
    { return pair<_T1, _T2>(__x, __y); }
#else
  template<typename _Tp>
    class reference_wrapper;

  // Helper which adds a reference to a type when given a reference_wrapper
  template<typename _Tp>
    struct __strip_reference_wrapper
    {
      typedef _Tp __type;
    };

  template<typename _Tp>
    struct __strip_reference_wrapper<reference_wrapper<_Tp> >
    {
      typedef _Tp& __type;
    };

  template<typename _Tp>
    struct __strip_reference_wrapper<const reference_wrapper<_Tp> >
    {
      typedef _Tp& __type;
    };

  template<typename _Tp>
    struct __decay_and_strip
    {
      typedef typename __strip_reference_wrapper<
	typename decay<_Tp>::type>::__type __type;
    };

  // NB: DR 706.
  template<class _T1, class _T2>
    inline pair<typename __decay_and_strip<_T1>::__type,
		typename __decay_and_strip<_T2>::__type>
    make_pair(_T1&& __x, _T2&& __y)
    {
      return pair<typename __decay_and_strip<_T1>::__type,
	          typename __decay_and_strip<_T2>::__type>
	(std::forward<_T1>(__x), std::forward<_T2>(__y));
    }
#endif

_GLIBCXX_END_NAMESPACE

#endif /* _STL_PAIR_H */
