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
 * Contains implementations of cc_ht_map_'s debug-mode functions.
 */

#ifdef _GLIBCXX_DEBUG

PB_DS_CLASS_T_DEC
void
PB_DS_CLASS_C_DEC::
assert_valid() const
{
  debug_base::check_size(m_num_used_e);
  assert_entry_pointer_array_valid(m_entries);
}

PB_DS_CLASS_T_DEC
void
PB_DS_CLASS_C_DEC::
assert_entry_pointer_array_valid(const entry_pointer_array a_p_entries) const
{
  size_type iterated_num_used_e = 0;
  for (size_type pos = 0; pos < m_num_e; ++pos)
    {
      entry_pointer p_e = a_p_entries[pos];
      while (p_e != NULL)
        {
	  ++iterated_num_used_e;
	  assert_entry_pointer_valid(p_e, traits_base::m_store_extra_indicator);
	  p_e = p_e->m_p_next;
        }
    }
  _GLIBCXX_DEBUG_ASSERT(iterated_num_used_e == m_num_used_e);
}

#include <ext/pb_ds/detail/cc_hash_table_map_/debug_store_hash_fn_imps.hpp>
#include <ext/pb_ds/detail/cc_hash_table_map_/debug_no_store_hash_fn_imps.hpp>

#endif 
