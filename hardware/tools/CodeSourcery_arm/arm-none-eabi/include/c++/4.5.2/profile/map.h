// Profiling map implementation -*- C++ -*-

// Copyright (C) 2009, 2010 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/** @file profile/map.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_MAP_H
#define _GLIBCXX_PROFILE_MAP_H 1

#include <utility>
#include <profile/base.h>

namespace std
{
namespace __profile
{
  /// Class std::map wrapper with performance instrumentation.
  template<typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
	   typename _Allocator = std::allocator<std::pair<const _Key, _Tp> > >
    class map
    : public _GLIBCXX_STD_D::map<_Key, _Tp, _Compare, _Allocator>
    {
      typedef _GLIBCXX_STD_D::map<_Key, _Tp, _Compare, _Allocator> _Base;

    public:
      // types:
      typedef _Key                                  key_type;
      typedef _Tp                                   mapped_type;
      typedef std::pair<const _Key, _Tp>            value_type;
      typedef _Compare                              key_compare;
      typedef _Allocator                            allocator_type;
      typedef typename _Base::reference             reference;
      typedef typename _Base::const_reference       const_reference;

      typedef typename _Base::iterator       iterator;
      typedef typename _Base::const_iterator       const_iterator;
      typedef typename _Base::size_type             size_type;
      typedef typename _Base::difference_type       difference_type;
      typedef typename _Base::pointer               pointer;
      typedef typename _Base::const_pointer         const_pointer;
      typedef std::reverse_iterator<iterator>       reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      using _Base::value_compare;

      // 23.3.1.1 construct/copy/destroy:
      explicit map(const _Compare& __comp = _Compare(),
		   const _Allocator& __a = _Allocator())
      : _Base(__comp, __a) {
          __profcxx_map_to_unordered_map_construct(this);
      }

      template<typename _InputIterator>
        map(_InputIterator __first, _InputIterator __last,
	    const _Compare& __comp = _Compare(),
	    const _Allocator& __a = _Allocator())
	: _Base(__first, __last, __comp, __a) {
          __profcxx_map_to_unordered_map_construct(this);
        }

      map(const map& __x)
      : _Base(__x) {
          __profcxx_map_to_unordered_map_construct(this);
      }

      map(const _Base& __x)
      : _Base(__x) {
          __profcxx_map_to_unordered_map_construct(this);
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      map(map&& __x)
      : _Base(std::forward<map>(__x))
      { }

      map(initializer_list<value_type> __l,
	  const _Compare& __c = _Compare(),
	  const allocator_type& __a = allocator_type())
      : _Base(__l, __c, __a) { }
#endif

      ~map() {
          __profcxx_map_to_unordered_map_destruct(this);
      }

      map&
      operator=(const map& __x)
      {
	*static_cast<_Base*>(this) = __x;
	return *this;
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      map&
      operator=(map&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__x);
	return *this;
      }

      map&
      operator=(initializer_list<value_type> __l)
      {
	this->clear();
	this->insert(__l);
	return *this;
      }
#endif

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 133. map missing get_allocator()
      using _Base::get_allocator;

      // iterators:
      iterator 
      begin()
      { return _Base::begin(); }

      const_iterator
      begin() const
      { return _Base::begin(); }

      iterator
      end()
      { return _Base::end(); }

      const_iterator
      end() const
      { return _Base::end(); }

      reverse_iterator
      rbegin()
      { 
        __profcxx_map_to_unordered_map_invalidate(this);
        return reverse_iterator(end()); 
      }

      const_reverse_iterator
      rbegin() const
      {
        __profcxx_map_to_unordered_map_invalidate(this);
        return const_reverse_iterator(end());
      }

      reverse_iterator
      rend()
      {
        __profcxx_map_to_unordered_map_invalidate(this);
        return reverse_iterator(begin());
      }

      const_reverse_iterator
      rend() const
      {
        __profcxx_map_to_unordered_map_invalidate(this);
        return const_reverse_iterator(begin());
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      const_iterator
      cbegin() const
      { return const_iterator(_Base::begin()); }

      const_iterator
      cend() const
      { return const_iterator(_Base::end()); }

      const_reverse_iterator
      crbegin() const
      {
        __profcxx_map_to_unordered_map_invalidate(this);
        return const_reverse_iterator(end());
      }

      const_reverse_iterator
      crend() const
      {
        __profcxx_map_to_unordered_map_invalidate(this);
        return const_reverse_iterator(begin());
      }
#endif

      // capacity:
      using _Base::empty;
      using _Base::size;
      using _Base::max_size;

      // 23.3.1.2 element access:
      mapped_type&
      operator[](const key_type& __k)
      {
        __profcxx_map_to_unordered_map_find(this, size());
        return _Base::operator[](__k);
      }

      mapped_type&
      at(const key_type& __k)
      {
        __profcxx_map_to_unordered_map_find(this, size());
        return _Base::at(__k);
      }

      const mapped_type&
      at(const key_type& __k) const
      {
        __profcxx_map_to_unordered_map_find(this, size());
        return _Base::at(__k);
      }

      // modifiers:
      std::pair<iterator, bool>
      insert(const value_type& __x)
      {
        __profcxx_map_to_unordered_map_insert(this, size(), 1);
	typedef typename _Base::iterator _Base_iterator;
	std::pair<_Base_iterator, bool> __res = _Base::insert(__x);
	return std::pair<iterator, bool>(iterator(__res.first),
					 __res.second);
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      void
      insert(std::initializer_list<value_type> __list)
      { 
        size_type size_before = size();
        _Base::insert(__list); 
        __profcxx_map_to_unordered_map_insert(this, size_before, 
                                                size() - size_before);
      }
#endif

      iterator
      insert(iterator __position, const value_type& __x)
      {
        size_type size_before = size();
	return iterator(_Base::insert(__position, __x));
        __profcxx_map_to_unordered_map_insert(this, size_before, 
                                                size() - size_before);
      }

      template<typename _InputIterator>
        void
        insert(_InputIterator __first, _InputIterator __last)
        {
          size_type size_before = size();
	  _Base::insert(__first, __last);
          __profcxx_map_to_unordered_map_insert(this, size_before, 
                                                size() - size_before);
	}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      iterator
      erase(iterator __position)
      {
	iterator __i = _Base::erase(__position);
        __profcxx_map_to_unordered_map_erase(this, size(), 1);
        return __i;
      }
#else
      void
      erase(iterator __position)
      {
	_Base::erase(__position);
        __profcxx_map_to_unordered_map_erase(this, size(), 1);
      }
#endif

      size_type
      erase(const key_type& __x)
      {
	iterator __victim = find(__x);
	if (__victim == end())
	  return 0;
	else
	{
	  _Base::erase(__victim);
	  return 1;
	}
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      iterator
      erase(iterator __first, iterator __last)
      {
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// 151. can't currently clear() empty container
	while (__first != __last)
	  this->erase(__first++);
	return __last;
      }
#else
      void
      erase(iterator __first, iterator __last)
      {
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// 151. can't currently clear() empty container
	while (__first != __last)
	  this->erase(__first++);
      }
#endif

      void

      swap(map& __x)
      {
	_Base::swap(__x);
      }

      void
      clear()
      { this->erase(begin(), end()); }

      // observers:
      using _Base::key_comp;
      using _Base::value_comp;

      // 23.3.1.3 map operations:
      iterator
      find(const key_type& __x)
      {
        __profcxx_map_to_unordered_map_find(this, size());
        return iterator(_Base::find(__x));
      }

      const_iterator
      find(const key_type& __x) const
      {
        __profcxx_map_to_unordered_map_find(this, size());
        return const_iterator(_Base::find(__x));
      }

      size_type
      count(const key_type& __x) const
      {
        __profcxx_map_to_unordered_map_find(this, size());
        return _Base::count(__x);
      }

      iterator
      lower_bound(const key_type& __x)
      { 
        __profcxx_map_to_unordered_map_invalidate(this);
        return iterator(_Base::lower_bound(__x)); 
      }

      const_iterator
      lower_bound(const key_type& __x) const
      { 
        __profcxx_map_to_unordered_map_invalidate(this);
        return const_iterator(_Base::lower_bound(__x)); 
      }

      iterator
      upper_bound(const key_type& __x)
      { 
        __profcxx_map_to_unordered_map_invalidate(this);
        return iterator(_Base::upper_bound(__x)); 
      }

      const_iterator
      upper_bound(const key_type& __x) const
      { 
        __profcxx_map_to_unordered_map_invalidate(this);
        return const_iterator(_Base::upper_bound(__x)); 
      }

      std::pair<iterator,iterator>
      equal_range(const key_type& __x)
      {
	typedef typename _Base::iterator _Base_iterator;
	std::pair<_Base_iterator, _Base_iterator> __res =
	_Base::equal_range(__x);
	return std::make_pair(iterator(__res.first),
			      iterator(__res.second));
      }

      std::pair<const_iterator,const_iterator>
      equal_range(const key_type& __x) const
      {
        __profcxx_map_to_unordered_map_find(this, size());
	typedef typename _Base::const_iterator _Base_const_iterator;
	std::pair<_Base_const_iterator, _Base_const_iterator> __res =
	_Base::equal_range(__x);
	return std::make_pair(const_iterator(__res.first),
			      const_iterator(__res.second));
      }

      _Base& 
      _M_base() { return *this; }

      const _Base&
      _M_base() const { return *this; }

    };

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator==(const map<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const map<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { 
      __profcxx_map_to_unordered_map_invalidate(&__lhs);
      __profcxx_map_to_unordered_map_invalidate(&__rhs);
      return __lhs._M_base() == __rhs._M_base(); 
    }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator!=(const map<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const map<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { 
      __profcxx_map_to_unordered_map_invalidate(&__lhs);
      __profcxx_map_to_unordered_map_invalidate(&__rhs);
      return __lhs._M_base() != __rhs._M_base(); 
    }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator<(const map<_Key, _Tp, _Compare, _Allocator>& __lhs,
	      const map<_Key, _Tp, _Compare, _Allocator>& __rhs)
    {
      __profcxx_map_to_unordered_map_invalidate(&__lhs);
      __profcxx_map_to_unordered_map_invalidate(&__rhs);
      return __lhs._M_base() < __rhs._M_base(); 
    }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator<=(const map<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const map<_Key, _Tp, _Compare, _Allocator>& __rhs)
    {
      __profcxx_map_to_unordered_map_invalidate(&__lhs);
      __profcxx_map_to_unordered_map_invalidate(&__rhs);
      return __lhs._M_base() <= __rhs._M_base();
    }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator>=(const map<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const map<_Key, _Tp, _Compare, _Allocator>& __rhs)
    {
      __profcxx_map_to_unordered_map_invalidate(&__lhs);
      __profcxx_map_to_unordered_map_invalidate(&__rhs);
      return __lhs._M_base() >= __rhs._M_base();
    }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator>(const map<_Key, _Tp, _Compare, _Allocator>& __lhs,
	      const map<_Key, _Tp, _Compare, _Allocator>& __rhs)
    {
      __profcxx_map_to_unordered_map_invalidate(&__lhs);
      __profcxx_map_to_unordered_map_invalidate(&__rhs);
      return __lhs._M_base() > __rhs._M_base();
    }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline void
    swap(map<_Key, _Tp, _Compare, _Allocator>& __lhs,
	 map<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { __lhs.swap(__rhs); }

} // namespace __profile
} // namespace std

#endif
