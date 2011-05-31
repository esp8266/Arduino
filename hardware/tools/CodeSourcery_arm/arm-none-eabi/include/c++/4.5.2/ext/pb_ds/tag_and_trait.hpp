// -*- C++ -*-

// Copyright (C) 2005, 2006, 2008, 2009 Free Software Foundation, Inc.
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
 * @file tag_and_trait.hpp
 * Contains tags and traits, e.g., ones describing underlying
 *    data structures.
 */

#ifndef PB_DS_TAG_AND_TRAIT_HPP
#define PB_DS_TAG_AND_TRAIT_HPP

#include <ext/pb_ds/detail/type_utils.hpp>

/**
 * @namespace __gnu_pbds
 * @brief GNU extensions for policy-based data structures for public use.
 */
namespace __gnu_pbds
{
  // A trivial iterator tag. Signifies that the iterators has none of
  // the STL's movement abilities.
  struct trivial_iterator_tag
  { };

  // Prohibit moving trivial iterators.
  typedef void trivial_iterator_difference_type;


  // Signifies a basic invalidation guarantee that any iterator,
  // pointer, or reference to a container object's mapped value type
  // is valid as long as the container is not modified.
  struct basic_invalidation_guarantee
  { };

  // Signifies an invalidation guarantee that includes all those of
  // its base, and additionally, that any point-type iterator,
  // pointer, or reference to a container object's mapped value type
  // is valid as long as its corresponding entry has not be erased,
  // regardless of modifications to the container object.
  struct point_invalidation_guarantee : public basic_invalidation_guarantee
  { };

  // Signifies an invalidation guarantee that includes all those of
  // its base, and additionally, that any range-type iterator
  // (including the returns of begin() and end()) is in the correct
  // relative positions to other range-type iterators as long as its
  // corresponding entry has not be erased, regardless of
  // modifications to the container object.
  struct range_invalidation_guarantee : public point_invalidation_guarantee
  { };


  /// A mapped-policy indicating that an associative container is a set.
  // XXX should this be a trait of the form is_set<T> ??
  struct null_mapped_type { };


  /// Base data structure tag.
  struct container_tag
  { };

  /// Basic string container, inclusive of strings, ropes, etc.
  struct string_tag : public container_tag { };

  /// Basic sequence.
  struct sequence_tag : public container_tag { };

  /// Basic associative-container.
  struct associative_container_tag : public container_tag { };

  /// Basic hash.
  struct basic_hash_tag : public associative_container_tag { };

  /// Collision-chaining hash.
  struct cc_hash_tag : public basic_hash_tag { };

  /// General-probing hash.
  struct gp_hash_tag : public basic_hash_tag { };

  /// Basic tree.
  struct basic_tree_tag : public associative_container_tag { };

  /// tree.
  struct tree_tag : public basic_tree_tag { };

  /// Red-black tree.
  struct rb_tree_tag : public tree_tag { };

  /// Splay tree.
  struct splay_tree_tag : public tree_tag { };

  /// Ordered-vector tree.
  struct ov_tree_tag : public tree_tag { };

  /// trie.
  struct trie_tag : public basic_tree_tag { };

  /// PATRICIA trie.
  struct pat_trie_tag : public trie_tag { };

  /// List-update.
  struct list_update_tag : public associative_container_tag { };

  /// Basic priority-queue.
  struct priority_queue_tag : public container_tag { };

  /// Pairing-heap.
  struct pairing_heap_tag : public priority_queue_tag { };

  /// Binomial-heap.
  struct binomial_heap_tag : public priority_queue_tag { };

  /// Redundant-counter binomial-heap.
  struct rc_binomial_heap_tag : public priority_queue_tag { };

  /// Binary-heap (array-based).
  struct binary_heap_tag : public priority_queue_tag { };

  /// Thin heap.
  struct thin_heap_tag : public priority_queue_tag { };


  /// Base traits type for containers.
  template<typename Tag>
  struct container_traits_base;

  template<>
  struct container_traits_base<cc_hash_tag>
  {
    typedef cc_hash_tag container_category;
    typedef point_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
        erase_can_throw = false,
	split_join_can_throw = false,
	reverse_iteration = false
      };
  };

  template<>
  struct container_traits_base<gp_hash_tag>
  {
    typedef gp_hash_tag container_category;
    typedef basic_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
	erase_can_throw = false,
	split_join_can_throw = false,
	reverse_iteration = false
      };
  };

  template<>
  struct container_traits_base<rb_tree_tag>
  {
    typedef rb_tree_tag container_category;
    typedef range_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = true,
        erase_can_throw = false,
	split_join_can_throw = false,
        reverse_iteration = true
      };
  };

  template<>
  struct container_traits_base<splay_tree_tag>
  {
    typedef splay_tree_tag container_category;
    typedef range_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = true,
        erase_can_throw = false,
        split_join_can_throw = false,
        reverse_iteration = true
      };
  };

  template<>
  struct container_traits_base<ov_tree_tag>
  {
    typedef ov_tree_tag container_category;
    typedef basic_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = true,
        erase_can_throw = true,
        split_join_can_throw = true,
        reverse_iteration = false
      };
  };

  template<>
  struct container_traits_base<pat_trie_tag>
  {
    typedef pat_trie_tag container_category;
    typedef range_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = true,
        erase_can_throw = false,
        split_join_can_throw = true,
        reverse_iteration = true
      };
  };

  template<>
  struct container_traits_base<list_update_tag>
  {
    typedef list_update_tag container_category;
    typedef point_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
        erase_can_throw = false,
	split_join_can_throw = false,
        reverse_iteration = false
      };
  };


  template<>
  struct container_traits_base<pairing_heap_tag>
  {
    typedef pairing_heap_tag container_category;
    typedef point_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
        erase_can_throw = false,
	split_join_can_throw = false,
        reverse_iteration = false
      };
  };

  template<>
  struct container_traits_base<thin_heap_tag>
  {
    typedef thin_heap_tag container_category;
    typedef point_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
        erase_can_throw = false,
	split_join_can_throw = false,
        reverse_iteration = false
      };
  };

  template<>
  struct container_traits_base<binomial_heap_tag>
  {
    typedef binomial_heap_tag container_category;
    typedef point_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
        erase_can_throw = false,
	split_join_can_throw = false,
        reverse_iteration = false
      };
  };

  template<>
  struct container_traits_base<rc_binomial_heap_tag>
  {
    typedef rc_binomial_heap_tag container_category;
    typedef point_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
        erase_can_throw = false,
	split_join_can_throw = false,
        reverse_iteration = false
      };
  };

  template<>
  struct container_traits_base<binary_heap_tag>
  {
    typedef binary_heap_tag container_category;
    typedef basic_invalidation_guarantee invalidation_guarantee;

    enum
      {
        order_preserving = false,
        erase_can_throw = false,
	split_join_can_throw = true,
        reverse_iteration = false
      };
  };


  /// container_traits
  // See Matt Austern for the name, S. Meyers MEFC++ #2, others.
  template<typename Cntnr>
  struct container_traits 
  : public container_traits_base<typename Cntnr::container_category>
  {
    typedef Cntnr container_type;
    typedef typename Cntnr::container_category container_category;
    typedef container_traits_base<container_category> base_type;
    typedef typename base_type::invalidation_guarantee invalidation_guarantee;

    enum
      {
	order_preserving = base_type::order_preserving,
	erase_can_throw = base_type::erase_can_throw,
	split_join_can_throw = base_type::split_join_can_throw,
	reverse_iteration = base_type::reverse_iteration
      };
  };
} // namespace __gnu_pbds

#endif
