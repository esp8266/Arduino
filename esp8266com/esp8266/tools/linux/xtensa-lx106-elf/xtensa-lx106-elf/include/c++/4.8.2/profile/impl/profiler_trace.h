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

/** @file profile/impl/profiler_trace.h
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_TRACE_H
#define _GLIBCXX_PROFILE_PROFILER_TRACE_H 1

#include <cstdio>  // fopen, fclose, fprintf, FILE
#include <cerrno>
#include <cstdlib> // atof, atoi, strtol, getenv, atexit, abort

#if __cplusplus >= 201103L
#define _GLIBCXX_IMPL_UNORDERED_MAP std::_GLIBCXX_STD_C::unordered_map
#include <unordered_map>
#else
#include <tr1/unordered_map>
#define _GLIBCXX_IMPL_UNORDERED_MAP std::tr1::unordered_map
#endif

#include <ext/concurrence.h>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "profile/impl/profiler_algos.h"
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
    std::string __warning_message;

    __warning_data()
    : __magnitude(0.0), __context(0), __warning_id(0) { }

    __warning_data(float __m, __stack_t __c, const char* __id, 
		   const std::string& __msg)
    : __magnitude(__m), __context(__c), __warning_id(__id), 
      __warning_message(__msg) { }

    bool
    operator<(const __warning_data& __other) const
    { return __magnitude < __other.__magnitude; }
  };

  typedef std::_GLIBCXX_STD_C::vector<__warning_data> __warning_vector_t;

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

  typedef std::_GLIBCXX_STD_C::vector<__cost_factor*> __cost_factor_vector;

  _GLIBCXX_PROFILE_DEFINE_DATA(__trace_hash_func*, _S_hash_func, 0);
  _GLIBCXX_PROFILE_DEFINE_DATA(__trace_hashtable_size*, _S_hashtable_size, 0);
  _GLIBCXX_PROFILE_DEFINE_DATA(__trace_map2umap*, _S_map2umap, 0);
  _GLIBCXX_PROFILE_DEFINE_DATA(__trace_vector_size*, _S_vector_size, 0);
  _GLIBCXX_PROFILE_DEFINE_DATA(__trace_vector_to_list*, _S_vector_to_list, 0);
  _GLIBCXX_PROFILE_DEFINE_DATA(__trace_list_to_slist*, _S_list_to_slist, 0); 
  _GLIBCXX_PROFILE_DEFINE_DATA(__trace_list_to_vector*, _S_list_to_vector, 0);

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
  _GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor_vector*, __cost_factors, 0);

  _GLIBCXX_PROFILE_DEFINE_DATA(const char*, _S_trace_file_name,
			       _GLIBCXX_PROFILE_TRACE_PATH_ROOT);
  _GLIBCXX_PROFILE_DEFINE_DATA(std::size_t, _S_max_warn_count,
			       _GLIBCXX_PROFILE_MAX_WARN_COUNT);
  _GLIBCXX_PROFILE_DEFINE_DATA(std::size_t, _S_max_stack_depth,
			       _GLIBCXX_PROFILE_MAX_STACK_DEPTH);
  _GLIBCXX_PROFILE_DEFINE_DATA(std::size_t, _S_max_mem,
			       _GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC);

  inline std::size_t
  __stack_max_depth()
  { return _GLIBCXX_PROFILE_DATA(_S_max_stack_depth); }

  inline std::size_t
  __max_mem()
  { return _GLIBCXX_PROFILE_DATA(_S_max_mem); }

  /** @brief Base class for all trace producers.  */
  template<typename __object_info, typename __stack_info>
    class __trace_base
    {
    public:
      // Do not pick the initial size too large, as we don't know which
      // diagnostics are more active.
      __trace_base()
      : __object_table(10000), __stack_table(10000),
	__stack_table_byte_size(0), __id(0) { }

      virtual ~__trace_base() { }

      void __add_object(__object_t object, __object_info __info);
      __object_info* __get_object_info(__object_t __object);
      void __retire_object(__object_t __object);
      void __write(FILE* __f);
      void __collect_warnings(__warning_vector_t& __warnings);

    private:
      __gnu_cxx::__mutex __object_table_lock;
      __gnu_cxx::__mutex __stack_table_lock;
      typedef _GLIBCXX_IMPL_UNORDERED_MAP<__object_t, 
					  __object_info> __object_table_t;
      typedef _GLIBCXX_IMPL_UNORDERED_MAP<__stack_t, __stack_info,
					  __stack_hash, 
					  __stack_hash> __stack_table_t;
      __object_table_t __object_table;
      __stack_table_t __stack_table;
      std::size_t __stack_table_byte_size;

    protected:
      const char* __id;
    };

  template<typename __object_info, typename __stack_info>
    void
    __trace_base<__object_info, __stack_info>::
    __collect_warnings(__warning_vector_t& __warnings)
    {
      for (typename __stack_table_t::iterator __it
	     = __stack_table.begin(); __it != __stack_table.end(); ++__it)
	__warnings.push_back(__warning_data((*__it).second.__magnitude(),
					    (*__it).first, __id,
					    (*__it).second.__advice()));
    }

  template<typename __object_info, typename __stack_info>
    void
    __trace_base<__object_info, __stack_info>::
    __add_object(__object_t __object, __object_info __info)
    {
      if (__max_mem() == 0 
	  || __object_table.size() * sizeof(__object_info) <= __max_mem())
	{
	  this->__object_table_lock.lock();
	  __object_table.insert(typename __object_table_t::
				value_type(__object, __info));
	  this->__object_table_lock.unlock();
	}
    }

  template<typename __object_info, typename __stack_info>
    __object_info*
    __trace_base<__object_info, __stack_info>::
    __get_object_info(__object_t __object)
    {
      // XXX: Revisit this to see if we can decrease mutex spans.
      // Without this mutex, the object table could be rehashed during an
      // insertion on another thread, which could result in a segfault.
      this->__object_table_lock.lock();
      typename __object_table_t::iterator __object_it
	=  __object_table.find(__object);

      if (__object_it == __object_table.end())
	{
	  this->__object_table_lock.unlock();
	  return 0;
	}
      else
	{
	  this->__object_table_lock.unlock();
	  return &__object_it->second;
	}
    }

  template<typename __object_info, typename __stack_info>
    void
    __trace_base<__object_info, __stack_info>::
    __retire_object(__object_t __object)
    {
      this->__object_table_lock.lock();
      this->__stack_table_lock.lock();
      typename __object_table_t::iterator __object_it
	= __object_table.find(__object);
  
      if (__object_it != __object_table.end())
	{
	  const __object_info& __info = __object_it->second;
	  const __stack_t& __stack = __info.__stack();
	  typename __stack_table_t::iterator __stack_it
	    = __stack_table.find(__stack);
    
	  if (__stack_it == __stack_table.end())
	    {
	      // First occurence of this call context.
	      if (__max_mem() == 0 || __stack_table_byte_size < __max_mem()) 
		{
		  __stack_table_byte_size 
		    += (sizeof(__instruction_address_t) * __size(__stack)
			+ sizeof(__stack) + sizeof(__stack_info));
		  __stack_table.insert(make_pair(__stack,
						 __stack_info(__info)));
		}
	    }
	  else
	    {
	      // Merge object info into info summary for this call context.
	      __stack_it->second.__merge(__info);
	      delete __stack;
	    }
	  __object_table.erase(__object);
	}

      this->__object_table_lock.unlock();
      this->__stack_table_lock.unlock();
    }

  template<typename __object_info, typename __stack_info>
    void
    __trace_base<__object_info, __stack_info>::
    __write(FILE* __f)
    {
      for (typename __stack_table_t::iterator __it
	     = __stack_table.begin(); __it != __stack_table.end(); ++__it)
	if (__it->second.__is_valid())
	  {
	    std::fprintf(__f, __id);
	    std::fprintf(__f, "|");
	    __gnu_profile::__write(__f, __it->first);
	    std::fprintf(__f, "|");
	    __it->second.__write(__f);
	  }
    }

  inline std::size_t
  __env_to_size_t(const char* __env_var, std::size_t __default_value)
  {
    char* __env_value = std::getenv(__env_var);
    if (__env_value)
      {
	errno = 0;
	long __converted_value = std::strtol(__env_value, 0, 10);
	if (errno || __converted_value < 0)
	  {
	    std::fprintf(stderr,
			 "Bad value for environment variable '%s'.\n",
			 __env_var);
	    std::abort();
	  }
	else
	  return static_cast<std::size_t>(__converted_value);
      }
    else
      return __default_value;
  }

  inline void
  __set_max_stack_trace_depth()
  {
    _GLIBCXX_PROFILE_DATA(_S_max_stack_depth)
      = __env_to_size_t(_GLIBCXX_PROFILE_MAX_STACK_DEPTH_ENV_VAR,
			_GLIBCXX_PROFILE_DATA(_S_max_stack_depth));
  }

  inline void
  __set_max_mem()
  {
    _GLIBCXX_PROFILE_DATA(_S_max_mem) 
      = __env_to_size_t(_GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC_ENV_VAR,
			_GLIBCXX_PROFILE_DATA(_S_max_mem));
  }

  inline int
  __log_magnitude(float __f)
  {
    const float __log_base = 10.0;
    int __result = 0;
    int __sign = 1;

    if (__f < 0) 
      {
	__f = -__f;
	__sign = -1;
      }

    while (__f > __log_base) 
      {
	++__result;
	__f /= 10.0;
      }
    return __sign * __result;
  }

  inline FILE* 
  __open_output_file(const char* __extension)
  {
    // The path is made of _S_trace_file_name + "." + extension.
    std::size_t __root_len 
      = __builtin_strlen(_GLIBCXX_PROFILE_DATA(_S_trace_file_name));
    std::size_t __ext_len = __builtin_strlen(__extension);
    char* __file_name = new char[__root_len + 1 + __ext_len + 1];
    __builtin_memcpy(__file_name,
		     _GLIBCXX_PROFILE_DATA(_S_trace_file_name),
		     __root_len);
    *(__file_name + __root_len) = '.';
    __builtin_memcpy(__file_name + __root_len + 1,
		     __extension, __ext_len + 1);

    FILE* __out_file = std::fopen(__file_name, "w");
    if (!__out_file)
      {
	std::fprintf(stderr, "Could not open trace file '%s'.\n",
		     __file_name);
	std::abort();
      }

    delete[] __file_name;
    return __out_file;
  }

  struct __warn
  {
    FILE* __file;

    __warn(FILE* __f)
    { __file = __f; }

    void
    operator()(const __warning_data& __info)
    {
      std::fprintf(__file,  __info.__warning_id);
      std::fprintf(__file, ": improvement = %d",
		   __log_magnitude(__info.__magnitude));
      std::fprintf(__file, ": call stack = ");
      __gnu_profile::__write(__file, __info.__context);
      std::fprintf(__file, ": advice = %s\n",
		   __info.__warning_message.c_str());
    }
  };

  /** @brief Final report method, registered with @b atexit.
   *
   * This can also be called directly by user code, including signal handlers.
   * It is protected against deadlocks by the reentrance guard in profiler.h.
   * However, when called from a signal handler that triggers while within
   * __gnu_profile (under the guarded zone), no output will be produced.
   */
  inline void
  __report(void)
  {
    _GLIBCXX_PROFILE_DATA(__global_lock).lock();

    __warning_vector_t __warnings, __top_warnings;

    FILE* __raw_file = __open_output_file("raw");
    __trace_vector_size_report(__raw_file, __warnings);
    __trace_hashtable_size_report(__raw_file, __warnings);
    __trace_hash_func_report(__raw_file, __warnings);
    __trace_vector_to_list_report(__raw_file, __warnings);
    __trace_list_to_slist_report(__raw_file, __warnings);
    __trace_list_to_vector_report(__raw_file, __warnings);
    __trace_map_to_unordered_map_report(__raw_file, __warnings);
    std::fclose(__raw_file);

    // Sort data by magnitude, keeping just top N.
    std::size_t __cutoff = std::min(_GLIBCXX_PROFILE_DATA(_S_max_warn_count),
				    __warnings.size());
    __top_n(__warnings, __top_warnings, __cutoff);

    FILE* __warn_file = __open_output_file("txt");
    __for_each(__top_warnings.begin(), __top_warnings.end(),
	       __warn(__warn_file));
    std::fclose(__warn_file);

    _GLIBCXX_PROFILE_DATA(__global_lock).unlock();
  }

  inline void
  __set_trace_path()
  {
    char* __env_trace_file_name = std::getenv(_GLIBCXX_PROFILE_TRACE_ENV_VAR);

    if (__env_trace_file_name)
      _GLIBCXX_PROFILE_DATA(_S_trace_file_name) = __env_trace_file_name;

    // Make sure early that we can create the trace file.
    std::fclose(__open_output_file("txt"));
  }

  inline void
  __set_max_warn_count()
  {
    char* __env_max_warn_count_str
      = std::getenv(_GLIBCXX_PROFILE_MAX_WARN_COUNT_ENV_VAR);

    if (__env_max_warn_count_str)
      _GLIBCXX_PROFILE_DATA(_S_max_warn_count)
	= static_cast<std::size_t>(std::atoi(__env_max_warn_count_str));
  }

  inline void
  __read_cost_factors()
  {
    std::string __conf_file_name(_GLIBCXX_PROFILE_DATA(_S_trace_file_name));
    __conf_file_name += ".conf";

    std::ifstream __conf_file(__conf_file_name.c_str());

    if (__conf_file.is_open())
      {
	std::string __line;

	while (std::getline(__conf_file, __line))
	  {
	    std::string::size_type __i = __line.find_first_not_of(" \t\n\v");

	    if (__line.length() <= 0 || __line[__i] == '#')
	      // Skip empty lines or comments.
	      continue;
	  }

	// Trim.
	__line.erase(__remove(__line.begin(), __line.end(), ' '),
		     __line.end());
	std::string::size_type __pos = __line.find("=");
	std::string __factor_name = __line.substr(0, __pos);
	std::string::size_type __end = __line.find_first_of(";\n");
	std::string __factor_value = __line.substr(__pos + 1, __end - __pos);

	_GLIBCXX_PROFILE_DATA(__env)[__factor_name] = __factor_value;
      }
  }

  struct __cost_factor_writer
  {
    FILE* __file;

    __cost_factor_writer(FILE* __f)
    : __file(__f) { }

    void
    operator() (const __cost_factor* __factor)
    { std::fprintf(__file, "%s = %f\n", __factor->__env_var,
		   __factor->__value); }
  };

  inline void
  __write_cost_factors()
  {
    FILE* __file = __open_output_file("conf.out");
    __for_each(_GLIBCXX_PROFILE_DATA(__cost_factors)->begin(),
	       _GLIBCXX_PROFILE_DATA(__cost_factors)->end(),
	       __cost_factor_writer(__file));
    std::fclose(__file);
  }

  struct __cost_factor_setter
  {
    void
    operator()(__cost_factor* __factor)
    {
      // Look it up in the process environment first.
      const char* __env_value = std::getenv(__factor->__env_var);

      if (!__env_value)
        {
          // Look it up in the config file.
          __env_t::iterator __it 
	    = _GLIBCXX_PROFILE_DATA(__env).find(__factor->__env_var);
          if (__it != _GLIBCXX_PROFILE_DATA(__env).end())
            __env_value = (*__it).second.c_str();
        }

      if (__env_value)
        __factor->__value = std::atof(__env_value);
    }
  };

  inline void
  __set_cost_factors()
  {
    _GLIBCXX_PROFILE_DATA(__cost_factors) = new __cost_factor_vector;
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__vector_shift_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__vector_iterate_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__vector_resize_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__list_shift_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__list_iterate_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__list_resize_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__map_insert_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__map_erase_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__map_find_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__map_iterate_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__umap_insert_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__umap_erase_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__umap_find_cost_factor));
    _GLIBCXX_PROFILE_DATA(__cost_factors)->
      push_back(&_GLIBCXX_PROFILE_DATA(__umap_iterate_cost_factor));
    __for_each(_GLIBCXX_PROFILE_DATA(__cost_factors)->begin(),
	       _GLIBCXX_PROFILE_DATA(__cost_factors)->end(),
	       __cost_factor_setter());
  }

  inline void
  __profcxx_init_unconditional()
  {
    _GLIBCXX_PROFILE_DATA(__global_lock).lock();

    if (__is_invalid())
      {
	__set_max_warn_count();

	if (_GLIBCXX_PROFILE_DATA(_S_max_warn_count) == 0)
	  __turn_off();
	else
	  {
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

	    std::atexit(__report);

	    __turn_on();
	  }
      }

    _GLIBCXX_PROFILE_DATA(__global_lock).unlock();
  }

  /** @brief This function must be called by each instrumentation point.
   *
   * The common path is inlined fully.
   */
  inline bool
  __profcxx_init()
  {
    if (__is_invalid())
      __profcxx_init_unconditional();

    return __is_on();
  }

} // namespace __gnu_profile

#endif /* _GLIBCXX_PROFILE_PROFILER_TRACE_H */
