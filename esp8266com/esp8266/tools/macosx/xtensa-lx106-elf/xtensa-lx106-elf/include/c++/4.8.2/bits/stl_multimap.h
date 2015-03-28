// Multimap implementation -*- C++ -*-

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

/** @file bits/stl_multimap.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{map}
 */

#ifndef _STL_MULTIMAP_H
#define _STL_MULTIMAP_H 1

#include <bits/concept_check.h>
#if __cplusplus >= 201103L
#include <initializer_list>
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_CONTAINER

  /**
   *  @brief A standard container made up of (key,value) pairs, which can be
   *  retrieved based on a key, in logarithmic time.
   *
   *  @ingroup associative_containers
   *
   *  @tparam _Key  Type of key objects.
   *  @tparam  _Tp  Type of mapped objects.
   *  @tparam _Compare  Comparison function object type, defaults to less<_Key>.
   *  @tparam _Alloc  Allocator type, defaults to 
   *                  allocator<pair<const _Key, _Tp>.
   *
   *  Meets the requirements of a <a href="tables.html#65">container</a>, a
   *  <a href="tables.html#66">reversible container</a>, and an
   *  <a href="tables.html#69">associative container</a> (using equivalent
   *  keys).  For a @c multimap<Key,T> the key_type is Key, the mapped_type
   *  is T, and the value_type is std::pair<const Key,T>.
   *
   *  Multimaps support bidirectional iterators.
   *
   *  The private tree data is declared exactly the same way for map and
   *  multimap; the distinction is made entirely in how the tree functions are
   *  called (*_unique versus *_equal, same as the standard).
  */
  template <typename _Key, typename _Tp,
	    typename _Compare = std::less<_Key>,
	    typename _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
    class multimap
    {
    public:
      typedef _Key                                          key_type;
      typedef _Tp                                           mapped_type;
      typedef std::pair<const _Key, _Tp>                    value_type;
      typedef _Compare                                      key_compare;
      typedef _Alloc                                        allocator_type;

    private:
      // concept requirements
      typedef typename _Alloc::value_type                   _Alloc_value_type;
      __glibcxx_class_requires(_Tp, _SGIAssignableConcept)
      __glibcxx_class_requires4(_Compare, bool, _Key, _Key,
				_BinaryFunctionConcept)
      __glibcxx_class_requires2(value_type, _Alloc_value_type, _SameTypeConcept)

    public:
      class value_compare
      : public std::binary_function<value_type, value_type, bool>
      {
	friend class multimap<_Key, _Tp, _Compare, _Alloc>;
      protected:
	_Compare comp;

	value_compare(_Compare __c)
	: comp(__c) { }

      public:
	bool operator()(const value_type& __x, const value_type& __y) const
	{ return comp(__x.first, __y.first); }
      };

    private:
      /// This turns a red-black tree into a [multi]map.
      typedef typename _Alloc::template rebind<value_type>::other 
        _Pair_alloc_type;

      typedef _Rb_tree<key_type, value_type, _Select1st<value_type>,
		       key_compare, _Pair_alloc_type> _Rep_type;
      /// The actual tree structure.
      _Rep_type _M_t;

    public:
      // many of these are specified differently in ISO, but the following are
      // "functionally equivalent"
      typedef typename _Pair_alloc_type::pointer         pointer;
      typedef typename _Pair_alloc_type::const_pointer   const_pointer;
      typedef typename _Pair_alloc_type::reference       reference;
      typedef typename _Pair_alloc_type::const_reference const_reference;
      typedef typename _Rep_type::iterator               iterator;
      typedef typename _Rep_type::const_iterator         const_iterator;
      typedef typename _Rep_type::size_type              size_type;
      typedef typename _Rep_type::difference_type        difference_type;
      typedef typename _Rep_type::reverse_iterator       reverse_iterator;
      typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;

      // [23.3.2] construct/copy/destroy
      // (get_allocator() is also listed in this section)
      /**
       *  @brief  Default constructor creates no elements.
       */
      multimap()
      : _M_t() { }

      /**
       *  @brief  Creates a %multimap with no elements.
       *  @param  __comp  A comparison object.
       *  @param  __a  An allocator object.
       */
      explicit
      multimap(const _Compare& __comp,
	       const allocator_type& __a = allocator_type())
      : _M_t(__comp, _Pair_alloc_type(__a)) { }

      /**
       *  @brief  %Multimap copy constructor.
       *  @param  __x  A %multimap of identical element and allocator types.
       *
       *  The newly-created %multimap uses a copy of the allocation object
       *  used by @a __x.
       */
      multimap(const multimap& __x)
      : _M_t(__x._M_t) { }

#if __cplusplus >= 201103L
      /**
       *  @brief  %Multimap move constructor.
       *  @param   __x  A %multimap of identical element and allocator types.
       *
       *  The newly-created %multimap contains the exact contents of @a __x.
       *  The contents of @a __x are a valid, but unspecified %multimap.
       */
      multimap(multimap&& __x)
      noexcept(is_nothrow_copy_constructible<_Compare>::value)
      : _M_t(std::move(__x._M_t)) { }

      /**
       *  @brief  Builds a %multimap from an initializer_list.
       *  @param  __l  An initializer_list.
       *  @param  __comp  A comparison functor.
       *  @param  __a  An allocator object.
       *
       *  Create a %multimap consisting of copies of the elements from
       *  the initializer_list.  This is linear in N if the list is already
       *  sorted, and NlogN otherwise (where N is @a __l.size()).
       */
      multimap(initializer_list<value_type> __l,
	       const _Compare& __comp = _Compare(),
	       const allocator_type& __a = allocator_type())
      : _M_t(__comp, _Pair_alloc_type(__a))
      { _M_t._M_insert_equal(__l.begin(), __l.end()); }
#endif

      /**
       *  @brief  Builds a %multimap from a range.
       *  @param  __first  An input iterator.
       *  @param  __last  An input iterator.
       *
       *  Create a %multimap consisting of copies of the elements from
       *  [__first,__last).  This is linear in N if the range is already sorted,
       *  and NlogN otherwise (where N is distance(__first,__last)).
       */
      template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last)
	: _M_t()
        { _M_t._M_insert_equal(__first, __last); }

      /**
       *  @brief  Builds a %multimap from a range.
       *  @param  __first  An input iterator.
       *  @param  __last  An input iterator.
       *  @param  __comp  A comparison functor.
       *  @param  __a  An allocator object.
       *
       *  Create a %multimap consisting of copies of the elements from
       *  [__first,__last).  This is linear in N if the range is already sorted,
       *  and NlogN otherwise (where N is distance(__first,__last)).
       */
      template<typename _InputIterator>
        multimap(_InputIterator __first, _InputIterator __last,
		 const _Compare& __comp,
		 const allocator_type& __a = allocator_type())
	: _M_t(__comp, _Pair_alloc_type(__a))
        { _M_t._M_insert_equal(__first, __last); }

      // FIXME There is no dtor declared, but we should have something generated
      // by Doxygen.  I don't know what tags to add to this paragraph to make
      // that happen:
      /**
       *  The dtor only erases the elements, and note that if the elements
       *  themselves are pointers, the pointed-to memory is not touched in any
       *  way.  Managing the pointer is the user's responsibility.
       */

      /**
       *  @brief  %Multimap assignment operator.
       *  @param  __x  A %multimap of identical element and allocator types.
       *
       *  All the elements of @a __x are copied, but unlike the copy
       *  constructor, the allocator object is not copied.
       */
      multimap&
      operator=(const multimap& __x)
      {
	_M_t = __x._M_t;
	return *this;
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  %Multimap move assignment operator.
       *  @param  __x  A %multimap of identical element and allocator types.
       *
       *  The contents of @a __x are moved into this multimap (without copying).
       *  @a __x is a valid, but unspecified multimap.
       */
      multimap&
      operator=(multimap&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__x);
	return *this;
      }

      /**
       *  @brief  %Multimap list assignment operator.
       *  @param  __l  An initializer_list.
       *
       *  This function fills a %multimap with copies of the elements
       *  in the initializer list @a __l.
       *
       *  Note that the assignment completely changes the %multimap and
       *  that the resulting %multimap's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
      multimap&
      operator=(initializer_list<value_type> __l)
      {
	this->clear();
	this->insert(__l.begin(), __l.end());
	return *this;
      }
#endif

      /// Get a copy of the memory allocation object.
      allocator_type
      get_allocator() const _GLIBCXX_NOEXCEPT 
      { return allocator_type(_M_t.get_allocator()); }

      // iterators
      /**
       *  Returns a read/write iterator that points to the first pair in the
       *  %multimap.  Iteration is done in ascending order according to the
       *  keys.
       */
      iterator
      begin() _GLIBCXX_NOEXCEPT
      { return _M_t.begin(); }

      /**
       *  Returns a read-only (constant) iterator that points to the first pair
       *  in the %multimap.  Iteration is done in ascending order according to
       *  the keys.
       */
      const_iterator
      begin() const _GLIBCXX_NOEXCEPT
      { return _M_t.begin(); }

      /**
       *  Returns a read/write iterator that points one past the last pair in
       *  the %multimap.  Iteration is done in ascending order according to the
       *  keys.
       */
      iterator
      end() _GLIBCXX_NOEXCEPT
      { return _M_t.end(); }

      /**
       *  Returns a read-only (constant) iterator that points one past the last
       *  pair in the %multimap.  Iteration is done in ascending order according
       *  to the keys.
       */
      const_iterator
      end() const _GLIBCXX_NOEXCEPT
      { return _M_t.end(); }

      /**
       *  Returns a read/write reverse iterator that points to the last pair in
       *  the %multimap.  Iteration is done in descending order according to the
       *  keys.
       */
      reverse_iterator
      rbegin() _GLIBCXX_NOEXCEPT
      { return _M_t.rbegin(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to the
       *  last pair in the %multimap.  Iteration is done in descending order
       *  according to the keys.
       */
      const_reverse_iterator
      rbegin() const _GLIBCXX_NOEXCEPT
      { return _M_t.rbegin(); }

      /**
       *  Returns a read/write reverse iterator that points to one before the
       *  first pair in the %multimap.  Iteration is done in descending order
       *  according to the keys.
       */
      reverse_iterator
      rend() _GLIBCXX_NOEXCEPT
      { return _M_t.rend(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to one
       *  before the first pair in the %multimap.  Iteration is done in
       *  descending order according to the keys.
       */
      const_reverse_iterator
      rend() const _GLIBCXX_NOEXCEPT
      { return _M_t.rend(); }

#if __cplusplus >= 201103L
      /**
       *  Returns a read-only (constant) iterator that points to the first pair
       *  in the %multimap.  Iteration is done in ascending order according to
       *  the keys.
       */
      const_iterator
      cbegin() const noexcept
      { return _M_t.begin(); }

      /**
       *  Returns a read-only (constant) iterator that points one past the last
       *  pair in the %multimap.  Iteration is done in ascending order according
       *  to the keys.
       */
      const_iterator
      cend() const noexcept
      { return _M_t.end(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to the
       *  last pair in the %multimap.  Iteration is done in descending order
       *  according to the keys.
       */
      const_reverse_iterator
      crbegin() const noexcept
      { return _M_t.rbegin(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to one
       *  before the first pair in the %multimap.  Iteration is done in
       *  descending order according to the keys.
       */
      const_reverse_iterator
      crend() const noexcept
      { return _M_t.rend(); }
#endif

      // capacity
      /** Returns true if the %multimap is empty.  */
      bool
      empty() const _GLIBCXX_NOEXCEPT
      { return _M_t.empty(); }

      /** Returns the size of the %multimap.  */
      size_type
      size() const _GLIBCXX_NOEXCEPT
      { return _M_t.size(); }

      /** Returns the maximum size of the %multimap.  */
      size_type
      max_size() const _GLIBCXX_NOEXCEPT
      { return _M_t.max_size(); }

      // modifiers
#if __cplusplus >= 201103L
      /**
       *  @brief Build and insert a std::pair into the %multimap.
       *
       *  @param __args  Arguments used to generate a new pair instance (see
       *	        std::piecewise_contruct for passing arguments to each
       *	        part of the pair constructor).
       *
       *  @return An iterator that points to the inserted (key,value) pair.
       *
       *  This function builds and inserts a (key, value) %pair into the
       *  %multimap.
       *  Contrary to a std::map the %multimap does not rely on unique keys and
       *  thus multiple pairs with the same key can be inserted.
       *
       *  Insertion requires logarithmic time.
       */
      template<typename... _Args>
	iterator
	emplace(_Args&&... __args)
	{ return _M_t._M_emplace_equal(std::forward<_Args>(__args)...); }

      /**
       *  @brief Builds and inserts a std::pair into the %multimap.
       *
       *  @param  __pos  An iterator that serves as a hint as to where the pair
       *                should be inserted.
       *  @param  __args  Arguments used to generate a new pair instance (see
       *	         std::piecewise_contruct for passing arguments to each
       *	         part of the pair constructor).
       *  @return An iterator that points to the inserted (key,value) pair.
       *
       *  This function inserts a (key, value) pair into the %multimap.
       *  Contrary to a std::map the %multimap does not rely on unique keys and
       *  thus multiple pairs with the same key can be inserted.
       *  Note that the first parameter is only a hint and can potentially
       *  improve the performance of the insertion process.  A bad hint would
       *  cause no gains in efficiency.
       *
       *  For more on @a hinting, see:
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt07ch17.html
       *
       *  Insertion requires logarithmic time (if the hint is not taken).
       */
      template<typename... _Args>
	iterator
	emplace_hint(const_iterator __pos, _Args&&... __args)
	{
	  return _M_t._M_emplace_hint_equal(__pos,
					    std::forward<_Args>(__args)...);
	}
#endif

      /**
       *  @brief Inserts a std::pair into the %multimap.
       *  @param  __x  Pair to be inserted (see std::make_pair for easy creation
       *             of pairs).
       *  @return An iterator that points to the inserted (key,value) pair.
       *
       *  This function inserts a (key, value) pair into the %multimap.
       *  Contrary to a std::map the %multimap does not rely on unique keys and
       *  thus multiple pairs with the same key can be inserted.
       *
       *  Insertion requires logarithmic time.
       */
      iterator
      insert(const value_type& __x)
      { return _M_t._M_insert_equal(__x); }

#if __cplusplus >= 201103L
      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
        iterator
        insert(_Pair&& __x)
        { return _M_t._M_insert_equal(std::forward<_Pair>(__x)); }
#endif

      /**
       *  @brief Inserts a std::pair into the %multimap.
       *  @param  __position  An iterator that serves as a hint as to where the
       *                      pair should be inserted.
       *  @param  __x  Pair to be inserted (see std::make_pair for easy creation
       *               of pairs).
       *  @return An iterator that points to the inserted (key,value) pair.
       *
       *  This function inserts a (key, value) pair into the %multimap.
       *  Contrary to a std::map the %multimap does not rely on unique keys and
       *  thus multiple pairs with the same key can be inserted.
       *  Note that the first parameter is only a hint and can potentially
       *  improve the performance of the insertion process.  A bad hint would
       *  cause no gains in efficiency.
       *
       *  For more on @a hinting, see:
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt07ch17.html
       *
       *  Insertion requires logarithmic time (if the hint is not taken).
       */
      iterator
#if __cplusplus >= 201103L
      insert(const_iterator __position, const value_type& __x)
#else
      insert(iterator __position, const value_type& __x)
#endif
      { return _M_t._M_insert_equal_(__position, __x); }

#if __cplusplus >= 201103L
      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
        iterator
        insert(const_iterator __position, _Pair&& __x)
        { return _M_t._M_insert_equal_(__position,
				       std::forward<_Pair>(__x)); }
#endif

      /**
       *  @brief A template function that attempts to insert a range
       *  of elements.
       *  @param  __first  Iterator pointing to the start of the range to be
       *                   inserted.
       *  @param  __last  Iterator pointing to the end of the range.
       *
       *  Complexity similar to that of the range constructor.
       */
      template<typename _InputIterator>
        void
        insert(_InputIterator __first, _InputIterator __last)
        { _M_t._M_insert_equal(__first, __last); }

#if __cplusplus >= 201103L
      /**
       *  @brief Attempts to insert a list of std::pairs into the %multimap.
       *  @param  __l  A std::initializer_list<value_type> of pairs to be
       *               inserted.
       *
       *  Complexity similar to that of the range constructor.
       */
      void
      insert(initializer_list<value_type> __l)
      { this->insert(__l.begin(), __l.end()); }
#endif

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 130. Associative erase should return an iterator.
      /**
       *  @brief Erases an element from a %multimap.
       *  @param  __position  An iterator pointing to the element to be erased.
       *  @return An iterator pointing to the element immediately following
       *          @a position prior to the element being erased. If no such 
       *          element exists, end() is returned.
       *
       *  This function erases an element, pointed to by the given iterator,
       *  from a %multimap.  Note that this function only erases the element,
       *  and that if the element is itself a pointer, the pointed-to memory is
       *  not touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      iterator
      erase(const_iterator __position)
      { return _M_t.erase(__position); }

      // LWG 2059.
      _GLIBCXX_ABI_TAG_CXX11
      iterator
      erase(iterator __position)
      { return _M_t.erase(__position); }
#else
      /**
       *  @brief Erases an element from a %multimap.
       *  @param  __position  An iterator pointing to the element to be erased.
       *
       *  This function erases an element, pointed to by the given iterator,
       *  from a %multimap.  Note that this function only erases the element,
       *  and that if the element is itself a pointer, the pointed-to memory is
       *  not touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      void
      erase(iterator __position)
      { _M_t.erase(__position); }
#endif

      /**
       *  @brief Erases elements according to the provided key.
       *  @param  __x  Key of element to be erased.
       *  @return  The number of elements erased.
       *
       *  This function erases all elements located by the given key from a
       *  %multimap.
       *  Note that this function only erases the element, and that if
       *  the element is itself a pointer, the pointed-to memory is not touched
       *  in any way.  Managing the pointer is the user's responsibility.
       */
      size_type
      erase(const key_type& __x)
      { return _M_t.erase(__x); }

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 130. Associative erase should return an iterator.
      /**
       *  @brief Erases a [first,last) range of elements from a %multimap.
       *  @param  __first  Iterator pointing to the start of the range to be
       *                   erased.
       *  @param __last Iterator pointing to the end of the range to be
       *                erased .
       *  @return The iterator @a __last.
       *
       *  This function erases a sequence of elements from a %multimap.
       *  Note that this function only erases the elements, and that if
       *  the elements themselves are pointers, the pointed-to memory is not
       *  touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      iterator
      erase(const_iterator __first, const_iterator __last)
      { return _M_t.erase(__first, __last); }
#else
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 130. Associative erase should return an iterator.
      /**
       *  @brief Erases a [first,last) range of elements from a %multimap.
       *  @param  __first  Iterator pointing to the start of the range to be
       *                 erased.
       *  @param __last Iterator pointing to the end of the range to
       *                be erased.
       *
       *  This function erases a sequence of elements from a %multimap.
       *  Note that this function only erases the elements, and that if
       *  the elements themselves are pointers, the pointed-to memory is not
       *  touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      void
      erase(iterator __first, iterator __last)
      { _M_t.erase(__first, __last); }
#endif

      /**
       *  @brief  Swaps data with another %multimap.
       *  @param  __x  A %multimap of the same element and allocator types.
       *
       *  This exchanges the elements between two multimaps in constant time.
       *  (It is only swapping a pointer, an integer, and an instance of
       *  the @c Compare type (which itself is often stateless and empty), so it
       *  should be quite fast.)
       *  Note that the global std::swap() function is specialized such that
       *  std::swap(m1,m2) will feed to this function.
       */
      void
      swap(multimap& __x)
      { _M_t.swap(__x._M_t); }

      /**
       *  Erases all elements in a %multimap.  Note that this function only
       *  erases the elements, and that if the elements themselves are pointers,
       *  the pointed-to memory is not touched in any way.  Managing the pointer
       *  is the user's responsibility.
       */
      void
      clear() _GLIBCXX_NOEXCEPT
      { _M_t.clear(); }

      // observers
      /**
       *  Returns the key comparison object out of which the %multimap
       *  was constructed.
       */
      key_compare
      key_comp() const
      { return _M_t.key_comp(); }

      /**
       *  Returns a value comparison object, built from the key comparison
       *  object out of which the %multimap was constructed.
       */
      value_compare
      value_comp() const
      { return value_compare(_M_t.key_comp()); }

      // multimap operations
      /**
       *  @brief Tries to locate an element in a %multimap.
       *  @param  __x  Key of (key, value) pair to be located.
       *  @return  Iterator pointing to sought-after element,
       *           or end() if not found.
       *
       *  This function takes a key and tries to locate the element with which
       *  the key matches.  If successful the function returns an iterator
       *  pointing to the sought after %pair.  If unsuccessful it returns the
       *  past-the-end ( @c end() ) iterator.
       */
      iterator
      find(const key_type& __x)
      { return _M_t.find(__x); }

      /**
       *  @brief Tries to locate an element in a %multimap.
       *  @param  __x  Key of (key, value) pair to be located.
       *  @return  Read-only (constant) iterator pointing to sought-after
       *           element, or end() if not found.
       *
       *  This function takes a key and tries to locate the element with which
       *  the key matches.  If successful the function returns a constant
       *  iterator pointing to the sought after %pair.  If unsuccessful it
       *  returns the past-the-end ( @c end() ) iterator.
       */
      const_iterator
      find(const key_type& __x) const
      { return _M_t.find(__x); }

      /**
       *  @brief Finds the number of elements with given key.
       *  @param  __x  Key of (key, value) pairs to be located.
       *  @return Number of elements with specified key.
       */
      size_type
      count(const key_type& __x) const
      { return _M_t.count(__x); }

      /**
       *  @brief Finds the beginning of a subsequence matching given key.
       *  @param  __x  Key of (key, value) pair to be located.
       *  @return  Iterator pointing to first element equal to or greater
       *           than key, or end().
       *
       *  This function returns the first element of a subsequence of elements
       *  that matches the given key.  If unsuccessful it returns an iterator
       *  pointing to the first element that has a greater value than given key
       *  or end() if no such element exists.
       */
      iterator
      lower_bound(const key_type& __x)
      { return _M_t.lower_bound(__x); }

      /**
       *  @brief Finds the beginning of a subsequence matching given key.
       *  @param  __x  Key of (key, value) pair to be located.
       *  @return  Read-only (constant) iterator pointing to first element
       *           equal to or greater than key, or end().
       *
       *  This function returns the first element of a subsequence of
       *  elements that matches the given key.  If unsuccessful the
       *  iterator will point to the next greatest element or, if no
       *  such greater element exists, to end().
       */
      const_iterator
      lower_bound(const key_type& __x) const
      { return _M_t.lower_bound(__x); }

      /**
       *  @brief Finds the end of a subsequence matching given key.
       *  @param  __x  Key of (key, value) pair to be located.
       *  @return Iterator pointing to the first element
       *          greater than key, or end().
       */
      iterator
      upper_bound(const key_type& __x)
      { return _M_t.upper_bound(__x); }

      /**
       *  @brief Finds the end of a subsequence matching given key.
       *  @param  __x  Key of (key, value) pair to be located.
       *  @return  Read-only (constant) iterator pointing to first iterator
       *           greater than key, or end().
       */
      const_iterator
      upper_bound(const key_type& __x) const
      { return _M_t.upper_bound(__x); }

      /**
       *  @brief Finds a subsequence matching given key.
       *  @param  __x  Key of (key, value) pairs to be located.
       *  @return  Pair of iterators that possibly points to the subsequence
       *           matching given key.
       *
       *  This function is equivalent to
       *  @code
       *    std::make_pair(c.lower_bound(val),
       *                   c.upper_bound(val))
       *  @endcode
       *  (but is faster than making the calls separately).
       */
      std::pair<iterator, iterator>
      equal_range(const key_type& __x)
      { return _M_t.equal_range(__x); }

      /**
       *  @brief Finds a subsequence matching given key.
       *  @param  __x  Key of (key, value) pairs to be located.
       *  @return  Pair of read-only (constant) iterators that possibly points
       *           to the subsequence matching given key.
       *
       *  This function is equivalent to
       *  @code
       *    std::make_pair(c.lower_bound(val),
       *                   c.upper_bound(val))
       *  @endcode
       *  (but is faster than making the calls separately).
       */
      std::pair<const_iterator, const_iterator>
      equal_range(const key_type& __x) const
      { return _M_t.equal_range(__x); }

      template<typename _K1, typename _T1, typename _C1, typename _A1>
        friend bool
        operator==(const multimap<_K1, _T1, _C1, _A1>&,
		   const multimap<_K1, _T1, _C1, _A1>&);

      template<typename _K1, typename _T1, typename _C1, typename _A1>
        friend bool
        operator<(const multimap<_K1, _T1, _C1, _A1>&,
		  const multimap<_K1, _T1, _C1, _A1>&);
  };

  /**
   *  @brief  Multimap equality comparison.
   *  @param  __x  A %multimap.
   *  @param  __y  A %multimap of the same type as @a __x.
   *  @return  True iff the size and elements of the maps are equal.
   *
   *  This is an equivalence relation.  It is linear in the size of the
   *  multimaps.  Multimaps are considered equivalent if their sizes are equal,
   *  and if corresponding elements compare equal.
  */
  template<typename _Key, typename _Tp, typename _Compare, typename _Alloc>
    inline bool
    operator==(const multimap<_Key, _Tp, _Compare, _Alloc>& __x,
               const multimap<_Key, _Tp, _Compare, _Alloc>& __y)
    { return __x._M_t == __y._M_t; }

  /**
   *  @brief  Multimap ordering relation.
   *  @param  __x  A %multimap.
   *  @param  __y  A %multimap of the same type as @a __x.
   *  @return  True iff @a x is lexicographically less than @a y.
   *
   *  This is a total ordering relation.  It is linear in the size of the
   *  multimaps.  The elements must be comparable with @c <.
   *
   *  See std::lexicographical_compare() for how the determination is made.
  */
  template<typename _Key, typename _Tp, typename _Compare, typename _Alloc>
    inline bool
    operator<(const multimap<_Key, _Tp, _Compare, _Alloc>& __x,
              const multimap<_Key, _Tp, _Compare, _Alloc>& __y)
    { return __x._M_t < __y._M_t; }

  /// Based on operator==
  template<typename _Key, typename _Tp, typename _Compare, typename _Alloc>
    inline bool
    operator!=(const multimap<_Key, _Tp, _Compare, _Alloc>& __x,
               const multimap<_Key, _Tp, _Compare, _Alloc>& __y)
    { return !(__x == __y); }

  /// Based on operator<
  template<typename _Key, typename _Tp, typename _Compare, typename _Alloc>
    inline bool
    operator>(const multimap<_Key, _Tp, _Compare, _Alloc>& __x,
              const multimap<_Key, _Tp, _Compare, _Alloc>& __y)
    { return __y < __x; }

  /// Based on operator<
  template<typename _Key, typename _Tp, typename _Compare, typename _Alloc>
    inline bool
    operator<=(const multimap<_Key, _Tp, _Compare, _Alloc>& __x,
               const multimap<_Key, _Tp, _Compare, _Alloc>& __y)
    { return !(__y < __x); }

  /// Based on operator<
  template<typename _Key, typename _Tp, typename _Compare, typename _Alloc>
    inline bool
    operator>=(const multimap<_Key, _Tp, _Compare, _Alloc>& __x,
               const multimap<_Key, _Tp, _Compare, _Alloc>& __y)
    { return !(__x < __y); }

  /// See std::multimap::swap().
  template<typename _Key, typename _Tp, typename _Compare, typename _Alloc>
    inline void
    swap(multimap<_Key, _Tp, _Compare, _Alloc>& __x,
         multimap<_Key, _Tp, _Compare, _Alloc>& __y)
    { __x.swap(__y); }

_GLIBCXX_END_NAMESPACE_CONTAINER
} // namespace std

#endif /* _STL_MULTIMAP_H */
