// -*- C++ -*-

// Copyright (C) 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
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
 * @file split_join_branch_bag.hpp
 * Contains an implementation class for pat_trie_.
 */

class split_join_branch_bag
{
private:
  typedef
  std::list<
  internal_node_pointer,
  typename Allocator::template rebind<
  internal_node_pointer>::other>
  bag_t;

public:

  void
  add_branch()
  {
    internal_node_pointer p_nd = s_internal_node_allocator.allocate(1);
    __try
      {
	m_bag.push_back(p_nd);
      }
    __catch(...)
      {
	s_internal_node_allocator.deallocate(p_nd, 1);
	__throw_exception_again;
      }
  }

  internal_node_pointer
  get_branch()
  {
    _GLIBCXX_DEBUG_ASSERT(!m_bag.empty());
    internal_node_pointer p_nd =* m_bag.begin();
    m_bag.pop_front();
    return p_nd;
  }

  ~split_join_branch_bag()
  {
    while (!m_bag.empty())
      {
	internal_node_pointer p_nd =* m_bag.begin();
	s_internal_node_allocator.deallocate(p_nd, 1);
	m_bag.pop_front();
      }
  }

  inline bool
  empty() const
  { return m_bag.empty(); }

private:
  bag_t m_bag;
};
