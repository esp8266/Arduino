// Profiling iterator implementation -*- C++ -*-

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

/** @file profile/iterator_tracker.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_ITERATOR_TRACKER
#define _GLIBCXX_PROFILE_ITERATOR_TRACKER 1

#include <ext/type_traits.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __profile
{

  template<typename _Iterator, typename _Sequence>
    class __iterator_tracker 
    {
      typedef __iterator_tracker _Self;

      // The underlying iterator
      _Iterator _M_current;

      // The underlying data structure
      const _Sequence* _M_ds;
      typedef std::iterator_traits<_Iterator> _Traits;

    public:
      typedef _Iterator		              _Base_iterator;
      typedef typename _Traits::iterator_category iterator_category; 
      typedef typename _Traits::value_type        value_type;
      typedef typename _Traits::difference_type   difference_type;
      typedef typename _Traits::reference         reference;
      typedef typename _Traits::pointer           pointer;

      __iterator_tracker()
      : _M_current(), _M_ds(0) { }

      __iterator_tracker(const _Iterator& __i, const _Sequence* __seq) 
      : _M_current(__i), _M_ds(__seq) { }

      __iterator_tracker(const __iterator_tracker& __x) 
      : _M_current(__x._M_current), _M_ds(__x._M_ds) { }

      template<typename _MutableIterator>
        __iterator_tracker(const __iterator_tracker<_MutableIterator,
			   typename __gnu_cxx::__enable_if
			   <(std::__are_same<_MutableIterator, typename
			     _Sequence::iterator::_Base_iterator>::__value),
			   _Sequence>::__type>& __x)
	:  _M_current(__x.base()), _M_ds(__x._M_get_sequence()) { }

      _Iterator
      base() const { return _M_current; }
  
      /**
       * @brief Conversion to underlying non-debug iterator to allow
       * better interaction with non-profile containers.
       */
      operator _Iterator() const { return _M_current; }

      pointer
      operator->() const { return &*_M_current; }

      __iterator_tracker&
      operator++()
      {
	_M_ds->_M_profile_iterate();
	++_M_current;
	return *this;
      }

      __iterator_tracker
      operator++(int)
      {
	_M_ds->_M_profile_iterate();
	__iterator_tracker __tmp(*this);
	++_M_current;
	return __tmp;
      }

      __iterator_tracker&
      operator--()
      {
	_M_ds->_M_profile_iterate(1);
	--_M_current;
	return *this;
      }

      __iterator_tracker
      operator--(int)
      {
	_M_ds->_M_profile_iterate(1);
	__iterator_tracker __tmp(*this);
	--_M_current;
	return __tmp;
      }

      __iterator_tracker&
      operator=(const __iterator_tracker& __x)
      {
	_M_current = __x._M_current;
	return *this;
      }

      reference
      operator*() const
      { return *_M_current; }

      // ------ Random access iterator requirements ------
      reference
      operator[](const difference_type& __n) const 
      { return _M_current[__n]; }

      __iterator_tracker&
      operator+=(const difference_type& __n)
      {
	_M_current += __n;
	return *this;
      }

      __iterator_tracker
      operator+(const difference_type& __n) const
      {
	__iterator_tracker __tmp(*this);
	__tmp += __n;
	return __tmp;
      }

      __iterator_tracker&
      operator-=(const difference_type& __n)
      {
	_M_current += -__n;
	return *this;
      }

      __iterator_tracker
      operator-(const difference_type& __n) const
      {
	__iterator_tracker __tmp(*this);
	__tmp -= __n;
	return __tmp;
      }

      void
      _M_find()
      { _M_ds->_M_profile_find(); }

      const _Sequence*
      _M_get_sequence() const
      { return static_cast<const _Sequence*>(_M_ds); }
  };

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator==(const __iterator_tracker<_IteratorL, _Sequence>& __lhs,
	       const __iterator_tracker<_IteratorR, _Sequence>& __rhs)
    { return __lhs.base() == __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator==(const __iterator_tracker<_Iterator, _Sequence>& __lhs,
	       const __iterator_tracker<_Iterator, _Sequence>& __rhs)
    { return __lhs.base() == __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator!=(const __iterator_tracker<_IteratorL, _Sequence>& __lhs,
	       const __iterator_tracker<_IteratorR, _Sequence>& __rhs)
    { return __lhs.base() != __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator!=(const __iterator_tracker<_Iterator, _Sequence>& __lhs,
               const __iterator_tracker<_Iterator, _Sequence>& __rhs)
    { return __lhs.base() != __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator<(const __iterator_tracker<_IteratorL, _Sequence>& __lhs,
	      const __iterator_tracker<_IteratorR, _Sequence>& __rhs)
    { return __lhs.base() < __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator<(const __iterator_tracker<_Iterator, _Sequence>& __lhs,
	      const __iterator_tracker<_Iterator, _Sequence>& __rhs)
    { return __lhs.base() < __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator<=(const __iterator_tracker<_IteratorL, _Sequence>& __lhs,
	       const __iterator_tracker<_IteratorR, _Sequence>& __rhs)
    { return __lhs.base() <= __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator<=(const __iterator_tracker<_Iterator, _Sequence>& __lhs,
	       const __iterator_tracker<_Iterator, _Sequence>& __rhs)
    { return __lhs.base() <= __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator>(const __iterator_tracker<_IteratorL, _Sequence>& __lhs,
	      const __iterator_tracker<_IteratorR, _Sequence>& __rhs)
    { return __lhs.base() > __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator>(const __iterator_tracker<_Iterator, _Sequence>& __lhs,
	      const __iterator_tracker<_Iterator, _Sequence>& __rhs)
    { return __lhs.base() > __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator>=(const __iterator_tracker<_IteratorL, _Sequence>& __lhs,
	       const __iterator_tracker<_IteratorR, _Sequence>& __rhs)
    { return __lhs.base() >= __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator>=(const __iterator_tracker<_Iterator, _Sequence>& __lhs,
	       const __iterator_tracker<_Iterator, _Sequence>& __rhs)
    { return __lhs.base() >= __rhs.base(); }

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // According to the resolution of DR179 not only the various comparison
  // operators but also operator- must accept mixed iterator/const_iterator
  // parameters.
  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline typename __iterator_tracker<_IteratorL, _Sequence>::difference_type
    operator-(const __iterator_tracker<_IteratorL, _Sequence>& __lhs,
	      const __iterator_tracker<_IteratorR, _Sequence>& __rhs)
    { return __lhs.base() - __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline typename __iterator_tracker<_Iterator, _Sequence>::difference_type
    operator-(const __iterator_tracker<_Iterator, _Sequence>& __lhs,
	      const __iterator_tracker<_Iterator, _Sequence>& __rhs)
    { return __lhs.base() - __rhs.base(); }

  template<typename _Iterator, typename _Sequence>
    inline __iterator_tracker<_Iterator, _Sequence>
    operator+(typename __iterator_tracker<_Iterator,_Sequence>::difference_type
	      __n,
	      const __iterator_tracker<_Iterator, _Sequence>& __i)
    { return __i + __n; }
	
}  // namespace __profile
}  // namespace std
#endif
