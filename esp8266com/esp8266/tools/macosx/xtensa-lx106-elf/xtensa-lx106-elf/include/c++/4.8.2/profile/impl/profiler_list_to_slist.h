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

/** @file profile/impl/profiler_list_to_slist.h
 *  @brief Diagnostics for list to slist.
 */

// Written by Changhee Jung.

#ifndef _GLIBCXX_PROFILE_PROFILER_LIST_TO_SLIST_H
#define _GLIBCXX_PROFILE_PROFILER_LIST_TO_SLIST_H 1

#include "profile/impl/profiler.h"
#include "profile/impl/profiler_node.h"
#include "profile/impl/profiler_trace.h"

namespace __gnu_profile
{
  class __list2slist_info
  : public __object_info_base
  {
  public:
    __list2slist_info()
    : _M_rewind(false), _M_operations(0) { }
  
    __list2slist_info(__stack_t __stack)
    : __object_info_base(__stack), _M_rewind(false), _M_operations(0) { }

    virtual ~__list2slist_info() { }

    __list2slist_info(const __list2slist_info& __o)
    : __object_info_base(__o), _M_rewind(__o._M_rewind),
      _M_operations(__o._M_operations) { }
  
    // XXX: the magnitude should be multiplied with a constant factor F,
    // where F is 1 when the malloc size class of list nodes is different
    // from the malloc size class of slist nodes.  When they fall into the same
    // class, the only slist benefit is from having to set fewer links, so
    // the factor F should be much smaller, closer to 0 than to 1.
    // This could be implemented by passing the size classes in the config 
    // file.  For now, we always assume F to be 1.

    float
    __magnitude() const
    {
      if (!_M_rewind)
	return _M_operations;
      else
	return 0;
    }
    
    void
    __merge(const __list2slist_info&) { }

    void
    __write(FILE* __f) const
    { std::fprintf(__f, "%s\n", _M_rewind ? "invalid" : "valid"); }

    std::string
    __advice() const
    { return "change std::list to std::forward_list"; }

    void
    __opr_rewind()
    {
      _M_rewind = true;
      _M_valid = false;
    }

    void
    __record_operation()
    { ++_M_operations; }

    bool
    __has_rewind()
    { return _M_rewind; }

  private:
    bool _M_rewind;
    std::size_t _M_operations;
  };

  class __list2slist_stack_info
  : public __list2slist_info
  {
  public:
    __list2slist_stack_info(const __list2slist_info& __o) 
    : __list2slist_info(__o) { }
  };

  class __trace_list_to_slist
  : public __trace_base<__list2slist_info, __list2slist_stack_info> 
  {
  public:
    ~__trace_list_to_slist() { }

    __trace_list_to_slist()
    : __trace_base<__list2slist_info, __list2slist_stack_info>()
    { __id = "list-to-slist"; }

    void
    __opr_rewind(const void* __obj)
    {
      __list2slist_info* __res = __get_object_info(__obj);
      if (__res)
	__res->__opr_rewind();
    }

    void
    __record_operation(const void* __obj)
    {
      __list2slist_info* __res = __get_object_info(__obj);
      if (__res)
	__res->__record_operation();
    }

    void
    __insert(const __object_t __obj, __stack_t __stack)
    { __add_object(__obj, __list2slist_info(__stack)); }
  
    void
    __destruct(const void* __obj)
    {
      if (!__is_on())
	return;

      __list2slist_info* __res = __get_object_info(__obj);
      if (!__res)
	return;

      __retire_object(__obj);
    }
  };


  inline void
  __trace_list_to_slist_init()
  { _GLIBCXX_PROFILE_DATA(_S_list_to_slist) = new __trace_list_to_slist(); }

  inline void
  __trace_list_to_slist_report(FILE* __f, __warning_vector_t& __warnings)
  {
    if (_GLIBCXX_PROFILE_DATA(_S_list_to_slist))
      {
	_GLIBCXX_PROFILE_DATA(_S_list_to_slist)->
	  __collect_warnings(__warnings);
	_GLIBCXX_PROFILE_DATA(_S_list_to_slist)->__write(__f);
      }
  }

  inline void
  __trace_list_to_slist_rewind(const void* __obj) 
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_list_to_slist)->__opr_rewind(__obj);
  }

  inline void
  __trace_list_to_slist_operation(const void* __obj) 
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_list_to_slist)->__record_operation(__obj);
  }

  inline void
  __trace_list_to_slist_construct(const void* __obj)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_list_to_slist)->__insert(__obj, __get_stack());
  }

  inline void
  __trace_list_to_slist_destruct(const void* __obj)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_list_to_slist)->__destruct(__obj);
  }

} // namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_LIST_TO_SLIST_H */
