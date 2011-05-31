// Temporary buffer implementation -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010
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

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file stl_tempbuf.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _STL_TEMPBUF_H
#define _STL_TEMPBUF_H 1

#include <bits/stl_algobase.h>
#include <bits/stl_construct.h>
#include <bits/stl_uninitialized.h>

_GLIBCXX_BEGIN_NAMESPACE(std)

  /**
   *  @brief Allocates a temporary buffer.
   *  @param  len  The number of objects of type Tp.
   *  @return See full description.
   *
   *  Reinventing the wheel, but this time with prettier spokes!
   *
   *  This function tries to obtain storage for @c len adjacent Tp
   *  objects.  The objects themselves are not constructed, of course.
   *  A pair<> is returned containing <em>the buffer s address and
   *  capacity (in the units of sizeof(Tp)), or a pair of 0 values if
   *  no storage can be obtained.</em>  Note that the capacity obtained
   *  may be less than that requested if the memory is unavailable;
   *  you should compare len with the .second return value.
   *
   * Provides the nothrow exception guarantee.
   */
  template<typename _Tp>
    pair<_Tp*, ptrdiff_t>
    get_temporary_buffer(ptrdiff_t __len)
    {
      const ptrdiff_t __max =
	__gnu_cxx::__numeric_traits<ptrdiff_t>::__max / sizeof(_Tp);
      if (__len > __max)
	__len = __max;
      
      while (__len > 0) 
	{
	  _Tp* __tmp = static_cast<_Tp*>(::operator new(__len * sizeof(_Tp), 
							std::nothrow));
	  if (__tmp != 0)
	    return std::pair<_Tp*, ptrdiff_t>(__tmp, __len);
	  __len /= 2;
	}
      return std::pair<_Tp*, ptrdiff_t>(static_cast<_Tp*>(0), 0);
    }

  /**
   *  @brief The companion to get_temporary_buffer().
   *  @param  p  A buffer previously allocated by get_temporary_buffer.
   *  @return   None.
   *
   *  Frees the memory pointed to by p.
   */
  template<typename _Tp>
    inline void
    return_temporary_buffer(_Tp* __p)
    { ::operator delete(__p, std::nothrow); }


  /**
   *  This class is used in two places: stl_algo.h and ext/memory,
   *  where it is wrapped as the temporary_buffer class.  See
   *  temporary_buffer docs for more notes.
   */
  template<typename _ForwardIterator, typename _Tp>
    class _Temporary_buffer
    {
      // concept requirements
      __glibcxx_class_requires(_ForwardIterator, _ForwardIteratorConcept)

    public:
      typedef _Tp         value_type;
      typedef value_type* pointer;
      typedef pointer     iterator;
      typedef ptrdiff_t   size_type;

    protected:
      size_type  _M_original_len;
      size_type  _M_len;
      pointer    _M_buffer;

    public:
      /// As per Table mumble.
      size_type
      size() const
      { return _M_len; }

      /// Returns the size requested by the constructor; may be >size().
      size_type
      requested_size() const
      { return _M_original_len; }

      /// As per Table mumble.
      iterator
      begin()
      { return _M_buffer; }

      /// As per Table mumble.
      iterator
      end()
      { return _M_buffer + _M_len; }

      /**
       * Constructs a temporary buffer of a size somewhere between
       * zero and the size of the given range.
       */
      _Temporary_buffer(_ForwardIterator __first, _ForwardIterator __last);

      ~_Temporary_buffer()
      {
	std::_Destroy(_M_buffer, _M_buffer + _M_len);
	std::return_temporary_buffer(_M_buffer);
      }

    private:
      // Disable copy constructor and assignment operator.
      _Temporary_buffer(const _Temporary_buffer&);

      void
      operator=(const _Temporary_buffer&);
    };

  template<typename _ForwardIterator, typename _Tp>
    _Temporary_buffer<_ForwardIterator, _Tp>::
    _Temporary_buffer(_ForwardIterator __first, _ForwardIterator __last)
    : _M_original_len(std::distance(__first, __last)),
      _M_len(0), _M_buffer(0)
    {
      __try
	{
	  std::pair<pointer, size_type> __p(std::get_temporary_buffer<
					    value_type>(_M_original_len));
	  _M_buffer = __p.first;
	  _M_len = __p.second;
	  if(_M_buffer)
	    std::__uninitialized_construct_range(_M_buffer, _M_buffer + _M_len,
						 *__first);
	}
      __catch(...)
	{
	  std::return_temporary_buffer(_M_buffer);
	  _M_buffer = 0;
	  _M_len = 0;
	  __throw_exception_again;
	}
    }

_GLIBCXX_END_NAMESPACE

#endif /* _STL_TEMPBUF_H */

