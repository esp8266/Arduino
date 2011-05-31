// -*- C++ -*-

// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010
// Free Software Foundation, Inc.
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
 * @file debug_map_base.hpp
 * Contains a debug-mode base for all maps.
 */

#ifndef PB_DS_DEBUG_MAP_BASE_HPP
#define PB_DS_DEBUG_MAP_BASE_HPP

#ifdef _GLIBCXX_DEBUG

#include <list>
#include <utility>
#include <cstdlib>
#include <iostream>
#include <ext/throw_allocator.h>
#include <debug/debug.h>

namespace __gnu_pbds
{
  namespace detail
  {
    // Need std::pair ostream extractor.
    template<typename _CharT, typename _Traits, typename _Tp1, typename _Tp2>
    inline std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __out, 
	       const std::pair<_Tp1, _Tp2>& p)
    { return (__out << '(' << p.first << ',' << p.second << ')'); }

#define PB_DS_CLASS_T_DEC \
    template<typename Key, class Eq_Fn, typename Const_Key_Reference>

#define PB_DS_CLASS_C_DEC \
    debug_map_base<Key, Eq_Fn, Const_Key_Reference>

    template<typename Key, class Eq_Fn, typename Const_Key_Reference>
    class debug_map_base
    {
    private:
      typedef typename std::allocator< Key> key_allocator;

      typedef typename key_allocator::size_type size_type;

      typedef Const_Key_Reference const_key_reference;

    protected:
      debug_map_base();

      debug_map_base(const PB_DS_CLASS_C_DEC& other);

      ~debug_map_base();

      inline void
      insert_new(const_key_reference r_key);

      inline void
      erase_existing(const_key_reference r_key);

      void
      clear();

      inline void
      check_key_exists(const_key_reference r_key) const;

      inline void
      check_key_does_not_exist(const_key_reference r_key) const;

      inline void
      check_size(size_type size) const;

      void
      swap(PB_DS_CLASS_C_DEC& other);

      template<typename Cmp_Fn>
      void
      split(const_key_reference, Cmp_Fn, PB_DS_CLASS_C_DEC&);

      void
      join(PB_DS_CLASS_C_DEC& other);

    private:
      typedef std::list< Key> 			key_set;
      typedef typename key_set::iterator 	key_set_iterator;
      typedef typename key_set::const_iterator 	const_key_set_iterator;

#ifdef _GLIBCXX_DEBUG
      void
      assert_valid() const;
#endif 

      const_key_set_iterator
      find(const_key_reference r_key) const;

      key_set_iterator
      find(const_key_reference r_key);

      key_set 	m_key_set;
      Eq_Fn 	m_eq;
    };

    PB_DS_CLASS_T_DEC
    PB_DS_CLASS_C_DEC::
    debug_map_base()
    { _GLIBCXX_DEBUG_ONLY(assert_valid();) }

    PB_DS_CLASS_T_DEC
    PB_DS_CLASS_C_DEC::
    debug_map_base(const PB_DS_CLASS_C_DEC& other) : m_key_set(other.m_key_set)
    { _GLIBCXX_DEBUG_ONLY(assert_valid();) }

    PB_DS_CLASS_T_DEC
    PB_DS_CLASS_C_DEC::
    ~debug_map_base()
    { _GLIBCXX_DEBUG_ONLY(assert_valid();) }

    PB_DS_CLASS_T_DEC
    inline void
    PB_DS_CLASS_C_DEC::
    insert_new(const_key_reference r_key)
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      // XXX FIXME: Adapt for __gnu_cxx::throw_allocator_random.
      //__gnu_cxx::throw_allocator<char> alloc;
      // const double orig_throw_prob = alloc.get_probability();
      // alloc.set_probability(0);
      if (find(r_key) != m_key_set.end())
	{
	  std::cerr << "insert_new" << r_key << std::endl;
	  std::abort();
	}

      __try
	{
	  m_key_set.push_back(r_key);
	}
      __catch(...)
	{
	  std::cerr << "insert_new" << r_key << std::endl;
	  std::abort();
	}
      // alloc.set_probability(orig_throw_prob);
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

    PB_DS_CLASS_T_DEC
    inline void
    PB_DS_CLASS_C_DEC::
    erase_existing(const_key_reference r_key)
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      key_set_iterator it = find(r_key);
      if (it == m_key_set.end())
	{
	  std::cerr << "erase_existing" << r_key << std::endl;
	  std::abort();
	}
      m_key_set.erase(it);
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

    PB_DS_CLASS_T_DEC
    void
    PB_DS_CLASS_C_DEC::
    clear()
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      m_key_set.clear();
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

    PB_DS_CLASS_T_DEC
    inline void
    PB_DS_CLASS_C_DEC::
    check_key_exists(const_key_reference r_key) const
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      if (find(r_key) == m_key_set.end())
        {
          std::cerr << "check_key_exists" << r_key << std::endl;
          std::abort();
        }
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

    PB_DS_CLASS_T_DEC
    inline void
    PB_DS_CLASS_C_DEC::
    check_key_does_not_exist(const_key_reference r_key) const
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      if (find(r_key) != m_key_set.end())
        {
	  using std::cerr;
	  using std::endl;
	  cerr << "check_key_does_not_exist" << r_key << endl;
          std::abort();
        }
    }

    PB_DS_CLASS_T_DEC
    inline void
    PB_DS_CLASS_C_DEC::
    check_size(size_type size) const
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      const size_type key_set_size = m_key_set.size();
      if (size != key_set_size)
	{
	  std::cerr << "check_size " << size 
		    << " " << key_set_size << std::endl;
	  std::abort();
	}
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
     }

    PB_DS_CLASS_T_DEC
    void
    PB_DS_CLASS_C_DEC::
    swap(PB_DS_CLASS_C_DEC& other)
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      m_key_set.swap(other.m_key_set);
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
    }

    PB_DS_CLASS_T_DEC
    typename PB_DS_CLASS_C_DEC::const_key_set_iterator
    PB_DS_CLASS_C_DEC::
    find(const_key_reference r_key) const
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      typedef const_key_set_iterator iterator_type;
      for (iterator_type it = m_key_set.begin(); it != m_key_set.end(); ++it)
	if (m_eq(*it, r_key))
          return it;
      return m_key_set.end();
    }

    PB_DS_CLASS_T_DEC
    typename PB_DS_CLASS_C_DEC::key_set_iterator
    PB_DS_CLASS_C_DEC::
    find(const_key_reference r_key)
    {
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
      key_set_iterator it = m_key_set.begin();
      while (it != m_key_set.end())
	{
	  if (m_eq(*it, r_key))
            return it;
	  ++it;
	}
      return it;
      _GLIBCXX_DEBUG_ONLY(assert_valid();)
     }

#ifdef _GLIBCXX_DEBUG
    PB_DS_CLASS_T_DEC
    void
    PB_DS_CLASS_C_DEC::
    assert_valid() const
    {
      const_key_set_iterator prime_it = m_key_set.begin();
      while (prime_it != m_key_set.end())
	{
	  const_key_set_iterator sec_it = prime_it;
	  ++sec_it;
	  while (sec_it != m_key_set.end())
	    {
	      _GLIBCXX_DEBUG_ASSERT(!m_eq(*sec_it, *prime_it));
	      _GLIBCXX_DEBUG_ASSERT(!m_eq(*prime_it, *sec_it));
	      ++sec_it;
	    }
	  ++prime_it;
	}
    }
#endif 

    PB_DS_CLASS_T_DEC
    template<typename Cmp_Fn>
    void
    PB_DS_CLASS_C_DEC::
    split(const_key_reference r_key, Cmp_Fn cmp_fn, PB_DS_CLASS_C_DEC& other)
    {
      // XXX FIXME: Adapt for __gnu_cxx::throw_allocator_random.
      // __gnu_cxx::throw_allocator<char> alloc;
      // const double orig_throw_prob = alloc.get_probability();
      // alloc.set_probability(0);
      other.clear();
      key_set_iterator it = m_key_set.begin();
      while (it != m_key_set.end())
        if (cmp_fn(r_key, * it))
	  {
            other.insert_new(*it);
            it = m_key_set.erase(it);
	  }
        else
	  ++it;
      // alloc.set_probability(orig_throw_prob);
    }

    PB_DS_CLASS_T_DEC
    void
    PB_DS_CLASS_C_DEC::
    join(PB_DS_CLASS_C_DEC& other)
    {
      // XXX FIXME: Adapt for __gnu_cxx::throw_allocator_random.
      // __gnu_cxx::throw_allocator<char> alloc;
      // const double orig_throw_prob = alloc.get_probability();
      // alloc.set_probability(0);
      key_set_iterator it = other.m_key_set.begin();
      while (it != other.m_key_set.end())
	{
	  insert_new(*it);
	  it = other.m_key_set.erase(it);
	}
      _GLIBCXX_DEBUG_ASSERT(other.m_key_set.empty());
      // alloc.set_probability(orig_throw_prob);
    }

#undef PB_DS_CLASS_T_DEC
#undef PB_DS_CLASS_C_DEC

} // namespace detail
} // namespace __gnu_pbds

#endif 

#endif 

