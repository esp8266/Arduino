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

/** @file profile/impl/profiler_hashtable_size.h
 *  @brief Collection of hashtable size traces.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_HASHTABLE_SIZE_H
#define _GLIBCXX_PROFILE_PROFILER_HASHTABLE_SIZE_H 1

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif
#include "profile/impl/profiler.h"
#include "profile/impl/profiler_node.h"
#include "profile/impl/profiler_trace.h"
#include "profile/impl/profiler_state.h"
#include "profile/impl/profiler_container_size.h"

namespace __gnu_profile
{

/** @brief Hashtable size instrumentation trace producer.  */
class __trace_hashtable_size : public __trace_container_size
{
 public:
  __trace_hashtable_size() : __trace_container_size()
  {
    __id = "hashtable-size";
  }
};

inline void __trace_hashtable_size_init()
{
  _GLIBCXX_PROFILE_DATA(_S_hashtable_size) = new __trace_hashtable_size();
}

inline void __trace_hashtable_size_report(FILE* __f, 
                                          __warning_vector_t& __warnings)
{
  if (_GLIBCXX_PROFILE_DATA(_S_hashtable_size)) {
    _GLIBCXX_PROFILE_DATA(_S_hashtable_size)->__collect_warnings(__warnings);
    _GLIBCXX_PROFILE_DATA(_S_hashtable_size)->__write(__f);
  }
}

inline void __trace_hashtable_size_construct(const void* __obj, size_t __num)
{
  if (!__profcxx_init()) return;
  
  _GLIBCXX_PROFILE_DATA(_S_hashtable_size)->__insert(__obj, __get_stack(),
                                                     __num);
}

inline void __trace_hashtable_size_destruct(const void* __obj, size_t __num, 
                                            size_t __inum)
{
  if (!__profcxx_init()) return;

  _GLIBCXX_PROFILE_DATA(_S_hashtable_size)->__destruct(__obj, __num, __inum);
}

inline void __trace_hashtable_size_resize(const void* __obj, size_t __from, 
                                          size_t __to)
{
  if (!__profcxx_init()) return;

  _GLIBCXX_PROFILE_DATA(_S_hashtable_size)->__resize(__obj, __from, __to);
}

} // namespace __gnu_profile

#endif /* _GLIBCXX_PROFILE_PROFILER_HASHTABLE_SIZE_H */
