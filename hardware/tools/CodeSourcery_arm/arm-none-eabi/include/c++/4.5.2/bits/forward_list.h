// <forward_list.h> -*- C++ -*-

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

/** @file forward_list.h
 *  This is a Standard C++ Library header.
 */

#ifndef _FORWARD_LIST_H
#define _FORWARD_LIST_H 1

#pragma GCC system_header

#include <memory>
#include <initializer_list>

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   *  @brief  A helper basic node class for %forward_list.
   *          This is just a linked list with nothing inside it.
   *          There are purely list shuffling utility methods here.
   */
  struct _Fwd_list_node_base
  {
    _Fwd_list_node_base() : _M_next(0) { }

    _Fwd_list_node_base* _M_next;

    static void
    swap(_Fwd_list_node_base& __x, _Fwd_list_node_base& __y)
    { std::swap(__x._M_next, __y._M_next); }

    _Fwd_list_node_base*
    _M_transfer_after(_Fwd_list_node_base* __begin)
    {
      _Fwd_list_node_base* __end = __begin;
      while (__end && __end->_M_next)
	__end = __end->_M_next;
      return _M_transfer_after(__begin, __end);
    }

    _Fwd_list_node_base*
    _M_transfer_after(_Fwd_list_node_base* __begin,
		      _Fwd_list_node_base* __end)
    {
      _Fwd_list_node_base* __keep = __begin->_M_next;
      if (__end)
	{
	  __begin->_M_next = __end->_M_next;
	  __end->_M_next = _M_next;
	}
      else
	__begin->_M_next = 0;
      _M_next = __keep;
      return __end;
    }

    void
    _M_reverse_after()
    {
      _Fwd_list_node_base* __tail = _M_next;
      if (!__tail)
	return;
      while (_Fwd_list_node_base* __temp = __tail->_M_next)
	{
	  _Fwd_list_node_base* __keep = _M_next;
	  _M_next = __temp;
	  __tail->_M_next = __temp->_M_next;
	  _M_next->_M_next = __keep;
	}
    }
  };

  /**
   *  @brief  A helper node class for %forward_list.
   *          This is just a linked list with a data value in each node.
   *          There is a sorting utility method.
   */
  template<typename _Tp>
    struct _Fwd_list_node
    : public _Fwd_list_node_base
    {
      template<typename... _Args>
        _Fwd_list_node(_Args&&... __args)
        : _Fwd_list_node_base(), 
          _M_value(std::forward<_Args>(__args)...) { }

      _Tp _M_value;
    };

  /**
   *   @brief A forward_list::iterator.
   * 
   *   All the functions are op overloads.
   */
  template<typename _Tp>
    struct _Fwd_list_iterator
    {
      typedef _Fwd_list_iterator<_Tp>            _Self;
      typedef _Fwd_list_node<_Tp>                _Node;

      typedef _Tp                                value_type;
      typedef _Tp*                               pointer;
      typedef _Tp&                               reference;
      typedef ptrdiff_t                          difference_type;
      typedef std::forward_iterator_tag          iterator_category;

      _Fwd_list_iterator()
      : _M_node() { }

      explicit
      _Fwd_list_iterator(_Fwd_list_node_base* __n) 
      : _M_node(__n) { }

      reference
      operator*() const
      { return static_cast<_Node*>(this->_M_node)->_M_value; }

      pointer
      operator->() const
      { return &static_cast<_Node*>(this->_M_node)->_M_value; }

      _Self&
      operator++()
      {
        _M_node = _M_node->_M_next;
        return *this;
      }

      _Self
      operator++(int)
      {
        _Self __tmp(*this);
        _M_node = _M_node->_M_next;
        return __tmp;
      }

      bool
      operator==(const _Self& __x) const
      { return _M_node == __x._M_node; }

      bool
      operator!=(const _Self& __x) const
      { return _M_node != __x._M_node; }

      _Self
      _M_next() const
      {
        if (_M_node)
          return _Fwd_list_iterator(_M_node->_M_next);
        else
          return _Fwd_list_iterator(0);
      }

      _Fwd_list_node_base* _M_node;
    };

  /**
   *   @brief A forward_list::const_iterator.
   * 
   *   All the functions are op overloads.
   */
  template<typename _Tp>
    struct _Fwd_list_const_iterator
    {
      typedef _Fwd_list_const_iterator<_Tp>      _Self;
      typedef const _Fwd_list_node<_Tp>          _Node;
      typedef _Fwd_list_iterator<_Tp>            iterator;

      typedef _Tp                                value_type;
      typedef const _Tp*                         pointer;
      typedef const _Tp&                         reference;
      typedef ptrdiff_t                          difference_type;
      typedef std::forward_iterator_tag          iterator_category;

      _Fwd_list_const_iterator()
      : _M_node() { }

      explicit
      _Fwd_list_const_iterator(const _Fwd_list_node_base* __n) 
      : _M_node(__n) { }

      _Fwd_list_const_iterator(const iterator& __iter)
      : _M_node(__iter._M_node) { }

      reference
      operator*() const
      { return static_cast<_Node*>(this->_M_node)->_M_value; }

      pointer
      operator->() const
      { return &static_cast<_Node*>(this->_M_node)->_M_value; }

      _Self&
      operator++()
      {
        _M_node = _M_node->_M_next;
        return *this;
      }

      _Self
      operator++(int)
      {
        _Self __tmp(*this);
        _M_node = _M_node->_M_next;
        return __tmp;
      }

      bool
      operator==(const _Self& __x) const
      { return _M_node == __x._M_node; }

      bool
      operator!=(const _Self& __x) const
      { return _M_node != __x._M_node; }

      _Self
      _M_next() const
      {
        if (this->_M_node)
          return _Fwd_list_const_iterator(_M_node->_M_next);
        else
          return _Fwd_list_const_iterator(0);
      }

      const _Fwd_list_node_base* _M_node;
    };

  /**
   *  @brief  Forward list iterator equality comparison.
   */
  template<typename _Tp>
    inline bool
    operator==(const _Fwd_list_iterator<_Tp>& __x,
               const _Fwd_list_const_iterator<_Tp>& __y)
    { return __x._M_node == __y._M_node; }

  /**
   *  @brief  Forward list iterator inequality comparison.
   */
  template<typename _Tp>
    inline bool
    operator!=(const _Fwd_list_iterator<_Tp>& __x,
               const _Fwd_list_const_iterator<_Tp>& __y)
    { return __x._M_node != __y._M_node; }

  /**
   *  @brief  Base class for %forward_list.
   */
  template<typename _Tp, typename _Alloc>
    struct _Fwd_list_base
    {
    protected:
      typedef typename _Alloc::template rebind<_Tp>::other _Tp_alloc_type;

      typedef typename _Alloc::template 
        rebind<_Fwd_list_node<_Tp>>::other _Node_alloc_type;

      struct _Fwd_list_impl 
      : public _Node_alloc_type
      {
        _Fwd_list_node_base _M_head;

        _Fwd_list_impl()
        : _Node_alloc_type(), _M_head()
        { }

        _Fwd_list_impl(const _Node_alloc_type& __a)
        : _Node_alloc_type(__a), _M_head()
        { }
      };

      _Fwd_list_impl _M_impl;

    public:
      typedef _Fwd_list_iterator<_Tp>                 iterator;
      typedef _Fwd_list_const_iterator<_Tp>           const_iterator;
      typedef _Fwd_list_node<_Tp>                     _Node;

      _Node_alloc_type&
      _M_get_Node_allocator()
      { return *static_cast<_Node_alloc_type*>(&this->_M_impl); }

      const _Node_alloc_type&
      _M_get_Node_allocator() const
      { return *static_cast<const _Node_alloc_type*>(&this->_M_impl); }

      _Fwd_list_base()
      : _M_impl()
      { this->_M_impl._M_head._M_next = 0; }

      _Fwd_list_base(const _Alloc& __a)
      : _M_impl(__a)
      { this->_M_impl._M_head._M_next = 0; }

      _Fwd_list_base(const _Fwd_list_base& __lst, const _Alloc& __a);

      _Fwd_list_base(_Fwd_list_base&& __lst, const _Alloc& __a)
      : _M_impl(__a)
      { _Fwd_list_node_base::swap(this->_M_impl._M_head,
				  __lst._M_impl._M_head); }

      _Fwd_list_base(_Fwd_list_base&& __lst)
      : _M_impl(__lst._M_get_Node_allocator())
      { _Fwd_list_node_base::swap(this->_M_impl._M_head,
				  __lst._M_impl._M_head); }

      ~_Fwd_list_base()
      { _M_erase_after(&_M_impl._M_head, 0); }

    protected:

      _Node*
      _M_get_node()
      { return _M_get_Node_allocator().allocate(1); }

      template<typename... _Args>
        _Node*
        _M_create_node(_Args&&... __args)
        {
          _Node* __node = this->_M_get_node();
          __try
            {
              _M_get_Node_allocator().construct(__node,
                                              std::forward<_Args>(__args)...);
              __node->_M_next = 0;
            }
          __catch(...)
            {
              this->_M_put_node(__node);
              __throw_exception_again;
            }
          return __node;
        }

      template<typename... _Args>
        _Fwd_list_node_base*
        _M_insert_after(const_iterator __pos, _Args&&... __args);

      void
      _M_put_node(_Node* __p)
      { _M_get_Node_allocator().deallocate(__p, 1); }

      void
      _M_erase_after(_Fwd_list_node_base* __pos);

      void
      _M_erase_after(_Fwd_list_node_base* __pos, 
                     _Fwd_list_node_base* __last);
    };

  /**
   *  @brief A standard container with linear time access to elements,
   *  and fixed time insertion/deletion at any point in the sequence.
   *
   *  @ingroup sequences
   *
   *  Meets the requirements of a <a href="tables.html#65">container</a>, a
   *  <a href="tables.html#67">sequence</a>, including the
   *  <a href="tables.html#68">optional sequence requirements</a> with the
   *  %exception of @c at and @c operator[].
   *
   *  This is a @e singly @e linked %list.  Traversal up the
   *  %list requires linear time, but adding and removing elements (or
   *  @e nodes) is done in constant time, regardless of where the
   *  change takes place.  Unlike std::vector and std::deque,
   *  random-access iterators are not provided, so subscripting ( @c
   *  [] ) access is not allowed.  For algorithms which only need
   *  sequential access, this lack makes no difference.
   *
   *  Also unlike the other standard containers, std::forward_list provides
   *  specialized algorithms %unique to linked lists, such as
   *  splicing, sorting, and in-place reversal.
   *
   *  A couple points on memory allocation for forward_list<Tp>:
   *
   *  First, we never actually allocate a Tp, we allocate
   *  Fwd_list_node<Tp>'s and trust [20.1.5]/4 to DTRT.  This is to ensure
   *  that after elements from %forward_list<X,Alloc1> are spliced into
   *  %forward_list<X,Alloc2>, destroying the memory of the second %list is a
   *  valid operation, i.e., Alloc1 giveth and Alloc2 taketh away.
   */
  template<typename _Tp, typename _Alloc = allocator<_Tp> >
    class forward_list : private _Fwd_list_base<_Tp, _Alloc>
    {
    private:
      typedef _Fwd_list_base<_Tp, _Alloc>                  _Base;
      typedef _Fwd_list_node<_Tp>                          _Node;
      typedef _Fwd_list_node_base                          _Node_base;
      typedef typename _Base::_Tp_alloc_type               _Tp_alloc_type;

    public:
      // types:
      typedef _Tp                                          value_type;
      typedef typename _Tp_alloc_type::pointer             pointer;
      typedef typename _Tp_alloc_type::const_pointer       const_pointer;
      typedef typename _Tp_alloc_type::reference           reference;
      typedef typename _Tp_alloc_type::const_reference     const_reference;
 
      typedef _Fwd_list_iterator<_Tp>                      iterator;
      typedef _Fwd_list_const_iterator<_Tp>                const_iterator;
      typedef std::size_t                                  size_type;
      typedef std::ptrdiff_t                               difference_type;
      typedef _Alloc                                       allocator_type;

      // 23.2.3.1 construct/copy/destroy:

      /**
       *  @brief  Creates a %forward_list with no elements.
       *  @param  al  An allocator object.
       */
      explicit
      forward_list(const _Alloc& __al = _Alloc())
      : _Base(__al)
      { }

      /**
       *  @brief  Copy constructor with allocator argument.
       *  @param  list  Input list to copy.
       *  @param  al    An allocator object.
       */
      forward_list(const forward_list& __list, const _Alloc& __al)
      : _Base(__list, __al)
      { }

      /**
       *  @brief  Move constructor with allocator argument.
       *  @param  list  Input list to move.
       *  @param  al    An allocator object.
       */
      forward_list(forward_list&& __list, const _Alloc& __al)
      : _Base(std::forward<_Base>(__list), __al)
      { }

      /**
       *  @brief  Creates a %forward_list with default constructed elements.
       *  @param  n  The number of elements to initially create.
       *
       *  This constructor creates the %forward_list with @a n default
       *  constructed elements.
       */
      explicit
      forward_list(size_type __n)
      : _Base()
      { _M_default_initialize(__n); }

      /**
       *  @brief  Creates a %forward_list with copies of an exemplar element.
       *  @param  n      The number of elements to initially create.
       *  @param  value  An element to copy.
       *  @param  al     An allocator object.
       *
       *  This constructor fills the %forward_list with @a n copies of @a
       *  value.
       */
      forward_list(size_type __n, const _Tp& __value,
                   const _Alloc& __al = _Alloc())
      : _Base(__al)
      { _M_fill_initialize(__n, __value); }

      /**
       *  @brief  Builds a %forward_list from a range.
       *  @param  first  An input iterator.
       *  @param  last   An input iterator.
       *  @param  al     An allocator object.
       *
       *  Create a %forward_list consisting of copies of the elements from
       *  [@a first,@a last).  This is linear in N (where N is
       *  distance(@a first,@a last)).
       */
      template<typename _InputIterator>
        forward_list(_InputIterator __first, _InputIterator __last,
                     const _Alloc& __al = _Alloc())
        : _Base(__al)
        {
          // Check whether it's an integral type.  If so, it's not an iterator.
          typedef typename std::__is_integer<_InputIterator>::__type _Integral;
          _M_initialize_dispatch(__first, __last, _Integral());
        }

      /**
       *  @brief  The %forward_list copy constructor.
       *  @param  list  A %forward_list of identical element and allocator
       *                types.
       *
       *  The newly-created %forward_list uses a copy of the allocation
       *  object used by @a list.
       */
      forward_list(const forward_list& __list)
      : _Base(__list._M_get_Node_allocator())
      { _M_initialize_dispatch(__list.begin(), __list.end(), __false_type()); }

      /**
       *  @brief  The %forward_list move constructor.
       *  @param  list  A %forward_list of identical element and allocator
       *                types.
       *
       *  The newly-created %forward_list contains the exact contents of @a
       *  forward_list. The contents of @a list are a valid, but unspecified
       *  %forward_list.
       */
      forward_list(forward_list&& __list)
      : _Base(std::forward<_Base>(__list)) { }

      /**
       *  @brief  Builds a %forward_list from an initializer_list
       *  @param  il  An initializer_list of value_type.
       *  @param  al  An allocator object.
       *
       *  Create a %forward_list consisting of copies of the elements
       *  in the initializer_list @a il.  This is linear in il.size().
       */
      forward_list(std::initializer_list<_Tp> __il,
                   const _Alloc& __al = _Alloc())
      : _Base(__al)
      { _M_initialize_dispatch(__il.begin(), __il.end(), __false_type()); }

      /**
       *  @brief  The forward_list dtor.
       */
      ~forward_list()
      { }

      /**
       *  @brief  The %forward_list assignment operator.
       *  @param  list  A %forward_list of identical element and allocator
       *                types.
       *
       *  All the elements of @a list are copied, but unlike the copy
       *  constructor, the allocator object is not copied.
       */
      forward_list&
      operator=(const forward_list& __list);

      /**
       *  @brief  The %forward_list move assignment operator.
       *  @param  list  A %forward_list of identical element and allocator
       *                types.
       *
       *  The contents of @a list are moved into this %forward_list
       *  (without copying). @a list is a valid, but unspecified
       *  %forward_list
       */
      forward_list&
      operator=(forward_list&& __list)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__list);
	return *this;
      }

      /**
       *  @brief  The %forward_list initializer list assignment operator.
       *  @param  il  An initializer_list of value_type.
       *
       *  Replace the contents of the %forward_list with copies of the
       *  elements in the initializer_list @a il.  This is linear in
       *  il.size().
       */
      forward_list&
      operator=(std::initializer_list<_Tp> __il)
      {
        assign(__il);
        return *this;
      }

      /**
       *  @brief  Assigns a range to a %forward_list.
       *  @param  first  An input iterator.
       *  @param  last   An input iterator.
       *
       *  This function fills a %forward_list with copies of the elements
       *  in the range [@a first,@a last).
       *
       *  Note that the assignment completely changes the %forward_list and
       *  that the resulting %forward_list's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
      template<typename _InputIterator>
        void
        assign(_InputIterator __first, _InputIterator __last)
        {
          clear();
          insert_after(cbefore_begin(), __first, __last);
        }

      /**
       *  @brief  Assigns a given value to a %forward_list.
       *  @param  n  Number of elements to be assigned.
       *  @param  val  Value to be assigned.
       *
       *  This function fills a %forward_list with @a n copies of the given
       *  value.  Note that the assignment completely changes the
       *  %forward_list and that the resulting %forward_list's size is the
       *  same as the number of elements assigned.  Old data may be lost.
       */
      void
      assign(size_type __n, const _Tp& __val)
      {
        clear();
        insert_after(cbefore_begin(), __n, __val);
      }

      /**
       *  @brief  Assigns an initializer_list to a %forward_list.
       *  @param  il  An initializer_list of value_type.
       *
       *  Replace the contents of the %forward_list with copies of the
       *  elements in the initializer_list @a il.  This is linear in
       *  il.size().
       */
      void
      assign(std::initializer_list<_Tp> __il)
      {
        clear();
        insert_after(cbefore_begin(), __il);
      }

      /// Get a copy of the memory allocation object.
      allocator_type
      get_allocator() const
      { return this->_M_get_Node_allocator(); }

      // 23.2.3.2 iterators:

      /**
       *  Returns a read/write iterator that points before the first element
       *  in the %forward_list.  Iteration is done in ordinary element order.
       */
      iterator
      before_begin()
      { return iterator(&this->_M_impl._M_head); }

      /**
       *  Returns a read-only (constant) iterator that points before the
       *  first element in the %forward_list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      before_begin() const
      { return const_iterator(&this->_M_impl._M_head); }

      /**
       *  Returns a read/write iterator that points to the first element
       *  in the %forward_list.  Iteration is done in ordinary element order.
       */
      iterator
      begin()
      { return iterator(this->_M_impl._M_head._M_next); }

      /**
       *  Returns a read-only (constant) iterator that points to the first
       *  element in the %forward_list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      begin() const
      { return const_iterator(this->_M_impl._M_head._M_next); }

      /**
       *  Returns a read/write iterator that points one past the last
       *  element in the %forward_list.  Iteration is done in ordinary
       *  element order.
       */
      iterator
      end()
      { return iterator(0); }

      /**
       *  Returns a read-only iterator that points one past the last
       *  element in the %forward_list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      end() const
      { return const_iterator(0); }

      /**
       *  Returns a read-only (constant) iterator that points to the
       *  first element in the %forward_list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      cbegin() const
      { return const_iterator(this->_M_impl._M_head._M_next); }

      /**
       *  Returns a read-only (constant) iterator that points before the
       *  first element in the %forward_list.  Iteration is done in ordinary
       *  element order.
       */
      const_iterator
      cbefore_begin() const
      { return const_iterator(&this->_M_impl._M_head); }

      /**
       *  Returns a read-only (constant) iterator that points one past
       *  the last element in the %forward_list.  Iteration is done in
       *  ordinary element order.
       */
      const_iterator
      cend() const
      { return const_iterator(0); }

      /**
       *  Returns true if the %forward_list is empty.  (Thus begin() would
       *  equal end().)
       */
      bool
      empty() const
      { return this->_M_impl._M_head._M_next == 0; }

      /**
       *  Returns the largest possible size of %forward_list.
       */
      size_type
      max_size() const
      { return this->_M_get_Node_allocator().max_size(); }

      // 23.2.3.3 element access:

      /**
       *  Returns a read/write reference to the data at the first
       *  element of the %forward_list.
       */
      reference
      front()
      {
        _Node* __front = static_cast<_Node*>(this->_M_impl._M_head._M_next);
        return __front->_M_value;
      }

      /**
       *  Returns a read-only (constant) reference to the data at the first
       *  element of the %forward_list.
       */
      const_reference
      front() const
      {
        _Node* __front = static_cast<_Node*>(this->_M_impl._M_head._M_next);
        return __front->_M_value;
      }

      // 23.2.3.4 modiﬁers:

      /**
       *  @brief  Constructs object in %forward_list at the front of the
       *          list.
       *  @param  args  Arguments.
       *
       *  This function will insert an object of type Tp constructed
       *  with Tp(std::forward<Args>(args)...) at the front of the list
       *  Due to the nature of a %forward_list this operation can
       *  be done in constant time, and does not invalidate iterators
       *  and references.
       */
      template<typename... _Args>
        void
        emplace_front(_Args&&... __args)
        { this->_M_insert_after(cbefore_begin(),
                                std::forward<_Args>(__args)...); }

      /**
       *  @brief  Add data to the front of the %forward_list.
       *  @param  val  Data to be added.
       *
       *  This is a typical stack operation.  The function creates an
       *  element at the front of the %forward_list and assigns the given
       *  data to it.  Due to the nature of a %forward_list this operation
       *  can be done in constant time, and does not invalidate iterators
       *  and references.
       */
      void
      push_front(const _Tp& __val)
      { this->_M_insert_after(cbefore_begin(), __val); }

      /**
       *
       */
      void
      push_front(_Tp&& __val)
      { this->_M_insert_after(cbefore_begin(), std::move(__val)); }

      /**
       *  @brief  Removes first element.
       *
       *  This is a typical stack operation.  It shrinks the %forward_list
       *  by one.  Due to the nature of a %forward_list this operation can
       *  be done in constant time, and only invalidates iterators/references
       *  to the element being removed.
       *
       *  Note that no data is returned, and if the first element's data
       *  is needed, it should be retrieved before pop_front() is
       *  called.
       */
      void
      pop_front()
      { this->_M_erase_after(&this->_M_impl._M_head); }

      /**
       *  @brief  Constructs object in %forward_list after the specified
       *          iterator.
       *  @param  pos  A const_iterator into the %forward_list.
       *  @param  args  Arguments.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert an object of type T constructed
       *  with T(std::forward<Args>(args)...) after the specified
       *  location.  Due to the nature of a %forward_list this operation can
       *  be done in constant time, and does not invalidate iterators
       *  and references.
       */
      template<typename... _Args>
        iterator
        emplace_after(const_iterator __pos, _Args&&... __args)
        { return iterator(this->_M_insert_after(__pos,
                                          std::forward<_Args>(__args)...)); }

      /**
       *  @brief  Inserts given value into %forward_list after specified
       *          iterator.
       *  @param  pos  An iterator into the %forward_list.
       *  @param  val  Data to be inserted.
       *  @return  An iterator that points to the inserted data.
       *
       *  This function will insert a copy of the given value after
       *  the specified location.  Due to the nature of a %forward_list this
       *  operation can be done in constant time, and does not
       *  invalidate iterators and references.
       */
      iterator
      insert_after(const_iterator __pos, const _Tp& __val)
      { return iterator(this->_M_insert_after(__pos, __val)); }

      /**
       *
       */
      iterator
      insert_after(const_iterator __pos, _Tp&& __val)
      { return iterator(this->_M_insert_after(__pos, std::move(__val))); }

      /**
       *  @brief  Inserts a number of copies of given data into the
       *          %forward_list.
       *  @param  pos  An iterator into the %forward_list.
       *  @param  n  Number of elements to be inserted.
       *  @param  val  Data to be inserted.
       *  @return  An iterator pointing to the last inserted copy of
       *           @a val or @a pos if @a n == 0.
       *
       *  This function will insert a specified number of copies of the
       *  given data after the location specified by @a pos.
       *
       *  This operation is linear in the number of elements inserted and
       *  does not invalidate iterators and references.
       */
      iterator
      insert_after(const_iterator __pos, size_type __n, const _Tp& __val);

      /**
       *  @brief  Inserts a range into the %forward_list.
       *  @param  position  An iterator into the %forward_list.
       *  @param  first  An input iterator.
       *  @param  last   An input iterator.
       *  @return  An iterator pointing to the last inserted element or
       *           @a pos if @a first == @a last.
       *
       *  This function will insert copies of the data in the range [@a
       *  first,@a last) into the %forward_list after the location specified
       *  by @a pos.
       *
       *  This operation is linear in the number of elements inserted and
       *  does not invalidate iterators and references.
       */
      template<typename _InputIterator>
        iterator
        insert_after(const_iterator __pos,
                     _InputIterator __first, _InputIterator __last);

      /**
       *  @brief  Inserts the contents of an initializer_list into
       *          %forward_list after the specified iterator.
       *  @param  pos  An iterator into the %forward_list.
       *  @param  il  An initializer_list of value_type.
       *  @return  An iterator pointing to the last inserted element
       *           or @a pos if @a il is empty.
       *
       *  This function will insert copies of the data in the
       *  initializer_list @a il into the %forward_list before the location
       *  specified by @a pos.
       *
       *  This operation is linear in the number of elements inserted and
       *  does not invalidate iterators and references.
       */
      iterator
      insert_after(const_iterator __pos, std::initializer_list<_Tp> __il);

      /**
       *  @brief  Removes the element pointed to by the iterator following
       *          @c pos.
       *  @param  pos  Iterator pointing before element to be erased.
       *
       *  This function will erase the element at the given position and
       *  thus shorten the %forward_list by one.
       *
       *  Due to the nature of a %forward_list this operation can be done
       *  in constant time, and only invalidates iterators/references to
       *  the element being removed.  The user is also cautioned that
       *  this function only erases the element, and that if the element
       *  is itself a pointer, the pointed-to memory is not touched in
       *  any way.  Managing the pointer is the user's responsibility.
       */
      void
      erase_after(const_iterator __pos)
      { this->_M_erase_after(const_cast<_Node_base*>(__pos._M_node)); }

      /**
       *  @brief  Remove a range of elements.
       *  @param  pos  Iterator pointing before the first element to be
       *               erased.
       *  @param  last  Iterator pointing to one past the last element to be
       *                erased.
       *
       *  This function will erase the elements in the range @a
       *  (pos,last) and shorten the %forward_list accordingly.
       *
       *  This operation is linear time in the size of the range and only
       *  invalidates iterators/references to the element being removed.
       *  The user is also cautioned that this function only erases the
       *  elements, and that if the elements themselves are pointers, the
       *  pointed-to memory is not touched in any way.  Managing the pointer
       *  is the user's responsibility.
       */
      void
      erase_after(const_iterator __pos, const_iterator __last)
      { this->_M_erase_after(const_cast<_Node_base*>(__pos._M_node),
			     const_cast<_Node_base*>(__last._M_node)); }

      /**
       *  @brief  Swaps data with another %forward_list.
       *  @param  list  A %forward_list of the same element and allocator
       *                types.
       *
       *  This exchanges the elements between two lists in constant
       *  time.  Note that the global std::swap() function is
       *  specialized such that std::swap(l1,l2) will feed to this
       *  function.
       */
      void
      swap(forward_list& __list)
      { _Node_base::swap(this->_M_impl._M_head, __list._M_impl._M_head); }

      /**
       *  @brief Resizes the %forward_list to the specified number of
       *         elements.
       *  @param sz Number of elements the %forward_list should contain.
       *
       *  This function will %resize the %forward_list to the specified
       *  number of elements.  If the number is smaller than the
       *  %forward_list's current size the %forward_list is truncated,
       *  otherwise the %forward_list is extended and the new elements
       *  are default constructed.
       */
      void
      resize(size_type __sz);

      /**
       *  @brief Resizes the %forward_list to the specified number of
       *         elements.
       *  @param sz Number of elements the %forward_list should contain.
       *  @param val Data with which new elements should be populated.
       *
       *  This function will %resize the %forward_list to the specified
       *  number of elements.  If the number is smaller than the
       *  %forward_list's current size the %forward_list is truncated,
       *  otherwise the %forward_list is extended and new elements are
       *  populated with given data.
       */
      void
      resize(size_type __sz, value_type __val);

      /**
       *  @brief  Erases all the elements.
       *
       *  Note that this function only erases
       *  the elements, and that if the elements themselves are
       *  pointers, the pointed-to memory is not touched in any way.
       *  Managing the pointer is the user's responsibility.
       */
      void
      clear()
      { this->_M_erase_after(&this->_M_impl._M_head, 0); }

      // 23.2.3.5 forward_list operations:

      /**
       *  @brief  Insert contents of another %forward_list.
       *  @param  pos  Iterator referencing the element to insert after.
       *  @param  list  Source list.
       *
       *  The elements of @a list are inserted in constant time after
       *  the element referenced by @a pos.  @a list becomes an empty
       *  list.
       *
       *  Requires this != @a x.
       */
      void
      splice_after(const_iterator __pos, forward_list&& __list)
      {
	if (!__list.empty())
	  _M_splice_after(__pos, std::move(__list));
      }

      /**
       *  @brief  Insert element from another %forward_list.
       *  @param  pos  Iterator referencing the element to insert after.
       *  @param  list  Source list.
       *  @param  i   Iterator referencing the element before the element
       *              to move.
       *
       *  Removes the element in list @a list referenced by @a i and
       *  inserts it into the current list after @a pos.
       */
      void
      splice_after(const_iterator __pos, forward_list&& __list,
                   const_iterator __i)
      {
	const_iterator __j = __i;
	++__j;
	if (__pos == __i || __pos == __j)
	  return;

	splice_after(__pos, std::move(__list), __i, __j);
      }

      /**
       *  @brief  Insert range from another %forward_list.
       *  @param  pos  Iterator referencing the element to insert after.
       *  @param  list  Source list.
       *  @param  before  Iterator referencing before the start of range
       *                  in list.
       *  @param  last  Iterator referencing the end of range in list.
       *
       *  Removes elements in the range (before,last) and inserts them
       *  after @a pos in constant time.
       *
       *  Undefined if @a pos is in (before,last).
       */
      void
      splice_after(const_iterator __pos, forward_list&& __list,
                   const_iterator __before, const_iterator __last);

      /**
       *  @brief  Remove all elements equal to value.
       *  @param  val  The value to remove.
       *
       *  Removes every element in the list equal to @a value.
       *  Remaining elements stay in list order.  Note that this
       *  function only erases the elements, and that if the elements
       *  themselves are pointers, the pointed-to memory is not
       *  touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      void
      remove(const _Tp& __val);

      /**
       *  @brief  Remove all elements satisfying a predicate.
       *  @param  pred  Unary predicate function or object.
       *
       *  Removes every element in the list for which the predicate
       *  returns true.  Remaining elements stay in list order.  Note
       *  that this function only erases the elements, and that if the
       *  elements themselves are pointers, the pointed-to memory is
       *  not touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      template<typename _Pred>
        void
        remove_if(_Pred __pred);

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
      unique()
      { this->unique(std::equal_to<_Tp>()); }

      /**
       *  @brief  Remove consecutive elements satisfying a predicate.
       *  @param  binary_pred  Binary predicate function or object.
       *
       *  For each consecutive set of elements [first,last) that
       *  satisfy predicate(first,i) where i is an iterator in
       *  [first,last), remove all but the first one.  Remaining
       *  elements stay in list order.  Note that this function only
       *  erases the elements, and that if the elements themselves are
       *  pointers, the pointed-to memory is not touched in any way.
       *  Managing the pointer is the user's responsibility.
       */
      template<typename _BinPred>
        void
        unique(_BinPred __binary_pred);

      /**
       *  @brief  Merge sorted lists.
       *  @param  list  Sorted list to merge.
       *
       *  Assumes that both @a list and this list are sorted according to
       *  operator<().  Merges elements of @a list into this list in
       *  sorted order, leaving @a list empty when complete.  Elements in
       *  this list precede elements in @a list that are equal.
       */
      void
      merge(forward_list&& __list)
      { this->merge(std::move(__list), std::less<_Tp>()); }

      /**
       *  @brief  Merge sorted lists according to comparison function.
       *  @param  list  Sorted list to merge.
       *  @param  comp Comparison function defining sort order.
       *
       *  Assumes that both @a list and this list are sorted according to
       *  comp.  Merges elements of @a list into this list
       *  in sorted order, leaving @a list empty when complete.  Elements
       *  in this list precede elements in @a list that are equivalent
       *  according to comp().
       */
      template<typename _Comp>
        void
        merge(forward_list&& __list, _Comp __comp);

      /**
       *  @brief  Sort the elements of the list.
       *
       *  Sorts the elements of this list in NlogN time.  Equivalent
       *  elements remain in list order.
       */
      void
      sort()
      { this->sort(std::less<_Tp>()); }

      /**
       *  @brief  Sort the forward_list using a comparison function.
       *
       *  Sorts the elements of this list in NlogN time.  Equivalent
       *  elements remain in list order.
       */
      template<typename _Comp>
        void
        sort(_Comp __comp);

      /**
       *  @brief  Reverse the elements in list.
       *
       *  Reverse the order of elements in the list in linear time.
       */
      void
      reverse()
      { this->_M_impl._M_head._M_reverse_after(); }

    private:
      template<typename _Integer>
        void
        _M_initialize_dispatch(_Integer __n, _Integer __x, __true_type)
        { _M_fill_initialize(static_cast<size_type>(__n), __x); }

      // Called by the range constructor to implement [23.1.1]/9
      template<typename _InputIterator>
        void
        _M_initialize_dispatch(_InputIterator __first, _InputIterator __last,
                               __false_type);

      // Called by forward_list(n,v,a), and the range constructor when it
      // turns out to be the same thing.
      void
      _M_fill_initialize(size_type __n, const value_type& __value);

      // Called by splice_after and insert_after.
      iterator
      _M_splice_after(const_iterator __pos, forward_list&& __list);

      // Called by forward_list(n).
      void
      _M_default_initialize(size_type __n);

      // Called by resize(sz).
      void
      _M_default_insert_after(const_iterator __pos, size_type __n);
    };

  /**
   *  @brief  Forward list equality comparison.
   *  @param  lx  A %forward_list
   *  @param  ly  A %forward_list of the same type as @a lx.
   *  @return  True iff the size and elements of the forward lists are equal.
   *
   *  This is an equivalence relation.  It is linear in the size of the
   *  forward lists.  Deques are considered equivalent if corresponding
   *  elements compare equal.
   */
  template<typename _Tp, typename _Alloc>
    bool
    operator==(const forward_list<_Tp, _Alloc>& __lx,
               const forward_list<_Tp, _Alloc>& __ly);

  /**
   *  @brief  Forward list ordering relation.
   *  @param  lx  A %forward_list.
   *  @param  ly  A %forward_list of the same type as @a lx.
   *  @return  True iff @a lx is lexicographically less than @a ly.
   *
   *  This is a total ordering relation.  It is linear in the size of the
   *  forward lists.  The elements must be comparable with @c <.
   *
   *  See std::lexicographical_compare() for how the determination is made.
   */
  template<typename _Tp, typename _Alloc>
    inline bool
    operator<(const forward_list<_Tp, _Alloc>& __lx,
              const forward_list<_Tp, _Alloc>& __ly)
    { return std::lexicographical_compare(__lx.cbegin(), __lx.cend(),
					  __ly.cbegin(), __ly.cend()); }

  /// Based on operator==
  template<typename _Tp, typename _Alloc>
    inline bool
    operator!=(const forward_list<_Tp, _Alloc>& __lx,
               const forward_list<_Tp, _Alloc>& __ly)
    { return !(__lx == __ly); }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator>(const forward_list<_Tp, _Alloc>& __lx,
              const forward_list<_Tp, _Alloc>& __ly)
    { return (__ly < __lx); }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator>=(const forward_list<_Tp, _Alloc>& __lx,
               const forward_list<_Tp, _Alloc>& __ly)
    { return !(__lx < __ly); }

  /// Based on operator<
  template<typename _Tp, typename _Alloc>
    inline bool
    operator<=(const forward_list<_Tp, _Alloc>& __lx,
               const forward_list<_Tp, _Alloc>& __ly)
    { return !(__ly < __lx); }

  /// See std::forward_list::swap().
  template<typename _Tp, typename _Alloc>
    inline void
    swap(forward_list<_Tp, _Alloc>& __lx,
	 forward_list<_Tp, _Alloc>& __ly)
    { __lx.swap(__ly); }

_GLIBCXX_END_NAMESPACE // namespace std

#endif // _FORWARD_LIST_H
