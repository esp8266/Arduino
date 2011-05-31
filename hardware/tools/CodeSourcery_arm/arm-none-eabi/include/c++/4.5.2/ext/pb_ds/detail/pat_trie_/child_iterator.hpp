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
 * @file child_iterator.hpp
 * Contains a iterator for a patricia tree.
 */

struct iterator : public const_iterator
{
public:
  typedef std::forward_iterator_tag iterator_category;
  typedef typename Allocator::difference_type difference_type;
  typedef node_pointer value_type;
  typedef node_pointer_pointer pointer;
  typedef node_pointer_reference reference;

  inline
  iterator(node_pointer_pointer p_p_cur = NULL,  
	   node_pointer_pointer p_p_end = NULL) 
  : const_iterator(p_p_cur, p_p_end)
  { }

  inline bool
  operator==(const iterator& other) const
  { return const_iterator::m_p_p_cur == other.m_p_p_cur; }

  inline bool
  operator!=(const iterator& other) const
  { return const_iterator::m_p_p_cur != other.m_p_p_cur; }

  inline iterator& 
  operator++()
  {
    const_iterator::operator++();
    return *this;
  }

  inline iterator
  operator++(int)
  {
    iterator ret_it(*this);
    operator++();
    return ret_it;
  }

  node_pointer_pointer
  operator->()
  {
    _GLIBCXX_DEBUG_ONLY(const_iterator::assert_referencible();)
    return const_iterator::m_p_p_cur;
  }

  node_pointer
  operator*()
  {
    _GLIBCXX_DEBUG_ONLY(const_iterator::assert_referencible();)
    return *const_iterator::m_p_p_cur;
  }
};

