// unique_ptr implementation -*- C++ -*-

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

/** @file bits/unique_ptr.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{memory}
 */

#ifndef _UNIQUE_PTR_H
#define _UNIQUE_PTR_H 1

#include <bits/c++config.h>
#include <debug/debug.h>
#include <type_traits>
#include <utility>
#include <tuple>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @addtogroup pointer_abstractions
   * @{
   */

#if _GLIBCXX_USE_DEPRECATED
  template<typename> class auto_ptr;
#endif

  /// Primary template, default_delete.
  template<typename _Tp>
    struct default_delete
    {
      constexpr default_delete() noexcept = default;

      template<typename _Up, typename = typename
	       enable_if<is_convertible<_Up*, _Tp*>::value>::type>
        default_delete(const default_delete<_Up>&) noexcept { }

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
    private:
      template<typename _Up>
	using __remove_cv = typename remove_cv<_Up>::type;

      // Like is_base_of<_Tp, _Up> but false if unqualified types are the same
      template<typename _Up>
	using __is_derived_Tp
	  = __and_< is_base_of<_Tp, _Up>,
		    __not_<is_same<__remove_cv<_Tp>, __remove_cv<_Up>>> >;

    public:
      constexpr default_delete() noexcept = default;

      template<typename _Up, typename = typename
	       enable_if<!__is_derived_Tp<_Up>::value>::type>
        default_delete(const default_delete<_Up[]>&) noexcept { }

      void
      operator()(_Tp* __ptr) const
      {
	static_assert(sizeof(_Tp)>0,
		      "can't delete pointer to incomplete type");
	delete [] __ptr;
      }

      template<typename _Up>
	typename enable_if<__is_derived_Tp<_Up>::value>::type
	operator()(_Up*) const = delete;
    };

  /// 20.7.1.2 unique_ptr for single objects.
  template <typename _Tp, typename _Dp = default_delete<_Tp> >
    class unique_ptr
    {
      // use SFINAE to determine whether _Del::pointer exists
      class _Pointer
      {
	template<typename _Up>
	  static typename _Up::pointer __test(typename _Up::pointer*);

	template<typename _Up>
	  static _Tp* __test(...);

	typedef typename remove_reference<_Dp>::type _Del;

      public:
	typedef decltype(__test<_Del>(0)) type;
      };

      typedef std::tuple<typename _Pointer::type, _Dp>  __tuple_type;
      __tuple_type                                      _M_t;

    public:
      typedef typename _Pointer::type   pointer;
      typedef _Tp                       element_type;
      typedef _Dp                       deleter_type;

      // Constructors.
      constexpr unique_ptr() noexcept
      : _M_t()
      { static_assert(!is_pointer<deleter_type>::value,
		     "constructed with null function pointer deleter"); }

      explicit
      unique_ptr(pointer __p) noexcept
      : _M_t(__p, deleter_type())
      { static_assert(!is_pointer<deleter_type>::value,
		     "constructed with null function pointer deleter"); }

      unique_ptr(pointer __p,
	  typename conditional<is_reference<deleter_type>::value,
	    deleter_type, const deleter_type&>::type __d) noexcept
      : _M_t(__p, __d) { }

      unique_ptr(pointer __p,
	  typename remove_reference<deleter_type>::type&& __d) noexcept
      : _M_t(std::move(__p), std::move(__d))
      { static_assert(!std::is_reference<deleter_type>::value,
		      "rvalue deleter bound to reference"); }

      constexpr unique_ptr(nullptr_t) noexcept : unique_ptr() { }

      // Move constructors.
      unique_ptr(unique_ptr&& __u) noexcept
      : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }

      template<typename _Up, typename _Ep, typename = _Require<
	       is_convertible<typename unique_ptr<_Up, _Ep>::pointer, pointer>,
	       __not_<is_array<_Up>>,
	       typename conditional<is_reference<_Dp>::value,
				    is_same<_Ep, _Dp>,
				    is_convertible<_Ep, _Dp>>::type>>
	unique_ptr(unique_ptr<_Up, _Ep>&& __u) noexcept
	: _M_t(__u.release(), std::forward<_Ep>(__u.get_deleter()))
	{ }

#if _GLIBCXX_USE_DEPRECATED
      template<typename _Up, typename = _Require<
	       is_convertible<_Up*, _Tp*>, is_same<_Dp, default_delete<_Tp>>>>
	unique_ptr(auto_ptr<_Up>&& __u) noexcept;
#endif

      // Destructor.
      ~unique_ptr() noexcept
      {
	auto& __ptr = std::get<0>(_M_t);
	if (__ptr != nullptr)
	  get_deleter()(__ptr);
	__ptr = pointer();
      }

      // Assignment.
      unique_ptr&
      operator=(unique_ptr&& __u) noexcept
      {
	reset(__u.release());
	get_deleter() = std::forward<deleter_type>(__u.get_deleter());
	return *this;
      }

      template<typename _Up, typename _Ep>
	typename enable_if< __and_<
	  is_convertible<typename unique_ptr<_Up, _Ep>::pointer, pointer>,
	  __not_<is_array<_Up>>
	  >::value,
	  unique_ptr&>::type
	operator=(unique_ptr<_Up, _Ep>&& __u) noexcept
	{
	  reset(__u.release());
	  get_deleter() = std::forward<_Ep>(__u.get_deleter());
	  return *this;
	}

      unique_ptr&
      operator=(nullptr_t) noexcept
      {
	reset();
	return *this;
      }

      // Observers.
      typename add_lvalue_reference<element_type>::type
      operator*() const
      {
	_GLIBCXX_DEBUG_ASSERT(get() != pointer());
	return *get();
      }

      pointer
      operator->() const noexcept
      {
	_GLIBCXX_DEBUG_ASSERT(get() != pointer());
	return get();
      }

      pointer
      get() const noexcept
      { return std::get<0>(_M_t); }

      deleter_type&
      get_deleter() noexcept
      { return std::get<1>(_M_t); }

      const deleter_type&
      get_deleter() const noexcept
      { return std::get<1>(_M_t); }

      explicit operator bool() const noexcept
      { return get() == pointer() ? false : true; }

      // Modifiers.
      pointer
      release() noexcept
      {
	pointer __p = get();
	std::get<0>(_M_t) = pointer();
	return __p;
      }

      void
      reset(pointer __p = pointer()) noexcept
      {
	using std::swap;
	swap(std::get<0>(_M_t), __p);
	if (__p != pointer())
	  get_deleter()(__p);
      }

      void
      swap(unique_ptr& __u) noexcept
      {
	using std::swap;
	swap(_M_t, __u._M_t);
      }

      // Disable copy from lvalue.
      unique_ptr(const unique_ptr&) = delete;
      unique_ptr& operator=(const unique_ptr&) = delete;
  };

  /// 20.7.1.3 unique_ptr for array objects with a runtime length
  // [unique.ptr.runtime]
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // DR 740 - omit specialization for array objects with a compile time length
  template<typename _Tp, typename _Dp>
    class unique_ptr<_Tp[], _Dp>
    {
      // use SFINAE to determine whether _Del::pointer exists
      class _Pointer
      {
	template<typename _Up>
	  static typename _Up::pointer __test(typename _Up::pointer*);

	template<typename _Up>
	  static _Tp* __test(...);

	typedef typename remove_reference<_Dp>::type _Del;

      public:
	typedef decltype(__test<_Del>(0)) type;
      };

      typedef std::tuple<typename _Pointer::type, _Dp>  __tuple_type;
      __tuple_type                                      _M_t;

      template<typename _Up>
	using __remove_cv = typename remove_cv<_Up>::type;

      // like is_base_of<_Tp, _Up> but false if unqualified types are the same
      template<typename _Up>
	using __is_derived_Tp
	  = __and_< is_base_of<_Tp, _Up>,
		    __not_<is_same<__remove_cv<_Tp>, __remove_cv<_Up>>> >;

      template<typename _Up, typename _Ep,
	       typename _Tp_pointer = typename _Pointer::type,
	       typename _Up_pointer = typename unique_ptr<_Up, _Ep>::pointer>
	using __safe_conversion = __and_<
	    is_convertible<_Up_pointer, _Tp_pointer>,
	    is_array<_Up>,
	    __or_<__not_<is_pointer<_Up_pointer>>,
		  __not_<is_pointer<_Tp_pointer>>,
		  __not_<__is_derived_Tp<typename remove_extent<_Up>::type>>
	    >
	  >;

    public:
      typedef typename _Pointer::type	pointer;
      typedef _Tp		 	element_type;
      typedef _Dp                       deleter_type;

      // Constructors.
      constexpr unique_ptr() noexcept
      : _M_t()
      { static_assert(!std::is_pointer<deleter_type>::value,
		      "constructed with null function pointer deleter"); }

      explicit
      unique_ptr(pointer __p) noexcept
      : _M_t(__p, deleter_type())
      { static_assert(!is_pointer<deleter_type>::value,
		      "constructed with null function pointer deleter"); }

      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	explicit
	unique_ptr(_Up* __p) = delete;

      unique_ptr(pointer __p,
	  typename conditional<is_reference<deleter_type>::value,
	      deleter_type, const deleter_type&>::type __d) noexcept
      : _M_t(__p, __d) { }

      unique_ptr(pointer __p, typename
		 remove_reference<deleter_type>::type&& __d) noexcept
      : _M_t(std::move(__p), std::move(__d))
      { static_assert(!is_reference<deleter_type>::value,
		      "rvalue deleter bound to reference"); }

      // Move constructor.
      unique_ptr(unique_ptr&& __u) noexcept
      : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }

      constexpr unique_ptr(nullptr_t) noexcept : unique_ptr() { }

      template<typename _Up, typename _Ep,
	       typename = _Require<__safe_conversion<_Up, _Ep>,
		 typename conditional<is_reference<_Dp>::value,
				      is_same<_Ep, _Dp>,
				      is_convertible<_Ep, _Dp>>::type
	       >>
	unique_ptr(unique_ptr<_Up, _Ep>&& __u) noexcept
	: _M_t(__u.release(), std::forward<_Ep>(__u.get_deleter()))
	{ }

      // Destructor.
      ~unique_ptr()
      {
	auto& __ptr = std::get<0>(_M_t);
	if (__ptr != nullptr)
	  get_deleter()(__ptr);
	__ptr = pointer();
      }

      // Assignment.
      unique_ptr&
      operator=(unique_ptr&& __u) noexcept
      {
	reset(__u.release());
	get_deleter() = std::forward<deleter_type>(__u.get_deleter());
	return *this;
      }

      template<typename _Up, typename _Ep>
	typename
	enable_if<__safe_conversion<_Up, _Ep>::value, unique_ptr&>::type
	operator=(unique_ptr<_Up, _Ep>&& __u) noexcept
	{
	  reset(__u.release());
	  get_deleter() = std::forward<_Ep>(__u.get_deleter());
	  return *this;
	}

      unique_ptr&
      operator=(nullptr_t) noexcept
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
      get() const noexcept
      { return std::get<0>(_M_t); }

      deleter_type&
      get_deleter() noexcept
      { return std::get<1>(_M_t); }

      const deleter_type&
      get_deleter() const noexcept
      { return std::get<1>(_M_t); }

      explicit operator bool() const noexcept
      { return get() == pointer() ? false : true; }

      // Modifiers.
      pointer
      release() noexcept
      {
	pointer __p = get();
	std::get<0>(_M_t) = pointer();
	return __p;
      }

      void
      reset() noexcept
      { reset(pointer()); }

      void
      reset(pointer __p) noexcept
      {
	using std::swap;
	swap(std::get<0>(_M_t), __p);
	if (__p != nullptr)
	  get_deleter()(__p);
      }

      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	void reset(_Up*) = delete;

      void
      swap(unique_ptr& __u) noexcept
      {
	using std::swap;
	swap(_M_t, __u._M_t);
      }

      // Disable copy from lvalue.
      unique_ptr(const unique_ptr&) = delete;
      unique_ptr& operator=(const unique_ptr&) = delete;

      // Disable construction from convertible pointer types.
      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	unique_ptr(_Up*, typename
		   conditional<is_reference<deleter_type>::value,
		   deleter_type, const deleter_type&>::type) = delete;

      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	unique_ptr(_Up*, typename
		   remove_reference<deleter_type>::type&&) = delete;
    };

  template<typename _Tp, typename _Dp>
    inline void
    swap(unique_ptr<_Tp, _Dp>& __x,
	 unique_ptr<_Tp, _Dp>& __y) noexcept
    { __x.swap(__y); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator==(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return __x.get() == __y.get(); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator==(const unique_ptr<_Tp, _Dp>& __x, nullptr_t) noexcept
    { return !__x; }

  template<typename _Tp, typename _Dp>
    inline bool
    operator==(nullptr_t, const unique_ptr<_Tp, _Dp>& __x) noexcept
    { return !__x; }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator!=(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return __x.get() != __y.get(); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator!=(const unique_ptr<_Tp, _Dp>& __x, nullptr_t) noexcept
    { return (bool)__x; }

  template<typename _Tp, typename _Dp>
    inline bool
    operator!=(nullptr_t, const unique_ptr<_Tp, _Dp>& __x) noexcept
    { return (bool)__x; }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator<(const unique_ptr<_Tp, _Dp>& __x,
	      const unique_ptr<_Up, _Ep>& __y)
    {
      typedef typename
	std::common_type<typename unique_ptr<_Tp, _Dp>::pointer,
	                 typename unique_ptr<_Up, _Ep>::pointer>::type _CT;
      return std::less<_CT>()(__x.get(), __y.get());
    }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(__x.get(),
								 nullptr); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(nullptr,
								 __x.get()); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator<=(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return !(__y < __x); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<=(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return !(nullptr < __x); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<=(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return !(__x < nullptr); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator>(const unique_ptr<_Tp, _Dp>& __x,
	      const unique_ptr<_Up, _Ep>& __y)
    { return (__y < __x); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(nullptr,
								 __x.get()); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(__x.get(),
								 nullptr); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator>=(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return !(__x < __y); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>=(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return !(__x < nullptr); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>=(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return !(nullptr < __x); }

  /// std::hash specialization for unique_ptr.
  template<typename _Tp, typename _Dp>
    struct hash<unique_ptr<_Tp, _Dp>>
    : public __hash_base<size_t, unique_ptr<_Tp, _Dp>>
    {
      size_t
      operator()(const unique_ptr<_Tp, _Dp>& __u) const noexcept
      {
	typedef unique_ptr<_Tp, _Dp> _UP;
	return std::hash<typename _UP::pointer>()(__u.get());
      }
    };

  // @} group pointer_abstractions

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif /* _UNIQUE_PTR_H */
