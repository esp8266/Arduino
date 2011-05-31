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
 * @file debug_fn_imps.hpp
 * Contains an implementation class for pat_trie_.
 */

#ifdef _GLIBCXX_DEBUG

PB_DS_CLASS_T_DEC
void
PB_DS_CLASS_C_DEC::
assert_valid() const
{
  if (m_p_head->m_p_parent != NULL)
    m_p_head->m_p_parent->assert_valid(this);
  assert_iterators();
  assert_reverse_iterators();
  if (m_p_head->m_p_parent == NULL)
    {
      _GLIBCXX_DEBUG_ASSERT(m_p_head->m_p_min == m_p_head);
      _GLIBCXX_DEBUG_ASSERT(m_p_head->m_p_max == m_p_head);
      _GLIBCXX_DEBUG_ASSERT(empty());
      return;
    }

  _GLIBCXX_DEBUG_ASSERT(m_p_head->m_p_min->m_type == pat_trie_leaf_node_type);
  _GLIBCXX_DEBUG_ASSERT(m_p_head->m_p_max->m_type == pat_trie_leaf_node_type);
  _GLIBCXX_DEBUG_ASSERT(!empty());
}

PB_DS_CLASS_T_DEC
void
PB_DS_CLASS_C_DEC::
assert_iterators() const
{
  size_type calc_size = 0;
  for (const_iterator it = begin(); it != end(); ++it)
    {
      ++calc_size;
      debug_base::check_key_exists(PB_DS_V2F(*it));
      _GLIBCXX_DEBUG_ASSERT(lower_bound(PB_DS_V2F(*it)) == it);
      _GLIBCXX_DEBUG_ASSERT(--upper_bound(PB_DS_V2F(*it)) == it);
    }
  _GLIBCXX_DEBUG_ASSERT(calc_size == m_size);
}

PB_DS_CLASS_T_DEC
void
PB_DS_CLASS_C_DEC::
assert_reverse_iterators() const
{
  size_type calc_size = 0;
  for (const_reverse_iterator it = rbegin(); it != rend(); ++it)
    {
      ++calc_size;
      const_node_pointer p_nd =
	const_cast<PB_DS_CLASS_C_DEC* >(this)->find_imp(PB_DS_V2F(*it));
      _GLIBCXX_DEBUG_ASSERT(p_nd == it.m_p_nd);
    }
  _GLIBCXX_DEBUG_ASSERT(calc_size == m_size);
}

PB_DS_CLASS_T_DEC
typename PB_DS_CLASS_C_DEC::size_type
PB_DS_CLASS_C_DEC::
recursive_count_leafs(const_node_pointer p_nd)
{
  if (p_nd == NULL)
    return (0);
  if (p_nd->m_type == pat_trie_leaf_node_type)
    return (1);
  _GLIBCXX_DEBUG_ASSERT(p_nd->m_type == pat_trie_internal_node_type);
  size_type ret = 0;
  for (typename internal_node::const_iterator it =
	 static_cast<const_internal_node_pointer>(p_nd)->begin();
       it != static_cast<const_internal_node_pointer>(p_nd)->end();
       ++it)
    ret += recursive_count_leafs(*it);
  return ret;
}

#endif 

