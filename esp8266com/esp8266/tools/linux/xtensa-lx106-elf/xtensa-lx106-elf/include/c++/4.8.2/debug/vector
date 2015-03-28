// Debugging vector implementation -*- C++ -*-

// Copyright (C) 2003-2013 Free Software Foundation, Inc.
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

/** @file debug/vector
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_VECTOR
#define _GLIBCXX_DEBUG_VECTOR 1

#include <vector>
#include <utility>
#include <debug/safe_sequence.h>
#include <debug/safe_iterator.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __debug
{
  /// Class std::vector with safety/checking/debug instrumentation.
  template<typename _Tp,
	   typename _Allocator = std::allocator<_Tp> >
    class vector
    : public _GLIBCXX_STD_C::vector<_Tp, _Allocator>,
      public __gnu_debug::_Safe_sequence<vector<_Tp, _Allocator> >
    {
      typedef _GLIBCXX_STD_C::vector<_Tp, _Allocator> _Base;

      typedef typename _Base::iterator _Base_iterator;
      typedef typename _Base::const_iterator _Base_const_iterator;
      typedef __gnu_debug::_Equal_to<_Base_const_iterator> _Equal;

#if __cplusplus >= 201103L
      typedef __gnu_cxx::__alloc_traits<_Allocator>  _Alloc_traits;
#endif

    public:
      typedef typename _Base::reference             reference;
      typedef typename _Base::const_reference       const_reference;

      typedef __gnu_debug::_Safe_iterator<_Base_iterator,vector>
      iterator;
      typedef __gnu_debug::_Safe_iterator<_Base_const_iterator,vector>
      const_iterator;

      typedef typename _Base::size_type             size_type;
      typedef typename _Base::difference_type       difference_type;

      typedef _Tp				    value_type;
      typedef _Allocator			    allocator_type;
      typedef typename _Base::pointer               pointer;
      typedef typename _Base::const_pointer         const_pointer;
      typedef std::reverse_iterator<iterator>       reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      // 23.2.4.1 construct/copy/destroy:
      explicit
      vector(const _Allocator& __a = _Allocator())
      : _Base(__a), _M_guaranteed_capacity(0) { }

#if __cplusplus >= 201103L
      explicit
      vector(size_type __n, const _Allocator& __a = _Allocator())
      : _Base(__n, __a), _M_guaranteed_capacity(__n) { }

      vector(size_type __n, const _Tp& __value,
	     const _Allocator& __a = _Allocator())
      : _Base(__n, __value, __a), _M_guaranteed_capacity(__n) { }
#else
      explicit
      vector(size_type __n, const _Tp& __value = _Tp(),
	     const _Allocator& __a = _Allocator())
      : _Base(__n, __value, __a), _M_guaranteed_capacity(__n) { }
#endif

#if __cplusplus >= 201103L
      template<class _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
#else
      template<class _InputIterator>
#endif
        vector(_InputIterator __first, _InputIterator __last,
	       const _Allocator& __a = _Allocator())
        : _Base(__gnu_debug::__base(__gnu_debug::__check_valid_range(__first,
								     __last)),
		__gnu_debug::__base(__last), __a),
	  _M_guaranteed_capacity(0)
        { _M_update_guaranteed_capacity(); }

      vector(const vector& __x)
      : _Base(__x), _M_guaranteed_capacity(__x.size()) { }

      /// Construction from a release-mode vector
      vector(const _Base& __x)
      : _Base(__x), _M_guaranteed_capacity(__x.size()) { }

#if __cplusplus >= 201103L
      vector(vector&& __x) noexcept
      : _Base(std::move(__x)),
	_M_guaranteed_capacity(this->size())
      {
	this->_M_swap(__x);
	__x._M_guaranteed_capacity = 0;
      }

      vector(const vector& __x, const allocator_type& __a)
      : _Base(__x, __a), _M_guaranteed_capacity(__x.size()) { }

      vector(vector&& __x, const allocator_type& __a)
      : _Base(std::move(__x), __a),
        _M_guaranteed_capacity(this->size())
      {
	__x._M_invalidate_all();
	__x._M_guaranteed_capacity = 0;
      }

      vector(initializer_list<value_type> __l,
	     const allocator_type& __a = allocator_type())
      : _Base(__l, __a),
	_M_guaranteed_capacity(__l.size()) { }
#endif

      ~vector() _GLIBCXX_NOEXCEPT { }

      vector&
      operator=(const vector& __x)
      {
	static_cast<_Base&>(*this) = __x;
	this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
	return *this;
      }

#if __cplusplus >= 201103L
      vector&
      operator=(vector&& __x) noexcept(_Alloc_traits::_S_nothrow_move())
      {
	__glibcxx_check_self_move_assign(__x);
	_Base::operator=(std::move(__x));
	this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
	__x._M_invalidate_all();
	__x._M_guaranteed_capacity = 0;
	return *this;
      }

      vector&
      operator=(initializer_list<value_type> __l)
      {
	static_cast<_Base&>(*this) = __l;
	this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
	return *this;
      }
#endif

#if __cplusplus >= 201103L
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
#else
      template<typename _InputIterator>
#endif
        void
        assign(_InputIterator __first, _InputIterator __last)
        {
	  __glibcxx_check_valid_range(__first, __last);
	  _Base::assign(__gnu_debug::__base(__first),
			__gnu_debug::__base(__last));
	  this->_M_invalidate_all();
	  _M_update_guaranteed_capacity();
	}

      void
      assign(size_type __n, const _Tp& __u)
      {
	_Base::assign(__n, __u);
	this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
      }

#if __cplusplus >= 201103L
      void
      assign(initializer_list<value_type> __l)
      {
	_Base::assign(__l);
	this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
      }
#endif

      using _Base::get_allocator;

      // iterators:
      iterator
      begin() _GLIBCXX_NOEXCEPT
      { return iterator(_Base::begin(), this); }

      const_iterator
      begin() const _GLIBCXX_NOEXCEPT
      { return const_iterator(_Base::begin(), this); }

      iterator
      end() _GLIBCXX_NOEXCEPT
      { return iterator(_Base::end(), this); }

      const_iterator
      end() const _GLIBCXX_NOEXCEPT
      { return const_iterator(_Base::end(), this); }

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
      { return const_iterator(_Base::begin(), this); }

      const_iterator
      cend() const noexcept
      { return const_iterator(_Base::end(), this); }

      const_reverse_iterator
      crbegin() const noexcept
      { return const_reverse_iterator(end()); }

      const_reverse_iterator
      crend() const noexcept
      { return const_reverse_iterator(begin()); }
#endif

      // 23.2.4.2 capacity:
      using _Base::size;
      using _Base::max_size;

#if __cplusplus >= 201103L
      void
      resize(size_type __sz)
      {
	bool __realloc = _M_requires_reallocation(__sz);
	if (__sz < this->size())
	  this->_M_invalidate_after_nth(__sz);
	_Base::resize(__sz);
	if (__realloc)
	  this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
      }

      void
      resize(size_type __sz, const _Tp& __c)
      {
	bool __realloc = _M_requires_reallocation(__sz);
	if (__sz < this->size())
	  this->_M_invalidate_after_nth(__sz);
	_Base::resize(__sz, __c);
	if (__realloc)
	  this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
      }
#else
      void
      resize(size_type __sz, _Tp __c = _Tp())
      {
	bool __realloc = _M_requires_reallocation(__sz);
	if (__sz < this->size())
	  this->_M_invalidate_after_nth(__sz);
	_Base::resize(__sz, __c);
	if (__realloc)
	  this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
      }
#endif

#if __cplusplus >= 201103L
      void
      shrink_to_fit()
      {
	if (_Base::_M_shrink_to_fit())
	  {
	    _M_guaranteed_capacity = _Base::capacity();
	    this->_M_invalidate_all();
	  }
      }
#endif

      size_type
      capacity() const _GLIBCXX_NOEXCEPT
      {
#ifdef _GLIBCXX_DEBUG_PEDANTIC
	return _M_guaranteed_capacity;
#else
	return _Base::capacity();
#endif
      }

      using _Base::empty;

      void
      reserve(size_type __n)
      {
	bool __realloc = _M_requires_reallocation(__n);
	_Base::reserve(__n);
	if (__n > _M_guaranteed_capacity)
	  _M_guaranteed_capacity = __n;
	if (__realloc)
	  this->_M_invalidate_all();
      }

      // element access:
      reference
      operator[](size_type __n)
      {
	__glibcxx_check_subscript(__n);
	return _M_base()[__n];
      }

      const_reference
      operator[](size_type __n) const
      {
	__glibcxx_check_subscript(__n);
	return _M_base()[__n];
      }

      using _Base::at;

      reference
      front()
      {
	__glibcxx_check_nonempty();
	return _Base::front();
      }

      const_reference
      front() const
      {
	__glibcxx_check_nonempty();
	return _Base::front();
      }

      reference
      back()
      {
	__glibcxx_check_nonempty();
	return _Base::back();
      }

      const_reference
      back() const
      {
	__glibcxx_check_nonempty();
	return _Base::back();
      }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 464. Suggestion for new member functions in standard containers.
      using _Base::data;

      // 23.2.4.3 modifiers:
      void
      push_back(const _Tp& __x)
      {
	bool __realloc = _M_requires_reallocation(this->size() + 1);
	_Base::push_back(__x);
	if (__realloc)
	  this->_M_invalidate_all();
	_M_update_guaranteed_capacity();
      }

#if __cplusplus >= 201103L
      template<typename _Up = _Tp>
        typename __gnu_cxx::__enable_if<!std::__are_same<_Up, bool>::__value,
					void>::__type
        push_back(_Tp&& __x)
	{ emplace_back(std::move(__x)); }

      template<typename... _Args>
        void
        emplace_back(_Args&&... __args)
	{
	  bool __realloc = _M_requires_reallocation(this->size() + 1);
	  _Base::emplace_back(std::forward<_Args>(__args)...);
	  if (__realloc)
	    this->_M_invalidate_all();
	  _M_update_guaranteed_capacity();
	}
#endif

      void
      pop_back()
      {
	__glibcxx_check_nonempty();
	this->_M_invalidate_if(_Equal(--_Base::end()));
	_Base::pop_back();
      }

#if __cplusplus >= 201103L
      template<typename... _Args>
        iterator
        emplace(iterator __position, _Args&&... __args)
	{
	  __glibcxx_check_insert(__position);
	  bool __realloc = _M_requires_reallocation(this->size() + 1);
	  difference_type __offset = __position.base() - _Base::begin();
	  _Base_iterator __res = _Base::emplace(__position.base(),
						std::forward<_Args>(__args)...);
	  if (__realloc)
	    this->_M_invalidate_all();
	  else
	    this->_M_invalidate_after_nth(__offset);
	  _M_update_guaranteed_capacity();
	  return iterator(__res, this);
	}
#endif

      iterator
      insert(iterator __position, const _Tp& __x)
      {
	__glibcxx_check_insert(__position);
	bool __realloc = _M_requires_reallocation(this->size() + 1);
	difference_type __offset = __position.base() - _Base::begin();
	_Base_iterator __res = _Base::insert(__position.base(), __x);
	if (__realloc)
	  this->_M_invalidate_all();
	else
	  this->_M_invalidate_after_nth(__offset);
	_M_update_guaranteed_capacity();
	return iterator(__res, this);
      }

#if __cplusplus >= 201103L
      template<typename _Up = _Tp>
        typename __gnu_cxx::__enable_if<!std::__are_same<_Up, bool>::__value,
					iterator>::__type
        insert(iterator __position, _Tp&& __x)
        { return emplace(__position, std::move(__x)); }

      void
      insert(iterator __position, initializer_list<value_type> __l)
      { this->insert(__position, __l.begin(), __l.end()); }
#endif

      void
      insert(iterator __position, size_type __n, const _Tp& __x)
      {
	__glibcxx_check_insert(__position);
	bool __realloc = _M_requires_reallocation(this->size() + __n);
	difference_type __offset = __position.base() - _Base::begin();
	_Base::insert(__position.base(), __n, __x);
	if (__realloc)
	  this->_M_invalidate_all();
	else
	  this->_M_invalidate_after_nth(__offset);
	_M_update_guaranteed_capacity();
      }

#if __cplusplus >= 201103L
      template<class _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
#else
      template<class _InputIterator>
#endif
        void
        insert(iterator __position,
	       _InputIterator __first, _InputIterator __last)
        {
	  __glibcxx_check_insert_range(__position, __first, __last);

	  /* Hard to guess if invalidation will occur, because __last
	     - __first can't be calculated in all cases, so we just
	     punt here by checking if it did occur. */
	  _Base_iterator __old_begin = _M_base().begin();
	  difference_type __offset = __position.base() - _Base::begin();
	  _Base::insert(__position.base(), __gnu_debug::__base(__first),
					   __gnu_debug::__base(__last));

	  if (_M_base().begin() != __old_begin)
	    this->_M_invalidate_all();
	  else
	    this->_M_invalidate_after_nth(__offset);
	  _M_update_guaranteed_capacity();
	}

      iterator
      erase(iterator __position)
      {
	__glibcxx_check_erase(__position);
	difference_type __offset = __position.base() - _Base::begin();
	_Base_iterator __res = _Base::erase(__position.base());
	this->_M_invalidate_after_nth(__offset);
	return iterator(__res, this);
      }

      iterator
      erase(iterator __first, iterator __last)
      {
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// 151. can't currently clear() empty container
	__glibcxx_check_erase_range(__first, __last);

	if (__first.base() != __last.base())
	  {
	    difference_type __offset = __first.base() - _Base::begin();
	    _Base_iterator __res = _Base::erase(__first.base(),
						__last.base());
	    this->_M_invalidate_after_nth(__offset);
	    return iterator(__res, this);
	  }
	else
	  return __first;
      }

      void
      swap(vector& __x)
#if __cplusplus >= 201103L
			noexcept(_Alloc_traits::_S_nothrow_swap())
#endif
      {
#if __cplusplus >= 201103L
	if (!_Alloc_traits::_S_propagate_on_swap())
	  __glibcxx_check_equal_allocs(__x);
#endif
	_Base::swap(__x);
	this->_M_swap(__x);
        std::swap(_M_guaranteed_capacity, __x._M_guaranteed_capacity);
      }

      void
      clear() _GLIBCXX_NOEXCEPT
      {
	_Base::clear();
	this->_M_invalidate_all();
        _M_guaranteed_capacity = 0;
      }

      _Base&
      _M_base() _GLIBCXX_NOEXCEPT { return *this; }

      const _Base&
      _M_base() const _GLIBCXX_NOEXCEPT { return *this; }

    private:
      size_type _M_guaranteed_capacity;

      bool
      _M_requires_reallocation(size_type __elements)
      { return __elements > this->capacity(); }

      void
      _M_update_guaranteed_capacity()
      {
	if (this->size() > _M_guaranteed_capacity)
	  _M_guaranteed_capacity = this->size();
      }

      void
      _M_invalidate_after_nth(difference_type __n)
      {
	typedef __gnu_debug::_After_nth_from<_Base_const_iterator> _After_nth;
	this->_M_invalidate_if(_After_nth(__n, _Base::begin()));
      }
    };

  template<typename _Tp, typename _Alloc>
    inline bool
    operator==(const vector<_Tp, _Alloc>& __lhs,
	       const vector<_Tp, _Alloc>& __rhs)
    { return __lhs._M_base() == __rhs._M_base(); }

  template<typename _Tp, typename _Alloc>
    inline bool
    operator!=(const vector<_Tp, _Alloc>& __lhs,
	       const vector<_Tp, _Alloc>& __rhs)
    { return __lhs._M_base() != __rhs._M_base(); }

  template<typename _Tp, typename _Alloc>
    inline bool
    operator<(const vector<_Tp, _Alloc>& __lhs,
	      const vector<_Tp, _Alloc>& __rhs)
    { return __lhs._M_base() < __rhs._M_base(); }

  template<typename _Tp, typename _Alloc>
    inline bool
    operator<=(const vector<_Tp, _Alloc>& __lhs,
	       const vector<_Tp, _Alloc>& __rhs)
    { return __lhs._M_base() <= __rhs._M_base(); }

  template<typename _Tp, typename _Alloc>
    inline bool
    operator>=(const vector<_Tp, _Alloc>& __lhs,
	       const vector<_Tp, _Alloc>& __rhs)
    { return __lhs._M_base() >= __rhs._M_base(); }

  template<typename _Tp, typename _Alloc>
    inline bool
    operator>(const vector<_Tp, _Alloc>& __lhs,
	      const vector<_Tp, _Alloc>& __rhs)
    { return __lhs._M_base() > __rhs._M_base(); }

  template<typename _Tp, typename _Alloc>
    inline void
    swap(vector<_Tp, _Alloc>& __lhs, vector<_Tp, _Alloc>& __rhs)
    { __lhs.swap(__rhs); }

} // namespace __debug

#if __cplusplus >= 201103L
  // DR 1182.
  /// std::hash specialization for vector<bool>.
  template<typename _Alloc>
    struct hash<__debug::vector<bool, _Alloc>>
    : public __hash_base<size_t, __debug::vector<bool, _Alloc>>
    {
      size_t
      operator()(const __debug::vector<bool, _Alloc>& __b) const noexcept
      { return std::hash<_GLIBCXX_STD_C::vector<bool, _Alloc>>()
	  (__b._M_base()); }
    };
#endif

} // namespace std

#endif
