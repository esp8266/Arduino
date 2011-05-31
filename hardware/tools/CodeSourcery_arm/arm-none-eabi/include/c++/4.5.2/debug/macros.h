// Debugging support implementation -*- C++ -*-

// Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010
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

/** @file debug/macros.h
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_MACROS_H
#define _GLIBCXX_DEBUG_MACROS_H 1

/**
 * Macros used by the implementation to verify certain
 * properties. These macros may only be used directly by the debug
 * wrappers. Note that these are macros (instead of the more obviously
 * @a correct choice of making them functions) because we need line and
 * file information at the call site, to minimize the distance between
 * the user error and where the error is reported.
 *
 */
#define _GLIBCXX_DEBUG_VERIFY(_Condition,_ErrorMessage)		        \
  do 									\
  {									\
    if (! (_Condition))							\
      __gnu_debug::_Error_formatter::_M_at(__FILE__, __LINE__)	        \
	  ._ErrorMessage._M_error();					\
  } while (false)

// Verify that [_First, _Last) forms a valid iterator range.
#define __glibcxx_check_valid_range(_First,_Last)			\
_GLIBCXX_DEBUG_VERIFY(__gnu_debug::__valid_range(_First, _Last),	\
		      _M_message(__gnu_debug::__msg_valid_range)	\
		      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last))

/** Verify that we can insert into *this with the iterator _Position.
 *  Insertion into a container at a specific position requires that
 *  the iterator be nonsingular (i.e., either dereferenceable or
 *  past-the-end) and that it reference the sequence we are inserting
 *  into. Note that this macro is only valid when the container is a
 *  _Safe_sequence and the iterator is a _Safe_iterator.
*/
#define __glibcxx_check_insert(_Position)				\
_GLIBCXX_DEBUG_VERIFY(!_Position._M_singular(),				\
		      _M_message(__gnu_debug::__msg_insert_singular) \
		      ._M_sequence(*this, "this")			\
		      ._M_iterator(_Position, #_Position));		\
_GLIBCXX_DEBUG_VERIFY(_Position._M_attached_to(this),			\
		      _M_message(__gnu_debug::__msg_insert_different) \
		      ._M_sequence(*this, "this")			\
		      ._M_iterator(_Position, #_Position))

/** Verify that we can insert the values in the iterator range
 *  [_First, _Last) into *this with the iterator _Position.  Insertion
 *  into a container at a specific position requires that the iterator
 *  be nonsingular (i.e., either dereferenceable or past-the-end),
 *  that it reference the sequence we are inserting into, and that the
 *  iterator range [_First, Last) is a valid (possibly empty)
 *  range. Note that this macro is only valid when the container is a
 *  _Safe_sequence and the iterator is a _Safe_iterator.
 *
 *  @todo We would like to be able to check for noninterference of
 *  _Position and the range [_First, _Last), but that can't (in
 *  general) be done.
*/
#define __glibcxx_check_insert_range(_Position,_First,_Last)		\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(!_Position._M_singular(),				\
		      _M_message(__gnu_debug::__msg_insert_singular)    \
                      ._M_sequence(*this, "this")			\
		      ._M_iterator(_Position, #_Position));		\
_GLIBCXX_DEBUG_VERIFY(_Position._M_attached_to(this),			\
		      _M_message(__gnu_debug::__msg_insert_different)   \
		      ._M_sequence(*this, "this")			\
		      ._M_iterator(_Position, #_Position))

/** Verify that we can erase the element referenced by the iterator
 * _Position. We can erase the element if the _Position iterator is
 * dereferenceable and references this sequence.
*/
#define __glibcxx_check_erase(_Position)				\
_GLIBCXX_DEBUG_VERIFY(_Position._M_dereferenceable(),			\
		      _M_message(__gnu_debug::__msg_erase_bad)	        \
                      ._M_sequence(*this, "this")			\
		      ._M_iterator(_Position, #_Position));		\
_GLIBCXX_DEBUG_VERIFY(_Position._M_attached_to(this),			\
		      _M_message(__gnu_debug::__msg_erase_different)    \
		      ._M_sequence(*this, "this")			\
		      ._M_iterator(_Position, #_Position))

/** Verify that we can erase the elements in the iterator range
 *  [_First, _Last). We can erase the elements if [_First, _Last) is a
 *  valid iterator range within this sequence.
*/
#define __glibcxx_check_erase_range(_First,_Last)			\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(_First._M_attached_to(this),			\
		      _M_message(__gnu_debug::__msg_erase_different)    \
                      ._M_sequence(*this, "this")			\
		      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last))

// Verify that the subscript _N is less than the container's size.
#define __glibcxx_check_subscript(_N)					\
_GLIBCXX_DEBUG_VERIFY(_N < this->size(),				\
		      _M_message(__gnu_debug::__msg_subscript_oob)      \
                      ._M_sequence(*this, "this")			\
		      ._M_integer(_N, #_N)				\
		      ._M_integer(this->size(), "size"))

// Verify that the container is nonempty
#define __glibcxx_check_nonempty()					\
_GLIBCXX_DEBUG_VERIFY(! this->empty(),					\
		      _M_message(__gnu_debug::__msg_empty)	        \
                      ._M_sequence(*this, "this"))

// Verify that the iterator range [_First, _Last) is sorted
#define __glibcxx_check_sorted(_First,_Last)				\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(__gnu_debug::__check_sorted(_First, _Last),	\
		      _M_message(__gnu_debug::__msg_unsorted)	        \
                      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last))

/** Verify that the iterator range [_First, _Last) is sorted by the
    predicate _Pred. */
#define __glibcxx_check_sorted_pred(_First,_Last,_Pred)			\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(__gnu_debug::__check_sorted(_First, _Last, _Pred), \
		      _M_message(__gnu_debug::__msg_unsorted_pred)      \
                      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last)			\
		      ._M_string(#_Pred))

// Special variant for std::merge, std::includes, std::set_*
#define __glibcxx_check_sorted_set(_First1,_Last1,_First2)		\
__glibcxx_check_valid_range(_First1,_Last1);				\
_GLIBCXX_DEBUG_VERIFY(                                                  \
  __gnu_debug::__check_sorted_set(_First1, _Last1, _First2),		\
  _M_message(__gnu_debug::__msg_unsorted)				\
  ._M_iterator(_First1, #_First1)					\
  ._M_iterator(_Last1, #_Last1))

// Likewise with a _Pred.
#define __glibcxx_check_sorted_set_pred(_First1,_Last1,_First2,_Pred)	\
__glibcxx_check_valid_range(_First1,_Last1);        			\
_GLIBCXX_DEBUG_VERIFY(							\
  __gnu_debug::__check_sorted_set(_First1, _Last1, _First2, _Pred),	\
  _M_message(__gnu_debug::__msg_unsorted_pred)				\
  ._M_iterator(_First1, #_First1)					\
  ._M_iterator(_Last1, #_Last1)						\
  ._M_string(#_Pred))

/** Verify that the iterator range [_First, _Last) is partitioned
    w.r.t. the value _Value. */
#define __glibcxx_check_partitioned_lower(_First,_Last,_Value)		\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(__gnu_debug::__check_partitioned_lower(_First, _Last, \
							    _Value),	\
		      _M_message(__gnu_debug::__msg_unpartitioned)      \
		      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last)			\
		      ._M_string(#_Value))

#define __glibcxx_check_partitioned_upper(_First,_Last,_Value)		\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(__gnu_debug::__check_partitioned_upper(_First, _Last, \
							    _Value),	\
		      _M_message(__gnu_debug::__msg_unpartitioned)      \
		      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last)			\
		      ._M_string(#_Value))

/** Verify that the iterator range [_First, _Last) is partitioned
    w.r.t. the value _Value and predicate _Pred. */
#define __glibcxx_check_partitioned_lower_pred(_First,_Last,_Value,_Pred) \
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(__gnu_debug::__check_partitioned_lower(_First, _Last, \
							 _Value, _Pred), \
		      _M_message(__gnu_debug::__msg_unpartitioned_pred) \
		      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last)			\
		      ._M_string(#_Pred)				\
                      ._M_string(#_Value))

/** Verify that the iterator range [_First, _Last) is partitioned
    w.r.t. the value _Value and predicate _Pred. */
#define __glibcxx_check_partitioned_upper_pred(_First,_Last,_Value,_Pred) \
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(__gnu_debug::__check_partitioned_upper(_First, _Last, \
							 _Value, _Pred), \
		      _M_message(__gnu_debug::__msg_unpartitioned_pred) \
		      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last)			\
		      ._M_string(#_Pred)				\
                      ._M_string(#_Value))

// Verify that the iterator range [_First, _Last) is a heap
#define __glibcxx_check_heap(_First,_Last)				\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(std::__is_heap(_First, _Last),		        \
		      _M_message(__gnu_debug::__msg_not_heap)	        \
		      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last))

/** Verify that the iterator range [_First, _Last) is a heap
    w.r.t. the predicate _Pred. */
#define __glibcxx_check_heap_pred(_First,_Last,_Pred)			\
__glibcxx_check_valid_range(_First,_Last);				\
_GLIBCXX_DEBUG_VERIFY(std::__is_heap(_First, _Last, _Pred),		\
		      _M_message(__gnu_debug::__msg_not_heap_pred)      \
                      ._M_iterator(_First, #_First)			\
		      ._M_iterator(_Last, #_Last)			\
		      ._M_string(#_Pred))

#ifdef _GLIBCXX_DEBUG_PEDANTIC
#  define __glibcxx_check_string(_String) _GLIBCXX_DEBUG_ASSERT(_String != 0)
#  define __glibcxx_check_string_len(_String,_Len) \
       _GLIBCXX_DEBUG_ASSERT(_String != 0 || _Len == 0)
#else
#  define __glibcxx_check_string(_String)
#  define __glibcxx_check_string_len(_String,_Len)
#endif

#endif
