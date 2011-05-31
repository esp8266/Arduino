// -*- C++ -*-

// Copyright (C) 2005, 2006, 2009 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

// Copyright (C) 2004 Ami Tavory and Vladimir Dreizin, IBM-HRL.

// Permission to use, copy, modify, sell, and distribute this software
// is hereby granted without fee, provided that the above copyright
// notice appears in all copies, and that both that copyright notice
// and this permission notice appear in supporting documentation. None
// of the above authors, nor IBM Haifa Research Laboratories, make any
// representation about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.

/**
 * @file constructors_destructor_fn_imps.hpp
 * Contains an implementation for rc_binomial_heap_.
 */

PB_DS_CLASS_T_DEC
PB_DS_CLASS_C_DEC::
rc_binomial_heap_()
{
  _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

PB_DS_CLASS_T_DEC
PB_DS_CLASS_C_DEC::
rc_binomial_heap_(const Cmp_Fn& r_cmp_fn) :
  PB_DS_BASE_C_DEC(r_cmp_fn)
{
  _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

PB_DS_CLASS_T_DEC
PB_DS_CLASS_C_DEC::
rc_binomial_heap_(const PB_DS_CLASS_C_DEC& other) :
  PB_DS_BASE_C_DEC(other)
{
  make_binomial_heap();

  base_type::find_max();

  _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

PB_DS_CLASS_T_DEC
PB_DS_CLASS_C_DEC::
~rc_binomial_heap_()
{ }

PB_DS_CLASS_T_DEC
void
PB_DS_CLASS_C_DEC::
swap(PB_DS_CLASS_C_DEC& other)
{
  _GLIBCXX_DEBUG_ONLY(assert_valid();)
    _GLIBCXX_DEBUG_ONLY(other.assert_valid();)

    base_type::swap(other);

  m_rc.swap(other.m_rc);

  _GLIBCXX_DEBUG_ONLY(assert_valid();)
    _GLIBCXX_DEBUG_ONLY(other.assert_valid();)
    }

