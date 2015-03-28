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

/** @file profile/impl/profiler_vector_to_list.h
 *  @brief diagnostics for vector to list.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_VECTOR_TO_LIST_H
#define _GLIBCXX_PROFILE_PROFILER_VECTOR_TO_LIST_H 1

#include "profile/impl/profiler.h"
#include "profile/impl/profiler_node.h"
#include "profile/impl/profiler_trace.h"

namespace __gnu_profile
{
  /** @brief A vector-to-list instrumentation line in the object table.  */
  class __vector2list_info
  : public __object_info_base
  {
  public:
    __vector2list_info()
    : _M_shift_count(0), _M_iterate(0), _M_resize(0), _M_list_cost(0),
      _M_vector_cost(0), _M_valid(true) { }

    __vector2list_info(__stack_t __stack)
    : __object_info_base(__stack), _M_shift_count(0), _M_iterate(0),
      _M_resize(0), _M_list_cost(0), _M_vector_cost(0), _M_valid(true) { }

    virtual ~__vector2list_info() { }

    __vector2list_info(const __vector2list_info& __o)
    : __object_info_base(__o), _M_shift_count(__o._M_shift_count),
      _M_iterate(__o._M_iterate), _M_resize(__o._M_resize),
      _M_list_cost(__o._M_list_cost), _M_vector_cost(__o._M_vector_cost),
      _M_valid(__o._M_valid) { }

    void
    __merge(const __vector2list_info& __o)
    {
      _M_shift_count  += __o._M_shift_count;
      _M_iterate      += __o._M_iterate;
      _M_vector_cost  += __o._M_vector_cost;
      _M_list_cost    += __o._M_list_cost;
      _M_valid        &= __o._M_valid;
      _M_resize       += __o._M_resize;
    }

    void
    __write(FILE* __f) const
    {
      std::fprintf(__f, "%Zu %Zu %Zu %.0f %.0f\n", _M_shift_count,
		   _M_resize, _M_iterate, _M_vector_cost, _M_list_cost);
    }

    float
    __magnitude() const
    { return _M_vector_cost - _M_list_cost; }

    std::string
    __advice() const 
    { return "change std::vector to std::list"; }

    std::size_t
    __shift_count()
    { return _M_shift_count; }

    std::size_t
    __iterate()
    { return _M_iterate; }

    float
    __list_cost()
    { return _M_list_cost; }

    std::size_t
    __resize()
    { return _M_resize; }

    void
    __set_list_cost(float __lc)
    { _M_list_cost = __lc; }
    
    void
    __set_vector_cost(float __vc)
    { _M_vector_cost = __vc; }
    
    bool
    __is_valid()
    { return _M_valid; }
    
    void
    __set_invalid()
    { _M_valid = false; }

    void
    __opr_insert(std::size_t __pos, std::size_t __num)
    { _M_shift_count += __num - __pos; }

    void
    __opr_iterate(std::size_t __num)
    { _M_iterate += __num; }

    void
    __resize(std::size_t __from, std::size_t)
    { _M_resize += __from; }

    void
    __opr_find(std::size_t __size)
    {
      // Use average case complexity.
      _M_iterate += 3.0 / 4.0 * __size;
    }

  private:
    std::size_t _M_shift_count;
    std::size_t _M_iterate;
    std::size_t _M_resize;
    float _M_list_cost;
    float _M_vector_cost;
    bool  _M_valid;
  };


  /** @brief A vector-to-list instrumentation line in the stack table.  */
  class __vector2list_stack_info
  : public __vector2list_info
  {
  public:
    __vector2list_stack_info(const __vector2list_info& __o) 
    : __vector2list_info(__o) { }
  };


  /** @brief Vector-to-list instrumentation producer.  */
  class __trace_vector_to_list
  : public __trace_base<__vector2list_info, __vector2list_stack_info> 
  {
  public:
    __trace_vector_to_list()
    : __trace_base<__vector2list_info, __vector2list_stack_info>()
    { __id = "vector-to-list"; }

    ~__trace_vector_to_list() { }

    // Insert a new node at construct with object, callstack and initial size. 
    void
    __insert(__object_t __obj, __stack_t __stack)
    { __add_object(__obj, __vector2list_info(__stack)); }

    // Call at destruction/clean to set container final size.
    void
    __destruct(const void* __obj)
    {
      if (!__is_on())
	return;

      __vector2list_info* __res = __get_object_info(__obj);
      if (!__res)
	return;

      float __vc = __vector_cost(__res->__shift_count(), __res->__iterate(),
				 __res->__resize());
      float __lc = __list_cost(__res->__shift_count(), __res->__iterate(),
			       __res->__resize());
      __res->__set_vector_cost(__vc);
      __res->__set_list_cost(__lc);

      __retire_object(__obj);
    }

    // Find the node in the live map.
    // XXX Undefined?!?
    __vector2list_info* __find(const void* __obj);

    // Collect cost of operations.
    void
    __opr_insert(const void* __obj, std::size_t __pos, std::size_t __num)
    {
      __vector2list_info* __res = __get_object_info(__obj);
      if (__res)
	__res->__opr_insert(__pos, __num);
    }

    void
    __opr_iterate(const void* __obj, std::size_t __num)
    {
      __vector2list_info* __res = __get_object_info(__obj);
      if (__res)
	__res->__opr_iterate(__num);
    }

    void
    __invalid_operator(const void* __obj)
    {
      __vector2list_info* __res = __get_object_info(__obj);
      if (__res)
	__res->__set_invalid();
    }

    void
    __resize(const void* __obj, std::size_t __from, std::size_t __to)
    {
      __vector2list_info* __res = __get_object_info(__obj);
      if (__res)
	__res->__resize(__from, __to);
    }

    float
    __vector_cost(std::size_t __shift, std::size_t __iterate,
		  std::size_t __resize)
    {
      return (__shift
	      * _GLIBCXX_PROFILE_DATA(__vector_shift_cost_factor).__value
	      + __iterate
	      * _GLIBCXX_PROFILE_DATA(__vector_iterate_cost_factor).__value
	      + __resize
	      * _GLIBCXX_PROFILE_DATA(__vector_resize_cost_factor).__value);
    }

    float
    __list_cost(std::size_t __shift, std::size_t __iterate,
		std::size_t __resize)
    {
      return (__shift
	      * _GLIBCXX_PROFILE_DATA(__list_shift_cost_factor).__value
	      + __iterate
	      * _GLIBCXX_PROFILE_DATA(__list_iterate_cost_factor).__value
	      + __resize
	      * _GLIBCXX_PROFILE_DATA(__list_resize_cost_factor).__value);
    }

    void
    __opr_find(const void* __obj, std::size_t __size)
    {
      __vector2list_info* __res = __get_object_info(__obj);
      if (__res)
	__res->__opr_find(__size);
    }
  };


  inline void
  __trace_vector_to_list_init()
  { _GLIBCXX_PROFILE_DATA(_S_vector_to_list) = new __trace_vector_to_list(); }

  inline void
  __trace_vector_to_list_report(FILE* __f, __warning_vector_t& __warnings)
  {
    if (_GLIBCXX_PROFILE_DATA(_S_vector_to_list))
      {
	_GLIBCXX_PROFILE_DATA(_S_vector_to_list)->
	  __collect_warnings(__warnings);
	_GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__write(__f);
      }
  }

  inline void
  __trace_vector_to_list_construct(const void* __obj)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__insert(__obj, __get_stack());
  }

  inline void
  __trace_vector_to_list_destruct(const void* __obj)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__destruct(__obj);
  }

  inline void
  __trace_vector_to_list_insert(const void* __obj, std::size_t __pos,
				std::size_t __num)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__opr_insert(__obj, __pos,
							   __num);
  }

  inline void
  __trace_vector_to_list_iterate(const void* __obj, std::size_t __num = 1)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__opr_iterate(__obj, __num);
  }

  inline void
  __trace_vector_to_list_invalid_operator(const void* __obj)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__invalid_operator(__obj);
  }

  inline void
  __trace_vector_to_list_resize(const void* __obj, std::size_t __from,
				std::size_t __to)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__resize(__obj, __from, __to);
  }

  inline void
  __trace_vector_to_list_find(const void* __obj, std::size_t __size)
  {
    if (!__profcxx_init())
      return;

    _GLIBCXX_PROFILE_DATA(_S_vector_to_list)->__opr_find(__obj, __size);
  }

} // namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_VECTOR_TO_LIST_H */
