// -*- C++ -*-
//
// Copyright (C) 2010-2013 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

/** @file profile/impl/profiler_algos.h
 *  @brief Algorithms used by the profile extension.
 *
 *  This file is needed to avoid including \<algorithm\> or \<bits/stl_algo.h\>.
 *  Including those files would result in recursive includes.
 *  These implementations are oversimplified.  In general, efficiency may be
 *  sacrificed to minimize maintenance overhead.
 */

#ifndef _GLIBCXX_PROFILE_PROFILER_ALGOS_H
#define _GLIBCXX_PROFILE_PROFILER_ALGOS_H 1

namespace __gnu_profile
{
  /* Helper for __top_n.  Insert in sorted vector, but not beyond Nth elem.  */
  template<typename _Container>
    void
    __insert_top_n(_Container& __output,
		   const typename _Container::value_type& __value,
		   typename _Container::size_type __n)
    {
      typename _Container::iterator __it = __output.begin();
      typename _Container::size_type __count = 0;

      // Skip up to N - 1 elements larger than VALUE.
      // XXX: Could do binary search for random iterators.
      while (true)
	{
	  if (__count >= __n)
	    // VALUE is not in top N.
	    return;

	  if (__it == __output.end())
	    break;

	  if (*__it < __value)
	    break;

	  ++__it;
	  ++__count;
	}

      __output.insert(__it, __value);
    }

  /* Copy the top N elements in INPUT, sorted in reverse order, to OUTPUT.  */
  template<typename _Container>
    void
    __top_n(const _Container& __input, _Container& __output,
	    typename _Container::size_type __n)
    {
      __output.clear();
      typename _Container::const_iterator __it;
      for (__it = __input.begin(); __it != __input.end(); ++__it)
	__insert_top_n(__output, *__it, __n);
    }

  /* Simplified clone of std::for_each.  */
  template<typename _InputIterator, typename _Function>
    _Function 
    __for_each(_InputIterator __first, _InputIterator __last, _Function __f)
    {
      for (; __first != __last; ++__first)
	__f(*__first);
      return __f;
    }

  /* Simplified clone of std::remove.  */
  template<typename _ForwardIterator, typename _Tp>
    _ForwardIterator
    __remove(_ForwardIterator __first, _ForwardIterator __last,
	     const _Tp& __value)
    {
      if(__first == __last)
	return __first;
      _ForwardIterator __result = __first;
      ++__first;
      for(; __first != __last; ++__first)
	if(!(*__first == __value))
	  {
	    *__result = *__first;
	    ++__result;
	  }
      return __result;
    }
} // namespace __gnu_profile

#endif /* _GLIBCXX_PROFILE_PROFILER_ALGOS_H */
