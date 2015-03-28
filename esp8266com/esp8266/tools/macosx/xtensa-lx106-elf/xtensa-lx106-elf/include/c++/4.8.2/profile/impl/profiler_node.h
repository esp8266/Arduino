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

/** @file profile/impl/profiler_node.h
 *  @brief Data structures to represent a single profiling event.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_NODE_H
#define _GLIBCXX_PROFILE_PROFILER_NODE_H 1

#include <cstdio> // FILE, fprintf

#include <vector>
#if defined _GLIBCXX_HAVE_EXECINFO_H
#include <execinfo.h>
#endif

namespace __gnu_profile
{
  typedef const void* __object_t;
  typedef void* __instruction_address_t;
  typedef std::_GLIBCXX_STD_C::vector<__instruction_address_t> __stack_npt;
  typedef __stack_npt* __stack_t;

  std::size_t __stack_max_depth();

  inline __stack_t
  __get_stack()
  {
#if defined _GLIBCXX_HAVE_EXECINFO_H
    std::size_t __max_depth = __stack_max_depth();
    if (__max_depth == 0)
      return 0;
    __stack_npt __buffer(__max_depth);
    int __depth = backtrace(&__buffer[0], __max_depth);
    __stack_t __stack = new __stack_npt(__depth);
    __builtin_memcpy(&(*__stack)[0], &__buffer[0],
		     __depth * sizeof(__object_t));
    return __stack;
#else
    return 0;
#endif
  }

  inline std::size_t
  __size(__stack_t __stack)
  {
    if (!__stack)
      return 0;
    else
      return __stack->size();
  }

  // XXX
  inline void
  __write(FILE* __f, __stack_t __stack)
  {
    if (!__stack)
      return;
  
    __stack_npt::const_iterator __it;
    for (__it = __stack->begin(); __it != __stack->end(); ++__it)
      std::fprintf(__f, "%p ", *__it);
  }

  /** @brief Hash function for summary trace using call stack as index.  */
  class __stack_hash 
  {
  public:
    std::size_t
    operator()(__stack_t __s) const
    {
      if (!__s) 
	return 0;

      std::size_t __index = 0;
      __stack_npt::const_iterator __it;
      for (__it = __s->begin(); __it != __s->end(); ++__it)
	__index += reinterpret_cast<std::size_t>(*__it);
      return __index;
    }

    bool operator() (__stack_t __stack1, __stack_t __stack2) const
    {
      if (!__stack1 && !__stack2)
	return true;
      if (!__stack1 || !__stack2)
	return false;
      if (__stack1->size() != __stack2->size())
	return false;

      std::size_t __byte_size
	= __stack1->size() * sizeof(__stack_npt::value_type);
      return __builtin_memcmp(&(*__stack1)[0], &(*__stack2)[0],
			      __byte_size) == 0;
    }
  };


  /** @brief Base class for a line in the object table.  */
  class __object_info_base
  {
  public:
    __object_info_base() { }

    __object_info_base(__stack_t __stack)
    : _M_stack(__stack), _M_valid(true) { }

    __object_info_base(const __object_info_base& __o)
    : _M_stack(__o._M_stack), _M_valid(__o._M_valid) { }

    virtual ~__object_info_base() { }

    bool
    __is_valid() const 
    { return _M_valid; }
    
    __stack_t
    __stack() const
    { return _M_stack; }
    
    virtual void __write(FILE* __f) const = 0;

  protected:
    __stack_t _M_stack;
    bool _M_valid;
  };


  /** @brief Base class for a line in the stack table.  */
  template<typename __object_info>
    class __stack_info_base
    {
    public:
      __stack_info_base() { }
      __stack_info_base(const __object_info& __info) = 0;
      virtual ~__stack_info_base() {}
      void __merge(const __object_info& __info) = 0;
      virtual float __magnitude() const = 0;
      virtual const char* __get_id() const = 0;
    };

} // namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_NODE_H */
