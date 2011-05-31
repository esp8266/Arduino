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
 * @file insert_fn_imps.hpp
 * Contains an implementation for thin_heap_.
 */

PB_DS_CLASS_T_DEC
inline typename PB_DS_CLASS_C_DEC::point_iterator
PB_DS_CLASS_C_DEC::
push(const_reference r_val)
{
  _GLIBCXX_DEBUG_ONLY(assert_valid();)

    node_pointer p_nd = base_type::get_new_node_for_insert(r_val);

  p_nd->m_metadata = 0;

  p_nd->m_p_prev_or_parent = p_nd->m_p_l_child = NULL;

  if (base_type::m_p_root == NULL)
    {
      p_nd->m_p_next_sibling = NULL;

      m_p_max = base_type::m_p_root = p_nd;

      _GLIBCXX_DEBUG_ONLY(assert_valid();)

        return point_iterator(p_nd);
    }

  p_nd->m_p_next_sibling = base_type::m_p_root;

  base_type::m_p_root->m_p_prev_or_parent = NULL;

  base_type::m_p_root = p_nd;

  update_max(p_nd);

  _GLIBCXX_DEBUG_ONLY(assert_valid();)

    return point_iterator(p_nd);
}

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
make_root(node_pointer p_nd)
{
  p_nd->m_metadata =
    p_nd->m_p_l_child == NULL?
    0 :
    1 + p_nd->m_p_l_child->m_metadata;
}

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
make_root_and_link(node_pointer p_nd)
{
  make_root(p_nd);

  p_nd->m_p_prev_or_parent = NULL;

  p_nd->m_p_next_sibling = base_type::m_p_root;

  if (base_type::m_p_root != NULL)
    base_type::m_p_root->m_p_prev_or_parent = NULL;

  base_type::m_p_root = p_nd;

  update_max(p_nd);
}

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
fix(node_pointer p_y)
{
  while (true)
    {
      if (p_y->m_p_prev_or_parent == NULL)
        {
	  fix_root(p_y);

	  return;
        }
      else if (p_y->m_metadata == 1&&  p_y->m_p_next_sibling == NULL)
        {
	  if (p_y->m_p_l_child != NULL)
            {
	      fix_sibling_rank_1_unmarked(p_y);

	      return;
            }

	  fix_sibling_rank_1_marked(p_y);

	  p_y = p_y->m_p_prev_or_parent;
        }
      else if (p_y->m_metadata > p_y->m_p_next_sibling->m_metadata + 1)
        {
	  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_l_child != NULL);

	  if (p_y->m_metadata != p_y->m_p_l_child->m_metadata + 2)
            {
	      fix_sibling_general_unmarked(p_y);

	      return;
            }

	  fix_sibling_general_marked(p_y);

	  p_y = p_y->m_p_prev_or_parent;
        }
      else if ((p_y->m_p_l_child == NULL&& 
                p_y->m_metadata == 2) ||(p_y->m_p_l_child != NULL&& 
					 p_y->m_metadata == p_y->m_p_l_child->m_metadata + 3))
        {
	  node_pointer p_z = p_y->m_p_prev_or_parent;

	  fix_child(p_y);

	  p_y = p_z;
        }
      else
	return;
    }
}

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
fix_root(node_pointer p_y)
{
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_prev_or_parent == NULL);

  make_root(p_y);

  _GLIBCXX_DEBUG_ONLY(assert_node_consistent(p_y, true);)
    }

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
fix_sibling_rank_1_unmarked(node_pointer p_y)
{
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_prev_or_parent != NULL);

  _GLIBCXX_DEBUG_ONLY(node_pointer p_w = p_y->m_p_l_child;)
    _GLIBCXX_DEBUG_ASSERT(p_w != NULL);
  _GLIBCXX_DEBUG_ASSERT(p_w->m_p_next_sibling == NULL);
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_next_sibling == NULL);

  p_y->m_p_next_sibling = p_y->m_p_l_child;

  p_y->m_p_next_sibling->m_p_prev_or_parent = p_y;

  p_y->m_p_l_child = NULL;

  _GLIBCXX_DEBUG_ONLY(assert_node_consistent(p_y, false);)
    }

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
fix_sibling_rank_1_marked(node_pointer p_y)
{
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_prev_or_parent != NULL);
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_l_child == NULL);

  p_y->m_metadata = 0;

  _GLIBCXX_DEBUG_ONLY(assert_node_consistent(p_y, false);)
    }

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
fix_sibling_general_unmarked(node_pointer p_y)
{
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_prev_or_parent != NULL);

  node_pointer p_w = p_y->m_p_l_child;
  _GLIBCXX_DEBUG_ASSERT(p_w != NULL);
  _GLIBCXX_DEBUG_ASSERT(p_w->m_p_next_sibling != NULL);

  p_y->m_p_l_child = p_w->m_p_next_sibling;
  p_w->m_p_next_sibling->m_p_prev_or_parent = p_y;

  p_w->m_p_next_sibling = p_y->m_p_next_sibling;
  _GLIBCXX_DEBUG_ASSERT(p_w->m_p_next_sibling != NULL);
  p_w->m_p_next_sibling->m_p_prev_or_parent = p_w;

  p_y->m_p_next_sibling = p_w;
  p_w->m_p_prev_or_parent = p_y;

  _GLIBCXX_DEBUG_ONLY(assert_node_consistent(p_y, false);)
    }

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
fix_sibling_general_marked(node_pointer p_y)
{
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_prev_or_parent != NULL);

  --p_y->m_metadata;

  _GLIBCXX_DEBUG_ONLY(assert_node_consistent(p_y, false);)
    }

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
fix_child(node_pointer p_y)
{
  _GLIBCXX_DEBUG_ASSERT(p_y->m_p_prev_or_parent != NULL);

  if (p_y->m_p_next_sibling != NULL)
    p_y->m_p_next_sibling->m_p_prev_or_parent = p_y->m_p_prev_or_parent;

  if (p_y->m_p_prev_or_parent->m_p_l_child == p_y)
    p_y->m_p_prev_or_parent->m_p_l_child = p_y->m_p_next_sibling;
  else
    p_y->m_p_prev_or_parent->m_p_next_sibling = p_y->m_p_next_sibling;

  make_root_and_link(p_y);
}

PB_DS_CLASS_T_DEC
void
PB_DS_CLASS_C_DEC::
modify(point_iterator it, const_reference r_new_val)
{
  _GLIBCXX_DEBUG_ONLY(assert_valid();)
    node_pointer p_nd = it.m_p_nd;

  _GLIBCXX_DEBUG_ASSERT(p_nd != NULL);

  const bool smaller = Cmp_Fn::operator()(r_new_val, p_nd->m_value);

  p_nd->m_value = r_new_val;

  if (smaller)
    {
      remove_node(p_nd);

      p_nd->m_p_l_child = NULL;

      make_root_and_link(p_nd);

      _GLIBCXX_DEBUG_ONLY(assert_valid();)

        return;
    }

  if (p_nd->m_p_prev_or_parent == NULL)
    {
      update_max(p_nd);

      _GLIBCXX_DEBUG_ONLY(assert_valid();)

        return;
    }

  node_pointer p_y = p_nd->m_p_prev_or_parent;
  _GLIBCXX_DEBUG_ASSERT(p_y != NULL);

  if (p_nd->m_p_next_sibling != NULL)
    p_nd->m_p_next_sibling->m_p_prev_or_parent = p_y;

  if (p_y->m_p_l_child == p_nd)
    p_y->m_p_l_child = p_nd->m_p_next_sibling;
  else
    p_y->m_p_next_sibling = p_nd->m_p_next_sibling;

  fix(p_y);

  make_root_and_link(p_nd);

  _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

PB_DS_CLASS_T_DEC
inline void
PB_DS_CLASS_C_DEC::
update_max(node_pointer p_nd)
{
  if (m_p_max == NULL || Cmp_Fn::operator()(m_p_max->m_value, p_nd->m_value))
    m_p_max = p_nd;
}

