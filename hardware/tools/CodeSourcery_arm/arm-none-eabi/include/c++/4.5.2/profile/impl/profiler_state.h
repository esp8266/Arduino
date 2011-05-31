// -*- C++ -*-
//
// Copyright (C) 2009 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 2, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this library; see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.

// As a special exception, you may use this file as part of a free
// software library without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to
// produce an executable, this file does not by itself cause the
// resulting executable to be covered by the GNU General Public
// License.  This exception does not however invalidate any other
// reasons why the executable file might be covered by the GNU General
// Public License.

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

inline bool __turn(__state_type __s)
{
  return (_GLIBCXX_PROFILE_DATA(__state)
          == __sync_val_compare_and_swap(&_GLIBCXX_PROFILE_DATA(__state),
                                         __INVALID, __s));
}

inline bool __turn_on()
{ return __turn(__ON); }

inline bool __turn_off()
{ return __turn(__OFF); }

inline bool __is_on()
{ return _GLIBCXX_PROFILE_DATA(__state) == __ON; }

inline bool __is_off()
{ return _GLIBCXX_PROFILE_DATA(__state) == __OFF; }

inline bool __is_invalid()
{ return _GLIBCXX_PROFILE_DATA(__state) == __INVALID; }

} // end namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_STATE_H */
