// String support -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
// 2010
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

/** @file stringfwd.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

//
// ISO C++ 14882: 21 Strings library
//

#ifndef _STRINGFWD_H
#define _STRINGFWD_H 1

#pragma GCC system_header

#include <bits/c++config.h>

_GLIBCXX_BEGIN_NAMESPACE(std)

  template<typename _Alloc>
    class allocator;

  /**
   *  @defgroup strings Strings
   *
   *  @{
  */

  template<class _CharT>
    struct char_traits;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
           typename _Alloc = allocator<_CharT> >
    class basic_string;

  template<> struct char_traits<char>;

  typedef basic_string<char>    string;   ///< A string of @c char

#ifdef _GLIBCXX_USE_WCHAR_T
  template<> struct char_traits<wchar_t>;

  typedef basic_string<wchar_t> wstring;   ///< A string of @c wchar_t
#endif

#if (defined(__GXX_EXPERIMENTAL_CXX0X__) \
     && defined(_GLIBCXX_USE_C99_STDINT_TR1))

  template<> struct char_traits<char16_t>;
  template<> struct char_traits<char32_t>;

  typedef basic_string<char16_t> u16string; ///< A string of @c char16_t
  typedef basic_string<char32_t> u32string; ///< A string of @c char32_t

#endif
  /** @}  */

_GLIBCXX_END_NAMESPACE

#endif	// _STRINGFWD_H
