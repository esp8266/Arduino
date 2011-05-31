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
 * Contains an implementation class for pat_trie_.
 */

#ifndef PB_DS_PAT_TRIE_NODE_AND_IT_TRAITS_HPP
#define PB_DS_PAT_TRIE_NODE_AND_IT_TRAITS_HPP

#include <ext/pb_ds/detail/pat_trie_/node_base.hpp>
#include <ext/pb_ds/detail/pat_trie_/head.hpp>
#include <ext/pb_ds/detail/pat_trie_/leaf.hpp>
#include <ext/pb_ds/detail/pat_trie_/internal_node.hpp>
#include <ext/pb_ds/detail/pat_trie_/point_iterators.hpp>
#include <ext/pb_ds/detail/pat_trie_/node_iterators.hpp>
#include <ext/pb_ds/detail/pat_trie_/synth_e_access_traits.hpp>

namespace __gnu_pbds
{
  namespace detail
  {

    template<typename Key,
	     typename Mapped,
	     class E_Access_Traits,
	     template<typename Const_Node_Iterator,
		      class Node_Iterator,
		      class Cmp_Fn_,
		      class Allocator_>
    class Node_Update,
	     class Allocator>
    struct trie_traits<
      Key,
      Mapped,
      E_Access_Traits,
      Node_Update,
      pat_trie_tag,
      Allocator>
    {
    private:
      typedef types_traits< Key, Mapped, Allocator, false> type_traits;

    public:
      typedef
      typename trie_node_metadata_selector<
      Key,
      Mapped,
      E_Access_Traits,
      Node_Update,
      Allocator>::type
      metadata_type;

      typedef E_Access_Traits e_access_traits;

      typedef
      __gnu_pbds::detail::synth_e_access_traits<
	type_traits,
	false,
	e_access_traits>
      synth_e_access_traits;

      typedef
      pat_trie_node_base<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      node;

      typedef
      pat_trie_leaf<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      leaf;

      typedef
      pat_trie_head<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      head;

      typedef
      pat_trie_internal_node<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      internal_node;

      typedef
      pat_trie_const_it_<
	type_traits,
	node,
	leaf,
	head,
	internal_node,
	true,
	Allocator>
      const_iterator;

      typedef
      pat_trie_it_<
	type_traits,
	node,
	leaf,
	head,
	internal_node,
	true,
	Allocator>
      iterator;

      typedef
      pat_trie_const_it_<
	type_traits,
	node,
	leaf,
	head,
	internal_node,
	false,
	Allocator>
      const_reverse_iterator;

      typedef
      pat_trie_it_<
	type_traits,
	node,
	leaf,
	head,
	internal_node,
	false,
	Allocator>
      reverse_iterator;

      typedef
      pat_trie_const_node_it_<
	node,
	leaf,
	head,
	internal_node,
	const_iterator,
	iterator,
	synth_e_access_traits,
	Allocator>
      const_node_iterator;

      typedef
      pat_trie_node_it_<
	node,
	leaf,
	head,
	internal_node,
	const_iterator,
	iterator,
	synth_e_access_traits,
	Allocator>
      node_iterator;

      typedef
      Node_Update<
	const_node_iterator,
	node_iterator,
	E_Access_Traits,
	Allocator>
      node_update;

      typedef
      __gnu_pbds::null_trie_node_update<
	const_node_iterator,
	node_iterator,
	E_Access_Traits,
	Allocator>* 
      null_node_update_pointer;
    };

    template<typename Key,
	     class E_Access_Traits,
	     template<typename Const_Node_Iterator,
		      class Node_Iterator,
		      class Cmp_Fn_,
		      class Allocator_>
    class Node_Update,
	     class Allocator>
    struct trie_traits<
      Key,
      null_mapped_type,
      E_Access_Traits,
      Node_Update,
      pat_trie_tag,
      Allocator>
    {
    private:
      typedef
      types_traits<
      Key,
      null_mapped_type,
      Allocator,
      false>
      type_traits;

    public:
      typedef
      typename trie_node_metadata_selector<
      Key,
      null_mapped_type,
      E_Access_Traits,
      Node_Update,
      Allocator>::type
      metadata_type;

      typedef E_Access_Traits e_access_traits;

      typedef
      __gnu_pbds::detail::synth_e_access_traits<
	type_traits,
	true,
	e_access_traits>
      synth_e_access_traits;

      typedef
      pat_trie_node_base<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      node;

      typedef
      pat_trie_leaf<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      leaf;

      typedef
      pat_trie_head<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      head;

      typedef
      pat_trie_internal_node<
	type_traits,
	synth_e_access_traits,
	metadata_type,
	Allocator>
      internal_node;

      typedef
      pat_trie_const_it_<
	type_traits,
	node,
	leaf,
	head,
	internal_node,
	true,
	Allocator>
      const_iterator;

      typedef const_iterator iterator;

      typedef
      pat_trie_const_it_<
	type_traits,
	node,
	leaf,
	head,
	internal_node,
	false,
	Allocator>
      const_reverse_iterator;

      typedef const_reverse_iterator reverse_iterator;

      typedef
      pat_trie_const_node_it_<
	node,
	leaf,
	head,
	internal_node,
	const_iterator,
	iterator,
	synth_e_access_traits,
	Allocator>
      const_node_iterator;

      typedef const_node_iterator node_iterator;

      typedef
      Node_Update<
	const_node_iterator,
	node_iterator,
	E_Access_Traits,
	Allocator>
      node_update;

      typedef
      __gnu_pbds::null_trie_node_update<
	const_node_iterator,
	const_node_iterator,
	E_Access_Traits,
	Allocator>* 
      null_node_update_pointer;
    };

  } // namespace detail
} // namespace __gnu_pbds

#endif // #ifndef PB_DS_PAT_TRIE_NODE_AND_IT_TRAITS_HPP

