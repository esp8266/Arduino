// List implementation -*- C++ -*-

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
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file bits/stl_list.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{list}
 */

#ifndef _STL_LIST_H
#define _STL_LIST_H 1

#include <bits/concept_check.h>
#if __cplusplus >= 201103L
#include <initializer_list>
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
  namespace __detail
  {
  _GLIBCXX_BEGIN_NAMESPACE_VERSION

    // Supporting structures are split into common and templated
    // types; the latter publicly inherits from the former in an
    // effort to reduce code duplication.  This results in some
    // "needless" static_cast'ing later on, but it's all safe
    // downcasting.

    /// Common part of a node in the %list. 
    struct _List_node_base
    {
      _List_node_base* _M_next;
      _List_node_base* _M_prev;

      static void
      swap(_List_node_base& __x, _List_node_base& __y) _GLIBCXX_USE_NOEXCEPT;

      void
      _M_transfer(_List_node_base* const __first,
		  _List_node_base* const __last) _GLIBCXX_USE_NOEXCEPT;

      void
      _M_reverse() _GLIBCXX_USE_NOEXCEPT;

      void
      _M_hook(_List_node_base* const __position) _GLIBCXX_USE_NOEXCEPT;

      void
      _M_unhook() _GLIBCXX_USE_NOEXCEPT;
    };

  _GLIBCXX_END_NAMESPACE_VERSION
  } // namespace detail

_GLIBCXX_BEGIN_NAMESPACE_CONTAINER

  /// An actual node in the %list.
  template<typename _Tp>
    struct _List_node : public __detail::_List_node_base
    {
      ///< User's data.
      _Tp _M_data;

#if __cplusplus >= 201103L
      template<typename... _Args>
        _List_node(_Args&&... __args)
	: __detail::_List_node_base(), _M_data(std::forward<_Args>(__args)...) 
        { }
#endif
    };

  /**
   *  @brief A list::iterator.
   *
   *  All the functions are op overloads.
  */
  template<typename _Tp>
    struct _List_iterator
    {
      typedef _List_iterator<_Tp>                _Self;
      typedef _List_node<_Tp>                    _Node;

      typedef ptrdiff_t                          difference_type;
      typedef std::bidirectional_iterator_tag    iterator_category;
      typedef _Tp                                value_type;
      typedef _Tp*                               pointer;
      typedef _Tp&                               reference;

      _List_iterator()
      : _M_node() { }

      explicit
      _List_iterator(__detail::_List_node_base* __x)
      : _M_node(__x) { }

      // Must downcast from _List_node_base to _List_node to get to _M_data.
      reference
      operator*() const
      { return static_cast<_Node*>(_M_node)->_M_data; }

      pointer
      operator->() const
      { return std::__addressof(static_cast<_Node*>(_M_node)->_M_data); }

      _Self&
      operator++()
      {
	_M_node = _M_node->_M_next;
	return *this;
      }

      _Self
      operator++(int)
      {
	_Self __tmp = *this;
	_M_node = _M_node->_M_next;
	return __tmp;
      }

      _Self&
      operator--()
      {
	_M_node = _M_node->_M_prev;
	return *this;
      }

      _Self
      operator--(int)
      {
	_Self __tmp = *this;
	_M_node = _M_node->_M_prev;
	return __tmp;
      }

      bool
      operator==(const _Self& __x) const
      { return _M_node == __x._M_node; }

      bool
      operator!=(const _Self& __x) const
      { return _M_node != __x._M_node; }

      // The only member points to the %list element.
      __detail::_List_node_base* _M_node;
    };

  /**
   *  @brief A list::const_iterator.
   *
   *  All the functions are op overloads.
  */
  template<typename _Tp>
    struct _List_const_iterator
    {
      typedef _List_const_iterator<_Tp>          _Self;
      typedef const _List_node<_Tp>              _Node;
      typedef _List_iterator<_Tp>                iterator;

      typedef ptrdiff_t                          difference_type;
      typedef std::bidirectional_iterator_tag    iterator_category;
      typedef _Tp                                value_type;
      typedef const _Tp*                         pointer;
      typedef const _Tp&                         reference;

      _List_const_iterator()
      : _M_node() { }

      explicit
      _List_const_iterator(const __detail::_List_node_base* __x)
      : _M_node(__x) { }

      _List_const_iterator(const iterator& __x)
      : _M_node(__x._M_node) { }

      // Must downcast from List_node_base to _List_node to get to
      // _M_data.
      reference
      operator*() const
      { return static_cast<_Node*>(_M_node)->_M_data; }

      pointer
      operator->() const
      { return std::__addressof(static_cast<_Node*>(_M_node)->_M_data); }

      _Self&
      operator++()
      {
	_M_node = _M_node->_M_next;
	return *this;
      }

      _Self
      operator++(int)
      {
	_Self __tmp = *this;
	_M_node = _M_node->_M_next;
	return __tmp;
      }

      _Self&
      operator--()
      {
	_M_node = _M_node->_M_prev;
	return *this;
      }

      _Self
      operator--(int)
      {
	_Self __tmp = *this;
	_M_node = _M_node->_M_prev;
	return __tmp;
      }

      bool
      operator==(const _Self& __x) const
      { return _M_node == __x._M_node; }

      bool
      operator!=(const _Self& __x) const
      { return _M_node != __x._M_node; }

      // The only member points to the %list element.
      const __detail::_List_node_base* _M_node;
    };

  template<typename _Val>
    inline bool
    operator==(const _List_iterator<_Val>& __x,
	       const _List_const_iterator<_Val>& __y)
    { return __x._M_node == __y._M_node; }

  template<typename _Val>
    inline bool
    operator!=(const _List_iterator<_Val>& __x,
               const _List_const_iterator<_Val>& __y)
    { return __x._M_node != __y._M_node; }


  /// See bits/stl_deque.h's _Deque_base for an explanation.
  template<typename _Tp, typename _Alloc>
    class _List_base
    {
    protected:
      // NOTA BENE
      // The stored instance is not actually of "allocator_type"'s
      // type.  Instead we rebind the type to
      // Allocator<List_node<Tp>>, which according to [20.1.5]/4
      // should probably be the same.  List_node<Tp> is not the same
      // size as Tp (it's two pointers larger), and specializations on
      // Tp may go unused because List_node<Tp> is being bound
      // instead.
      //
      // We put this to the test in the constructors and in
      // get_allocator, where we use conversions between
      // allocator_type and _Node_alloc_type. The conversion is
      // required by table 32 in [20.1.5].
      typedef typename _Alloc::template rebind<_List_node<_Tp> >::other
        _Node_alloc_type;

      typedef typename _Alloc::template rebind<_Tp>::other _Tp_alloc_type;

      struct _List_impl
      : public _Node_alloc_type
      {
	__detail::_List_node_base _M_node;

	_List_impl()
	: _Node_alloc_type(), _M_node()
	{ }

	_List_impl(const _Node_alloc_type& __a)
	: _Node_alloc_type(__a), _M_node()
	{ }

#if __cplusplus >= 201103L
	_List_impl(_Node_alloc_type&& __a)
	: _Node_alloc_type(std::move(__a)), _M_node()
	{ }
#endif
      };

      _List_impl _M_impl;

      _List_node<_Tp>*
      _M_get_node()
      { return _M_impl._Node_alloc_type::allocate(1); }

      void
      _M_put_node(_List_node<_Tp>* __p)
      { _M_impl._Node_alloc_type::deallocate(__p, 1); }

  public:
      typedef _Alloc allocator_type;

      _Node_alloc_type&
      _M_get_Node_allocator() _GLIBCXX_NOEXCEPT
      { return *static_cast<_Node_alloc_type*>(&_M_impl); }

      const _Node_alloc_type&
      _M_get_Node_allocator() const _GLIBCXX_NOEXCEPT
      { return *static_cast<const _Node_alloc_type*>(&_M_impl); }

      _Tp_alloc_type
      _M_get_Tp_allocator() const _GLIBCXX_NOEXCEPT
      { return _Tp_alloc_type(_M_get_Node_allocator()); }

      allocator_type
      get_allocator() const _GLIBCXX_NOEXCEPT
      { return allocator_type(_M_get_Node_allocator()); }

      _List_base()
      : _M_impl()
      { _M_init(); }

      _List_base(const _Node_alloc_type& __a)
      : _M_impl(__a)
      { _M_init(); }

#if __cplusplus >= 201103L
      _List_base(_List_base&& __x)
      : _M_impl(std::move(__x._M_get_Node_allocator()))
      {
	_M_init();
	__detail::_List_node_base::swap(_M_impl._M_node, __x._M_impl._M_node);
      }
#endif

      // This is what actually destroys the list.
      ~_List_base() _GLIBCXX_NOEXCEPT
      { _M_clear(); }

      void
      _M_clear();

      void
      _M_init()
      {
        this->_M_impl._M_node._M_next = &this->_M_impl._M_node;
        this->_M_impl._M_node._M_prev = &this->_M_impl._M_node;
      }
    };

  /**
   *  @brief A standard container with linear time access to elements,
   *  and fixed time insertion/deletion at any point in the sequence.
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
   *  %exception of @c at and @c operator[].
   *
   *  This is a @e doubly @e linked %list.  Traversal up and down the
   *  %list requires linear time, but adding and removing elements (or
   *  @e nodes) is done in constant time, regardless of where the
   *  change takes place.  Unlike std::vector and std::deque,
   *  random-access iterators are not provided, so subscripting ( @c
   *  [] ) access is not allowed.  For algorithms which only need
   *  sequential access, this lack makes no difference.
   *
   *  Also unlike the other standard containers, std::list provides
   *  specialized algorithms %unique to linked lists, such as
   *  splicing, sorting, and in-place reversal.
   *
   *  A couple points on memory allocation for list<Tp>:
   *
   *  First, we never actually allocate a Tp, we allocate
   *  List_node<Tp>'s and trust [20.1.5]/4 to DTRT.  This is to ensure
   *  that after elements from %list<X,Alloc1> are spliced into
   *  %list<X,Alloc2>, destroying the memory of the second %list is a
   *  valid operation, i.e., Alloc1 giveth and Alloc2 taketh away.
   *
   *  Second, a %list conceptually represented as
   *  @code
   *    A <---> B <---> C <---> D
   *  @endcode
   *  is actually circular; a link exists between A and D.  The %list
   *  class holds (as its only data member) a private list::iterator
   *  pointing to @e D, not to @e A!  To get to the head of the %list,
   *  we start at the tail and move forward by one.  When this member
   *  iterator's next/previous pointers refer to itself, the %list is
   *  %empty. 
  */
  template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
    class list : protected _List_base<_Tp, _Alloc>
    {
      // concept requirements
      typedef typename _Alloc::value_type                _Alloc_value_type;
      __glibcxx_class_requires(_Tp, _SGIAssignableConcept)
      __glibcxx_class_requires2(_Tp, _Alloc_value_type, _SameTypeConcept)

      typedef _List_base<_Tp, _Alloc>                    _Base;
      typedef typename _Base::_Tp_alloc_type		 _Tp_alloc_type;
      typedef typename _Base::_Node_alloc_type		 _Node_alloc_type;

    public:
      typedef _Tp                                        value_type;
      typedef typename _Tp_alloc_type::pointer           pointer;
      typedef typename _Tp_alloc_type::const_pointer     const_pointer;
      typedef typename _Tp_alloc_type::reference         reference;
      typedef typename _Tp_alloc_type::const_reference   const_reference;
      typedef _List_iterator<_Tp>                        iterator;
      typedef _List_const_iterator<_Tp>                  const_iterator;
      typedef std::reverse_iterator<const_iterator>      const_reverse_iterator;
      typedef std::reverse_iterator<iterator>            reverse_iterator;
      typedef size_t                                     size_type;
      typedef ptrdiff_t                                  difference_type;
      typedef _Alloc                                     allocator_type;

    protected:
      // Note that pointers-to-_Node's can be ctor-converted to
      // iterator types.
      typedef _List_node<_Tp>				 _Node;

      using _Base::_M_impl;
      using _Base::_M_put_node;
      using _Base::_M_get_node;
      using _Base::_M_get_Tp_allocator;
      using _Base::_M_get_Node_allocator;

      /**
       *  @param  __args  An instance of user data.
       *
       *  Allocates space for a new node and constructs a copy of
       *  @a __args in it.
       */
#if __cplusplus < 201103L
      _Node*
      _M_create_node(const value_type& __x)
      {
	_Node* __p = this->_M_get_node();
	__try
	  {
	    _M_get_Tp_allocator().construct
	      (std::__addressof(__p->_M_data), __x);
	  }
	__catch(...)
	  {
	    _M_put_node(__p);
	    __throw_exception_again;
	  }
	return __p;
      }
#else
      template<typename... _Args>
        _Node*
        _M_create_node(_Args&&... __args)
	{
	  _Node* __p = this->_M_get_node();
	  __try
	    {
	      _M_get_Node_allocator().construct(__p,
						std::forward<_Args>(__args)...);
	    }
	  __catch(...)
	    {
	      _M_put_node(__p);
	      __throw_exception_again;
	    }
	  return __p;
	}
#endif

    public:
      // [23.2.2.1] construct/copy/destroy
      // (assign() and get_allocator() are also listed in this section)
      /**
       *  @brief  Default constructor creates no elements.
       */
      list()
      : _Base() { }

      /**
       *  @brief  Creates a %list with no elements.
       *  @param  __a  An allocator object.
       */
      explicit
      list(const allocator_type& __a)
      : _Base(_Node_alloc_type(__a)) { }

#if __cplusplus >= 201103L
      /**
       *  @brief  Creates a %list with default constructed elements.
       *  @param  __n  The number of elements to initially create.
       *
       *  This constructor fills the %list with @a __n default
       *  constructed elements.
       */
      explicit
      list(size_type __n)
      : _Base()
      { _M_default_initialize(__n); }

      /**
       *  @brief  Creates a %list with copies of an exemplar element.
       *  @param  __n  The number of elements to initially create.
       *  @param  __value  An element to copy.
       *  @param  __a  An allocator object.
       *
       *  This constructor fills the %list with @a __n copies of @a __value.
       */
      list(size_type __n, const value_type& __value,
	   const allocator_type& __a = allocator_type())
      : _Base(_Node_alloc_type(__a))
      { _M_fill_initialize(__n, __value); }
#else
      /**
       *  @brief  Creates a %list with copies of an exemplar element.
       *  @param  __n  The number of elements to initially create.
       *  @param  __value  An element to copy.
       *  @param  __a  An allocator object.
       *
       *  This constructor fills the %list with @a __n copies of @a __value.
       */
      explicit
      list(size_type __n, const value_type& __value = value_type(),
	   const allocator_type& __a = allocator_type())
      : _Base(_Node_alloc_type(__a))
      { _M_fill_initialize(__n, __value); }
#endif

      /**
       *  @brief  %List copy constructor.
       *  @param  __x  A %list of identical element and allocator types.
       *
       *  The newly-created %list uses a copy of the allocation object used
       *  by @a __x.
       */
      list(const list& __x)
      : _Base(__x._M_get_Node_allocator())
      { _M_initialize_dispatch(__x.begin(), __x.end(), __false_type()); }

#if __cplusplus >= 201103L
      /**
       *  @brief  %List move constructor.
       *  @param  __x  A %list of identical element and allocator types.
       *
       *  The newly-created %list contains the exact contents of @a __x.
       *  The contents of @a __x are a valid, but unspecified %list.
       */
      list(list&& __x) noexcept
      : _Base(std::move(__x)) { }

      /**
       *  @brief  Builds a %list from an initializer_list
       *  @param  __l  An initializer_list of value_type.
       *  @param  __a  An allocator object.
       *
       *  Create a %list consisting of copies of the elements in the
       *  initializer_list @a __l.  This is linear in __l.size().
       */
      list(initializer_list<value_type> __l,
           const allocator_type& __a = allocator_type())
      : _Base(_Node_alloc_type(__a))
      { _M_initialize_dispatch(__l.begin(), __l.end(), __false_type()); }
#endif

      /**
       *  @brief  Builds a %list from a range.
       *  @param  __first  An input iterator.
       *  @param  __last  An input iterator.
       *  @param  __a  An allocator object.
       *
       *  Create a %list consisting of copies of the elements from
       *  [@a __first,@a __last).  This is linear in N (where N is
       *  distance(@a __first,@a __last)).
       */
#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
        list(_InputIterator __first, _InputIterator __last,
	     const allocator_type& __a = allocator_type())
	: _Base(_Node_alloc_type(__a))
        { _M_initialize_dispatch(__first, __last, __false_type()); }
#else
      template<typename _InputIterator>
        list(_InputIterator __first, _InputIterator __last,
	     const allocator_type& __a = allocator_type())
	: _Base(_Node_alloc_type(__a))
        { 
	  // Check whether it's an integral type.  If so, it's not an iterator.
	  typedef typename std::__is_integer<_InputIterator>::__type _Integral;
	  _M_initialize_dispatch(__first, __last, _Integral());
	}
#endif

      /**
       *  No explicit dtor needed as the _Base dtor takes care of
       *  things.  The _Base dtor only erases the elements, and note
       *  that if the elements themselves are pointers, the pointed-to
       *  memory is not touched in any way.  Managing the pointer is
       *  the user's responsibility.
       */

      /**
       *  @brief  %List assignment operator.
       *  @param  __x  A %list of identical element and allocator types.
       *
       *  All the elements of @a __x are copied, but unlike the copy
       *  constructor, the allocator object is not copied.
       */
      list&
      operator=(const list& __x);

#if __cplusplus >= 201103L
      /**
       *  @brief  %List move assignment operator.
       *  @param  __x  A %list of identical element and allocator types.
       *
       *  The contents of @a __x are moved into this %list (without copying).
       *  @a __x is a valid, but unspecified %list
       */
      list&
      operator=(list&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__x);
	return *this;
      }

      /**
       *  @brief  %List initializer list assignment operator.
       *  @param  __l  An initializer_list of value_type.
       *
       *  Replace the contents of the %list with copies of the elements
       *  in the initializer_list @a __l.  This is linear in l.size().
       */
      list&
      operator=(initializer_list<value_type> __l)
      {
	this->assign(__l.begin(), __l.end());
	return *this;
      }
#endif

      /**
       *  @brief  Assigns a given value to a %list.
       *  @param  __n  Number of elements to be assigned.
       *  @param  __val  Value to be assigned.
       *
       *  This function fills a %list with @a __n copies of the given
       *  value.  Note that the assignment completely changes the %list
       *  and that the resulting %list's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
      void
      assign(size_type __n, const value_type& __val)
      { _M_fill_assign(__n, __val); }

      /**
       *  @brief  Assigns a range to a %list.
       *  @param  __first  An input iterator.
       *  @param  __last   An input iterator.
       *
       *  This function fills a %list with copies of the elements in the
       *  range [@a __first,@a __last).
       *
       *  Note that the assignment completely changes the %list and
       *  that the resulting %list's size is the same as the number of
       *  elements assigned.  Old data may be lost.
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
       *  @brief  Assigns an initializer_list to a %list.
       *  @param  __l  An initializer_list of value_type.
       *
       *  Replace the contents of the %list with copies of the elements
       *  in the initializer_list @a __l.  This is linear in __l.size().
       */
      void
      assign(initializer_list<value_type> __l)
      { this->assign(__l.begin(), __l.end()); }
#endif

      /// Get a copy of the memory allocation object.
      allocator_type
      get_allocator() const _GLIBCXX_NOEXCEPT
      { return _Base::get_allocator(); }

      // iterators
      /**
       *  Returns a read/write iterator that points to the first element in the
       *  %list.  Iteration is done in ordinary element order.
       */
      iterator
      begin() _GLIBCXX_NOEXCEPT
      { return iterator(this->_M_impl._M_node._M_next); }

      /**
       *  Returns a read-only (constant) iterator that points to the
       *  first element in the %list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      begin() const _GLIBCXX_NOEXCEPT
      { return const_iterator(this->_M_impl._M_node._M_next); }

      /**
       *  Returns a read/write iterator that points one past the last
       *  element in the %list.  Iteration is done in ordinary element
       *  order.
       */
      iterator
      end() _GLIBCXX_NOEXCEPT
      { return iterator(&this->_M_impl._M_node); }

      /**
       *  Returns a read-only (constant) iterator that points one past
       *  the last element in the %list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      end() const _GLIBCXX_NOEXCEPT
      { return const_iterator(&this->_M_impl._M_node); }

      /**
       *  Returns a read/write reverse iterator that points to the last
       *  element in the %list.  Iteration is done in reverse element
       *  order.
       */
      reverse_iterator
      rbegin() _GLIBCXX_NOEXCEPT
      { return reverse_iterator(end()); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to
       *  the last element in the %list.  Iteration is done in reverse
       *  element order.
       */
      const_reverse_iterator
      rbegin() const _GLIBCXX_NOEXCEPT
      { return const_reverse_iterator(end()); }

      /**
       *  Returns a read/write reverse iterator that points to one
       *  before the first element in the %list.  Iteration is done in
       *  reverse element order.
       */
      reverse_iterator
      rend() _GLIBCXX_NOEXCEPT
      { return reverse_iterator(begin()); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to one
       *  before the first element in the %list.  Iteration is done in reverse
       *  element order.
       */
      const_reverse_iterator
      rend() const _GLIBCXX_NOEXCEPT
      { return const_reverse_iterator(begin()); }

#if __cplusplus >= 201103L
      /**
       *  Returns a read-only (constant) iterator that points to the
       *  first element in the %list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      cbegin() const noexcept
      { return const_iterator(this->_M_impl._M_node._M_next); }

      /**
       *  Returns a read-only (constant) iterator that points one past
       *  the last element in the %list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      cend() const noexcept
      { return const_iterator(&this->_M_impl._M_node); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to
       *  the last element in the %list.  Iteration is done in reverse
       *  element order.
       */
      const_reverse_iterator
      crbegin() const noexcept
      { return const_reverse_iterator(end()); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to one
       *  before the first element in the %list.  Iteration is done in reverse
       *  element order.
       */
      const_reverse_iterator
      crend() const noexcept
      { return const_reverse_iterator(begin()); }
#endif

      // [23.2.2.2] capacity
      /**
       *  Returns true if the %list is empty.  (Thus begin() would equal
       *  end().)
       */
      bool
      empty() const _GLIBCXX_NOEXCEPT
      { return this->_M_impl._M_node._M_next == &this->_M_impl._M_node; }

      /**  Returns the number of elements in the %list.  */
      size_type
      size() const _GLIBCXX_NOEXCEPT
      { return std::distance(begin(), end()); }

      /**  Returns the size() of the largest possible %list.  */
      size_type
      max_size() const _GLIBCXX_NOEXCEPT
      { return _M_get_Node_allocator().max_size(); }

#if __cplusplus >= 201103L
      /**
       *  @brief Resizes the %list to the specified number of elements.
       *  @param __new_size Number of elements the %list should contain.
       *
       *  This function will %resize the %list to the specified number
       *  of elements.  If the number is smaller than the %list's
       *  current size the %list is truncated, otherwise default
       *  constructed elements are appended.
       */
      void
      resize(size_type __new_size);

      /**
       *  @brief Resizes the %list to the specified number of elements.
       *  @param __new_size Number of elements the %list should contain.
       *  @param __x Data with which new elements should be populated.
       *
       *  This function will %resize the %list to the specified number
       *  of elements.  If the number is smaller than the %list's
       *  current size the %list is truncated, otherwise the %list is
       *  extended and new elements are populated with given data.
       */
      void
      resize(size_type __new_size, const value_type& __x);
#else
      /**
       *  @brief Resizes the %list to the specified number of elements.
       *  @param __new_size Number of elements the %list should contain.
       *  @param __x Data with which new elements should be populated.
       *
       *  This function will %resize the %list to the specified number
       *  of elements.  If the number is smaller than the %list's
       *  current size the %list is truncated, otherwise the %list is
       *  extended and new elements are populated with given data.
       */
      void
      resize(size_type __new_size, value_type __x = value_type());
#endif

      // element access
      /**
       *  Returns a read/write reference to the data at the first
       *  element of the %list.
       */
      reference
      front()
      { return *begin(); }

      /**
       *  Returns a read-only (constant) reference to the data at the first
       *  element of the %list.
       */
      const_reference
      front() const
      { return *begin(); }

      /**
       *  Returns a read/write reference to the data at the last element
       *  of the %list.
       */
      reference
      back()
      { 
	iterator __tmp = end();
	--__tmp;
	return *__tmp;
      }

      /**
       *  Returns a read-only (constant) reference to the data at the last
       *  element of the %list.
       */
      const_reference
      back() const
      { 
	const_iterator __tmp = end();
	--__tmp;
	return *__tmp;
      }

      // [23.2.2.3] modifiers
      /**
       *  @brief  Add data to the front of the %list.
       *  @param  __x  Data to be added.
       *
       *  This is a typical stack operation.  The function creates an
       *  element at the front of the %list and assigns the given data
       *  to it.  Due to the nature of a %list this operation can be
       *  done in constant time, and does not invalidate iterators and
       *  references.
       */
      void
      push_front(const value_type& __x)
      { this->_M_insert(begin(), __x); }

#if __cplusplus >= 201103L
      void
      push_front(value_type&& __x)
      { this->_M_insert(begin(), std::move(__x)); }

      template<typename... _Args>
        void
        emplace_front(_Args&&... __args)
        { this->_M_insert(begin(), std::forward<_Args>(__args)...); }
#endif

      /**
       *  @brief  Removes first element.
       *
       *  This is a typical stack operation.  It shrinks the %list by
       *  one.  Due to the nature of a %list this operation can be done
       *  in constant time, and only invalidates iterators/references to
       *  the element being removed.
       *
       *  Note that no data is returned, and if the first element's data
       *  is needed, it should be retrieved before pop_front() is
       *  called.
       */
      void
      pop_front()
      { this->_M_erase(begin()); }

      /**
       *  @brief  Add data to the end of the %list.
       *  @param  __x  Data to be added.
       *
       *  This is a typical stack operation.  The function creates an
       *  element at the end of the %list and assigns the given data to
       *  it.  Due to the nature of a %list this operation can be done
       *  in constant time, and does not invalidate iterators and
       *  references.
       */
      void
      push_back(const value_type& __x)
      { this->_M_insert(end(), __x); }

#if __cplusplus >= 201103L
      void
      push_back(value_type&& __x)
      { this->_M_insert(end(), std::move(__x)); }

      template<typename... _Args>
        void
        emplace_back(_Args&&... __args)
        { this->_M_insert(end(), std::forward<_Args>(__args)...); }
#endif

      /**
       *  @brief  Removes last element.
       *
       *  This is a typical stack operation.  It shrinks the %list by
       *  one.  Due to the nature of a %list this operation can be done
       *  in constant time, and only invalidates iterators/references to
       *  the element being removed.
       *
       *  Note that no data is returned, and if the last element's data
       *  is needed, it should be retrieved before pop_back() is called.
       */
      void
      pop_back()
      { this->_M_erase(iterator(this->_M_impl._M_node._M_prev)); }

#if __cplusplus >= 201103L
      /**
       *  @brief  Constructs object in %list before specified iterator.
       *  @param  __position  A const_iterator into the %list.
       *  @param  __args  Arguments.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert an object of type T constructed
       *  with T(std::forward<Args>(args)...) before the specified
       *  location.  Due to the nature of a %list this operation can
       *  be done in constant time, and does not invalidate iterators
       *  and references.
       */
      template<typename... _Args>
        iterator
        emplace(iterator __position, _Args&&... __args);
#endif

      /**
       *  @brief  Inserts given value into %list before specified iterator.
       *  @param  __position  An iterator into the %list.
       *  @param  __x  Data to be inserted.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert a copy of the given value before
       *  the specified location.  Due to the nature of a %list this
       *  operation can be done in constant time, and does not
       *  invalidate iterators and references.
       */
      iterator
      insert(iterator __position, const value_type& __x);

#if __cplusplus >= 201103L
      /**
       *  @brief  Inserts given rvalue into %list before specified iterator.
       *  @param  __position  An iterator into the %list.
       *  @param  __x  Data to be inserted.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert a copy of the given rvalue before
       *  the specified location.  Due to the nature of a %list this
       *  operation can be done in constant time, and does not
       *  invalidate iterators and references.
        */
      iterator
      insert(iterator __position, value_type&& __x)
      { return emplace(__position, std::move(__x)); }

      /**
       *  @brief  Inserts the contents of an initializer_list into %list
       *          before specified iterator.
       *  @param  __p  An iterator into the %list.
       *  @param  __l  An initializer_list of value_type.
       *
       *  This function will insert copies of the data in the
       *  initializer_list @a l into the %list before the location
       *  specified by @a p.
       *
       *  This operation is linear in the number of elements inserted and
       *  does not invalidate iterators and references.
       */
      void
      insert(iterator __p, initializer_list<value_type> __l)
      { this->insert(__p, __l.begin(), __l.end()); }
#endif

      /**
       *  @brief  Inserts a number of copies of given data into the %list.
       *  @param  __position  An iterator into the %list.
       *  @param  __n  Number of elements to be inserted.
       *  @param  __x  Data to be inserted.
       *
       *  This function will insert a specified number of copies of the
       *  given data before the location specified by @a position.
       *
       *  This operation is linear in the number of elements inserted and
       *  does not invalidate iterators and references.
       */
      void
      insert(iterator __position, size_type __n, const value_type& __x)
      {
	list __tmp(__n, __x, get_allocator());
	splice(__position, __tmp);
      }

      /**
       *  @brief  Inserts a range into the %list.
       *  @param  __position  An iterator into the %list.
       *  @param  __first  An input iterator.
       *  @param  __last   An input iterator.
       *
       *  This function will insert copies of the data in the range [@a
       *  first,@a last) into the %list before the location specified by
       *  @a position.
       *
       *  This operation is linear in the number of elements inserted and
       *  does not invalidate iterators and references.
       */
#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
#else
      template<typename _InputIterator>
#endif
        void
        insert(iterator __position, _InputIterator __first,
	       _InputIterator __last)
        {
	  list __tmp(__first, __last, get_allocator());
	  splice(__position, __tmp);
	}

      /**
       *  @brief  Remove element at given position.
       *  @param  __position  Iterator pointing to element to be erased.
       *  @return  An iterator pointing to the next element (or end()).
       *
       *  This function will erase the element at the given position and thus
       *  shorten the %list by one.
       *
       *  Due to the nature of a %list this operation can be done in
       *  constant time, and only invalidates iterators/references to
       *  the element being removed.  The user is also cautioned that
       *  this function only erases the element, and that if the element
       *  is itself a pointer, the pointed-to memory is not touched in
       *  any way.  Managing the pointer is the user's responsibility.
       */
      iterator
      erase(iterator __position);

      /**
       *  @brief  Remove a range of elements.
       *  @param  __first  Iterator pointing to the first element to be erased.
       *  @param  __last  Iterator pointing to one past the last element to be
       *                erased.
       *  @return  An iterator pointing to the element pointed to by @a last
       *           prior to erasing (or end()).
       *
       *  This function will erase the elements in the range @a
       *  [first,last) and shorten the %list accordingly.
       *
       *  This operation is linear time in the size of the range and only
       *  invalidates iterators/references to the element being removed.
       *  The user is also cautioned that this function only erases the
       *  elements, and that if the elements themselves are pointers, the
       *  pointed-to memory is not touched in any way.  Managing the pointer
       *  is the user's responsibility.
       */
      iterator
      erase(iterator __first, iterator __last)
      {
	while (__first != __last)
	  __first = erase(__first);
	return __last;
      }

      /**
       *  @brief  Swaps data with another %list.
       *  @param  __x  A %list of the same element and allocator types.
       *
       *  This exchanges the elements between two lists in constant
       *  time.  Note that the global std::swap() function is
       *  specialized such that std::swap(l1,l2) will feed to this
       *  function.
       */
      void
      swap(list& __x)
      {
	__detail::_List_node_base::swap(this->_M_impl._M_node, 
					__x._M_impl._M_node);

	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// 431. Swapping containers with unequal allocators.
	std::__alloc_swap<typename _Base::_Node_alloc_type>::
	  _S_do_it(_M_get_Node_allocator(), __x._M_get_Node_allocator());
      }

      /**
       *  Erases all the elements.  Note that this function only erases
       *  the elements, and that if the elements themselves are
       *  pointers, the pointed-to memory is not touched in any way.
       *  Managing the pointer is the user's responsibility.
       */
      void
      clear() _GLIBCXX_NOEXCEPT
      {
        _Base::_M_clear();
        _Base::_M_init();
      }

      // [23.2.2.4] list operations
      /**
       *  @brief  Insert contents of another %list.
       *  @param  __position  Iterator referencing the element to insert before.
       *  @param  __x  Source list.
       *
       *  The elements of @a __x are inserted in constant time in front of
       *  the element referenced by @a __position.  @a __x becomes an empty
       *  list.
       *
       *  Requires this != @a __x.
       */
      void
#if __cplusplus >= 201103L
      splice(iterator __position, list&& __x)
#else
      splice(iterator __position, list& __x)
#endif
      {
	if (!__x.empty())
	  {
	    _M_check_equal_allocators(__x);

	    this->_M_transfer(__position, __x.begin(), __x.end());
	  }
      }

#if __cplusplus >= 201103L
      void
      splice(iterator __position, list& __x)
      { splice(__position, std::move(__x)); }
#endif

      /**
       *  @brief  Insert element from another %list.
       *  @param  __position  Iterator referencing the element to insert before.
       *  @param  __x  Source list.
       *  @param  __i  Iterator referencing the element to move.
       *
       *  Removes the element in list @a __x referenced by @a __i and
       *  inserts it into the current list before @a __position.
       */
      void
#if __cplusplus >= 201103L
      splice(iterator __position, list&& __x, iterator __i)
#else
      splice(iterator __position, list& __x, iterator __i)
#endif
      {
	iterator __j = __i;
	++__j;
	if (__position == __i || __position == __j)
	  return;

	if (this != &__x)
	  _M_check_equal_allocators(__x);

	this->_M_transfer(__position, __i, __j);
      }

#if __cplusplus >= 201103L
      void
      splice(iterator __position, list& __x, iterator __i)
      { splice(__position, std::move(__x), __i); }
#endif

      /**
       *  @brief  Insert range from another %list.
       *  @param  __position  Iterator referencing the element to insert before.
       *  @param  __x  Source list.
       *  @param  __first  Iterator referencing the start of range in x.
       *  @param  __last  Iterator referencing the end of range in x.
       *
       *  Removes elements in the range [__first,__last) and inserts them
       *  before @a __position in constant time.
       *
       *  Undefined if @a __position is in [__first,__last).
       */
      void
#if __cplusplus >= 201103L
      splice(iterator __position, list&& __x, iterator __first,
	     iterator __last)
#else
      splice(iterator __position, list& __x, iterator __first,
	     iterator __last)
#endif
      {
	if (__first != __last)
	  {
	    if (this != &__x)
	      _M_check_equal_allocators(__x);

	    this->_M_transfer(__position, __first, __last);
	  }
      }

#if __cplusplus >= 201103L
      void
      splice(iterator __position, list& __x, iterator __first, iterator __last)
      { splice(__position, std::move(__x), __first, __last); }
#endif

      /**
       *  @brief  Remove all elements equal to value.
       *  @param  __value  The value to remove.
       *
       *  Removes every element in the list equal to @a value.
       *  Remaining elements stay in list order.  Note that this
       *  function only erases the elements, and that if the elements
       *  themselves are pointers, the pointed-to memory is not
       *  touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      void
      remove(const _Tp& __value);

      /**
       *  @brief  Remove all elements satisfying a predicate.
       *  @tparam  _Predicate  Unary predicate function or object.
       *
       *  Removes every element in the list for which the predicate
       *  returns true.  Remaining elements stay in list order.  Note
       *  that this function only erases the elements, and that if the
       *  elements themselves are pointers, the pointed-to memory is
       *  not touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      template<typename _Predicate>
        void
        remove_if(_Predicate);

      /**
       *  @brief  Remove consecutive duplicate elements.
       *
       *  For each consecutive set of elements with the same value,
       *  remove all but the first one.  Remaining elements stay in
       *  list order.  Note that this function only erases the
       *  elements, and that if the elements themselves are pointers,
       *  the pointed-to memory is not touched in any way.  Managing
       *  the pointer is the user's responsibility.
       */
      void
      unique();

      /**
       *  @brief  Remove consecutive elements satisfying a predicate.
       *  @tparam _BinaryPredicate  Binary predicate function or object.
       *
       *  For each consecutive set of elements [first,last) that
       *  satisfy predicate(first,i) where i is an iterator in
       *  [first,last), remove all but the first one.  Remaining
       *  elements stay in list order.  Note that this function only
       *  erases the elements, and that if the elements themselves are
       *  pointers, the pointed-to memory is not touched in any way.
       *  Managing the pointer is the user's responsibility.
       */
      template<typename _BinaryPredicate>
        void
        unique(_BinaryPredicate);

      /**
       *  @brief  Merge sorted lists.
       *  @param  __x  Sorted list to merge.
       *
       *  Assumes that both @a __x and this list are sorted according to
       *  operator<().  Merges elements of @a __x into this list in
       *  sorted order, leaving @a __x empty when complete.  Elements in
       *  this list precede elements in @a __x that are equal.
       */
#if __cplusplus >= 201103L
      void
      merge(list&& __x);

      void
      merge(list& __x)
      { merge(std::move(__x)); }
#else
      void
      merge(list& __x);
#endif

      /**
       *  @brief  Merge sorted lists according to comparison function.
       *  @tparam _StrictWeakOrdering Comparison function defining
       *  sort order.
       *  @param  __x  Sorted list to merge.
       *  @param  __comp  Comparison functor.
       *
       *  Assumes that both @a __x and this list are sorted according to
       *  StrictWeakOrdering.  Merges elements of @a __x into this list
       *  in sorted order, leaving @a __x empty when complete.  Elements
       *  in this list precede elements in @a __x that are equivalent
       *  according to StrictWeakOrdering().
       */
#if __cplusplus >= 201103L
      template<typename _StrictWeakOrdering>
        void
        merge(list&& __x, _StrictWeakOrdering __comp);

      template<typename _StrictWeakOrdering>
        void
        merge(list& __x, _StrictWeakOrdering __comp)
        { merge(std::move(__x), __comp); }
#else
      template<typename _StrictWeakOrdering>
        void
        merge(list& __x, _StrictWeakOrdering __comp);
#endif

      /**
       *  @brief  Reverse the elements in list.
       *
       *  Reverse the order of elements in the list in linear time.
       */
      void
      reverse() _GLIBCXX_NOEXCEPT
      { this->_M_impl._M_node._M_reverse(); }

      /**
       *  @brief  Sort the elements.
       *
       *  Sorts the elements of this list in NlogN time.  Equivalent
       *  elements remain in list order.
       */
      void
      sort();

      /**
       *  @brief  Sort the elements according to comparison function.
       *
       *  Sorts the elements of this list in NlogN time.  Equivalent
       *  elements remain in list order.
       */
      template<typename _StrictWeakOrdering>
        void
        sort(_StrictWeakOrdering);

    protected:
      // Internal constructor functions follow.

      // Called by the range constructor to implement [23.1.1]/9

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 438. Ambiguity in the "do the right thing" clause
      template<typename _Integer>
        void
        _M_initialize_dispatch(_Integer __n, _Integer __x, __true_type)
        { _M_fill_initialize(static_cast<size_type>(__n), __x); }

      // Called by the range constructor to implement [23.1.1]/9
      template<typename _InputIterator>
        void
        _M_initialize_dispatch(_InputIterator __first, _InputIterator __last,
			       __false_type)
        {
	  for (; __first != __last; ++__first)
#if __cplusplus >= 201103L
	    emplace_back(*__first);
#else
	    push_back(*__first);
#endif
	}

      // Called by list(n,v,a), and the range constructor when it turns out
      // to be the same thing.
      void
      _M_fill_initialize(size_type __n, const value_type& __x)
      {
	for (; __n; --__n)
	  push_back(__x);
      }

#if __cplusplus >= 201103L
      // Called by list(n).
      void
      _M_default_initialize(size_type __n)
      {
	for (; __n; --__n)
	  emplace_back();
      }

      // Called by resize(sz).
      void
      _M_default_append(size_type __n);
#endif

      // Internal assign functions follow.

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
			   __false_type);

      // Called by assign(n,t), and the range assign when it turns out
      // to be the same thing.
      void
      _M_fill_assign(size_type __n, const value_type& __val);


      // Moves the elements from [first,last) before position.
      void
      _M_transfer(iterator __position, iterator __first, iterator __last)
      { __position._M_node->_M_transfer(__first._M_node, __last._M_node); }

      // Inserts new element at position given and with value given.
#if __cplusplus < 201103L
      void
      _M_insert(iterator __position, const value_type& __x)
      {
        _Node* __tmp = _M_create_node(__x);
        __tmp->_M_hook(__position._M_node);
      }
#else
     template<typename... _Args>
       void
       _M_insert(iterator __position, _Args&&... __args)
       {
	 _Node* __tmp = _M_create_node(std::forward<_Args>(__args)...);
	 __tmp->_M_hook(__position._M_node);
       }
#endif

      // Erases element at position given.
      void
      _M_erase(iterator __position)
      {
        __position._M_node->_M_unhook();
        _Node* __n = static_cast<_Node*>(__position._M_node);
#if __cplusplus >= 201103L
        _M_get_Node_allocator().destroy(__n);
#else
	_M_get_Tp_allocator().destroy(std::__addressof(__n->_M_data));
#endif
        _M_put_node(__n);
      }

      // To implement the splice (and merge) bits of N1599.
      void
      _M_check_equal_allocators(list& __x)
      {
	if (std::__alloc_neq<typename _Base::_Node_alloc_type>::
	    _S_do_it(_M_get_Node_allocator(), __x._M_get_Node_allocator()))
	  __throw_runtime_error(__N("list::_M_check_equal_allocators"));
      }
    };

  /**
   *  @brief  List equality comparison.
   *  @param  __x  A %list.
   *  @param  __y  A %list of the same type as @a __x.
   *  @return  True iff the size and elements of the lists are equal.
   *
   *  This is an equivalence relation.  It is linear in the size of
   *  the lists.  Lists are considered equivalent if their sizes are
   *  equal, and if corresponding elements compare equal.
  */
  template<typename _Tp, typename _Alloc>
    inline bool
    operator==(const list<_Tp, _Alloc>& __x, const list<_Tp, _Alloc>& __y)
    {
      typedef typename list<_Tp, _Alloc>::const_iterator const_iterator;
      const_iterator __end1 = __x.end();
      const_iterator __end2 = __y.end();

      const_iterator __i1 = __x.begin();
      const_iterator __i2 = __y.begin();
      while (__i1 != __end1 && __i2 != __end2 && *__i1 == *__i2)
	{
	  ++__i1;
	  ++__i2;
	}
      return __i1 == __end1 && __i2 == __end2;
    }

  /**
   *  @brief  List ordering relation.
   *  @param  __x  A %list.
   *  @param  __y  A %list of the same type as @a __x.
   *  @return  True iff @a __x is lexicographically less than @a __y.
   *
   *  This is a total ordering relation.  It is linear in the size of the
   *  lists.  The elements must be comparable with @c <.
   *
   *  See std::lexicographical_compare() for how the determination is made.
  */
  template<typename _Tp, typename _Alloc>
    inline bool
    operator<(const list<_Tp, _Alloc>& __x, const list<_Tp, _Alloc>& __y)
    { return std::lexicographical_compare(__x.begin(), __x.end(),
					  __y.begin(), __y.end()); }

  /// Based on operator==
  template<typename _Tp, typename _Alloc>
    inline bool
    operator!=(const list<_Tp, _Alloc>& __x, const list<_Tp, _Alloc>& __y)
    { return !(__x == __y); }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator>(const list<_Tp, _Alloc>& __x, const list<_Tp, _Alloc>& __y)
    { return __y < __x; }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator<=(const list<_Tp, _Alloc>& __x, const list<_Tp, _Alloc>& __y)
    { return !(__y < __x); }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator>=(const list<_Tp, _Alloc>& __x, const list<_Tp, _Alloc>& __y)
    { return !(__x < __y); }

  /// See std::list::swap().
  template<typename _Tp, typename _Alloc>
    inline void
    swap(list<_Tp, _Alloc>& __x, list<_Tp, _Alloc>& __y)
    { __x.swap(__y); }

_GLIBCXX_END_NAMESPACE_CONTAINER
} // namespace std

#endif /* _STL_LIST_H */
