// <condition_variable> -*- C++ -*-

// Copyright (C) 2008-2013 Free Software Foundation, Inc.
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

/** @file include/condition_variable
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_CONDITION_VARIABLE
#define _GLIBCXX_CONDITION_VARIABLE 1

#pragma GCC system_header

#if __cplusplus < 201103L
# include <bits/c++0x_warning.h>
#else

#include <chrono>
#include <mutex> // unique_lock

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @defgroup condition_variables Condition Variables
   * @ingroup concurrency
   *
   * Classes for condition_variable support.
   * @{
   */

  /// cv_status
  enum class cv_status { no_timeout, timeout };
  
  /// condition_variable
  class condition_variable
  {
    typedef chrono::system_clock	__clock_t;
    typedef __gthread_cond_t		__native_type;

#ifdef __GTHREAD_COND_INIT
    __native_type			_M_cond = __GTHREAD_COND_INIT;
#else
    __native_type			_M_cond;
#endif

  public:
    typedef __native_type* 		native_handle_type;

    condition_variable() noexcept;
    ~condition_variable() noexcept;

    condition_variable(const condition_variable&) = delete;
    condition_variable& operator=(const condition_variable&) = delete;

    void
    notify_one() noexcept;

    void
    notify_all() noexcept;

    void
    wait(unique_lock<mutex>& __lock);

    template<typename _Predicate>
      void
      wait(unique_lock<mutex>& __lock, _Predicate __p)
      {
	while (!__p())
	  wait(__lock);
      }

    template<typename _Duration>
      cv_status
      wait_until(unique_lock<mutex>& __lock,
		 const chrono::time_point<__clock_t, _Duration>& __atime)
      { return __wait_until_impl(__lock, __atime); }

    template<typename _Clock, typename _Duration>
      cv_status
      wait_until(unique_lock<mutex>& __lock,
		 const chrono::time_point<_Clock, _Duration>& __atime)
      {
	// DR 887 - Sync unknown clock to known clock.
	const typename _Clock::time_point __c_entry = _Clock::now();
	const __clock_t::time_point __s_entry = __clock_t::now();
	const auto __delta = __atime - __c_entry;
	const auto __s_atime = __s_entry + __delta;

	return __wait_until_impl(__lock, __s_atime);
      }

    template<typename _Clock, typename _Duration, typename _Predicate>
      bool
      wait_until(unique_lock<mutex>& __lock,
		 const chrono::time_point<_Clock, _Duration>& __atime,
		 _Predicate __p)
      {
	while (!__p())
	  if (wait_until(__lock, __atime) == cv_status::timeout)
	    return __p();
	return true;
      }

    template<typename _Rep, typename _Period>
      cv_status
      wait_for(unique_lock<mutex>& __lock,
	       const chrono::duration<_Rep, _Period>& __rtime)
      { return wait_until(__lock, __clock_t::now() + __rtime); }

    template<typename _Rep, typename _Period, typename _Predicate>
      bool
      wait_for(unique_lock<mutex>& __lock,
	       const chrono::duration<_Rep, _Period>& __rtime,
	       _Predicate __p)
      { return wait_until(__lock, __clock_t::now() + __rtime, std::move(__p)); }

    native_handle_type
    native_handle()
    { return &_M_cond; }

  private:
    template<typename _Dur>
      cv_status
      __wait_until_impl(unique_lock<mutex>& __lock,
			const chrono::time_point<__clock_t, _Dur>& __atime)
      {
	auto __s = chrono::time_point_cast<chrono::seconds>(__atime);
	auto __ns = chrono::duration_cast<chrono::nanoseconds>(__atime - __s);

	__gthread_time_t __ts =
	  {
	    static_cast<std::time_t>(__s.time_since_epoch().count()),
	    static_cast<long>(__ns.count())
	  };

	__gthread_cond_timedwait(&_M_cond, __lock.mutex()->native_handle(),
				 &__ts);

	return (__clock_t::now() < __atime
		? cv_status::no_timeout : cv_status::timeout);
      }
  };

  /// condition_variable_any
  // Like above, but mutex is not required to have try_lock.
  class condition_variable_any
  {
    typedef chrono::system_clock	__clock_t;
    condition_variable			_M_cond;
    mutex				_M_mutex;

    // scoped unlock - unlocks in ctor, re-locks in dtor
    template<typename _Lock>
      struct _Unlock
      {
	explicit _Unlock(_Lock& __lk) : _M_lock(__lk) { __lk.unlock(); }

	~_Unlock() noexcept(false)
	{
	  if (uncaught_exception())
	    __try { _M_lock.lock(); } __catch(...) { }
	  else
	    _M_lock.lock();
	}

	_Unlock(const _Unlock&) = delete;
	_Unlock& operator=(const _Unlock&) = delete;

	_Lock& _M_lock;
      };

  public:

    condition_variable_any() noexcept;
    ~condition_variable_any() noexcept;

    condition_variable_any(const condition_variable_any&) = delete;
    condition_variable_any& operator=(const condition_variable_any&) = delete;

    void
    notify_one() noexcept
    {
      lock_guard<mutex> __lock(_M_mutex);
      _M_cond.notify_one();
    }

    void
    notify_all() noexcept
    {
      lock_guard<mutex> __lock(_M_mutex);
      _M_cond.notify_all();
    }

    template<typename _Lock>
      void
      wait(_Lock& __lock)
      {
	unique_lock<mutex> __my_lock(_M_mutex);
	_Unlock<_Lock> __unlock(__lock);
	// _M_mutex must be unlocked before re-locking __lock so move
	// ownership of _M_mutex lock to an object with shorter lifetime.
	unique_lock<mutex> __my_lock2(std::move(__my_lock));
	_M_cond.wait(__my_lock2);
      }
      

    template<typename _Lock, typename _Predicate>
      void
      wait(_Lock& __lock, _Predicate __p)
      {
	while (!__p())
	  wait(__lock);
      }

    template<typename _Lock, typename _Clock, typename _Duration>
      cv_status
      wait_until(_Lock& __lock,
		 const chrono::time_point<_Clock, _Duration>& __atime)
      {
	unique_lock<mutex> __my_lock(_M_mutex);
	_Unlock<_Lock> __unlock(__lock);
	// _M_mutex must be unlocked before re-locking __lock so move
	// ownership of _M_mutex lock to an object with shorter lifetime.
	unique_lock<mutex> __my_lock2(std::move(__my_lock));
	return _M_cond.wait_until(__my_lock2, __atime);
      }

    template<typename _Lock, typename _Clock,
	     typename _Duration, typename _Predicate>
      bool
      wait_until(_Lock& __lock,
		 const chrono::time_point<_Clock, _Duration>& __atime,
		 _Predicate __p)
      {
	while (!__p())
	  if (wait_until(__lock, __atime) == cv_status::timeout)
	    return __p();
	return true;
      }

    template<typename _Lock, typename _Rep, typename _Period>
      cv_status
      wait_for(_Lock& __lock, const chrono::duration<_Rep, _Period>& __rtime)
      { return wait_until(__lock, __clock_t::now() + __rtime); }

    template<typename _Lock, typename _Rep,
	     typename _Period, typename _Predicate>
      bool
      wait_for(_Lock& __lock,
	       const chrono::duration<_Rep, _Period>& __rtime, _Predicate __p)
      { return wait_until(__lock, __clock_t::now() + __rtime, std::move(__p)); }
  };

  // @} group condition_variables
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

#endif // C++11

#endif // _GLIBCXX_CONDITION_VARIABLE
