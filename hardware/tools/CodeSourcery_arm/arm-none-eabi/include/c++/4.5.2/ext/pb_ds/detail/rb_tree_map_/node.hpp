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
 * @file node.hpp
 * Contains an implementation for rb_tree_.
 */

#ifndef PB_DS_RB_TREE_NODE_HPP
#define PB_DS_RB_TREE_NODE_HPP

#include <ext/pb_ds/detail/basic_tree_policy/null_node_metadata.hpp>

namespace __gnu_pbds
{
  namespace detail
  {
    template<typename Value_Type, class Metadata, class Allocator>
    struct rb_tree_node_
    {
    public:
      typedef Value_Type value_type;
      typedef Metadata metadata_type;

      typedef
      typename Allocator::template rebind<
      rb_tree_node_<
      Value_Type,
      Metadata,
      Allocator> >::other::pointer
      node_pointer;

      typedef
      typename Allocator::template rebind<
	metadata_type>::other::reference
      metadata_reference;

      typedef
      typename Allocator::template rebind<
	metadata_type>::other::const_reference
      const_metadata_reference;

      inline bool
      special() const
      { return m_red; }

      inline const_metadata_reference
      get_metadata() const
      { return m_metadata; }

      inline metadata_reference
      get_metadata()
      { return m_metadata; }

#ifdef PB_DS_BIN_SEARCH_TREE_TRACE_
      void
      trace() const
      {
	std::cout << PB_DS_V2F(m_value) <<(m_red? " <r> " : " <b> ") 
		  << "(" << m_metadata << ")";
      }
#endif

      node_pointer m_p_left;
      node_pointer m_p_right;
      node_pointer m_p_parent;
      value_type m_value;
      bool m_red;
      metadata_type m_metadata;
    };

    template<typename Value_Type, class Allocator>
    struct rb_tree_node_<Value_Type, null_node_metadata, Allocator>
    {
    public:
      typedef Value_Type value_type;
      typedef null_node_metadata metadata_type;

      typedef
      typename Allocator::template rebind<
      rb_tree_node_<
      Value_Type,
      null_node_metadata,
      Allocator> >::other::pointer
      node_pointer;

      inline bool
      special() const
      { return m_red; }

#ifdef PB_DS_BIN_SEARCH_TREE_TRACE_
      void
      trace() const
      { std::cout << PB_DS_V2F(m_value) <<(m_red? " <r> " : " <b> "); }
#endif 

      node_pointer m_p_left;
      node_pointer m_p_right;
      node_pointer m_p_parent;
      value_type m_value;
      bool m_red;
    };
  } // namespace detail
} // namespace __gnu_pbds

#endif 
