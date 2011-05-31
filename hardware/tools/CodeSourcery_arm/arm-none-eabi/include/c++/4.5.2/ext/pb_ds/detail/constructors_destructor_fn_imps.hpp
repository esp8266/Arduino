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
 * Contains constructors_destructor_fn_imps applicable to different containers.
 */

inline
PB_DS_CLASS_NAME()
{ }

inline
PB_DS_CLASS_NAME(const PB_DS_CLASS_NAME& other) 
: base_type((const base_type&)other)
{ }

template<typename T0>
inline
PB_DS_CLASS_NAME(T0 t0) : base_type(t0)
{ }

template<typename T0, typename T1>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1) : base_type(t0, t1)
{ }

template<typename T0, typename T1, typename T2>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1, T2 t2) : base_type(t0, t1, t2)
{ }

template<typename T0, typename T1, typename T2, typename T3>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1, T2 t2, T3 t3) 
: base_type(t0, t1, t2, t3)
{ }

template<typename T0, typename T1, typename T2, typename T3, typename T4>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4) 
: base_type(t0, t1, t2, t3, t4)
{ }

template<typename T0, typename T1, typename T2, typename T3, typename T4,
	 typename T5>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) 
: base_type(t0, t1, t2, t3, t4, t5)
{ }

template<typename T0, typename T1, typename T2, typename T3, typename T4,
	 typename T5, typename T6>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) 
: base_type(t0, t1, t2, t3, t4, t5, t6)
{ }

template<typename T0, typename T1, typename T2, typename T3, typename T4,
	 typename T5, typename T6, typename T7>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) 
: base_type(t0, t1, t2, t3, t4, t5, t6, t7)
{ }

template<typename T0, typename T1, typename T2, typename T3, typename T4,
	 typename T5, typename T6, typename T7, typename T8>
inline
PB_DS_CLASS_NAME(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
: base_type(t0, t1, t2, t3, t4, t5, t6, t7, t8)
{ }
