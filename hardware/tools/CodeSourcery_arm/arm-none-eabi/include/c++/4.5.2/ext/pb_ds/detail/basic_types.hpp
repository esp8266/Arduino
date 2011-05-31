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
 * @file basic_types.hpp
 * Contains basic types used by containers.
 */

#ifndef PB_DS_BASIC_TYPES_HPP
#define PB_DS_BASIC_TYPES_HPP

#include <algorithm>
#include <utility>
#include <ext/pb_ds/tag_and_trait.hpp>
#include <ext/pb_ds/detail/type_utils.hpp>

namespace __gnu_pbds
{
  namespace detail
  {
    template<typename Key, typename Mapped, typename Allocator, bool Store_Hash>
    struct value_type_base;

    /**
     * Specialization of value_type_base for the case where the hash value
     * is not stored alongside each value.
     **/
    template<typename Key, typename Mapped, typename Allocator>
    struct value_type_base<Key, Mapped, Allocator, false>
    {
      typedef typename Allocator::template rebind<Mapped>::other mapped_type_allocator;
      typedef typename mapped_type_allocator::value_type mapped_type;
      typedef typename mapped_type_allocator::pointer mapped_pointer;
      typedef typename mapped_type_allocator::const_pointer const_mapped_pointer;
      typedef typename mapped_type_allocator::reference mapped_reference;
      typedef typename mapped_type_allocator::const_reference const_mapped_reference;

      typedef typename Allocator::template rebind<std::pair<const Key, Mapped> >::other value_type_allocator;
      typedef typename value_type_allocator::value_type value_type;
      typedef typename value_type_allocator::pointer pointer;
      typedef typename value_type_allocator::const_pointer const_pointer;
      typedef typename value_type_allocator::reference reference;
      typedef typename value_type_allocator::const_reference const_reference;

      struct stored_value_type
      {
	value_type m_value;
      };
    };

    /**
     * Specialization of value_type_base for the case where the hash value
     * is stored alongside each value.
     **/
    template<typename Key, typename Mapped, typename Allocator>
    struct value_type_base<Key, Mapped, Allocator, true>
    {
      typedef typename Allocator::template rebind<Mapped>::other mapped_type_allocator;
      typedef typename mapped_type_allocator::value_type mapped_type;
      typedef typename mapped_type_allocator::pointer mapped_pointer;
      typedef typename mapped_type_allocator::const_pointer const_mapped_pointer;
      typedef typename mapped_type_allocator::reference mapped_reference;
      typedef typename mapped_type_allocator::const_reference const_mapped_reference;

      typedef typename Allocator::template rebind<std::pair<const Key, Mapped> >::other value_type_allocator;
      typedef typename value_type_allocator::value_type value_type;
      typedef typename value_type_allocator::pointer pointer;
      typedef typename value_type_allocator::const_pointer const_pointer;
      typedef typename value_type_allocator::reference reference;
      typedef typename value_type_allocator::const_reference const_reference;

      struct stored_value_type
      {
	value_type m_value;
	typename Allocator::size_type m_hash;
      };
    };

#define PB_DS_CLASS_T_DEC \
    template<typename Key, typename Allocator>

#define PB_DS_CLASS_C_DEC \
    value_type_base<Key, null_mapped_type, Allocator, false>

    /**
     * Specialization of value_type_base for the case where the hash value
     * is not stored alongside each value.
     **/
    template<typename Key, typename Allocator>
    struct value_type_base<Key, null_mapped_type, Allocator, false>
    {
      typedef typename Allocator::template rebind<null_mapped_type>::other mapped_type_allocator;
      typedef typename mapped_type_allocator::value_type mapped_type;
      typedef typename mapped_type_allocator::pointer mapped_pointer;
      typedef typename mapped_type_allocator::const_pointer const_mapped_pointer;
      typedef typename mapped_type_allocator::reference mapped_reference;
      typedef typename mapped_type_allocator::const_reference const_mapped_reference;

      typedef Key value_type;

      typedef typename Allocator::template rebind<value_type>::other value_type_allocator;
      typedef typename value_type_allocator::pointer pointer;
      typedef typename value_type_allocator::const_pointer const_pointer;
      typedef typename value_type_allocator::reference reference;
      typedef typename value_type_allocator::const_reference const_reference;

      struct stored_value_type
      {
	value_type m_value;
      };

      static null_mapped_type s_null_mapped;
    };

    PB_DS_CLASS_T_DEC
    null_mapped_type PB_DS_CLASS_C_DEC::s_null_mapped;

#undef PB_DS_CLASS_T_DEC
#undef PB_DS_CLASS_C_DEC

#define PB_DS_CLASS_T_DEC \
    template<typename Key, typename Allocator>

#define PB_DS_CLASS_C_DEC \
    value_type_base<Key, null_mapped_type, Allocator, true>

    /**
     * Specialization of value_type_base for the case where the hash value
     * is stored alongside each value.
     **/
    template<typename Key, typename Allocator>
    struct value_type_base<Key, null_mapped_type, Allocator, true>
    {
      typedef typename Allocator::template rebind<null_mapped_type>::other mapped_type_allocator;
      typedef typename mapped_type_allocator::value_type mapped_type;
      typedef typename mapped_type_allocator::pointer mapped_pointer;
      typedef typename mapped_type_allocator::const_pointer const_mapped_pointer;
      typedef typename mapped_type_allocator::reference mapped_reference;
      typedef typename mapped_type_allocator::const_reference const_mapped_reference;

      typedef Key value_type;

      typedef typename Allocator::template rebind<value_type>::other value_type_allocator;
      typedef typename value_type_allocator::pointer pointer;
      typedef typename value_type_allocator::const_pointer const_pointer;
      typedef typename value_type_allocator::reference reference;
      typedef typename value_type_allocator::const_reference const_reference;

      struct stored_value_type
      {
	value_type m_value;
	typename Allocator::size_type m_hash;
      };

      static null_mapped_type s_null_mapped;
    };

    PB_DS_CLASS_T_DEC
    null_mapped_type PB_DS_CLASS_C_DEC::s_null_mapped;

#undef PB_DS_CLASS_T_DEC
#undef PB_DS_CLASS_C_DEC

    template<typename Key, typename Mapped>
    struct no_throw_copies
    {
      typedef integral_constant<int, is_simple<Key>::value && is_simple<Mapped>::value> indicator;
    };

    template<typename Key>
    struct no_throw_copies<Key, null_mapped_type>
    {
      typedef integral_constant<int, is_simple<Key>::value> indicator;
    };
  } // namespace detail
} // namespace __gnu_pbds

#endif 

