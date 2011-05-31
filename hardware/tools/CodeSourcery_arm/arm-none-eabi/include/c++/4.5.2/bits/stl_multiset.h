// Multiset implementation -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010
// Free Software Foundation, Inc.
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
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file stl_multiset.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _STL_MULTISET_H
#define _STL_MULTISET_H 1

#include <bits/concept_check.h>
#include <initializer_list>

_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD_D)

  /**
   *  @brief A standard container made up of elements, which can be retrieved
   *  in logarithmic time.
   *
   *  @ingroup associative_containers
   *
   *  Meets the requirements of a <a href="tables.html#65">container</a>, a
   *  <a href="tables.html#66">reversible container</a>, and an
   *  <a href="tables.html#69">associative container</a> (using equivalent
   *  keys).  For a @c multiset<Key> the key_type and value_type are Key.
   *
   *  Multisets support bidirectional iterators.
   *
   *  The private tree data is declared exactly the same way for set and
   *  multiset; the distinction is made entirely in how the tree functions are
   *  called (*_unique versus *_equal, same as the standard).
  */
  template <typename _Key, typename _Compare = std::less<_Key>,
	    typename _Alloc = std::allocator<_Key> >
    class multiset
    {
      // concept requirements
      typedef typename _Alloc::value_type                   _Alloc_value_type;
      __glibcxx_class_requires(_Key, _SGIAssignableConcept)
      __glibcxx_class_requires4(_Compare, bool, _Key, _Key,
				_BinaryFunctionConcept)
      __glibcxx_class_requires2(_Key, _Alloc_value_type, _SameTypeConcept)	

    public:
      // typedefs:
      typedef _Key     key_type;
      typedef _Key     value_type;
      typedef _Compare key_compare;
      typedef _Compare value_compare;
      typedef _Alloc   allocator_type;

    private:
      /// This turns a red-black tree into a [multi]set.
      typedef typename _Alloc::template rebind<_Key>::other _Key_alloc_type;

      typedef _Rb_tree<key_type, value_type, _Identity<value_type>,
		       key_compare, _Key_alloc_type> _Rep_type;
      /// The actual tree structure.
      _Rep_type _M_t;

    public:
      typedef typename _Key_alloc_type::pointer             pointer;
      typedef typename _Key_alloc_type::const_pointer       const_pointer;
      typedef typename _Key_alloc_type::reference           reference;
      typedef typename _Key_alloc_type::const_reference     const_reference;
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 103. set::iterator is required to be modifiable,
      // but this allows modification of keys.
      typedef typename _Rep_type::const_iterator            iterator;
      typedef typename _Rep_type::const_iterator            const_iterator;
      typedef typename _Rep_type::const_reverse_iterator    reverse_iterator;
      typedef typename _Rep_type::const_reverse_iterator    const_reverse_iterator;
      typedef typename _Rep_type::size_type                 size_type;
      typedef typename _Rep_type::difference_type           difference_type;

      // allocation/deallocation
      /**
       *  @brief  Default constructor creates no elements.
       */
      multiset()
      : _M_t() { }

      /**
       *  @brief  Creates a %multiset with no elements.
       *  @param  comp  Comparator to use.
       *  @param  a  An allocator object.
       */
      explicit
      multiset(const _Compare& __comp,
	       const allocator_type& __a = allocator_type())
      : _M_t(__comp, __a) { }

      /**
       *  @brief  Builds a %multiset from a range.
       *  @param  first  An input iterator.
       *  @param  last  An input iterator.
       *
       *  Create a %multiset consisting of copies of the elements from
       *  [first,last).  This is linear in N if the range is already sorted,
       *  and NlogN otherwise (where N is distance(first,last)).
       */
      template<typename _InputIterator>
        multiset(_InputIterator __first, _InputIterator __last)
	: _M_t()
        { _M_t._M_insert_equal(__first, __last); }

      /**
       *  @brief  Builds a %multiset from a range.
       *  @param  first  An input iterator.
       *  @param  last  An input iterator.
       *  @param  comp  A comparison functor.
       *  @param  a  An allocator object.
       *
       *  Create a %multiset consisting of copies of the elements from
       *  [first,last).  This is linear in N if the range is already sorted,
       *  and NlogN otherwise (where N is distance(first,last)).
       */
      template<typename _InputIterator>
        multiset(_InputIterator __first, _InputIterator __last,
		 const _Compare& __comp,
		 const allocator_type& __a = allocator_type())
	: _M_t(__comp, __a)
        { _M_t._M_insert_equal(__first, __last); }

      /**
       *  @brief  %Multiset copy constructor.
       *  @param  x  A %multiset of identical element and allocator types.
       *
       *  The newly-created %multiset uses a copy of the allocation object used
       *  by @a x.
       */
      multiset(const multiset& __x)
      : _M_t(__x._M_t) { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
     /**
       *  @brief  %Multiset move constructor.
       *  @param  x  A %multiset of identical element and allocator types.
       *
       *  The newly-created %multiset contains the exact contents of @a x.
       *  The contents of @a x are a valid, but unspecified %multiset.
       */
      multiset(multiset&& __x)
      : _M_t(std::forward<_Rep_type>(__x._M_t)) { }

      /**
       *  @brief  Builds a %multiset from an initializer_list.
       *  @param  l  An initializer_list.
       *  @param  comp  A comparison functor.
       *  @param  a  An allocator object.
       *
       *  Create a %multiset consisting of copies of the elements from
       *  the list.  This is linear in N if the list is already sorted,
       *  and NlogN otherwise (where N is @a l.size()).
       */
      multiset(initializer_list<value_type> __l,
	       const _Compare& __comp = _Compare(),
	       const allocator_type& __a = allocator_type())
      : _M_t(__comp, __a)
      { _M_t._M_insert_equal(__l.begin(), __l.end()); }
#endif

      /**
       *  @brief  %Multiset assignment operator.
       *  @param  x  A %multiset of identical element and allocator types.
       *
       *  All the elements of @a x are copied, but unlike the copy constructor,
       *  the allocator object is not copied.
       */
      multiset&
      operator=(const multiset& __x)
      {
	_M_t = __x._M_t;
	return *this;
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       *  @brief  %Multiset move assignment operator.
       *  @param  x  A %multiset of identical element and allocator types.
       *
       *  The contents of @a x are moved into this %multiset (without copying).
       *  @a x is a valid, but unspecified %multiset.
       */
      multiset&
      operator=(multiset&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__x);
	return *this;
      }

      /**
       *  @brief  %Multiset list assignment operator.
       *  @param  l  An initializer_list.
       *
       *  This function fills a %multiset with copies of the elements in the
       *  initializer list @a l.
       *
       *  Note that the assignment completely changes the %multiset and
       *  that the resulting %multiset's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
      multiset&
      operator=(initializer_list<value_type> __l)
      {
	this->clear();
	this->insert(__l.begin(), __l.end());
	return *this;
      }
#endif

      // accessors:

      ///  Returns the comparison object.
      key_compare
      key_comp() const
      { return _M_t.key_comp(); }
      ///  Returns the comparison object.
      value_compare
      value_comp() const
      { return _M_t.key_comp(); }
      ///  Returns the memory allocation object.
      allocator_type
      get_allocator() const
      { return _M_t.get_allocator(); }

      /**
       *  Returns a read-only (constant) iterator that points to the first
       *  element in the %multiset.  Iteration is done in ascending order
       *  according to the keys.
       */
      iterator
      begin() const
      { return _M_t.begin(); }

      /**
       *  Returns a read-only (constant) iterator that points one past the last
       *  element in the %multiset.  Iteration is done in ascending order
       *  according to the keys.
       */
      iterator
      end() const
      { return _M_t.end(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to the
       *  last element in the %multiset.  Iteration is done in descending order
       *  according to the keys.
       */
      reverse_iterator
      rbegin() const
      { return _M_t.rbegin(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to the
       *  last element in the %multiset.  Iteration is done in descending order
       *  according to the keys.
       */
      reverse_iterator
      rend() const
      { return _M_t.rend(); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       *  Returns a read-only (constant) iterator that points to the first
       *  element in the %multiset.  Iteration is done in ascending order
       *  according to the keys.
       */
      iterator
      cbegin() const
      { return _M_t.begin(); }

      /**
       *  Returns a read-only (constant) iterator that points one past the last
       *  element in the %multiset.  Iteration is done in ascending order
       *  according to the keys.
       */
      iterator
      cend() const
      { return _M_t.end(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to the
       *  last element in the %multiset.  Iteration is done in descending order
       *  according to the keys.
       */
      reverse_iterator
      crbegin() const
      { return _M_t.rbegin(); }

      /**
       *  Returns a read-only (constant) reverse iterator that points to the
       *  last element in the %multiset.  Iteration is done in descending order
       *  according to the keys.
       */
      reverse_iterator
      crend() const
      { return _M_t.rend(); }
#endif

      ///  Returns true if the %set is empty.
      bool
      empty() const
      { return _M_t.empty(); }

      ///  Returns the size of the %set.
      size_type
      size() const
      { return _M_t.size(); }

      ///  Returns the maximum size of the %set.
      size_type
      max_size() const
      { return _M_t.max_size(); }

      /**
       *  @brief  Swaps data with another %multiset.
       *  @param  x  A %multiset of the same element and allocator types.
       *
       *  This exchanges the elements between two multisets in constant time.
       *  (It is only swapping a pointer, an integer, and an instance of the @c
       *  Compare type (which itself is often stateless and empty), so it should
       *  be quite fast.)
       *  Note that the global std::swap() function is specialized such that
       *  std::swap(s1,s2) will feed to this function.
       */
      void
      swap(multiset& __x)
      { _M_t.swap(__x._M_t); }

      // insert/erase
      /**
       *  @brief Inserts an element into the %multiset.
       *  @param  x  Element to be inserted.
       *  @return An iterator that points to the inserted element.
       *
       *  This function inserts an element into the %multiset.  Contrary
       *  to a std::set the %multiset does not rely on unique keys and thus
       *  multiple copies of the same element can be inserted.
       *
       *  Insertion requires logarithmic time.
       */
      iterator
      insert(const value_type& __x)
      { return _M_t._M_insert_equal(__x); }

      /**
       *  @brief Inserts an element into the %multiset.
       *  @param  position  An iterator that serves as a hint as to where the
       *                    element should be inserted.
       *  @param  x  Element to be inserted.
       *  @return An iterator that points to the inserted element.
       *
       *  This function inserts an element into the %multiset.  Contrary
       *  to a std::set the %multiset does not rely on unique keys and thus
       *  multiple copies of the same element can be inserted.
       *
       *  Note that the first parameter is only a hint and can potentially
       *  improve the performance of the insertion process.  A bad hint would
       *  cause no gains in efficiency.
       *
       *  See http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt07ch17.html
       *  for more on @a hinting.
       *
       *  Insertion requires logarithmic time (if the hint is not taken).
       */
      iterator
      insert(iterator __position, const value_type& __x)
      { return _M_t._M_insert_equal_(__position, __x); }

      /**
       *  @brief A template function that tries to insert a range of elements.
       *  @param  first  Iterator pointing to the start of the range to be
       *                 inserted.
       *  @param  last  Iterator pointing to the end of the range.
       *
       *  Complexity similar to that of the range constructor.
       */
      template<typename _InputIterator>
        void
        insert(_InputIterator __first, _InputIterator __last)
        { _M_t._M_insert_equal(__first, __last); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       *  @brief Attempts to insert a list of elements into the %multiset.
       *  @param  list  A std::initializer_list<value_type> of elements
       *                to be inserted.
       *
       *  Complexity similar to that of the range constructor.
       */
      void
      insert(initializer_list<value_type> __l)
      { this->insert(__l.begin(), __l.end()); }
#endif

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 130. Associative erase should return an iterator.
      /**
       *  @brief Erases an element from a %multiset.
       *  @param  position  An iterator pointing to the element to be erased.
       *  @return An iterator pointing to the element immediately following
       *          @a position prior to the element being erased. If no such 
       *          element exists, end() is returned.
       *
       *  This function erases an element, pointed to by the given iterator,
       *  from a %multiset.  Note that this function only erases the element,
       *  and that if the element is itself a pointer, the pointed-to memory is
       *  not touched in any way.  Managing the pointer is the user's
       *  responsibility.
       */
      iterator
      erase(iterator __position)
      { return _M_t.erase(__position); }
#else
      /**
       *  @brief Erases an element from a %multiset.
       *  @param  position  An iterator pointing to the element to be erased.
       *
       *  This function erases an element, pointed to by the given iterator,
       *  from a %multiset.  Note that this function only erases the element,
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
       *  @param  x  Key of element to be erased.
       *  @return  The number of elements erased.
       *
       *  This function erases all elements located by the given key from a
       *  %multiset.
       *  Note that this function only erases the element, and that if
       *  the element is itself a pointer, the pointed-to memory is not touched
       *  in any way.  Managing the pointer is the user's responsibility.
       */
      size_type
      erase(const key_type& __x)
      { return _M_t.erase(__x); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 130. Associative erase should return an iterator.
      /**
       *  @brief Erases a [first,last) range of elements from a %multiset.
       *  @param  first  Iterator pointing to the start of the range to be
       *                 erased.
       *  @param  last  Iterator pointing to the end of the range to be erased.
       *  @return The iterator @a last.
       *
       *  This function erases a sequence of elements from a %multiset.
       *  Note that this function only erases the elements, and that if
       *  the elements themselves are pointers, the pointed-to memory is not
       *  touched in any way.  Managing the pointer is the user's responsibility.
       */
      iterator
      erase(iterator __first, iterator __last)
      { return _M_t.erase(__first, __last); }
#else
      /**
       *  @brief Erases a [first,last) range of elements from a %multiset.
       *  @param  first  Iterator pointing to the start of the range to be
       *                 erased.
       *  @param  last  Iterator pointing to the end of the range to be erased.
       *
       *  This function erases a sequence of elements from a %multiset.
       *  Note that this function only erases the elements, and that if
       *  the elements themselves are pointers, the pointed-to memory is not
       *  touched in any way.  Managing the pointer is the user's responsibility.
       */
      void
      erase(iterator __first, iterator __last)
      { _M_t.erase(__first, __last); }
#endif

      /**
       *  Erases all elements in a %multiset.  Note that this function only
       *  erases the elements, and that if the elements themselves are pointers,
       *  the pointed-to memory is not touched in any way.  Managing the pointer
       *  is the user's responsibility.
       */
      void
      clear()
      { _M_t.clear(); }

      // multiset operations:

      /**
       *  @brief Finds the number of elements with given key.
       *  @param  x  Key of elements to be located.
       *  @return Number of elements with specified key.
       */
      size_type
      count(const key_type& __x) const
      { return _M_t.count(__x); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214.  set::find() missing const overload
      //@{
      /**
       *  @brief Tries to locate an element in a %set.
       *  @param  x  Element to be located.
       *  @return  Iterator pointing to sought-after element, or end() if not
       *           found.
       *
       *  This function takes a key and tries to locate the element with which
       *  the key matches.  If successful the function returns an iterator
       *  pointing to the sought after element.  If unsuccessful it returns the
       *  past-the-end ( @c end() ) iterator.
       */
      iterator
      find(const key_type& __x)
      { return _M_t.find(__x); }

      const_iterator
      find(const key_type& __x) const
      { return _M_t.find(__x); }
      //@}

      //@{
      /**
       *  @brief Finds the beginning of a subsequence matching given key.
       *  @param  x  Key to be located.
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

      const_iterator
      lower_bound(const key_type& __x) const
      { return _M_t.lower_bound(__x); }
      //@}

      //@{
      /**
       *  @brief Finds the end of a subsequence matching given key.
       *  @param  x  Key to be located.
       *  @return Iterator pointing to the first element
       *          greater than key, or end().
       */
      iterator
      upper_bound(const key_type& __x)
      { return _M_t.upper_bound(__x); }

      const_iterator
      upper_bound(const key_type& __x) const
      { return _M_t.upper_bound(__x); }
      //@}

      //@{
      /**
       *  @brief Finds a subsequence matching given key.
       *  @param  x  Key to be located.
       *  @return  Pair of iterators that possibly points to the subsequence
       *           matching given key.
       *
       *  This function is equivalent to
       *  @code
       *    std::make_pair(c.lower_bound(val),
       *                   c.upper_bound(val))
       *  @endcode
       *  (but is faster than making the calls separately).
       *
       *  This function probably only makes sense for multisets.
       */
      std::pair<iterator, iterator>
      equal_range(const key_type& __x)
      { return _M_t.equal_range(__x); }

      std::pair<const_iterator, const_iterator>
      equal_range(const key_type& __x) const
      { return _M_t.equal_range(__x); }

      template<typename _K1, typename _C1, typename _A1>
        friend bool
        operator==(const multiset<_K1, _C1, _A1>&,
		   const multiset<_K1, _C1, _A1>&);

      template<typename _K1, typename _C1, typename _A1>
        friend bool
        operator< (const multiset<_K1, _C1, _A1>&,
		   const multiset<_K1, _C1, _A1>&);
    };

  /**
   *  @brief  Multiset equality comparison.
   *  @param  x  A %multiset.
   *  @param  y  A %multiset of the same type as @a x.
   *  @return  True iff the size and elements of the multisets are equal.
   *
   *  This is an equivalence relation.  It is linear in the size of the
   *  multisets.
   *  Multisets are considered equivalent if their sizes are equal, and if
   *  corresponding elements compare equal.
  */
  template<typename _Key, typename _Compare, typename _Alloc>
    inline bool
    operator==(const multiset<_Key, _Compare, _Alloc>& __x,
	       const multiset<_Key, _Compare, _Alloc>& __y)
    { return __x._M_t == __y._M_t; }

  /**
   *  @brief  Multiset ordering relation.
   *  @param  x  A %multiset.
   *  @param  y  A %multiset of the same type as @a x.
   *  @return  True iff @a x is lexicographically less than @a y.
   *
   *  This is a total ordering relation.  It is linear in the size of the
   *  maps.  The elements must be comparable with @c <.
   *
   *  See std::lexicographical_compare() for how the determination is made.
  */
  template<typename _Key, typename _Compare, typename _Alloc>
    inline bool
    operator<(const multiset<_Key, _Compare, _Alloc>& __x,
	      const multiset<_Key, _Compare, _Alloc>& __y)
    { return __x._M_t < __y._M_t; }

  ///  Returns !(x == y).
  template<typename _Key, typename _Compare, typename _Alloc>
    inline bool
    operator!=(const multiset<_Key, _Compare, _Alloc>& __x,
	       const multiset<_Key, _Compare, _Alloc>& __y)
    { return !(__x == __y); }

  ///  Returns y < x.
  template<typename _Key, typename _Compare, typename _Alloc>
    inline bool
    operator>(const multiset<_Key,_Compare,_Alloc>& __x,
	      const multiset<_Key,_Compare,_Alloc>& __y)
    { return __y < __x; }

  ///  Returns !(y < x)
  template<typename _Key, typename _Compare, typename _Alloc>
    inline bool
    operator<=(const multiset<_Key, _Compare, _Alloc>& __x,
	       const multiset<_Key, _Compare, _Alloc>& __y)
    { return !(__y < __x); }

  ///  Returns !(x < y)
  template<typename _Key, typename _Compare, typename _Alloc>
    inline bool
    operator>=(const multiset<_Key, _Compare, _Alloc>& __x,
	       const multiset<_Key, _Compare, _Alloc>& __y)
    { return !(__x < __y); }

  /// See std::multiset::swap().
  template<typename _Key, typename _Compare, typename _Alloc>
    inline void
    swap(multiset<_Key, _Compare, _Alloc>& __x,
	 multiset<_Key, _Compare, _Alloc>& __y)
    { __x.swap(__y); }

_GLIBCXX_END_NESTED_NAMESPACE

#endif /* _STL_MULTISET_H */
