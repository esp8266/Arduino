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
 * @file priority_queue_base_dispatch.hpp
 * Contains an pqiative container dispatching base.
 */

#ifndef PB_DS_PRIORITY_QUEUE_BASE_DS_DISPATCHER_HPP
#define PB_DS_PRIORITY_QUEUE_BASE_DS_DISPATCHER_HPP

#include <ext/pb_ds/detail/pairing_heap_/pairing_heap_.hpp>
#include <ext/pb_ds/detail/binomial_heap_/binomial_heap_.hpp>
#include <ext/pb_ds/detail/rc_binomial_heap_/rc_binomial_heap_.hpp>
#include <ext/pb_ds/detail/binary_heap_/binary_heap_.hpp>
#include <ext/pb_ds/detail/thin_heap_/thin_heap_.hpp>

namespace __gnu_pbds
{
    namespace detail
    {

      template<typename Value_Type, typename Cmp_Fn, typename Tag, typename Allocator>
      struct priority_queue_base_dispatch;

      template<typename Value_Type, typename Cmp_Fn, typename Allocator>
      struct priority_queue_base_dispatch<Value_Type, Cmp_Fn, pairing_heap_tag, Allocator>
      {
	typedef pairing_heap_< Value_Type, Cmp_Fn, Allocator> type;
      };

      template<typename Value_Type, typename Cmp_Fn, typename Allocator>
      struct priority_queue_base_dispatch<Value_Type, Cmp_Fn, binomial_heap_tag, Allocator>
      {
	typedef binomial_heap_< Value_Type, Cmp_Fn, Allocator> type;
      };

      template<typename Value_Type, typename Cmp_Fn, typename Allocator>
      struct priority_queue_base_dispatch<Value_Type, Cmp_Fn, rc_binomial_heap_tag, Allocator>
      {
	typedef rc_binomial_heap_< Value_Type, Cmp_Fn, Allocator> type;
      };

      template<typename Value_Type, typename Cmp_Fn, typename Allocator>
      struct priority_queue_base_dispatch<Value_Type, Cmp_Fn, binary_heap_tag, Allocator>
      {
	typedef binary_heap_< Value_Type, Cmp_Fn, Allocator> type;
      };

      template<typename Value_Type, typename Cmp_Fn, typename Allocator>
      struct priority_queue_base_dispatch<Value_Type, Cmp_Fn, thin_heap_tag, Allocator>
      {
	typedef thin_heap_< Value_Type, Cmp_Fn, Allocator> type;
      };

    } // namespace detail
} // namespace __gnu_pbds

#endif // #ifndef PB_DS_PRIORITY_QUEUE_BASE_DS_DISPATCHER_HPP
