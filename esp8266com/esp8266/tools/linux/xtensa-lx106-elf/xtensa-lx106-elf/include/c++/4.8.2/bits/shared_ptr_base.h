// shared_ptr and weak_ptr implementation details -*- C++ -*-

// Copyright (C) 2007-2013 Free Software Foundation, Inc.
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

// GCC Note: Based on files from version 1.32.0 of the Boost library.

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

/** @file bits/shared_ptr_base.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{memory}
 */

#ifndef _SHARED_PTR_BASE_H
#define _SHARED_PTR_BASE_H 1

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#if _GLIBCXX_USE_DEPRECATED
  template<typename> class auto_ptr;
#endif

 /**
   *  @brief  Exception possibly thrown by @c shared_ptr.
   *  @ingroup exceptions
   */
  class bad_weak_ptr : public std::exception
  {
  public:
    virtual char const*
    what() const noexcept;

    virtual ~bad_weak_ptr() noexcept;    
  };

  // Substitute for bad_weak_ptr object in the case of -fno-exceptions.
  inline void
  __throw_bad_weak_ptr()
  { _GLIBCXX_THROW_OR_ABORT(bad_weak_ptr()); }

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
      _Sp_counted_base() noexcept
      : _M_use_count(1), _M_weak_count(1) { }
      
      virtual
      ~_Sp_counted_base() noexcept
      { }
  
      // Called when _M_use_count drops to zero, to release the resources
      // managed by *this.
      virtual void
      _M_dispose() noexcept = 0;
      
      // Called when _M_weak_count drops to zero.
      virtual void
      _M_destroy() noexcept
      { delete this; }
      
      virtual void*
      _M_get_deleter(const std::type_info&) = 0;

      void
      _M_add_ref_copy()
      { __gnu_cxx::__atomic_add_dispatch(&_M_use_count, 1); }
  
      void
      _M_add_ref_lock();
      
      void
      _M_release() noexcept
      {
        // Be race-detector-friendly.  For more info see bits/c++config.
        _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(&_M_use_count);
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, -1) == 1)
	  {
            _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&_M_use_count);
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

            // Be race-detector-friendly.  For more info see bits/c++config.
            _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(&_M_weak_count);
	    if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count,
						       -1) == 1)
              {
                _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&_M_weak_count);
	        _M_destroy();
              }
	  }
      }
  
      void
      _M_weak_add_ref() noexcept
      { __gnu_cxx::__atomic_add_dispatch(&_M_weak_count, 1); }

      void
      _M_weak_release() noexcept
      {
        // Be race-detector-friendly. For more info see bits/c++config.
        _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(&_M_weak_count);
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
	  {
            _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&_M_weak_count);
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
      _M_get_use_count() const noexcept
      {
        // No memory barrier is used here so there is no synchronization
        // with other threads.
        return __atomic_load_n(&_M_use_count, __ATOMIC_RELAXED);
      }

    private:  
      _Sp_counted_base(_Sp_counted_base const&) = delete;
      _Sp_counted_base& operator=(_Sp_counted_base const&) = delete;

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
      _Atomic_word __count = _M_use_count;
      do
	{
	  if (__count == 0)
	    __throw_bad_weak_ptr();
	  // Replace the current counter value with the old value + 1, as
	  // long as it's not changed meanwhile. 
	}
      while (!__atomic_compare_exchange_n(&_M_use_count, &__count, __count + 1,
					  true, __ATOMIC_ACQ_REL, 
					  __ATOMIC_RELAXED));
    }


  // Forward declarations.
  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __shared_ptr;

  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __weak_ptr;

  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __enable_shared_from_this;

  template<typename _Tp>
    class shared_ptr;

  template<typename _Tp>
    class weak_ptr;

  template<typename _Tp>
    struct owner_less;

  template<typename _Tp>
    class enable_shared_from_this;

  template<_Lock_policy _Lp = __default_lock_policy>
    class __weak_count;

  template<_Lock_policy _Lp = __default_lock_policy>
    class __shared_count;


  // Counted ptr with no deleter or allocator support
  template<typename _Ptr, _Lock_policy _Lp>
    class _Sp_counted_ptr final : public _Sp_counted_base<_Lp>
    {
    public:
      explicit
      _Sp_counted_ptr(_Ptr __p)
      : _M_ptr(__p) { }

      virtual void
      _M_dispose() noexcept
      { delete _M_ptr; }

      virtual void
      _M_destroy() noexcept
      { delete this; }

      virtual void*
      _M_get_deleter(const std::type_info&)
      { return 0; }

      _Sp_counted_ptr(const _Sp_counted_ptr&) = delete;
      _Sp_counted_ptr& operator=(const _Sp_counted_ptr&) = delete;

    protected:
      _Ptr             _M_ptr;  // copy constructor must not throw
    };

  template<>
    inline void
    _Sp_counted_ptr<nullptr_t, _S_single>::_M_dispose() noexcept { }

  template<>
    inline void
    _Sp_counted_ptr<nullptr_t, _S_mutex>::_M_dispose() noexcept { }

  template<>
    inline void
    _Sp_counted_ptr<nullptr_t, _S_atomic>::_M_dispose() noexcept { }

  // Support for custom deleter and/or allocator
  template<typename _Ptr, typename _Deleter, typename _Alloc, _Lock_policy _Lp>
    class _Sp_counted_deleter final : public _Sp_counted_base<_Lp>
    {
      // Helper class that stores the Deleter and also acts as an allocator.
      // Used to dispose of the owned pointer and the internal refcount
      // Requires that copies of _Alloc can free each other's memory.
      struct _My_Deleter
      : public _Alloc           // copy constructor must not throw
      {
	_Deleter _M_del;        // copy constructor must not throw
	_My_Deleter(_Deleter __d, const _Alloc& __a)
	: _Alloc(__a), _M_del(__d) { }
      };

    public:
      // __d(__p) must not throw.
      _Sp_counted_deleter(_Ptr __p, _Deleter __d)
      : _M_ptr(__p), _M_del(__d, _Alloc()) { }

      // __d(__p) must not throw.
      _Sp_counted_deleter(_Ptr __p, _Deleter __d, const _Alloc& __a)
      : _M_ptr(__p), _M_del(__d, __a) { }

      ~_Sp_counted_deleter() noexcept { }

      virtual void
      _M_dispose() noexcept
      { _M_del._M_del(_M_ptr); }

      virtual void
      _M_destroy() noexcept
      {
	typedef typename allocator_traits<_Alloc>::template
	  rebind_traits<_Sp_counted_deleter> _Alloc_traits;
	typename _Alloc_traits::allocator_type __a(_M_del);
	_Alloc_traits::destroy(__a, this);
	_Alloc_traits::deallocate(__a, this, 1);
      }

      virtual void*
      _M_get_deleter(const std::type_info& __ti)
      {
#ifdef __GXX_RTTI
        return __ti == typeid(_Deleter) ? &_M_del._M_del : 0;
#else
        return 0;
#endif
      }

    protected:
      _Ptr             _M_ptr;  // copy constructor must not throw
      _My_Deleter      _M_del;  // copy constructor must not throw
    };

  // helpers for make_shared / allocate_shared

  struct _Sp_make_shared_tag { };

  template<typename _Tp, typename _Alloc, _Lock_policy _Lp>
    class _Sp_counted_ptr_inplace final : public _Sp_counted_base<_Lp>
    {
      // Helper class that stores the pointer and also acts as an allocator.
      // Used to dispose of the owned pointer and the internal refcount
      // Requires that copies of _Alloc can free each other's memory.
      struct _Impl
      : public _Alloc           // copy constructor must not throw
      {
	_Impl(_Alloc __a) : _Alloc(__a), _M_ptr() { }
	_Tp* _M_ptr;
      };

    public:
      template<typename... _Args>
	_Sp_counted_ptr_inplace(_Alloc __a, _Args&&... __args)
	: _M_impl(__a), _M_storage()
	{
	  _M_impl._M_ptr = static_cast<_Tp*>(static_cast<void*>(&_M_storage));
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 2070.  allocate_shared should use allocator_traits<A>::construct
	  allocator_traits<_Alloc>::construct(__a, _M_impl._M_ptr,
	      std::forward<_Args>(__args)...); // might throw
	}

      ~_Sp_counted_ptr_inplace() noexcept { }

      virtual void
      _M_dispose() noexcept
      { allocator_traits<_Alloc>::destroy(_M_impl, _M_impl._M_ptr); }

      // Override because the allocator needs to know the dynamic type
      virtual void
      _M_destroy() noexcept
      {
	typedef typename allocator_traits<_Alloc>::template
	  rebind_traits<_Sp_counted_ptr_inplace> _Alloc_traits;
	typename _Alloc_traits::allocator_type __a(_M_impl);
	_Alloc_traits::destroy(__a, this);
	_Alloc_traits::deallocate(__a, this, 1);
      }

      // Sneaky trick so __shared_ptr can get the managed pointer
      virtual void*
      _M_get_deleter(const std::type_info& __ti) noexcept
      {
#ifdef __GXX_RTTI
	return __ti == typeid(_Sp_make_shared_tag)
	       ? static_cast<void*>(&_M_storage)
	       : 0;
#else
        return 0;
#endif
      }

    private:
      _Impl _M_impl;
      typename aligned_storage<sizeof(_Tp), alignment_of<_Tp>::value>::type
	_M_storage;
    };

  template<_Lock_policy _Lp>
    class __shared_count
    {
    public:
      constexpr __shared_count() noexcept : _M_pi(0)
      { }

      template<typename _Ptr>
        explicit
	__shared_count(_Ptr __p) : _M_pi(0)
	{
	  __try
	    {
	      _M_pi = new _Sp_counted_ptr<_Ptr, _Lp>(__p);
	    }
	  __catch(...)
	    {
	      delete __p;
	      __throw_exception_again;
	    }
	}

      template<typename _Ptr, typename _Deleter>
	__shared_count(_Ptr __p, _Deleter __d)
	: __shared_count(__p, std::move(__d), allocator<int>())
	{ }

      template<typename _Ptr, typename _Deleter, typename _Alloc>
	__shared_count(_Ptr __p, _Deleter __d, _Alloc __a) : _M_pi(0)
	{
	  typedef _Sp_counted_deleter<_Ptr, _Deleter, _Alloc, _Lp> _Sp_cd_type;
	  typedef typename allocator_traits<_Alloc>::template
	    rebind_traits<_Sp_cd_type> _Alloc_traits;
	  typename _Alloc_traits::allocator_type __a2(__a);
	  _Sp_cd_type* __mem = 0;
	  __try
	    {
	      __mem = _Alloc_traits::allocate(__a2, 1);
	      _Alloc_traits::construct(__a2, __mem,
		  __p, std::move(__d), std::move(__a));
	      _M_pi = __mem;
	    }
	  __catch(...)
	    {
	      __d(__p); // Call _Deleter on __p.
	      if (__mem)
	        _Alloc_traits::deallocate(__a2, __mem, 1);
	      __throw_exception_again;
	    }
	}

      template<typename _Tp, typename _Alloc, typename... _Args>
	__shared_count(_Sp_make_shared_tag, _Tp*, const _Alloc& __a,
		       _Args&&... __args)
	: _M_pi(0)
	{
	  typedef _Sp_counted_ptr_inplace<_Tp, _Alloc, _Lp> _Sp_cp_type;
	  typedef typename allocator_traits<_Alloc>::template
	    rebind_traits<_Sp_cp_type> _Alloc_traits;
	  typename _Alloc_traits::allocator_type __a2(__a);
	  _Sp_cp_type* __mem = _Alloc_traits::allocate(__a2, 1);
	  __try
	    {
	      _Alloc_traits::construct(__a2, __mem, std::move(__a),
		    std::forward<_Args>(__args)...);
	      _M_pi = __mem;
	    }
	  __catch(...)
	    {
	      _Alloc_traits::deallocate(__a2, __mem, 1);
	      __throw_exception_again;
	    }
	}

#if _GLIBCXX_USE_DEPRECATED
      // Special case for auto_ptr<_Tp> to provide the strong guarantee.
      template<typename _Tp>
        explicit
	__shared_count(std::auto_ptr<_Tp>&& __r);
#endif

      // Special case for unique_ptr<_Tp,_Del> to provide the strong guarantee.
      template<typename _Tp, typename _Del>
        explicit
	__shared_count(std::unique_ptr<_Tp, _Del>&& __r) : _M_pi(0)
	{
	  using _Ptr = typename unique_ptr<_Tp, _Del>::pointer;
	  using _Del2 = typename conditional<is_reference<_Del>::value,
	      reference_wrapper<typename remove_reference<_Del>::type>,
	      _Del>::type;
	  using _Sp_cd_type
	    = _Sp_counted_deleter<_Ptr, _Del2, allocator<void>, _Lp>;
	  using _Alloc = allocator<_Sp_cd_type>;
	  using _Alloc_traits = allocator_traits<_Alloc>;
	  _Alloc __a;
	  _Sp_cd_type* __mem = _Alloc_traits::allocate(__a, 1);
	  _Alloc_traits::construct(__a, __mem, __r.release(),
				   __r.get_deleter());  // non-throwing
	  _M_pi = __mem;
	}

      // Throw bad_weak_ptr when __r._M_get_use_count() == 0.
      explicit __shared_count(const __weak_count<_Lp>& __r);

      ~__shared_count() noexcept
      {
	if (_M_pi != nullptr)
	  _M_pi->_M_release();
      }

      __shared_count(const __shared_count& __r) noexcept
      : _M_pi(__r._M_pi)
      {
	if (_M_pi != 0)
	  _M_pi->_M_add_ref_copy();
      }

      __shared_count&
      operator=(const __shared_count& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != _M_pi)
	  {
	    if (__tmp != 0)
	      __tmp->_M_add_ref_copy();
	    if (_M_pi != 0)
	      _M_pi->_M_release();
	    _M_pi = __tmp;
	  }
	return *this;
      }

      void
      _M_swap(__shared_count& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }

      long
      _M_get_use_count() const noexcept
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      bool
      _M_unique() const noexcept
      { return this->_M_get_use_count() == 1; }

      void*
      _M_get_deleter(const std::type_info& __ti) const noexcept
      { return _M_pi ? _M_pi->_M_get_deleter(__ti) : 0; }

      bool
      _M_less(const __shared_count& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      bool
      _M_less(const __weak_count<_Lp>& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      // Friend function injected into enclosing namespace and found by ADL
      friend inline bool
      operator==(const __shared_count& __a, const __shared_count& __b) noexcept
      { return __a._M_pi == __b._M_pi; }

    private:
      friend class __weak_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };


  template<_Lock_policy _Lp>
    class __weak_count
    {
    public:
      constexpr __weak_count() noexcept : _M_pi(0)
      { }

      __weak_count(const __shared_count<_Lp>& __r) noexcept
      : _M_pi(__r._M_pi)
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }

      __weak_count(const __weak_count<_Lp>& __r) noexcept
      : _M_pi(__r._M_pi)
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }

      ~__weak_count() noexcept
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
      }

      __weak_count<_Lp>&
      operator=(const __shared_count<_Lp>& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != 0)
	  __tmp->_M_weak_add_ref();
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
	_M_pi = __tmp;
	return *this;
      }

      __weak_count<_Lp>&
      operator=(const __weak_count<_Lp>& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != 0)
	  __tmp->_M_weak_add_ref();
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
	_M_pi = __tmp;
	return *this;
      }

      void
      _M_swap(__weak_count<_Lp>& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }

      long
      _M_get_use_count() const noexcept
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      bool
      _M_less(const __weak_count& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      bool
      _M_less(const __shared_count<_Lp>& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      // Friend function injected into enclosing namespace and found by ADL
      friend inline bool
      operator==(const __weak_count& __a, const __weak_count& __b) noexcept
      { return __a._M_pi == __b._M_pi; }

    private:
      friend class __shared_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };

  // Now that __weak_count is defined we can define this constructor:
  template<_Lock_policy _Lp>
    inline __shared_count<_Lp>:: __shared_count(const __weak_count<_Lp>& __r)
    : _M_pi(__r._M_pi)
    {
      if (_M_pi != 0)
	_M_pi->_M_add_ref_lock();
      else
	__throw_bad_weak_ptr();
    }


  // Support for enable_shared_from_this.

  // Friend of __enable_shared_from_this.
  template<_Lock_policy _Lp, typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&,
				     const __enable_shared_from_this<_Tp1,
				     _Lp>*, const _Tp2*) noexcept;

  // Friend of enable_shared_from_this.
  template<typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<>&,
				     const enable_shared_from_this<_Tp1>*,
				     const _Tp2*) noexcept;

  template<_Lock_policy _Lp>
    inline void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&, ...) noexcept
    { }


  template<typename _Tp, _Lock_policy _Lp>
    class __shared_ptr
    {
    public:
      typedef _Tp   element_type;

      constexpr __shared_ptr() noexcept
      : _M_ptr(0), _M_refcount()
      { }

      template<typename _Tp1>
	explicit __shared_ptr(_Tp1* __p)
        : _M_ptr(__p), _M_refcount(__p)
	{
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  static_assert( sizeof(_Tp1) > 0, "incomplete type" );
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}

      template<typename _Tp1, typename _Deleter>
	__shared_ptr(_Tp1* __p, _Deleter __d)
	: _M_ptr(__p), _M_refcount(__p, __d)
	{
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  // TODO requires _Deleter CopyConstructible and __d(__p) well-formed
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}

      template<typename _Tp1, typename _Deleter, typename _Alloc>
	__shared_ptr(_Tp1* __p, _Deleter __d, _Alloc __a)
	: _M_ptr(__p), _M_refcount(__p, __d, std::move(__a))
	{
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  // TODO requires _Deleter CopyConstructible and __d(__p) well-formed
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}

      template<typename _Deleter>
	__shared_ptr(nullptr_t __p, _Deleter __d)
	: _M_ptr(0), _M_refcount(__p, __d)
	{ }

      template<typename _Deleter, typename _Alloc>
        __shared_ptr(nullptr_t __p, _Deleter __d, _Alloc __a)
	: _M_ptr(0), _M_refcount(__p, __d, std::move(__a))
	{ }

      template<typename _Tp1>
	__shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, _Tp* __p) noexcept
	: _M_ptr(__p), _M_refcount(__r._M_refcount) // never throws
	{ }

      __shared_ptr(const __shared_ptr&) noexcept = default;
      __shared_ptr& operator=(const __shared_ptr&) noexcept = default;
      ~__shared_ptr() = default;

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount)
	{ }

      __shared_ptr(__shared_ptr&& __r) noexcept
      : _M_ptr(__r._M_ptr), _M_refcount()
      {
	_M_refcount._M_swap(__r._M_refcount);
	__r._M_ptr = 0;
      }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__shared_ptr(__shared_ptr<_Tp1, _Lp>&& __r) noexcept
	: _M_ptr(__r._M_ptr), _M_refcount()
	{
	  _M_refcount._M_swap(__r._M_refcount);
	  __r._M_ptr = 0;
	}

      template<typename _Tp1>
	explicit __shared_ptr(const __weak_ptr<_Tp1, _Lp>& __r)
	: _M_refcount(__r._M_refcount) // may throw
	{
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)

	  // It is now safe to copy __r._M_ptr, as
	  // _M_refcount(__r._M_refcount) did not throw.
	  _M_ptr = __r._M_ptr;
	}

      // If an exception is thrown this constructor has no effect.
      template<typename _Tp1, typename _Del>
	__shared_ptr(std::unique_ptr<_Tp1, _Del>&& __r)
	: _M_ptr(__r.get()), _M_refcount()
	{
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  auto __tmp = std::__addressof(*__r.get());
	  _M_refcount = __shared_count<_Lp>(std::move(__r));
	  __enable_shared_from_this_helper(_M_refcount, __tmp, __tmp);
	}

#if _GLIBCXX_USE_DEPRECATED
      // Postcondition: use_count() == 1 and __r.get() == 0
      template<typename _Tp1>
	__shared_ptr(std::auto_ptr<_Tp1>&& __r);
#endif

      /* TODO: use delegating constructor */
      constexpr __shared_ptr(nullptr_t) noexcept
      : _M_ptr(0), _M_refcount()
      { }

      template<typename _Tp1>
	__shared_ptr&
	operator=(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	{
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount; // __shared_count::op= doesn't throw
	  return *this;
	}

#if _GLIBCXX_USE_DEPRECATED
      template<typename _Tp1>
	__shared_ptr&
	operator=(std::auto_ptr<_Tp1>&& __r)
	{
	  __shared_ptr(std::move(__r)).swap(*this);
	  return *this;
	}
#endif

      __shared_ptr&
      operator=(__shared_ptr&& __r) noexcept
      {
	__shared_ptr(std::move(__r)).swap(*this);
	return *this;
      }

      template<class _Tp1>
	__shared_ptr&
	operator=(__shared_ptr<_Tp1, _Lp>&& __r) noexcept
	{
	  __shared_ptr(std::move(__r)).swap(*this);
	  return *this;
	}

      template<typename _Tp1, typename _Del>
	__shared_ptr&
	operator=(std::unique_ptr<_Tp1, _Del>&& __r)
	{
	  __shared_ptr(std::move(__r)).swap(*this);
	  return *this;
	}

      void
      reset() noexcept
      { __shared_ptr().swap(*this); }

      template<typename _Tp1>
	void
	reset(_Tp1* __p) // _Tp1 must be complete.
	{
	  // Catch self-reset errors.
	  _GLIBCXX_DEBUG_ASSERT(__p == 0 || __p != _M_ptr);
	  __shared_ptr(__p).swap(*this);
	}

      template<typename _Tp1, typename _Deleter>
	void
	reset(_Tp1* __p, _Deleter __d)
	{ __shared_ptr(__p, __d).swap(*this); }

      template<typename _Tp1, typename _Deleter, typename _Alloc>
	void
        reset(_Tp1* __p, _Deleter __d, _Alloc __a)
        { __shared_ptr(__p, __d, std::move(__a)).swap(*this); }

      // Allow class instantiation when _Tp is [cv-qual] void.
      typename std::add_lvalue_reference<_Tp>::type
      operator*() const noexcept
      {
	_GLIBCXX_DEBUG_ASSERT(_M_ptr != 0);
	return *_M_ptr;
      }

      _Tp*
      operator->() const noexcept
      {
	_GLIBCXX_DEBUG_ASSERT(_M_ptr != 0);
	return _M_ptr;
      }

      _Tp*
      get() const noexcept
      { return _M_ptr; }

      explicit operator bool() const // never throws
      { return _M_ptr == 0 ? false : true; }

      bool
      unique() const noexcept
      { return _M_refcount._M_unique(); }

      long
      use_count() const noexcept
      { return _M_refcount._M_get_use_count(); }

      void
      swap(__shared_ptr<_Tp, _Lp>& __other) noexcept
      {
	std::swap(_M_ptr, __other._M_ptr);
	_M_refcount._M_swap(__other._M_refcount);
      }

      template<typename _Tp1>
	bool
	owner_before(__shared_ptr<_Tp1, _Lp> const& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

      template<typename _Tp1>
	bool
	owner_before(__weak_ptr<_Tp1, _Lp> const& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

#ifdef __GXX_RTTI
    protected:
      // This constructor is non-standard, it is used by allocate_shared.
      template<typename _Alloc, typename... _Args>
	__shared_ptr(_Sp_make_shared_tag __tag, const _Alloc& __a,
		     _Args&&... __args)
	: _M_ptr(), _M_refcount(__tag, (_Tp*)0, __a,
				std::forward<_Args>(__args)...)
	{
	  // _M_ptr needs to point to the newly constructed object.
	  // This relies on _Sp_counted_ptr_inplace::_M_get_deleter.
	  void* __p = _M_refcount._M_get_deleter(typeid(__tag));
	  _M_ptr = static_cast<_Tp*>(__p);
	  __enable_shared_from_this_helper(_M_refcount, _M_ptr, _M_ptr);
	}
#else
      template<typename _Alloc>
        struct _Deleter
        {
          void operator()(_Tp* __ptr)
          {
	    typedef allocator_traits<_Alloc> _Alloc_traits;
	    _Alloc_traits::destroy(_M_alloc, __ptr);
	    _Alloc_traits::deallocate(_M_alloc, __ptr, 1);
          }
          _Alloc _M_alloc;
        };

      template<typename _Alloc, typename... _Args>
	__shared_ptr(_Sp_make_shared_tag __tag, const _Alloc& __a,
		     _Args&&... __args)
	: _M_ptr(), _M_refcount()
        {
	  typedef typename _Alloc::template rebind<_Tp>::other _Alloc2;
          _Deleter<_Alloc2> __del = { _Alloc2(__a) };
	  typedef allocator_traits<_Alloc2> __traits;
          _M_ptr = __traits::allocate(__del._M_alloc, 1);
	  __try
	    {
	      // _GLIBCXX_RESOLVE_LIB_DEFECTS
	      // 2070. allocate_shared should use allocator_traits<A>::construct
	      __traits::construct(__del._M_alloc, _M_ptr,
		                  std::forward<_Args>(__args)...);
	    }
	  __catch(...)
	    {
	      __traits::deallocate(__del._M_alloc, _M_ptr, 1);
	      __throw_exception_again;
	    }
          __shared_count<_Lp> __count(_M_ptr, __del, __del._M_alloc);
          _M_refcount._M_swap(__count);
	  __enable_shared_from_this_helper(_M_refcount, _M_ptr, _M_ptr);
        }
#endif

      template<typename _Tp1, _Lock_policy _Lp1, typename _Alloc,
	       typename... _Args>
	friend __shared_ptr<_Tp1, _Lp1>
	__allocate_shared(const _Alloc& __a, _Args&&... __args);

    private:
      void*
      _M_get_deleter(const std::type_info& __ti) const noexcept
      { return _M_refcount._M_get_deleter(__ti); }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;

      template<typename _Del, typename _Tp1, _Lock_policy _Lp1>
	friend _Del* get_deleter(const __shared_ptr<_Tp1, _Lp1>&) noexcept;

      _Tp*	   	   _M_ptr;         // Contained pointer.
      __shared_count<_Lp>  _M_refcount;    // Reference counter.
    };


  // 20.7.2.2.7 shared_ptr comparisons
  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator==(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return __a.get() == __b.get(); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator==(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return !__a; }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator==(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return !__a; }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator!=(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return __a.get() != __b.get(); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator!=(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return (bool)__a; }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator!=(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return (bool)__a; }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator<(const __shared_ptr<_Tp1, _Lp>& __a,
	      const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    {
      typedef typename std::common_type<_Tp1*, _Tp2*>::type _CT;
      return std::less<_CT>()(__a.get(), __b.get());
    }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator<=(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return !(__b < __a); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<=(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return !(nullptr < __a); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<=(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator>(const __shared_ptr<_Tp1, _Lp>& __a,
	      const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return (__b < __a); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator>=(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return !(__a < __b); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>=(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>=(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return !(nullptr < __a); }

  template<typename _Sp>
    struct _Sp_less : public binary_function<_Sp, _Sp, bool>
    {
      bool
      operator()(const _Sp& __lhs, const _Sp& __rhs) const noexcept
      {
	typedef typename _Sp::element_type element_type;
	return std::less<element_type*>()(__lhs.get(), __rhs.get());
      }
    };

  template<typename _Tp, _Lock_policy _Lp>
    struct less<__shared_ptr<_Tp, _Lp>>
    : public _Sp_less<__shared_ptr<_Tp, _Lp>>
    { };

  // 2.2.3.8 shared_ptr specialized algorithms.
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__shared_ptr<_Tp, _Lp>& __a, __shared_ptr<_Tp, _Lp>& __b) noexcept
    { __a.swap(__b); }

  // 2.2.3.9 shared_ptr casts

  // The seemingly equivalent code:
  // shared_ptr<_Tp, _Lp>(static_cast<_Tp*>(__r.get()))
  // will eventually result in undefined behaviour, attempting to
  // delete the same object twice.
  /// static_pointer_cast
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    static_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
    { return __shared_ptr<_Tp, _Lp>(__r, static_cast<_Tp*>(__r.get())); }

  // The seemingly equivalent code:
  // shared_ptr<_Tp, _Lp>(const_cast<_Tp*>(__r.get()))
  // will eventually result in undefined behaviour, attempting to
  // delete the same object twice.
  /// const_pointer_cast
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    const_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
    { return __shared_ptr<_Tp, _Lp>(__r, const_cast<_Tp*>(__r.get())); }

  // The seemingly equivalent code:
  // shared_ptr<_Tp, _Lp>(dynamic_cast<_Tp*>(__r.get()))
  // will eventually result in undefined behaviour, attempting to
  // delete the same object twice.
  /// dynamic_pointer_cast
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    dynamic_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
    {
      if (_Tp* __p = dynamic_cast<_Tp*>(__r.get()))
	return __shared_ptr<_Tp, _Lp>(__r, __p);
      return __shared_ptr<_Tp, _Lp>();
    }


  template<typename _Tp, _Lock_policy _Lp>
    class __weak_ptr
    {
    public:
      typedef _Tp element_type;

      constexpr __weak_ptr() noexcept
      : _M_ptr(0), _M_refcount()
      { }

      __weak_ptr(const __weak_ptr&) noexcept = default;
      __weak_ptr& operator=(const __weak_ptr&) noexcept = default;
      ~__weak_ptr() = default;

      // The "obvious" converting constructor implementation:
      //
      //  template<typename _Tp1>
      //    __weak_ptr(const __weak_ptr<_Tp1, _Lp>& __r)
      //    : _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount) // never throws
      //    { }
      //
      // has a serious problem.
      //
      //  __r._M_ptr may already have been invalidated. The _M_ptr(__r._M_ptr)
      //  conversion may require access to *__r._M_ptr (virtual inheritance).
      //
      // It is not possible to avoid spurious access violations since
      // in multithreaded programs __r._M_ptr may be invalidated at any point.
      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__weak_ptr(const __weak_ptr<_Tp1, _Lp>& __r) noexcept
	: _M_refcount(__r._M_refcount)
        { _M_ptr = __r.lock().get(); }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__weak_ptr(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount)
	{ }

      template<typename _Tp1>
	__weak_ptr&
	operator=(const __weak_ptr<_Tp1, _Lp>& __r) noexcept
	{
	  _M_ptr = __r.lock().get();
	  _M_refcount = __r._M_refcount;
	  return *this;
	}

      template<typename _Tp1>
	__weak_ptr&
	operator=(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	{
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount;
	  return *this;
	}

      __shared_ptr<_Tp, _Lp>
      lock() const noexcept
      {
#ifdef __GTHREADS
	// Optimization: avoid throw overhead.
	if (expired())
	  return __shared_ptr<element_type, _Lp>();

	__try
	  {
	    return __shared_ptr<element_type, _Lp>(*this);
	  }
	__catch(const bad_weak_ptr&)
	  {
	    // Q: How can we get here?
	    // A: Another thread may have invalidated r after the
	    //    use_count test above.
	    return __shared_ptr<element_type, _Lp>();
	  }

#else
	// Optimization: avoid try/catch overhead when single threaded.
	return expired() ? __shared_ptr<element_type, _Lp>()
			 : __shared_ptr<element_type, _Lp>(*this);

#endif
      } // XXX MT

      long
      use_count() const noexcept
      { return _M_refcount._M_get_use_count(); }

      bool
      expired() const noexcept
      { return _M_refcount._M_get_use_count() == 0; }

      template<typename _Tp1>
	bool
	owner_before(const __shared_ptr<_Tp1, _Lp>& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

      template<typename _Tp1>
	bool
	owner_before(const __weak_ptr<_Tp1, _Lp>& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

      void
      reset() noexcept
      { __weak_ptr().swap(*this); }

      void
      swap(__weak_ptr& __s) noexcept
      {
	std::swap(_M_ptr, __s._M_ptr);
	_M_refcount._M_swap(__s._M_refcount);
      }

    private:
      // Used by __enable_shared_from_this.
      void
      _M_assign(_Tp* __ptr, const __shared_count<_Lp>& __refcount) noexcept
      {
	_M_ptr = __ptr;
	_M_refcount = __refcount;
      }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;
      friend class __enable_shared_from_this<_Tp, _Lp>;
      friend class enable_shared_from_this<_Tp>;

      _Tp*	 	 _M_ptr;         // Contained pointer.
      __weak_count<_Lp>  _M_refcount;    // Reference counter.
    };

  // 20.7.2.3.6 weak_ptr specialized algorithms.
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__weak_ptr<_Tp, _Lp>& __a, __weak_ptr<_Tp, _Lp>& __b) noexcept
    { __a.swap(__b); }

  template<typename _Tp, typename _Tp1>
    struct _Sp_owner_less : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __lhs, const _Tp& __rhs) const
      { return __lhs.owner_before(__rhs); }

      bool
      operator()(const _Tp& __lhs, const _Tp1& __rhs) const
      { return __lhs.owner_before(__rhs); }

      bool
      operator()(const _Tp1& __lhs, const _Tp& __rhs) const
      { return __lhs.owner_before(__rhs); }
    };

  template<typename _Tp, _Lock_policy _Lp>
    struct owner_less<__shared_ptr<_Tp, _Lp>>
    : public _Sp_owner_less<__shared_ptr<_Tp, _Lp>, __weak_ptr<_Tp, _Lp>>
    { };

  template<typename _Tp, _Lock_policy _Lp>
    struct owner_less<__weak_ptr<_Tp, _Lp>>
    : public _Sp_owner_less<__weak_ptr<_Tp, _Lp>, __shared_ptr<_Tp, _Lp>>
    { };


  template<typename _Tp, _Lock_policy _Lp>
    class __enable_shared_from_this
    {
    protected:
      constexpr __enable_shared_from_this() noexcept { }

      __enable_shared_from_this(const __enable_shared_from_this&) noexcept { }

      __enable_shared_from_this&
      operator=(const __enable_shared_from_this&) noexcept
      { return *this; }

      ~__enable_shared_from_this() { }

    public:
      __shared_ptr<_Tp, _Lp>
      shared_from_this()
      { return __shared_ptr<_Tp, _Lp>(this->_M_weak_this); }

      __shared_ptr<const _Tp, _Lp>
      shared_from_this() const
      { return __shared_ptr<const _Tp, _Lp>(this->_M_weak_this); }

    private:
      template<typename _Tp1>
	void
	_M_weak_assign(_Tp1* __p, const __shared_count<_Lp>& __n) const noexcept
	{ _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
	friend void
	__enable_shared_from_this_helper(const __shared_count<_Lp>& __pn,
					 const __enable_shared_from_this* __pe,
					 const _Tp1* __px) noexcept
	{
	  if (__pe != 0)
	    __pe->_M_weak_assign(const_cast<_Tp1*>(__px), __pn);
	}

      mutable __weak_ptr<_Tp, _Lp>  _M_weak_this;
    };


  template<typename _Tp, _Lock_policy _Lp, typename _Alloc, typename... _Args>
    inline __shared_ptr<_Tp, _Lp>
    __allocate_shared(const _Alloc& __a, _Args&&... __args)
    {
      return __shared_ptr<_Tp, _Lp>(_Sp_make_shared_tag(), __a,
				    std::forward<_Args>(__args)...);
    }

  template<typename _Tp, _Lock_policy _Lp, typename... _Args>
    inline __shared_ptr<_Tp, _Lp>
    __make_shared(_Args&&... __args)
    {
      typedef typename std::remove_const<_Tp>::type _Tp_nc;
      return std::__allocate_shared<_Tp, _Lp>(std::allocator<_Tp_nc>(),
					      std::forward<_Args>(__args)...);
    }

  /// std::hash specialization for __shared_ptr.
  template<typename _Tp, _Lock_policy _Lp>
    struct hash<__shared_ptr<_Tp, _Lp>>
    : public __hash_base<size_t, __shared_ptr<_Tp, _Lp>>
    {
      size_t
      operator()(const __shared_ptr<_Tp, _Lp>& __s) const noexcept
      { return std::hash<_Tp*>()(__s.get()); }
    };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif // _SHARED_PTR_BASE_H
