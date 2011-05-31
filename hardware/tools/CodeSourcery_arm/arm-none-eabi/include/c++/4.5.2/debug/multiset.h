// Debugging multiset implementation -*- C++ -*-

// Copyright (C) 2003, 2004, 2005, 2006, 2007, 2009, 2010
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

/** @file debug/multiset.h
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_MULTISET_H
#define _GLIBCXX_DEBUG_MULTISET_H 1

#include <debug/safe_sequence.h>
#include <debug/safe_iterator.h>
#include <utility>

namespace std
{
namespace __debug
{
  /// Class std::multiset with safety/checking/debug instrumentation.
  template<typename _Key, typename _Compare = std::less<_Key>,
	   typename _Allocator = std::allocator<_Key> >
    class multiset
    : public _GLIBCXX_STD_D::multiset<_Key, _Compare, _Allocator>,
      public __gnu_debug::_Safe_sequence<multiset<_Key, _Compare, _Allocator> >
    {
      typedef _GLIBCXX_STD_D::multiset<_Key, _Compare, _Allocator> _Base;
      typedef __gnu_debug::_Safe_sequence<multiset> _Safe_base;

    public:
      // types:
      typedef _Key				     key_type;
      typedef _Key				     value_type;
      typedef _Compare				     key_compare;
      typedef _Compare				     value_compare;
      typedef _Allocator			     allocator_type;
      typedef typename _Base::reference	             reference;
      typedef typename _Base::const_reference        const_reference;

      typedef __gnu_debug::_Safe_iterator<typename _Base::iterator, multiset>
      iterator;
      typedef __gnu_debug::_Safe_iterator<typename _Base::const_iterator,
					  multiset> const_iterator;

      typedef typename _Base::size_type              size_type;
      typedef typename _Base::difference_type        difference_type;
      typedef typename _Base::pointer                pointer;
      typedef typename _Base::const_pointer          const_pointer;
      typedef std::reverse_iterator<iterator>        reverse_iterator;
      typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;

      // 23.3.3.1 construct/copy/destroy:
      explicit multiset(const _Compare& __comp = _Compare(),
			const _Allocator& __a = _Allocator())
      : _Base(__comp, __a) { }

      template<typename _InputIterator>
        multiset(_InputIterator __first, _InputIterator __last,
		 const _Compare& __comp = _Compare(),
		 const _Allocator& __a = _Allocator())
	: _Base(__gnu_debug::__check_valid_range(__first, __last), __last,
		__comp, __a) { }

      multiset(const multiset& __x)
      : _Base(__x), _Safe_base() { }

      multiset(const _Base& __x)
      : _Base(__x), _Safe_base() { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      multiset(multiset&& __x)
      : _Base(std::forward<multiset>(__x)), _Safe_base()
      { this->_M_swap(__x); }

      multiset(initializer_list<value_type> __l,
	       const _Compare& __comp = _Compare(),
	       const allocator_type& __a = allocator_type())
      : _Base(__l, __comp, __a), _Safe_base() { }
#endif

      ~multiset() { }

      multiset&
      operator=(const multiset& __x)
      {
	*static_cast<_Base*>(this) = __x;
	this->_M_invalidate_all();
	return *this;
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      multiset&
      operator=(multiset&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	clear();
	swap(__x);
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
      { return iterator(_Base::begin(), this); }

      const_iterator
      begin() const
      { return const_iterator(_Base::begin(), this); }

      iterator
      end()
      { return iterator(_Base::end(), this); }

      const_iterator
      end() const
      { return const_iterator(_Base::end(), this); }

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
      { return const_iterator(_Base::begin(), this); }

      const_iterator
      cend() const
      { return const_iterator(_Base::end(), this); }

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
      { return iterator(_Base::insert(__x), this); }

      iterator
      insert(iterator __position, const value_type& __x)
      {
	__glibcxx_check_insert(__position);
	return iterator(_Base::insert(__position.base(), __x), this);
      }

      template<typename _InputIterator>
      void
      insert(_InputIterator __first, _InputIterator __last)
      {
	__glibcxx_check_valid_range(__first, __last);
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
      {
	__glibcxx_check_erase(__position);
	__position._M_invalidate();
	return iterator(_Base::erase(__position.base()), this);
      }
#else
      void
      erase(iterator __position)
      {
	__glibcxx_check_erase(__position);
	__position._M_invalidate();
	_Base::erase(__position.base());
      }
#endif

      size_type
      erase(const key_type& __x)
      {
	std::pair<iterator, iterator> __victims = this->equal_range(__x);
	size_type __count = 0;
	while (__victims.first != __victims.second)
	{
	  iterator __victim = __victims.first++;
	  __victim._M_invalidate();
	  _Base::erase(__victim.base());
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
	__glibcxx_check_erase_range(__first, __last);
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
	__glibcxx_check_erase_range(__first, __last);
	while (__first != __last)
	  this->erase(__first++);
      }
#endif

      void
      swap(multiset& __x)
      {
	_Base::swap(__x);
	this->_M_swap(__x);
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
      { return iterator(_Base::find(__x), this); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      const_iterator
      find(const key_type& __x) const
      { return const_iterator(_Base::find(__x), this); }

      using _Base::count;

      iterator
      lower_bound(const key_type& __x)
      { return iterator(_Base::lower_bound(__x), this); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      const_iterator
      lower_bound(const key_type& __x) const
      { return const_iterator(_Base::lower_bound(__x), this); }

      iterator
      upper_bound(const key_type& __x)
      { return iterator(_Base::upper_bound(__x), this); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      const_iterator
      upper_bound(const key_type& __x) const
      { return const_iterator(_Base::upper_bound(__x), this); }

      std::pair<iterator,iterator>
      equal_range(const key_type& __x)
      {
	typedef typename _Base::iterator _Base_iterator;
	std::pair<_Base_iterator, _Base_iterator> __res =
        _Base::equal_range(__x);
	return std::make_pair(iterator(__res.first, this),
			      iterator(__res.second, this));
      }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 214. set::find() missing const overload
      std::pair<const_iterator,const_iterator>
      equal_range(const key_type& __x) const
      {
	typedef typename _Base::const_iterator _Base_iterator;
	std::pair<_Base_iterator, _Base_iterator> __res =
        _Base::equal_range(__x);
	return std::make_pair(const_iterator(__res.first, this),
			      const_iterator(__res.second, this));
      }

      _Base&
      _M_base() { return *this; }

      const _Base&
      _M_base() const { return *this; }

    private:
      void
      _M_invalidate_all()
      {
	typedef typename _Base::const_iterator _Base_const_iterator;
	typedef __gnu_debug::_Not_equal_to<_Base_const_iterator> _Not_equal;
	this->_M_invalidate_if(_Not_equal(_M_base().end()));
      }
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

} // namespace __debug
} // namespace std

#endif
