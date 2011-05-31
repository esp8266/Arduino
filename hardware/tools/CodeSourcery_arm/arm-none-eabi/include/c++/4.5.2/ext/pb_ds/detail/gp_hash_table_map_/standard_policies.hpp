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
 * @file standard_policies.hpp
 * Contains standard policies for gp_ht_map types.
 */

#ifndef PB_DS_GP_HT_MAP_STANDARD_POLICIES_HPP
#define PB_DS_GP_HT_MAP_STANDARD_POLICIES_HPP

#include <ext/pb_ds/detail/standard_policies.hpp>
#include <ext/pb_ds/ht_load_check_resize_trigger.hpp>
#include <ext/pb_ds/linear_probe_fn.hpp>
#include <ext/pb_ds/quadratic_probe_fn.hpp>

namespace __gnu_pbds
{
  namespace detail
  {
    template<typename Comb_Probe_Fn>
    struct default_probe_fn
    {
    private:
      typedef typename Comb_Probe_Fn::size_type size_type;

    public:
      typedef
      typename __conditional_type<
      is_same<
      __gnu_pbds::direct_mask_range_hashing<size_t>,
      Comb_Probe_Fn>::value,
      __gnu_pbds::linear_probe_fn<size_type>,
      __gnu_pbds::quadratic_probe_fn<size_type> >::__type
      type;
    };

  } // namespace detail
} // namespace __gnu_pbds

#endif 

