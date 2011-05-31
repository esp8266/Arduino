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
 * @file cond_dtor_entry_dealtor.hpp
 * Contains a binary tree container conditional deallocator
 */

class cond_dealtor
{
public:
  inline
  cond_dealtor(leaf_pointer p_nd) : m_p_nd(p_nd),
				    m_no_action_dtor(false),
				    m_call_destructor(false)
  { }

  inline void
  set_no_action_dtor()
  {
    m_no_action_dtor = true;
  }

  inline void
  set_call_destructor()
  {
    m_call_destructor = true;
  }

  inline
  ~cond_dealtor()
  {
    if (m_no_action_dtor)
      return;

    if (m_call_destructor)
      m_p_nd->~leaf();

    s_leaf_allocator.deallocate(m_p_nd, 1);
  }

protected:
  leaf_pointer m_p_nd;
  bool m_no_action_dtor;
  bool m_call_destructor;
};

