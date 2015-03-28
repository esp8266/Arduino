// Profiling multimap implementation -*- C++ -*-

// Copyright (C) 2009-2013 Free Software Foundation, Inc.
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

/** @file profile/multimap.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_MULTIMAP_H
#define _GLIBCXX_PROFILE_MULTIMAP_H 1

#include <utility>

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __profile
{
  /// Class std::multimap wrapper with performance instrumentation.
  template<typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
	   typename _Allocator = std::allocator<std::pair<const _Key, _Tp> > >
    class multimap
    : public _GLIBCXX_STD_C::multimap<_Key, _Tp, _Compare, _Allocator>
    {
      typedef _GLIBCXX_STD_C::multimap<_Key, _Tp, _Compare, _Allocator> _Base;

    public:
      // types:
      typedef _Key				     key_type;
      typedef _Tp				     mapped_type;
      typedef std::pair<const _Key, _Tp>             value_type;
      typedef _Compare                               key_compare;
      typedef _Allocator                             allocator_type;
      typedef typename _Base::reference              reference;
      typedef typename _Base::const_reference        const_reference;

      typedef typename _Base::iterator               iterator;
      typedef typename _Base::const_iterator         const_iterator;
      typedef typename _Base::reverse_iterator       reverse_iterator;
      typedef typename _Base::const_reverse_iterator const_reverse_iterator;

      typedef typename _Base::size_type              size_type;
      typedef typename _Base::difference_type        difference_type;
      typedef typename _Base::pointer                pointer;
      typedef typename _Base::const_pointer          const_pointer;

      // 23.3.1.1 construct/copy/destroy:
      explicit multimap(const _Compare& __comp = _Compare(),
			const _Allocator& __a = _Allocator())
      : _Base(__comp, __a) { }

#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
#else
      template<typename _InputIterator>
#endif
      multimap(_InputIterator __first, _InputIterator __last,
	       const _Compare& __comp = _Compare(),
	       const _Allocator& __a = _Allocator())
      : _Base(__first, __last, __comp, __a) { }

      multimap(const multimap& __x)
      : _Base(__x) { }

      multimap(const _Base& __x)
      : _Base(__x) { }

#if __cplusplus >= 201103L
      multimap(multimap&& __x)
      noexcept(is_nothrow_copy_constructible<_Compare>::value)
      : _Base(std::move(__x))
      { }

      multimap(initializer_list<value_type> __l,
	       const _Compare& __c = _Compare(),
	       const allocator_type& __a = allocator_type())
      : _Base(__l, __c, __a) { }
#endif

      ~multimap() _GLIBCXX_NOEXCEPT { }

      multimap&
      operator=(const multimap& __x)
      {
	*static_cast<_Base*>(this) = __x;
	return *this;
      }

#if __cplusplus >= 201103L
      multimap&
      operator=(multimap&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__x);
	return *this;
      }

      multimap&
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
      begin() _GLIBCXX_NOEXCEPT
      { return iterator(_Base::begin()); }

      const_iterator
      begin() const _GLIBCXX_NOEXCEPT
      { return const_iterator(_Base::begin()); }

      iterator
      end() _GLIBCXX_NOEXCEPT
      { return iterator(_Base::end()); }

      const_iterator
      end() const _GLIBCXX_NOEXCEPT
      { return const_iterator(_Base::end()); }

      reverse_iterator
      rbegin() _GLIBCXX_NOEXCEPT
      { return reverse_iterator(end()); }

      const_reverse_iterator
      rbegin() const _GLIBCXX_NOEXCEPT
      { return const_reverse_iterator(end()); }

      reverse_iterator
      rend() _GLIBCXX_NOEXCEPT
      { return reverse_iterator(begin()); }

      const_reverse_iterator
      rend() const _GLIBCXX_NOEXCEPT
      { return const_reverse_iterator(begin()); }

#if __cplusplus >= 201103L
      const_iterator
      cbegin() const noexcept
      { return const_iterator(_Base::begin()); }

      const_iterator
      cend() const noexcept
      { return const_iterator(_Base::end()); }

      const_reverse_iterator
      crbegin() const noexcept
      { return const_reverse_iterator(end()); }

      const_reverse_iterator
      crend() const noexcept
      { return const_reverse_iterator(begin()); }
#endif

      // capacity:
      using _Base::empty;
      using _Base::size;
      using _Base::max_size;

      // modifiers:
#if __cplusplus >= 201103L
      template<typename... _Args>
	iterator
	emplace(_Args&&... __args)
	{
	  return iterator(_Base::emplace(std::forward<_Args>(__args)...));
	}

      template<typename... _Args>
	iterator
	emplace_hint(const_iterator __pos, _Args&&... __args)
	{
	  return iterator(_Base::emplace_hint(__pos,
					      std::forward<_Args>(__args)...));
	}
#endif
      
      iterator
      insert(const value_type& __x)
      { return iterator(_Base::insert(__x)); }

#if __cplusplus >= 201103L
      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
        iterator
        insert(_Pair&& __x)
        { return iterator(_Base::insert(std::forward<_Pair>(__x))); }
#endif

#if __cplusplus >= 201103L
      void
      insert(std::initializer_list<value_type> __list)
      { _Base::insert(__list); }
#endif

      iterator
#if __cplusplus >= 201103L
      insert(const_iterator __position, const value_type& __x)
#else
      insert(iterator __position, const value_type& __x)
#endif
      { return iterator(_Base::insert(__position, __x)); }

#if __cplusplus >= 201103L
      template<typename _Pair, typename = typename
	       std::enable_if<std::is_constructible<value_type,
						    _Pair&&>::value>::type>
        iterator
        insert(const_iterator __position, _Pair&& __x)
        { return iterator(_Base::insert(__position,
					std::forward<_Pair>(__x))); }
#endif

#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
#else
      template<typename _InputIterator>
#endif
        void
        insert(_InputIterator __first, _InputIterator __last)
        { _Base::insert(__first, __last); }

#if __cplusplus >= 201103L
      iterator
      erase(const_iterator __position)
      { return iterator(_Base::erase(__position)); }

      iterator
      erase(iterator __position)
      { return iterator(_Base::erase(__position)); }
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

#if __cplusplus >= 201103L
      iterator
      erase(const_iterator __first, const_iterator __last)
      { return iterator(_Base::erase(__first, __last)); }
#else
      void
      erase(iterator __first, iterator __last)
      { _Base::erase(__first, __last); }
#endif

      void
      swap(multimap& __x)
      { _Base::swap(__x); }

      void
      clear() _GLIBCXX_NOEXCEPT
      { this->erase(begin(), end()); }

      // observers:
      using _Base::key_comp;
      using _Base::value_comp;

      // 23.3.1.3 multimap operations:
      iterator
      find(const key_type& __x)
      { return iterator(_Base::find(__x)); }

      const_iterator
      find(const key_type& __x) const
      { return const_iterator(_Base::find(__x)); }

      using _Base::count;

      iterator
      lower_bound(const key_type& __x)
      { return iterator(_Base::lower_bound(__x)); }

      const_iterator
      lower_bound(const key_type& __x) const
      { return const_iterator(_Base::lower_bound(__x)); }

      iterator
      upper_bound(const key_type& __x)
      { return iterator(_Base::upper_bound(__x)); }

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

      std::pair<const_iterator,const_iterator>
      equal_range(const key_type& __x) const
      {
	typedef typename _Base::const_iterator _Base_const_iterator;
	std::pair<_Base_const_iterator, _Base_const_iterator> __res =
	_Base::equal_range(__x);
	return std::make_pair(const_iterator(__res.first),
			      const_iterator(__res.second));
      }

      _Base&
      _M_base() _GLIBCXX_NOEXCEPT       { return *this; }

      const _Base&
      _M_base() const _GLIBCXX_NOEXCEPT { return *this; }
    };

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator==(const multimap<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const multimap<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() == __rhs._M_base(); }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator!=(const multimap<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const multimap<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() != __rhs._M_base(); }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator<(const multimap<_Key, _Tp, _Compare, _Allocator>& __lhs,
	      const multimap<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() < __rhs._M_base(); }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator<=(const multimap<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const multimap<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() <= __rhs._M_base(); }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator>=(const multimap<_Key, _Tp, _Compare, _Allocator>& __lhs,
	       const multimap<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() >= __rhs._M_base(); }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline bool
    operator>(const multimap<_Key, _Tp, _Compare, _Allocator>& __lhs,
	      const multimap<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() > __rhs._M_base(); }

  template<typename _Key, typename _Tp,
	   typename _Compare, typename _Allocator>
    inline void
    swap(multimap<_Key, _Tp, _Compare, _Allocator>& __lhs,
	 multimap<_Key, _Tp, _Compare, _Allocator>& __rhs)
    { __lhs.swap(__rhs); }

} // namespace __profile
} // namespace std

#endif
