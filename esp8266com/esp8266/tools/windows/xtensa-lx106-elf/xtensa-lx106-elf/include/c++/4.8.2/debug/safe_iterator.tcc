// Debugging iterator implementation (out of line) -*- C++ -*-

// Copyright (C) 2003-2013 Free Software Foundation, Inc.
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

/** @file debug/safe_iterator.tcc
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_SAFE_ITERATOR_TCC
#define _GLIBCXX_DEBUG_SAFE_ITERATOR_TCC 1

namespace __gnu_debug
{
  template<typename _Iterator, typename _Sequence>
    bool
    _Safe_iterator<_Iterator, _Sequence>::
    _M_can_advance(const difference_type& __n) const
    {
      typedef typename _Sequence::const_iterator const_debug_iterator;
      typedef typename const_debug_iterator::iterator_type const_iterator;

      if (this->_M_singular())
	return false;
      if (__n == 0)
	return true;
      if (__n < 0)
	{
	  const_iterator __begin = _M_get_sequence()->_M_base().begin();
	  std::pair<difference_type, _Distance_precision> __dist =
	    __get_distance(__begin, base());
	  bool __ok =  ((__dist.second == __dp_exact && __dist.first >= -__n)
			|| (__dist.second != __dp_exact && __dist.first > 0));
	  return __ok;
	}
      else
	{
	  const_iterator __end = _M_get_sequence()->_M_base().end();
	  std::pair<difference_type, _Distance_precision> __dist =
	    __get_distance(base(), __end);
	  bool __ok = ((__dist.second == __dp_exact && __dist.first >= __n)
		       || (__dist.second != __dp_exact && __dist.first > 0));
	  return __ok;
	}
    }

  template<typename _Iterator, typename _Sequence>
    template<typename _Other>
      bool
      _Safe_iterator<_Iterator, _Sequence>::
      _M_valid_range(const _Safe_iterator<_Other, _Sequence>& __rhs) const
      {
	if (!_M_can_compare(__rhs))
	  return false;

	/* Determine if we can order the iterators without the help of
	   the container */
	std::pair<difference_type, _Distance_precision> __dist =
	  __get_distance(base(), __rhs.base());
	switch (__dist.second) {
	case __dp_equality:
	  if (__dist.first == 0)
	    return true;
	  break;

	case __dp_sign:
	case __dp_exact:
	  return __dist.first >= 0;
	}

	/* We can only test for equality, but check if one of the
	   iterators is at an extreme. */
	/* Optim for classic [begin, it) or [it, end) ranges, limit checks
	 * when code is valid.  Note, for the special case of forward_list,
	 * before_begin replaces the role of begin.  */ 
	if (_M_is_beginnest() || __rhs._M_is_end())
	  return true;
	if (_M_is_end() || __rhs._M_is_beginnest())
	  return false;

	// Assume that this is a valid range; we can't check anything else
	return true;
      }
} // namespace __gnu_debug

#endif

