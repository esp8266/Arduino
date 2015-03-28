// -*- C++ -*-

// Copyright (C) 2005-2013 Free Software Foundation, Inc.
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
 * @file detail/types_traits.hpp
 * Contains a traits class of types used by containers.
 */

#ifndef PB_DS_TYPES_TRAITS_HPP
#define PB_DS_TYPES_TRAITS_HPP

#include <algorithm>
#include <utility>
#include <ext/pb_ds/tag_and_trait.hpp>
#include <ext/pb_ds/detail/type_utils.hpp>
#include <utility>

namespace __gnu_pbds
{
  namespace detail
  {
    /**
     *  @addtogroup traits Traits
     *  @{
     */

    /// Primary template.
    template<typename Key, typename Mapped>
      struct no_throw_copies
      {
	static const bool __simple = is_simple<Key>::value
			   	 && is_simple<Mapped>::value;
	typedef integral_constant<int, __simple> 	       	indicator;
      };

    /// Specialization.
    template<typename Key>
      struct no_throw_copies<Key, null_type>
      {
	typedef integral_constant<int, is_simple<Key>::value> 	indicator;
      };


    /// Stored value.
    template<typename _Tv>
      struct stored_value
      {
	typedef _Tv 	value_type;
	value_type 	m_value;
      };

    /// Stored hash.
    template<typename _Th>
      struct stored_hash
      {
	typedef _Th 	hash_type;
	hash_type 	m_hash;
      };

    /// Primary template for representation of stored data.
    /// Two types of data can be stored: value and hash.
    template<typename _Tv, typename _Th>
      struct stored_data
      : public stored_value<_Tv>, public stored_hash<_Th>
      { };

    /// Specialization for representation of stored data of just value type.
    template<typename _Tv>
      struct stored_data<_Tv, null_type>
      : public stored_value<_Tv>
      { };

    /// Primary template.
    template<typename Key, typename Mapped, typename _Alloc, bool Store_Hash>
      struct type_base;

    /**
     * Specialization of type_base for the case where the hash value
     * is not stored alongside each value.
     */
    template<typename Key, typename Mapped, typename _Alloc>
      struct type_base<Key, Mapped, _Alloc, false>
      {
      public:
	typedef typename _Alloc::size_type 		size_type;

      private:
	typedef typename _Alloc::template rebind<Mapped>	__rebind_m;
	typedef typename __rebind_m::other			__rebind_ma;
	typedef std::pair<const Key, Mapped>			__value_type;
	typedef typename _Alloc::template rebind<__value_type>	__rebind_v;
       	typedef typename __rebind_v::other			__rebind_va;

      public:
	typedef typename __rebind_ma::value_type       	mapped_type;
	typedef typename __rebind_ma::pointer 		mapped_pointer;
	typedef typename __rebind_ma::const_pointer 	mapped_const_pointer;
	typedef typename __rebind_ma::reference	       	mapped_reference;
	typedef typename __rebind_ma::const_reference 	mapped_const_reference;

	typedef typename __rebind_va::value_type 	value_type;
	typedef typename __rebind_va::pointer 		pointer;
	typedef typename __rebind_va::const_pointer 	const_pointer;
	typedef typename __rebind_va::reference 	reference;
	typedef typename __rebind_va::const_reference 	const_reference;

	typedef stored_data<value_type, null_type>	stored_data_type;
      };

    /**
     * Specialization of type_base for the case where the hash value
     * is stored alongside each value.
     */
    template<typename Key, typename Mapped, typename _Alloc>
      struct type_base<Key, Mapped, _Alloc, true>
      {
      public:
	typedef typename _Alloc::size_type 		size_type;

      private:
	typedef typename _Alloc::template rebind<Mapped>	__rebind_m;
	typedef typename __rebind_m::other			__rebind_ma;
	typedef std::pair<const Key, Mapped>			__value_type;
	typedef typename _Alloc::template rebind<__value_type>	__rebind_v;
       	typedef typename __rebind_v::other			__rebind_va;

      public:
	typedef typename __rebind_ma::value_type       	mapped_type;
	typedef typename __rebind_ma::pointer 		mapped_pointer;
	typedef typename __rebind_ma::const_pointer 	mapped_const_pointer;
	typedef typename __rebind_ma::reference	       	mapped_reference;
	typedef typename __rebind_ma::const_reference 	mapped_const_reference;

	typedef typename __rebind_va::value_type 	value_type;
	typedef typename __rebind_va::pointer 		pointer;
	typedef typename __rebind_va::const_pointer 	const_pointer;
	typedef typename __rebind_va::reference 	reference;
	typedef typename __rebind_va::const_reference 	const_reference;

	typedef stored_data<value_type, size_type>	stored_data_type;
      };


    /**
     * Specialization of type_base for the case where the hash value
     * is not stored alongside each value.
     */
    template<typename Key, typename _Alloc>
      struct type_base<Key, null_type, _Alloc, false>
      {
      public:
	typedef typename _Alloc::size_type 		size_type;
	typedef Key 					value_type;

      private:
	typedef typename _Alloc::template rebind<null_type>  	__rebind_m;
	typedef typename __rebind_m::other			__rebind_ma;
	typedef typename _Alloc::template rebind<value_type>	__rebind_v;
       	typedef typename __rebind_v::other			__rebind_va;

      public:
	typedef typename __rebind_ma::value_type       	mapped_type;
	typedef typename __rebind_ma::pointer 		mapped_pointer;
	typedef typename __rebind_ma::const_pointer 	mapped_const_pointer;
	typedef typename __rebind_ma::reference	       	mapped_reference;
	typedef typename __rebind_ma::const_reference 	mapped_const_reference;

	typedef typename __rebind_va::pointer 		pointer;
	typedef typename __rebind_va::const_pointer 	const_pointer;
	typedef typename __rebind_va::reference 	reference;
	typedef typename __rebind_va::const_reference 	const_reference;

	typedef stored_data<value_type, null_type>	stored_data_type;

	static null_type 			s_null_type;
      };

    template<typename Key, typename _Alloc>
      null_type
      type_base<Key, null_type, _Alloc, false>::s_null_type;


    /**
     * Specialization of type_base for the case where the hash value
     * is stored alongside each value.
     */
    template<typename Key, typename _Alloc>
      struct type_base<Key, null_type, _Alloc, true>
      {
      public:
	typedef typename _Alloc::size_type 		size_type;
	typedef Key 					value_type;

      private:
	typedef typename _Alloc::template rebind<null_type>  	__rebind_m;
	typedef typename __rebind_m::other			__rebind_ma;
	typedef typename _Alloc::template rebind<value_type>	__rebind_v;
       	typedef typename __rebind_v::other			__rebind_va;

      public:
	typedef typename __rebind_ma::value_type       	mapped_type;
	typedef typename __rebind_ma::pointer 		mapped_pointer;
	typedef typename __rebind_ma::const_pointer 	mapped_const_pointer;
	typedef typename __rebind_ma::reference	       	mapped_reference;
	typedef typename __rebind_ma::const_reference 	mapped_const_reference;

	typedef typename __rebind_va::pointer 		pointer;
	typedef typename __rebind_va::const_pointer 	const_pointer;
	typedef typename __rebind_va::reference 	reference;
	typedef typename __rebind_va::const_reference 	const_reference;

	typedef stored_data<value_type, size_type>	stored_data_type;

	static null_type 		      	s_null_type;
      };

    template<typename Key, typename _Alloc>
      null_type
      type_base<Key, null_type, _Alloc, true>::s_null_type;


    /// Type base dispatch.
    template<typename Key, typename Mapped, typename _Alloc, bool Store_Hash>
      struct type_dispatch
      {
	typedef type_base<Key, Mapped, _Alloc, Store_Hash> type;
      };

    /// Traits for abstract types.
    template<typename Key, typename Mapped, typename _Alloc, bool Store_Hash>
      struct types_traits
      : public type_dispatch<Key, Mapped, _Alloc, Store_Hash>::type
      {
      private:
	typedef no_throw_copies<Key, Mapped>		__nothrowcopy;
	typedef typename _Alloc::template rebind<Key>::other __rebind_a;

      public:
	typedef typename _Alloc::size_type 		size_type;
	typedef typename __rebind_a::value_type 	key_type;
	typedef typename __rebind_a::pointer 		key_pointer;
	typedef typename __rebind_a::const_pointer 	key_const_pointer;
	typedef typename __rebind_a::reference 		key_reference;
	typedef typename __rebind_a::const_reference 	key_const_reference;
	typedef std::pair<size_type, size_type> 	comp_hash;
	typedef integral_constant<int, Store_Hash> 	store_extra;
	typedef typename __nothrowcopy::indicator 	no_throw_indicator;

	store_extra			       	m_store_extra_indicator;
	no_throw_indicator 		 	m_no_throw_copies_indicator;
    };
    //@}
  } // namespace detail
} // namespace __gnu_pbds

#endif
