// <thread> -*- C++ -*-

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

/** @file include/thread
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_THREAD
#define _GLIBCXX_THREAD 1

#pragma GCC system_header

#if __cplusplus < 201103L
# include <bits/c++0x_warning.h>
#else

#include <chrono>
#include <functional>
#include <memory>
#include <bits/functexcept.h>
#include <bits/functional_hash.h>
#include <bits/gthr.h>

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @defgroup threads Threads
   * @ingroup concurrency
   *
   * Classes for thread support.
   * @{
   */

  /// thread
  class thread
  {
  public:
    typedef __gthread_t			native_handle_type;
    struct _Impl_base;
    typedef shared_ptr<_Impl_base>	__shared_base_type;

    /// thread::id
    class id
    {
      native_handle_type	_M_thread;

    public:
      id() noexcept : _M_thread() { }

      explicit
      id(native_handle_type __id) : _M_thread(__id) { }

    private:
      friend class thread;
      friend class hash<thread::id>;

      friend bool
      operator==(thread::id __x, thread::id __y) noexcept
      { return __gthread_equal(__x._M_thread, __y._M_thread); }

      friend bool
      operator<(thread::id __x, thread::id __y) noexcept
      { return __x._M_thread < __y._M_thread; }

      template<class _CharT, class _Traits>
	friend basic_ostream<_CharT, _Traits>&
	operator<<(basic_ostream<_CharT, _Traits>& __out, thread::id __id);
    };

    // Simple base type that the templatized, derived class containing
    // an arbitrary functor can be converted to and called.
    struct _Impl_base
    {
      __shared_base_type	_M_this_ptr;

      inline virtual ~_Impl_base();

      virtual void _M_run() = 0;
    };

    template<typename _Callable>
      struct _Impl : public _Impl_base
      {
	_Callable		_M_func;

	_Impl(_Callable&& __f) : _M_func(std::forward<_Callable>(__f))
	{ }

	void
	_M_run() { _M_func(); }
      };

  private:
    id				_M_id;

  public:
    thread() noexcept = default;
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 2097.  packaged_task constructors should be constrained
    thread(thread&) = delete;
    thread(const thread&) = delete;

    thread(thread&& __t) noexcept
    { swap(__t); }

    template<typename _Callable, typename... _Args>
      explicit 
      thread(_Callable&& __f, _Args&&... __args)
      {
        _M_start_thread(_M_make_routine(std::__bind_simple(
                std::forward<_Callable>(__f),
                std::forward<_Args>(__args)...)));
      }

    ~thread()
    {
      if (joinable())
	std::terminate();
    }

    thread& operator=(const thread&) = delete;

    thread& operator=(thread&& __t) noexcept
    {
      if (joinable())
	std::terminate();
      swap(__t);
      return *this;
    }

    void
    swap(thread& __t) noexcept
    { std::swap(_M_id, __t._M_id); }

    bool
    joinable() const noexcept
    { return !(_M_id == id()); }

    void
    join();

    void
    detach();

    thread::id
    get_id() const noexcept
    { return _M_id; }

    /** @pre thread is joinable
     */
    native_handle_type
    native_handle()
    { return _M_id._M_thread; }

    // Returns a value that hints at the number of hardware thread contexts.
    static unsigned int
    hardware_concurrency() noexcept;

  private:
    void
    _M_start_thread(__shared_base_type);

    template<typename _Callable>
      shared_ptr<_Impl<_Callable>>
      _M_make_routine(_Callable&& __f)
      {
	// Create and allocate full data structure, not base.
	return std::make_shared<_Impl<_Callable>>(std::forward<_Callable>(__f));
      }
  };

  inline thread::_Impl_base::~_Impl_base() = default;

  inline void
  swap(thread& __x, thread& __y) noexcept
  { __x.swap(__y); }

  inline bool
  operator!=(thread::id __x, thread::id __y) noexcept
  { return !(__x == __y); }

  inline bool
  operator<=(thread::id __x, thread::id __y) noexcept
  { return !(__y < __x); }

  inline bool
  operator>(thread::id __x, thread::id __y) noexcept
  { return __y < __x; }

  inline bool
  operator>=(thread::id __x, thread::id __y) noexcept
  { return !(__x < __y); }

  // DR 889.
  /// std::hash specialization for thread::id.
  template<>
    struct hash<thread::id>
    : public __hash_base<size_t, thread::id>
    {
      size_t
      operator()(const thread::id& __id) const noexcept
      { return std::_Hash_impl::hash(__id._M_thread); }
    };

  template<class _CharT, class _Traits>
    inline basic_ostream<_CharT, _Traits>&
    operator<<(basic_ostream<_CharT, _Traits>& __out, thread::id __id)
    {
      if (__id == thread::id())
	return __out << "thread::id of a non-executing thread";
      else
	return __out << __id._M_thread;
    }

_GLIBCXX_END_NAMESPACE_VERSION

  /** @namespace std::this_thread
   *  @brief ISO C++ 2011 entities sub-namespace for thread.
   *  30.3.2 Namespace this_thread.
   */
  namespace this_thread
  {
  _GLIBCXX_BEGIN_NAMESPACE_VERSION

    /// get_id
    inline thread::id
    get_id() noexcept { return thread::id(__gthread_self()); }

    /// yield
    inline void
    yield() noexcept
    {
#ifdef _GLIBCXX_USE_SCHED_YIELD
      __gthread_yield();
#endif
    }

    void
    __sleep_for(chrono::seconds, chrono::nanoseconds);

    /// sleep_for
    template<typename _Rep, typename _Period>
      inline void
      sleep_for(const chrono::duration<_Rep, _Period>& __rtime)
      {
	auto __s = chrono::duration_cast<chrono::seconds>(__rtime);
	auto __ns = chrono::duration_cast<chrono::nanoseconds>(__rtime - __s);
#ifdef _GLIBCXX_USE_NANOSLEEP
	__gthread_time_t __ts =
	  {
	    static_cast<std::time_t>(__s.count()),
	    static_cast<long>(__ns.count())
	  };
	::nanosleep(&__ts, 0);
#else
	__sleep_for(__s, __ns);
#endif
      }

    /// sleep_until
    template<typename _Clock, typename _Duration>
      inline void
      sleep_until(const chrono::time_point<_Clock, _Duration>& __atime)
      { sleep_for(__atime - _Clock::now()); }

  _GLIBCXX_END_NAMESPACE_VERSION
  }

  // @} group threads

} // namespace

#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

#endif // C++11

#endif // _GLIBCXX_THREAD
