// Locale support -*- C++ -*-

// Copyright (C) 2000, 2001, 2002, 2009 Free Software Foundation, Inc.
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

/** @file ctype_noninline.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

//
// ISO C++ 14882: 22.1  Locales
//
  
// Information as gleaned from /usr/include/ctype.h
  
  const ctype_base::mask*
  ctype<char>::classic_table() throw()
  { return _ctype_ + 1; }

  ctype<char>::ctype(__c_locale, const mask* __table, bool __del, 
		     size_t __refs) 
  : facet(__refs), _M_del(__table != 0 && __del), 
  _M_toupper(NULL), _M_tolower(NULL), 
  _M_table(__table ? __table : classic_table()) 
  { 
    memset(_M_widen, 0, sizeof(_M_widen));
    _M_widen_ok = 0;
    memset(_M_narrow, 0, sizeof(_M_narrow));
    _M_narrow_ok = 0;
  }

  ctype<char>::ctype(const mask* __table, bool __del, size_t __refs) 
  : facet(__refs), _M_del(__table != 0 && __del), 
  _M_toupper(NULL), _M_tolower(NULL), 
  _M_table(__table ? __table : classic_table()) 
  { 
    memset(_M_widen, 0, sizeof(_M_widen));
    _M_widen_ok = 0;
    memset(_M_narrow, 0, sizeof(_M_narrow));
    _M_narrow_ok = 0;
  }

  char
  ctype<char>::do_toupper(char __c) const
  { 
    int __x = __c;
    return (this->is(ctype_base::lower, __c) ? (__x - 'a' + 'A') : __x);
  }

  const char*
  ctype<char>::do_toupper(char* __low, const char* __high) const
  {
    while (__low < __high)
      {
	*__low = this->do_toupper(*__low);
	++__low;
      }
    return __high;
  }

  char
  ctype<char>::do_tolower(char __c) const
  { 
    int __x = __c;
    return (this->is(ctype_base::upper, __c) ? (__x - 'A' + 'a') : __x);
  }

  const char* 
  ctype<char>::do_tolower(char* __low, const char* __high) const
  {
    while (__low < __high)
      {
	*__low = this->do_tolower(*__low);
	++__low;
      }
    return __high;
  }

