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
 * @file head.hpp
 * Contains a leaf for a patricia tree.
 */

#ifndef PB_DS_PAT_TRIE_IHEAD_HPP
#define PB_DS_PAT_TRIE_IHEAD_HPP

#include <ext/pb_ds/detail/pat_trie_/node_base.hpp>
#include <debug/debug.h>

namespace __gnu_pbds
{
  namespace detail
  {
#define PB_DS_CLASS_T_DEC \
    template<typename Type_Traits, typename E_Access_Traits,	\
	      typename Metadata, typename Allocator>

#define PB_DS_CLASS_C_DEC \
    pat_trie_head<Type_Traits, E_Access_Traits,	Metadata, Allocator>

#define PB_DS_BASE_C_DEC \
    pat_trie_node_base<Type_Traits, E_Access_Traits, Metadata, Allocator>

    template<typename Type_Traits,
	     typename E_Access_Traits,
	     typename Metadata,
	     typename Allocator>
    struct pat_trie_head : public PB_DS_BASE_C_DEC
    {
    private:
      typedef E_Access_Traits e_access_traits;

      typedef
      typename Allocator::template rebind<
	e_access_traits>::other::const_pointer
      const_e_access_traits_pointer;

      typedef
      typename Allocator::template rebind<
	PB_DS_BASE_C_DEC>::other::pointer
      node_pointer;

#ifdef _GLIBCXX_DEBUG
      typedef
      typename PB_DS_BASE_C_DEC::subtree_debug_info
      subtree_debug_info;
#endif 

    public:
      pat_trie_head();

#ifdef _GLIBCXX_DEBUG
      virtual subtree_debug_info
      assert_valid_imp(const_e_access_traits_pointer p_traits) const;
#endif 

    public:
      node_pointer m_p_min;

      node_pointer m_p_max;
    };

    PB_DS_CLASS_T_DEC
    PB_DS_CLASS_C_DEC::
    pat_trie_head() : PB_DS_BASE_C_DEC(pat_trie_head_node_type)
    { }

#ifdef _GLIBCXX_DEBUG
    PB_DS_CLASS_T_DEC
    typename PB_DS_CLASS_C_DEC::subtree_debug_info
    PB_DS_CLASS_C_DEC::
    assert_valid_imp(const_e_access_traits_pointer /*p_traits*/) const
    {
      _GLIBCXX_DEBUG_ASSERT(false);
      return subtree_debug_info();
    }
#endif 

#undef PB_DS_CLASS_T_DEC
#undef PB_DS_CLASS_C_DEC
#undef PB_DS_BASE_C_DEC

  } // namespace detail
} // namespace __gnu_pbds

#endif

