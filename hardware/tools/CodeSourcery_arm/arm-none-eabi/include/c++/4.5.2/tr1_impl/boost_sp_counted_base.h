// <tr1_impl/boost_sp_counted_base.h> -*- C++ -*-

// Copyright (C) 2007, 2009 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

//  shared_count.hpp
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.

//  shared_ptr.hpp
//  Copyright (C) 1998, 1999 Greg Colvin and Beman Dawes.
//  Copyright (C) 2001, 2002, 2003 Peter Dimov

//  weak_ptr.hpp
//  Copyright (C) 2001, 2002, 2003 Peter Dimov

//  enable_shared_from_this.hpp
//  Copyright (C) 2002 Peter Dimov

// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// GCC Note:  based on version 1.32.0 of the Boost library.

/** @file tr1_impl/boost_sp_counted_base.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */


namespace std
{
_GLIBCXX_BEGIN_NAMESPACE_TR1

 /**
   *  @brief  Exception possibly thrown by @c shared_ptr.
   *  @ingroup exceptions
   */
  class bad_weak_ptr : public std::exception
  {
  public:
    virtual char const*
    what() const throw()
#ifdef _GLIBCXX_INCLUDE_AS_CXX0X
    { return "std::bad_weak_ptr"; }
#else
    { return "tr1::bad_weak_ptr"; }
#endif
  };

  // Substitute for bad_weak_ptr object in the case of -fno-exceptions.
  inline void
  __throw_bad_weak_ptr()
  {
#if __EXCEPTIONS
    throw bad_weak_ptr();
#else
    __builtin_abort();
#endif
  }

  using __gnu_cxx::_Lock_policy;
  using __gnu_cxx::__default_lock_policy;
  using __gnu_cxx::_S_single;
  using __gnu_cxx::_S_mutex;
  using __gnu_cxx::_S_atomic;

  // Empty helper class except when the template argument is _S_mutex.
  template<_Lock_policy _Lp>
    class _Mutex_base
    {
    protected:
      // The atomic policy uses fully-fenced builtins, single doesn't care.
      enum { _S_need_barriers = 0 };
    };

  template<>
    class _Mutex_base<_S_mutex>
    : public __gnu_cxx::__mutex
    {
    protected:
      // This policy is used when atomic builtins are not available.
      // The replacement atomic operations might not have the necessary
      // memory barriers.
      enum { _S_need_barriers = 1 };
    };

  template<_Lock_policy _Lp = __default_lock_policy>
    class _Sp_counted_base
    : public _Mutex_base<_Lp>
    {
    public:  
      _Sp_counted_base()
      : _M_use_count(1), _M_weak_count(1) { }
      
      virtual
      ~_Sp_counted_base() // nothrow 
      { }
  
      // Called when _M_use_count drops to zero, to release the resources
      // managed by *this.
      virtual void
      _M_dispose() = 0; // nothrow
      
      // Called when _M_weak_count drops to zero.
      virtual void
      _M_destroy() // nothrow
      { delete this; }
      
      virtual void*
      _M_get_deleter(const std::type_info&) = 0;

      void
      _M_add_ref_copy()
      { __gnu_cxx::__atomic_add_dispatch(&_M_use_count, 1); }
  
      void
      _M_add_ref_lock();
      
      void
      _M_release() // nothrow
      {
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, -1) == 1)
	  {
	    _M_dispose();
	    // There must be a memory barrier between dispose() and destroy()
	    // to ensure that the effects of dispose() are observed in the
	    // thread that runs destroy().
	    // See http://gcc.gnu.org/ml/libstdc++/2005-11/msg00136.html
	    if (_Mutex_base<_Lp>::_S_need_barriers)
	      {
	        _GLIBCXX_READ_MEM_BARRIER;
	        _GLIBCXX_WRITE_MEM_BARRIER;
	      }

	    if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count,
						       -1) == 1)
	      _M_destroy();
	  }
      }
  
      void
      _M_weak_add_ref() // nothrow
      { __gnu_cxx::__atomic_add_dispatch(&_M_weak_count, 1); }

      void
      _M_weak_release() // nothrow
      {
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
	  {
	    if (_Mutex_base<_Lp>::_S_need_barriers)
	      {
	        // See _M_release(),
	        // destroy() must observe results of dispose()
	        _GLIBCXX_READ_MEM_BARRIER;
	        _GLIBCXX_WRITE_MEM_BARRIER;
	      }
	    _M_destroy();
	  }
      }
  
      long
      _M_get_use_count() const // nothrow
      {
        // No memory barrier is used here so there is no synchronization
        // with other threads.
        return const_cast<const volatile _Atomic_word&>(_M_use_count);
      }

    private:  
      _Sp_counted_base(_Sp_counted_base const&);
      _Sp_counted_base& operator=(_Sp_counted_base const&);

      _Atomic_word  _M_use_count;     // #shared
      _Atomic_word  _M_weak_count;    // #weak + (#shared != 0)
    };

  template<>
    inline void
    _Sp_counted_base<_S_single>::
    _M_add_ref_lock()
    {
      if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, 1) == 0)
	{
	  _M_use_count = 0;
	  __throw_bad_weak_ptr();
	}
    }

  template<>
    inline void
    _Sp_counted_base<_S_mutex>::
    _M_add_ref_lock()
    {
      __gnu_cxx::__scoped_lock sentry(*this);
      if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, 1) == 0)
	{
	  _M_use_count = 0;
	  __throw_bad_weak_ptr();
	}
    }

  template<> 
    inline void
    _Sp_counted_base<_S_atomic>::
    _M_add_ref_lock()
    {
      // Perform lock-free add-if-not-zero operation.
      _Atomic_word __count;
      do
	{
	  __count = _M_use_count;
	  if (__count == 0)
	    __throw_bad_weak_ptr();
	  
	  // Replace the current counter value with the old value + 1, as
	  // long as it's not changed meanwhile. 
	}
      while (!__sync_bool_compare_and_swap(&_M_use_count, __count,
					   __count + 1));
    }

_GLIBCXX_END_NAMESPACE_TR1
}
