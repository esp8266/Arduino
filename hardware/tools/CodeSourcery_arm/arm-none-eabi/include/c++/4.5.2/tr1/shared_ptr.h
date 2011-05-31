// <tr1/shared_ptr.h> -*- C++ -*-

// Copyright (C) 2007, 2008, 2009 Free Software Foundation, Inc.
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

/** @file tr1/shared_ptr.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _TR1_SHARED_PTR_H
#define _TR1_SHARED_PTR_H 1

#if defined(_GLIBCXX_INCLUDE_AS_CXX0X)
#  error TR1 header cannot be included from C++0x header
#endif

namespace std
{
namespace tr1
{
  template<typename _Ptr, typename _Deleter, _Lock_policy _Lp>
    class _Sp_counted_base_impl
    : public _Sp_counted_base<_Lp>
    {
    public:
      // Precondition: __d(__p) must not throw.
      _Sp_counted_base_impl(_Ptr __p, _Deleter __d)
      : _M_ptr(__p), _M_del(__d) { }
    
      virtual void
      _M_dispose() // nothrow
      { _M_del(_M_ptr); }
      
      virtual void*
      _M_get_deleter(const std::type_info& __ti)
      {
#ifdef __GXX_RTTI
        return __ti == typeid(_Deleter) ? &_M_del : 0;
#else
        return 0;
#endif
      }
      
    private:
      _Sp_counted_base_impl(const _Sp_counted_base_impl&);
      _Sp_counted_base_impl& operator=(const _Sp_counted_base_impl&);
      
      _Ptr      _M_ptr;  // copy constructor must not throw
      _Deleter  _M_del;  // copy constructor must not throw
    };

  template<_Lock_policy _Lp = __default_lock_policy>
    class __weak_count;

  template<typename _Tp>
    struct _Sp_deleter
    {
      typedef void result_type;
      typedef _Tp* argument_type;
      void operator()(_Tp* __p) const { delete __p; }
    };

  template<_Lock_policy _Lp = __default_lock_policy>
    class __shared_count
    {
    public: 
      __shared_count()
      : _M_pi(0) // nothrow
      { }
  
      template<typename _Ptr>
        __shared_count(_Ptr __p) : _M_pi(0)
        {
	  __try
	    {
	      typedef typename std::tr1::remove_pointer<_Ptr>::type _Tp;
	      _M_pi = new _Sp_counted_base_impl<_Ptr, _Sp_deleter<_Tp>, _Lp>(
	          __p, _Sp_deleter<_Tp>());
	    }
	  __catch(...)
	    {
	      delete __p;
	      __throw_exception_again;
	    }
	}

      template<typename _Ptr, typename _Deleter>
        __shared_count(_Ptr __p, _Deleter __d) : _M_pi(0)
        {
	  __try
	    {
	      _M_pi = new _Sp_counted_base_impl<_Ptr, _Deleter, _Lp>(__p, __d);
	    }
	  __catch(...)
	    {
	      __d(__p); // Call _Deleter on __p.
	      __throw_exception_again;
	    }
	}

      // Special case for auto_ptr<_Tp> to provide the strong guarantee.
      template<typename _Tp>
        explicit
        __shared_count(std::auto_ptr<_Tp>& __r)
	: _M_pi(new _Sp_counted_base_impl<_Tp*,
		_Sp_deleter<_Tp>, _Lp >(__r.get(), _Sp_deleter<_Tp>()))
        { __r.release(); }

      // Throw bad_weak_ptr when __r._M_get_use_count() == 0.
      explicit
      __shared_count(const __weak_count<_Lp>& __r);
  
      ~__shared_count() // nothrow
      {
	if (_M_pi != 0)
	  _M_pi->_M_release();
      }
      
      __shared_count(const __shared_count& __r)
      : _M_pi(__r._M_pi) // nothrow
      {
	if (_M_pi != 0)
	  _M_pi->_M_add_ref_copy();
      }
  
      __shared_count&
      operator=(const __shared_count& __r) // nothrow
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
      _M_swap(__shared_count& __r) // nothrow
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }
  
      long
      _M_get_use_count() const // nothrow
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      bool
      _M_unique() const // nothrow
      { return this->_M_get_use_count() == 1; }
      
      friend inline bool
      operator==(const __shared_count& __a, const __shared_count& __b)
      { return __a._M_pi == __b._M_pi; }
  
      friend inline bool
      operator<(const __shared_count& __a, const __shared_count& __b)
      { return std::less<_Sp_counted_base<_Lp>*>()(__a._M_pi, __b._M_pi); }
  
      void*
      _M_get_deleter(const std::type_info& __ti) const
      { return _M_pi ? _M_pi->_M_get_deleter(__ti) : 0; }

    private:
      friend class __weak_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };


  template<_Lock_policy _Lp>
    class __weak_count
    {
    public:
      __weak_count()
      : _M_pi(0) // nothrow
      { }
  
      __weak_count(const __shared_count<_Lp>& __r)
      : _M_pi(__r._M_pi) // nothrow
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }
      
      __weak_count(const __weak_count<_Lp>& __r)
      : _M_pi(__r._M_pi) // nothrow
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }
      
      ~__weak_count() // nothrow
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
      }
      
      __weak_count<_Lp>&
      operator=(const __shared_count<_Lp>& __r) // nothrow
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
      operator=(const __weak_count<_Lp>& __r) // nothrow
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
      _M_swap(__weak_count<_Lp>& __r) // nothrow
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }
  
      long
      _M_get_use_count() const // nothrow
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      friend inline bool
      operator==(const __weak_count<_Lp>& __a, const __weak_count<_Lp>& __b)
      { return __a._M_pi == __b._M_pi; }
      
      friend inline bool
      operator<(const __weak_count<_Lp>& __a, const __weak_count<_Lp>& __b)
      { return std::less<_Sp_counted_base<_Lp>*>()(__a._M_pi, __b._M_pi); }

    private:
      friend class __shared_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };

  // now that __weak_count is defined we can define this constructor:
  template<_Lock_policy _Lp>
    inline
    __shared_count<_Lp>::
    __shared_count(const __weak_count<_Lp>& __r)
    : _M_pi(__r._M_pi)
    {
      if (_M_pi != 0)
	_M_pi->_M_add_ref_lock();
      else
	__throw_bad_weak_ptr();
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
    class enable_shared_from_this;

  // Support for enable_shared_from_this.

  // Friend of __enable_shared_from_this.
  template<_Lock_policy _Lp, typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&,
				     const __enable_shared_from_this<_Tp1,
				     _Lp>*, const _Tp2*);

  // Friend of enable_shared_from_this.
  template<typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<>&,
				     const enable_shared_from_this<_Tp1>*,
				     const _Tp2*);

  template<_Lock_policy _Lp>
    inline void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&, ...)
    { }


  struct __static_cast_tag { };
  struct __const_cast_tag { };
  struct __dynamic_cast_tag { };

  // A smart pointer with reference-counted copy semantics.  The
  // object pointed to is deleted when the last shared_ptr pointing to
  // it is destroyed or reset.
  template<typename _Tp, _Lock_policy _Lp>
    class __shared_ptr
    {
    public:
      typedef _Tp   element_type;
      
      __shared_ptr()
      : _M_ptr(0), _M_refcount() // never throws
      { }

      template<typename _Tp1>
        explicit
        __shared_ptr(_Tp1* __p)
	: _M_ptr(__p), _M_refcount(__p)
        {
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  // __glibcxx_function_requires(_CompleteConcept<_Tp1*>)
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
      
      //  generated copy constructor, assignment, destructor are fine.
      
      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r)
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount) // never throws
        { __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>) }

      template<typename _Tp1>
        explicit
        __shared_ptr(const __weak_ptr<_Tp1, _Lp>& __r)
	: _M_refcount(__r._M_refcount) // may throw
        {
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  // It is now safe to copy __r._M_ptr, as _M_refcount(__r._M_refcount)
	  // did not throw.
	  _M_ptr = __r._M_ptr;
	}

#if !defined(__GXX_EXPERIMENTAL_CXX0X__) || _GLIBCXX_DEPRECATED
      // Postcondition: use_count() == 1 and __r.get() == 0
      template<typename _Tp1>
        explicit
        __shared_ptr(std::auto_ptr<_Tp1>& __r)
	: _M_ptr(__r.get()), _M_refcount()
        {
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  // TODO requires _Tp1 is complete, delete __r.release() well-formed
	  _Tp1* __tmp = __r.get();
	  _M_refcount = __shared_count<_Lp>(__r);
	  __enable_shared_from_this_helper(_M_refcount, __tmp, __tmp);
	}

#endif

      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, __static_cast_tag)
	: _M_ptr(static_cast<element_type*>(__r._M_ptr)),
	  _M_refcount(__r._M_refcount)
        { }

      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, __const_cast_tag)
	: _M_ptr(const_cast<element_type*>(__r._M_ptr)),
	  _M_refcount(__r._M_refcount)
        { }

      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, __dynamic_cast_tag)
	: _M_ptr(dynamic_cast<element_type*>(__r._M_ptr)),
	  _M_refcount(__r._M_refcount)
        {
	  if (_M_ptr == 0) // need to allocate new counter -- the cast failed
	    _M_refcount = __shared_count<_Lp>();
	}

      template<typename _Tp1>
        __shared_ptr&
        operator=(const __shared_ptr<_Tp1, _Lp>& __r) // never throws
        {
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount; // __shared_count::op= doesn't throw
	  return *this;
	}

#if !defined(__GXX_EXPERIMENTAL_CXX0X__) || _GLIBCXX_DEPRECATED
      template<typename _Tp1>
        __shared_ptr&
        operator=(std::auto_ptr<_Tp1>& __r)
        {
	  __shared_ptr(__r).swap(*this);
	  return *this;
	}
#endif

      void
      reset() // never throws
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

      // Allow class instantiation when _Tp is [cv-qual] void.
      typename std::tr1::add_reference<_Tp>::type
      operator*() const // never throws
      {
	_GLIBCXX_DEBUG_ASSERT(_M_ptr != 0);
	return *_M_ptr;
      }

      _Tp*
      operator->() const // never throws
      {
	_GLIBCXX_DEBUG_ASSERT(_M_ptr != 0);
	return _M_ptr;
      }
    
      _Tp*
      get() const // never throws
      { return _M_ptr; }

      // Implicit conversion to "bool"
    private:
      typedef _Tp* __shared_ptr::*__unspecified_bool_type;

    public:
      operator __unspecified_bool_type() const // never throws
      { return _M_ptr == 0 ? 0 : &__shared_ptr::_M_ptr; }

      bool
      unique() const // never throws
      { return _M_refcount._M_unique(); }

      long
      use_count() const // never throws
      { return _M_refcount._M_get_use_count(); }

      void
      swap(__shared_ptr<_Tp, _Lp>& __other) // never throws
      {
	std::swap(_M_ptr, __other._M_ptr);
	_M_refcount._M_swap(__other._M_refcount);
      }

    private:
      void*
      _M_get_deleter(const std::type_info& __ti) const
      { return _M_refcount._M_get_deleter(__ti); }

      template<typename _Tp1, _Lock_policy _Lp1>
        bool
        _M_less(const __shared_ptr<_Tp1, _Lp1>& __rhs) const
        { return _M_refcount < __rhs._M_refcount; }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;

      template<typename _Del, typename _Tp1, _Lock_policy _Lp1>
        friend _Del* get_deleter(const __shared_ptr<_Tp1, _Lp1>&);

      // Friends injected into enclosing namespace and found by ADL:
      template<typename _Tp1>
        friend inline bool
        operator==(const __shared_ptr& __a, const __shared_ptr<_Tp1, _Lp>& __b)
        { return __a.get() == __b.get(); }

      template<typename _Tp1>
        friend inline bool
        operator!=(const __shared_ptr& __a, const __shared_ptr<_Tp1, _Lp>& __b)
        { return __a.get() != __b.get(); }

      template<typename _Tp1>
        friend inline bool
        operator<(const __shared_ptr& __a, const __shared_ptr<_Tp1, _Lp>& __b)
        { return __a._M_less(__b); }

      _Tp*         	   _M_ptr;         // Contained pointer.
      __shared_count<_Lp>  _M_refcount;    // Reference counter.
    };

  // 2.2.3.8 shared_ptr specialized algorithms.
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__shared_ptr<_Tp, _Lp>& __a, __shared_ptr<_Tp, _Lp>& __b)
    { __a.swap(__b); }

  // 2.2.3.9 shared_ptr casts
  /*  The seemingly equivalent
   *           shared_ptr<_Tp, _Lp>(static_cast<_Tp*>(__r.get()))
   *  will eventually result in undefined behaviour,
   *  attempting to delete the same object twice.
   */
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    static_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r)
    { return __shared_ptr<_Tp, _Lp>(__r, __static_cast_tag()); }

  /*  The seemingly equivalent
   *           shared_ptr<_Tp, _Lp>(const_cast<_Tp*>(__r.get()))
   *  will eventually result in undefined behaviour,
   *  attempting to delete the same object twice.
   */
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    const_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r)
    { return __shared_ptr<_Tp, _Lp>(__r, __const_cast_tag()); }

  /*  The seemingly equivalent
   *           shared_ptr<_Tp, _Lp>(dynamic_cast<_Tp*>(__r.get()))
   *  will eventually result in undefined behaviour,
   *  attempting to delete the same object twice.
   */
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    dynamic_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r)
    { return __shared_ptr<_Tp, _Lp>(__r, __dynamic_cast_tag()); }

  // 2.2.3.7 shared_ptr I/O
  template<typename _Ch, typename _Tr, typename _Tp, _Lock_policy _Lp>
    std::basic_ostream<_Ch, _Tr>&
    operator<<(std::basic_ostream<_Ch, _Tr>& __os, 
	       const __shared_ptr<_Tp, _Lp>& __p)
    {
      __os << __p.get();
      return __os;
    }

  // 2.2.3.10 shared_ptr get_deleter (experimental)
  template<typename _Del, typename _Tp, _Lock_policy _Lp>
    inline _Del*
    get_deleter(const __shared_ptr<_Tp, _Lp>& __p)
    {
#ifdef __GXX_RTTI
      return static_cast<_Del*>(__p._M_get_deleter(typeid(_Del)));
#else
      return 0;
#endif
    }


  template<typename _Tp, _Lock_policy _Lp>
    class __weak_ptr
    {
    public:
      typedef _Tp element_type;
      
      __weak_ptr()
      : _M_ptr(0), _M_refcount() // never throws
      { }

      // Generated copy constructor, assignment, destructor are fine.
      
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
      template<typename _Tp1>
        __weak_ptr(const __weak_ptr<_Tp1, _Lp>& __r)
	: _M_refcount(__r._M_refcount) // never throws
        {
	  __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>)
	  _M_ptr = __r.lock().get();
	}

      template<typename _Tp1>
        __weak_ptr(const __shared_ptr<_Tp1, _Lp>& __r)
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount) // never throws
        { __glibcxx_function_requires(_ConvertibleConcept<_Tp1*, _Tp*>) }

      template<typename _Tp1>
        __weak_ptr&
        operator=(const __weak_ptr<_Tp1, _Lp>& __r) // never throws
        {
	  _M_ptr = __r.lock().get();
	  _M_refcount = __r._M_refcount;
	  return *this;
	}
      
      template<typename _Tp1>
        __weak_ptr&
        operator=(const __shared_ptr<_Tp1, _Lp>& __r) // never throws
        {
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount;
	  return *this;
	}

      __shared_ptr<_Tp, _Lp>
      lock() const // never throws
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
      use_count() const // never throws
      { return _M_refcount._M_get_use_count(); }

      bool
      expired() const // never throws
      { return _M_refcount._M_get_use_count() == 0; }
      
      void
      reset() // never throws
      { __weak_ptr().swap(*this); }

      void
      swap(__weak_ptr& __s) // never throws
      {
	std::swap(_M_ptr, __s._M_ptr);
	_M_refcount._M_swap(__s._M_refcount);
      }

    private:
      // Used by __enable_shared_from_this.
      void
      _M_assign(_Tp* __ptr, const __shared_count<_Lp>& __refcount)
      {
	_M_ptr = __ptr;
	_M_refcount = __refcount;
      }

      template<typename _Tp1>
        bool
        _M_less(const __weak_ptr<_Tp1, _Lp>& __rhs) const
        { return _M_refcount < __rhs._M_refcount; }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;
      friend class __enable_shared_from_this<_Tp, _Lp>;
      friend class enable_shared_from_this<_Tp>;

      // Friend injected into namespace and found by ADL.
      template<typename _Tp1>
        friend inline bool
        operator<(const __weak_ptr& __lhs, const __weak_ptr<_Tp1, _Lp>& __rhs)
        { return __lhs._M_less(__rhs); }

      _Tp*       	 _M_ptr;         // Contained pointer.
      __weak_count<_Lp>  _M_refcount;    // Reference counter.
    };

  // 2.2.4.7 weak_ptr specialized algorithms.
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__weak_ptr<_Tp, _Lp>& __a, __weak_ptr<_Tp, _Lp>& __b)
    { __a.swap(__b); }


  template<typename _Tp, _Lock_policy _Lp>
    class __enable_shared_from_this
    {
    protected:
      __enable_shared_from_this() { }
      
      __enable_shared_from_this(const __enable_shared_from_this&) { }
      
      __enable_shared_from_this&
      operator=(const __enable_shared_from_this&)
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
        _M_weak_assign(_Tp1* __p, const __shared_count<_Lp>& __n) const
        { _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
        friend void
        __enable_shared_from_this_helper(const __shared_count<_Lp>& __pn,
					 const __enable_shared_from_this* __pe,
					 const _Tp1* __px)
        {
	  if (__pe != 0)
	    __pe->_M_weak_assign(const_cast<_Tp1*>(__px), __pn);
	}

      mutable __weak_ptr<_Tp, _Lp>  _M_weak_this;
    };


  // The actual shared_ptr, with forwarding constructors and
  // assignment operators.
  template<typename _Tp>
    class shared_ptr
    : public __shared_ptr<_Tp>
    {
    public:
      shared_ptr()
      : __shared_ptr<_Tp>() { }

      template<typename _Tp1>
        explicit
        shared_ptr(_Tp1* __p)
	: __shared_ptr<_Tp>(__p) { }

      template<typename _Tp1, typename _Deleter>
        shared_ptr(_Tp1* __p, _Deleter __d)
	: __shared_ptr<_Tp>(__p, __d) { }

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        explicit
        shared_ptr(const weak_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }

#if !defined(__GXX_EXPERIMENTAL_CXX0X__) || _GLIBCXX_DEPRECATED
      template<typename _Tp1>
        explicit
        shared_ptr(std::auto_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }
#endif

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r, __static_cast_tag)
	: __shared_ptr<_Tp>(__r, __static_cast_tag()) { }

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r, __const_cast_tag)
	: __shared_ptr<_Tp>(__r, __const_cast_tag()) { }

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r, __dynamic_cast_tag)
	: __shared_ptr<_Tp>(__r, __dynamic_cast_tag()) { }

      template<typename _Tp1>
        shared_ptr&
        operator=(const shared_ptr<_Tp1>& __r) // never throws
        {
	  this->__shared_ptr<_Tp>::operator=(__r);
	  return *this;
	}

#if !defined(__GXX_EXPERIMENTAL_CXX0X__) || _GLIBCXX_DEPRECATED
      template<typename _Tp1>
        shared_ptr&
        operator=(std::auto_ptr<_Tp1>& __r)
        {
	  this->__shared_ptr<_Tp>::operator=(__r);
	  return *this;
	}
#endif
    };

  // 2.2.3.8 shared_ptr specialized algorithms.
  template<typename _Tp>
    inline void
    swap(__shared_ptr<_Tp>& __a, __shared_ptr<_Tp>& __b)
    { __a.swap(__b); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    static_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, __static_cast_tag()); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    const_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, __const_cast_tag()); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    dynamic_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, __dynamic_cast_tag()); }


  // The actual weak_ptr, with forwarding constructors and
  // assignment operators.
  template<typename _Tp>
    class weak_ptr
    : public __weak_ptr<_Tp>
    {
    public:
      weak_ptr()
      : __weak_ptr<_Tp>() { }
      
      template<typename _Tp1>
        weak_ptr(const weak_ptr<_Tp1>& __r)
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        weak_ptr(const shared_ptr<_Tp1>& __r)
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        weak_ptr&
        operator=(const weak_ptr<_Tp1>& __r) // never throws
        {
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      template<typename _Tp1>
        weak_ptr&
        operator=(const shared_ptr<_Tp1>& __r) // never throws
        {
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      shared_ptr<_Tp>
      lock() const // never throws
      {
#ifdef __GTHREADS
	if (this->expired())
	  return shared_ptr<_Tp>();

	__try
	  {
	    return shared_ptr<_Tp>(*this);
	  }
	__catch(const bad_weak_ptr&)
	  {
	    return shared_ptr<_Tp>();
	  }
#else
	return this->expired() ? shared_ptr<_Tp>()
	                       : shared_ptr<_Tp>(*this);
#endif
      }
    };

  template<typename _Tp>
    class enable_shared_from_this
    {
    protected:
      enable_shared_from_this() { }
      
      enable_shared_from_this(const enable_shared_from_this&) { }

      enable_shared_from_this&
      operator=(const enable_shared_from_this&)
      { return *this; }

      ~enable_shared_from_this() { }

    public:
      shared_ptr<_Tp>
      shared_from_this()
      { return shared_ptr<_Tp>(this->_M_weak_this); }

      shared_ptr<const _Tp>
      shared_from_this() const
      { return shared_ptr<const _Tp>(this->_M_weak_this); }

    private:
      template<typename _Tp1>
        void
        _M_weak_assign(_Tp1* __p, const __shared_count<>& __n) const
        { _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
        friend void
        __enable_shared_from_this_helper(const __shared_count<>& __pn,
					 const enable_shared_from_this* __pe,
					 const _Tp1* __px)
        {
	  if (__pe != 0)
	    __pe->_M_weak_assign(const_cast<_Tp1*>(__px), __pn);
	}

      mutable weak_ptr<_Tp>  _M_weak_this;
    };
}
}

#endif // _TR1_SHARED_PTR_H
