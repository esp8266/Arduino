// Debug-mode error formatting implementation -*- C++ -*-

// Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009
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

/** @file debug/formatter.h
 *  This file is a GNU debug extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_DEBUG_FORMATTER_H
#define _GLIBCXX_DEBUG_FORMATTER_H 1

#include <bits/c++config.h>
#include <typeinfo>
#include <debug/debug.h>

namespace __gnu_debug
{
  using std::type_info;

  /** Determine if the two types are the same. */
  template<typename _Type1, typename _Type2>
    struct __is_same
    {
      static const bool value = false;
    };

  template<typename _Type>
    struct __is_same<_Type, _Type>
    {
      static const bool value = true;
    };

  template<bool> struct __truth { };

  class _Safe_sequence_base;

  template<typename _Iterator, typename _Sequence>
    class _Safe_iterator;

  template<typename _Sequence>
    class _Safe_sequence;

  enum _Debug_msg_id
  {
    // General checks
    __msg_valid_range,
    __msg_insert_singular,
    __msg_insert_different,
    __msg_erase_bad,
    __msg_erase_different,
    __msg_subscript_oob,
    __msg_empty,
    __msg_unpartitioned,
    __msg_unpartitioned_pred,
    __msg_unsorted,
    __msg_unsorted_pred,
    __msg_not_heap,
    __msg_not_heap_pred,
    // std::bitset checks
    __msg_bad_bitset_write,
    __msg_bad_bitset_read,
    __msg_bad_bitset_flip,
    // std::list checks
    __msg_self_splice,
    __msg_splice_alloc,
    __msg_splice_bad,
    __msg_splice_other,
    __msg_splice_overlap,
    // iterator checks
    __msg_init_singular,
    __msg_init_copy_singular,
    __msg_init_const_singular,
    __msg_copy_singular,
    __msg_bad_deref,
    __msg_bad_inc,
    __msg_bad_dec,
    __msg_iter_subscript_oob,
    __msg_advance_oob,
    __msg_retreat_oob,
    __msg_iter_compare_bad,
    __msg_compare_different,
    __msg_iter_order_bad,
    __msg_order_different,
    __msg_distance_bad,
    __msg_distance_different,
    // istream_iterator
    __msg_deref_istream,
    __msg_inc_istream,
    // ostream_iterator
    __msg_output_ostream,
    // istreambuf_iterator
    __msg_deref_istreambuf,
    __msg_inc_istreambuf
  };

  class _Error_formatter
  {
    /// Whether an iterator is constant, mutable, or unknown
    enum _Constness
    {
      __unknown_constness,
      __const_iterator,
      __mutable_iterator,
      __last_constness
    };

    // The state of the iterator (fine-grained), if we know it.
    enum _Iterator_state
    {
      __unknown_state,
      __singular,      // singular, may still be attached to a sequence
      __begin,         // dereferenceable, and at the beginning
      __middle,        // dereferenceable, not at the beginning
      __end,           // past-the-end, may be at beginning if sequence empty
      __last_state
    };

    // Tags denoting the type of parameter for construction
    struct _Is_iterator { };
    struct _Is_sequence { };

    // A parameter that may be referenced by an error message
    struct _Parameter
    {
      enum
      {
	__unused_param,
	__iterator,
	__sequence,
	__integer,
	__string
      } _M_kind;

      union
      {
	// When _M_kind == __iterator
	struct
	{
	  const char*      _M_name;
	  const void*      _M_address;
	  const type_info* _M_type;
	  _Constness       _M_constness;
	  _Iterator_state  _M_state;
	  const void*      _M_sequence;
	  const type_info* _M_seq_type;
	} _M_iterator;

	// When _M_kind == __sequence
	struct
	{
	  const char*      _M_name;
	  const void*      _M_address;
	  const type_info* _M_type;
	} _M_sequence;

	// When _M_kind == __integer
	struct
	{
	  const char* _M_name;
	  long        _M_value;
	} _M_integer;

	// When _M_kind == __string
	struct
	{
	  const char* _M_name;
	  const char* _M_value;
	} _M_string;
      } _M_variant;

      _Parameter() : _M_kind(__unused_param), _M_variant() { }

      _Parameter(long __value, const char* __name) 
      : _M_kind(__integer), _M_variant()
      {
	_M_variant._M_integer._M_name = __name;
	_M_variant._M_integer._M_value = __value;
      }

      _Parameter(const char* __value, const char* __name) 
      : _M_kind(__string), _M_variant()
      {
	_M_variant._M_string._M_name = __name;
	_M_variant._M_string._M_value = __value;
      }

      template<typename _Iterator, typename _Sequence>
        _Parameter(const _Safe_iterator<_Iterator, _Sequence>& __it,
		   const char* __name, _Is_iterator)
	: _M_kind(__iterator),  _M_variant()
        {
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
#ifdef __GXX_RTTI
	  _M_variant._M_iterator._M_type = &typeid(__it);
#else
	  _M_variant._M_iterator._M_type = 0;
#endif
	  _M_variant._M_iterator._M_constness =
	    __is_same<_Safe_iterator<_Iterator, _Sequence>,
	                         typename _Sequence::iterator>::
	      value? __mutable_iterator : __const_iterator;
	  _M_variant._M_iterator._M_sequence = __it._M_get_sequence();
#ifdef __GXX_RTTI
	  _M_variant._M_iterator._M_seq_type = &typeid(_Sequence);
#else
	  _M_variant._M_iterator._M_seq_type = 0;
#endif

	  if (__it._M_singular())
	    _M_variant._M_iterator._M_state = __singular;
	  else
	    {
	      bool __is_begin = __it._M_is_begin();
	      bool __is_end = __it._M_is_end();
	      if (__is_end)
		_M_variant._M_iterator._M_state = __end;
	      else if (__is_begin)
		_M_variant._M_iterator._M_state = __begin;
	      else
		_M_variant._M_iterator._M_state = __middle;
	    }
	}

      template<typename _Type>
        _Parameter(const _Type*& __it, const char* __name, _Is_iterator)
        : _M_kind(__iterator), _M_variant()
        {
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
#ifdef __GXX_RTTI
	  _M_variant._M_iterator._M_type = &typeid(__it);
#else
	  _M_variant._M_iterator._M_type = 0;
#endif
	  _M_variant._M_iterator._M_constness = __mutable_iterator;
	  _M_variant._M_iterator._M_state = __it? __unknown_state : __singular;
	  _M_variant._M_iterator._M_sequence = 0;
	  _M_variant._M_iterator._M_seq_type = 0;
	}

      template<typename _Type>
        _Parameter(_Type*& __it, const char* __name, _Is_iterator)
        : _M_kind(__iterator), _M_variant()
        {
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
#ifdef __GXX_RTTI
	  _M_variant._M_iterator._M_type = &typeid(__it);
#else
	  _M_variant._M_iterator._M_type = 0;
#endif
	  _M_variant._M_iterator._M_constness = __const_iterator;
	  _M_variant._M_iterator._M_state = __it? __unknown_state : __singular;
	  _M_variant._M_iterator._M_sequence = 0;
	  _M_variant._M_iterator._M_seq_type = 0;
	}

      template<typename _Iterator>
        _Parameter(const _Iterator& __it, const char* __name, _Is_iterator)
        : _M_kind(__iterator), _M_variant()
        {
	  _M_variant._M_iterator._M_name = __name;
	  _M_variant._M_iterator._M_address = &__it;
#ifdef __GXX_RTTI
	  _M_variant._M_iterator._M_type = &typeid(__it);
#else
	  _M_variant._M_iterator._M_type = 0;
#endif
	  _M_variant._M_iterator._M_constness = __unknown_constness;
	  _M_variant._M_iterator._M_state =
	    __gnu_debug::__check_singular(__it)? __singular : __unknown_state;
	  _M_variant._M_iterator._M_sequence = 0;
	  _M_variant._M_iterator._M_seq_type = 0;
	}

      template<typename _Sequence>
        _Parameter(const _Safe_sequence<_Sequence>& __seq,
		   const char* __name, _Is_sequence)
        : _M_kind(__sequence), _M_variant()
        {
	  _M_variant._M_sequence._M_name = __name;
	  _M_variant._M_sequence._M_address =
	    static_cast<const _Sequence*>(&__seq);
#ifdef __GXX_RTTI
	  _M_variant._M_sequence._M_type = &typeid(_Sequence);
#else
	  _M_variant._M_sequence._M_type = 0;
#endif
	}

      template<typename _Sequence>
        _Parameter(const _Sequence& __seq, const char* __name, _Is_sequence)
        : _M_kind(__sequence), _M_variant()
        {
	  _M_variant._M_sequence._M_name = __name;
	  _M_variant._M_sequence._M_address = &__seq;
#ifdef __GXX_RTTI
	  _M_variant._M_sequence._M_type = &typeid(_Sequence);
#else
	  _M_variant._M_sequence._M_type = 0;
#endif
	}

      void
      _M_print_field(const _Error_formatter* __formatter,
		     const char* __name) const;

      void
      _M_print_description(const _Error_formatter* __formatter) const;
    };

    friend struct _Parameter;

  public:
    template<typename _Iterator>
      const _Error_formatter&
      _M_iterator(const _Iterator& __it, const char* __name = 0)  const
      {
	if (_M_num_parameters < size_t(__max_parameters))
	  _M_parameters[_M_num_parameters++] = _Parameter(__it, __name,
							  _Is_iterator());
	return *this;
      }

    const _Error_formatter&
    _M_integer(long __value, const char* __name = 0) const
    {
      if (_M_num_parameters < size_t(__max_parameters))
	_M_parameters[_M_num_parameters++] = _Parameter(__value, __name);
      return *this;
    }

    const _Error_formatter&
    _M_string(const char* __value, const char* __name = 0) const
    {
      if (_M_num_parameters < size_t(__max_parameters))
	_M_parameters[_M_num_parameters++] = _Parameter(__value, __name);
      return *this;
    }

    template<typename _Sequence>
      const _Error_formatter&
      _M_sequence(const _Sequence& __seq, const char* __name = 0) const
      {
	if (_M_num_parameters < size_t(__max_parameters))
	  _M_parameters[_M_num_parameters++] = _Parameter(__seq, __name,
							  _Is_sequence());
	return *this;
      }

    const _Error_formatter&
    _M_message(const char* __text) const
    { _M_text = __text; return *this; }

    const _Error_formatter&
    _M_message(_Debug_msg_id __id) const throw ();

    _GLIBCXX_NORETURN void
    _M_error() const;

  private:
    _Error_formatter(const char* __file, size_t __line)
    : _M_file(__file), _M_line(__line), _M_num_parameters(0), _M_text(0),
      _M_max_length(78), _M_column(1), _M_first_line(true), _M_wordwrap(false)
    { _M_get_max_length(); }

    template<typename _Tp>
      void
      _M_format_word(char*, int, const char*, _Tp) const throw ();

    void
    _M_print_word(const char* __word) const;

    void
    _M_print_string(const char* __string) const;

    void
    _M_get_max_length() const throw ();

    enum { __max_parameters = 9 };

    const char*         _M_file;
    size_t              _M_line;
    mutable _Parameter  _M_parameters[__max_parameters];
    mutable size_t      _M_num_parameters;
    mutable const char* _M_text;
    mutable size_t      _M_max_length;
    enum { _M_indent = 4 } ;
    mutable size_t      _M_column;
    mutable bool        _M_first_line;
    mutable bool        _M_wordwrap;

  public:
    static _Error_formatter
    _M_at(const char* __file, size_t __line)
    { return _Error_formatter(__file, __line); }
  };
} // namespace __gnu_debug

#endif
