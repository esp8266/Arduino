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

#ifndef _GLIBCXX_PROFILE_PROFILER_TRACE_H
#define _GLIBCXX_PROFILE_PROFILER_TRACE_H 1

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#define _GLIBCXX_IMPL_UNORDERED_MAP std::_GLIBCXX_STD_PR::unordered_map
#include <unordered_map>
#else
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tr1/unordered_map>
#define _GLIBCXX_IMPL_UNORDERED_MAP std::tr1::unordered_map
#endif

#include <ext/concurrence.h>
#include <fstream>
#include <string>
#include <utility>
#include <bits/stl_heap.h> // for std::make_heap, std::sort_heap

#include "profile/impl/profiler_state.h"
#include "profile/impl/profiler_node.h"

namespace __gnu_profile
{
/** @brief Internal environment.  Values can be set one of two ways:
    1. In config file "var = value".  The default config file path is 
       libstdcxx-profile.conf.
    2. By setting process environment variables.  For instance, in a Bash
       shell you can set the unit cost of iterating through a map like this:
       export __map_iterate_cost_factor=5.0.
    If a value is set both in the input file and through an environment
    variable, the environment value takes precedence.  */
typedef _GLIBCXX_IMPL_UNORDERED_MAP<std::string, std::string> __env_t;
_GLIBCXX_PROFILE_DEFINE_UNINIT_DATA(__env_t, __env);

/** @brief Master lock.  */
_GLIBCXX_PROFILE_DEFINE_UNINIT_DATA(__gnu_cxx::__mutex, __global_lock);

/** @brief Representation of a warning.  */
struct __warning_data
{
  float __magnitude;
  __stack_t __context;
  const char* __warning_id;
  const char* __warning_message;

  __warning_data()
  : __magnitude(0.0), __context(NULL), __warning_id(NULL),
    __warning_message(NULL) { }

  __warning_data(float __m, __stack_t __c, const char* __id, 
                 const char* __msg)
  : __magnitude(__m), __context(__c), __warning_id(__id),
    __warning_message(__msg) { }

  bool
  operator>(const struct __warning_data& __other) const
  { return __magnitude > __other.__magnitude; }
};

typedef std::_GLIBCXX_STD_PR::vector<__warning_data> __warning_vector_t;

// Defined in profiler_<diagnostic name>.h.
class __trace_hash_func;
class __trace_hashtable_size;
class __trace_map2umap;
class __trace_vector_size;
class __trace_vector_to_list;
class __trace_list_to_slist; 
class __trace_list_to_vector; 
void __trace_vector_size_init();
void __trace_hashtable_size_init();
void __trace_hash_func_init();
void __trace_vector_to_list_init();
void __trace_list_to_slist_init();  
void __trace_list_to_vector_init();  
void __trace_map_to_unordered_map_init();
void __trace_vector_size_report(FILE*, __warning_vector_t&);
void __trace_hashtable_size_report(FILE*, __warning_vector_t&);
void __trace_hash_func_report(FILE*, __warning_vector_t&);
void __trace_vector_to_list_report(FILE*, __warning_vector_t&);
void __trace_list_to_slist_report(FILE*, __warning_vector_t&); 
void __trace_list_to_vector_report(FILE*, __warning_vector_t&);
void __trace_map_to_unordered_map_report(FILE*, __warning_vector_t&);

struct __cost_factor
{
  const char* __env_var;
  float __value;
};

typedef std::_GLIBCXX_STD_PR::vector<__cost_factor*> __cost_factor_vector;

_GLIBCXX_PROFILE_DEFINE_DATA(__trace_hash_func*, _S_hash_func, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_hashtable_size*, _S_hashtable_size, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_map2umap*, _S_map2umap, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_vector_size*, _S_vector_size, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_vector_to_list*, _S_vector_to_list, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_list_to_slist*, _S_list_to_slist, NULL); 
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_list_to_vector*, _S_list_to_vector, NULL);

_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __vector_shift_cost_factor, 
                             {"__vector_shift_cost_factor", 1.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __vector_iterate_cost_factor,
                             {"__vector_iterate_cost_factor", 1.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __vector_resize_cost_factor,
                             {"__vector_resize_cost_factor", 1.0}); 
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __list_shift_cost_factor,
                             {"__list_shift_cost_factor", 0.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __list_iterate_cost_factor,
                             {"__list_iterate_cost_factor", 10.0}); 
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __list_resize_cost_factor,
                             {"__list_resize_cost_factor", 0.0}); 
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_insert_cost_factor,
                             {"__map_insert_cost_factor", 1.5});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_erase_cost_factor,
                             {"__map_erase_cost_factor", 1.5});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_find_cost_factor,
                             {"__map_find_cost_factor", 1});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_iterate_cost_factor,
                             {"__map_iterate_cost_factor", 2.3});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_insert_cost_factor,
                             {"__umap_insert_cost_factor", 12.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_erase_cost_factor,
                             {"__umap_erase_cost_factor", 12.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_find_cost_factor,
                             {"__umap_find_cost_factor", 10.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_iterate_cost_factor,
                             {"__umap_iterate_cost_factor", 1.7});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor_vector*, __cost_factors, NULL);

_GLIBCXX_PROFILE_DEFINE_DATA(const char*, _S_trace_file_name,
                             _GLIBCXX_PROFILE_TRACE_PATH_ROOT);
_GLIBCXX_PROFILE_DEFINE_DATA(size_t, _S_max_warn_count,
                             _GLIBCXX_PROFILE_MAX_WARN_COUNT);
_GLIBCXX_PROFILE_DEFINE_DATA(size_t, _S_max_stack_depth,
                             _GLIBCXX_PROFILE_MAX_STACK_DEPTH);
_GLIBCXX_PROFILE_DEFINE_DATA(size_t, _S_max_mem,
                             _GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC);

inline size_t __stack_max_depth()
{
  return _GLIBCXX_PROFILE_DATA(_S_max_stack_depth);
}

inline size_t __max_mem()
{
  return _GLIBCXX_PROFILE_DATA(_S_max_mem);
}

/** @brief Base class for all trace producers.  */
template <typename __object_info, typename __stack_info>
class __trace_base
{
 public:
  __trace_base();
  virtual ~__trace_base() {}

  void __add_object(__object_t object, __object_info __info);
  __object_info* __get_object_info(__object_t __object);
  void __retire_object(__object_t __object);
  void __write(FILE* f);
  void __collect_warnings(__warning_vector_t& __warnings);

 private:
  __gnu_cxx::__mutex __object_table_lock;
  __gnu_cxx::__mutex __stack_table_lock;
  typedef _GLIBCXX_IMPL_UNORDERED_MAP<__object_t, 
                                      __object_info> __object_table_t;
  typedef _GLIBCXX_IMPL_UNORDERED_MAP<__stack_t, __stack_info, __stack_hash, 
                                      __stack_hash> __stack_table_t;
  __object_table_t __object_table;
  __stack_table_t __stack_table;
  size_t __stack_table_byte_size;

 protected:
  const char* __id;
};

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__collect_warnings(
    __warning_vector_t& __warnings)
{
  typename __stack_table_t::iterator __i = __stack_table.begin();
  for ( ; __i != __stack_table.end(); ++__i )
  {
    __warnings.push_back(__warning_data((*__i).second.__magnitude(), 
                                        (*__i).first, 
                                        __id,
                                        (*__i).second.__advice()));
  }
}

template <typename __object_info, typename __stack_info>
__trace_base<__object_info, __stack_info>::__trace_base()
{
  // Do not pick the initial size too large, as we don't know which diagnostics
  // are more active.
  __object_table.rehash(10000);
  __stack_table.rehash(10000);
  __stack_table_byte_size = 0;
  __id = NULL;
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__add_object(
    __object_t __object, __object_info __info)
{
  if (__max_mem() == 0 
      || __object_table.size() * sizeof(__object_info) <= __max_mem()) {
    this->__object_table_lock.lock();
    __object_table.insert(
        typename __object_table_t::value_type(__object, __info));
    this->__object_table_lock.unlock();
  }
}

template <typename __object_info, typename __stack_info>
__object_info* __trace_base<__object_info, __stack_info>::__get_object_info(
    __object_t __object)
{
  // XXX: Revisit this to see if we can decrease mutex spans.
  // Without this mutex, the object table could be rehashed during an
  // insertion on another thread, which could result in a segfault.
  this->__object_table_lock.lock();
  typename __object_table_t::iterator __object_it = 
      __object_table.find(__object);
  if (__object_it == __object_table.end()){
    this->__object_table_lock.unlock();
    return NULL;
  } else {
    this->__object_table_lock.unlock();
    return &__object_it->second;
  }
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__retire_object(
    __object_t __object)
{
  this->__object_table_lock.lock();
  this->__stack_table_lock.lock();
  typename __object_table_t::iterator __object_it =
      __object_table.find(__object);
  if (__object_it != __object_table.end()){
    const __object_info& __info = __object_it->second;
    const __stack_t& __stack = __info.__stack();
    typename __stack_table_t::iterator __stack_it = 
        __stack_table.find(__stack);
    if (__stack_it == __stack_table.end()) {
      // First occurence of this call context.
      if (__max_mem() == 0 || __stack_table_byte_size < __max_mem()) {
        __stack_table_byte_size += 
            (sizeof(__instruction_address_t) * __size(__stack)
             + sizeof(__stack) + sizeof(__stack_info));
        __stack_table.insert(make_pair(__stack, __stack_info(__info)));
      }
    } else {
      // Merge object info into info summary for this call context.
      __stack_it->second.__merge(__info);
      delete __stack;
    }
    __object_table.erase(__object);
  }
  this->__object_table_lock.unlock();
  this->__stack_table_lock.unlock();
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__write(FILE* __f)
{
  typename __stack_table_t::iterator __it;

  for (__it = __stack_table.begin(); __it != __stack_table.end(); __it++) {
    if (__it->second.__is_valid()) {
      fprintf(__f, __id);
      fprintf(__f, "|");
      __gnu_profile::__write(__f, __it->first);
      fprintf(__f, "|");
      __it->second.__write(__f);
    }
  }
}

inline size_t __env_to_size_t(const char* __env_var, size_t __default_value)
{
  char* __env_value = getenv(__env_var);
  if (__env_value) {
    long int __converted_value = strtol(__env_value, NULL, 10);
    if (errno || __converted_value < 0) {
      fprintf(stderr, "Bad value for environment variable '%s'.\n", __env_var);
      abort();
    } else {
      return static_cast<size_t>(__converted_value);
    }
  } else {
    return __default_value;
  }
}

inline void __set_max_stack_trace_depth()
{
  _GLIBCXX_PROFILE_DATA(_S_max_stack_depth) = __env_to_size_t(
      _GLIBCXX_PROFILE_MAX_STACK_DEPTH_ENV_VAR,
      _GLIBCXX_PROFILE_DATA(_S_max_stack_depth));
}

inline void __set_max_mem()
{
  _GLIBCXX_PROFILE_DATA(_S_max_mem) = __env_to_size_t(
      _GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC_ENV_VAR,
      _GLIBCXX_PROFILE_DATA(_S_max_mem));
}

inline int __log_magnitude(float __f)
{
  const float __log_base = 10.0;
  int __result = 0;
  int __sign = 1;
  if (__f < 0) {
    __f = -__f;
    __sign = -1;
  }
  while (__f > __log_base) {
    ++__result;
    __f /= 10.0;
  }
  return __sign * __result;
}

inline FILE* __open_output_file(const char* __extension)
{
  // The path is made of _S_trace_file_name + "." + extension.
  size_t __root_len = strlen(_GLIBCXX_PROFILE_DATA(_S_trace_file_name));
  size_t __ext_len = strlen(__extension);
  char* __file_name = new char[__root_len + 1 + __ext_len + 1];
  memcpy(__file_name, _GLIBCXX_PROFILE_DATA(_S_trace_file_name), __root_len);
  *(__file_name + __root_len) = '.';
  memcpy(__file_name + __root_len + 1, __extension, __ext_len + 1);
  FILE* __out_file = fopen(__file_name, "w");
  if (__out_file) {
    return __out_file;
  } else {
    fprintf(stderr, "Could not open trace file '%s'.\n", __file_name);
    abort();
  }
}

/** @brief Final report method, registered with @b atexit.
 *
 * This can also be called directly by user code, including signal handlers.
 * It is protected against deadlocks by the reentrance guard in profiler.h.
 * However, when called from a signal handler that triggers while within
 * __gnu_profile (under the guarded zone), no output will be produced.
 */
inline void __report(void)
{
  _GLIBCXX_PROFILE_DATA(__global_lock).lock();

  __warning_vector_t __warnings;

  FILE* __raw_file = __open_output_file("raw");
  __trace_vector_size_report(__raw_file, __warnings);
  __trace_hashtable_size_report(__raw_file, __warnings);
  __trace_hash_func_report(__raw_file, __warnings);
  __trace_vector_to_list_report(__raw_file, __warnings);
  __trace_list_to_slist_report(__raw_file, __warnings);
  __trace_list_to_vector_report(__raw_file, __warnings);
  __trace_map_to_unordered_map_report(__raw_file, __warnings);
  fclose(__raw_file);

  // Sort data by magnitude.
  // XXX: instead of sorting, should collect only top N for better performance.
  size_t __cutoff = std::min(_GLIBCXX_PROFILE_DATA(_S_max_warn_count),
			     __warnings.size());

  std::make_heap(__warnings.begin(), __warnings.end(),
		 std::greater<__warning_vector_t::value_type>());
  std::sort_heap(__warnings.begin(), __warnings.end(),
		 std::greater<__warning_vector_t::value_type>());
  __warnings.resize(__cutoff);

  FILE* __warn_file = __open_output_file("txt");

  for (__warning_vector_t::iterator __it = __warnings.begin();
       __it != __warnings.end(); ++__it)
    {
      fprintf(__warn_file,  __it->__warning_id);
      fprintf(__warn_file, ": improvement = %d",
	      __log_magnitude(__it->__magnitude));
      fprintf(__warn_file, ": call stack = ");
      __gnu_profile::__write(__warn_file, __it->__context);
      fprintf(__warn_file, ": advice = %s\n", __it->__warning_message);
      free(const_cast<void*>(reinterpret_cast<const void*>
			     (__it->__warning_message)));
    }

  fclose(__warn_file);

  _GLIBCXX_PROFILE_DATA(__global_lock).unlock();
}

inline void __set_trace_path()
{
  char* __env_trace_file_name = getenv(_GLIBCXX_PROFILE_TRACE_ENV_VAR);

  if (__env_trace_file_name) { 
    _GLIBCXX_PROFILE_DATA(_S_trace_file_name) = __env_trace_file_name; 
  }

  // Make sure early that we can create the trace file.
  fclose(__open_output_file("txt"));
}

inline void __set_max_warn_count()
{
  char* __env_max_warn_count_str = getenv(
      _GLIBCXX_PROFILE_MAX_WARN_COUNT_ENV_VAR);

  if (__env_max_warn_count_str) {
    _GLIBCXX_PROFILE_DATA(_S_max_warn_count) = static_cast<size_t>(
        atoi(__env_max_warn_count_str));
  }
}

inline void __read_cost_factors()
{
  std::string __conf_file_name(_GLIBCXX_PROFILE_DATA(_S_trace_file_name));
  __conf_file_name += ".conf";

  std::ifstream __conf_file(__conf_file_name.c_str());

  if (__conf_file.is_open())
    {
      std::string __line;

      while (getline(__conf_file, __line))
	{
	  std::string::size_type __i = __line.find_first_not_of(" \t\n\v");

	  if (__line.length() <= 0 || __line[__i] == '#') {
	    // Skip empty lines or comments.
	    continue;
	  }

	  // Trim.
	  if (__line.begin() != __line.end())
	    {
	      // A simple remove operation.
	      std::string::iterator __first = __line.begin();
	      std::string::iterator __result = __first;
	      ++__first;
	      for(; __first != __line.end(); ++__first)
		if(!(*__first == ' '))
		  {
		    *__result = *__first;
		    ++__result;
		  }
	      __line.erase(__result, __line.end());
	    }
	  std::string::size_type __pos = __line.find("=");
	  std::string __factor_name = __line.substr(0, __pos);
	  std::string::size_type __end = __line.find_first_of(";\n");
	  std::string __factor_value = __line.substr(__pos + 1, __end - __pos);

          _GLIBCXX_PROFILE_DATA(__env)[__factor_name] = __factor_value;
	}
    }
}

inline void __write_cost_factors()
{
  FILE* __file = __open_output_file("conf.out");

  for (__decltype(_GLIBCXX_PROFILE_DATA(__cost_factors)->begin()) __it
	 = _GLIBCXX_PROFILE_DATA(__cost_factors)->begin();
       __it != _GLIBCXX_PROFILE_DATA(__cost_factors)->end(); ++__it)
    fprintf(__file, "%s = %f\n", (*__it)->__env_var, (*__it)->__value);

  fclose(__file);
}

inline void __set_cost_factors()
{
  _GLIBCXX_PROFILE_DATA(__cost_factors) = new __cost_factor_vector;
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__vector_shift_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__vector_iterate_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__vector_resize_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__list_shift_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__list_iterate_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__list_resize_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_insert_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_erase_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_find_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_iterate_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_insert_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_erase_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_find_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_iterate_cost_factor));

  for (__decltype(_GLIBCXX_PROFILE_DATA(__cost_factors)->begin()) __it
	 = _GLIBCXX_PROFILE_DATA(__cost_factors)->begin();
       __it != _GLIBCXX_PROFILE_DATA(__cost_factors)->end(); ++__it)
    {
      const char* __env_cost_factor = getenv((*__it)->__env_var);
      if (!__env_cost_factor)
        {
          __env_t::iterator __found = _GLIBCXX_PROFILE_DATA(__env).find(
              (*__it)->__env_var);
          if (__found != _GLIBCXX_PROFILE_DATA(__env).end())
            __env_cost_factor = (*__found).second.c_str();
        }
      if (__env_cost_factor)
        (*__it)->__value = atof(__env_cost_factor);
    }
}

inline void __profcxx_init_unconditional()
{
  _GLIBCXX_PROFILE_DATA(__global_lock).lock();

  if (__is_invalid()) {

    __set_max_warn_count();

    if (_GLIBCXX_PROFILE_DATA(_S_max_warn_count) == 0) {

      __turn_off();

    } else {

      __set_max_stack_trace_depth();
      __set_max_mem();
      __set_trace_path();
      __read_cost_factors(); 
      __set_cost_factors();
      __write_cost_factors();

      __trace_vector_size_init();
      __trace_hashtable_size_init();
      __trace_hash_func_init();
      __trace_vector_to_list_init();
      __trace_list_to_slist_init(); 
      __trace_list_to_vector_init();
      __trace_map_to_unordered_map_init();

      atexit(__report);

      __turn_on();

    }
  }

  _GLIBCXX_PROFILE_DATA(__global_lock).unlock();
}

/** @brief This function must be called by each instrumentation point.
 *
 * The common path is inlined fully.
 */
inline bool __profcxx_init(void)
{
  if (__is_invalid()) {
    __profcxx_init_unconditional();
  }

  return __is_on();
}

} // namespace __gnu_profile

#endif /* _GLIBCXX_PROFILE_PROFILER_TRACE_H */
