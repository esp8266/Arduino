// unordered_map implementation -*- C++ -*-

// Copyright (C) 2010-2013 Free Software Foundation, Inc.
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

/** @file bits/unordered_map.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{unordered_map}
 */

#ifndef _UNORDERED_MAP_H
#define _UNORDERED_MAP_H

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_CONTAINER

  /// Base types for unordered_map.
  template<bool _Cache>
    using __umap_traits = __detail::_Hashtable_traits<_Cache, false, true>;

  template<typename _Key,
	   typename _Tp,
	   typename _Hash = hash<_Key>,
	   typename _Pred = std::equal_to<_Key>,
	   typename _Alloc = std::allocator<std::pair<const _Key, _Tp> >,
	   typename _Tr = __umap_traits<__cache_default<_Key, _Hash>::value>>
    using __umap_hashtable = _Hashtable<_Key, std::pair<const _Key, _Tp>,
                                        _Alloc, __detail::_Select1st,
				        _Pred, _Hash,
				        __detail::_Mod_range_hashing,
				        __detail::_Default_ranged_hash,
				        __detail::_Prime_rehash_policy, _Tr>;

  /// Base types for unordered_multimap.
  template<bool _Cache>
    using __ummap_traits = __detail::_Hashtable_traits<_Cache, false, false>;

  template<typename _Key,
	   typename _Tp,
	   typename _Hash = hash<_Key>,
	   typename _Pred = std::equal_to<_Key>,
	   typename _Alloc = std::allocator<std::pair<const _Key, _Tp> >,
	   typename _Tr = __ummap_traits<__cache_default<_Key, _Hash>::value>>
    using __ummap_hashtable = _Hashtable<_Key, std::pair<const _Key, _Tp>,
					 _Alloc, __detail::_Select1st,
					 _Pred, _Hash,
					 __detail::_Mod_range_hashing,
					 __detail::_Default_ranged_hash,
					 __detail::_Prime_rehash_policy, _Tr>;

  /**
   *  @brief A standard container composed of unique keys (containing
   *  at most one of each key value) that associates values of another type
   *  with the keys.
   *
   *  @ingroup unordered_associative_containers
   *
   *  @tparam  _Key  Type of key objects.
   *  @tparam  _Tp  Type of mapped objects.
   *  @tparam  _Hash  Hashing function object type, defaults to hash<_Value>.
   *  @tparam  _Pred  Predicate function object type, defaults
   *                  to equal_to<_Value>.
   *  @tparam  _Alloc  Allocator type, defaults to allocator<_Key>.
   *
   *  Meets the requirements of a <a href="tables.html#65">container</a>, and
   *  <a href="tables.html#xx">unordered associative container</a>
   *
   * The resulting value type of the container is std::pair<const _Key, _Tp>.
   *
   *  Base is _Hashtable, dispatched at compile time via template
   *  alias __umap_hashtable.
   */
  template<class _Key, class _Tp,
	   class _Hash = hash<_Key>,
	   class _Pred = std::equal_to<_Key>,
	   class _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
    class unordered_map : __check_copy_constructible<_Alloc>
    {
      typedef __umap_hashtable<_Key, _Tp, _Hash, _Pred, _Alloc>  _Hashtable;
      _Hashtable _M_h;

    public:
      // typedefs:
      //@{
      /// Public typedefs.
      typedef typename _Hashtable::key_type	key_type;
      typedef typename _Hashtable::value_type	value_type;
      typedef typename _Hashtable::mapped_type	mapped_type;
      typedef typename _Hashtable::hasher	hasher;
      typedef typename _Hashtable::key_equal	key_equal;
      typedef typename _Hashtable::allocator_type allocator_type;
      //@}

      //@{
      ///  Iterator-related typedefs.
      typedef typename allocator_type::pointer		pointer;
      typedef typename allocator_type::const_pointer	const_pointer;
      typedef typename allocator_type::reference	reference;
      typedef typename allocator_type::const_reference	const_reference;
      typedef typename _Hashtable::iterator		iterator;
      typedef typename _Hashtable::const_iterator	const_iterator;
      typedef typename _Hashtable::local_iterator	local_iterator;
      typedef typename _Hashtable::const_local_iterator	const_local_iterator;
      typedef typename _Hashtable::size_type		size_type;
      typedef typename _Hashtable::difference_type	difference_type;
      //@}

      //construct/destroy/copy

      /**
       *  @brief  Default constructor creates no elements.
       *  @param __n  Initial number of buckets.
       *  @param __hf  A hash functor.
       *  @param __eql  A key equality functor.
       *  @param __a  An allocator object.
       */
      explicit
      unordered_map(size_type __n = 10,
		    const hasher& __hf = hasher(),
		    const key_equal& __eql = key_equal(),
		    const allocator_type& __a = allocator_type())
      : _M_h(__n, __hf, __eql, __a)
      { }

      /**
       *  @brief  Builds an %unordered_map from a range.
       *  @param  __first  An input iterator.
       *  @param  __last  An input iterator.
       *  @param __n  Minimal initial number of buckets.
       *  @param __hf  A hash functor.
       *  @param __eql  A key equality functor.
       *  @param __a  An allocator object.
       *
       *  Create an %unordered_map consisting of copies of the elements from
       *  [__first,__last).  This is linear in N (where N is
       *  distance(__first,__last)).
       */
      template<typename _InputIterator>
	unordered_map(_InputIterator __f, _InputIterator __l,
		      size_type __n = 0,
		      const hasher& __hf = hasher(),
		      const key_equal& __eql = key_equal(),
		      const allocator_type& __a = allocator_type())
	: _M_h(__f, __l, __n, __hf, __eql, __a)
	{ }

      /// Copy constructor.
      unordered_map(const unordered_map&) = default;

      /// Move constructor.
      unordered_map(unordered_map&&) = default;

      /**
       *  @brief  Builds an %unordered_map from an initializer_list.
       *  @param  __l  An initializer_list.
       *  @param __n  Minimal initial number of buckets.
       *  @param __hf  A hash functor.
       *  @param __eql  A key equality functor.
       *  @param  __a  An allocator object.
       *
       *  Create an %unordered_map consisting of copies of the elements in the
       *  list. This is linear in N (where N is @a __l.size()).
       */
      unordered_map(initializer_list<value_type> __l,
		    size_type __n = 0,
		    const hasher& __hf = hasher(),
		    const key_equal& __eql = key_equal(),
		    const allocator_type& __a = allocator_type())
	: _M_h(__l, __n, __hf, __eql, __a)
      { }

      /// Copy assignment operator.
      unordered_map&
      operator=(const unordered_map&) = default;

      /// Move assignment operator.
      unordered_map&
      operator=(unordered_map&&) = default;

      /**
       *  @brief  %Unordered_map list assignment operator.
       *  @param  __l  An initializer_list.
       *
       *  This function fills an %unordered_map with copies of the elements in
       *  the initializer list @a __l.
       *
       *  Note that the assignment completely changes the %unordered_map and
       *  that the resulting %unordered_map's size is the same as the number
       *  of elements assigned.  Old data may be lost.
       */
      unordered_map&
      operator=(initializer_list<value_type> __l)
      {
	_M_h = __l;
	return *this;
      }

      ///  Returns the allocator object with which the %unordered_map was
      ///  constructed.
      allocator_type
      get_allocator() const noexcept
      { return _M_h.get_allocator(); }

      // size and capacity:

      ///  Returns true if the %unordered_map is empty.
      bool
      empty() const noexcept
      { return _M_h.empty(); }

      ///  Returns the size of the %unordered_map.
      size_type
      size() const noexcept
      { return _M_h.size(); }

      ///  Returns the maximum size of the %unordered_map.
      size_type
      max_size() const noexcept
      { return _M_h.max_size(); }

      // iterators.

      /**
       *  Returns a read/write iterator that points to the first element in the
       *  %unordered_map.
       */
      iterator
      begin() noexcept
      { return _M_h.begin(); }

      //@{
      /**
       *  Returns a read-only (constant) iterator that points to the first
       *  element in the %unordered_map.
       */
      const_iterator
      begin() const noexcept
      { return _M_h.begin(); }

      const_iterator
      cbegin() const noexcept
      { return _M_h.begin(); }
      //@}

      /**
       *  Returns a read/write iterator that points one past the last element in
       *  the %unordered_map.
       */
      iterator
      end() noexcept
      { return _M_h.end(); }

      //@{
      /**
       *  Returns a read-only (constant) iterator that points one past the last
       *  element in the %unordered_map.
       */
      const_iterator
      end() const noexcept
      { return _M_h.end(); }

      const_iterator
      cend() const noexcept
      { return _M_h.end(); }
      //@}

      // modifiers.

      /**
       *  @brief Attempts to build and insert a std::pair into the %unordered_map.
       *
       *  @param __args  Arguments used to generate a new pair instance (see
       *	        std::piecewise_contruct for passing arguments to each
       *	        part of the pair constructor).
       *
       *  @return  A pair, of which the first element is an iterator that points
       *           to the possibly inserted pair, and the second is a bool that
       *           is true if the pair was actually inserted.
       *
       *  This function attempts to build and insert a (key, value) %pair into
       *  the %unordered_map.
       *  An %unordered_map relies on unique keys and thus a %pair is only
       *  inserted if its first element (the key) is not already present in the
       *  %unordered_map.
       *
       *  Insertion requires amortized constant time.
       */
      template<typename... _Args>
	std::pair<iterator, bool>
	emplace(_Args&&... __args)
	{ return _M_h.emplace(std::forward<_Args>(__args)...); }

      /**
       *  @brief Attempts to build and insert a std::pair into the %unordered_map.
       *
       *  @param  __pos  An iterator that serves as a hint as to where the pair
       *                should be inserted.
       *  @param  __args  Arguments used to generate a new pair instance (see
       *	         std::piecewise_contruct for passing arguments to each
       *	         part of the pair constructor).
       *  @return An iterator that points to the element with key of the
       *          std::pair built from @a __args (may or may not be that
       *          std::pair).
       *
       *  This function is not concerned about whether the insertion took place,
       *  and thus does not return a boolean like the single-argument emplace()
       *  does.
       *  Note that the first parameter is only a hint and can potentially
       *  improve the performance of the insertion process. A bad hint would
       *  cause no gains in efficiency.
       *
       *  See
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt07ch17.html
       *  for more on @a hinting.
       *
       *  Insertion requires amortized constant time.
       */
      template<typename... _Args>
	iterator
	emplace_hint(const_iterator __pos, _Args&&... __args)
	{ return _M_h.emplace_hint(__pos, std::forward<_Args>(__args)...); }

      //@{
      /**
       *  @brief Attempts to insert a std::pair into the %unordered_map.

       *  @param __x Pair to be inserted (see std::make_pair for easy
       *	     creation of pairs).
       *
       *  @return  A pair, of which the first element is an iterator that 
       *           points to the possibly inserted pair, and the second is 
       *           a bool that is true if the pair was actually inserted.
       *
       *  This function attempts to insert a (key, value) %pair into the
       *  %unordered_map. An %unordered_map relies on unique keys and thus a
       *  %pair is only inserted if its first element (the key) is not already
       *  present in the %unordered_map.
       *
       *  Insertion requires amortized constant time.
       */
      std::pair<iterator, bool>
      insert(const value_type& __x)
      { return _M_h.insert(__x); }

      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
	std::pair<iterator, bool>
	insert(_Pair&& __x)
        { return _M_h.insert(std::forward<_Pair>(__x)); }
      //@}

      //@{
      /**
       *  @brief Attempts to insert a std::pair into the %unordered_map.
       *  @param  __hint  An iterator that serves as a hint as to where the
       *                 pair should be inserted.
       *  @param  __x  Pair to be inserted (see std::make_pair for easy creation
       *               of pairs).
       *  @return An iterator that points to the element with key of
       *           @a __x (may or may not be the %pair passed in).
       *
       *  This function is not concerned about whether the insertion took place,
       *  and thus does not return a boolean like the single-argument insert()
       *  does.  Note that the first parameter is only a hint and can
       *  potentially improve the performance of the insertion process.  A bad
       *  hint would cause no gains in efficiency.
       *
       *  See
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt07ch17.html
       *  for more on @a hinting.
       *
       *  Insertion requires amortized constant time.
       */
      iterator
      insert(const_iterator __hint, const value_type& __x)
      { return _M_h.insert(__hint, __x); }

      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
	iterator
	insert(const_iterator __hint, _Pair&& __x)
	{ return _M_h.insert(__hint, std::forward<_Pair>(__x)); }
      //@}

      /**
       *  @brief A template function that attempts to insert a range of
       *  elements.
       *  @param  __first  Iterator pointing to the start of the range to be
       *                   inserted.
       *  @param  __last  Iterator pointing to the end of the range.
       *
       *  Complexity similar to that of the range constructor.
       */
      template<typename _InputIterator>
	void
	insert(_InputIterator __first, _InputIterator __last)
	{ _M_h.insert(__first, __last); }

      /**
       *  @brief Attempts to insert a list of elements into the %unordered_map.
       *  @param  __l  A std::initializer_list<value_type> of elements
       *               to be inserted.
       *
       *  Complexity similar to that of the range constructor.
       */
      void
      insert(initializer_list<value_type> __l)
      { _M_h.insert(__l); }

      //@{
      /**
       *  @brief Erases an element from an %unordered_map.
       *  @param  __position  An iterator pointing to the element to be erased.
       *  @return An iterator pointing to the element immediately following
       *          @a __position prior to the element being erased. If no such
       *          element exists, end() is returned.
       *
       *  This function erases an element, pointed to by the given iterator,
       *  from an %unordered_map.
       *  Note that this function only erases the element, and that if the
       *  element is itself a pointer, the pointed-to memory is not touched in
       *  any way.  Managing the pointer is the user's responsibility.
       */
      iterator
      erase(const_iterator __position)
      { return _M_h.erase(__position); }

      // LWG 2059.
      iterator
      erase(iterator __it)
      { return _M_h.erase(__it); }
      //@}

      /**
       *  @brief Erases elements according to the provided key.
       *  @param  __x  Key of element to be erased.
       *  @return  The number of elements erased.
       *
       *  This function erases all the elements located by the given key from
       *  an %unordered_map. For an %unordered_map the result of this function
       *  can only be 0 (not present) or 1 (present).
       *  Note that this function only erases the element, and that if the
       *  element is itself a pointer, the pointed-to memory is not touched in
       *  any way.  Managing the pointer is the user's responsibility.
       */
      size_type
      erase(const key_type& __x)
      { return _M_h.erase(__x); }

      /**
       *  @brief Erases a [__first,__last) range of elements from an
       *  %unordered_map.
       *  @param  __first  Iterator pointing to the start of the range to be
       *                  erased.
       *  @param __last  Iterator pointing to the end of the range to
       *                be erased.
       *  @return The iterator @a __last.
       *
       *  This function erases a sequence of elements from an %unordered_map.
       *  Note that this function only erases the elements, and that if
       *  the element is itself a pointer, the pointed-to memory is not touched
       *  in any way.  Managing the pointer is the user's responsibility.
       */
      iterator
      erase(const_iterator __first, const_iterator __last)
      { return _M_h.erase(__first, __last); }

      /**
       *  Erases all elements in an %unordered_map.
       *  Note that this function only erases the elements, and that if the
       *  elements themselves are pointers, the pointed-to memory is not touched
       *  in any way.  Managing the pointer is the user's responsibility.
       */
      void
      clear() noexcept
      { _M_h.clear(); }

      /**
       *  @brief  Swaps data with another %unordered_map.
       *  @param  __x  An %unordered_map of the same element and allocator
       *  types.
       *
       *  This exchanges the elements between two %unordered_map in constant time.
       *  Note that the global std::swap() function is specialized such that
       *  std::swap(m1,m2) will feed to this function.
       */
      void
      swap(unordered_map& __x)
      { _M_h.swap(__x._M_h); }

      // observers.

      ///  Returns the hash functor object with which the %unordered_map was
      ///  constructed.
      hasher
      hash_function() const
      { return _M_h.hash_function(); }

      ///  Returns the key comparison object with which the %unordered_map was
      ///  constructed.
      key_equal
      key_eq() const
      { return _M_h.key_eq(); }

      // lookup.

      //@{
      /**
       *  @brief Tries to locate an element in an %unordered_map.
       *  @param  __x  Key to be located.
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
      { return _M_h.find(__x); }

      const_iterator
      find(const key_type& __x) const
      { return _M_h.find(__x); }
      //@}

      /**
       *  @brief  Finds the number of elements.
       *  @param  __x  Key to count.
       *  @return  Number of elements with specified key.
       *
       *  This function only makes sense for %unordered_multimap; for
       *  %unordered_map the result will either be 0 (not present) or 1
       *  (present).
       */
      size_type
      count(const key_type& __x) const
      { return _M_h.count(__x); }

      //@{
      /**
       *  @brief Finds a subsequence matching given key.
       *  @param  __x  Key to be located.
       *  @return  Pair of iterators that possibly points to the subsequence
       *           matching given key.
       *
       *  This function probably only makes sense for %unordered_multimap.
       */
      std::pair<iterator, iterator>
      equal_range(const key_type& __x)
      { return _M_h.equal_range(__x); }

      std::pair<const_iterator, const_iterator>
      equal_range(const key_type& __x) const
      { return _M_h.equal_range(__x); }
      //@}

      //@{
      /**
       *  @brief  Subscript ( @c [] ) access to %unordered_map data.
       *  @param  __k  The key for which data should be retrieved.
       *  @return  A reference to the data of the (key,data) %pair.
       *
       *  Allows for easy lookup with the subscript ( @c [] )operator.  Returns
       *  data associated with the key specified in subscript.  If the key does
       *  not exist, a pair with that key is created using default values, which
       *  is then returned.
       *
       *  Lookup requires constant time.
       */
      mapped_type&
      operator[](const key_type& __k)
      { return _M_h[__k]; }

      mapped_type&
      operator[](key_type&& __k)
      { return _M_h[std::move(__k)]; }
      //@}

      //@{
      /**
       *  @brief  Access to %unordered_map data.
       *  @param  __k  The key for which data should be retrieved.
       *  @return  A reference to the data whose key is equal to @a __k, if
       *           such a data is present in the %unordered_map.
       *  @throw  std::out_of_range  If no such data is present.
       */
      mapped_type&
      at(const key_type& __k)
      { return _M_h.at(__k); }

      const mapped_type&
      at(const key_type& __k) const
      { return _M_h.at(__k); }
      //@}

      // bucket interface.

      /// Returns the number of buckets of the %unordered_map.
      size_type
      bucket_count() const noexcept
      { return _M_h.bucket_count(); }

      /// Returns the maximum number of buckets of the %unordered_map.
      size_type
      max_bucket_count() const noexcept
      { return _M_h.max_bucket_count(); }

      /*
       * @brief  Returns the number of elements in a given bucket.
       * @param  __n  A bucket index.
       * @return  The number of elements in the bucket.
       */
      size_type
      bucket_size(size_type __n) const
      { return _M_h.bucket_size(__n); }

      /*
       * @brief  Returns the bucket index of a given element.
       * @param  __key  A key instance.
       * @return  The key bucket index.
       */
      size_type
      bucket(const key_type& __key) const
      { return _M_h.bucket(__key); }
      
      /**
       *  @brief  Returns a read/write iterator pointing to the first bucket
       *         element.
       *  @param  __n The bucket index.
       *  @return  A read/write local iterator.
       */
      local_iterator
      begin(size_type __n)
      { return _M_h.begin(__n); }

      //@{
      /**
       *  @brief  Returns a read-only (constant) iterator pointing to the first
       *         bucket element.
       *  @param  __n The bucket index.
       *  @return  A read-only local iterator.
       */
      const_local_iterator
      begin(size_type __n) const
      { return _M_h.begin(__n); }

      const_local_iterator
      cbegin(size_type __n) const
      { return _M_h.cbegin(__n); }
      //@}

      /**
       *  @brief  Returns a read/write iterator pointing to one past the last
       *         bucket elements.
       *  @param  __n The bucket index.
       *  @return  A read/write local iterator.
       */
      local_iterator
      end(size_type __n)
      { return _M_h.end(__n); }

      //@{
      /**
       *  @brief  Returns a read-only (constant) iterator pointing to one past
       *         the last bucket elements.
       *  @param  __n The bucket index.
       *  @return  A read-only local iterator.
       */
      const_local_iterator
      end(size_type __n) const
      { return _M_h.end(__n); }

      const_local_iterator
      cend(size_type __n) const
      { return _M_h.cend(__n); }
      //@}

      // hash policy.

      /// Returns the average number of elements per bucket.
      float
      load_factor() const noexcept
      { return _M_h.load_factor(); }

      /// Returns a positive number that the %unordered_map tries to keep the
      /// load factor less than or equal to.
      float
      max_load_factor() const noexcept
      { return _M_h.max_load_factor(); }

      /**
       *  @brief  Change the %unordered_map maximum load factor.
       *  @param  __z The new maximum load factor.
       */
      void
      max_load_factor(float __z)
      { _M_h.max_load_factor(__z); }

      /**
       *  @brief  May rehash the %unordered_map.
       *  @param  __n The new number of buckets.
       *
       *  Rehash will occur only if the new number of buckets respect the
       *  %unordered_map maximum load factor.
       */
      void
      rehash(size_type __n)
      { _M_h.rehash(__n); }

      /**
       *  @brief  Prepare the %unordered_map for a specified number of
       *          elements.
       *  @param  __n Number of elements required.
       *
       *  Same as rehash(ceil(n / max_load_factor())).
       */
      void
      reserve(size_type __n)
      { _M_h.reserve(__n); }

      template<typename _Key1, typename _Tp1, typename _Hash1, typename _Pred1,
	       typename _Alloc1>
        friend bool
      operator==(const unordered_map<_Key1, _Tp1, _Hash1, _Pred1, _Alloc1>&,
		 const unordered_map<_Key1, _Tp1, _Hash1, _Pred1, _Alloc1>&);
    };

  /**
   *  @brief A standard container composed of equivalent keys
   *  (possibly containing multiple of each key value) that associates
   *  values of another type with the keys.
   *
   *  @ingroup unordered_associative_containers
   *
   *  @tparam  _Key  Type of key objects.
   *  @tparam  _Tp  Type of mapped objects.
   *  @tparam  _Hash  Hashing function object type, defaults to hash<_Value>.
   *  @tparam  _Pred  Predicate function object type, defaults
   *                  to equal_to<_Value>.
   *  @tparam  _Alloc  Allocator type, defaults to allocator<_Key>.
   *
   *  Meets the requirements of a <a href="tables.html#65">container</a>, and
   *  <a href="tables.html#xx">unordered associative container</a>
   *
   * The resulting value type of the container is std::pair<const _Key, _Tp>.
   *
   *  Base is _Hashtable, dispatched at compile time via template
   *  alias __ummap_hashtable.
   */
  template<class _Key, class _Tp,
	   class _Hash = hash<_Key>,
	   class _Pred = std::equal_to<_Key>,
	   class _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
    class unordered_multimap : __check_copy_constructible<_Alloc>
    {
      typedef __ummap_hashtable<_Key, _Tp, _Hash, _Pred, _Alloc>  _Hashtable;
      _Hashtable _M_h;

    public:
      // typedefs:
      //@{
      /// Public typedefs.
      typedef typename _Hashtable::key_type	key_type;
      typedef typename _Hashtable::value_type	value_type;
      typedef typename _Hashtable::mapped_type	mapped_type;
      typedef typename _Hashtable::hasher	hasher;
      typedef typename _Hashtable::key_equal	key_equal;
      typedef typename _Hashtable::allocator_type allocator_type;
      //@}

      //@{
      ///  Iterator-related typedefs.
      typedef typename allocator_type::pointer		pointer;
      typedef typename allocator_type::const_pointer	const_pointer;
      typedef typename allocator_type::reference	reference;
      typedef typename allocator_type::const_reference	const_reference;
      typedef typename _Hashtable::iterator		iterator;
      typedef typename _Hashtable::const_iterator	const_iterator;
      typedef typename _Hashtable::local_iterator	local_iterator;
      typedef typename _Hashtable::const_local_iterator	const_local_iterator;
      typedef typename _Hashtable::size_type		size_type;
      typedef typename _Hashtable::difference_type	difference_type;
      //@}

      //construct/destroy/copy

      /**
       *  @brief  Default constructor creates no elements.
       *  @param __n  Initial number of buckets.
       *  @param __hf  A hash functor.
       *  @param __eql  A key equality functor.
       *  @param __a  An allocator object.
       */
      explicit
      unordered_multimap(size_type __n = 10,
			 const hasher& __hf = hasher(),
			 const key_equal& __eql = key_equal(),
			 const allocator_type& __a = allocator_type())
      : _M_h(__n, __hf, __eql, __a)
      { }

      /**
       *  @brief  Builds an %unordered_multimap from a range.
       *  @param  __first  An input iterator.
       *  @param  __last  An input iterator.
       *  @param __n  Minimal initial number of buckets.
       *  @param __hf  A hash functor.
       *  @param __eql  A key equality functor.
       *  @param __a  An allocator object.
       *
       *  Create an %unordered_multimap consisting of copies of the elements
       *  from [__first,__last).  This is linear in N (where N is
       *  distance(__first,__last)).
       */
      template<typename _InputIterator>
	unordered_multimap(_InputIterator __f, _InputIterator __l,
			   size_type __n = 0,
			   const hasher& __hf = hasher(),
			   const key_equal& __eql = key_equal(),
			   const allocator_type& __a = allocator_type())
	: _M_h(__f, __l, __n, __hf, __eql, __a)
	{ }

      /// Copy constructor.
      unordered_multimap(const unordered_multimap&) = default;

      /// Move constructor.
      unordered_multimap(unordered_multimap&&) = default;

      /**
       *  @brief  Builds an %unordered_multimap from an initializer_list.
       *  @param  __l  An initializer_list.
       *  @param __n  Minimal initial number of buckets.
       *  @param __hf  A hash functor.
       *  @param __eql  A key equality functor.
       *  @param  __a  An allocator object.
       *
       *  Create an %unordered_multimap consisting of copies of the elements in
       *  the list. This is linear in N (where N is @a __l.size()).
       */
      unordered_multimap(initializer_list<value_type> __l,
			 size_type __n = 0,
			 const hasher& __hf = hasher(),
			 const key_equal& __eql = key_equal(),
			 const allocator_type& __a = allocator_type())
	: _M_h(__l, __n, __hf, __eql, __a)
      { }

      /// Copy assignment operator.
      unordered_multimap&
      operator=(const unordered_multimap&) = default;

      /// Move assignment operator.
      unordered_multimap&
      operator=(unordered_multimap&&) = default;

      /**
       *  @brief  %Unordered_multimap list assignment operator.
       *  @param  __l  An initializer_list.
       *
       *  This function fills an %unordered_multimap with copies of the elements
       *  in the initializer list @a __l.
       *
       *  Note that the assignment completely changes the %unordered_multimap
       *  and that the resulting %unordered_multimap's size is the same as the
       *  number of elements assigned.  Old data may be lost.
       */
      unordered_multimap&
      operator=(initializer_list<value_type> __l)
      {
	_M_h = __l;
	return *this;
      }

      ///  Returns the allocator object with which the %unordered_multimap was
      ///  constructed.
      allocator_type
      get_allocator() const noexcept
      { return _M_h.get_allocator(); }

      // size and capacity:

      ///  Returns true if the %unordered_multimap is empty.
      bool
      empty() const noexcept
      { return _M_h.empty(); }

      ///  Returns the size of the %unordered_multimap.
      size_type
      size() const noexcept
      { return _M_h.size(); }

      ///  Returns the maximum size of the %unordered_multimap.
      size_type
      max_size() const noexcept
      { return _M_h.max_size(); }

      // iterators.

      /**
       *  Returns a read/write iterator that points to the first element in the
       *  %unordered_multimap.
       */
      iterator
      begin() noexcept
      { return _M_h.begin(); }

      //@{
      /**
       *  Returns a read-only (constant) iterator that points to the first
       *  element in the %unordered_multimap.
       */
      const_iterator
      begin() const noexcept
      { return _M_h.begin(); }

      const_iterator
      cbegin() const noexcept
      { return _M_h.begin(); }
      //@}

      /**
       *  Returns a read/write iterator that points one past the last element in
       *  the %unordered_multimap.
       */
      iterator
      end() noexcept
      { return _M_h.end(); }

      //@{
      /**
       *  Returns a read-only (constant) iterator that points one past the last
       *  element in the %unordered_multimap.
       */
      const_iterator
      end() const noexcept
      { return _M_h.end(); }

      const_iterator
      cend() const noexcept
      { return _M_h.end(); }
      //@}

      // modifiers.

      /**
       *  @brief Attempts to build and insert a std::pair into the
       *  %unordered_multimap.
       *
       *  @param __args  Arguments used to generate a new pair instance (see
       *	        std::piecewise_contruct for passing arguments to each
       *	        part of the pair constructor).
       *
       *  @return  An iterator that points to the inserted pair.
       *
       *  This function attempts to build and insert a (key, value) %pair into
       *  the %unordered_multimap.
       *
       *  Insertion requires amortized constant time.
       */
      template<typename... _Args>
	iterator
	emplace(_Args&&... __args)
	{ return _M_h.emplace(std::forward<_Args>(__args)...); }

      /**
       *  @brief Attempts to build and insert a std::pair into the %unordered_multimap.
       *
       *  @param  __pos  An iterator that serves as a hint as to where the pair
       *                should be inserted.
       *  @param  __args  Arguments used to generate a new pair instance (see
       *	         std::piecewise_contruct for passing arguments to each
       *	         part of the pair constructor).
       *  @return An iterator that points to the element with key of the
       *          std::pair built from @a __args.
       *
       *  Note that the first parameter is only a hint and can potentially
       *  improve the performance of the insertion process. A bad hint would
       *  cause no gains in efficiency.
       *
       *  See
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt07ch17.html
       *  for more on @a hinting.
       *
       *  Insertion requires amortized constant time.
       */
      template<typename... _Args>
	iterator
	emplace_hint(const_iterator __pos, _Args&&... __args)
	{ return _M_h.emplace_hint(__pos, std::forward<_Args>(__args)...); }

      //@{
      /**
       *  @brief Inserts a std::pair into the %unordered_multimap.
       *  @param __x Pair to be inserted (see std::make_pair for easy
       *	     creation of pairs).
       *
       *  @return  An iterator that points to the inserted pair.
       *
       *  Insertion requires amortized constant time.
       */
      iterator
      insert(const value_type& __x)
      { return _M_h.insert(__x); }

      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
	iterator
	insert(_Pair&& __x)
        { return _M_h.insert(std::forward<_Pair>(__x)); }
      //@}

      //@{
      /**
       *  @brief Inserts a std::pair into the %unordered_multimap.
       *  @param  __hint  An iterator that serves as a hint as to where the
       *                 pair should be inserted.
       *  @param  __x  Pair to be inserted (see std::make_pair for easy creation
       *               of pairs).
       *  @return An iterator that points to the element with key of
       *           @a __x (may or may not be the %pair passed in).
       *
       *  Note that the first parameter is only a hint and can potentially
       *  improve the performance of the insertion process.  A bad hint would
       *  cause no gains in efficiency.
       *
       *  See
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt07ch17.html
       *  for more on @a hinting.
       *
       *  Insertion requires amortized constant time.
       */
      iterator
      insert(const_iterator __hint, const value_type& __x)
      { return _M_h.insert(__hint, __x); }

      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
	iterator
	insert(const_iterator __hint, _Pair&& __x)
        { return _M_h.insert(__hint, std::forward<_Pair>(__x)); }
      //@}

      /**
       *  @brief A template function that attempts to insert a range of
       *  elements.
       *  @param  __first  Iterator pointing to the start of the range to be
       *                   inserted.
       *  @param  __last  Iterator pointing to the end of the range.
       *
       *  Complexity similar to that of the range constructor.
       */
      template<typename _InputIterator>
	void
	insert(_InputIterator __first, _InputIterator __last)
	{ _M_h.insert(__first, __last); }

      /**
       *  @brief Attempts to insert a list of elements into the
       *  %unordered_multimap.
       *  @param  __l  A std::initializer_list<value_type> of elements
       *               to be inserted.
       *
       *  Complexity similar to that of the range constructor.
       */
      void
      insert(initializer_list<value_type> __l)
      { _M_h.insert(__l); }

      //@{
      /**
       *  @brief Erases an element from an %unordered_multimap.
       *  @param  __position  An iterator pointing to the element to be erased.
       *  @return An iterator pointing to the element immediately following
       *          @a __position prior to the element being erased. If no such
       *          element exists, end() is returned.
       *
       *  This function erases an element, pointed to by the given iterator,
       *  from an %unordered_multimap.
       *  Note that this function only erases the element, and that if the
       *  element is itself a pointer, the pointed-to memory is not touched in
       *  any way.  Managing the pointer is the user's responsibility.
       */
      iterator
      erase(const_iterator __position)
      { return _M_h.erase(__position); }

      // LWG 2059.
      iterator
      erase(iterator __it)
      { return _M_h.erase(__it); }
      //@}

      /**
       *  @brief Erases elements according to the provided key.
       *  @param  __x  Key of elements to be erased.
       *  @return  The number of elements erased.
       *
       *  This function erases all the elements located by the given key from
       *  an %unordered_multimap.
       *  Note that this function only erases the element, and that if the
       *  element is itself a pointer, the pointed-to memory is not touched in
       *  any way.  Managing the pointer is the user's responsibility.
       */
      size_type
      erase(const key_type& __x)
      { return _M_h.erase(__x); }

      /**
       *  @brief Erases a [__first,__last) range of elements from an
       *  %unordered_multimap.
       *  @param  __first  Iterator pointing to the start of the range to be
       *                  erased.
       *  @param __last  Iterator pointing to the end of the range to
       *                be erased.
       *  @return The iterator @a __last.
       *
       *  This function erases a sequence of elements from an
       *  %unordered_multimap.
       *  Note that this function only erases the elements, and that if
       *  the element is itself a pointer, the pointed-to memory is not touched
       *  in any way.  Managing the pointer is the user's responsibility.
       */
      iterator
      erase(const_iterator __first, const_iterator __last)
      { return _M_h.erase(__first, __last); }

      /**
       *  Erases all elements in an %unordered_multimap.
       *  Note that this function only erases the elements, and that if the
       *  elements themselves are pointers, the pointed-to memory is not touched
       *  in any way.  Managing the pointer is the user's responsibility.
       */
      void
      clear() noexcept
      { _M_h.clear(); }

      /**
       *  @brief  Swaps data with another %unordered_multimap.
       *  @param  __x  An %unordered_multimap of the same element and allocator
       *  types.
       *
       *  This exchanges the elements between two %unordered_multimap in
       *  constant time.
       *  Note that the global std::swap() function is specialized such that
       *  std::swap(m1,m2) will feed to this function.
       */
      void
      swap(unordered_multimap& __x)
      { _M_h.swap(__x._M_h); }

      // observers.

      ///  Returns the hash functor object with which the %unordered_multimap
      ///  was constructed.
      hasher
      hash_function() const
      { return _M_h.hash_function(); }

      ///  Returns the key comparison object with which the %unordered_multimap
      ///  was constructed.
      key_equal
      key_eq() const
      { return _M_h.key_eq(); }

      // lookup.

      //@{
      /**
       *  @brief Tries to locate an element in an %unordered_multimap.
       *  @param  __x  Key to be located.
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
      { return _M_h.find(__x); }

      const_iterator
      find(const key_type& __x) const
      { return _M_h.find(__x); }
      //@}

      /**
       *  @brief  Finds the number of elements.
       *  @param  __x  Key to count.
       *  @return  Number of elements with specified key.
       */
      size_type
      count(const key_type& __x) const
      { return _M_h.count(__x); }

      //@{
      /**
       *  @brief Finds a subsequence matching given key.
       *  @param  __x  Key to be located.
       *  @return  Pair of iterators that possibly points to the subsequence
       *           matching given key.
       */
      std::pair<iterator, iterator>
      equal_range(const key_type& __x)
      { return _M_h.equal_range(__x); }

      std::pair<const_iterator, const_iterator>
      equal_range(const key_type& __x) const
      { return _M_h.equal_range(__x); }
      //@}

      // bucket interface.

      /// Returns the number of buckets of the %unordered_multimap.
      size_type
      bucket_count() const noexcept
      { return _M_h.bucket_count(); }

      /// Returns the maximum number of buckets of the %unordered_multimap.
      size_type
      max_bucket_count() const noexcept
      { return _M_h.max_bucket_count(); }

      /*
       * @brief  Returns the number of elements in a given bucket.
       * @param  __n  A bucket index.
       * @return  The number of elements in the bucket.
       */
      size_type
      bucket_size(size_type __n) const
      { return _M_h.bucket_size(__n); }

      /*
       * @brief  Returns the bucket index of a given element.
       * @param  __key  A key instance.
       * @return  The key bucket index.
       */
      size_type
      bucket(const key_type& __key) const
      { return _M_h.bucket(__key); }
      
      /**
       *  @brief  Returns a read/write iterator pointing to the first bucket
       *         element.
       *  @param  __n The bucket index.
       *  @return  A read/write local iterator.
       */
      local_iterator
      begin(size_type __n)
      { return _M_h.begin(__n); }

      //@{
      /**
       *  @brief  Returns a read-only (constant) iterator pointing to the first
       *         bucket element.
       *  @param  __n The bucket index.
       *  @return  A read-only local iterator.
       */
      const_local_iterator
      begin(size_type __n) const
      { return _M_h.begin(__n); }

      const_local_iterator
      cbegin(size_type __n) const
      { return _M_h.cbegin(__n); }
      //@}

      /**
       *  @brief  Returns a read/write iterator pointing to one past the last
       *         bucket elements.
       *  @param  __n The bucket index.
       *  @return  A read/write local iterator.
       */
      local_iterator
      end(size_type __n)
      { return _M_h.end(__n); }

      //@{
      /**
       *  @brief  Returns a read-only (constant) iterator pointing to one past
       *         the last bucket elements.
       *  @param  __n The bucket index.
       *  @return  A read-only local iterator.
       */
      const_local_iterator
      end(size_type __n) const
      { return _M_h.end(__n); }

      const_local_iterator
      cend(size_type __n) const
      { return _M_h.cend(__n); }
      //@}

      // hash policy.

      /// Returns the average number of elements per bucket.
      float
      load_factor() const noexcept
      { return _M_h.load_factor(); }

      /// Returns a positive number that the %unordered_multimap tries to keep
      /// the load factor less than or equal to.
      float
      max_load_factor() const noexcept
      { return _M_h.max_load_factor(); }

      /**
       *  @brief  Change the %unordered_multimap maximum load factor.
       *  @param  __z The new maximum load factor.
       */
      void
      max_load_factor(float __z)
      { _M_h.max_load_factor(__z); }

      /**
       *  @brief  May rehash the %unordered_multimap.
       *  @param  __n The new number of buckets.
       *
       *  Rehash will occur only if the new number of buckets respect the
       *  %unordered_multimap maximum load factor.
       */
      void
      rehash(size_type __n)
      { _M_h.rehash(__n); }

      /**
       *  @brief  Prepare the %unordered_multimap for a specified number of
       *          elements.
       *  @param  __n Number of elements required.
       *
       *  Same as rehash(ceil(n / max_load_factor())).
       */
      void
      reserve(size_type __n)
      { _M_h.reserve(__n); }

      template<typename _Key1, typename _Tp1, typename _Hash1, typename _Pred1,
	       typename _Alloc1>
        friend bool
	operator==(const unordered_multimap<_Key1, _Tp1,
					    _Hash1, _Pred1, _Alloc1>&,
		   const unordered_multimap<_Key1, _Tp1,
					    _Hash1, _Pred1, _Alloc1>&);
    };

  template<class _Key, class _Tp, class _Hash, class _Pred, class _Alloc>
    inline void
    swap(unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>& __x,
	 unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>& __y)
    { __x.swap(__y); }

  template<class _Key, class _Tp, class _Hash, class _Pred, class _Alloc>
    inline void
    swap(unordered_multimap<_Key, _Tp, _Hash, _Pred, _Alloc>& __x,
	 unordered_multimap<_Key, _Tp, _Hash, _Pred, _Alloc>& __y)
    { __x.swap(__y); }

  template<class _Key, class _Tp, class _Hash, class _Pred, class _Alloc>
    inline bool
    operator==(const unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>& __x,
	       const unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>& __y)
    { return __x._M_h._M_equal(__y._M_h); }

  template<class _Key, class _Tp, class _Hash, class _Pred, class _Alloc>
    inline bool
    operator!=(const unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>& __x,
	       const unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>& __y)
    { return !(__x == __y); }

  template<class _Key, class _Tp, class _Hash, class _Pred, class _Alloc>
    inline bool
    operator==(const unordered_multimap<_Key, _Tp, _Hash, _Pred, _Alloc>& __x,
	       const unordered_multimap<_Key, _Tp, _Hash, _Pred, _Alloc>& __y)
    { return __x._M_h._M_equal(__y._M_h); }

  template<class _Key, class _Tp, class _Hash, class _Pred, class _Alloc>
    inline bool
    operator!=(const unordered_multimap<_Key, _Tp, _Hash, _Pred, _Alloc>& __x,
	       const unordered_multimap<_Key, _Tp, _Hash, _Pred, _Alloc>& __y)
    { return !(__x == __y); }

_GLIBCXX_END_NAMESPACE_CONTAINER
} // namespace std

#endif /* _UNORDERED_MAP_H */
