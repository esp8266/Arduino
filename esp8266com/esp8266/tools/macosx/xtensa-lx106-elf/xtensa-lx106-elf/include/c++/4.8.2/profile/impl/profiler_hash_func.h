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

/** @file profile/impl/profiler_hash_func.h
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_HASH_FUNC_H
#define _GLIBCXX_PROFILE_PROFILER_HASH_FUNC_H 1

#include "profile/impl/profiler.h"
#include "profile/impl/profiler_node.h"
#include "profile/impl/profiler_trace.h"

namespace __gnu_profile
{
  /** @brief A hash performance instrumentation line in the object table.  */
  class __hashfunc_info
  : public __object_info_base
  {
  public:
    __hashfunc_info()
    : _M_longest_chain(0), _M_accesses(0), _M_hops(0) { }

    __hashfunc_info(const __hashfunc_info& __o)
    : __object_info_base(__o), _M_longest_chain(__o._M_longest_chain),
      _M_accesses(__o._M_accesses), _M_hops(__o._M_hops) { }

    __hashfunc_info(__stack_t __stack)
    : __object_info_base(__stack), _M_longest_chain(0),
      _M_accesses(0), _M_hops(0) { }

    virtual ~__hashfunc_info() { }

    void
    __merge(const __hashfunc_info& __o)
    {
      _M_longest_chain  = std::max(_M_longest_chain, __o._M_longest_chain);
      _M_accesses      += __o._M_accesses;
      _M_hops          += __o._M_hops;
    }

    void
    __destruct(std::size_t __chain, std::size_t __accesses,
	       std::size_t __hops)
    { 
      _M_longest_chain  = std::max(_M_longest_chain, __chain);
      _M_accesses      += __accesses;
      _M_hops          += __hops;
    }

    void
    __write(FILE* __f) const
    { std::fprintf(__f, "%Zu %Zu %Zu\n", _M_hops,
		   _M_accesses, _M_longest_chain); }

    float
    __magnitude() const
    { return static_cast<float>(_M_hops); }

    std::string
    __advice() const
    { return "change hash function"; }

  private:
    std::size_t _M_longest_chain;
    std::size_t _M_accesses;
    std::size_t _M_hops;
  };


  /** @brief A hash performance instrumentation line in the stack table.  */
  class __hashfunc_stack_info 
  : public __hashfunc_info
  {
  public:
    __hashfunc_stack_info(const __hashfunc_info& __o)
    : __hashfunc_info(__o) { }
  };


  /** @brief Hash performance instrumentation producer.  */
  class __trace_hash_func
  : public __trace_base<__hashfunc_info, __hashfunc_stack_info> 
  {
  public:
    __trace_hash_func()
    : __trace_base<__hashfunc_info, __hashfunc_stack_info>()
    { __id = "hash-distr"; }

    ~__trace_hash_func() {}
    
    // Insert a new node at construct with object, callstack and initial size.
    void
    __insert(__object_t __obj, __stack_t __stack)
    { __add_object(__obj, __hashfunc_info(__stack)); }

    // Call at destruction/clean to set container final size.
    void
    __destruct(const void* __obj, std::size_t __chain,
	       std::size_t __accesses, std::size_t __hops)
    {
      if (!__is_on())
	return;

      // First find the item from the live objects and update the informations.
      __hashfunc_info* __objs = __get_object_info(__obj);
      if (!__objs)
	return;

      __objs->__destruct(__chain, __accesses, __hops);
      __retire_object(__obj);
    }
  };


  inline void
  __trace_hash_func_init()
  { _GLIBCXX_PROFILE_DATA(_S_hash_func) = new __trace_hash_func(); }

  inline void
  __trace_hash_func_report(FILE* __f, __warning_vector_t& __warnings)
  {
    if (_GLIBCXX_PROFILE_DATA(_S_hash_func))
      {
	_GLIBCXX_PROFILE_DATA(_S_hash_func)->__collect_warnings(__warnings);
	_GLIBCXX_PROFILE_DATA(_S_hash_func)->__write(__f);
      }
  }

  inline void
  __trace_hash_func_construct(const void* __obj)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_hash_func)->__insert(__obj, __get_stack());
  }

  inline void
  __trace_hash_func_destruct(const void* __obj, std::size_t __chain,
			     std::size_t __accesses, std::size_t __hops)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_hash_func)->__destruct(__obj, __chain,
						    __accesses, __hops);
  }

} // namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_HASH_FUNC_H */
