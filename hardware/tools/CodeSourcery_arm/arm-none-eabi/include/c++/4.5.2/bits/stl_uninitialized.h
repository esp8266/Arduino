// Raw memory manipulators -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
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

/** @file stl_uninitialized.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _STL_UNINITIALIZED_H
#define _STL_UNINITIALIZED_H 1

_GLIBCXX_BEGIN_NAMESPACE(std)

  template<bool>
    struct __uninitialized_copy
    {
      template<typename _InputIterator, typename _ForwardIterator>
        static _ForwardIterator
        uninitialized_copy(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result)
        {
	  _ForwardIterator __cur = __result;
	  __try
	    {
	      for (; __first != __last; ++__first, ++__cur)
		std::_Construct(&*__cur, *__first);
	      return __cur;
	    }
	  __catch(...)
	    {
	      std::_Destroy(__result, __cur);
	      __throw_exception_again;
	    }
	}
    };

  template<>
    struct __uninitialized_copy<true>
    {
      template<typename _InputIterator, typename _ForwardIterator>
        static _ForwardIterator
        uninitialized_copy(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result)
        { return std::copy(__first, __last, __result); }
    };

  /**
   *  @brief Copies the range [first,last) into result.
   *  @param  first  An input iterator.
   *  @param  last   An input iterator.
   *  @param  result An output iterator.
   *  @return   result + (first - last)
   *
   *  Like copy(), but does not require an initialized output range.
  */
  template<typename _InputIterator, typename _ForwardIterator>
    inline _ForwardIterator
    uninitialized_copy(_InputIterator __first, _InputIterator __last,
		       _ForwardIterator __result)
    {
      typedef typename iterator_traits<_InputIterator>::value_type
	_ValueType1;
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType2;

      return std::__uninitialized_copy<(__is_pod(_ValueType1)
					&& __is_pod(_ValueType2))>::
	uninitialized_copy(__first, __last, __result);
    }


  template<bool>
    struct __uninitialized_fill
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        uninitialized_fill(_ForwardIterator __first,
			   _ForwardIterator __last, const _Tp& __x)
        {
	  _ForwardIterator __cur = __first;
	  __try
	    {
	      for (; __cur != __last; ++__cur)
		std::_Construct(&*__cur, __x);
	    }
	  __catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      __throw_exception_again;
	    }
	}
    };

  template<>
    struct __uninitialized_fill<true>
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        uninitialized_fill(_ForwardIterator __first,
			   _ForwardIterator __last, const _Tp& __x)
        { std::fill(__first, __last, __x); }
    };

  /**
   *  @brief Copies the value x into the range [first,last).
   *  @param  first  An input iterator.
   *  @param  last   An input iterator.
   *  @param  x      The source value.
   *  @return   Nothing.
   *
   *  Like fill(), but does not require an initialized output range.
  */
  template<typename _ForwardIterator, typename _Tp>
    inline void
    uninitialized_fill(_ForwardIterator __first, _ForwardIterator __last,
		       const _Tp& __x)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;

      std::__uninitialized_fill<__is_pod(_ValueType)>::
	uninitialized_fill(__first, __last, __x);
    }


  template<bool>
    struct __uninitialized_construct_range_dispatch
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        __ucr(_ForwardIterator __first, _ForwardIterator __last,
	      _Tp& __value)
        {
	  if(__first == __last)
	    return;

	  _ForwardIterator __cur = __first;
	  __try
	    {
	      std::_Construct(&*__first, _GLIBCXX_MOVE(__value));
	      _ForwardIterator __prev = __cur;
	      ++__cur;
	      for(; __cur != __last; ++__cur, ++__prev)
		std::_Construct(&*__cur, _GLIBCXX_MOVE(*__prev));
	      __value = _GLIBCXX_MOVE(*__prev);
	    }
	  __catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      __throw_exception_again;
	    }
	}
    };

  template<>
    struct __uninitialized_construct_range_dispatch<true>
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        __ucr(_ForwardIterator, _ForwardIterator, _Tp&) { }
    };

  // Constructs objects in the range [first, last).
  // Note that while these new objects will take valid values,
  // their exact value is not defined. In particular they may
  // be 'moved from'.
  //
  // While __value may altered during this algorithm, it will have
  // the same value when the algorithm finishes, unless one of the
  // constructions throws.
  //
  // Requirements: _ForwardIterator::value_type(_Tp&&) is valid.
  template<typename _ForwardIterator, typename _Tp>
    inline void
    __uninitialized_construct_range(_ForwardIterator __first,
				    _ForwardIterator __last,
				    _Tp& __value)
    {
      typedef typename std::iterator_traits<_ForwardIterator>::value_type
	_ValueType;

      std::__uninitialized_construct_range_dispatch<
        __has_trivial_constructor(_ValueType)>::
	  __ucr(__first, __last, __value);
    }


  template<bool>
    struct __uninitialized_fill_n
    {
      template<typename _ForwardIterator, typename _Size, typename _Tp>
        static void
        uninitialized_fill_n(_ForwardIterator __first, _Size __n,
			     const _Tp& __x)
        {
	  _ForwardIterator __cur = __first;
	  __try
	    {
	      for (; __n > 0; --__n, ++__cur)
		std::_Construct(&*__cur, __x);
	    }
	  __catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      __throw_exception_again;
	    }
	}
    };

  template<>
    struct __uninitialized_fill_n<true>
    {
      template<typename _ForwardIterator, typename _Size, typename _Tp>
        static void
        uninitialized_fill_n(_ForwardIterator __first, _Size __n,
			     const _Tp& __x)
        { std::fill_n(__first, __n, __x); }
    };

  /**
   *  @brief Copies the value x into the range [first,first+n).
   *  @param  first  An input iterator.
   *  @param  n      The number of copies to make.
   *  @param  x      The source value.
   *  @return   Nothing.
   *
   *  Like fill_n(), but does not require an initialized output range.
  */
  template<typename _ForwardIterator, typename _Size, typename _Tp>
    inline void
    uninitialized_fill_n(_ForwardIterator __first, _Size __n, const _Tp& __x)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;

      std::__uninitialized_fill_n<__is_pod(_ValueType)>::
	uninitialized_fill_n(__first, __n, __x);
    }

  // Extensions: versions of uninitialized_copy, uninitialized_fill,
  //  and uninitialized_fill_n that take an allocator parameter.
  //  We dispatch back to the standard versions when we're given the
  //  default allocator.  For nondefault allocators we do not use 
  //  any of the POD optimizations.

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    _ForwardIterator
    __uninitialized_copy_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __result;
      __try
	{
	  for (; __first != __last; ++__first, ++__cur)
	    __alloc.construct(&*__cur, *__first);
	  return __cur;
	}
      __catch(...)
	{
	  std::_Destroy(__result, __cur, __alloc);
	  __throw_exception_again;
	}
    }

  template<typename _InputIterator, typename _ForwardIterator, typename _Tp>
    inline _ForwardIterator
    __uninitialized_copy_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, allocator<_Tp>&)
    { return std::uninitialized_copy(__first, __last, __result); }

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, _Allocator& __alloc)
    {
      return std::__uninitialized_copy_a(_GLIBCXX_MAKE_MOVE_ITERATOR(__first),
					 _GLIBCXX_MAKE_MOVE_ITERATOR(__last),
					 __result, __alloc);
    }

  template<typename _ForwardIterator, typename _Tp, typename _Allocator>
    void
    __uninitialized_fill_a(_ForwardIterator __first, _ForwardIterator __last,
			   const _Tp& __x, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      __try
	{
	  for (; __cur != __last; ++__cur)
	    __alloc.construct(&*__cur, __x);
	}
      __catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  __throw_exception_again;
	}
    }

  template<typename _ForwardIterator, typename _Tp, typename _Tp2>
    inline void
    __uninitialized_fill_a(_ForwardIterator __first, _ForwardIterator __last,
			   const _Tp& __x, allocator<_Tp2>&)
    { std::uninitialized_fill(__first, __last, __x); }

  template<typename _ForwardIterator, typename _Size, typename _Tp,
	   typename _Allocator>
    void
    __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n, 
			     const _Tp& __x, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      __try
	{
	  for (; __n > 0; --__n, ++__cur)
	    __alloc.construct(&*__cur, __x);
	}
      __catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  __throw_exception_again;
	}
    }

  template<typename _ForwardIterator, typename _Size, typename _Tp,
	   typename _Tp2>
    inline void
    __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n, 
			     const _Tp& __x, allocator<_Tp2>&)
    { std::uninitialized_fill_n(__first, __n, __x); }


  // Extensions: __uninitialized_copy_move, __uninitialized_move_copy,
  // __uninitialized_fill_move, __uninitialized_move_fill.
  // All of these algorithms take a user-supplied allocator, which is used
  // for construction and destruction.

  // __uninitialized_copy_move
  // Copies [first1, last1) into [result, result + (last1 - first1)), and
  //  move [first2, last2) into
  //  [result, result + (last1 - first1) + (last2 - first2)).
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _ForwardIterator, typename _Allocator>
    inline _ForwardIterator
    __uninitialized_copy_move(_InputIterator1 __first1,
			      _InputIterator1 __last1,
			      _InputIterator2 __first2,
			      _InputIterator2 __last2,
			      _ForwardIterator __result,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid = std::__uninitialized_copy_a(__first1, __last1,
							   __result,
							   __alloc);
      __try
	{
	  return std::__uninitialized_move_a(__first2, __last2, __mid, __alloc);
	}
      __catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  __throw_exception_again;
	}
    }

  // __uninitialized_move_copy
  // Moves [first1, last1) into [result, result + (last1 - first1)), and
  //  copies [first2, last2) into
  //  [result, result + (last1 - first1) + (last2 - first2)).
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _ForwardIterator, typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_copy(_InputIterator1 __first1,
			      _InputIterator1 __last1,
			      _InputIterator2 __first2,
			      _InputIterator2 __last2,
			      _ForwardIterator __result,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid = std::__uninitialized_move_a(__first1, __last1,
							   __result,
							   __alloc);
      __try
	{
	  return std::__uninitialized_copy_a(__first2, __last2, __mid, __alloc);
	}
      __catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  __throw_exception_again;
	}
    }
  
  // __uninitialized_fill_move
  // Fills [result, mid) with x, and moves [first, last) into
  //  [mid, mid + (last - first)).
  template<typename _ForwardIterator, typename _Tp, typename _InputIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_fill_move(_ForwardIterator __result, _ForwardIterator __mid,
			      const _Tp& __x, _InputIterator __first,
			      _InputIterator __last, _Allocator& __alloc)
    {
      std::__uninitialized_fill_a(__result, __mid, __x, __alloc);
      __try
	{
	  return std::__uninitialized_move_a(__first, __last, __mid, __alloc);
	}
      __catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  __throw_exception_again;
	}
    }

  // __uninitialized_move_fill
  // Moves [first1, last1) into [first2, first2 + (last1 - first1)), and
  //  fills [first2 + (last1 - first1), last2) with x.
  template<typename _InputIterator, typename _ForwardIterator, typename _Tp,
	   typename _Allocator>
    inline void
    __uninitialized_move_fill(_InputIterator __first1, _InputIterator __last1,
			      _ForwardIterator __first2,
			      _ForwardIterator __last2, const _Tp& __x,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid2 = std::__uninitialized_move_a(__first1, __last1,
							    __first2,
							    __alloc);
      __try
	{
	  std::__uninitialized_fill_a(__mid2, __last2, __x, __alloc);
	}
      __catch(...)
	{
	  std::_Destroy(__first2, __mid2, __alloc);
	  __throw_exception_again;
	}
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  template<typename _InputIterator, typename _Size,
	   typename _ForwardIterator>
    _ForwardIterator
    __uninitialized_copy_n(_InputIterator __first, _Size __n,
			   _ForwardIterator __result, input_iterator_tag)
    {
      _ForwardIterator __cur = __result;
      __try
	{
	  for (; __n > 0; --__n, ++__first, ++__cur)
	    ::new(static_cast<void*>(&*__cur)) typename
		iterator_traits<_ForwardIterator>::value_type(*__first);
	  return __cur;
	}
      __catch(...)
	{
	  std::_Destroy(__result, __cur);
	  __throw_exception_again;
	}
    }

  template<typename _RandomAccessIterator, typename _Size,
	   typename _ForwardIterator>
    inline _ForwardIterator
    __uninitialized_copy_n(_RandomAccessIterator __first, _Size __n,
			   _ForwardIterator __result,
			   random_access_iterator_tag)
    { return std::uninitialized_copy(__first, __first + __n, __result); }

  /**
   *  @brief Copies the range [first,first+n) into result.
   *  @param  first  An input iterator.
   *  @param  n      The number of elements to copy.
   *  @param  result An output iterator.
   *  @return  result + n
   *
   *  Like copy_n(), but does not require an initialized output range.
  */
  template<typename _InputIterator, typename _Size, typename _ForwardIterator>
    inline _ForwardIterator
    uninitialized_copy_n(_InputIterator __first, _Size __n,
			 _ForwardIterator __result)
    { return std::__uninitialized_copy_n(__first, __n, __result,
					 std::__iterator_category(__first)); }
#endif

_GLIBCXX_END_NAMESPACE

#endif /* _STL_UNINITIALIZED_H */
