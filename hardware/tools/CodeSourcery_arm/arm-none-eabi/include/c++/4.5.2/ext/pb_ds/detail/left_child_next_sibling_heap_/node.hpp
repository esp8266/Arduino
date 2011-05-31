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
 * Contains an implementation struct for this type of heap's node.
 */

#ifndef PB_DS_LEFT_CHILD_NEXT_SIBLING_HEAP_NODE_HPP
#define PB_DS_LEFT_CHILD_NEXT_SIBLING_HEAP_NODE_HPP

#include <ext/pb_ds/detail/left_child_next_sibling_heap_/null_metadata.hpp>

namespace __gnu_pbds
{
  namespace detail
  {

    template<typename Value_Type, typename Metadata_Type, class Allocator>
    struct left_child_next_sibling_heap_node_
    {
    private:
      typedef
      left_child_next_sibling_heap_node_<
      Value_Type,
      Metadata_Type,
      Allocator>
      this_type;

    public:
      typedef typename Allocator::size_type size_type;

      typedef
      typename Allocator::template rebind<
	this_type>::other::pointer
      node_pointer;

      typedef Value_Type value_type;

      typedef Metadata_Type metadata_type;

    public:
      value_type m_value;

      metadata_type m_metadata;

      node_pointer m_p_l_child;

      node_pointer m_p_next_sibling;

      node_pointer m_p_prev_or_parent;
    };

    template<typename Value_Type, class Allocator>
    struct left_child_next_sibling_heap_node_<
      Value_Type,
      null_left_child_next_sibling_heap_node_metadata,
      Allocator>
    {
    private:
      typedef
      left_child_next_sibling_heap_node_<
      Value_Type,
      null_left_child_next_sibling_heap_node_metadata,
      Allocator>
      this_type;

    public:
      typedef typename Allocator::size_type size_type;

      typedef
      typename Allocator::template rebind<
	this_type>::other::pointer
      node_pointer;

      typedef Value_Type value_type;

    public:
      value_type m_value;

      node_pointer m_p_l_child;

      node_pointer m_p_next_sibling;

      node_pointer m_p_prev_or_parent;
    };

  } // namespace detail
} // namespace __gnu_pbds

#endif // #ifndef PB_DS_LEFT_CHILD_NEXT_SIBLING_HEAP_NODE_HPP
