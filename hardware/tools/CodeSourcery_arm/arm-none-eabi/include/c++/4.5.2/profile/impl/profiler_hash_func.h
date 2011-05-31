// -*- C++ -*-
//
// Copyright (C) 2009, 2010 Free Software Foundation, Inc.
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

/** @file profile/impl/profiler_trace.h
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_HASH_FUNC_H
#define _GLIBCXX_PROFILE_PROFILER_HASH_FUNC_H 1

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

namespace __gnu_profile
{

/** @brief A hash performance instrumentation line in the object table.  */
class __hashfunc_info: public __object_info_base
{
 public:
  __hashfunc_info()
      :_M_longest_chain(0), _M_accesses(0), _M_hops(0) {}
  __hashfunc_info(const __hashfunc_info& o);
  __hashfunc_info(__stack_t __stack)
      : __object_info_base(__stack),
        _M_longest_chain(0), _M_accesses(0), _M_hops(0){} 
  virtual ~__hashfunc_info() {}

  void __merge(const __hashfunc_info& __o);
  void __destruct(size_t __chain, size_t __accesses, size_t __hops);
  void __write(FILE* __f) const;
  float __magnitude() const { return static_cast<float>(_M_hops); }
  const char* __advice() const { return strdup("change hash function"); }

private:
  size_t _M_longest_chain;
  size_t _M_accesses;
  size_t _M_hops;
};

inline __hashfunc_info::__hashfunc_info(const __hashfunc_info& __o)
    : __object_info_base(__o)
{
  _M_longest_chain = __o._M_longest_chain;
  _M_accesses      = __o._M_accesses;
  _M_hops          = __o._M_hops;
}

inline void __hashfunc_info::__merge(const __hashfunc_info& __o)
{
  _M_longest_chain  = std::max(_M_longest_chain, __o._M_longest_chain);
  _M_accesses      += __o._M_accesses;
  _M_hops          += __o._M_hops;
}

inline void __hashfunc_info::__destruct(size_t __chain, size_t __accesses, 
                                        size_t __hops)
{ 
  _M_longest_chain  = std::max(_M_longest_chain, __chain);
  _M_accesses      += __accesses;
  _M_hops          += __hops;
}

/** @brief A hash performance instrumentation line in the stack table.  */
class __hashfunc_stack_info: public __hashfunc_info {
 public:
  __hashfunc_stack_info(const __hashfunc_info& __o) : __hashfunc_info(__o) {}
};

/** @brief Hash performance instrumentation producer.  */
class __trace_hash_func
    : public __trace_base<__hashfunc_info, __hashfunc_stack_info> 
{
 public:
  __trace_hash_func();
  ~__trace_hash_func() {}

  // Insert a new node at construct with object, callstack and initial size. 
  void __insert(__object_t __obj, __stack_t __stack);
  // Call at destruction/clean to set container final size.
  void __destruct(const void* __obj, size_t __chain,
                  size_t __accesses, size_t __hops);
};

inline __trace_hash_func::__trace_hash_func()
    : __trace_base<__hashfunc_info, __hashfunc_stack_info>()
{
  __id = "hash-distr";
}

inline void __trace_hash_func::__insert(__object_t __obj, __stack_t __stack)
{
  __add_object(__obj, __hashfunc_info(__stack));
}

inline void __hashfunc_info::__write(FILE* __f) const
{
  fprintf(__f, "%Zu %Zu %Zu\n", _M_hops, _M_accesses, _M_longest_chain);
}

inline void __trace_hash_func::__destruct(const void* __obj, size_t __chain,
                                          size_t __accesses, size_t __hops)
{
  if (!__is_on()) return;

  // First find the item from the live objects and update the informations.
  __hashfunc_info* __objs = __get_object_info(__obj);
  if (!__objs)
    return;

  __objs->__destruct(__chain, __accesses, __hops);
  __retire_object(__obj);
}

inline void __trace_hash_func_init()
{
  _GLIBCXX_PROFILE_DATA(_S_hash_func) = new __trace_hash_func();
}

inline void __trace_hash_func_report(FILE* __f,
                                     __warning_vector_t& __warnings)
{
  if (_GLIBCXX_PROFILE_DATA(_S_hash_func)) {
    _GLIBCXX_PROFILE_DATA(_S_hash_func)->__collect_warnings(__warnings);
    _GLIBCXX_PROFILE_DATA(_S_hash_func)->__write(__f);
  }
}

inline void __trace_hash_func_construct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _GLIBCXX_PROFILE_DATA(_S_hash_func)->__insert(__obj, __get_stack());
}

inline void __trace_hash_func_destruct(const void* __obj, size_t __chain,
                                       size_t __accesses, size_t __hops)
{
  if (!__profcxx_init()) return;

  _GLIBCXX_PROFILE_DATA(_S_hash_func)->__destruct(__obj, __chain, __accesses, 
                                                  __hops);
}

} // namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_HASH_FUNC_H */
