// -*- C++ -*-

// Copyright (C) 2005, 2006, 2007, 2009 Free Software Foundation, Inc.
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
 * @file list_update_policy.hpp
 * Contains policies for list update containers.
 */

#ifndef PB_DS_LU_POLICY_HPP
#define PB_DS_LU_POLICY_HPP

#include <cstdlib>
#include <ext/pb_ds/detail/list_update_policy/counter_lu_metadata.hpp>

namespace __gnu_pbds
{
  // A null type that means that each link in a list-based container
  // does not actually need metadata.
  struct null_lu_metadata
  { };

#define PB_DS_CLASS_T_DEC template<typename Allocator>
#define PB_DS_CLASS_C_DEC move_to_front_lu_policy<Allocator>

  // A list-update policy that unconditionally moves elements to the
  // front of the list.
  template<typename Allocator = std::allocator<char> >
  class move_to_front_lu_policy
  {
  public:
    typedef Allocator allocator_type;
      
    // Metadata on which this functor operates.
    typedef null_lu_metadata metadata_type;
      
    // Reference to metadata on which this functor operates.
    typedef typename allocator_type::template rebind<metadata_type>::other metadata_rebind;
    typedef typename metadata_rebind::reference metadata_reference;
      
    // Creates a metadata object.
    metadata_type
    operator()() const;
      
    // Decides whether a metadata object should be moved to the front
    // of the list.
    inline bool
    operator()(metadata_reference r_metadata) const;
      
  private:
    static null_lu_metadata s_metadata;
  };
  
#include <ext/pb_ds/detail/list_update_policy/mtf_lu_policy_imp.hpp>

#undef PB_DS_CLASS_T_DEC
#undef PB_DS_CLASS_C_DEC

#define PB_DS_CLASS_T_DEC template<size_t Max_Count, class Allocator>
#define PB_DS_CLASS_C_DEC counter_lu_policy<Max_Count, Allocator>

  // A list-update policy that moves elements to the front of the list
  // based on the counter algorithm.
  template<size_t Max_Count = 5, typename Allocator = std::allocator<char> >
  class counter_lu_policy 
  : private detail::counter_lu_policy_base<typename Allocator::size_type>
  {
  public:
    typedef Allocator allocator_type;

    enum
      {
	max_count = Max_Count
      };

    typedef typename allocator_type::size_type size_type;

    // Metadata on which this functor operates.
    typedef detail::counter_lu_metadata<size_type> metadata_type;

    // Reference to metadata on which this functor operates.
    typedef typename Allocator::template rebind<metadata_type>::other metadata_rebind;
    typedef typename metadata_rebind::reference metadata_reference;

    // Creates a metadata object.
    metadata_type
    operator()() const;

    // Decides whether a metadata object should be moved to the front
    // of the list.
    bool
    operator()(metadata_reference r_metadata) const;

  private:
    typedef detail::counter_lu_policy_base<typename Allocator::size_type> base_type;
  };

#include <ext/pb_ds/detail/list_update_policy/counter_lu_policy_imp.hpp>

#undef PB_DS_CLASS_T_DEC
#undef PB_DS_CLASS_C_DEC

} // namespace __gnu_pbds

#endif
