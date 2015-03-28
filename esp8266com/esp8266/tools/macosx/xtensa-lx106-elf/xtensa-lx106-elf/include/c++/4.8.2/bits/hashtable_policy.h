// Internal policy header for unordered_set and unordered_map -*- C++ -*-

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

/** @file bits/hashtable_policy.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly.
 *  @headername{unordered_map,unordered_set}
 */

#ifndef _HASHTABLE_POLICY_H
#define _HASHTABLE_POLICY_H 1

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    class _Hashtable;

_GLIBCXX_END_NAMESPACE_VERSION

namespace __detail
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  @defgroup hashtable-detail Base and Implementation Classes
   *  @ingroup unordered_associative_containers
   *  @{
   */
  template<typename _Key, typename _Value,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash, typename _Traits>
    struct _Hashtable_base;

  // Helper function: return distance(first, last) for forward
  // iterators, or 0 for input iterators.
  template<class _Iterator>
    inline typename std::iterator_traits<_Iterator>::difference_type
    __distance_fw(_Iterator __first, _Iterator __last,
		  std::input_iterator_tag)
    { return 0; }

  template<class _Iterator>
    inline typename std::iterator_traits<_Iterator>::difference_type
    __distance_fw(_Iterator __first, _Iterator __last,
		  std::forward_iterator_tag)
    { return std::distance(__first, __last); }

  template<class _Iterator>
    inline typename std::iterator_traits<_Iterator>::difference_type
    __distance_fw(_Iterator __first, _Iterator __last)
    {
      typedef typename std::iterator_traits<_Iterator>::iterator_category _Tag;
      return __distance_fw(__first, __last, _Tag());
    }

  // Helper type used to detect whether the hash functor is noexcept.
  template <typename _Key, typename _Hash>
    struct __is_noexcept_hash : std::integral_constant<bool,
	noexcept(declval<const _Hash&>()(declval<const _Key&>()))>
    { };

  struct _Identity
  {
    template<typename _Tp>
      _Tp&&
      operator()(_Tp&& __x) const
      { return std::forward<_Tp>(__x); }
  };

  struct _Select1st
  {
    template<typename _Tp>
      auto
      operator()(_Tp&& __x) const
      -> decltype(std::get<0>(std::forward<_Tp>(__x)))
      { return std::get<0>(std::forward<_Tp>(__x)); }
  };

  // Auxiliary types used for all instantiations of _Hashtable nodes
  // and iterators.

  /**
   *  struct _Hashtable_traits
   *
   *  Important traits for hash tables.
   *
   *  @tparam _Cache_hash_code  Boolean value. True if the value of
   *  the hash function is stored along with the value. This is a
   *  time-space tradeoff.  Storing it may improve lookup speed by
   *  reducing the number of times we need to call the _Equal
   *  function.
   *
   *  @tparam _Constant_iterators  Boolean value. True if iterator and
   *  const_iterator are both constant iterator types. This is true
   *  for unordered_set and unordered_multiset, false for
   *  unordered_map and unordered_multimap.
   *
   *  @tparam _Unique_keys  Boolean value. True if the return value
   *  of _Hashtable::count(k) is always at most one, false if it may
   *  be an arbitrary number. This is true for unordered_set and
   *  unordered_map, false for unordered_multiset and
   *  unordered_multimap.
   */
  template<bool _Cache_hash_code, bool _Constant_iterators, bool _Unique_keys>
    struct _Hashtable_traits
    {
      template<bool _Cond>
	using __bool_constant = integral_constant<bool, _Cond>;

      using __hash_cached = __bool_constant<_Cache_hash_code>;
      using __constant_iterators = __bool_constant<_Constant_iterators>;
      using __unique_keys = __bool_constant<_Unique_keys>;
    };

  /**
   *  struct _Hash_node_base
   *
   *  Nodes, used to wrap elements stored in the hash table.  A policy
   *  template parameter of class template _Hashtable controls whether
   *  nodes also store a hash code. In some cases (e.g. strings) this
   *  may be a performance win.
   */
  struct _Hash_node_base
  {
    _Hash_node_base* _M_nxt;

    _Hash_node_base() : _M_nxt() { }

    _Hash_node_base(_Hash_node_base* __next) : _M_nxt(__next) { }
  };

  /**
   *  Primary template struct _Hash_node.
   */
  template<typename _Value, bool _Cache_hash_code>
    struct _Hash_node;

  /**
   *  Specialization for nodes with caches, struct _Hash_node.
   *
   *  Base class is __detail::_Hash_node_base.
   */
  template<typename _Value>
    struct _Hash_node<_Value, true> : _Hash_node_base
    {
      _Value       _M_v;
      std::size_t  _M_hash_code;

      template<typename... _Args>
	_Hash_node(_Args&&... __args)
	: _M_v(std::forward<_Args>(__args)...), _M_hash_code() { }

      _Hash_node*
      _M_next() const { return static_cast<_Hash_node*>(_M_nxt); }
    };

  /**
   *  Specialization for nodes without caches, struct _Hash_node.
   *
   *  Base class is __detail::_Hash_node_base.
   */
  template<typename _Value>
    struct _Hash_node<_Value, false> : _Hash_node_base
    {
      _Value       _M_v;

      template<typename... _Args>
	_Hash_node(_Args&&... __args)
	: _M_v(std::forward<_Args>(__args)...) { }

      _Hash_node*
      _M_next() const { return static_cast<_Hash_node*>(_M_nxt); }
    };

  /// Base class for node iterators.
  template<typename _Value, bool _Cache_hash_code>
    struct _Node_iterator_base
    {
      using __node_type = _Hash_node<_Value, _Cache_hash_code>;

      __node_type*  _M_cur;

      _Node_iterator_base(__node_type* __p)
      : _M_cur(__p) { }

      void
      _M_incr()
      { _M_cur = _M_cur->_M_next(); }
    };

  template<typename _Value, bool _Cache_hash_code>
    inline bool
    operator==(const _Node_iterator_base<_Value, _Cache_hash_code>& __x,
	       const _Node_iterator_base<_Value, _Cache_hash_code >& __y)
    { return __x._M_cur == __y._M_cur; }

  template<typename _Value, bool _Cache_hash_code>
    inline bool
    operator!=(const _Node_iterator_base<_Value, _Cache_hash_code>& __x,
	       const _Node_iterator_base<_Value, _Cache_hash_code>& __y)
    { return __x._M_cur != __y._M_cur; }

  /// Node iterators, used to iterate through all the hashtable.
  template<typename _Value, bool __constant_iterators, bool __cache>
    struct _Node_iterator
    : public _Node_iterator_base<_Value, __cache>
    {
    private:
      using __base_type = _Node_iterator_base<_Value, __cache>;
      using __node_type = typename __base_type::__node_type;

    public:
      typedef _Value                                   value_type;
      typedef std::ptrdiff_t                           difference_type;
      typedef std::forward_iterator_tag                iterator_category;

      using pointer = typename std::conditional<__constant_iterators,
						const _Value*, _Value*>::type;

      using reference = typename std::conditional<__constant_iterators,
						  const _Value&, _Value&>::type;

      _Node_iterator()
      : __base_type(0) { }

      explicit
      _Node_iterator(__node_type* __p)
      : __base_type(__p) { }

      reference
      operator*() const
      { return this->_M_cur->_M_v; }

      pointer
      operator->() const
      { return std::__addressof(this->_M_cur->_M_v); }

      _Node_iterator&
      operator++()
      {
	this->_M_incr();
	return *this;
      }

      _Node_iterator
      operator++(int)
      {
	_Node_iterator __tmp(*this);
	this->_M_incr();
	return __tmp;
      }
    };

  /// Node const_iterators, used to iterate through all the hashtable.
  template<typename _Value, bool __constant_iterators, bool __cache>
    struct _Node_const_iterator
    : public _Node_iterator_base<_Value, __cache>
    {
    private:
      using __base_type = _Node_iterator_base<_Value, __cache>;
      using __node_type = typename __base_type::__node_type;

    public:
      typedef _Value                                   value_type;
      typedef std::ptrdiff_t                           difference_type;
      typedef std::forward_iterator_tag                iterator_category;

      typedef const _Value*                            pointer;
      typedef const _Value&                            reference;

      _Node_const_iterator()
      : __base_type(0) { }

      explicit
      _Node_const_iterator(__node_type* __p)
      : __base_type(__p) { }

      _Node_const_iterator(const _Node_iterator<_Value, __constant_iterators,
			   __cache>& __x)
      : __base_type(__x._M_cur) { }

      reference
      operator*() const
      { return this->_M_cur->_M_v; }

      pointer
      operator->() const
      { return std::__addressof(this->_M_cur->_M_v); }

      _Node_const_iterator&
      operator++()
      {
	this->_M_incr();
	return *this;
      }

      _Node_const_iterator
      operator++(int)
      {
	_Node_const_iterator __tmp(*this);
	this->_M_incr();
	return __tmp;
      }
    };

  // Many of class template _Hashtable's template parameters are policy
  // classes.  These are defaults for the policies.

  /// Default range hashing function: use division to fold a large number
  /// into the range [0, N).
  struct _Mod_range_hashing
  {
    typedef std::size_t first_argument_type;
    typedef std::size_t second_argument_type;
    typedef std::size_t result_type;

    result_type
    operator()(first_argument_type __num, second_argument_type __den) const
    { return __num % __den; }
  };

  /// Default ranged hash function H.  In principle it should be a
  /// function object composed from objects of type H1 and H2 such that
  /// h(k, N) = h2(h1(k), N), but that would mean making extra copies of
  /// h1 and h2.  So instead we'll just use a tag to tell class template
  /// hashtable to do that composition.
  struct _Default_ranged_hash { };

  /// Default value for rehash policy.  Bucket size is (usually) the
  /// smallest prime that keeps the load factor small enough.
  struct _Prime_rehash_policy
  {
    _Prime_rehash_policy(float __z = 1.0)
    : _M_max_load_factor(__z), _M_next_resize(0) { }

    float
    max_load_factor() const noexcept
    { return _M_max_load_factor; }

    // Return a bucket size no smaller than n.
    std::size_t
    _M_next_bkt(std::size_t __n) const;

    // Return a bucket count appropriate for n elements
    std::size_t
    _M_bkt_for_elements(std::size_t __n) const
    { return __builtin_ceil(__n / (long double)_M_max_load_factor); }

    // __n_bkt is current bucket count, __n_elt is current element count,
    // and __n_ins is number of elements to be inserted.  Do we need to
    // increase bucket count?  If so, return make_pair(true, n), where n
    // is the new bucket count.  If not, return make_pair(false, 0).
    std::pair<bool, std::size_t>
    _M_need_rehash(std::size_t __n_bkt, std::size_t __n_elt,
		   std::size_t __n_ins) const;

    typedef std::size_t _State;

    _State
    _M_state() const
    { return _M_next_resize; }

    void
    _M_reset(_State __state)
    { _M_next_resize = __state; }

    enum { _S_n_primes = sizeof(unsigned long) != 8 ? 256 : 256 + 48 };

    static const std::size_t _S_growth_factor = 2;

    float                _M_max_load_factor;
    mutable std::size_t  _M_next_resize;
  };

  // Base classes for std::_Hashtable.  We define these base classes
  // because in some cases we want to do different things depending on
  // the value of a policy class.  In some cases the policy class
  // affects which member functions and nested typedefs are defined;
  // we handle that by specializing base class templates.  Several of
  // the base class templates need to access other members of class
  // template _Hashtable, so we use a variant of the "Curiously
  // Recurring Template Pattern" (CRTP) technique.

  /**
   *  Primary class template _Map_base.
   *
   *  If the hashtable has a value type of the form pair<T1, T2> and a
   *  key extraction policy (_ExtractKey) that returns the first part
   *  of the pair, the hashtable gets a mapped_type typedef.  If it
   *  satisfies those criteria and also has unique keys, then it also
   *  gets an operator[].
   */
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits,
	   bool _Unique_keys = _Traits::__unique_keys::value>
    struct _Map_base { };

  /// Partial specialization, __unique_keys set to false.
  template<typename _Key, typename _Pair, typename _Alloc, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
		     _H1, _H2, _Hash, _RehashPolicy, _Traits, false>
    {
      using mapped_type = typename std::tuple_element<1, _Pair>::type;
    };

  /// Partial specialization, __unique_keys set to true.
  template<typename _Key, typename _Pair, typename _Alloc, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
		     _H1, _H2, _Hash, _RehashPolicy, _Traits, true>
    {
    private:
      using __hashtable_base = __detail::_Hashtable_base<_Key, _Pair,
							 _Select1st,
							_Equal, _H1, _H2, _Hash,
							  _Traits>;

      using __hashtable = _Hashtable<_Key, _Pair, _Alloc,
				     _Select1st, _Equal,
				     _H1, _H2, _Hash, _RehashPolicy, _Traits>;

      using __hash_code = typename __hashtable_base::__hash_code;
      using __node_type = typename __hashtable_base::__node_type;

    public:
      using key_type = typename __hashtable_base::key_type;
      using iterator = typename __hashtable_base::iterator;
      using mapped_type = typename std::tuple_element<1, _Pair>::type;

      mapped_type&
      operator[](const key_type& __k);

      mapped_type&
      operator[](key_type&& __k);

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 761. unordered_map needs an at() member function.
      mapped_type&
      at(const key_type& __k);

      const mapped_type&
      at(const key_type& __k) const;
    };

  template<typename _Key, typename _Pair, typename _Alloc, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    typename _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
		       _H1, _H2, _Hash, _RehashPolicy, _Traits, true>
		       ::mapped_type&
    _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
	      _H1, _H2, _Hash, _RehashPolicy, _Traits, true>::
    operator[](const key_type& __k)
    {
      __hashtable* __h = static_cast<__hashtable*>(this);
      __hash_code __code = __h->_M_hash_code(__k);
      std::size_t __n = __h->_M_bucket_index(__k, __code);
      __node_type* __p = __h->_M_find_node(__n, __k, __code);

      if (!__p)
	{
	  __p = __h->_M_allocate_node(std::piecewise_construct,
				      std::tuple<const key_type&>(__k),
				      std::tuple<>());
	  return __h->_M_insert_unique_node(__n, __code, __p)->second;
	}

      return (__p->_M_v).second;
    }

  template<typename _Key, typename _Pair, typename _Alloc, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    typename _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
		       _H1, _H2, _Hash, _RehashPolicy, _Traits, true>
		       ::mapped_type&
    _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
	      _H1, _H2, _Hash, _RehashPolicy, _Traits, true>::
    operator[](key_type&& __k)
    {
      __hashtable* __h = static_cast<__hashtable*>(this);
      __hash_code __code = __h->_M_hash_code(__k);
      std::size_t __n = __h->_M_bucket_index(__k, __code);
      __node_type* __p = __h->_M_find_node(__n, __k, __code);

      if (!__p)
	{
	  __p = __h->_M_allocate_node(std::piecewise_construct,
				      std::forward_as_tuple(std::move(__k)),
				      std::tuple<>());
	  return __h->_M_insert_unique_node(__n, __code, __p)->second;
	}

      return (__p->_M_v).second;
    }

  template<typename _Key, typename _Pair, typename _Alloc, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    typename _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
		       _H1, _H2, _Hash, _RehashPolicy, _Traits, true>
		       ::mapped_type&
    _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
	      _H1, _H2, _Hash, _RehashPolicy, _Traits, true>::
    at(const key_type& __k)
    {
      __hashtable* __h = static_cast<__hashtable*>(this);
      __hash_code __code = __h->_M_hash_code(__k);
      std::size_t __n = __h->_M_bucket_index(__k, __code);
      __node_type* __p = __h->_M_find_node(__n, __k, __code);

      if (!__p)
	__throw_out_of_range(__N("_Map_base::at"));
      return (__p->_M_v).second;
    }

  template<typename _Key, typename _Pair, typename _Alloc, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    const typename _Map_base<_Key, _Pair, _Alloc, _Select1st,
			     _Equal, _H1, _H2, _Hash, _RehashPolicy,
			     _Traits, true>::mapped_type&
    _Map_base<_Key, _Pair, _Alloc, _Select1st, _Equal,
	      _H1, _H2, _Hash, _RehashPolicy, _Traits, true>::
    at(const key_type& __k) const
    {
      const __hashtable* __h = static_cast<const __hashtable*>(this);
      __hash_code __code = __h->_M_hash_code(__k);
      std::size_t __n = __h->_M_bucket_index(__k, __code);
      __node_type* __p = __h->_M_find_node(__n, __k, __code);

      if (!__p)
	__throw_out_of_range(__N("_Map_base::at"));
      return (__p->_M_v).second;
    }

  /**
   *  Primary class template _Insert_base.
   *
   *  insert member functions appropriate to all _Hashtables.
   */
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Insert_base
    {
      using __hashtable = _Hashtable<_Key, _Value, _Alloc, _ExtractKey,
				     _Equal, _H1, _H2, _Hash,
				     _RehashPolicy, _Traits>;

      using __hashtable_base = _Hashtable_base<_Key, _Value, _ExtractKey,
					       _Equal, _H1, _H2, _Hash,
					       _Traits>;

      using value_type = typename __hashtable_base::value_type;
      using iterator = typename __hashtable_base::iterator;
      using const_iterator =  typename __hashtable_base::const_iterator;
      using size_type = typename __hashtable_base::size_type;

      using __unique_keys = typename __hashtable_base::__unique_keys;
      using __ireturn_type = typename __hashtable_base::__ireturn_type;
      using __iconv_type = typename __hashtable_base::__iconv_type;

      __hashtable&
      _M_conjure_hashtable()
      { return *(static_cast<__hashtable*>(this)); }

      __ireturn_type
      insert(const value_type& __v)
      {
	__hashtable& __h = _M_conjure_hashtable();
	return __h._M_insert(__v, __unique_keys());
      }

      iterator
      insert(const_iterator, const value_type& __v)
      { return __iconv_type()(insert(__v)); }

      void
      insert(initializer_list<value_type> __l)
      { this->insert(__l.begin(), __l.end()); }

      template<typename _InputIterator>
	void
	insert(_InputIterator __first, _InputIterator __last);
    };

  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    template<typename _InputIterator>
      void
      _Insert_base<_Key, _Value, _Alloc, _ExtractKey, _Equal, _H1, _H2, _Hash,
		    _RehashPolicy, _Traits>::
      insert(_InputIterator __first, _InputIterator __last)
      {
	using __rehash_type = typename __hashtable::__rehash_type;
	using __rehash_state = typename __hashtable::__rehash_state;
	using pair_type = std::pair<bool, std::size_t>;

	size_type __n_elt = __detail::__distance_fw(__first, __last);

	__hashtable& __h = _M_conjure_hashtable();
	__rehash_type& __rehash = __h._M_rehash_policy;
	const __rehash_state& __saved_state = __rehash._M_state();
	pair_type __do_rehash = __rehash._M_need_rehash(__h._M_bucket_count,
							__h._M_element_count,
							__n_elt);

	if (__do_rehash.first)
	  __h._M_rehash(__do_rehash.second, __saved_state);

	for (; __first != __last; ++__first)
	  __h._M_insert(*__first, __unique_keys());
      }

  /**
   *  Primary class template _Insert.
   *
   *  Select insert member functions appropriate to _Hashtable policy choices.
   */
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits,
	   bool _Constant_iterators = _Traits::__constant_iterators::value,
	   bool _Unique_keys = _Traits::__unique_keys::value>
    struct _Insert;

  /// Specialization.
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Insert<_Key, _Value, _Alloc, _ExtractKey, _Equal, _H1, _H2, _Hash,
		   _RehashPolicy, _Traits, true, true>
    : public _Insert_base<_Key, _Value, _Alloc, _ExtractKey, _Equal,
			   _H1, _H2, _Hash, _RehashPolicy, _Traits>
    {
      using __base_type = _Insert_base<_Key, _Value, _Alloc, _ExtractKey,
					_Equal, _H1, _H2, _Hash,
					_RehashPolicy, _Traits>;
      using value_type = typename __base_type::value_type;
      using iterator = typename __base_type::iterator;
      using const_iterator =  typename __base_type::const_iterator;

      using __unique_keys = typename __base_type::__unique_keys;
      using __hashtable = typename __base_type::__hashtable;

      using __base_type::insert;

      std::pair<iterator, bool>
      insert(value_type&& __v)
      {
	__hashtable& __h = this->_M_conjure_hashtable();
	return __h._M_insert(std::move(__v), __unique_keys());
      }

      iterator
      insert(const_iterator, value_type&& __v)
      { return insert(std::move(__v)).first; }
    };

  /// Specialization.
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Insert<_Key, _Value, _Alloc, _ExtractKey, _Equal, _H1, _H2, _Hash,
		   _RehashPolicy, _Traits, true, false>
    : public _Insert_base<_Key, _Value, _Alloc, _ExtractKey, _Equal,
			   _H1, _H2, _Hash, _RehashPolicy, _Traits>
    {
      using __base_type = _Insert_base<_Key, _Value, _Alloc, _ExtractKey,
					_Equal, _H1, _H2, _Hash,
					_RehashPolicy, _Traits>;
      using value_type = typename __base_type::value_type;
      using iterator = typename __base_type::iterator;
      using const_iterator =  typename __base_type::const_iterator;

      using __unique_keys = typename __base_type::__unique_keys;
      using __hashtable = typename __base_type::__hashtable;

      using __base_type::insert;

      iterator
      insert(value_type&& __v)
      {
	__hashtable& __h = this->_M_conjure_hashtable();
	return __h._M_insert(std::move(__v), __unique_keys());
      }

      iterator
      insert(const_iterator, value_type&& __v)
      { return insert(std::move(__v)); }
     };

  /// Specialization.
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits, bool _Unique_keys>
    struct _Insert<_Key, _Value, _Alloc, _ExtractKey, _Equal, _H1, _H2, _Hash,
		   _RehashPolicy, _Traits, false, _Unique_keys>
    : public _Insert_base<_Key, _Value, _Alloc, _ExtractKey, _Equal,
			   _H1, _H2, _Hash, _RehashPolicy, _Traits>
    {
      using __base_type = _Insert_base<_Key, _Value, _Alloc, _ExtractKey,
				       _Equal, _H1, _H2, _Hash,
				       _RehashPolicy, _Traits>;
      using value_type = typename __base_type::value_type;
      using iterator = typename __base_type::iterator;
      using const_iterator =  typename __base_type::const_iterator;

      using __unique_keys = typename __base_type::__unique_keys;
      using __hashtable = typename __base_type::__hashtable;
      using __ireturn_type = typename __base_type::__ireturn_type;
      using __iconv_type = typename __base_type::__iconv_type;

      using __base_type::insert;

      template<typename _Pair>
	using __is_cons = std::is_constructible<value_type, _Pair&&>;

      template<typename _Pair>
	using _IFcons = std::enable_if<__is_cons<_Pair>::value>;

      template<typename _Pair>
	using _IFconsp = typename _IFcons<_Pair>::type;

      template<typename _Pair, typename = _IFconsp<_Pair>>
	__ireturn_type
	insert(_Pair&& __v)
	{
	  __hashtable& __h = this->_M_conjure_hashtable();
	  return __h._M_emplace(__unique_keys(), std::forward<_Pair>(__v));
	}

      template<typename _Pair, typename = _IFconsp<_Pair>>
	iterator
	insert(const_iterator, _Pair&& __v)
	{ return __iconv_type()(insert(std::forward<_Pair>(__v))); }
   };

  /**
   *  Primary class template  _Rehash_base.
   *
   *  Give hashtable the max_load_factor functions and reserve iff the
   *  rehash policy is _Prime_rehash_policy.
  */
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Rehash_base;

  /// Specialization.
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash, typename _Traits>
    struct _Rehash_base<_Key, _Value, _Alloc, _ExtractKey, _Equal,
			_H1, _H2, _Hash, _Prime_rehash_policy, _Traits>
    {
      using __hashtable = _Hashtable<_Key, _Value, _Alloc, _ExtractKey,
				     _Equal, _H1, _H2, _Hash,
				     _Prime_rehash_policy, _Traits>;

      float
      max_load_factor() const noexcept
      {
	const __hashtable* __this = static_cast<const __hashtable*>(this);
	return __this->__rehash_policy().max_load_factor();
      }

      void
      max_load_factor(float __z)
      {
	__hashtable* __this = static_cast<__hashtable*>(this);
	__this->__rehash_policy(_Prime_rehash_policy(__z));
      }

      void
      reserve(std::size_t __n)
      {
	__hashtable* __this = static_cast<__hashtable*>(this);
	__this->rehash(__builtin_ceil(__n / max_load_factor()));
      }
    };

  /**
   *  Primary class template _Hashtable_ebo_helper.
   *
   *  Helper class using EBO when it is not forbidden, type is not
   *  final, and when it worth it, type is empty.
   */
  template<int _Nm, typename _Tp,
	   bool __use_ebo = !__is_final(_Tp) && __is_empty(_Tp)>
    struct _Hashtable_ebo_helper;

  /// Specialization using EBO.
  template<int _Nm, typename _Tp>
    struct _Hashtable_ebo_helper<_Nm, _Tp, true>
    : private _Tp
    {
      _Hashtable_ebo_helper() = default;

      _Hashtable_ebo_helper(const _Tp& __tp) : _Tp(__tp)
      { }

      static const _Tp&
      _S_cget(const _Hashtable_ebo_helper& __eboh)
      { return static_cast<const _Tp&>(__eboh); }

      static _Tp&
      _S_get(_Hashtable_ebo_helper& __eboh)
      { return static_cast<_Tp&>(__eboh); }
    };

  /// Specialization not using EBO.
  template<int _Nm, typename _Tp>
    struct _Hashtable_ebo_helper<_Nm, _Tp, false>
    {
      _Hashtable_ebo_helper() = default;

      _Hashtable_ebo_helper(const _Tp& __tp) : _M_tp(__tp)
      { }

      static const _Tp&
      _S_cget(const _Hashtable_ebo_helper& __eboh)
      { return __eboh._M_tp; }

      static _Tp&
      _S_get(_Hashtable_ebo_helper& __eboh)
      { return __eboh._M_tp; }

    private:
      _Tp _M_tp;
    };

  /**
   *  Primary class template _Local_iterator_base.
   *
   *  Base class for local iterators, used to iterate within a bucket
   *  but not between buckets.
   */
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash,
	   bool __cache_hash_code>
    struct _Local_iterator_base;

  /**
   *  Primary class template _Hash_code_base.
   *
   *  Encapsulates two policy issues that aren't quite orthogonal.
   *   (1) the difference between using a ranged hash function and using
   *       the combination of a hash function and a range-hashing function.
   *       In the former case we don't have such things as hash codes, so
   *       we have a dummy type as placeholder.
   *   (2) Whether or not we cache hash codes.  Caching hash codes is
   *       meaningless if we have a ranged hash function.
   *
   *  We also put the key extraction objects here, for convenience.
   *  Each specialization derives from one or more of the template
   *  parameters to benefit from Ebo. This is important as this type
   *  is inherited in some cases by the _Local_iterator_base type used
   *  to implement local_iterator and const_local_iterator. As with
   *  any iterator type we prefer to make it as small as possible.
   *
   *  Primary template is unused except as a hook for specializations.
   */
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash,
	   bool __cache_hash_code>
    struct _Hash_code_base;

  /// Specialization: ranged hash function, no caching hash codes.  H1
  /// and H2 are provided but ignored.  We define a dummy hash code type.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash>
    struct _Hash_code_base<_Key, _Value, _ExtractKey, _H1, _H2, _Hash, false>
    : private _Hashtable_ebo_helper<0, _ExtractKey>,
      private _Hashtable_ebo_helper<1, _Hash>
    {
    private:
      using __ebo_extract_key = _Hashtable_ebo_helper<0, _ExtractKey>;
      using __ebo_hash = _Hashtable_ebo_helper<1, _Hash>;

    protected:
      typedef void* 					__hash_code;
      typedef _Hash_node<_Value, false>			__node_type;

      // We need the default constructor for the local iterators.
      _Hash_code_base() = default;

      _Hash_code_base(const _ExtractKey& __ex, const _H1&, const _H2&,
		      const _Hash& __h)
      : __ebo_extract_key(__ex), __ebo_hash(__h) { }

      __hash_code
      _M_hash_code(const _Key& __key) const
      { return 0; }

      std::size_t
      _M_bucket_index(const _Key& __k, __hash_code, std::size_t __n) const
      { return _M_ranged_hash()(__k, __n); }

      std::size_t
      _M_bucket_index(const __node_type* __p, std::size_t __n) const
      { return _M_ranged_hash()(_M_extract()(__p->_M_v), __n); }

      void
      _M_store_code(__node_type*, __hash_code) const
      { }

      void
      _M_copy_code(__node_type*, const __node_type*) const
      { }

      void
      _M_swap(_Hash_code_base& __x)
      {
	std::swap(_M_extract(), __x._M_extract());
	std::swap(_M_ranged_hash(), __x._M_ranged_hash());
      }

      const _ExtractKey&
      _M_extract() const { return __ebo_extract_key::_S_cget(*this); }

      _ExtractKey&
      _M_extract() { return __ebo_extract_key::_S_get(*this); }

      const _Hash&
      _M_ranged_hash() const { return __ebo_hash::_S_cget(*this); }

      _Hash&
      _M_ranged_hash() { return __ebo_hash::_S_get(*this); }
    };

  // No specialization for ranged hash function while caching hash codes.
  // That combination is meaningless, and trying to do it is an error.

  /// Specialization: ranged hash function, cache hash codes.  This
  /// combination is meaningless, so we provide only a declaration
  /// and no definition.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash>
    struct _Hash_code_base<_Key, _Value, _ExtractKey, _H1, _H2, _Hash, true>;

  /// Specialization: hash function and range-hashing function, no
  /// caching of hash codes.
  /// Provides typedef and accessor required by C++ 11.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2>
    struct _Hash_code_base<_Key, _Value, _ExtractKey, _H1, _H2,
			   _Default_ranged_hash, false>
    : private _Hashtable_ebo_helper<0, _ExtractKey>,
      private _Hashtable_ebo_helper<1, _H1>,
      private _Hashtable_ebo_helper<2, _H2>
    {
    private:
      using __ebo_extract_key = _Hashtable_ebo_helper<0, _ExtractKey>;
      using __ebo_h1 = _Hashtable_ebo_helper<1, _H1>;
      using __ebo_h2 = _Hashtable_ebo_helper<2, _H2>;

    public:
      typedef _H1 					hasher;

      hasher
      hash_function() const
      { return _M_h1(); }

    protected:
      typedef std::size_t 				__hash_code;
      typedef _Hash_node<_Value, false>			__node_type;

      // We need the default constructor for the local iterators.
      _Hash_code_base() = default;

      _Hash_code_base(const _ExtractKey& __ex,
		      const _H1& __h1, const _H2& __h2,
		      const _Default_ranged_hash&)
      : __ebo_extract_key(__ex), __ebo_h1(__h1), __ebo_h2(__h2) { }

      __hash_code
      _M_hash_code(const _Key& __k) const
      { return _M_h1()(__k); }

      std::size_t
      _M_bucket_index(const _Key&, __hash_code __c, std::size_t __n) const
      { return _M_h2()(__c, __n); }

      std::size_t
      _M_bucket_index(const __node_type* __p,
		      std::size_t __n) const
      { return _M_h2()(_M_h1()(_M_extract()(__p->_M_v)), __n); }

      void
      _M_store_code(__node_type*, __hash_code) const
      { }

      void
      _M_copy_code(__node_type*, const __node_type*) const
      { }

      void
      _M_swap(_Hash_code_base& __x)
      {
	std::swap(_M_extract(), __x._M_extract());
	std::swap(_M_h1(), __x._M_h1());
	std::swap(_M_h2(), __x._M_h2());
      }

      const _ExtractKey&
      _M_extract() const { return __ebo_extract_key::_S_cget(*this); }

      _ExtractKey&
      _M_extract() { return __ebo_extract_key::_S_get(*this); }

      const _H1&
      _M_h1() const { return __ebo_h1::_S_cget(*this); }

      _H1&
      _M_h1() { return __ebo_h1::_S_get(*this); }

      const _H2&
      _M_h2() const { return __ebo_h2::_S_cget(*this); }

      _H2&
      _M_h2() { return __ebo_h2::_S_get(*this); }
    };

  /// Specialization: hash function and range-hashing function,
  /// caching hash codes.  H is provided but ignored.  Provides
  /// typedef and accessor required by C++ 11.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2>
    struct _Hash_code_base<_Key, _Value, _ExtractKey, _H1, _H2,
			   _Default_ranged_hash, true>
    : private _Hashtable_ebo_helper<0, _ExtractKey>,
      private _Hashtable_ebo_helper<1, _H1>,
      private _Hashtable_ebo_helper<2, _H2>
    {
    private:
      // Gives access to _M_h2() to the local iterator implementation.
      friend struct _Local_iterator_base<_Key, _Value, _ExtractKey, _H1, _H2,
					 _Default_ranged_hash, true>;

      using __ebo_extract_key = _Hashtable_ebo_helper<0, _ExtractKey>;
      using __ebo_h1 = _Hashtable_ebo_helper<1, _H1>;
      using __ebo_h2 = _Hashtable_ebo_helper<2, _H2>;

    public:
      typedef _H1 					hasher;

      hasher
      hash_function() const
      { return _M_h1(); }

    protected:
      typedef std::size_t 				__hash_code;
      typedef _Hash_node<_Value, true>			__node_type;

      _Hash_code_base(const _ExtractKey& __ex,
		      const _H1& __h1, const _H2& __h2,
		      const _Default_ranged_hash&)
      : __ebo_extract_key(__ex), __ebo_h1(__h1), __ebo_h2(__h2) { }

      __hash_code
      _M_hash_code(const _Key& __k) const
      { return _M_h1()(__k); }

      std::size_t
      _M_bucket_index(const _Key&, __hash_code __c,
		      std::size_t __n) const
      { return _M_h2()(__c, __n); }

      std::size_t
      _M_bucket_index(const __node_type* __p, std::size_t __n) const
      { return _M_h2()(__p->_M_hash_code, __n); }

      void
      _M_store_code(__node_type* __n, __hash_code __c) const
      { __n->_M_hash_code = __c; }

      void
      _M_copy_code(__node_type* __to, const __node_type* __from) const
      { __to->_M_hash_code = __from->_M_hash_code; }

      void
      _M_swap(_Hash_code_base& __x)
      {
	std::swap(_M_extract(), __x._M_extract());
	std::swap(_M_h1(), __x._M_h1());
	std::swap(_M_h2(), __x._M_h2());
      }

      const _ExtractKey&
      _M_extract() const { return __ebo_extract_key::_S_cget(*this); }

      _ExtractKey&
      _M_extract() { return __ebo_extract_key::_S_get(*this); }

      const _H1&
      _M_h1() const { return __ebo_h1::_S_cget(*this); }

      _H1&
      _M_h1() { return __ebo_h1::_S_get(*this); }

      const _H2&
      _M_h2() const { return __ebo_h2::_S_cget(*this); }

      _H2&
      _M_h2() { return __ebo_h2::_S_get(*this); }
    };

  /**
   *  Primary class template _Equal_helper.
   *
   */
  template <typename _Key, typename _Value, typename _ExtractKey,
	    typename _Equal, typename _HashCodeType,
	    bool __cache_hash_code>
  struct _Equal_helper;

  /// Specialization.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _Equal, typename _HashCodeType>
  struct _Equal_helper<_Key, _Value, _ExtractKey, _Equal, _HashCodeType, true>
  {
    static bool
    _S_equals(const _Equal& __eq, const _ExtractKey& __extract,
	      const _Key& __k, _HashCodeType __c, _Hash_node<_Value, true>* __n)
    { return __c == __n->_M_hash_code && __eq(__k, __extract(__n->_M_v)); }
  };

  /// Specialization.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _Equal, typename _HashCodeType>
  struct _Equal_helper<_Key, _Value, _ExtractKey, _Equal, _HashCodeType, false>
  {
    static bool
    _S_equals(const _Equal& __eq, const _ExtractKey& __extract,
	      const _Key& __k, _HashCodeType, _Hash_node<_Value, false>* __n)
    { return __eq(__k, __extract(__n->_M_v)); }
  };


  /// Specialization.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash>
    struct _Local_iterator_base<_Key, _Value, _ExtractKey,
				_H1, _H2, _Hash, true>
    : private _Hashtable_ebo_helper<0, _H2>
    {
    protected:
      using __base_type = _Hashtable_ebo_helper<0, _H2>;
      using __hash_code_base = _Hash_code_base<_Key, _Value, _ExtractKey,
					       _H1, _H2, _Hash, true>;

    public:
      _Local_iterator_base() = default;
      _Local_iterator_base(const __hash_code_base& __base,
			   _Hash_node<_Value, true>* __p,
			   std::size_t __bkt, std::size_t __bkt_count)
      : __base_type(__base._M_h2()),
	_M_cur(__p), _M_bucket(__bkt), _M_bucket_count(__bkt_count) { }

      void
      _M_incr()
      {
	_M_cur = _M_cur->_M_next();
	if (_M_cur)
	  {
	    std::size_t __bkt
	      = __base_type::_S_get(*this)(_M_cur->_M_hash_code,
					   _M_bucket_count);
	    if (__bkt != _M_bucket)
	      _M_cur = nullptr;
	  }
      }

      _Hash_node<_Value, true>*  _M_cur;
      std::size_t _M_bucket;
      std::size_t _M_bucket_count;
    };

  /// Specialization.
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash>
    struct _Local_iterator_base<_Key, _Value, _ExtractKey,
				_H1, _H2, _Hash, false>
    : private _Hash_code_base<_Key, _Value, _ExtractKey,
			      _H1, _H2, _Hash, false>
    {
    protected:
      using __hash_code_base = _Hash_code_base<_Key, _Value, _ExtractKey,
					       _H1, _H2, _Hash, false>;

    public:
      _Local_iterator_base() = default;
      _Local_iterator_base(const __hash_code_base& __base,
			   _Hash_node<_Value, false>* __p,
			   std::size_t __bkt, std::size_t __bkt_count)
	: __hash_code_base(__base),
	  _M_cur(__p), _M_bucket(__bkt), _M_bucket_count(__bkt_count) { }

      void
      _M_incr()
      {
	_M_cur = _M_cur->_M_next();
	if (_M_cur)
	  {
	    std::size_t __bkt = this->_M_bucket_index(_M_cur, _M_bucket_count);
	    if (__bkt != _M_bucket)
	      _M_cur = nullptr;
	  }
      }

      _Hash_node<_Value, false>*  _M_cur;
      std::size_t _M_bucket;
      std::size_t _M_bucket_count;
    };

  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash, bool __cache>
    inline bool
    operator==(const _Local_iterator_base<_Key, _Value, _ExtractKey,
					  _H1, _H2, _Hash, __cache>& __x,
	       const _Local_iterator_base<_Key, _Value, _ExtractKey,
					  _H1, _H2, _Hash, __cache>& __y)
    { return __x._M_cur == __y._M_cur; }

  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash, bool __cache>
    inline bool
    operator!=(const _Local_iterator_base<_Key, _Value, _ExtractKey,
					  _H1, _H2, _Hash, __cache>& __x,
	       const _Local_iterator_base<_Key, _Value, _ExtractKey,
					  _H1, _H2, _Hash, __cache>& __y)
    { return __x._M_cur != __y._M_cur; }

  /// local iterators
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash,
	   bool __constant_iterators, bool __cache>
    struct _Local_iterator
    : public _Local_iterator_base<_Key, _Value, _ExtractKey,
				  _H1, _H2, _Hash, __cache>
    {
    private:
      using __base_type = _Local_iterator_base<_Key, _Value, _ExtractKey,
					       _H1, _H2, _Hash, __cache>;
      using __hash_code_base = typename __base_type::__hash_code_base;
    public:
      typedef _Value                                   value_type;
      typedef typename std::conditional<__constant_iterators,
					const _Value*, _Value*>::type
						       pointer;
      typedef typename std::conditional<__constant_iterators,
					const _Value&, _Value&>::type
						       reference;
      typedef std::ptrdiff_t                           difference_type;
      typedef std::forward_iterator_tag                iterator_category;

      _Local_iterator() = default;

      _Local_iterator(const __hash_code_base& __base,
		      _Hash_node<_Value, __cache>* __p,
		      std::size_t __bkt, std::size_t __bkt_count)
	: __base_type(__base, __p, __bkt, __bkt_count)
      { }

      reference
      operator*() const
      { return this->_M_cur->_M_v; }

      pointer
      operator->() const
      { return std::__addressof(this->_M_cur->_M_v); }

      _Local_iterator&
      operator++()
      {
	this->_M_incr();
	return *this;
      }

      _Local_iterator
      operator++(int)
      {
	_Local_iterator __tmp(*this);
	this->_M_incr();
	return __tmp;
      }
    };

  /// local const_iterators
  template<typename _Key, typename _Value, typename _ExtractKey,
	   typename _H1, typename _H2, typename _Hash,
	   bool __constant_iterators, bool __cache>
    struct _Local_const_iterator
    : public _Local_iterator_base<_Key, _Value, _ExtractKey,
				  _H1, _H2, _Hash, __cache>
    {
    private:
      using __base_type = _Local_iterator_base<_Key, _Value, _ExtractKey,
					       _H1, _H2, _Hash, __cache>;
      using __hash_code_base = typename __base_type::__hash_code_base;

    public:
      typedef _Value                                   value_type;
      typedef const _Value*                            pointer;
      typedef const _Value&                            reference;
      typedef std::ptrdiff_t                           difference_type;
      typedef std::forward_iterator_tag                iterator_category;

      _Local_const_iterator() = default;

      _Local_const_iterator(const __hash_code_base& __base,
			    _Hash_node<_Value, __cache>* __p,
			    std::size_t __bkt, std::size_t __bkt_count)
	: __base_type(__base, __p, __bkt, __bkt_count)
      { }

      _Local_const_iterator(const _Local_iterator<_Key, _Value, _ExtractKey,
						  _H1, _H2, _Hash,
						  __constant_iterators,
						  __cache>& __x)
	: __base_type(__x)
      { }

      reference
      operator*() const
      { return this->_M_cur->_M_v; }

      pointer
      operator->() const
      { return std::__addressof(this->_M_cur->_M_v); }

      _Local_const_iterator&
      operator++()
      {
	this->_M_incr();
	return *this;
      }

      _Local_const_iterator
      operator++(int)
      {
	_Local_const_iterator __tmp(*this);
	this->_M_incr();
	return __tmp;
      }
    };

  /**
   *  Primary class template _Hashtable_base.
   *
   *  Helper class adding management of _Equal functor to
   *  _Hash_code_base type.
   *
   *  Base class templates are:
   *    - __detail::_Hash_code_base
   *    - __detail::_Hashtable_ebo_helper
   */
  template<typename _Key, typename _Value,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash, typename _Traits>
  struct _Hashtable_base
  : public _Hash_code_base<_Key, _Value, _ExtractKey, _H1, _H2, _Hash,
			   _Traits::__hash_cached::value>,
    private _Hashtable_ebo_helper<0, _Equal>
  {
  public:
    typedef _Key                                    key_type;
    typedef _Value                                  value_type;
    typedef _Equal                                  key_equal;
    typedef std::size_t                             size_type;
    typedef std::ptrdiff_t                          difference_type;

    using __traits_type = _Traits;
    using __hash_cached = typename __traits_type::__hash_cached;
    using __constant_iterators = typename __traits_type::__constant_iterators;
    using __unique_keys = typename __traits_type::__unique_keys;

    using __hash_code_base = _Hash_code_base<_Key, _Value, _ExtractKey,
					     _H1, _H2, _Hash,
					     __hash_cached::value>;

    using __hash_code = typename __hash_code_base::__hash_code;
    using __node_type = typename __hash_code_base::__node_type;

    using iterator = __detail::_Node_iterator<value_type,
					      __constant_iterators::value,
					      __hash_cached::value>;

    using const_iterator = __detail::_Node_const_iterator<value_type,
						   __constant_iterators::value,
						   __hash_cached::value>;

    using local_iterator = __detail::_Local_iterator<key_type, value_type,
						  _ExtractKey, _H1, _H2, _Hash,
						  __constant_iterators::value,
						     __hash_cached::value>;

    using const_local_iterator = __detail::_Local_const_iterator<key_type,
								 value_type,
					_ExtractKey, _H1, _H2, _Hash,
					__constant_iterators::value,
					__hash_cached::value>;

    using __ireturn_type = typename std::conditional<__unique_keys::value,
						     std::pair<iterator, bool>,
						     iterator>::type;

    using __iconv_type = typename  std::conditional<__unique_keys::value,
						    _Select1st, _Identity
						    >::type;
  private:
    using _EqualEBO = _Hashtable_ebo_helper<0, _Equal>;
    using _EqualHelper =  _Equal_helper<_Key, _Value, _ExtractKey, _Equal,
					__hash_code, __hash_cached::value>;

  protected:
    using __node_base = __detail::_Hash_node_base;
    using __bucket_type = __node_base*;

    _Hashtable_base(const _ExtractKey& __ex, const _H1& __h1, const _H2& __h2,
		    const _Hash& __hash, const _Equal& __eq)
    : __hash_code_base(__ex, __h1, __h2, __hash), _EqualEBO(__eq)
    { }

    bool
    _M_equals(const _Key& __k, __hash_code __c, __node_type* __n) const
    {
      return _EqualHelper::_S_equals(_M_eq(), this->_M_extract(),
				     __k, __c, __n);
    }

    void
    _M_swap(_Hashtable_base& __x)
    {
      __hash_code_base::_M_swap(__x);
      std::swap(_M_eq(), __x._M_eq());
    }

    const _Equal&
    _M_eq() const { return _EqualEBO::_S_cget(*this); }

    _Equal&
    _M_eq() { return _EqualEBO::_S_get(*this); }
  };

  /**
   *  struct _Equality_base.
   *
   *  Common types and functions for class _Equality.
   */
  struct _Equality_base
  {
  protected:
    template<typename _Uiterator>
      static bool
      _S_is_permutation(_Uiterator, _Uiterator, _Uiterator);
  };

  // See std::is_permutation in N3068.
  template<typename _Uiterator>
    bool
    _Equality_base::
    _S_is_permutation(_Uiterator __first1, _Uiterator __last1,
		      _Uiterator __first2)
    {
      for (; __first1 != __last1; ++__first1, ++__first2)
	if (!(*__first1 == *__first2))
	  break;

      if (__first1 == __last1)
	return true;

      _Uiterator __last2 = __first2;
      std::advance(__last2, std::distance(__first1, __last1));

      for (_Uiterator __it1 = __first1; __it1 != __last1; ++__it1)
	{
	  _Uiterator __tmp =  __first1;
	  while (__tmp != __it1 && !bool(*__tmp == *__it1))
	    ++__tmp;

	  // We've seen this one before.
	  if (__tmp != __it1)
	    continue;

	  std::ptrdiff_t __n2 = 0;
	  for (__tmp = __first2; __tmp != __last2; ++__tmp)
	    if (*__tmp == *__it1)
	      ++__n2;

	  if (!__n2)
	    return false;

	  std::ptrdiff_t __n1 = 0;
	  for (__tmp = __it1; __tmp != __last1; ++__tmp)
	    if (*__tmp == *__it1)
	      ++__n1;

	  if (__n1 != __n2)
	    return false;
	}
      return true;
    }

  /**
   *  Primary class template  _Equality.
   *
   *  This is for implementing equality comparison for unordered
   *  containers, per N3068, by John Lakos and Pablo Halpern.
   *  Algorithmically, we follow closely the reference implementations
   *  therein.
   */
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits,
	   bool _Unique_keys = _Traits::__unique_keys::value>
    struct _Equality;

  /// Specialization.
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Equality<_Key, _Value, _Alloc, _ExtractKey, _Equal,
		     _H1, _H2, _Hash, _RehashPolicy, _Traits, true>
    {
      using __hashtable = _Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal,
				     _H1, _H2, _Hash, _RehashPolicy, _Traits>;

      bool
      _M_equal(const __hashtable&) const;
    };

  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    bool
    _Equality<_Key, _Value, _Alloc, _ExtractKey, _Equal,
	      _H1, _H2, _Hash, _RehashPolicy, _Traits, true>::
    _M_equal(const __hashtable& __other) const
    {
      const __hashtable* __this = static_cast<const __hashtable*>(this);

      if (__this->size() != __other.size())
	return false;

      for (auto __itx = __this->begin(); __itx != __this->end(); ++__itx)
	{
	  const auto __ity = __other.find(_ExtractKey()(*__itx));
	  if (__ity == __other.end() || !bool(*__ity == *__itx))
	    return false;
	}
      return true;
    }

  /// Specialization.
  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    struct _Equality<_Key, _Value, _Alloc, _ExtractKey, _Equal,
		     _H1, _H2, _Hash, _RehashPolicy, _Traits, false>
    : public _Equality_base
    {
      using __hashtable = _Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal,
				     _H1, _H2, _Hash, _RehashPolicy, _Traits>;

      bool
      _M_equal(const __hashtable&) const;
    };

  template<typename _Key, typename _Value, typename _Alloc,
	   typename _ExtractKey, typename _Equal,
	   typename _H1, typename _H2, typename _Hash,
	   typename _RehashPolicy, typename _Traits>
    bool
    _Equality<_Key, _Value, _Alloc, _ExtractKey, _Equal,
	      _H1, _H2, _Hash, _RehashPolicy, _Traits, false>::
    _M_equal(const __hashtable& __other) const
    {
      const __hashtable* __this = static_cast<const __hashtable*>(this);

      if (__this->size() != __other.size())
	return false;

      for (auto __itx = __this->begin(); __itx != __this->end();)
	{
	  const auto __xrange = __this->equal_range(_ExtractKey()(*__itx));
	  const auto __yrange = __other.equal_range(_ExtractKey()(*__itx));

	  if (std::distance(__xrange.first, __xrange.second)
	      != std::distance(__yrange.first, __yrange.second))
	    return false;

	  if (!_S_is_permutation(__xrange.first, __xrange.second,
				 __yrange.first))
	    return false;

	  __itx = __xrange.second;
	}
      return true;
    }

  /**
   * This type is to combine a _Hash_node_base instance with an allocator
   * instance through inheritance to benefit from EBO when possible.
   */
  template<typename _NodeAlloc>
    struct _Before_begin : public _NodeAlloc
    {
      _Hash_node_base _M_node;

      _Before_begin(const _Before_begin&) = default;
      _Before_begin(_Before_begin&&) = default;

      template<typename _Alloc>
	_Before_begin(_Alloc&& __a)
	  : _NodeAlloc(std::forward<_Alloc>(__a))
	{ }
    };

 //@} hashtable-detail
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __detail
} // namespace std

#endif // _HASHTABLE_POLICY_H
