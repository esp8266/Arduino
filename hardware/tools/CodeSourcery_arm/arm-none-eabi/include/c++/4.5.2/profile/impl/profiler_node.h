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

/** @file profile/impl/profiler_node.h
 *  @brief Data structures to represent a single profiling event.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_NODE_H
#define _GLIBCXX_PROFILE_PROFILER_NODE_H 1

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cstdio>
#include <cstdint>
#include <cstring>
#else
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif
#include <vector>
#if defined _GLIBCXX_HAVE_EXECINFO_H
#include <execinfo.h>
#endif

namespace __gnu_profile
{
typedef const void* __object_t;
typedef void* __instruction_address_t;
typedef std::_GLIBCXX_STD_PR::vector<__instruction_address_t> __stack_npt;
typedef __stack_npt* __stack_t;

size_t __stack_max_depth();

inline __stack_t __get_stack()
{
#if defined _GLIBCXX_HAVE_EXECINFO_H
  size_t __max_depth = __stack_max_depth();
  if (__max_depth == 0)
    return NULL;
  __stack_npt __buffer(__max_depth);
  int __depth = backtrace(&__buffer[0], __max_depth);
  __stack_t __stack = new __stack_npt(__depth);
  memcpy(&(*__stack)[0], &__buffer[0], __depth * sizeof(__object_t));
  return __stack;
#else
  return NULL;
#endif
}

inline __size(const __stack_t& __stack)
{
  if (!__stack) {
    return 0;
  } else {
    return __stack->size();
  }
}

inline void __write(FILE* __f, const __stack_t __stack)
{
  if (!__stack) {
    return;
  }

  __stack_npt::const_iterator __it;
  for (__it = __stack->begin(); __it != __stack->end(); ++__it) {
    fprintf(__f, "%p ", *__it);
  }
}

/** @brief Hash function for summary trace using call stack as index.  */
class __stack_hash 
{
 public:
  size_t operator()(const __stack_t __s) const
  {
    if (!__s) {
      return 0;
    }

    uintptr_t __index = 0;
    __stack_npt::const_iterator __it;
    for (__it = __s->begin(); __it != __s->end(); ++__it) {
      __index += reinterpret_cast<uintptr_t>(*__it);
    } 
    return __index;
  }

  bool operator() (const __stack_t __stack1, const __stack_t __stack2) const
  {
    if (!__stack1 && !__stack2) return true;
    if (!__stack1 || !__stack2) return false;
    if (__stack1->size() != __stack2->size()) return false;

    size_t __byte_size = __stack1->size() * sizeof(__stack_npt::value_type);
    return memcmp(&(*__stack1)[0], &(*__stack2)[0], __byte_size) == 0;
  }
};

/** @brief Base class for a line in the object table.  */
class __object_info_base
{
 public:
  __object_info_base() {}
  __object_info_base(__stack_t __stack);
  __object_info_base(const __object_info_base& o);
  virtual ~__object_info_base() {}
  bool __is_valid() const { return _M_valid; }
  __stack_t __stack() const { return _M_stack; }
  virtual void __write(FILE* f) const = 0;

 protected:
  __stack_t _M_stack;
  bool _M_valid;
};

inline __object_info_base::__object_info_base(__stack_t __stack)
{
  _M_stack = __stack;
  _M_valid = true;
}

inline __object_info_base::__object_info_base(const __object_info_base& __o)
{
  _M_stack = __o._M_stack;
  _M_valid = __o._M_valid;
}

/** @brief Base class for a line in the stack table.  */
template<typename __object_info>
class __stack_info_base
{
 public:
  __stack_info_base() {}
  __stack_info_base(const __object_info& __info) = 0;
  virtual ~__stack_info_base() {}
  void __merge(const __object_info& __info) = 0;
  virtual float __magnitude() const = 0;
  virtual const char* __get_id() const = 0;
};

} // namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_NODE_H */
