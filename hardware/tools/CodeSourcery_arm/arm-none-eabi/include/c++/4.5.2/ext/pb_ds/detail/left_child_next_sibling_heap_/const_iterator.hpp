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
 * @file const_iterator.hpp
 * Contains an iterator class returned by the table's const find and insert
 *     methods.
 */

#ifndef PB_DS_LEFT_CHILD_NEXT_SIBLING_HEAP_CONST_ITERATOR_HPP
#define PB_DS_LEFT_CHILD_NEXT_SIBLING_HEAP_CONST_ITERATOR_HPP

#include <ext/pb_ds/detail/left_child_next_sibling_heap_/const_point_iterator.hpp>
#include <debug/debug.h>

namespace __gnu_pbds
{
  namespace detail
  {

#define PB_DS_CLASS_C_DEC						\
    left_child_next_sibling_heap_const_iterator_<Node, Allocator>

#define PB_DS_BASE_C_DEC						\
    left_child_next_sibling_heap_node_const_point_iterator_<Node, Allocator>

    // Const point-type iterator.
    template<typename Node, class Allocator>
    class left_child_next_sibling_heap_const_iterator_ : public PB_DS_BASE_C_DEC
    {

    private:
      typedef typename PB_DS_BASE_C_DEC::node_pointer node_pointer;

      typedef PB_DS_BASE_C_DEC base_type;

    public:

      // Category.
      typedef std::forward_iterator_tag iterator_category;

      // Difference type.
      typedef typename Allocator::difference_type difference_type;

      // Iterator's value type.
      typedef typename base_type::value_type value_type;

      // Iterator's pointer type.
      typedef typename base_type::pointer pointer;

      // Iterator's const pointer type.
      typedef typename base_type::const_pointer const_pointer;

      // Iterator's reference type.
      typedef typename base_type::reference reference;

      // Iterator's const reference type.
      typedef typename base_type::const_reference const_reference;

    public:

      inline
      left_child_next_sibling_heap_const_iterator_(node_pointer p_nd) : base_type(p_nd)
      { }

      // Default constructor.
      inline
      left_child_next_sibling_heap_const_iterator_()
      { }

      // Copy constructor.
      inline
      left_child_next_sibling_heap_const_iterator_(const PB_DS_CLASS_C_DEC& other) : base_type(other)
      { }

      // Compares content to a different iterator object.
      inline bool
      operator==(const PB_DS_CLASS_C_DEC& other) const
      { return (base_type::m_p_nd == other.m_p_nd); }

      // Compares content (negatively) to a different iterator object.
      inline bool
      operator!=(const PB_DS_CLASS_C_DEC& other) const
      { return (base_type::m_p_nd != other.m_p_nd); }

      inline PB_DS_CLASS_C_DEC& 
      operator++()
      {
	_GLIBCXX_DEBUG_ASSERT(base_type::m_p_nd != NULL);
	inc();
	return (*this);
      }

      inline PB_DS_CLASS_C_DEC
      operator++(int)
      {
	PB_DS_CLASS_C_DEC ret_it(base_type::m_p_nd);
	operator++();
	return (ret_it);
      }

    private:
      void
      inc()
      {
	if (base_type::m_p_nd->m_p_next_sibling != NULL)
	  {
	    base_type::m_p_nd = base_type::m_p_nd->m_p_next_sibling;
	    while (base_type::m_p_nd->m_p_l_child != NULL)
	      base_type::m_p_nd = base_type::m_p_nd->m_p_l_child;
	    return;
	  }

	while (true)
	  {
	    node_pointer p_next = base_type::m_p_nd;
	    base_type::m_p_nd = base_type::m_p_nd->m_p_prev_or_parent;
	    if (base_type::m_p_nd == NULL || base_type::m_p_nd->m_p_l_child == p_next)
	      return;
	  }
      }
    };

#undef PB_DS_CLASS_C_DEC
#undef PB_DS_BASE_C_DEC

  } // namespace detail
} // namespace __gnu_pbds

#endif 
