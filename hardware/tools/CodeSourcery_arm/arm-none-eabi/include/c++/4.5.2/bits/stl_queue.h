// Queue implementation -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
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

/** @file stl_queue.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _STL_QUEUE_H
#define _STL_QUEUE_H 1

#include <bits/concept_check.h>
#include <debug/debug.h>

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   *  @brief  A standard container giving FIFO behavior.
   *
   *  @ingroup sequences
   *
   *  Meets many of the requirements of a
   *  <a href="tables.html#65">container</a>,
   *  but does not define anything to do with iterators.  Very few of the
   *  other standard container interfaces are defined.
   *
   *  This is not a true container, but an @e adaptor.  It holds another
   *  container, and provides a wrapper interface to that container.  The
   *  wrapper is what enforces strict first-in-first-out %queue behavior.
   *
   *  The second template parameter defines the type of the underlying
   *  sequence/container.  It defaults to std::deque, but it can be any type
   *  that supports @c front, @c back, @c push_back, and @c pop_front,
   *  such as std::list or an appropriate user-defined type.
   *
   *  Members not found in @a normal containers are @c container_type,
   *  which is a typedef for the second Sequence parameter, and @c push and
   *  @c pop, which are standard %queue/FIFO operations.
  */
  template<typename _Tp, typename _Sequence = deque<_Tp> >
    class queue
    {
      // concept requirements
      typedef typename _Sequence::value_type _Sequence_value_type;
      __glibcxx_class_requires(_Tp, _SGIAssignableConcept)
      __glibcxx_class_requires(_Sequence, _FrontInsertionSequenceConcept)
      __glibcxx_class_requires(_Sequence, _BackInsertionSequenceConcept)
      __glibcxx_class_requires2(_Tp, _Sequence_value_type, _SameTypeConcept)

      template<typename _Tp1, typename _Seq1>
        friend bool
        operator==(const queue<_Tp1, _Seq1>&, const queue<_Tp1, _Seq1>&);

      template<typename _Tp1, typename _Seq1>
        friend bool
        operator<(const queue<_Tp1, _Seq1>&, const queue<_Tp1, _Seq1>&);

    public:
      typedef typename _Sequence::value_type                value_type;
      typedef typename _Sequence::reference                 reference;
      typedef typename _Sequence::const_reference           const_reference;
      typedef typename _Sequence::size_type                 size_type;
      typedef          _Sequence                            container_type;

    protected:
      /**
       *  'c' is the underlying container.  Maintainers wondering why
       *  this isn't uglified as per style guidelines should note that
       *  this name is specified in the standard, [23.2.3.1].  (Why?
       *  Presumably for the same reason that it's protected instead
       *  of private: to allow derivation.  But none of the other
       *  containers allow for derivation.  Odd.)
       */
      _Sequence c;

    public:
      /**
       *  @brief  Default constructor creates no elements.
       */
#ifndef __GXX_EXPERIMENTAL_CXX0X__
      explicit
      queue(const _Sequence& __c = _Sequence())
      : c(__c) { }
#else
      explicit
      queue(const _Sequence& __c)
      : c(__c) { }

      explicit
      queue(_Sequence&& __c = _Sequence())
      : c(std::move(__c)) { }

      queue(queue&& __q)
      : c(std::move(__q.c)) { }

      queue&
      operator=(queue&& __q)
      {
	c = std::move(__q.c);
	return *this;
      }
#endif

      /**
       *  Returns true if the %queue is empty.
       */
      bool
      empty() const
      { return c.empty(); }

      /**  Returns the number of elements in the %queue.  */
      size_type
      size() const
      { return c.size(); }

      /**
       *  Returns a read/write reference to the data at the first
       *  element of the %queue.
       */
      reference
      front()
      {
	__glibcxx_requires_nonempty();
	return c.front();
      }

      /**
       *  Returns a read-only (constant) reference to the data at the first
       *  element of the %queue.
       */
      const_reference
      front() const
      {
	__glibcxx_requires_nonempty();
	return c.front();
      }

      /**
       *  Returns a read/write reference to the data at the last
       *  element of the %queue.
       */
      reference
      back()
      {
	__glibcxx_requires_nonempty();
	return c.back();
      }

      /**
       *  Returns a read-only (constant) reference to the data at the last
       *  element of the %queue.
       */
      const_reference
      back() const
      {
	__glibcxx_requires_nonempty();
	return c.back();
      }

      /**
       *  @brief  Add data to the end of the %queue.
       *  @param  x  Data to be added.
       *
       *  This is a typical %queue operation.  The function creates an
       *  element at the end of the %queue and assigns the given data
       *  to it.  The time complexity of the operation depends on the
       *  underlying sequence.
       */
      void
      push(const value_type& __x)
      { c.push_back(__x); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      void
      push(value_type&& __x)
      { c.push_back(std::move(__x)); }

      template<typename... _Args>
        void
        emplace(_Args&&... __args)
	{ c.emplace_back(std::forward<_Args>(__args)...); }
#endif

      /**
       *  @brief  Removes first element.
       *
       *  This is a typical %queue operation.  It shrinks the %queue by one.
       *  The time complexity of the operation depends on the underlying
       *  sequence.
       *
       *  Note that no data is returned, and if the first element's
       *  data is needed, it should be retrieved before pop() is
       *  called.
       */
      void
      pop()
      {
	__glibcxx_requires_nonempty();
	c.pop_front();
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      void
      swap(queue& __q)
      { c.swap(__q.c); }
#endif
    };

  /**
   *  @brief  Queue equality comparison.
   *  @param  x  A %queue.
   *  @param  y  A %queue of the same type as @a x.
   *  @return  True iff the size and elements of the queues are equal.
   *
   *  This is an equivalence relation.  Complexity and semantics depend on the
   *  underlying sequence type, but the expected rules are:  this relation is
   *  linear in the size of the sequences, and queues are considered equivalent
   *  if their sequences compare equal.
  */
  template<typename _Tp, typename _Seq>
    inline bool
    operator==(const queue<_Tp, _Seq>& __x, const queue<_Tp, _Seq>& __y)
    { return __x.c == __y.c; }

  /**
   *  @brief  Queue ordering relation.
   *  @param  x  A %queue.
   *  @param  y  A %queue of the same type as @a x.
   *  @return  True iff @a x is lexicographically less than @a y.
   *
   *  This is an total ordering relation.  Complexity and semantics
   *  depend on the underlying sequence type, but the expected rules
   *  are: this relation is linear in the size of the sequences, the
   *  elements must be comparable with @c <, and
   *  std::lexicographical_compare() is usually used to make the
   *  determination.
  */
  template<typename _Tp, typename _Seq>
    inline bool
    operator<(const queue<_Tp, _Seq>& __x, const queue<_Tp, _Seq>& __y)
    { return __x.c < __y.c; }

  /// Based on operator==
  template<typename _Tp, typename _Seq>
    inline bool
    operator!=(const queue<_Tp, _Seq>& __x, const queue<_Tp, _Seq>& __y)
    { return !(__x == __y); }

  /// Based on operator<
  template<typename _Tp, typename _Seq>
    inline bool
    operator>(const queue<_Tp, _Seq>& __x, const queue<_Tp, _Seq>& __y)
    { return __y < __x; }

  /// Based on operator<
  template<typename _Tp, typename _Seq>
    inline bool
    operator<=(const queue<_Tp, _Seq>& __x, const queue<_Tp, _Seq>& __y)
    { return !(__y < __x); }

  /// Based on operator<
  template<typename _Tp, typename _Seq>
    inline bool
    operator>=(const queue<_Tp, _Seq>& __x, const queue<_Tp, _Seq>& __y)
    { return !(__x < __y); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  template<typename _Tp, typename _Seq>
    inline void
    swap(queue<_Tp, _Seq>& __x, queue<_Tp, _Seq>& __y)
    { __x.swap(__y); }
#endif

  /**
   *  @brief  A standard container automatically sorting its contents.
   *
   *  @ingroup sequences
   *
   *  This is not a true container, but an @e adaptor.  It holds
   *  another container, and provides a wrapper interface to that
   *  container.  The wrapper is what enforces priority-based sorting 
   *  and %queue behavior.  Very few of the standard container/sequence
   *  interface requirements are met (e.g., iterators).
   *
   *  The second template parameter defines the type of the underlying
   *  sequence/container.  It defaults to std::vector, but it can be
   *  any type that supports @c front(), @c push_back, @c pop_back,
   *  and random-access iterators, such as std::deque or an
   *  appropriate user-defined type.
   *
   *  The third template parameter supplies the means of making
   *  priority comparisons.  It defaults to @c less<value_type> but
   *  can be anything defining a strict weak ordering.
   *
   *  Members not found in @a normal containers are @c container_type,
   *  which is a typedef for the second Sequence parameter, and @c
   *  push, @c pop, and @c top, which are standard %queue operations.
   *
   *  @note No equality/comparison operators are provided for
   *  %priority_queue.
   *
   *  @note Sorting of the elements takes place as they are added to,
   *  and removed from, the %priority_queue using the
   *  %priority_queue's member functions.  If you access the elements
   *  by other means, and change their data such that the sorting
   *  order would be different, the %priority_queue will not re-sort
   *  the elements for you.  (How could it know to do so?)
  */
  template<typename _Tp, typename _Sequence = vector<_Tp>,
	   typename _Compare  = less<typename _Sequence::value_type> >
    class priority_queue
    {
      // concept requirements
      typedef typename _Sequence::value_type _Sequence_value_type;
      __glibcxx_class_requires(_Tp, _SGIAssignableConcept)
      __glibcxx_class_requires(_Sequence, _SequenceConcept)
      __glibcxx_class_requires(_Sequence, _RandomAccessContainerConcept)
      __glibcxx_class_requires2(_Tp, _Sequence_value_type, _SameTypeConcept)
      __glibcxx_class_requires4(_Compare, bool, _Tp, _Tp,
				_BinaryFunctionConcept)

    public:
      typedef typename _Sequence::value_type                value_type;
      typedef typename _Sequence::reference                 reference;
      typedef typename _Sequence::const_reference           const_reference;
      typedef typename _Sequence::size_type                 size_type;
      typedef          _Sequence                            container_type;

    protected:
      //  See queue::c for notes on these names.
      _Sequence  c;
      _Compare   comp;

    public:
      /**
       *  @brief  Default constructor creates no elements.
       */
#ifndef __GXX_EXPERIMENTAL_CXX0X__
      explicit
      priority_queue(const _Compare& __x = _Compare(),
		     const _Sequence& __s = _Sequence())
      : c(__s), comp(__x)
      { std::make_heap(c.begin(), c.end(), comp); }
#else
      explicit
      priority_queue(const _Compare& __x,
		     const _Sequence& __s)
      : c(__s), comp(__x)
      { std::make_heap(c.begin(), c.end(), comp); }

      explicit
      priority_queue(const _Compare& __x = _Compare(),
		     _Sequence&& __s = _Sequence())
      : c(std::move(__s)), comp(__x)
      { std::make_heap(c.begin(), c.end(), comp); }
#endif

      /**
       *  @brief  Builds a %queue from a range.
       *  @param  first  An input iterator.
       *  @param  last  An input iterator.
       *  @param  x  A comparison functor describing a strict weak ordering.
       *  @param  s  An initial sequence with which to start.
       *
       *  Begins by copying @a s, inserting a copy of the elements
       *  from @a [first,last) into the copy of @a s, then ordering
       *  the copy according to @a x.
       *
       *  For more information on function objects, see the
       *  documentation on @link functors functor base
       *  classes@endlink.
       */
#ifndef __GXX_EXPERIMENTAL_CXX0X__
      template<typename _InputIterator>
        priority_queue(_InputIterator __first, _InputIterator __last,
		       const _Compare& __x = _Compare(),
		       const _Sequence& __s = _Sequence())
	: c(__s), comp(__x)
        {
	  __glibcxx_requires_valid_range(__first, __last);
	  c.insert(c.end(), __first, __last);
	  std::make_heap(c.begin(), c.end(), comp);
	}
#else
      template<typename _InputIterator>
        priority_queue(_InputIterator __first, _InputIterator __last,
		       const _Compare& __x,
		       const _Sequence& __s)
	: c(__s), comp(__x)
        {
	  __glibcxx_requires_valid_range(__first, __last);
	  c.insert(c.end(), __first, __last);
	  std::make_heap(c.begin(), c.end(), comp);
	}

      template<typename _InputIterator>
        priority_queue(_InputIterator __first, _InputIterator __last,
		       const _Compare& __x = _Compare(),
		       _Sequence&& __s = _Sequence())
	: c(std::move(__s)), comp(__x)
        {
	  __glibcxx_requires_valid_range(__first, __last);
	  c.insert(c.end(), __first, __last);
	  std::make_heap(c.begin(), c.end(), comp);
	}

      priority_queue(priority_queue&& __pq)
      : c(std::move(__pq.c)), comp(std::move(__pq.comp)) { }

      priority_queue&
      operator=(priority_queue&& __pq)
      {
	c = std::move(__pq.c);
	comp = std::move(__pq.comp);
	return *this;
      }
#endif

      /**
       *  Returns true if the %queue is empty.
       */
      bool
      empty() const
      { return c.empty(); }

      /**  Returns the number of elements in the %queue.  */
      size_type
      size() const
      { return c.size(); }

      /**
       *  Returns a read-only (constant) reference to the data at the first
       *  element of the %queue.
       */
      const_reference
      top() const
      {
	__glibcxx_requires_nonempty();
	return c.front();
      }

      /**
       *  @brief  Add data to the %queue.
       *  @param  x  Data to be added.
       *
       *  This is a typical %queue operation.
       *  The time complexity of the operation depends on the underlying
       *  sequence.
       */
      void
      push(const value_type& __x)
      {
	c.push_back(__x);
	std::push_heap(c.begin(), c.end(), comp);
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      void
      push(value_type&& __x)
      {
	c.push_back(std::move(__x));
	std::push_heap(c.begin(), c.end(), comp);
      }

      template<typename... _Args>
        void
        emplace(_Args&&... __args)
	{
	  c.emplace_back(std::forward<_Args>(__args)...);
	  std::push_heap(c.begin(), c.end(), comp);
	}
#endif

      /**
       *  @brief  Removes first element.
       *
       *  This is a typical %queue operation.  It shrinks the %queue
       *  by one.  The time complexity of the operation depends on the
       *  underlying sequence.
       *
       *  Note that no data is returned, and if the first element's
       *  data is needed, it should be retrieved before pop() is
       *  called.
       */
      void
      pop()
      {
	__glibcxx_requires_nonempty();
	std::pop_heap(c.begin(), c.end(), comp);
	c.pop_back();
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      void
      swap(priority_queue& __pq)
      {
	using std::swap;
	c.swap(__pq.c);
	swap(comp, __pq.comp);
      }
#endif
    };

  // No equality/comparison operators are provided for priority_queue.

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  template<typename _Tp, typename _Sequence, typename _Compare>
    inline void
    swap(priority_queue<_Tp, _Sequence, _Compare>& __x,
	 priority_queue<_Tp, _Sequence, _Compare>& __y)
    { __x.swap(__y); }
#endif

_GLIBCXX_END_NAMESPACE

#endif /* _STL_QUEUE_H */
