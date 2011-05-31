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

/** @file profile/impl/profiler_map_to_unordered_map.h
 *  @brief Diagnostics for map to unordered_map.
 */

// Written by Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_MAP_TO_UNORDERED_MAP_H
#define _GLIBCXX_PROFILE_PROFILER_MAP_TO_UNORDERED_MAP_H 1

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

inline int __log2(size_t __size)
{
  for (int __bit_count = sizeof(size_t) - 1; __bit_count >= 0; -- __bit_count) 
  {
    if ((2 << __bit_count) & __size) {
      return __bit_count;
    }
  }
  return 0;
}

inline float __map_insert_cost(size_t __size)
{
  return (_GLIBCXX_PROFILE_DATA(__map_insert_cost_factor).__value 
          * static_cast<float>(__log2(__size)));
}

inline float __map_erase_cost(size_t __size)
{
  return (_GLIBCXX_PROFILE_DATA(__map_erase_cost_factor).__value
          * static_cast<float>(__log2(__size)));
}

inline float __map_find_cost(size_t __size)
{
  return (_GLIBCXX_PROFILE_DATA(__map_find_cost_factor).__value
          * static_cast<float>(__log2(__size)));
}

/** @brief A map-to-unordered_map instrumentation line in the object table.  */
class __map2umap_info: public __object_info_base
{
 public:
  __map2umap_info()
      : _M_insert(0), _M_erase(0), _M_find(0), _M_iterate(0),
        _M_map_cost(0.0), _M_umap_cost(0.0), _M_valid(true) {}
  __map2umap_info(__stack_t __stack)
      : __object_info_base(__stack), _M_insert(0), _M_erase(0), _M_find(0), 
        _M_iterate(0), _M_map_cost(0.0), _M_umap_cost(0.0), _M_valid(true) {} 
  virtual ~__map2umap_info() {}
  __map2umap_info(const __map2umap_info& o);
  void __merge(const __map2umap_info& o);
  void __write(FILE* __f) const;
  float __magnitude() const { return _M_map_cost - _M_umap_cost; }
  const char* __advice() const;

  void __record_insert(size_t __size, size_t __count);
  void __record_erase(size_t __size, size_t __count);
  void __record_find(size_t __size);
  void __record_iterate(size_t __count);
  void __record_invalidate();

 private:
  size_t _M_insert;
  size_t _M_erase;
  size_t _M_find;
  size_t _M_iterate;
  float _M_umap_cost;
  float _M_map_cost;
  bool  _M_valid;
};

inline const char* __map2umap_info::__advice() const
{
  return strdup("change std::map to std::unordered_map");
}

inline __map2umap_info::__map2umap_info(const __map2umap_info& __o)
    : __object_info_base(__o), 
      _M_insert(__o._M_insert),
      _M_erase(__o._M_erase),
      _M_find(__o._M_find),
      _M_iterate(__o._M_iterate),
      _M_map_cost(__o._M_map_cost),
      _M_umap_cost(__o._M_umap_cost),
      _M_valid(__o._M_valid)
{}

inline void __map2umap_info::__merge(const __map2umap_info& __o)
{
  _M_insert    += __o._M_insert;
  _M_erase     += __o._M_erase;
  _M_find      += __o._M_find;
  _M_map_cost  += __o._M_map_cost;
  _M_umap_cost += __o._M_umap_cost;
  _M_valid     &= __o._M_valid;
}

inline void __map2umap_info:: __record_insert(size_t __size, size_t __count)
{
  _M_insert += __count;
  _M_map_cost += __count * __map_insert_cost(__size);
  _M_umap_cost += (__count
                   * _GLIBCXX_PROFILE_DATA(__umap_insert_cost_factor).__value);
}

inline void __map2umap_info:: __record_erase(size_t __size, size_t __count)
{
  _M_erase += __count;
  _M_map_cost += __count * __map_erase_cost(__size);
  _M_umap_cost += (__count
                   * _GLIBCXX_PROFILE_DATA(__umap_erase_cost_factor).__value);
}

inline void __map2umap_info:: __record_find(size_t __size)
{
  _M_find += 1;
  _M_map_cost += __map_find_cost(__size);
  _M_umap_cost += _GLIBCXX_PROFILE_DATA(__umap_find_cost_factor).__value;
}

inline void __map2umap_info:: __record_iterate(size_t __count)
{
  _M_iterate += __count;
  _M_map_cost += (__count
                  * _GLIBCXX_PROFILE_DATA(__map_iterate_cost_factor).__value);
  _M_umap_cost += (
      __count * _GLIBCXX_PROFILE_DATA(__umap_iterate_cost_factor).__value);
}

inline void __map2umap_info:: __record_invalidate()
{
  _M_valid = false;
}

inline void __map2umap_info::__write(FILE* __f) const
{
  fprintf(__f, "%Zu %Zu %Zu %Zu %.0f %.0f %s\n",
          _M_insert, _M_erase, _M_find, _M_iterate, _M_map_cost, _M_umap_cost,
          _M_valid ? "valid" : "invalid");
}

/** @brief A map-to-unordered_map instrumentation line in the stack table.  */
class __map2umap_stack_info: public __map2umap_info
{
 public:
  __map2umap_stack_info(const __map2umap_info& o) : __map2umap_info(o) {}
};

/** @brief Map-to-unordered_map instrumentation producer.  */
class __trace_map2umap
    : public __trace_base<__map2umap_info, __map2umap_stack_info> 
{
 public:
  __trace_map2umap();
};

inline __trace_map2umap::__trace_map2umap()
    : __trace_base<__map2umap_info, __map2umap_stack_info>()
{
  __id = "map-to-unordered-map";
}

inline void __trace_map_to_unordered_map_init()
{
  _GLIBCXX_PROFILE_DATA(_S_map2umap) = new __trace_map2umap();
}

inline void __trace_map_to_unordered_map_report(
    FILE* __f, __warning_vector_t& __warnings)
{
  if (_GLIBCXX_PROFILE_DATA(_S_map2umap)) {
    _GLIBCXX_PROFILE_DATA(_S_map2umap)->__collect_warnings(__warnings);
    _GLIBCXX_PROFILE_DATA(_S_map2umap)->__write(__f);
  }
}

inline void __trace_map_to_unordered_map_construct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _GLIBCXX_PROFILE_DATA(_S_map2umap)->__add_object(
      __obj, __map2umap_info(__get_stack()));
}

inline void __trace_map_to_unordered_map_destruct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _GLIBCXX_PROFILE_DATA(_S_map2umap)->__retire_object(__obj);
}

inline void __trace_map_to_unordered_map_insert(const void* __obj, 
                                                size_t __size, size_t __count)
{
  if (!__profcxx_init()) return;

  __map2umap_info* __info =
      _GLIBCXX_PROFILE_DATA(_S_map2umap)->__get_object_info(__obj);

  if (__info) __info->__record_insert(__size, __count);
}

inline void __trace_map_to_unordered_map_erase(const void* __obj, 
                                               size_t __size, size_t __count)
{
  if (!__profcxx_init()) return;

  __map2umap_info* __info =
      _GLIBCXX_PROFILE_DATA(_S_map2umap)->__get_object_info(__obj);

  if (__info) __info->__record_erase(__size, __count);
}

inline void __trace_map_to_unordered_map_find(const void* __obj, size_t __size)
{
  if (!__profcxx_init()) return;

  __map2umap_info* __info =
      _GLIBCXX_PROFILE_DATA(_S_map2umap)->__get_object_info(__obj);

  if (__info) __info->__record_find(__size);
}

inline void __trace_map_to_unordered_map_iterate(const void* __obj, 
                                                 size_t __count)
{
  if (!__profcxx_init()) return;

  __map2umap_info* __info =
      _GLIBCXX_PROFILE_DATA(_S_map2umap)->__get_object_info(__obj);

  if (__info) __info->__record_iterate(__count);
}

inline void __trace_map_to_unordered_map_invalidate(const void* __obj)
{
  if (!__profcxx_init()) return;

  __map2umap_info* __info =
      _GLIBCXX_PROFILE_DATA(_S_map2umap)->__get_object_info(__obj);

  if (__info) __info->__record_invalidate();
}

} // namespace __gnu_profile
#endif /* _GLIBCXX_PROFILE_PROFILER_MAP_TO_UNORDERED_MAP_H */
