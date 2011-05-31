// -*- C++ -*-

// Copyright (C) 2005, 2006, 2009, 2010 Free Software Foundation, Inc.
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
 * @file assoc_container.hpp
 * Contains associative containers.
 */

#ifndef PB_DS_ASSOC_CNTNR_HPP
#define PB_DS_ASSOC_CNTNR_HPP

#include <ext/typelist.h>
#include <ext/pb_ds/tag_and_trait.hpp>
#include <ext/pb_ds/detail/standard_policies.hpp>
#include <ext/pb_ds/detail/container_base_dispatch.hpp>
#include <ext/pb_ds/detail/basic_tree_policy/traits.hpp>

namespace __gnu_pbds
{
  /** @defgroup pbds Policy-Based Data Structures
   *  @ingroup extensions
   *
   *  This is a library of policy-based elementary data structures:
   *  associative containers and priority queues. It is designed for
   *  high-performance, flexibility, semantic safety, and conformance
   *  to the corresponding containers in std (except for some points
   *  where it differs by design).
   *
   *  For details, see: 
   *  http://gcc.gnu.org/onlinedocs/libstdc++/ext/pb_ds/index.html
   *
   *  @{
   */
 
#define PB_DS_BASE_C_DEC \
  detail::container_base_dispatch<Key, Mapped, Tag, Policy_Tl, Allocator>::type

  /// An abstract basic associative container.
  template<typename Key, 
	   typename Mapped, 
	   typename Tag, 
	   typename Policy_Tl, 
	   typename Allocator>
  class container_base : public PB_DS_BASE_C_DEC
  {
  private:
    typedef typename PB_DS_BASE_C_DEC 			base_type;

  public:
    typedef Tag 					container_category;
    typedef Allocator 					allocator_type;
    typedef typename allocator_type::size_type 		size_type;
    typedef typename allocator_type::difference_type 	difference_type;

    // key_type
    typedef typename allocator_type::template rebind<Key>::other::value_type key_type;
    typedef typename allocator_type::template rebind<key_type>::other key_rebind;
    typedef typename key_rebind::reference 		key_reference;
    typedef typename key_rebind::const_reference 	const_key_reference;
    typedef typename key_rebind::pointer 		key_pointer;
    typedef typename key_rebind::const_pointer 		const_key_pointer;

    // mapped_type
    typedef Mapped 					mapped_type;
    typedef typename allocator_type::template rebind<mapped_type>::other mapped_rebind;
    typedef typename mapped_rebind::reference 		mapped_reference;
    typedef typename mapped_rebind::const_reference	const_mapped_reference;
    typedef typename mapped_rebind::pointer 		mapped_pointer;
    typedef typename mapped_rebind::const_pointer 	const_mapped_pointer;

    // value_type
    typedef typename base_type::value_type 		value_type;
    typedef typename allocator_type::template rebind<value_type>::other value_rebind;
    typedef typename value_rebind::reference		reference;
    typedef typename value_rebind::const_reference 	const_reference;
    typedef typename value_rebind::pointer 		pointer;
    typedef typename value_rebind::const_pointer 	const_pointer;

    // iterators
    typedef typename base_type::iterator 		iterator;
    typedef typename base_type::const_iterator 		const_iterator;
    typedef typename base_type::point_iterator 		point_iterator;
    typedef typename base_type::const_point_iterator 	const_point_iterator;

    virtual
    ~container_base() { }

  protected:
#define PB_DS_CLASS_NAME 		container_base
#include <ext/pb_ds/detail/constructors_destructor_fn_imps.hpp>
#undef PB_DS_CLASS_NAME
  };

#undef PB_DS_BASE_C_DEC


#define PB_DS_BASE_C_DEC \
  container_base<Key, Mapped, Tag, typename __gnu_cxx::typelist::append< \
  typename __gnu_cxx::typelist::create4<Hash_Fn, Eq_Fn, Resize_Policy, detail::integral_constant<int, Store_Hash> >::type, Policy_TL>::type, Allocator>

  /// An abstract basic hash-based associative container.
  template<typename Key,
	   typename Mapped,
	   typename Hash_Fn,
	   typename Eq_Fn,
	   typename Resize_Policy,
	   bool Store_Hash,
	   typename Tag,
	   typename Policy_TL,
	   typename Allocator>
  class basic_hash_table : public PB_DS_BASE_C_DEC
  {
  private:
    typedef PB_DS_BASE_C_DEC base_type;

  public:
    virtual
    ~basic_hash_table() { }

  protected:
#define PB_DS_CLASS_NAME basic_hash_table
#include <ext/pb_ds/detail/constructors_destructor_fn_imps.hpp>
#undef PB_DS_CLASS_NAME

  private:
    basic_hash_table& 
    operator=(const base_type&);
  };

#undef PB_DS_BASE_C_DEC


#define PB_DS_BASE_C_DEC \
  basic_hash_table<Key, Mapped,	Hash_Fn, Eq_Fn, Resize_Policy, Store_Hash, \
		   cc_hash_tag,	\
	  typename __gnu_cxx::typelist::create1<Comb_Hash_Fn>::type, Allocator>

  /// A concrete collision-chaining hash-based associative container.
  template<typename Key,
	   typename Mapped,
	   typename Hash_Fn = typename detail::default_hash_fn<Key>::type,
	   typename Eq_Fn = typename detail::default_eq_fn<Key>::type,
	   typename Comb_Hash_Fn = detail::default_comb_hash_fn::type,
	   typename Resize_Policy = typename detail::default_resize_policy<Comb_Hash_Fn>::type,
	   bool Store_Hash = detail::default_store_hash,
	   typename Allocator = std::allocator<char> >
  class cc_hash_table :  public PB_DS_BASE_C_DEC
  {
  private:
    typedef PB_DS_BASE_C_DEC 	base_type;

  public:
    typedef Hash_Fn 		hash_fn;
    typedef Eq_Fn 		eq_fn;
    typedef Resize_Policy 	resize_policy;
    typedef Comb_Hash_Fn 	comb_hash_fn;

    // Default constructor.
    cc_hash_table() { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the Hash_Fn object of the container object.
    cc_hash_table(const hash_fn& h) 
    : base_type(h) { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object, and
    // r_eq_fn will be copied by the eq_fn object of the container
    // object.
    cc_hash_table(const hash_fn& h, const eq_fn& e)
    : base_type(h, e) { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object, r_eq_fn
    // will be copied by the eq_fn object of the container object, and
    // r_comb_hash_fn will be copied by the comb_hash_fn object of the
    // container object.
    cc_hash_table(const hash_fn& h, const eq_fn& e, const comb_hash_fn& ch)
    : base_type(h, e, ch) { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object, r_eq_fn
    // will be copied by the eq_fn object of the container object,
    // r_comb_hash_fn will be copied by the comb_hash_fn object of the
    // container object, and r_resize_policy will be copied by the
    // resize_policy object of the container object.
    cc_hash_table(const hash_fn& h, const eq_fn& e, const comb_hash_fn& ch, 
		  const resize_policy& rp)    
    : base_type(h, e, ch, rp) { }

    // Constructor taking __iterators to a range of value_types. The
    // value_types between first_it and last_it will be inserted into
    // the container object.
    template<typename It>
    cc_hash_table(It first, It last)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects. The value_types between first_it and
    // last_it will be inserted into the container object.
    template<typename It>
    cc_hash_table(It first, It last, const hash_fn& h)
    : base_type(h)
    { copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object,
    // and r_eq_fn will be copied by the eq_fn object of the container
    // object.
    template<typename It>
    cc_hash_table(It first, It last, const hash_fn& h, const eq_fn& e)
    : base_type(h, e)
    { copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object,
    // r_eq_fn will be copied by the eq_fn object of the container
    // object, and r_comb_hash_fn will be copied by the comb_hash_fn
    // object of the container object.
    template<typename It>
    cc_hash_table(It first, It last, const hash_fn& h, const eq_fn& e,
		  const comb_hash_fn& ch)
    : base_type(h, e, ch)
    { copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object,
    // r_eq_fn will be copied by the eq_fn object of the container
    // object, r_comb_hash_fn will be copied by the comb_hash_fn
    // object of the container object, and r_resize_policy will be
    // copied by the resize_policy object of the container object.
    template<typename It>
    cc_hash_table(It first, It last, const hash_fn& h, const eq_fn& e, 
		  const comb_hash_fn& ch, const resize_policy& rp)
    : base_type(h, e, ch, rp)
    { copy_from_range(first, last); }

    cc_hash_table(const cc_hash_table& other)
    : base_type((const base_type&)other)
    { }

    virtual
    ~cc_hash_table() { }

    cc_hash_table& 
    operator=(const cc_hash_table& other)
    {
      if (this != &other)
	{
	  cc_hash_table tmp(other);
	  swap(tmp);
	}
      return *this;
    }

    void
    swap(cc_hash_table& other)
    { base_type::swap(other); }
  };

#undef PB_DS_BASE_C_DEC


#define PB_DS_BASE_C_DEC \
  basic_hash_table<Key, Mapped,	Hash_Fn, Eq_Fn, Resize_Policy, Store_Hash, \
		   gp_hash_tag, \
		   typename __gnu_cxx::typelist::create2<Comb_Probe_Fn, Probe_Fn>::type, Allocator>

  /// A concrete general-probing hash-based associative container.
  template<typename Key,
	   typename Mapped,
	   typename Hash_Fn = typename detail::default_hash_fn<Key>::type,
	   typename Eq_Fn = typename detail::default_eq_fn<Key>::type,
	   typename Comb_Probe_Fn = detail::default_comb_hash_fn::type,
	   typename Probe_Fn = typename detail::default_probe_fn<Comb_Probe_Fn>::type,
	   typename Resize_Policy = typename detail::default_resize_policy<Comb_Probe_Fn>::type,
	   bool Store_Hash = detail::default_store_hash,
	   typename Allocator = std::allocator<char> >
  class gp_hash_table : public PB_DS_BASE_C_DEC
  {
  private:
    typedef PB_DS_BASE_C_DEC 	base_type;

  public:
    typedef Hash_Fn 		hash_fn;
    typedef Eq_Fn 		eq_fn;
    typedef Comb_Probe_Fn	comb_probe_fn;
    typedef Probe_Fn 		probe_fn;
    typedef Resize_Policy 	resize_policy;

    // Default constructor.
    gp_hash_table() { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object.
    gp_hash_table(const hash_fn& h)
    : base_type(h) { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object, and
    // r_eq_fn will be copied by the eq_fn object of the container
    // object.
    gp_hash_table(const hash_fn& h, const eq_fn& e)
    : base_type(h, e) { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object, r_eq_fn
    // will be copied by the eq_fn object of the container object, and
    // r_comb_probe_fn will be copied by the comb_probe_fn object of
    // the container object.
    gp_hash_table(const hash_fn& h, const eq_fn& e, const comb_probe_fn& cp)
    : base_type(h, e, cp) { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object, r_eq_fn
    // will be copied by the eq_fn object of the container object,
    // r_comb_probe_fn will be copied by the comb_probe_fn object of
    // the container object, and r_probe_fn will be copied by the
    // probe_fn object of the container object.
    gp_hash_table(const hash_fn& h, const eq_fn& e, const comb_probe_fn& cp, 
		  const probe_fn& p)
    : base_type(h, e, cp, p) { }

    // Constructor taking some policy objects. r_hash_fn will be
    // copied by the hash_fn object of the container object, r_eq_fn
    // will be copied by the eq_fn object of the container object,
    // r_comb_probe_fn will be copied by the comb_probe_fn object of
    // the container object, r_probe_fn will be copied by the probe_fn
    // object of the container object, and r_resize_policy will be
    // copied by the Resize_Policy object of the container object.
    gp_hash_table(const hash_fn& h, const eq_fn& e, const comb_probe_fn& cp, 
		  const probe_fn& p, const resize_policy& rp)
    : base_type(h, e, cp, p, rp) { }

    // Constructor taking __iterators to a range of value_types. The
    // value_types between first_it and last_it will be inserted into
    // the container object.
    template<typename It>
    gp_hash_table(It first, It last)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects. The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object.
    template<typename It>
    gp_hash_table(It first, It last, const hash_fn& h)
    : base_type(h)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects. The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object,
    // and r_eq_fn will be copied by the eq_fn object of the container
    // object.
    template<typename It>
    gp_hash_table(It first, It last, const hash_fn& h, const eq_fn& e)
    : base_type(h, e)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects. The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object,
    // r_eq_fn will be copied by the eq_fn object of the container
    // object, and r_comb_probe_fn will be copied by the comb_probe_fn
    // object of the container object.
    template<typename It>
    gp_hash_table(It first, It last, const hash_fn& h, const eq_fn& e, 
		  const comb_probe_fn& cp)
    : base_type(h, e, cp)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects. The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object,
    // r_eq_fn will be copied by the eq_fn object of the container
    // object, r_comb_probe_fn will be copied by the comb_probe_fn
    // object of the container object, and r_probe_fn will be copied
    // by the probe_fn object of the container object.
    template<typename It>
    gp_hash_table(It first, It last, const hash_fn& h, const eq_fn& e, 
		  const comb_probe_fn& cp, const probe_fn& p)
    : base_type(h, e, cp, p)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects. The value_types between first_it and
    // last_it will be inserted into the container object. r_hash_fn
    // will be copied by the hash_fn object of the container object,
    // r_eq_fn will be copied by the eq_fn object of the container
    // object, r_comb_probe_fn will be copied by the comb_probe_fn
    // object of the container object, r_probe_fn will be copied by
    // the probe_fn object of the container object, and
    // r_resize_policy will be copied by the resize_policy object of
    // the container object.
    template<typename It>
    gp_hash_table(It first, It last, const hash_fn& h, const eq_fn& e, 
		  const comb_probe_fn& cp, const probe_fn& p, 
		  const resize_policy& rp)
    : base_type(h, e, cp, p, rp)
    { base_type::copy_from_range(first, last); }

    gp_hash_table(const gp_hash_table& other)
    : base_type((const base_type&)other)
    { }

    virtual
    ~gp_hash_table() { }

    gp_hash_table& 
    operator=(const gp_hash_table& other)
    {
      if (this != &other)
	{
	  gp_hash_table tmp(other);
	  swap(tmp);
	}
      return *this;
    }

    void
    swap(gp_hash_table& other)
    { base_type::swap(other); }
  };

#undef PB_DS_BASE_C_DEC


#define PB_DS_BASE_C_DEC \
  container_base<Key, Mapped, Tag, Policy_Tl, Allocator>

  /// An abstract basic tree-like (tree, trie) associative container.
  template<typename Key, typename Mapped, typename Tag, 
	   typename Node_Update, typename Policy_Tl, typename Allocator>
  class basic_tree : public PB_DS_BASE_C_DEC
  {
  private:
    typedef PB_DS_BASE_C_DEC 	base_type;

  public:
    typedef Node_Update 	node_update;

    virtual
    ~basic_tree() { }

  protected:
#define PB_DS_CLASS_NAME 		basic_tree
#include <ext/pb_ds/detail/constructors_destructor_fn_imps.hpp>
#undef PB_DS_CLASS_NAME
  };

#undef PB_DS_BASE_C_DEC


#define PB_DS_TREE_NODE_AND_IT_TRAITS_C_DEC \
  detail::tree_traits<Key, Mapped,Cmp_Fn,Node_Update,Tag, Allocator>

#define PB_DS_BASE_C_DEC \
  basic_tree<Key,Mapped,Tag,typename PB_DS_TREE_NODE_AND_IT_TRAITS_C_DEC::node_update, \
	     typename __gnu_cxx::typelist::create2<Cmp_Fn, PB_DS_TREE_NODE_AND_IT_TRAITS_C_DEC >::type, Allocator>

  /// A concrete basic tree-based associative container.
  template<typename Key, typename Mapped, typename Cmp_Fn = std::less<Key>,
	   typename Tag = rb_tree_tag,
	   template<typename Const_Node_Iterator, typename Node_Iterator, typename Cmp_Fn_, typename Allocator_>
  class Node_Update = __gnu_pbds::null_tree_node_update,
	   typename Allocator = std::allocator<char> >
  class tree : public PB_DS_BASE_C_DEC
  {
  private:
    typedef PB_DS_BASE_C_DEC 	base_type;

  public:
    // Comparison functor type.
    typedef Cmp_Fn 		cmp_fn;

    tree() { }

    // Constructor taking some policy objects. r_cmp_fn will be copied
    // by the Cmp_Fn object of the container object.
    tree(const cmp_fn& c)
    : base_type(c) { }

    // Constructor taking __iterators to a range of value_types. The
    // value_types between first_it and last_it will be inserted into
    // the container object.
    template<typename It>
    tree(It first, It last)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects The value_types between first_it and
    // last_it will be inserted into the container object. r_cmp_fn
    // will be copied by the cmp_fn object of the container object.
    template<typename It>
    tree(It first, It last, const cmp_fn& c)
      : base_type(c)
    { base_type::copy_from_range(first, last); }

    tree(const tree& other)
    : base_type((const base_type&)other) { }

    virtual
    ~tree() { }

    tree& 
    operator=(const tree& other)
    {
      if (this != &other)
	{
	  tree tmp(other);
	  swap(tmp);
	}
      return *this;
    }

    void
    swap(tree& other)
    { base_type::swap(other); }
  };

#undef PB_DS_BASE_C_DEC
#undef PB_DS_TREE_NODE_AND_IT_TRAITS_C_DEC


#define PB_DS_TRIE_NODE_AND_ITS_TRAITS \
  detail::trie_traits<Key,Mapped,E_Access_Traits,Node_Update,Tag,Allocator>

#define PB_DS_BASE_C_DEC \
  basic_tree<Key,Mapped,Tag, typename PB_DS_TRIE_NODE_AND_ITS_TRAITS::node_update, \
	     typename __gnu_cxx::typelist::create2<E_Access_Traits, PB_DS_TRIE_NODE_AND_ITS_TRAITS >::type, Allocator>

  /// A concrete basic trie-based associative container.
  template<typename Key,
	   typename Mapped,
	   typename E_Access_Traits = typename detail::default_trie_e_access_traits<Key>::type,
	   typename Tag = pat_trie_tag,
	   template<typename Const_Node_Iterator,
		    typename Node_Iterator,
		    typename E_Access_Traits_,
		    typename Allocator_>
  class Node_Update = null_trie_node_update,
	   typename Allocator = std::allocator<char> >
  class trie : public PB_DS_BASE_C_DEC
  {
  private:
    typedef PB_DS_BASE_C_DEC base_type;

  public:
    // Element access traits type.
    typedef E_Access_Traits e_access_traits;

    trie() { }

    // Constructor taking some policy objects. r_e_access_traits will
    // be copied by the E_Access_Traits object of the container
    // object.
    trie(const e_access_traits& t)
    : base_type(t) { }

    // Constructor taking __iterators to a range of value_types. The
    // value_types between first_it and last_it will be inserted into
    // the container object.
    template<typename It>
    trie(It first, It last)
    { base_type::copy_from_range(first, last); }

    // Constructor taking __iterators to a range of value_types and
    // some policy objects. The value_types between first_it and
    // last_it will be inserted into the container object.
    template<typename It>
    trie(It first, It last, const e_access_traits& t)
    : base_type(t)
    { base_type::copy_from_range(first, last); }

    trie(const trie& other)
    : base_type((const base_type&)other) { }

    virtual
    ~trie() { }

    trie& 
    operator=(const trie& other)
    {
      if (this != &other)
	{
	  trie tmp(other);
	  swap(tmp);
	}
      return *this;
    }

    void
    swap(trie& other)
    { base_type::swap(other); }
  };

#undef PB_DS_BASE_C_DEC
#undef PB_DS_TRIE_NODE_AND_ITS_TRAITS


#define PB_DS_BASE_C_DEC \
  container_base<Key, Mapped, list_update_tag, \
		 typename __gnu_cxx::typelist::create2<Eq_Fn, Update_Policy>::type, Allocator>

  /// A list-update based associative container.
  template<typename Key,
	   typename Mapped,
	   class Eq_Fn = typename detail::default_eq_fn<Key>::type,
	   class Update_Policy = detail::default_update_policy::type,
	   class Allocator = std::allocator<char> >
  class list_update : public PB_DS_BASE_C_DEC
  {
  private:
    typedef PB_DS_BASE_C_DEC 	base_type;

  public:
    typedef Eq_Fn 		eq_fn;
    typedef Update_Policy 	update_policy;
    typedef Allocator 		allocator;

    list_update() { }

    // Constructor taking __iterators to a range of value_types. The
    // value_types between first_it and last_it will be inserted into
    // the container object.
    template<typename It>
    list_update(It first, It last)
    { base_type::copy_from_range(first, last); }

    list_update(const list_update& other)
    : base_type((const base_type&)other) { }

    virtual
    ~list_update() { }

    list_update& 
    operator=(const list_update& other)
    {
      if (this !=& other)
	{
	  list_update tmp(other);
	  swap(tmp);
	}
      return *this;
    }

    void
    swap(list_update& other)
    { base_type::swap(other); }
  };

#undef PB_DS_BASE_C_DEC

  // @} group pbds
} // namespace __gnu_pbds

#endif 
