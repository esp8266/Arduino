// Debugging support implementation -*- C++ -*-

// Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009
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

/** @file debug/functions.h
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_FUNCTIONS_H
#define _GLIBCXX_DEBUG_FUNCTIONS_H 1

#include <bits/c++config.h>
#include <cstddef>                       // for ptrdiff_t
#include <bits/stl_iterator_base_types.h> // for iterator_traits, categories
#include <bits/cpp_type_traits.h>         // for __is_integer

namespace __gnu_debug
{
  template<typename _Iterator, typename _Sequence>
    class _Safe_iterator;

  // An arbitrary iterator pointer is not singular.
  inline bool
  __check_singular_aux(const void*) { return false; }

  // We may have an iterator that derives from _Safe_iterator_base but isn't
  // a _Safe_iterator.
  template<typename _Iterator>
    inline bool
    __check_singular(_Iterator& __x)
    { return __check_singular_aux(&__x); }

  /** Non-NULL pointers are nonsingular. */
  template<typename _Tp>
    inline bool
    __check_singular(const _Tp* __ptr)
    { return __ptr == 0; }

  /** Safe iterators know if they are singular. */
  template<typename _Iterator, typename _Sequence>
    inline bool
    __check_singular(const _Safe_iterator<_Iterator, _Sequence>& __x)
    { return __x._M_singular(); }

  /** Assume that some arbitrary iterator is dereferenceable, because we
      can't prove that it isn't. */
  template<typename _Iterator>
    inline bool
    __check_dereferenceable(_Iterator&)
    { return true; }

  /** Non-NULL pointers are dereferenceable. */
  template<typename _Tp>
    inline bool
    __check_dereferenceable(const _Tp* __ptr)
    { return __ptr; }

  /** Safe iterators know if they are singular. */
  template<typename _Iterator, typename _Sequence>
    inline bool
    __check_dereferenceable(const _Safe_iterator<_Iterator, _Sequence>& __x)
    { return __x._M_dereferenceable(); }

  /** If the distance between two random access iterators is
   *  nonnegative, assume the range is valid.
  */
  template<typename _RandomAccessIterator>
    inline bool
    __valid_range_aux2(const _RandomAccessIterator& __first,
		       const _RandomAccessIterator& __last,
		       std::random_access_iterator_tag)
    { return __last - __first >= 0; }

  /** Can't test for a valid range with input iterators, because
   *  iteration may be destructive. So we just assume that the range
   *  is valid.
  */
  template<typename _InputIterator>
    inline bool
    __valid_range_aux2(const _InputIterator&, const _InputIterator&,
		       std::input_iterator_tag)
    { return true; }

  /** We say that integral types for a valid range, and defer to other
   *  routines to realize what to do with integral types instead of
   *  iterators.
  */
  template<typename _Integral>
    inline bool
    __valid_range_aux(const _Integral&, const _Integral&, std::__true_type)
    { return true; }

  /** We have iterators, so figure out what kind of iterators that are
   *  to see if we can check the range ahead of time.
  */
  template<typename _InputIterator>
    inline bool
    __valid_range_aux(const _InputIterator& __first,
		      const _InputIterator& __last, std::__false_type)
  {
    typedef typename std::iterator_traits<_InputIterator>::iterator_category
      _Category;
    return __valid_range_aux2(__first, __last, _Category());
  }

  /** Don't know what these iterators are, or if they are even
   *  iterators (we may get an integral type for InputIterator), so
   *  see if they are integral and pass them on to the next phase
   *  otherwise.
  */
  template<typename _InputIterator>
    inline bool
    __valid_range(const _InputIterator& __first, const _InputIterator& __last)
    {
      typedef typename std::__is_integer<_InputIterator>::__type _Integral;
      return __valid_range_aux(__first, __last, _Integral());
    }

  /** Safe iterators know how to check if they form a valid range. */
  template<typename _Iterator, typename _Sequence>
    inline bool
    __valid_range(const _Safe_iterator<_Iterator, _Sequence>& __first,
		  const _Safe_iterator<_Iterator, _Sequence>& __last)
    { return __first._M_valid_range(__last); }

  /* Checks that [first, last) is a valid range, and then returns
   * __first. This routine is useful when we can't use a separate
   * assertion statement because, e.g., we are in a constructor.
  */
  template<typename _InputIterator>
    inline _InputIterator
    __check_valid_range(const _InputIterator& __first,
			const _InputIterator& __last
			__attribute__((__unused__)))
    {
      _GLIBCXX_DEBUG_ASSERT(__valid_range(__first, __last));
      return __first;
    }

  /** Checks that __s is non-NULL or __n == 0, and then returns __s. */
  template<typename _CharT, typename _Integer>
    inline const _CharT*
    __check_string(const _CharT* __s,
		   const _Integer& __n __attribute__((__unused__)))
    {
#ifdef _GLIBCXX_DEBUG_PEDANTIC
      _GLIBCXX_DEBUG_ASSERT(__s != 0 || __n == 0);
#endif
      return __s;
    }

  /** Checks that __s is non-NULL and then returns __s. */
  template<typename _CharT>
    inline const _CharT*
    __check_string(const _CharT* __s)
    {
#ifdef _GLIBCXX_DEBUG_PEDANTIC
      _GLIBCXX_DEBUG_ASSERT(__s != 0);
#endif
      return __s;
    }

  // Can't check if an input iterator sequence is sorted, because we
  // can't step through the sequence.
  template<typename _InputIterator>
    inline bool
    __check_sorted_aux(const _InputIterator&, const _InputIterator&,
                       std::input_iterator_tag)
    { return true; }

  // Can verify if a forward iterator sequence is in fact sorted using
  // std::__is_sorted
  template<typename _ForwardIterator>
    inline bool
    __check_sorted_aux(_ForwardIterator __first, _ForwardIterator __last,
                       std::forward_iterator_tag)
    {
      if (__first == __last)
        return true;

      _ForwardIterator __next = __first;
      for (++__next; __next != __last; __first = __next, ++__next)
        if (*__next < *__first)
          return false;

      return true;
    }

  // Can't check if an input iterator sequence is sorted, because we can't step
  // through the sequence.
  template<typename _InputIterator, typename _Predicate>
    inline bool
    __check_sorted_aux(const _InputIterator&, const _InputIterator&,
                       _Predicate, std::input_iterator_tag)
    { return true; }

  // Can verify if a forward iterator sequence is in fact sorted using
  // std::__is_sorted
  template<typename _ForwardIterator, typename _Predicate>
    inline bool
    __check_sorted_aux(_ForwardIterator __first, _ForwardIterator __last,
                       _Predicate __pred, std::forward_iterator_tag)
    {
      if (__first == __last)
        return true;

      _ForwardIterator __next = __first;
      for (++__next; __next != __last; __first = __next, ++__next)
        if (__pred(*__next, *__first))
          return false;

      return true;
    }

  // Determine if a sequence is sorted.
  template<typename _InputIterator>
    inline bool
    __check_sorted(const _InputIterator& __first, const _InputIterator& __last)
    {
      typedef typename std::iterator_traits<_InputIterator>::iterator_category
        _Category;

      // Verify that the < operator for elements in the sequence is a
      // StrictWeakOrdering by checking that it is irreflexive.
      _GLIBCXX_DEBUG_ASSERT(__first == __last || !(*__first < *__first));

      return __check_sorted_aux(__first, __last, _Category());
    }

  template<typename _InputIterator, typename _Predicate>
    inline bool
    __check_sorted(const _InputIterator& __first, const _InputIterator& __last,
                   _Predicate __pred)
    {
      typedef typename std::iterator_traits<_InputIterator>::iterator_category
        _Category;

      // Verify that the predicate is StrictWeakOrdering by checking that it
      // is irreflexive.
      _GLIBCXX_DEBUG_ASSERT(__first == __last || !__pred(*__first, *__first));

      return __check_sorted_aux(__first, __last, __pred, _Category());
    }

  template<typename _InputIterator>
    inline bool
    __check_sorted_set_aux(const _InputIterator& __first,
			   const _InputIterator& __last,
			   std::__true_type)
    { return __check_sorted(__first, __last); }

  template<typename _InputIterator>
    inline bool
    __check_sorted_set_aux(const _InputIterator&,
			   const _InputIterator&,
			   std::__false_type)
    { return true; }

  template<typename _InputIterator, typename _Predicate>
    inline bool
    __check_sorted_set_aux(const _InputIterator& __first,
			   const _InputIterator& __last,
			   _Predicate __pred, std::__true_type)
    { return __check_sorted(__first, __last, __pred); }

  template<typename _InputIterator, typename _Predicate>
    inline bool
    __check_sorted_set_aux(const _InputIterator&,
			   const _InputIterator&, _Predicate,
			   std::__false_type)
    { return true; }

  // ... special variant used in std::merge, std::includes, std::set_*.
  template<typename _InputIterator1, typename _InputIterator2>
    inline bool
    __check_sorted_set(const _InputIterator1& __first,
		       const _InputIterator1& __last,
		       const _InputIterator2&)
    {
      typedef typename std::iterator_traits<_InputIterator1>::value_type
	_ValueType1;
      typedef typename std::iterator_traits<_InputIterator2>::value_type
	_ValueType2;

      typedef typename std::__are_same<_ValueType1, _ValueType2>::__type
	_SameType;
      return __check_sorted_set_aux(__first, __last, _SameType());
    }

  template<typename _InputIterator1, typename _InputIterator2,
	   typename _Predicate>
    inline bool
    __check_sorted_set(const _InputIterator1& __first,
		       const _InputIterator1& __last,
		       const _InputIterator2&, _Predicate __pred)
    {
      typedef typename std::iterator_traits<_InputIterator1>::value_type
	_ValueType1;
      typedef typename std::iterator_traits<_InputIterator2>::value_type
	_ValueType2;

      typedef typename std::__are_same<_ValueType1, _ValueType2>::__type
	_SameType;
      return __check_sorted_set_aux(__first, __last, __pred, _SameType());
   }

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 270. Binary search requirements overly strict
  // Determine if a sequence is partitioned w.r.t. this element.
  template<typename _ForwardIterator, typename _Tp>
    inline bool
    __check_partitioned_lower(_ForwardIterator __first,
			      _ForwardIterator __last, const _Tp& __value)
    {
      while (__first != __last && *__first < __value)
	++__first;
      while (__first != __last && !(*__first < __value))
	++__first;
      return __first == __last;
    }

  template<typename _ForwardIterator, typename _Tp>
    inline bool
    __check_partitioned_upper(_ForwardIterator __first,
			      _ForwardIterator __last, const _Tp& __value)
    {
      while (__first != __last && !(__value < *__first))
	++__first;
      while (__first != __last && __value < *__first)
	++__first;
      return __first == __last;
    }

  // Determine if a sequence is partitioned w.r.t. this element.
  template<typename _ForwardIterator, typename _Tp, typename _Pred>
    inline bool
    __check_partitioned_lower(_ForwardIterator __first,
			      _ForwardIterator __last, const _Tp& __value,
			      _Pred __pred)
    {
      while (__first != __last && bool(__pred(*__first, __value)))
	++__first;
      while (__first != __last && !bool(__pred(*__first, __value)))
	++__first;
      return __first == __last;
    }

  template<typename _ForwardIterator, typename _Tp, typename _Pred>
    inline bool
    __check_partitioned_upper(_ForwardIterator __first,
			      _ForwardIterator __last, const _Tp& __value,
			      _Pred __pred)
    {
      while (__first != __last && !bool(__pred(__value, *__first)))
	++__first;
      while (__first != __last && bool(__pred(__value, *__first)))
	++__first;
      return __first == __last;
    }
} // namespace __gnu_debug

#endif
