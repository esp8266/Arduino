// unique_ptr implementation -*- C++ -*-

// Copyright (C) 2008, 2009, 2010 Free Software Foundation, Inc.
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

/** @file unique_ptr.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _UNIQUE_PTR_H
#define _UNIQUE_PTR_H 1

#include <bits/c++config.h>
#include <debug/debug.h>
#include <type_traits>
#include <utility>
#include <tuple>

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   * @addtogroup pointer_abstractions
   * @{
   */

  /// Primary template, default_delete.
  template<typename _Tp> 
    struct default_delete
      {
	default_delete() { }

	template<typename _Up>
	  default_delete(const default_delete<_Up>&) { }

	void
	operator()(_Tp* __ptr) const
	{
	  static_assert(sizeof(_Tp)>0,
			"can't delete pointer to incomplete type");
	  delete __ptr;
	}
    };

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // DR 740 - omit specialization for array objects with a compile time length
  /// Specialization, default_delete.
  template<typename _Tp> 
    struct default_delete<_Tp[]>
    {
      void
      operator()(_Tp* __ptr) const
      {
	static_assert(sizeof(_Tp)>0,
		      "can't delete pointer to incomplete type");
	delete [] __ptr;
      }
    };

  /// 20.7.12.2 unique_ptr for single objects.
  template <typename _Tp, typename _Tp_Deleter = default_delete<_Tp> > 
    class unique_ptr
    {
      typedef std::tuple<_Tp*, _Tp_Deleter>  __tuple_type;
      typedef _Tp* unique_ptr::*             __unspecified_pointer_type;

    public:
      typedef _Tp*               pointer;
      typedef _Tp                element_type;      
      typedef _Tp_Deleter        deleter_type;

      // Constructors.
      unique_ptr()
      : _M_t(pointer(), deleter_type())
      { static_assert(!std::is_pointer<deleter_type>::value,
		      "constructed with null function pointer deleter"); }

      explicit
      unique_ptr(pointer __p)
      : _M_t(__p, deleter_type())
      { static_assert(!std::is_pointer<deleter_type>::value,
		     "constructed with null function pointer deleter"); }

      unique_ptr(pointer __p,
          typename std::conditional<std::is_reference<deleter_type>::value, 
            deleter_type, const deleter_type&>::type __d)
      : _M_t(__p, __d) { }

      unique_ptr(pointer __p,
          typename std::remove_reference<deleter_type>::type&& __d)
      : _M_t(std::move(__p), std::move(__d))
      { static_assert(!std::is_reference<deleter_type>::value, 
		      "rvalue deleter bound to reference"); }

      // Move constructors.
      unique_ptr(unique_ptr&& __u) 
      : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }

      template<typename _Up, typename _Up_Deleter> 
        unique_ptr(unique_ptr<_Up, _Up_Deleter>&& __u) 
        : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter()))
	{ }

      // Destructor.
      ~unique_ptr() { reset(); }
    
      // Assignment.
      unique_ptr&
      operator=(unique_ptr&& __u)
      { 
        reset(__u.release()); 
        get_deleter() = std::move(__u.get_deleter()); 
        return *this;
      }

      template<typename _Up, typename _Up_Deleter> 
        unique_ptr&
        operator=(unique_ptr<_Up, _Up_Deleter>&& __u)
	{
          reset(__u.release()); 
          get_deleter() = std::move(__u.get_deleter()); 
          return *this;
        }

      unique_ptr&
      operator=(__unspecified_pointer_type) 
      {
	reset();
	return *this;
      }

      // Observers.
      typename std::add_lvalue_reference<element_type>::type
      operator*() const
      {
	_GLIBCXX_DEBUG_ASSERT(get() != pointer());
	return *get();
      }

      pointer
      operator->() const
      {
	_GLIBCXX_DEBUG_ASSERT(get() != pointer());
	return get();
      }

      pointer
      get() const
      { return std::get<0>(_M_t); }

      deleter_type&
      get_deleter()
      { return std::get<1>(_M_t); }

      const deleter_type&
      get_deleter() const
      { return std::get<1>(_M_t); }

      explicit operator bool() const
      { return get() == pointer() ? false : true; }

      // Modifiers.
      pointer
      release() 
      {
	pointer __p = get();
	std::get<0>(_M_t) = pointer();
	return __p;
      }

      void
      reset(pointer __p = pointer())
      {
	using std::swap;
	swap(std::get<0>(_M_t), __p);
	if (__p != pointer())
	  get_deleter()(__p);
      }

      void
      swap(unique_ptr& __u)
      {
	using std::swap;
	swap(_M_t, __u._M_t);
      }

      // Disable copy from lvalue.
      unique_ptr(const unique_ptr&) = delete;
      unique_ptr& operator=(const unique_ptr&) = delete;

    private:
      __tuple_type _M_t;
  };
 
  /// 20.7.12.3 unique_ptr for array objects with a runtime length
  // [unique.ptr.runtime]
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // DR 740 - omit specialization for array objects with a compile time length
  template<typename _Tp, typename _Tp_Deleter> 
    class unique_ptr<_Tp[], _Tp_Deleter>
    {
      typedef std::tuple<_Tp*, _Tp_Deleter>  __tuple_type;
      typedef _Tp* unique_ptr::*             __unspecified_pointer_type;

    public:
      typedef _Tp*               pointer;
      typedef _Tp                element_type;      
      typedef _Tp_Deleter        deleter_type;

      // Constructors.
      unique_ptr()
      : _M_t(pointer(), deleter_type())
      { static_assert(!std::is_pointer<deleter_type>::value,
		      "constructed with null function pointer deleter"); }

      explicit
      unique_ptr(pointer __p)
      : _M_t(__p, deleter_type())
      { static_assert(!std::is_pointer<deleter_type>::value,
		      "constructed with null function pointer deleter"); }

      unique_ptr(pointer __p,
          typename std::conditional<std::is_reference<deleter_type>::value, 
              deleter_type, const deleter_type&>::type __d) 
      : _M_t(__p, __d) { }

      unique_ptr(pointer __p,
		 typename std::remove_reference<deleter_type>::type && __d)
      : _M_t(std::move(__p), std::move(__d))
      { static_assert(!std::is_reference<deleter_type>::value, 
		      "rvalue deleter bound to reference"); }

      // Move constructors.
      unique_ptr(unique_ptr&& __u) 
      : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }

      template<typename _Up, typename _Up_Deleter> 
        unique_ptr(unique_ptr<_Up, _Up_Deleter>&& __u) 
	: _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter()))
	{ }

      // Destructor.
      ~unique_ptr() { reset(); }

      // Assignment.
      unique_ptr&
      operator=(unique_ptr&& __u)
      {
	reset(__u.release());
	get_deleter() = std::move(__u.get_deleter()); 
	return *this; 
      }

      template<typename _Up, typename _Up_Deleter> 
        unique_ptr&
        operator=(unique_ptr<_Up, _Up_Deleter>&& __u)
	{
          reset(__u.release());
          get_deleter() = std::move(__u.get_deleter()); 
          return *this;
        }

      unique_ptr&
      operator=(__unspecified_pointer_type)
      {
	reset();
	return *this;
      }

      // Observers.
      typename std::add_lvalue_reference<element_type>::type 
      operator[](size_t __i) const 
      {
	_GLIBCXX_DEBUG_ASSERT(get() != pointer());
	return get()[__i];
      }

      pointer
      get() const
      { return std::get<0>(_M_t); }

      deleter_type& 
      get_deleter()
      { return std::get<1>(_M_t); }

      const deleter_type&
      get_deleter() const
      { return std::get<1>(_M_t); }    

      explicit operator bool() const 
      { return get() == pointer() ? false : true; }
    
      // Modifiers.
      pointer
      release() 
      {
	pointer __p = get();
	std::get<0>(_M_t) = pointer();
	return __p;
      }

      void
      reset(pointer __p = pointer()) 
      {
	using std::swap;
	swap(std::get<0>(_M_t), __p);
	if (__p != pointer())
	  get_deleter()(__p);
      }

      // DR 821.
      template<typename _Up>
        void reset(_Up) = delete;

      void
      swap(unique_ptr& __u)
      {
	using std::swap;
	swap(_M_t, __u._M_t);
      }

      // Disable copy from lvalue.
      unique_ptr(const unique_ptr&) = delete;
      unique_ptr& operator=(const unique_ptr&) = delete;

      // Disable construction from convertible pointer types.
      // (N2315 - 20.6.5.3.1)
      template<typename _Up>
        unique_ptr(_Up*, typename
		   std::conditional<std::is_reference<deleter_type>::value,
		   deleter_type, const deleter_type&>::type,
		   typename std::enable_if<std::is_convertible<_Up*, 
		   pointer>::value>::type* = 0) = delete;

      template<typename _Up>
        unique_ptr(_Up*, typename std::remove_reference<deleter_type>::type&&,
		   typename std::enable_if<std::is_convertible<_Up*, 
		   pointer>::value>::type* = 0) = delete;

      template<typename _Up>
        explicit
        unique_ptr(_Up*, typename std::enable_if<std::is_convertible<_Up*, 
		   pointer>::value>::type* = 0) = delete;

    private:
      __tuple_type _M_t;
  };
  
  template<typename _Tp, typename _Tp_Deleter> 
    inline void
    swap(unique_ptr<_Tp, _Tp_Deleter>& __x,
	 unique_ptr<_Tp, _Tp_Deleter>& __y)
    { __x.swap(__y); }

  template<typename _Tp, typename _Tp_Deleter,
	   typename _Up, typename _Up_Deleter>
    inline bool
    operator==(const unique_ptr<_Tp, _Tp_Deleter>& __x,
	       const unique_ptr<_Up, _Up_Deleter>& __y)
    { return __x.get() == __y.get(); }

  template<typename _Tp, typename _Tp_Deleter,
	   typename _Up, typename _Up_Deleter>
    inline bool
    operator!=(const unique_ptr<_Tp, _Tp_Deleter>& __x,
	       const unique_ptr<_Up, _Up_Deleter>& __y)
    { return !(__x.get() == __y.get()); }

  template<typename _Tp, typename _Tp_Deleter,
	   typename _Up, typename _Up_Deleter>
    inline bool
    operator<(const unique_ptr<_Tp, _Tp_Deleter>& __x,
	      const unique_ptr<_Up, _Up_Deleter>& __y)
    { return __x.get() < __y.get(); }

  template<typename _Tp, typename _Tp_Deleter,
	   typename _Up, typename _Up_Deleter>
    inline bool
    operator<=(const unique_ptr<_Tp, _Tp_Deleter>& __x,
	       const unique_ptr<_Up, _Up_Deleter>& __y)
    { return !(__y.get() < __x.get()); }

  template<typename _Tp, typename _Tp_Deleter,
	   typename _Up, typename _Up_Deleter>
    inline bool
    operator>(const unique_ptr<_Tp, _Tp_Deleter>& __x,
	      const unique_ptr<_Up, _Up_Deleter>& __y)
    { return __y.get() < __x.get(); }

  template<typename _Tp, typename _Tp_Deleter,
	   typename _Up, typename _Up_Deleter>
    inline bool
    operator>=(const unique_ptr<_Tp, _Tp_Deleter>& __x,
	       const unique_ptr<_Up, _Up_Deleter>& __y)
    { return !(__x.get() < __y.get()); }

  // @} group pointer_abstractions

_GLIBCXX_END_NAMESPACE

#endif /* _UNIQUE_PTR_H */
