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
 * @file basic_tree_policy_base.hpp
 * Contains a base class for tree_like policies.
 */

#ifndef PB_DS_TREE_LIKE_POLICY_BASE_HPP
#define PB_DS_TREE_LIKE_POLICY_BASE_HPP

namespace __gnu_pbds
{
  namespace detail
  {

#define PB_DS_CLASS_C_DEC						\
    basic_tree_policy_base<						\
							Const_Node_Iterator, \
							Node_Iterator,	\
							Allocator>

    template<typename Const_Node_Iterator,
	     typename Node_Iterator,
	     typename Allocator>
    struct basic_tree_policy_base
    {
    protected:
      typedef typename Node_Iterator::value_type it_type;

      typedef typename std::iterator_traits< it_type>::value_type value_type;

      typedef typename value_type::first_type key_type;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	key_type>::type>::other::const_reference
      const_key_reference;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	value_type>::type>::other::const_reference
      const_reference;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	value_type>::type>::other::reference
      reference;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	value_type>::type>::other::const_pointer
      const_pointer;

      static inline const_key_reference
      extract_key(const_reference r_val)
      {
	return (r_val.first);
      }

      virtual it_type
      end() = 0;

      it_type
      end_iterator() const
      {
	return (const_cast<PB_DS_CLASS_C_DEC* >(this)->end());
      }

      virtual
      ~basic_tree_policy_base()
      { }
    };

    template<typename Const_Node_Iterator, typename Allocator>
    struct basic_tree_policy_base<
      Const_Node_Iterator,
      Const_Node_Iterator,
      Allocator>
    {
    protected:
      typedef typename Const_Node_Iterator::value_type it_type;

      typedef typename std::iterator_traits< it_type>::value_type value_type;

      typedef value_type key_type;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	key_type>::type>::other::const_reference
      const_key_reference;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	value_type>::type>::other::const_reference
      const_reference;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	value_type>::type>::other::reference
      reference;

      typedef
      typename Allocator::template rebind<
	typename remove_const<
	value_type>::type>::other::const_pointer
      const_pointer;

      static inline const_key_reference
      extract_key(const_reference r_val)
      {
	return (r_val);
      }

      virtual it_type
      end() const = 0;

      it_type
      end_iterator() const
      {
	return (end());
      }

      virtual
      ~basic_tree_policy_base()
      { }
    };

#undef PB_DS_CLASS_C_DEC

  } // namespace detail
} // namespace __gnu_pbds

#endif // #ifndef PB_DS_TREE_LIKE_POLICY_BASE_HPP
