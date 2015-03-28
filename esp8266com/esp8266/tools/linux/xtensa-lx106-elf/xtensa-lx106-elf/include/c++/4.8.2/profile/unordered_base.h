// Profiling unordered containers implementation details -*- C++ -*-

// Copyright (C) 2013 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

/** @file profile/unordered_base.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_UNORDERED
#define _GLIBCXX_PROFILE_UNORDERED 1

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __profile
{
  template<typename _UnorderedCont,
	   typename _Value, bool _Cache_hash_code>
    struct _Bucket_index_helper;

  template<typename _UnorderedCont, typename _Value>
    struct _Bucket_index_helper<_UnorderedCont, _Value, true>
    {
      static std::size_t
      bucket(const _UnorderedCont& __uc,
	     const __detail::_Hash_node<_Value, true>* __node)
      { return __node->_M_hash_code % __uc.bucket_count(); }
    };

  template<typename _UnorderedCont, typename _Value>
    struct _Bucket_index_helper<_UnorderedCont, _Value, false>
    {
      static std::size_t
      bucket(const _UnorderedCont& __uc,
	     const __detail::_Hash_node<_Value, false>* __node)
      { return __uc.bucket(__node->_M_v); }
    };

  template<typename _UnorderedCont, typename _Key, typename _Mapped>
    struct _Bucket_index_helper<_UnorderedCont,
				std::pair<const _Key, _Mapped>, false>
    {
      typedef std::pair<const _Key, _Mapped> _Value;

      static std::size_t
      bucket(const _UnorderedCont& __uc,
	     const __detail::_Hash_node<_Value, false>* __node)
      { return __uc.bucket(__node->_M_v.first); }
    };

  template<typename _UnorderedCont, typename _Value, bool _Cache_hash_code>
    std::size_t
    __get_bucket_index(const _UnorderedCont& __uc,
		       const __detail::_Hash_node<_Value, _Cache_hash_code>* __node)
    {
      using __bucket_index_helper
	= _Bucket_index_helper<_UnorderedCont, _Value, _Cache_hash_code>;
      return __bucket_index_helper::bucket(__uc, __node);
    }

  template<typename _UnorderedCont,
	   typename _Value, bool _Cache_hash_code>
    struct _Equal_helper;

  template<typename _UnorderedCont, typename _Value>
    struct _Equal_helper<_UnorderedCont, _Value, true>
    {
      static std::size_t
      are_equal(const _UnorderedCont& __uc,
		const __detail::_Hash_node<_Value, true>* __lhs,
		const __detail::_Hash_node<_Value, true>* __rhs)
      {
	return __lhs->_M_hash_code == __rhs->_M_hash_code
	  && __uc.key_eq()(__lhs->_M_v, __rhs->_M_v);
      }
    };

  template<typename _UnorderedCont,
	   typename _Value>
    struct _Equal_helper<_UnorderedCont, _Value, false>
    {
      static std::size_t
      are_equal(const _UnorderedCont& __uc,
		const __detail::_Hash_node<_Value, false>* __lhs,
		const __detail::_Hash_node<_Value, false>* __rhs)
      { return __uc.key_eq()(__lhs->_M_v, __rhs->_M_v); }
    };

  template<typename _UnorderedCont,
	   typename _Key, typename _Mapped>
    struct _Equal_helper<_UnorderedCont, std::pair<const _Key, _Mapped>, true>
    {
      typedef std::pair<const _Key, _Mapped> _Value;

      static std::size_t
      are_equal(const _UnorderedCont& __uc,
		const __detail::_Hash_node<_Value, true>* __lhs,
		const __detail::_Hash_node<_Value, true>* __rhs)
      {
	return __lhs->_M_hash_code == __rhs->_M_hash_code
	  && __uc.key_eq()(__lhs->_M_v.first, __rhs->_M_v.first);
      }
    };

  template<typename _UnorderedCont,
	   typename _Key, typename _Mapped>
    struct _Equal_helper<_UnorderedCont, std::pair<const _Key, _Mapped>, false>
    {
      typedef std::pair<const _Key, _Mapped> _Value;

      static std::size_t
      are_equal(const _UnorderedCont& __uc,
		const __detail::_Hash_node<_Value, false>* __lhs,
		const __detail::_Hash_node<_Value, false>* __rhs)
      { return __uc.key_eq()(__lhs->_M_v.first, __rhs->_M_v.first); }
    };

  template<typename _UnorderedCont, typename _Value, bool _Cache_hash_code>
    bool
    __are_equal(const _UnorderedCont& __uc,
		const __detail::_Hash_node<_Value, _Cache_hash_code>* __lhs,
		const __detail::_Hash_node<_Value, _Cache_hash_code>* __rhs)
  {
    using __equal_helper
      = _Equal_helper<_UnorderedCont, _Value, _Cache_hash_code>;
    return __equal_helper::are_equal(__uc, __lhs, __rhs);
  }

  template<typename _UnorderedCont, bool _Unique_keys>
    class _Unordered_profile
    {
      _UnorderedCont&
      _M_conjure()
      { return *(static_cast<_UnorderedCont*>(this)); }

      using __unique_keys = std::integral_constant<bool, _Unique_keys>;

    protected:
      _Unordered_profile()
      {
	auto& __uc = _M_conjure();
        __profcxx_hashtable_construct(&__uc, __uc.bucket_count());
	__profcxx_hashtable_construct2(&__uc);
      }
      _Unordered_profile(const _Unordered_profile&)
	: _Unordered_profile() { }
      _Unordered_profile(_Unordered_profile&&)
	: _Unordered_profile() { }

      ~_Unordered_profile() noexcept
      {
	auto& __uc = _M_conjure();
        __profcxx_hashtable_destruct(&__uc, __uc.bucket_count(), __uc.size());
        _M_profile_destruct();
      }

      _Unordered_profile&
      operator=(const _Unordered_profile&) = default;

      _Unordered_profile&
      operator=(_Unordered_profile&&) = default;

      void
      _M_profile_destruct()
      {
	if (!__profcxx_inefficient_hash_is_on())
	  return;

	_M_profile_destruct(__unique_keys());
      }

    private:
      void
      _M_profile_destruct(std::true_type);

      void
      _M_profile_destruct(std::false_type);
    };

  template<typename _UnorderedCont, bool _Unique_keys>
    void
    _Unordered_profile<_UnorderedCont, _Unique_keys>::
    _M_profile_destruct(std::true_type)
    {
      auto& __uc = _M_conjure();
      std::size_t __hops = 0, __lc = 0, __chain = 0;
      auto __it = __uc.begin();
      while (__it != __uc.end())
	{
	  auto __bkt = __get_bucket_index(__uc, __it._M_cur);
	  auto __lit = __uc.begin(__bkt);
	  auto __lend = __uc.end(__bkt);
	  for (++__it, ++__lit; __lit != __lend; ++__it, ++__lit)
	    ++__chain;
	  if (__chain)
	    {
	      ++__chain;
	      __lc = __lc > __chain ? __lc : __chain;
	      __hops += __chain * (__chain - 1) / 2;
	      __chain = 0;
	    }
	}
      __profcxx_hashtable_destruct2(&__uc, __lc, __uc.size(), __hops);
    }

  template<typename _UnorderedCont, bool _Unique_keys>
    void
    _Unordered_profile<_UnorderedCont, _Unique_keys>::
    _M_profile_destruct(std::false_type)
    {
      auto& __uc = _M_conjure();
      std::size_t __hops = 0, __lc = 0, __chain = 0, __unique_size = 0;
      auto __it = __uc.begin();
      while (__it != __uc.end())
	{
	  auto __bkt = __get_bucket_index(__uc, __it._M_cur);
	  auto __lit = __uc.begin(__bkt);
	  auto __lend = __uc.end(__bkt);
	  auto __pit = __it;
	  ++__unique_size;
	  for (++__it, ++__lit; __lit != __lend; ++__it, ++__lit)
	    {
	      if (!__are_equal(__uc, __pit._M_cur, __it._M_cur))
		{
		  ++__chain;
		  ++__unique_size;
		  __pit = __it;
		}
	    }
	  if (__chain)
	    {
	      ++__chain;
	      __lc = __lc > __chain ? __lc : __chain;
	      __hops += __chain * (__chain - 1) / 2;
	      __chain = 0;
	    }
	}
      __profcxx_hashtable_destruct2(&__uc, __lc, __unique_size, __hops);
    }

} // namespace __profile
} // namespace std

#endif
