// Vector implementation -*- C++ -*-

// Copyright (C) 2001-2013 Free Software Foundation, Inc.
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

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this  software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file bits/stl_vector.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{vector}
 */

#ifndef _STL_VECTOR_H
#define _STL_VECTOR_H 1

#include <bits/stl_iterator_base_funcs.h>
#include <bits/functexcept.h>
#include <bits/concept_check.h>
#if __cplusplus >= 201103L
#include <initializer_list>
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_CONTAINER

  /// See bits/stl_deque.h's _Deque_base for an explanation.
  template<typename _Tp, typename _Alloc>
    struct _Vector_base
    {
      typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template
        rebind<_Tp>::other _Tp_alloc_type;
      typedef typename __gnu_cxx::__alloc_traits<_Tp_alloc_type>::pointer
       	pointer;

      struct _Vector_impl 
      : public _Tp_alloc_type
      {
	pointer _M_start;
	pointer _M_finish;
	pointer _M_end_of_storage;

	_Vector_impl()
	: _Tp_alloc_type(), _M_start(0), _M_finish(0), _M_end_of_storage(0)
	{ }

	_Vector_impl(_Tp_alloc_type const& __a)
	: _Tp_alloc_type(__a), _M_start(0), _M_finish(0), _M_end_of_storage(0)
	{ }

#if __cplusplus >= 201103L
	_Vector_impl(_Tp_alloc_type&& __a)
	: _Tp_alloc_type(std::move(__a)),
	  _M_start(0), _M_finish(0), _M_end_of_storage(0)
	{ }
#endif

	void _M_swap_data(_Vector_impl& __x)
	{
	  std::swap(_M_start, __x._M_start);
	  std::swap(_M_finish, __x._M_finish);
	  std::swap(_M_end_of_storage, __x._M_end_of_storage);
	}
      };
      
    public:
      typedef _Alloc allocator_type;

      _Tp_alloc_type&
      _M_get_Tp_allocator() _GLIBCXX_NOEXCEPT
      { return *static_cast<_Tp_alloc_type*>(&this->_M_impl); }

      const _Tp_alloc_type&
      _M_get_Tp_allocator() const _GLIBCXX_NOEXCEPT
      { return *static_cast<const _Tp_alloc_type*>(&this->_M_impl); }

      allocator_type
      get_allocator() const _GLIBCXX_NOEXCEPT
      { return allocator_type(_M_get_Tp_allocator()); }

      _Vector_base()
      : _M_impl() { }

      _Vector_base(const allocator_type& __a)
      : _M_impl(__a) { }

      _Vector_base(size_t __n)
      : _M_impl()
      { _M_create_storage(__n); }

      _Vector_base(size_t __n, const allocator_type& __a)
      : _M_impl(__a)
      { _M_create_storage(__n); }

#if __cplusplus >= 201103L
      _Vector_base(_Tp_alloc_type&& __a)
      : _M_impl(std::move(__a)) { }

      _Vector_base(_Vector_base&& __x)
      : _M_impl(std::move(__x._M_get_Tp_allocator()))
      { this->_M_impl._M_swap_data(__x._M_impl); }

      _Vector_base(_Vector_base&& __x, const allocator_type& __a)
      : _M_impl(__a)
      {
	if (__x.get_allocator() == __a)
	  this->_M_impl._M_swap_data(__x._M_impl);
	else
	  {
	    size_t __n = __x._M_impl._M_finish - __x._M_impl._M_start;
	    _M_create_storage(__n);
	  }
      }
#endif

      ~_Vector_base()
      { _M_deallocate(this->_M_impl._M_start, this->_M_impl._M_end_of_storage
		      - this->_M_impl._M_start); }

    public:
      _Vector_impl _M_impl;

      pointer
      _M_allocate(size_t __n)
      { return __n != 0 ? _M_impl.allocate(__n) : 0; }

      void
      _M_deallocate(pointer __p, size_t __n)
      {
	if (__p)
	  _M_impl.deallocate(__p, __n);
      }

    private:
      void
      _M_create_storage(size_t __n)
      {
	this->_M_impl._M_start = this->_M_allocate(__n);
	this->_M_impl._M_finish = this->_M_impl._M_start;
	this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
      }
    };


  /**
   *  @brief A standard container which offers fixed time access to
   *  individual elements in any order.
   *
   *  @ingroup sequences
   *
   *  @tparam _Tp  Type of element.
   *  @tparam _Alloc  Allocator type, defaults to allocator<_Tp>.
   *
   *  Meets the requirements of a <a href="tables.html#65">container</a>, a
   *  <a href="tables.html#66">reversible container</a>, and a
   *  <a href="tables.html#67">sequence</a>, including the
   *  <a href="tables.html#68">optional sequence requirements</a> with the
   *  %exception of @c push_front and @c pop_front.
   *
   *  In some terminology a %vector can be described as a dynamic
   *  C-style array, it offers fast and efficient access to individual
   *  elements in any order and saves the user from worrying about
   *  memory and size allocation.  Subscripting ( @c [] ) access is
   *  also provided as with C-style arrays.
  */
  template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
    class vector : protected _Vector_base<_Tp, _Alloc>
    {
      // Concept requirements.
      typedef typename _Alloc::value_type                _Alloc_value_type;
      __glibcxx_class_requires(_Tp, _SGIAssignableConcept)
      __glibcxx_class_requires2(_Tp, _Alloc_value_type, _SameTypeConcept)
      
      typedef _Vector_base<_Tp, _Alloc>			 _Base;
      typedef typename _Base::_Tp_alloc_type		 _Tp_alloc_type;
      typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type>  _Alloc_traits;

    public:
      typedef _Tp					 value_type;
      typedef typename _Base::pointer                    pointer;
      typedef typename _Alloc_traits::const_pointer      const_pointer;
      typedef typename _Alloc_traits::reference          reference;
      typedef typename _Alloc_traits::const_reference    const_reference;
      typedef __gnu_cxx::__normal_iterator<pointer, vector> iterator;
      typedef __gnu_cxx::__normal_iterator<const_pointer, vector>
      const_iterator;
      typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;
      typedef std::reverse_iterator<iterator>		 reverse_iterator;
      typedef size_t					 size_type;
      typedef ptrdiff_t					 difference_type;
      typedef _Alloc                        		 allocator_type;

    protected:
      using _Base::_M_allocate;
      using _Base::_M_deallocate;
      using _Base::_M_impl;
      using _Base::_M_get_Tp_allocator;

    public:
      // [23.2.4.1] construct/copy/destroy
      // (assign() and get_allocator() are also listed in this section)
      /**
       *  @brief  Default constructor creates no elements.
       */
      vector()
      : _Base() { }

      /**
       *  @brief  Creates a %vector with no elements.
       *  @param  __a  An allocator object.
       */
      explicit
      vector(const allocator_type& __a)
      : _Base(__a) { }

#if __cplusplus >= 201103L
      /**
       *  @brief  Creates a %vector with default constructed elements.
       *  @param  __n  The number of elements to initially create.
       *  @param  __a  An allocator.
       *
       *  This constructor fills the %vector with @a __n default
       *  constructed elements.
       */
      explicit
      vector(size_type __n, const allocator_type& __a = allocator_type())
      : _Base(__n, __a)
      { _M_default_initialize(__n); }

      /**
       *  @brief  Creates a %vector with copies of an exemplar element.
       *  @param  __n  The number of elements to initially create.
       *  @param  __value  An element to copy.
       *  @param  __a  An allocator.
       *
       *  This constructor fills the %vector with @a __n copies of @a __value.
       */
      vector(size_type __n, const value_type& __value,
	     const allocator_type& __a = allocator_type())
      : _Base(__n, __a)
      { _M_fill_initialize(__n, __value); }
#else
      /**
       *  @brief  Creates a %vector with copies of an exemplar element.
       *  @param  __n  The number of elements to initially create.
       *  @param  __value  An element to copy.
       *  @param  __a  An allocator.
       *
       *  This constructor fills the %vector with @a __n copies of @a __value.
       */
      explicit
      vector(size_type __n, const value_type& __value = value_type(),
	     const allocator_type& __a = allocator_type())
      : _Base(__n, __a)
      { _M_fill_initialize(__n, __value); }
#endif

      /**
       *  @brief  %Vector copy constructor.
       *  @param  __x  A %vector of identical element and allocator types.
       *
       *  The newly-created %vector uses a copy of the allocation
       *  object used by @a __x.  All the elements of @a __x are copied,
       *  but any extra memory in
       *  @a __x (for fast expansion) will not be copied.
       */
      vector(const vector& __x)
      : _Base(__x.size(),
        _Alloc_traits::_S_select_on_copy(__x._M_get_Tp_allocator()))
      { this->_M_impl._M_finish =
	  std::__uninitialized_copy_a(__x.begin(), __x.end(),
				      this->_M_impl._M_start,
				      _M_get_Tp_allocator());
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  %Vector move constructor.
       *  @param  __x  A %vector of identical element and allocator types.
       *
       *  The newly-created %vector contains the exact contents of @a __x.
       *  The contents of @a __x are a valid, but unspecified %vector.
       */
      vector(vector&& __x) noexcept
      : _Base(std::move(__x)) { }

      /// Copy constructor with alternative allocator
      vector(const vector& __x, const allocator_type& __a)
      : _Base(__x.size(), __a)
      { this->_M_impl._M_finish =
	  std::__uninitialized_copy_a(__x.begin(), __x.end(),
				      this->_M_impl._M_start,
				      _M_get_Tp_allocator());
      }

      /// Move constructor with alternative allocator
      vector(vector&& __rv, const allocator_type& __m)
      : _Base(std::move(__rv), __m)
      {
	if (__rv.get_allocator() != __m)
	  {
	    this->_M_impl._M_finish =
	      std::__uninitialized_move_a(__rv.begin(), __rv.end(),
					  this->_M_impl._M_start,
					  _M_get_Tp_allocator());
	    __rv.clear();
	  }
      }

      /**
       *  @brief  Builds a %vector from an initializer list.
       *  @param  __l  An initializer_list.
       *  @param  __a  An allocator.
       *
       *  Create a %vector consisting of copies of the elements in the
       *  initializer_list @a __l.
       *
       *  This will call the element type's copy constructor N times
       *  (where N is @a __l.size()) and do no memory reallocation.
       */
      vector(initializer_list<value_type> __l,
	     const allocator_type& __a = allocator_type())
      : _Base(__a)
      {
	_M_range_initialize(__l.begin(), __l.end(),
			    random_access_iterator_tag());
      }
#endif

      /**
       *  @brief  Builds a %vector from a range.
       *  @param  __first  An input iterator.
       *  @param  __last  An input iterator.
       *  @param  __a  An allocator.
       *
       *  Create a %vector consisting of copies of the elements from
       *  [first,last).
       *
       *  If the iterators are forward, bidirectional, or
       *  random-access, then this will call the elements' copy
       *  constructor N times (where N is distance(first,last)) and do
       *  no memory reallocation.  But if only input iterators are
       *  used, then this will do at most 2N calls to the copy
       *  constructor, and logN memory reallocations.
       */
#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
        vector(_InputIterator __first, _InputIterator __last,
	       const allocator_type& __a = allocator_type())
	: _Base(__a)
        { _M_initialize_dispatch(__first, __last, __false_type()); }
#else
      template<typename _InputIterator>
        vector(_InputIterator __first, _InputIterator __last,
	       const allocator_type& __a = allocator_type())
	: _Base(__a)
        {
	  // Check whether it's an integral type.  If so, it's not an iterator.
	  typedef typename std::__is_integer<_InputIterator>::__type _Integral;
	  _M_initialize_dispatch(__first, __last, _Integral());
	}
#endif

      /**
       *  The dtor only erases the elements, and note that if the
       *  elements themselves are pointers, the pointed-to memory is
       *  not touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      ~vector() _GLIBCXX_NOEXCEPT
      { std::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
		      _M_get_Tp_allocator()); }

      /**
       *  @brief  %Vector assignment operator.
       *  @param  __x  A %vector of identical element and allocator types.
       *
       *  All the elements of @a __x are copied, but any extra memory in
       *  @a __x (for fast expansion) will not be copied.  Unlike the
       *  copy constructor, the allocator object is not copied.
       */
      vector&
      operator=(const vector& __x);

#if __cplusplus >= 201103L
      /**
       *  @brief  %Vector move assignment operator.
       *  @param  __x  A %vector of identical element and allocator types.
       *
       *  The contents of @a __x are moved into this %vector (without copying,
       *  if the allocators permit it).
       *  @a __x is a valid, but unspecified %vector.
       */
      vector&
      operator=(vector&& __x) noexcept(_Alloc_traits::_S_nothrow_move())
      {
        constexpr bool __move_storage =
          _Alloc_traits::_S_propagate_on_move_assign()
          || _Alloc_traits::_S_always_equal();
        _M_move_assign(std::move(__x),
                       integral_constant<bool, __move_storage>());
	return *this;
      }

      /**
       *  @brief  %Vector list assignment operator.
       *  @param  __l  An initializer_list.
       *
       *  This function fills a %vector with copies of the elements in the
       *  initializer list @a __l.
       *
       *  Note that the assignment completely changes the %vector and
       *  that the resulting %vector's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
      vector&
      operator=(initializer_list<value_type> __l)
      {
	this->assign(__l.begin(), __l.end());
	return *this;
      }
#endif

      /**
       *  @brief  Assigns a given value to a %vector.
       *  @param  __n  Number of elements to be assigned.
       *  @param  __val  Value to be assigned.
       *
       *  This function fills a %vector with @a __n copies of the given
       *  value.  Note that the assignment completely changes the
       *  %vector and that the resulting %vector's size is the same as
       *  the number of elements assigned.  Old data may be lost.
       */
      void
      assign(size_type __n, const value_type& __val)
      { _M_fill_assign(__n, __val); }

      /**
       *  @brief  Assigns a range to a %vector.
       *  @param  __first  An input iterator.
       *  @param  __last   An input iterator.
       *
       *  This function fills a %vector with copies of the elements in the
       *  range [__first,__last).
       *
       *  Note that the assignment completely changes the %vector and
       *  that the resulting %vector's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
        void
        assign(_InputIterator __first, _InputIterator __last)
        { _M_assign_dispatch(__first, __last, __false_type()); }
#else
      template<typename _InputIterator>
        void
        assign(_InputIterator __first, _InputIterator __last)
        {
	  // Check whether it's an integral type.  If so, it's not an iterator.
	  typedef typename std::__is_integer<_InputIterator>::__type _Integral;
	  _M_assign_dispatch(__first, __last, _Integral());
	}
#endif

#if __cplusplus >= 201103L
      /**
       *  @brief  Assigns an initializer list to a %vector.
       *  @param  __l  An initializer_list.
       *
       *  This function fills a %vector with copies of the elements in the
       *  initializer list @a __l.
       *
       *  Note that the assignment completely changes the %vector and
       *  that the resulting %vector's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
      void
      assign(initializer_list<value_type> __l)
      { this->assign(__l.begin(), __l.end()); }
#endif

      /// Get a copy of the memory allocation object.
      using _Base::get_allocator;

      // iterators
      /**
       *  Returns a read/write iterator that points to the first
       *  element in the %vector.  Iteration is done in ordinary
       *  element order.
       */
      iterator
      begin() _GLIBCXX_NOEXCEPT
      { return iterator(this->_M_impl._M_start); }

      /**
       *  Returns a read-only (constant) iterator that points to the
       *  first element in the %vector.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      begin() const _GLIBCXX_NOEXCEPT
      { return const_iterator(this->_M_impl._M_start); }

      /**
       *  Returns a read/write iterator that points one past the last
       *  element in the %vector.  Iteration is done in ordinary
       *  element order.
       */
      iterator
      end() _GLIBCXX_NOEXCEPT
      { return iterator(this->_M_impl._M_finish); }

      /**
       *  Returns a read-only (constant) iterator that points one past
       *  the last element in the %vector.  Iteration is done in
       *  ordinary element order.
       */
      const_iterator
      end() const _GLIBCXX_NOEXCEPT
      { return const_iterator(this->_M_impl._M_finish); }

      /**
       *  Returns a read/write reverse iterator that points to the
       *  last element in the %vector.  Iteration is done in reverse
       *  element order.
       */
      reverse_iterator
      rbegin() _GLIBCXX_NOEXCEPT
      { return reverse_iterator(end()); }

      /**
       *  Returns a read-only (constant) reverse iterator that points
       *  to the last element in the %vector.  Iteration is done in
       *  reverse element order.
       */
      const_reverse_iterator
      rbegin() const _GLIBCXX_NOEXCEPT
      { return const_reverse_iterator(end()); }

      /**
       *  Returns a read/write reverse iterator that points to one
       *  before the first element in the %vector.  Iteration is done
       *  in reverse element order.
       */
      reverse_iterator
      rend() _GLIBCXX_NOEXCEPT
      { return reverse_iterator(begin()); }

      /**
       *  Returns a read-only (constant) reverse iterator that points
       *  to one before the first element in the %vector.  Iteration
       *  is done in reverse element order.
       */
      const_reverse_iterator
      rend() const _GLIBCXX_NOEXCEPT
      { return const_reverse_iterator(begin()); }

#if __cplusplus >= 201103L
      /**
       *  Returns a read-only (constant) iterator that points to the
       *  first element in the %vector.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      cbegin() const noexcept
      { return const_iterator(this->_M_impl._M_start); }

      /**
       *  Returns a read-only (constant) iterator that points one past
       *  the last element in the %vector.  Iteration is done in
       *  ordinary element order.
       */
      const_iterator
      cend() const noexcept
      { return const_iterator(this->_M_impl._M_finish); }

      /**
       *  Returns a read-only (constant) reverse iterator that points
       *  to the last element in the %vector.  Iteration is done in
       *  reverse element order.
       */
      const_reverse_iterator
      crbegin() const noexcept
      { return const_reverse_iterator(end()); }

      /**
       *  Returns a read-only (constant) reverse iterator that points
       *  to one before the first element in the %vector.  Iteration
       *  is done in reverse element order.
       */
      const_reverse_iterator
      crend() const noexcept
      { return const_reverse_iterator(begin()); }
#endif

      // [23.2.4.2] capacity
      /**  Returns the number of elements in the %vector.  */
      size_type
      size() const _GLIBCXX_NOEXCEPT
      { return size_type(this->_M_impl._M_finish - this->_M_impl._M_start); }

      /**  Returns the size() of the largest possible %vector.  */
      size_type
      max_size() const _GLIBCXX_NOEXCEPT
      { return _Alloc_traits::max_size(_M_get_Tp_allocator()); }

#if __cplusplus >= 201103L
      /**
       *  @brief  Resizes the %vector to the specified number of elements.
       *  @param  __new_size  Number of elements the %vector should contain.
       *
       *  This function will %resize the %vector to the specified
       *  number of elements.  If the number is smaller than the
       *  %vector's current size the %vector is truncated, otherwise
       *  default constructed elements are appended.
       */
      void
      resize(size_type __new_size)
      {
	if (__new_size > size())
	  _M_default_append(__new_size - size());
	else if (__new_size < size())
	  _M_erase_at_end(this->_M_impl._M_start + __new_size);
      }

      /**
       *  @brief  Resizes the %vector to the specified number of elements.
       *  @param  __new_size  Number of elements the %vector should contain.
       *  @param  __x  Data with which new elements should be populated.
       *
       *  This function will %resize the %vector to the specified
       *  number of elements.  If the number is smaller than the
       *  %vector's current size the %vector is truncated, otherwise
       *  the %vector is extended and new elements are populated with
       *  given data.
       */
      void
      resize(size_type __new_size, const value_type& __x)
      {
	if (__new_size > size())
	  insert(end(), __new_size - size(), __x);
	else if (__new_size < size())
	  _M_erase_at_end(this->_M_impl._M_start + __new_size);
      }
#else
      /**
       *  @brief  Resizes the %vector to the specified number of elements.
       *  @param  __new_size  Number of elements the %vector should contain.
       *  @param  __x  Data with which new elements should be populated.
       *
       *  This function will %resize the %vector to the specified
       *  number of elements.  If the number is smaller than the
       *  %vector's current size the %vector is truncated, otherwise
       *  the %vector is extended and new elements are populated with
       *  given data.
       */
      void
      resize(size_type __new_size, value_type __x = value_type())
      {
	if (__new_size > size())
	  insert(end(), __new_size - size(), __x);
	else if (__new_size < size())
	  _M_erase_at_end(this->_M_impl._M_start + __new_size);
      }
#endif

#if __cplusplus >= 201103L
      /**  A non-binding request to reduce capacity() to size().  */
      void
      shrink_to_fit()
      { _M_shrink_to_fit(); }
#endif

      /**
       *  Returns the total number of elements that the %vector can
       *  hold before needing to allocate more memory.
       */
      size_type
      capacity() const _GLIBCXX_NOEXCEPT
      { return size_type(this->_M_impl._M_end_of_storage
			 - this->_M_impl._M_start); }

      /**
       *  Returns true if the %vector is empty.  (Thus begin() would
       *  equal end().)
       */
      bool
      empty() const _GLIBCXX_NOEXCEPT
      { return begin() == end(); }

      /**
       *  @brief  Attempt to preallocate enough memory for specified number of
       *          elements.
       *  @param  __n  Number of elements required.
       *  @throw  std::length_error  If @a n exceeds @c max_size().
       *
       *  This function attempts to reserve enough memory for the
       *  %vector to hold the specified number of elements.  If the
       *  number requested is more than max_size(), length_error is
       *  thrown.
       *
       *  The advantage of this function is that if optimal code is a
       *  necessity and the user can determine the number of elements
       *  that will be required, the user can reserve the memory in
       *  %advance, and thus prevent a possible reallocation of memory
       *  and copying of %vector data.
       */
      void
      reserve(size_type __n);

      // element access
      /**
       *  @brief  Subscript access to the data contained in the %vector.
       *  @param __n The index of the element for which data should be
       *  accessed.
       *  @return  Read/write reference to data.
       *
       *  This operator allows for easy, array-style, data access.
       *  Note that data access with this operator is unchecked and
       *  out_of_range lookups are not defined. (For checked lookups
       *  see at().)
       */
      reference
      operator[](size_type __n)
      { return *(this->_M_impl._M_start + __n); }

      /**
       *  @brief  Subscript access to the data contained in the %vector.
       *  @param __n The index of the element for which data should be
       *  accessed.
       *  @return  Read-only (constant) reference to data.
       *
       *  This operator allows for easy, array-style, data access.
       *  Note that data access with this operator is unchecked and
       *  out_of_range lookups are not defined. (For checked lookups
       *  see at().)
       */
      const_reference
      operator[](size_type __n) const
      { return *(this->_M_impl._M_start + __n); }

    protected:
      /// Safety check used only from at().
      void
      _M_range_check(size_type __n) const
      {
	if (__n >= this->size())
	  __throw_out_of_range(__N("vector::_M_range_check"));
      }

    public:
      /**
       *  @brief  Provides access to the data contained in the %vector.
       *  @param __n The index of the element for which data should be
       *  accessed.
       *  @return  Read/write reference to data.
       *  @throw  std::out_of_range  If @a __n is an invalid index.
       *
       *  This function provides for safer data access.  The parameter
       *  is first checked that it is in the range of the vector.  The
       *  function throws out_of_range if the check fails.
       */
      reference
      at(size_type __n)
      {
	_M_range_check(__n);
	return (*this)[__n]; 
      }

      /**
       *  @brief  Provides access to the data contained in the %vector.
       *  @param __n The index of the element for which data should be
       *  accessed.
       *  @return  Read-only (constant) reference to data.
       *  @throw  std::out_of_range  If @a __n is an invalid index.
       *
       *  This function provides for safer data access.  The parameter
       *  is first checked that it is in the range of the vector.  The
       *  function throws out_of_range if the check fails.
       */
      const_reference
      at(size_type __n) const
      {
	_M_range_check(__n);
	return (*this)[__n];
      }

      /**
       *  Returns a read/write reference to the data at the first
       *  element of the %vector.
       */
      reference
      front()
      { return *begin(); }

      /**
       *  Returns a read-only (constant) reference to the data at the first
       *  element of the %vector.
       */
      const_reference
      front() const
      { return *begin(); }

      /**
       *  Returns a read/write reference to the data at the last
       *  element of the %vector.
       */
      reference
      back()
      { return *(end() - 1); }
      
      /**
       *  Returns a read-only (constant) reference to the data at the
       *  last element of the %vector.
       */
      const_reference
      back() const
      { return *(end() - 1); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 464. Suggestion for new member functions in standard containers.
      // data access
      /**
       *   Returns a pointer such that [data(), data() + size()) is a valid
       *   range.  For a non-empty %vector, data() == &front().
       */
#if __cplusplus >= 201103L
      _Tp*
#else
      pointer
#endif
      data() _GLIBCXX_NOEXCEPT
      { return std::__addressof(front()); }

#if __cplusplus >= 201103L
      const _Tp*
#else
      const_pointer
#endif
      data() const _GLIBCXX_NOEXCEPT
      { return std::__addressof(front()); }

      // [23.2.4.3] modifiers
      /**
       *  @brief  Add data to the end of the %vector.
       *  @param  __x  Data to be added.
       *
       *  This is a typical stack operation.  The function creates an
       *  element at the end of the %vector and assigns the given data
       *  to it.  Due to the nature of a %vector this operation can be
       *  done in constant time if the %vector has preallocated space
       *  available.
       */
      void
      push_back(const value_type& __x)
      {
	if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage)
	  {
	    _Alloc_traits::construct(this->_M_impl, this->_M_impl._M_finish,
	                             __x);
	    ++this->_M_impl._M_finish;
	  }
	else
#if __cplusplus >= 201103L
	  _M_emplace_back_aux(__x);
#else
	  _M_insert_aux(end(), __x);
#endif
      }

#if __cplusplus >= 201103L
      void
      push_back(value_type&& __x)
      { emplace_back(std::move(__x)); }

      template<typename... _Args>
        void
        emplace_back(_Args&&... __args);
#endif

      /**
       *  @brief  Removes last element.
       *
       *  This is a typical stack operation. It shrinks the %vector by one.
       *
       *  Note that no data is returned, and if the last element's
       *  data is needed, it should be retrieved before pop_back() is
       *  called.
       */
      void
      pop_back()
      {
	--this->_M_impl._M_finish;
	_Alloc_traits::destroy(this->_M_impl, this->_M_impl._M_finish);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Inserts an object in %vector before specified iterator.
       *  @param  __position  An iterator into the %vector.
       *  @param  __args  Arguments.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert an object of type T constructed
       *  with T(std::forward<Args>(args)...) before the specified location.
       *  Note that this kind of operation could be expensive for a %vector
       *  and if it is frequently used the user should consider using
       *  std::list.
       */
      template<typename... _Args>
        iterator
        emplace(iterator __position, _Args&&... __args);
#endif

      /**
       *  @brief  Inserts given value into %vector before specified iterator.
       *  @param  __position  An iterator into the %vector.
       *  @param  __x  Data to be inserted.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert a copy of the given value before
       *  the specified location.  Note that this kind of operation
       *  could be expensive for a %vector and if it is frequently
       *  used the user should consider using std::list.
       */
      iterator
      insert(iterator __position, const value_type& __x);

#if __cplusplus >= 201103L
      /**
       *  @brief  Inserts given rvalue into %vector before specified iterator.
       *  @param  __position  An iterator into the %vector.
       *  @param  __x  Data to be inserted.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert a copy of the given rvalue before
       *  the specified location.  Note that this kind of operation
       *  could be expensive for a %vector and if it is frequently
       *  used the user should consider using std::list.
       */
      iterator
      insert(iterator __position, value_type&& __x)
      { return emplace(__position, std::move(__x)); }

      /**
       *  @brief  Inserts an initializer_list into the %vector.
       *  @param  __position  An iterator into the %vector.
       *  @param  __l  An initializer_list.
       *
       *  This function will insert copies of the data in the 
       *  initializer_list @a l into the %vector before the location
       *  specified by @a position.
       *
       *  Note that this kind of operation could be expensive for a
       *  %vector and if it is frequently used the user should
       *  consider using std::list.
       */
      void
      insert(iterator __position, initializer_list<value_type> __l)
      { this->insert(__position, __l.begin(), __l.end()); }
#endif

      /**
       *  @brief  Inserts a number of copies of given data into the %vector.
       *  @param  __position  An iterator into the %vector.
       *  @param  __n  Number of elements to be inserted.
       *  @param  __x  Data to be inserted.
       *
       *  This function will insert a specified number of copies of
       *  the given data before the location specified by @a position.
       *
       *  Note that this kind of operation could be expensive for a
       *  %vector and if it is frequently used the user should
       *  consider using std::list.
       */
      void
      insert(iterator __position, size_type __n, const value_type& __x)
      { _M_fill_insert(__position, __n, __x); }

      /**
       *  @brief  Inserts a range into the %vector.
       *  @param  __position  An iterator into the %vector.
       *  @param  __first  An input iterator.
       *  @param  __last   An input iterator.
       *
       *  This function will insert copies of the data in the range
       *  [__first,__last) into the %vector before the location specified
       *  by @a pos.
       *
       *  Note that this kind of operation could be expensive for a
       *  %vector and if it is frequently used the user should
       *  consider using std::list.
       */
#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
        void
        insert(iterator __position, _InputIterator __first,
	       _InputIterator __last)
        { _M_insert_dispatch(__position, __first, __last, __false_type()); }
#else
      template<typename _InputIterator>
        void
        insert(iterator __position, _InputIterator __first,
	       _InputIterator __last)
        {
	  // Check whether it's an integral type.  If so, it's not an iterator.
	  typedef typename std::__is_integer<_InputIterator>::__type _Integral;
	  _M_insert_dispatch(__position, __first, __last, _Integral());
	}
#endif

      /**
       *  @brief  Remove element at given position.
       *  @param  __position  Iterator pointing to element to be erased.
       *  @return  An iterator pointing to the next element (or end()).
       *
       *  This function will erase the element at the given position and thus
       *  shorten the %vector by one.
       *
       *  Note This operation could be expensive and if it is
       *  frequently used the user should consider using std::list.
       *  The user is also cautioned that this function only erases
       *  the element, and that if the element is itself a pointer,
       *  the pointed-to memory is not touched in any way.  Managing
       *  the pointer is the user's responsibility.
       */
      iterator
      erase(iterator __position);

      /**
       *  @brief  Remove a range of elements.
       *  @param  __first  Iterator pointing to the first element to be erased.
       *  @param  __last  Iterator pointing to one past the last element to be
       *                  erased.
       *  @return  An iterator pointing to the element pointed to by @a __last
       *           prior to erasing (or end()).
       *
       *  This function will erase the elements in the range
       *  [__first,__last) and shorten the %vector accordingly.
       *
       *  Note This operation could be expensive and if it is
       *  frequently used the user should consider using std::list.
       *  The user is also cautioned that this function only erases
       *  the elements, and that if the elements themselves are
       *  pointers, the pointed-to memory is not touched in any way.
       *  Managing the pointer is the user's responsibility.
       */
      iterator
      erase(iterator __first, iterator __last);

      /**
       *  @brief  Swaps data with another %vector.
       *  @param  __x  A %vector of the same element and allocator types.
       *
       *  This exchanges the elements between two vectors in constant time.
       *  (Three pointers, so it should be quite fast.)
       *  Note that the global std::swap() function is specialized such that
       *  std::swap(v1,v2) will feed to this function.
       */
      void
      swap(vector& __x)
#if __cplusplus >= 201103L
			noexcept(_Alloc_traits::_S_nothrow_swap())
#endif
      {
	this->_M_impl._M_swap_data(__x._M_impl);
	_Alloc_traits::_S_on_swap(_M_get_Tp_allocator(),
	                          __x._M_get_Tp_allocator());
      }

      /**
       *  Erases all the elements.  Note that this function only erases the
       *  elements, and that if the elements themselves are pointers, the
       *  pointed-to memory is not touched in any way.  Managing the pointer is
       *  the user's responsibility.
       */
      void
      clear() _GLIBCXX_NOEXCEPT
      { _M_erase_at_end(this->_M_impl._M_start); }

    protected:
      /**
       *  Memory expansion handler.  Uses the member allocation function to
       *  obtain @a n bytes of memory, and then copies [first,last) into it.
       */
      template<typename _ForwardIterator>
        pointer
        _M_allocate_and_copy(size_type __n,
			     _ForwardIterator __first, _ForwardIterator __last)
        {
	  pointer __result = this->_M_allocate(__n);
	  __try
	    {
	      std::__uninitialized_copy_a(__first, __last, __result,
					  _M_get_Tp_allocator());
	      return __result;
	    }
	  __catch(...)
	    {
	      _M_deallocate(__result, __n);
	      __throw_exception_again;
	    }
	}


      // Internal constructor functions follow.

      // Called by the range constructor to implement [23.1.1]/9

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 438. Ambiguity in the "do the right thing" clause
      template<typename _Integer>
        void
        _M_initialize_dispatch(_Integer __n, _Integer __value, __true_type)
        {
	  this->_M_impl._M_start = _M_allocate(static_cast<size_type>(__n));
	  this->_M_impl._M_end_of_storage =
	    this->_M_impl._M_start + static_cast<size_type>(__n);
	  _M_fill_initialize(static_cast<size_type>(__n), __value);
	}

      // Called by the range constructor to implement [23.1.1]/9
      template<typename _InputIterator>
        void
        _M_initialize_dispatch(_InputIterator __first, _InputIterator __last,
			       __false_type)
        {
	  typedef typename std::iterator_traits<_InputIterator>::
	    iterator_category _IterCategory;
	  _M_range_initialize(__first, __last, _IterCategory());
	}

      // Called by the second initialize_dispatch above
      template<typename _InputIterator>
        void
        _M_range_initialize(_InputIterator __first,
			    _InputIterator __last, std::input_iterator_tag)
        {
	  for (; __first != __last; ++__first)
#if __cplusplus >= 201103L
	    emplace_back(*__first);
#else
	    push_back(*__first);
#endif
	}

      // Called by the second initialize_dispatch above
      template<typename _ForwardIterator>
        void
        _M_range_initialize(_ForwardIterator __first,
			    _ForwardIterator __last, std::forward_iterator_tag)
        {
	  const size_type __n = std::distance(__first, __last);
	  this->_M_impl._M_start = this->_M_allocate(__n);
	  this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
	  this->_M_impl._M_finish =
	    std::__uninitialized_copy_a(__first, __last,
					this->_M_impl._M_start,
					_M_get_Tp_allocator());
	}

      // Called by the first initialize_dispatch above and by the
      // vector(n,value,a) constructor.
      void
      _M_fill_initialize(size_type __n, const value_type& __value)
      {
	std::__uninitialized_fill_n_a(this->_M_impl._M_start, __n, __value, 
				      _M_get_Tp_allocator());
	this->_M_impl._M_finish = this->_M_impl._M_end_of_storage;
      }

#if __cplusplus >= 201103L
      // Called by the vector(n) constructor.
      void
      _M_default_initialize(size_type __n)
      {
	std::__uninitialized_default_n_a(this->_M_impl._M_start, __n, 
					 _M_get_Tp_allocator());
	this->_M_impl._M_finish = this->_M_impl._M_end_of_storage;
      }
#endif

      // Internal assign functions follow.  The *_aux functions do the actual
      // assignment work for the range versions.

      // Called by the range assign to implement [23.1.1]/9

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 438. Ambiguity in the "do the right thing" clause
      template<typename _Integer>
        void
        _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
        { _M_fill_assign(__n, __val); }

      // Called by the range assign to implement [23.1.1]/9
      template<typename _InputIterator>
        void
        _M_assign_dispatch(_InputIterator __first, _InputIterator __last,
			   __false_type)
        {
	  typedef typename std::iterator_traits<_InputIterator>::
	    iterator_category _IterCategory;
	  _M_assign_aux(__first, __last, _IterCategory());
	}

      // Called by the second assign_dispatch above
      template<typename _InputIterator>
        void
        _M_assign_aux(_InputIterator __first, _InputIterator __last,
		      std::input_iterator_tag);

      // Called by the second assign_dispatch above
      template<typename _ForwardIterator>
        void
        _M_assign_aux(_ForwardIterator __first, _ForwardIterator __last,
		      std::forward_iterator_tag);

      // Called by assign(n,t), and the range assign when it turns out
      // to be the same thing.
      void
      _M_fill_assign(size_type __n, const value_type& __val);


      // Internal insert functions follow.

      // Called by the range insert to implement [23.1.1]/9

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 438. Ambiguity in the "do the right thing" clause
      template<typename _Integer>
        void
        _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __val,
			   __true_type)
        { _M_fill_insert(__pos, __n, __val); }

      // Called by the range insert to implement [23.1.1]/9
      template<typename _InputIterator>
        void
        _M_insert_dispatch(iterator __pos, _InputIterator __first,
			   _InputIterator __last, __false_type)
        {
	  typedef typename std::iterator_traits<_InputIterator>::
	    iterator_category _IterCategory;
	  _M_range_insert(__pos, __first, __last, _IterCategory());
	}

      // Called by the second insert_dispatch above
      template<typename _InputIterator>
        void
        _M_range_insert(iterator __pos, _InputIterator __first,
			_InputIterator __last, std::input_iterator_tag);

      // Called by the second insert_dispatch above
      template<typename _ForwardIterator>
        void
        _M_range_insert(iterator __pos, _ForwardIterator __first,
			_ForwardIterator __last, std::forward_iterator_tag);

      // Called by insert(p,n,x), and the range insert when it turns out to be
      // the same thing.
      void
      _M_fill_insert(iterator __pos, size_type __n, const value_type& __x);

#if __cplusplus >= 201103L
      // Called by resize(n).
      void
      _M_default_append(size_type __n);

      bool
      _M_shrink_to_fit();
#endif

      // Called by insert(p,x)
#if __cplusplus < 201103L
      void
      _M_insert_aux(iterator __position, const value_type& __x);
#else
      template<typename... _Args>
        void
        _M_insert_aux(iterator __position, _Args&&... __args);

      template<typename... _Args>
        void
        _M_emplace_back_aux(_Args&&... __args);
#endif

      // Called by the latter.
      size_type
      _M_check_len(size_type __n, const char* __s) const
      {
	if (max_size() - size() < __n)
	  __throw_length_error(__N(__s));

	const size_type __len = size() + std::max(size(), __n);
	return (__len < size() || __len > max_size()) ? max_size() : __len;
      }

      // Internal erase functions follow.

      // Called by erase(q1,q2), clear(), resize(), _M_fill_assign,
      // _M_assign_aux.
      void
      _M_erase_at_end(pointer __pos)
      {
	std::_Destroy(__pos, this->_M_impl._M_finish, _M_get_Tp_allocator());
	this->_M_impl._M_finish = __pos;
      }

#if __cplusplus >= 201103L
    private:
      // Constant-time move assignment when source object's memory can be
      // moved, either because the source's allocator will move too
      // or because the allocators are equal.
      void
      _M_move_assign(vector&& __x, std::true_type) noexcept
      {
	const vector __tmp(std::move(*this));
	this->_M_impl._M_swap_data(__x._M_impl);
	if (_Alloc_traits::_S_propagate_on_move_assign())
	  std::__alloc_on_move(_M_get_Tp_allocator(),
			       __x._M_get_Tp_allocator());
      }

      // Do move assignment when it might not be possible to move source
      // object's memory, resulting in a linear-time operation.
      void
      _M_move_assign(vector&& __x, std::false_type)
      {
	if (__x._M_get_Tp_allocator() == this->_M_get_Tp_allocator())
	  _M_move_assign(std::move(__x), std::true_type());
	else
	  {
	    // The rvalue's allocator cannot be moved and is not equal,
	    // so we need to individually move each element.
	    this->assign(std::__make_move_if_noexcept_iterator(__x.begin()),
			 std::__make_move_if_noexcept_iterator(__x.end()));
	    __x.clear();
	  }
      }
#endif
    };


  /**
   *  @brief  Vector equality comparison.
   *  @param  __x  A %vector.
   *  @param  __y  A %vector of the same type as @a __x.
   *  @return  True iff the size and elements of the vectors are equal.
   *
   *  This is an equivalence relation.  It is linear in the size of the
   *  vectors.  Vectors are considered equivalent if their sizes are equal,
   *  and if corresponding elements compare equal.
  */
  template<typename _Tp, typename _Alloc>
    inline bool
    operator==(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
    { return (__x.size() == __y.size()
	      && std::equal(__x.begin(), __x.end(), __y.begin())); }

  /**
   *  @brief  Vector ordering relation.
   *  @param  __x  A %vector.
   *  @param  __y  A %vector of the same type as @a __x.
   *  @return  True iff @a __x is lexicographically less than @a __y.
   *
   *  This is a total ordering relation.  It is linear in the size of the
   *  vectors.  The elements must be comparable with @c <.
   *
   *  See std::lexicographical_compare() for how the determination is made.
  */
  template<typename _Tp, typename _Alloc>
    inline bool
    operator<(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
    { return std::lexicographical_compare(__x.begin(), __x.end(),
					  __y.begin(), __y.end()); }

  /// Based on operator==
  template<typename _Tp, typename _Alloc>
    inline bool
    operator!=(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
    { return !(__x == __y); }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator>(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
    { return __y < __x; }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator<=(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
    { return !(__y < __x); }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator>=(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
    { return !(__x < __y); }

  /// See std::vector::swap().
  template<typename _Tp, typename _Alloc>
    inline void
    swap(vector<_Tp, _Alloc>& __x, vector<_Tp, _Alloc>& __y)
    { __x.swap(__y); }

_GLIBCXX_END_NAMESPACE_CONTAINER
} // namespace std

#endif /* _STL_VECTOR_H */
