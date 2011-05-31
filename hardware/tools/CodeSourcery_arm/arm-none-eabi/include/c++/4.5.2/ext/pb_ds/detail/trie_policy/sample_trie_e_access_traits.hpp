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
 * @file sample_trie_e_access_traits.hpp
 * Contains a sample probe policy.
 */

#ifndef PB_DS_SAMPLE_TRIE_E_ACCESS_TRAITS_HPP
#define PB_DS_SAMPLE_TRIE_E_ACCESS_TRAITS_HPP

// A sample trie element-access traits.
class sample_trie_e_access_traits
{

public:

  // Size type.
  typedef size_t size_type;

  // Key type.
  typedef std::string key_type;

  // Const key reference type.
  typedef
  typename Allocator::template rebind<
    key_type>::other::const_reference
  const_key_reference;

  // Element const iterator type.
  typedef std::string::const_iterator const_iterator;

  // Element type.
  typedef char e_type;

  enum
    {
      max_size = 4
    };

public:

  // Returns a const_iterator to the first element of r_key.
  inline static const_iterator
  begin(const_key_reference r_key);

  // Returns a const_iterator to the after-last element of r_key.
  inline static const_iterator
  end(const_key_reference r_key);

  // Maps an element to a position.
  inline static size_type
  e_pos(e_type e);

};

#endif // #ifndef PB_DS_SAMPLE_TRIE_E_ACCESS_TRAITS_HPP
