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
 * @file container_base_dispatch.hpp
 * Contains an associative container dispatching base.
 */

#ifndef PB_DS_ASSOC_CNTNR_BASE_DS_DISPATCHER_HPP
#define PB_DS_ASSOC_CNTNR_BASE_DS_DISPATCHER_HPP

#include <ext/typelist.h>

#define PB_DS_DATA_TRUE_INDICATOR
#include <ext/pb_ds/detail/list_update_map_/lu_map_.hpp>
#undef PB_DS_DATA_TRUE_INDICATOR

#define PB_DS_DATA_FALSE_INDICATOR
#include <ext/pb_ds/detail/list_update_map_/lu_map_.hpp>
#undef PB_DS_DATA_FALSE_INDICATOR

#define PB_DS_DATA_TRUE_INDICATOR
#include <ext/pb_ds/detail/rb_tree_map_/rb_tree_.hpp>
#undef PB_DS_DATA_TRUE_INDICATOR

#define PB_DS_DATA_FALSE_INDICATOR
#include <ext/pb_ds/detail/rb_tree_map_/rb_tree_.hpp>
#undef PB_DS_DATA_FALSE_INDICATOR

#define PB_DS_DATA_TRUE_INDICATOR
#include <ext/pb_ds/detail/splay_tree_/splay_tree_.hpp>
#undef PB_DS_DATA_TRUE_INDICATOR

#define PB_DS_DATA_FALSE_INDICATOR
#include <ext/pb_ds/detail/splay_tree_/splay_tree_.hpp>
#undef PB_DS_DATA_FALSE_INDICATOR

#define PB_DS_DATA_TRUE_INDICATOR
#include <ext/pb_ds/detail/ov_tree_map_/ov_tree_map_.hpp>
#undef PB_DS_DATA_TRUE_INDICATOR

#define PB_DS_DATA_FALSE_INDICATOR
#include <ext/pb_ds/detail/ov_tree_map_/ov_tree_map_.hpp>
#undef PB_DS_DATA_FALSE_INDICATOR

#define PB_DS_DATA_TRUE_INDICATOR
#include <ext/pb_ds/detail/cc_hash_table_map_/cc_ht_map_.hpp>
#undef PB_DS_DATA_TRUE_INDICATOR

#define PB_DS_DATA_FALSE_INDICATOR
#include <ext/pb_ds/detail/cc_hash_table_map_/cc_ht_map_.hpp>
#undef PB_DS_DATA_FALSE_INDICATOR

#define PB_DS_DATA_TRUE_INDICATOR
#include <ext/pb_ds/detail/gp_hash_table_map_/gp_ht_map_.hpp>
#undef PB_DS_DATA_TRUE_INDICATOR

#define PB_DS_DATA_FALSE_INDICATOR
#include <ext/pb_ds/detail/gp_hash_table_map_/gp_ht_map_.hpp>
#undef PB_DS_DATA_FALSE_INDICATOR

#define PB_DS_DATA_TRUE_INDICATOR
#include <ext/pb_ds/detail/pat_trie_/pat_trie_.hpp>
#undef PB_DS_DATA_TRUE_INDICATOR

#define PB_DS_DATA_FALSE_INDICATOR
#include <ext/pb_ds/detail/pat_trie_/pat_trie_.hpp>
#undef PB_DS_DATA_FALSE_INDICATOR

namespace __gnu_pbds
{
namespace detail
{
  // Primary template.
  template<typename Key, typename Mapped, typename Data_Structure_Taq,
	   typename Policy_Tl, typename Alloc>
    struct container_base_dispatch;

  template<typename Key, typename Mapped, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, Mapped, list_update_tag, 
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;
      
    public:
      typedef lu_map_data_<Key, Mapped, at0t, Alloc, at1t>	type;
    };

  template<typename Key, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, null_mapped_type, list_update_tag,
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef lu_map_no_data_<Key, null_mapped_type, at0t, Alloc, at1t> type;
    };

  template<typename Key, typename Mapped, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, Mapped, pat_trie_tag, Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef pat_trie_data_<Key, Mapped, at1t, Alloc> 		type;
    };

  template<typename Key, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, null_mapped_type, pat_trie_tag,
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef pat_trie_no_data_<Key, null_mapped_type, at1t, Alloc> type;
    };

  template<typename Key, typename Mapped, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, Mapped, rb_tree_tag, Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef rb_tree_data_<Key, Mapped, at0t, at1t, Alloc> 	type;
    };

  template<typename Key, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, null_mapped_type, rb_tree_tag,
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef rb_tree_no_data_<Key, null_mapped_type, at0t, at1t, Alloc> type;
    };

  template<typename Key, typename Mapped, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, Mapped, splay_tree_tag, 
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef splay_tree_data_<Key, Mapped, at0t, at1t, Alloc> 	type;
    };

  template<typename Key, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, null_mapped_type, splay_tree_tag,
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef splay_tree_no_data_<Key, null_mapped_type, at0t, at1t, Alloc> type;
  };

  template<typename Key, typename Mapped, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, Mapped, ov_tree_tag, Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef ov_tree_data_<Key, Mapped, at0t, at1t, Alloc> 	type;
  };

  template<typename Key, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, null_mapped_type, ov_tree_tag,
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;

    public:
      typedef ov_tree_no_data_<Key, null_mapped_type, at0t, at1t, Alloc> type;
  };

  template<typename Key, typename Mapped, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, Mapped, cc_hash_tag, Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 2>	at2;
      typedef typename at2::type			    	at2t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 3>	at3;
      typedef typename at3::type				at3t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 4> 	at4;
      typedef typename at4::type			    	at4t;

    public:
      typedef cc_ht_map_data_<Key, Mapped, at0t, at1t, Alloc, at3t::value, 
			      at4t, at2t> 			type;
  };

  template<typename Key, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, null_mapped_type, cc_hash_tag, 
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 2>	at2;
      typedef typename at2::type			    	at2t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 3>	at3;
      typedef typename at3::type				at3t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 4> 	at4;
      typedef typename at4::type			    	at4t;

    public:
      typedef cc_ht_map_no_data_<Key, null_mapped_type, at0t, at1t, Alloc, 
				 at3t::value, at4t, at2t>    	type;
  };

  template<typename Key, typename Mapped, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, Mapped, gp_hash_tag, Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 2>	at2;
      typedef typename at2::type			    	at2t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 3>	at3;
      typedef typename at3::type				at3t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 4> 	at4;
      typedef typename at4::type			    	at4t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 5> 	at5;
      typedef typename at5::type			    	at5t;

    public:
      typedef gp_ht_map_data_<Key, Mapped, at0t, at1t, Alloc, at3t::value, 
			      at4t, at5t, at2t> 		type;
  };

  template<typename Key, typename Policy_Tl, typename Alloc>
    struct container_base_dispatch<Key, null_mapped_type, gp_hash_tag,
				   Policy_Tl, Alloc>
    {
    private:
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 0>	at0;
      typedef typename at0::type			    	at0t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 1> 	at1;
      typedef typename at1::type			    	at1t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 2>	at2;
      typedef typename at2::type			    	at2t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 3>	at3;
      typedef typename at3::type				at3t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 4> 	at4;
      typedef typename at4::type			    	at4t;
      typedef __gnu_cxx::typelist::at_index<Policy_Tl, 5> 	at5;
      typedef typename at5::type			    	at5t;

    public:
      typedef gp_ht_map_no_data_<Key, null_mapped_type, at0t, at1t, Alloc,
				 at3t::value, at4t, at5t, at2t>	type;
  };
} // namespace detail
} // namespace __gnu_pbds

#endif 
