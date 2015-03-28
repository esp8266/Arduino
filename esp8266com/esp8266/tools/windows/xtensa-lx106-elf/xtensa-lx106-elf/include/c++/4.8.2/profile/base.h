// -*- C++ -*-

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

/** @file profile/base.h
 *  @brief Sequential helper functions.
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

// Written by Lixia Liu

#ifndef _GLIBCXX_PROFILE_BASE_H
#define _GLIBCXX_PROFILE_BASE_H 1

#include <functional>
#include <profile/impl/profiler.h>

// Profiling mode namespaces.

/**
 * @namespace std::__profile
 * @brief GNU profile code, replaces standard behavior with profile behavior.
 */
namespace std _GLIBCXX_VISIBILITY(default) 
{ 
  namespace __profile { } 
}

/**
 * @namespace __gnu_profile
 * @brief GNU profile code for public use.
 */
namespace __gnu_profile
{
  // Import all the profile versions of components in namespace std.
  using namespace std::__profile;
}


#endif /* _GLIBCXX_PROFILE_BASE_H */
