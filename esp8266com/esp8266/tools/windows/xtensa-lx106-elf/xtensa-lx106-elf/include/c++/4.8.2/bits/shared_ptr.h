// shared_ptr and weak_ptr implementation -*- C++ -*-

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

/** @file bits/shared_ptr.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{memory}
 */

#ifndef _SHARED_PTR_H
#define _SHARED_PTR_H 1

#include <bits/shared_ptr_base.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

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
    get_deleter(const __shared_ptr<_Tp, _Lp>& __p) noexcept
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
      constexpr shared_ptr() noexcept
      : __shared_ptr<_Tp>() { }

      shared_ptr(const shared_ptr&) noexcept = default;

      /**
       *  @brief  Construct a %shared_ptr that owns the pointer @a __p.
       *  @param  __p  A pointer that is convertible to element_type*.
       *  @post   use_count() == 1 && get() == __p
       *  @throw  std::bad_alloc, in which case @c delete @a __p is called.
       */
      template<typename _Tp1>
	explicit shared_ptr(_Tp1* __p)
        : __shared_ptr<_Tp>(__p) { }

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
	shared_ptr(_Tp1* __p, _Deleter __d)
        : __shared_ptr<_Tp>(__p, __d) { }

      /**
       *  @brief  Construct a %shared_ptr that owns a null pointer
       *          and the deleter @a __d.
       *  @param  __p  A null pointer constant.
       *  @param  __d  A deleter.
       *  @post   use_count() == 1 && get() == __p
       *  @throw  std::bad_alloc, in which case @a __d(__p) is called.
       *
       *  Requirements: _Deleter's copy constructor and destructor must
       *  not throw
       *
       *  The last owner will call __d(__p)
       */
      template<typename _Deleter>
	shared_ptr(nullptr_t __p, _Deleter __d)
        : __shared_ptr<_Tp>(__p, __d) { }

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
	shared_ptr(_Tp1* __p, _Deleter __d, _Alloc __a)
	: __shared_ptr<_Tp>(__p, __d, std::move(__a)) { }

      /**
       *  @brief  Construct a %shared_ptr that owns a null pointer
       *          and the deleter @a __d.
       *  @param  __p  A null pointer constant.
       *  @param  __d  A deleter.
       *  @param  __a  An allocator.
       *  @post   use_count() == 1 && get() == __p
       *  @throw  std::bad_alloc, in which case @a __d(__p) is called.
       *
       *  Requirements: _Deleter's copy constructor and destructor must
       *  not throw _Alloc's copy constructor and destructor must not
       *  throw.
       *
       *  The last owner will call __d(__p)
       */
      template<typename _Deleter, typename _Alloc>
	shared_ptr(nullptr_t __p, _Deleter __d, _Alloc __a)
	: __shared_ptr<_Tp>(__p, __d, std::move(__a)) { }

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
	shared_ptr(const shared_ptr<_Tp1>& __r, _Tp* __p) noexcept
	: __shared_ptr<_Tp>(__r, __p) { }

      /**
       *  @brief  If @a __r is empty, constructs an empty %shared_ptr;
       *          otherwise construct a %shared_ptr that shares ownership
       *          with @a __r.
       *  @param  __r  A %shared_ptr.
       *  @post   get() == __r.get() && use_count() == __r.use_count()
       */
      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	shared_ptr(const shared_ptr<_Tp1>& __r) noexcept
        : __shared_ptr<_Tp>(__r) { }

      /**
       *  @brief  Move-constructs a %shared_ptr instance from @a __r.
       *  @param  __r  A %shared_ptr rvalue.
       *  @post   *this contains the old value of @a __r, @a __r is empty.
       */
      shared_ptr(shared_ptr&& __r) noexcept
      : __shared_ptr<_Tp>(std::move(__r)) { }

      /**
       *  @brief  Move-constructs a %shared_ptr instance from @a __r.
       *  @param  __r  A %shared_ptr rvalue.
       *  @post   *this contains the old value of @a __r, @a __r is empty.
       */
      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	shared_ptr(shared_ptr<_Tp1>&& __r) noexcept
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

#if _GLIBCXX_USE_DEPRECATED
      template<typename _Tp1>
	shared_ptr(std::auto_ptr<_Tp1>&& __r);
#endif

      template<typename _Tp1, typename _Del>
	shared_ptr(std::unique_ptr<_Tp1, _Del>&& __r)
	: __shared_ptr<_Tp>(std::move(__r)) { }

      /**
       *  @brief  Construct an empty %shared_ptr.
       *  @param  __p  A null pointer constant.
       *  @post   use_count() == 0 && get() == nullptr
       */
      constexpr shared_ptr(nullptr_t __p) noexcept
      : __shared_ptr<_Tp>(__p) { }

      shared_ptr& operator=(const shared_ptr&) noexcept = default;

      template<typename _Tp1>
	shared_ptr&
	operator=(const shared_ptr<_Tp1>& __r) noexcept
	{
	  this->__shared_ptr<_Tp>::operator=(__r);
	  return *this;
	}

#if _GLIBCXX_USE_DEPRECATED
      template<typename _Tp1>
	shared_ptr&
	operator=(std::auto_ptr<_Tp1>&& __r)
	{
	  this->__shared_ptr<_Tp>::operator=(std::move(__r));
	  return *this;
	}
#endif

      shared_ptr&
      operator=(shared_ptr&& __r) noexcept
      {
	this->__shared_ptr<_Tp>::operator=(std::move(__r));
	return *this;
      }

      template<class _Tp1>
	shared_ptr&
	operator=(shared_ptr<_Tp1>&& __r) noexcept
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
	shared_ptr(_Sp_make_shared_tag __tag, const _Alloc& __a,
		   _Args&&... __args)
	: __shared_ptr<_Tp>(__tag, __a, std::forward<_Args>(__args)...)
	{ }

      template<typename _Tp1, typename _Alloc, typename... _Args>
	friend shared_ptr<_Tp1>
	allocate_shared(const _Alloc& __a, _Args&&... __args);
    };

  // 20.7.2.2.7 shared_ptr comparisons
  template<typename _Tp1, typename _Tp2>
    inline bool
    operator==(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return __a.get() == __b.get(); }

  template<typename _Tp>
    inline bool
    operator==(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return !__a; }

  template<typename _Tp>
    inline bool
    operator==(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return !__a; }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator!=(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return __a.get() != __b.get(); }

  template<typename _Tp>
    inline bool
    operator!=(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return (bool)__a; }

  template<typename _Tp>
    inline bool
    operator!=(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return (bool)__a; }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator<(const shared_ptr<_Tp1>& __a,
	      const shared_ptr<_Tp2>& __b) noexcept
    {
      typedef typename std::common_type<_Tp1*, _Tp2*>::type _CT;
      return std::less<_CT>()(__a.get(), __b.get());
    }

  template<typename _Tp>
    inline bool
    operator<(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp>
    inline bool
    operator<(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator<=(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return !(__b < __a); }

  template<typename _Tp>
    inline bool
    operator<=(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return !(nullptr < __a); }

  template<typename _Tp>
    inline bool
    operator<=(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator>(const shared_ptr<_Tp1>& __a,
	      const shared_ptr<_Tp2>& __b) noexcept
    { return (__b < __a); }

  template<typename _Tp>
    inline bool
    operator>(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp>
    inline bool
    operator>(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator>=(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return !(__a < __b); }

  template<typename _Tp>
    inline bool
    operator>=(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp>
    inline bool
    operator>=(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return !(nullptr < __a); }

  template<typename _Tp>
    struct less<shared_ptr<_Tp>> : public _Sp_less<shared_ptr<_Tp>>
    { };

  // 20.7.2.2.8 shared_ptr specialized algorithms.
  template<typename _Tp>
    inline void
    swap(shared_ptr<_Tp>& __a, shared_ptr<_Tp>& __b) noexcept
    { __a.swap(__b); }

  // 20.7.2.2.9 shared_ptr casts.
  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    static_pointer_cast(const shared_ptr<_Tp1>& __r) noexcept
    { return shared_ptr<_Tp>(__r, static_cast<_Tp*>(__r.get())); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    const_pointer_cast(const shared_ptr<_Tp1>& __r) noexcept
    { return shared_ptr<_Tp>(__r, const_cast<_Tp*>(__r.get())); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    dynamic_pointer_cast(const shared_ptr<_Tp1>& __r) noexcept
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
      constexpr weak_ptr() noexcept
      : __weak_ptr<_Tp>() { }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	weak_ptr(const weak_ptr<_Tp1>& __r) noexcept
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	weak_ptr(const shared_ptr<_Tp1>& __r) noexcept
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1>
	weak_ptr&
	operator=(const weak_ptr<_Tp1>& __r) noexcept
	{
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      template<typename _Tp1>
	weak_ptr&
	operator=(const shared_ptr<_Tp1>& __r) noexcept
	{
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      shared_ptr<_Tp>
      lock() const noexcept
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

  // 20.7.2.3.6 weak_ptr specialized algorithms.
  template<typename _Tp>
    inline void
    swap(weak_ptr<_Tp>& __a, weak_ptr<_Tp>& __b) noexcept
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
      constexpr enable_shared_from_this() noexcept { }

      enable_shared_from_this(const enable_shared_from_this&) noexcept { }

      enable_shared_from_this&
      operator=(const enable_shared_from_this&) noexcept
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
	_M_weak_assign(_Tp1* __p, const __shared_count<>& __n) const noexcept
	{ _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
	friend void
	__enable_shared_from_this_helper(const __shared_count<>& __pn,
					 const enable_shared_from_this* __pe,
					 const _Tp1* __px) noexcept
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
    allocate_shared(const _Alloc& __a, _Args&&... __args)
    {
      return shared_ptr<_Tp>(_Sp_make_shared_tag(), __a,
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
      return std::allocate_shared<_Tp>(std::allocator<_Tp_nc>(),
				       std::forward<_Args>(__args)...);
    }

  /// std::hash specialization for shared_ptr.
  template<typename _Tp>
    struct hash<shared_ptr<_Tp>>
    : public __hash_base<size_t, shared_ptr<_Tp>>
    {
      size_t
      operator()(const shared_ptr<_Tp>& __s) const noexcept
      { return std::hash<_Tp*>()(__s.get()); }
    };

  // @} group pointer_abstractions

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif // _SHARED_PTR_H
