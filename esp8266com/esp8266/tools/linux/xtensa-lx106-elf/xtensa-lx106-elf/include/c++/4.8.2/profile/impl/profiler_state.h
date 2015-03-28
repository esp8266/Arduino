// -*- C++ -*-
//
// Copyright (C) 2009-2013 Free Software Foundation, Inc.
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

/** @file profile/impl/profiler_state.h
 *  @brief Global profiler state.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_STATE_H
#define _GLIBCXX_PROFILE_PROFILER_STATE_H 1

namespace __gnu_profile
{
  enum __state_type { __ON, __OFF, __INVALID };

  _GLIBCXX_PROFILE_DEFINE_DATA(__state_type, __state, __INVALID);

  inline bool
  __turn(__state_type __s)
  { 
    __state_type inv(__INVALID);
    return __atomic_compare_exchange_n(&_GLIBCXX_PROFILE_DATA(__state),
				       &inv, __s, false, __ATOMIC_ACQ_REL, 
				       __ATOMIC_RELAXED);
  }

  inline bool
  __turn_on()
  { return __turn(__ON); }

  inline bool
  __turn_off()
  { return __turn(__OFF); }

  inline bool
  __is_on()
  { return _GLIBCXX_PROFILE_DATA(__state) == __ON; }

  inline bool
  __is_off()
  { return _GLIBCXX_PROFILE_DATA(__state) == __OFF; }

  inline bool
  __is_invalid()
  { return _GLIBCXX_PROFILE_DATA(__state) == __INVALID; }

} // end namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_STATE_H */
