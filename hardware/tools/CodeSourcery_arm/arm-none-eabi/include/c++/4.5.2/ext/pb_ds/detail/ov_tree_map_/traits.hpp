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
 * @file traits.hpp
 * Contains an implementation class for ov_tree_.
 */

#ifndef PB_DS_OV_TREE_NODE_AND_IT_TRAITS_HPP
#define PB_DS_OV_TREE_NODE_AND_IT_TRAITS_HPP

#include <ext/pb_ds/detail/ov_tree_map_/node_iterators.hpp>

namespace __gnu_pbds
{
  namespace detail
  {

    template<typename Key,
	     typename Mapped,
	     class Cmp_Fn,
	     template<typename Const_Node_Iterator,
		      class Node_Iterator,
		      class Cmp_Fn_,
		      class Allocator_>
    class Node_Update,
	     class Allocator>
    struct tree_traits<
      Key,
      Mapped,
      Cmp_Fn,
      Node_Update,
      ov_tree_tag,
      Allocator>
    {
    private:
      typedef
      typename types_traits<
      Key,
      Mapped,
      Allocator,
      false>::value_type
      value_type;

    public:
      typedef
      typename tree_node_metadata_selector<
      Key,
      Mapped,
      Cmp_Fn,
      Node_Update,
      Allocator>::type
      metadata_type;

      typedef
      ov_tree_node_const_it_<
	value_type,
	metadata_type,
	Allocator>
      const_node_iterator;

      typedef
      ov_tree_node_it_<
	value_type,
	metadata_type,
	Allocator>
      node_iterator;

      typedef
      Node_Update<
	const_node_iterator,
	node_iterator,
	Cmp_Fn,
	Allocator>
      node_update;

      typedef
      __gnu_pbds::null_tree_node_update<
	const_node_iterator,
	node_iterator,
	Cmp_Fn,
	Allocator>* 
      null_node_update_pointer;
    };

    template<typename Key,
	     class Cmp_Fn,
	     template<typename Const_Node_Iterator,
		      class Node_Iterator,
		      class Cmp_Fn_,
		      class Allocator_>
    class Node_Update,
	     class Allocator>
    struct tree_traits<
      Key,
      null_mapped_type,
      Cmp_Fn,
      Node_Update,
      ov_tree_tag,
      Allocator>
    {
    private:
      typedef
      typename types_traits<
      Key,
      null_mapped_type,
      Allocator,
      false>::value_type
      value_type;

    public:
      typedef
      typename tree_node_metadata_selector<
      Key,
      null_mapped_type,
      Cmp_Fn,
      Node_Update,
      Allocator>::type
      metadata_type;

      typedef
      ov_tree_node_const_it_<
	value_type,
	metadata_type,
	Allocator>
      const_node_iterator;

      typedef const_node_iterator node_iterator;

      typedef
      Node_Update<
	const_node_iterator,
	const_node_iterator,
	Cmp_Fn,
	Allocator>
      node_update;

      typedef
      __gnu_pbds::null_tree_node_update<
	const_node_iterator,
	node_iterator,
	Cmp_Fn,
	Allocator>* 
      null_node_update_pointer;
    };

  } // namespace detail
} // namespace __gnu_pbds

#endif // #ifndef PB_DS_OV_TREE_NODE_AND_IT_TRAITS_HPP

