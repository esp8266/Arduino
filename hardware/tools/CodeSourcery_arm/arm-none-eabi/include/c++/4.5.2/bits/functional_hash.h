// functional_hash.h header -*- C++ -*-

// Copyright (C) 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
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

/** @file bits/functional_hash.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _FUNCTIONAL_HASH_H
#define _FUNCTIONAL_HASH_H 1

#pragma GCC system_header

#include <cstddef>
#include <bits/stl_function.h>

namespace std
{
  /** @defgroup hashes Hashes
   *  @ingroup functors
   *
   *   Hashing functors taking a variable type and returning a @c std::size_t.
   *
   *  @{
   */

  /// Primary class template hash.
  template<typename _Tp>
    struct hash : public std::unary_function<_Tp, size_t>
    {
      size_t
      operator()(_Tp __val) const;
    };

  /// Partial specializations for pointer types.
  template<typename _Tp>
    struct hash<_Tp*> : public std::unary_function<_Tp*, size_t>
    {
      size_t
      operator()(_Tp* __p) const
      { return reinterpret_cast<size_t>(__p); }
    };

  // Explicit specializations for integer types.
#define _Cxx_hashtable_define_trivial_hash(_Tp) 	\
  template<>						\
    inline size_t					\
    hash<_Tp>::operator()(_Tp __val) const		\
    { return static_cast<size_t>(__val); }

  /// Explicit specialization for bool.
  _Cxx_hashtable_define_trivial_hash(bool);

  /// Explicit specialization for char.
  _Cxx_hashtable_define_trivial_hash(char);

  /// Explicit specialization for signed char.
  _Cxx_hashtable_define_trivial_hash(signed char);

  /// Explicit specialization for unsigned char.
  _Cxx_hashtable_define_trivial_hash(unsigned char);

  /// Explicit specialization for wchar_t.
  _Cxx_hashtable_define_trivial_hash(wchar_t);

  /// Explicit specialization for char16_t.
  _Cxx_hashtable_define_trivial_hash(char16_t);

  /// Explicit specialization for char32_t.
  _Cxx_hashtable_define_trivial_hash(char32_t);

  /// Explicit specialization for short.
  _Cxx_hashtable_define_trivial_hash(short);

  /// Explicit specialization for int.
  _Cxx_hashtable_define_trivial_hash(int);

  /// Explicit specialization for long.
  _Cxx_hashtable_define_trivial_hash(long);

  /// Explicit specialization for long long.
  _Cxx_hashtable_define_trivial_hash(long long);

  /// Explicit specialization for unsigned short.
  _Cxx_hashtable_define_trivial_hash(unsigned short);

  /// Explicit specialization for unsigned int.
  _Cxx_hashtable_define_trivial_hash(unsigned int);

  /// Explicit specialization for unsigned long.
  _Cxx_hashtable_define_trivial_hash(unsigned long);

  /// Explicit specialization for unsigned long long.
  _Cxx_hashtable_define_trivial_hash(unsigned long long);

#undef _Cxx_hashtable_define_trivial_hash

  // Fowler / Noll / Vo (FNV) Hash (type FNV-1a)

  // Dummy generic implementation (for sizeof(size_t) != 4, 8).
  template<size_t>
    struct _Fnv_hash_base
    {
      template<typename _Tp>
        static size_t
        hash(const _Tp* __ptr, size_t __clength, size_t __hash = 0)
        {
	  const char* __cptr = reinterpret_cast<const char*>(__ptr);
	  for (; __clength; --__clength)
	    __hash = (__hash * 131) + *__cptr++;
	  return __hash;
	}
    };

  template<>
    struct _Fnv_hash_base<4>
    {
      template<typename _Tp>
        static size_t
        hash(const _Tp* __ptr, size_t __clength,
	     size_t __hash = static_cast<size_t>(2166136261UL))
        {
	  const char* __cptr = reinterpret_cast<const char*>(__ptr);
	  for (; __clength; --__clength)
	    {
	      __hash ^= static_cast<size_t>(*__cptr++);
	      __hash *= static_cast<size_t>(16777619UL);
	    }
	  return __hash;
	}
    };
  
  template<>
    struct _Fnv_hash_base<8>
    {
      template<typename _Tp>
        static size_t
        hash(const _Tp* __ptr, size_t __clength,
	     size_t __hash = static_cast<size_t>(14695981039346656037ULL))
        {
	  const char* __cptr = reinterpret_cast<const char*>(__ptr);
	  for (; __clength; --__clength)
	    {
	      __hash ^= static_cast<size_t>(*__cptr++);
	      __hash *= static_cast<size_t>(1099511628211ULL);
	    }
	  return __hash;
	}
    };

    struct _Fnv_hash
    : public _Fnv_hash_base<sizeof(size_t)>
    {
      using _Fnv_hash_base<sizeof(size_t)>::hash;

      template<typename _Tp>
        static size_t
        hash(const _Tp& __val)
        { return hash(&__val, sizeof(__val)); }

      template<typename _Tp>
        static size_t
        __hash_combine(const _Tp& __val, size_t __hash)
        { return hash(&__val, sizeof(__val), __hash); }
    };

  /// Specialization for float.
  template<>
    inline size_t
    hash<float>::operator()(float __val) const
    {
      // 0 and -0 both hash to zero.
      return __val != 0.0f ? std::_Fnv_hash::hash(__val) : 0;
    }

  /// Specialization for double.
  template<>
    inline size_t
    hash<double>::operator()(double __val) const
    {
      // 0 and -0 both hash to zero.
      return __val != 0.0 ? std::_Fnv_hash::hash(__val) : 0;
    }

  /// Specialization for long double.
  template<>
    _GLIBCXX_PURE size_t
    hash<long double>::operator()(long double __val) const;

  // @} group hashes
}

#endif // _FUNCTIONAL_HASH_H
