// Safe iterator implementation  -*- C++ -*-

// Copyright (C) 2011-2013 Free Software Foundation, Inc.
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

/** @file debug/safe_local_iterator.h
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_SAFE_LOCAL_ITERATOR_H
#define _GLIBCXX_DEBUG_SAFE_LOCAL_ITERATOR_H 1

#include <debug/debug.h>
#include <debug/macros.h>
#include <debug/functions.h>
#include <debug/safe_unordered_base.h>
#include <ext/type_traits.h>

namespace __gnu_debug
{
  /** \brief Safe iterator wrapper.
   *
   *  The class template %_Safe_local_iterator is a wrapper around an
   *  iterator that tracks the iterator's movement among sequences and
   *  checks that operations performed on the "safe" iterator are
   *  legal. In additional to the basic iterator operations (which are
   *  validated, and then passed to the underlying iterator),
   *  %_Safe_local_iterator has member functions for iterator invalidation,
   *  attaching/detaching the iterator from sequences, and querying
   *  the iterator's state.
   */
  template<typename _Iterator, typename _Sequence>
    class _Safe_local_iterator : public _Safe_local_iterator_base
    {
      typedef _Safe_local_iterator _Self;
      typedef typename _Sequence::size_type size_type;

      /// The underlying iterator
      _Iterator _M_current;

      /// The bucket this local iterator belongs to 
      size_type _M_bucket;

      /// Determine if this is a constant iterator.
      bool
      _M_constant() const
      {
	typedef typename _Sequence::const_local_iterator const_iterator;
	return std::__are_same<const_iterator, _Safe_local_iterator>::__value;
      }

      typedef std::iterator_traits<_Iterator> _Traits;

    public:
      typedef _Iterator                           iterator_type;
      typedef typename _Traits::iterator_category iterator_category;
      typedef typename _Traits::value_type        value_type;
      typedef typename _Traits::difference_type   difference_type;
      typedef typename _Traits::reference         reference;
      typedef typename _Traits::pointer           pointer;

      /// @post the iterator is singular and unattached
      _Safe_local_iterator() : _M_current() { }

      /**
       * @brief Safe iterator construction from an unsafe iterator and
       * its sequence.
       *
       * @pre @p seq is not NULL
       * @post this is not singular
       */
      _Safe_local_iterator(const _Iterator& __i, size_type __bucket,
			   const _Sequence* __seq)
      : _Safe_local_iterator_base(__seq, _M_constant()), _M_current(__i),
	_M_bucket(__bucket)
      {
	_GLIBCXX_DEBUG_VERIFY(!this->_M_singular(),
			      _M_message(__msg_init_singular)
			      ._M_iterator(*this, "this"));
      }

      /**
       * @brief Copy construction.
       */
      _Safe_local_iterator(const _Safe_local_iterator& __x)
      : _Safe_local_iterator_base(__x, _M_constant()),
	_M_current(__x._M_current), _M_bucket(__x._M_bucket)
      {
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// DR 408. Is vector<reverse_iterator<char*> > forbidden?
	_GLIBCXX_DEBUG_VERIFY(!__x._M_singular()
			      || __x._M_current == _Iterator(),
			      _M_message(__msg_init_copy_singular)
			      ._M_iterator(*this, "this")
			      ._M_iterator(__x, "other"));
      }

      /**
       *  @brief Converting constructor from a mutable iterator to a
       *  constant iterator.
      */
      template<typename _MutableIterator>
	_Safe_local_iterator(
	  const _Safe_local_iterator<_MutableIterator,
	  typename __gnu_cxx::__enable_if<std::__are_same<
	      _MutableIterator,
	      typename _Sequence::local_iterator::iterator_type>::__value,
					  _Sequence>::__type>& __x)
	: _Safe_local_iterator_base(__x, _M_constant()),
	  _M_current(__x.base()), _M_bucket(__x._M_bucket)
	{
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // DR 408. Is vector<reverse_iterator<char*> > forbidden?
	  _GLIBCXX_DEBUG_VERIFY(!__x._M_singular()
				|| __x.base() == _Iterator(),
				_M_message(__msg_init_const_singular)
				._M_iterator(*this, "this")
				._M_iterator(__x, "other"));
	}

      /**
       * @brief Copy assignment.
       */
      _Safe_local_iterator&
      operator=(const _Safe_local_iterator& __x)
      {
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// DR 408. Is vector<reverse_iterator<char*> > forbidden?
	_GLIBCXX_DEBUG_VERIFY(!__x._M_singular()
			      || __x._M_current == _Iterator(),
			      _M_message(__msg_copy_singular)
			      ._M_iterator(*this, "this")
			      ._M_iterator(__x, "other"));
	_M_current = __x._M_current;
	_M_bucket = __x._M_bucket;
	this->_M_attach(__x._M_sequence);
	return *this;
      }

      /**
       *  @brief Iterator dereference.
       *  @pre iterator is dereferenceable
       */
      reference
      operator*() const
      {
	_GLIBCXX_DEBUG_VERIFY(this->_M_dereferenceable(),
			      _M_message(__msg_bad_deref)
			      ._M_iterator(*this, "this"));
	return *_M_current;
      }

      /**
       *  @brief Iterator dereference.
       *  @pre iterator is dereferenceable
       *  @todo Make this correct w.r.t. iterators that return proxies
       *  @todo Use addressof() instead of & operator
       */
      pointer
      operator->() const
      {
	_GLIBCXX_DEBUG_VERIFY(this->_M_dereferenceable(),
			      _M_message(__msg_bad_deref)
			      ._M_iterator(*this, "this"));
	return &*_M_current;
      }

      // ------ Input iterator requirements ------
      /**
       *  @brief Iterator preincrement
       *  @pre iterator is incrementable
       */
      _Safe_local_iterator&
      operator++()
      {
	_GLIBCXX_DEBUG_VERIFY(this->_M_incrementable(),
			      _M_message(__msg_bad_inc)
			      ._M_iterator(*this, "this"));
	++_M_current;
	return *this;
      }

      /**
       *  @brief Iterator postincrement
       *  @pre iterator is incrementable
       */
      _Safe_local_iterator
      operator++(int)
      {
	_GLIBCXX_DEBUG_VERIFY(this->_M_incrementable(),
			      _M_message(__msg_bad_inc)
			      ._M_iterator(*this, "this"));
	_Safe_local_iterator __tmp(*this);
	++_M_current;
	return __tmp;
      }

      // ------ Utilities ------
      /**
       * @brief Return the underlying iterator
       */
      _Iterator
      base() const { return _M_current; }

      /**
       * @brief Return the bucket
       */
      size_type
      bucket() const { return _M_bucket; }

      /**
       * @brief Conversion to underlying non-debug iterator to allow
       * better interaction with non-debug containers.
       */
      operator _Iterator() const { return _M_current; }

      /** Attach iterator to the given sequence. */
      void
      _M_attach(_Safe_sequence_base* __seq)
      { _Safe_iterator_base::_M_attach(__seq, _M_constant()); }

      /** Likewise, but not thread-safe. */
      void
      _M_attach_single(_Safe_sequence_base* __seq)
      { _Safe_iterator_base::_M_attach_single(__seq, _M_constant()); }

      /// Is the iterator dereferenceable?
      bool
      _M_dereferenceable() const
      { return !this->_M_singular() && !_M_is_end(); }

      /// Is the iterator incrementable?
      bool
      _M_incrementable() const
      { return !this->_M_singular() && !_M_is_end(); }

      // Is the iterator range [*this, __rhs) valid?
      template<typename _Other>
	bool
	_M_valid_range(const _Safe_local_iterator<_Other,
						  _Sequence>& __rhs) const;

      // The sequence this iterator references.
      const _Sequence*
      _M_get_sequence() const
      { return static_cast<const _Sequence*>(_M_sequence); }

      /// Is this iterator equal to the sequence's begin() iterator?
      bool _M_is_begin() const
      { return base() == _M_get_sequence()->_M_base().begin(_M_bucket); }

      /// Is this iterator equal to the sequence's end() iterator?
      bool _M_is_end() const
      { return base() == _M_get_sequence()->_M_base().end(_M_bucket); }

      /// Is this iterator part of the same bucket as the other one?
      template <typename _Other>
	bool _M_in_same_bucket(const _Safe_local_iterator<_Other,
						_Sequence>& __other) const
	{ return _M_bucket == __other.bucket(); }
    };

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator==(const _Safe_local_iterator<_IteratorL, _Sequence>& __lhs,
	       const _Safe_local_iterator<_IteratorR, _Sequence>& __rhs)
    {
      _GLIBCXX_DEBUG_VERIFY(! __lhs._M_singular() && ! __rhs._M_singular(),
			    _M_message(__msg_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_can_compare(__rhs),
			    _M_message(__msg_compare_different)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_can_compare(__rhs),
			    _M_message(__msg_compare_different)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_in_same_bucket(__rhs),
			    _M_message(__msg_local_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      return __lhs.base() == __rhs.base();
    }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator==(const _Safe_local_iterator<_Iterator, _Sequence>& __lhs,
	       const _Safe_local_iterator<_Iterator, _Sequence>& __rhs)
    {
      _GLIBCXX_DEBUG_VERIFY(! __lhs._M_singular() && ! __rhs._M_singular(),
			    _M_message(__msg_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_can_compare(__rhs),
			    _M_message(__msg_compare_different)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_in_same_bucket(__rhs),
			    _M_message(__msg_local_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      return __lhs.base() == __rhs.base();
    }

  template<typename _IteratorL, typename _IteratorR, typename _Sequence>
    inline bool
    operator!=(const _Safe_local_iterator<_IteratorL, _Sequence>& __lhs,
	       const _Safe_local_iterator<_IteratorR, _Sequence>& __rhs)
    {
      _GLIBCXX_DEBUG_VERIFY(! __lhs._M_singular() && ! __rhs._M_singular(),
			    _M_message(__msg_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_can_compare(__rhs),
			    _M_message(__msg_compare_different)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_in_same_bucket(__rhs),
			    _M_message(__msg_local_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      return __lhs.base() != __rhs.base();
    }

  template<typename _Iterator, typename _Sequence>
    inline bool
    operator!=(const _Safe_local_iterator<_Iterator, _Sequence>& __lhs,
	       const _Safe_local_iterator<_Iterator, _Sequence>& __rhs)
    {
      _GLIBCXX_DEBUG_VERIFY(! __lhs._M_singular() && ! __rhs._M_singular(),
			    _M_message(__msg_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_can_compare(__rhs),
			    _M_message(__msg_compare_different)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      _GLIBCXX_DEBUG_VERIFY(__lhs._M_in_same_bucket(__rhs),
			    _M_message(__msg_local_iter_compare_bad)
			    ._M_iterator(__lhs, "lhs")
			    ._M_iterator(__rhs, "rhs"));
      return __lhs.base() != __rhs.base();
    }
} // namespace __gnu_debug

#include <debug/safe_local_iterator.tcc>

#endif
