// Profiling set implementation -*- C++ -*-

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

/** @file profile/set.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_SET_H
#define _GLIBCXX_PROFILE_SET_H 1

#include <utility>

namespace std _GLIBCXX_VISIBILITY(default) 
{
namespace __profile
{
  /// Class std::set wrapper with performance instrumentation.
  template<typename _Key, typename _Compare = std::less<_Key>,
	   typename _Allocator = std::allocator<_Key> >
    class set
    : public _GLIBCXX_STD_C::set<_Key,_Compare,_Allocator>
    {
      typedef _GLIBCXX_STD_C::set<_Key, _Compare, _Allocator> _Base;

    public:
      // types:
      typedef _Key				    key_type;
      typedef _Key				    value_type;
      typedef _Compare				    key_compare;
      typedef _Compare				    value_compare;
      typedef _Allocator			    allocator_type;
      typedef typename _Base::reference             reference;
      typedef typename _Base::const_reference       const_reference;

      typedef typename _Base::iterator               iterator;
      typedef typename _Base::const_iterator         const_iterator;
      typedef typename _Base::reverse_iterator       reverse_iterator;
      typedef typename _Base::const_reverse_iterator const_reverse_iterator;

      typedef typename _Base::size_type             size_type;
      typedef typename _Base::difference_type       difference_type;
      typedef typename _Base::pointer               pointer;
      typedef typename _Base::const_pointer         const_pointer;

      // 23.3.3.1 construct/copy/destroy:
      explicit set(const _Compare& __comp = _Compare(),
		   const _Allocator& __a = _Allocator())
      : _Base(__comp, __a) { }

#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
#else
      template<typename _InputIterator>
#endif
        set(_InputIterator __first, _InputIterator __last,
	    const _Compare& __comp = _Compare(),
	    const _Allocator& __a = _Allocator())
	: _Base(__first, __last, __comp, __a) { }

      set(const set& __x)
      : _Base(__x) { }

      set(const _Base& __x)
      : _Base(__x) { }

#if __cplusplus >= 201103L
      set(set&& __x)
      noexcept(is_nothrow_copy_constructible<_Compare>::value)
      : _Base(std::move(__x))
      { }

      set(initializer_list<value_type> __l,
	  const _Compare& __comp = _Compare(),
	  const allocator_type& __a = allocator_type())
      : _Base(__l, __comp, __a) { }
#endif

      ~set() _GLIBCXX_NOEXCEPT { }

      set&
      operator=(const set& __x)
      {
	*static_cast<_Base*>(this) = __x;
	return *this;
      }

#if __cplusplus >= 201103L
      set&
      operator=(set&& __x)
      {
	// NB: DR 1204.
	// NB: DR 675.
	this->clear();
	this->swap(__x);
	return *this;
      }

      set&
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
	std::pair<iterator, bool>
	emplace(_Args&&... __args)
	{
	  auto __res = _Base::emplace(std::forward<_Args>(__args)...);
	  return std::pair<iterator, bool>(iterator(__res.first),
					   __res.second);
	}

      template<typename... _Args>
	iterator
	emplace_hint(const_iterator __pos, _Args&&... __args)
	{
	  return iterator(_Base::emplace_hint(__pos,
					      std::forward<_Args>(__args)...));
	}
#endif

      std::pair<iterator, bool>
      insert(const value_type& __x)
      {
	typedef typename _Base::iterator _Base_iterator;
	std::pair<_Base_iterator, bool> __res = _Base::insert(__x);
	return std::pair<iterator, bool>(iterator(__res.first),
					 __res.second);
      }

#if __cplusplus >= 201103L
      std::pair<iterator, bool>
      insert(value_type&& __x)
      {
	typedef typename _Base::iterator _Base_iterator;
	std::pair<_Base_iterator, bool> __res
	  = _Base::insert(std::move(__x));
	return std::pair<iterator, bool>(iterator(__res.first),
					 __res.second);
      }
#endif

      iterator
      insert(const_iterator __position, const value_type& __x)
      { return iterator(_Base::insert(__position, __x)); }

#if __cplusplus >= 201103L
      iterator
      insert(const_iterator __position, value_type&& __x)
      { return iterator(_Base::insert(__position, std::move(__x))); }
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
      void
      insert(initializer_list<value_type> __l)
      { _Base::insert(__l); }
#endif

#if __cplusplus >= 201103L
      iterator
      erase(const_iterator __position)
      { return iterator(_Base::erase(__position)); }
#else
      void
      erase(iterator __position)
      { _Base::erase(__position); }
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
      swap(set& __x)
      { _Base::swap(__x); }

      void
      clear() _GLIBCXX_NOEXCEPT
      { this->erase(begin(), end()); }

      // observers:
      using _Base::key_comp;
      using _Base::value_comp;

      // set operations:
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
      _M_base() _GLIBCXX_NOEXCEPT       { return *this; }

      const _Base&
      _M_base() const _GLIBCXX_NOEXCEPT { return *this; }

    };

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator==(const set<_Key, _Compare, _Allocator>& __lhs,
	       const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() == __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator!=(const set<_Key, _Compare, _Allocator>& __lhs,
	       const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() != __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<(const set<_Key, _Compare, _Allocator>& __lhs,
	      const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() < __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<=(const set<_Key, _Compare, _Allocator>& __lhs,
	       const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() <= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>=(const set<_Key, _Compare, _Allocator>& __lhs,
	       const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() >= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>(const set<_Key, _Compare, _Allocator>& __lhs,
	      const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() > __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(set<_Key, _Compare, _Allocator>& __x,
	 set<_Key, _Compare, _Allocator>& __y)
    { return __x.swap(__y); }

} // namespace __profile
} // namespace std

#endif
