// Profiling multiset implementation -*- C++ -*-

// Copyright (C) 2009, 2010 Free Software Foundation, Inc.
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

/** @file profile/multiset.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_MULTISET_H
#define _GLIBCXX_PROFILE_MULTISET_H 1

#include <utility>

namespace std
{
namespace __profile
{
  /// Class std::multiset wrapper with performance instrumentation.
  template<typename _Key, typename _Compare = std::less<_Key>,
	   typename _Allocator = std::allocator<_Key> >
    class multiset
    : public _GLIBCXX_STD_D::multiset<_Key, _Compare, _Allocator>
    {
      typedef _GLIBCXX_STD_D::multiset<_Key, _Compare, _Allocator> _Base;

    public:
      // types:
      typedef _Key				     key_type;
      typedef _Key				     value_type;
      typedef _Compare				     key_compare;
      typedef _Compare				     value_compare;
      typedef _Allocator			     allocator_type;
      typedef typename _Base::reference	             reference;
      typedef typename _Base::const_reference        const_reference;

      typedef typename _Base::iterator               iterator;
      typedef typename _Base::const_iterator         const_iterator;
      typedef typename _Base::reverse_iterator       reverse_iterator;
      typedef typename _Base::const_reverse_iterator const_reverse_iterator;

      typedef typename _Base::size_type              size_type;
      typedef typename _Base::difference_type        difference_type;
      typedef typename _Base::pointer                pointer;
      typedef typename _Base::const_pointer          const_pointer;

      // 23.3.3.1 construct/copy/destroy:
      explicit multiset(const _Compare& __comp = _Compare(),
			const _Allocator& __a = _Allocator())
      : _Base(__comp, __a) { }

      template<typename _InputIterator>
        multiset(_InputIterator __first, _InputIterator __last,
		 const _Compare& __comp = _Compare(),
		 const _Allocator& __a = _Allocator())
	: _Base(__first, __last, __comp, __a) { }

      multiset(const multiset& __x)
      : _Base(__x) { }

      multiset(const _Base& __x)
      : _Base(__x) { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      multiset(multiset&& __x)
      : _Base(std::forward<multiset>(__x))
      { }

      multiset(initializer_list<value_type> __l,
	       const _Compare& __comp = _Compare(),
	       const allocator_type& __a = allocator_type())
      : _Base(__l, __comp, __a) { }
#endif

      ~multiset() { }

      multiset&
      operator=(const multiset& __x)
      {
	*static_cast<_Base*>(this) = __x;
	return *this;
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      multiset&
      operator=(multiset&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__x);
	return *this;
      }

      multiset&
      operator=(initializer_list<value_type> __l)
      {
	this->clear();
	this->insert(__l);
	return *this;
      }
#endif

      using _Base::get_allocator;

      // iterators:
      iterator
      begin()
      { return iterator(_Base::begin()); }

      const_iterator
      begin() const
      { return const_iterator(_Base::begin()); }

      iterator
      end()
      { return iterator(_Base::end()); }

      const_iterator
      end() const
      { return const_iterator(_Base::end()); }

      reverse_iterator
      rbegin()
      { return reverse_iterator(end()); }

      const_reverse_iterator
      rbegin() const
      { return const_reverse_iterator(end()); }

      reverse_iterator
      rend()
      { return reverse_iterator(begin()); }

      const_reverse_iterator
      rend() const
      { return const_reverse_iterator(begin()); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      const_iterator
      cbegin() const
      { return const_iterator(_Base::begin()); }

      const_iterator
      cend() const
      { return const_iterator(_Base::end()); }

      const_reverse_iterator
      crbegin() const
      { return const_reverse_iterator(end()); }

      const_reverse_iterator
      crend() const
      { return const_reverse_iterator(begin()); }
#endif

      // capacity:
      using _Base::empty;
      using _Base::size;
      using _Base::max_size;

      // modifiers:
      iterator
      insert(const value_type& __x)
      { return iterator(_Base::insert(__x)); }

      iterator
      insert(iterator __position, const value_type& __x)
      {
	return iterator(_Base::insert(__position, __x));
      }

      template<typename _InputIterator>
      void
      insert(_InputIterator __first, _InputIterator __last)
      {
	_Base::insert(__first, __last);
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      void
      insert(initializer_list<value_type> __l)
      { _Base::insert(__l); }
#endif

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      iterator
      erase(iterator __position)
      { return _Base::erase(__position); }
#else
      void
      erase(iterator __position)
      { _Base::erase(__position); }
#endif

      size_type
      erase(const key_type& __x)
      {
	std::pair<iterator, iterator> __victims = this->equal_range(__x);
	size_type __count = 0;
	while (__victims.first != __victims.second)
	{
	  iterator __victim = __victims.first++;
	  _Base::erase(__victim);
	  ++__count;
	}
	return __count;
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
      swap(multiset& __x)
      {
	_Base::swap(__x);
      }

      void
      clear()
      { this->erase(begin(), end()); }

      // observers:
      using _Base::key_comp;
      using _Base::value_comp;

      // multiset operations:
      iterator
      find(const key_type& __x)
      { return iterator(_Base::find(__x)); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      const_iterator
      find(const key_type& __x) const
      { return const_iterator(_Base::find(__x)); }

      using _Base::count;

      iterator
      lower_bound(const key_type& __x)
      { return iterator(_Base::lower_bound(__x)); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      const_iterator
      lower_bound(const key_type& __x) const
      { return const_iterator(_Base::lower_bound(__x)); }

      iterator
      upper_bound(const key_type& __x)
      { return iterator(_Base::upper_bound(__x)); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      const_iterator
      upper_bound(const key_type& __x) const
      { return const_iterator(_Base::upper_bound(__x)); }

      std::pair<iterator,iterator>
      equal_range(const key_type& __x)
      {
	typedef typename _Base::iterator _Base_iterator;
	std::pair<_Base_iterator, _Base_iterator> __res =
        _Base::equal_range(__x);
	return std::make_pair(iterator(__res.first),
			      iterator(__res.second));
      }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      std::pair<const_iterator,const_iterator>
      equal_range(const key_type& __x) const
      {
	typedef typename _Base::const_iterator _Base_iterator;
	std::pair<_Base_iterator, _Base_iterator> __res =
        _Base::equal_range(__x);
	return std::make_pair(const_iterator(__res.first),
			      const_iterator(__res.second));
      }

      _Base&
      _M_base() { return *this; }

      const _Base&
      _M_base() const { return *this; }

    };

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator==(const multiset<_Key, _Compare, _Allocator>& __lhs,
	       const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() == __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator!=(const multiset<_Key, _Compare, _Allocator>& __lhs,
	       const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() != __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<(const multiset<_Key, _Compare, _Allocator>& __lhs,
	      const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() < __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<=(const multiset<_Key, _Compare, _Allocator>& __lhs,
	       const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() <= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>=(const multiset<_Key, _Compare, _Allocator>& __lhs,
	       const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() >= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>(const multiset<_Key, _Compare, _Allocator>& __lhs,
	      const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() > __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(multiset<_Key, _Compare, _Allocator>& __x,
	 multiset<_Key, _Compare, _Allocator>& __y)
    { return __x.swap(__y); }

} // namespace __profile
} // namespace std

#endif
