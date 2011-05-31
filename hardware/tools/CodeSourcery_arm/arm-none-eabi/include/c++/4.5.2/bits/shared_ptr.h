// shared_ptr and weak_ptr implementation -*- C++ -*-

// Copyright (C) 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
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

/** @file bits/shared_ptr.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _SHARED_PTR_H
#define _SHARED_PTR_H 1

#include <bits/shared_ptr_base.h>

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   * @addtogroup pointer_abstractions
   * @{
   */

  /// 2.2.3.7 shared_ptr I/O
  template<typename _Ch, typename _Tr, typename _Tp, _Lock_policy _Lp>
    inline std::basic_ostream<_Ch, _Tr>&
    operator<<(std::basic_ostream<_Ch, _Tr>& __os,
	       const __shared_ptr<_Tp, _Lp>& __p)
    {
      __os << __p.get();
      return __os;
    }

  /// 2.2.3.10 shared_ptr get_deleter (experimental)
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


  /**
   *  @brief  A smart pointer with reference-counted copy semantics.
   *
   *  The object pointed to is deleted when the last shared_ptr pointing to
   *  it is destroyed or reset.
  */
  template<typename _Tp>
    class shared_ptr : public __shared_ptr<_Tp>
    {
    public:
      /**
       *  @brief  Construct an empty %shared_ptr.
       *  @post   use_count()==0 && get()==0
       */
      shared_ptr() : __shared_ptr<_Tp>() { }

      /**
       *  @brief  Construct a %shared_ptr that owns the pointer @a __p.
       *  @param  __p  A pointer that is convertible to element_type*.
       *  @post   use_count() == 1 && get() == __p
       *  @throw  std::bad_alloc, in which case @c delete @a __p is called.
       */
      template<typename _Tp1>
	explicit shared_ptr(_Tp1* __p) : __shared_ptr<_Tp>(__p) { }

      /**
       *  @brief  Construct a %shared_ptr that owns the pointer @a __p
       *          and the deleter @a __d.
       *  @param  __p  A pointer.
       *  @param  __d  A deleter.
       *  @post   use_count() == 1 && get() == __p
       *  @throw  std::bad_alloc, in which case @a __d(__p) is called.
       *
       *  Requirements: _Deleter's copy constructor and destructor must
       *  not throw
       *
       *  __shared_ptr will release __p by calling __d(__p)
       */
      template<typename _Tp1, typename _Deleter>
	shared_ptr(_Tp1* __p, _Deleter __d) : __shared_ptr<_Tp>(__p, __d) { }

      /**
       *  @brief  Construct a %shared_ptr that owns the pointer @a __p
       *          and the deleter @a __d.
       *  @param  __p  A pointer.
       *  @param  __d  A deleter.
       *  @param  __a  An allocator.
       *  @post   use_count() == 1 && get() == __p
       *  @throw  std::bad_alloc, in which case @a __d(__p) is called.
       *
       *  Requirements: _Deleter's copy constructor and destructor must
       *  not throw _Alloc's copy constructor and destructor must not
       *  throw.
       *
       *  __shared_ptr will release __p by calling __d(__p)
       */
      template<typename _Tp1, typename _Deleter, typename _Alloc>
	shared_ptr(_Tp1* __p, _Deleter __d, const _Alloc& __a)
	: __shared_ptr<_Tp>(__p, __d, __a) { }

      // Aliasing constructor

      /**
       *  @brief  Constructs a %shared_ptr instance that stores @a __p
       *          and shares ownership with @a __r.
       *  @param  __r  A %shared_ptr.
       *  @param  __p  A pointer that will remain valid while @a *__r is valid.
       *  @post   get() == __p && use_count() == __r.use_count()
       *
       *  This can be used to construct a @c shared_ptr to a sub-object
       *  of an object managed by an existing @c shared_ptr.
       *
       * @code
       * shared_ptr< pair<int,int> > pii(new pair<int,int>());
       * shared_ptr<int> pi(pii, &pii->first);
       * assert(pii.use_count() == 2);
       * @endcode
       */
      template<typename _Tp1>
	shared_ptr(const shared_ptr<_Tp1>& __r, _Tp* __p)
	: __shared_ptr<_Tp>(__r, __p) { }

      /**
       *  @brief  If @a __r is empty, constructs an empty %shared_ptr;
       *          otherwise construct a %shared_ptr that shares ownership
       *          with @a __r.
       *  @param  __r  A %shared_ptr.
       *  @post   get() == __r.get() && use_count() == __r.use_count()
       */
      template<typename _Tp1>
	shared_ptr(const shared_ptr<_Tp1>& __r) : __shared_ptr<_Tp>(__r) { }

      /**
       *  @brief  Move-constructs a %shared_ptr instance from @a __r.
       *  @param  __r  A %shared_ptr rvalue.
       *  @post   *this contains the old value of @a __r, @a __r is empty.
       */
      shared_ptr(shared_ptr&& __r)
      : __shared_ptr<_Tp>(std::move(__r)) { }

      /**
       *  @brief  Move-constructs a %shared_ptr instance from @a __r.
       *  @param  __r  A %shared_ptr rvalue.
       *  @post   *this contains the old value of @a __r, @a __r is empty.
       */
      template<typename _Tp1>
	shared_ptr(shared_ptr<_Tp1>&& __r)
	: __shared_ptr<_Tp>(std::move(__r)) { }

      /**
       *  @brief  Constructs a %shared_ptr that shares ownership with @a __r
       *          and stores a copy of the pointer stored in @a __r.
       *  @param  __r  A weak_ptr.
       *  @post   use_count() == __r.use_count()
       *  @throw  bad_weak_ptr when __r.expired(),
       *          in which case the constructor has no effect.
       */
      template<typename _Tp1>
	explicit shared_ptr(const weak_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }

#if _GLIBCXX_DEPRECATED
      template<typename _Tp1>
	shared_ptr(std::auto_ptr<_Tp1>&& __r)
	: __shared_ptr<_Tp>(std::move(__r)) { }
#endif

      template<typename _Tp1, typename _Del>
	shared_ptr(std::unique_ptr<_Tp1, _Del>&& __r)
	: __shared_ptr<_Tp>(std::move(__r)) { }

      template<typename _Tp1>
	shared_ptr&
	operator=(const shared_ptr<_Tp1>& __r) // never throws
	{
	  this->__shared_ptr<_Tp>::operator=(__r);
	  return *this;
	}

#if _GLIBCXX_DEPRECATED
      template<typename _Tp1>
	shared_ptr&
	operator=(std::auto_ptr<_Tp1>&& __r)
	{
	  this->__shared_ptr<_Tp>::operator=(std::move(__r));
	  return *this;
	}
#endif

      shared_ptr&
      operator=(shared_ptr&& __r)
      {
	this->__shared_ptr<_Tp>::operator=(std::move(__r));
	return *this;
      }

      template<class _Tp1>
	shared_ptr&
	operator=(shared_ptr<_Tp1>&& __r)
	{
	  this->__shared_ptr<_Tp>::operator=(std::move(__r));
	  return *this;
	}

      template<typename _Tp1, typename _Del>
	shared_ptr&
	operator=(std::unique_ptr<_Tp1, _Del>&& __r)
	{
	  this->__shared_ptr<_Tp>::operator=(std::move(__r));
	  return *this;
	}

    private:
      // This constructor is non-standard, it is used by allocate_shared.
      template<typename _Alloc, typename... _Args>
	shared_ptr(_Sp_make_shared_tag __tag, _Alloc __a, _Args&&... __args)
	: __shared_ptr<_Tp>(__tag, __a, std::forward<_Args>(__args)...)
	{ }

      template<typename _Tp1, typename _Alloc, typename... _Args>
	friend shared_ptr<_Tp1>
	allocate_shared(_Alloc __a, _Args&&... __args);
    };

  // 20.8.13.2.7 shared_ptr comparisons
  template<typename _Tp1, typename _Tp2>
    inline bool
    operator==(const shared_ptr<_Tp1>& __a, const shared_ptr<_Tp2>& __b)
    { return __a.get() == __b.get(); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator!=(const shared_ptr<_Tp1>& __a, const shared_ptr<_Tp2>& __b)
    { return __a.get() != __b.get(); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator<(const shared_ptr<_Tp1>& __a, const shared_ptr<_Tp2>& __b)
    { return __a.get() < __b.get(); }

  template<typename _Tp>
    struct less<shared_ptr<_Tp>> : public _Sp_less<shared_ptr<_Tp>>
    { };

  // 20.8.13.2.9 shared_ptr specialized algorithms.
  template<typename _Tp>
    inline void
    swap(shared_ptr<_Tp>& __a, shared_ptr<_Tp>& __b)
    { __a.swap(__b); }

  // 20.8.13.2.10 shared_ptr casts.
  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    static_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, static_cast<_Tp*>(__r.get())); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    const_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, const_cast<_Tp*>(__r.get())); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    dynamic_pointer_cast(const shared_ptr<_Tp1>& __r)
    {
      if (_Tp* __p = dynamic_cast<_Tp*>(__r.get()))
	return shared_ptr<_Tp>(__r, __p);
      return shared_ptr<_Tp>();
    }


  /**
   *  @brief  A smart pointer with weak semantics.
   *
   *  With forwarding constructors and assignment operators.
   */
  template<typename _Tp>
    class weak_ptr : public __weak_ptr<_Tp>
    {
    public:
      weak_ptr() : __weak_ptr<_Tp>() { }

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
	return this->expired() ? shared_ptr<_Tp>() : shared_ptr<_Tp>(*this);
#endif
      }
    };

  // 20.8.13.3.7 weak_ptr specialized algorithms.
  template<typename _Tp>
    inline void
    swap(weak_ptr<_Tp>& __a, weak_ptr<_Tp>& __b)
    { __a.swap(__b); }


  /// Primary template owner_less
  template<typename _Tp>
    struct owner_less;

  /// Partial specialization of owner_less for shared_ptr.
  template<typename _Tp>
    struct owner_less<shared_ptr<_Tp>>
    : public _Sp_owner_less<shared_ptr<_Tp>, weak_ptr<_Tp>>
    { };

  /// Partial specialization of owner_less for weak_ptr.
  template<typename _Tp>
    struct owner_less<weak_ptr<_Tp>>
    : public _Sp_owner_less<weak_ptr<_Tp>, shared_ptr<_Tp>>
    { };

  /**
   *  @brief Base class allowing use of member function shared_from_this.
   */
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

  /**
   *  @brief  Create an object that is owned by a shared_ptr.
   *  @param  __a     An allocator.
   *  @param  __args  Arguments for the @a _Tp object's constructor.
   *  @return A shared_ptr that owns the newly created object.
   *  @throw  An exception thrown from @a _Alloc::allocate or from the
   *          constructor of @a _Tp.
   *
   *  A copy of @a __a will be used to allocate memory for the shared_ptr
   *  and the new object.
   */
  template<typename _Tp, typename _Alloc, typename... _Args>
    inline shared_ptr<_Tp>
    allocate_shared(_Alloc __a, _Args&&... __args)
    {
      return shared_ptr<_Tp>(_Sp_make_shared_tag(), std::forward<_Alloc>(__a),
			     std::forward<_Args>(__args)...);
    }

  /**
   *  @brief  Create an object that is owned by a shared_ptr.
   *  @param  __args  Arguments for the @a _Tp object's constructor.
   *  @return A shared_ptr that owns the newly created object.
   *  @throw  std::bad_alloc, or an exception thrown from the
   *          constructor of @a _Tp.
   */
  template<typename _Tp, typename... _Args>
    inline shared_ptr<_Tp>
    make_shared(_Args&&... __args)
    {
      typedef typename std::remove_const<_Tp>::type _Tp_nc;
      return allocate_shared<_Tp>(std::allocator<_Tp_nc>(),
				  std::forward<_Args>(__args)...);
    }

  // @} group pointer_abstractions

_GLIBCXX_END_NAMESPACE

#endif // _SHARED_PTR_H
