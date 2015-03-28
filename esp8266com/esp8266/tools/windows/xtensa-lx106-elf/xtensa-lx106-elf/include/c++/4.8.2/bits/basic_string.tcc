// Components for manipulating sequences of characters -*- C++ -*-

// Copyright (C) 1997-2013 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file bits/basic_string.tcc
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{string}
 */

//
// ISO C++ 14882: 21  Strings library
//

// Written by Jason Merrill based upon the specification by Takanori Adachi
// in ANSI X3J16/94-0013R2.  Rewritten by Nathan Myers to ISO-14882.

#ifndef _BASIC_STRING_TCC
#define _BASIC_STRING_TCC 1

#pragma GCC system_header

#include <bits/cxxabi_forced.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _CharT, typename _Traits, typename _Alloc>
    const typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    _Rep::_S_max_size = (((npos - sizeof(_Rep_base))/sizeof(_CharT)) - 1) / 4;

  template<typename _CharT, typename _Traits, typename _Alloc>
    const _CharT
    basic_string<_CharT, _Traits, _Alloc>::
    _Rep::_S_terminal = _CharT();

  template<typename _CharT, typename _Traits, typename _Alloc>
    const typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::npos;

  // Linker sets _S_empty_rep_storage to all 0s (one reference, empty string)
  // at static init time (before static ctors are run).
  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::_Rep::_S_empty_rep_storage[
    (sizeof(_Rep_base) + sizeof(_CharT) + sizeof(size_type) - 1) /
      sizeof(size_type)];

  // NB: This is the special case for Input Iterators, used in
  // istreambuf_iterators, etc.
  // Input Iterators have a cost structure very different from
  // pointers, calling for a different coding style.
  template<typename _CharT, typename _Traits, typename _Alloc>
    template<typename _InIterator>
      _CharT*
      basic_string<_CharT, _Traits, _Alloc>::
      _S_construct(_InIterator __beg, _InIterator __end, const _Alloc& __a,
		   input_iterator_tag)
      {
#if _GLIBCXX_FULLY_DYNAMIC_STRING == 0
	if (__beg == __end && __a == _Alloc())
	  return _S_empty_rep()._M_refdata();
#endif
	// Avoid reallocation for common case.
	_CharT __buf[128];
	size_type __len = 0;
	while (__beg != __end && __len < sizeof(__buf) / sizeof(_CharT))
	  {
	    __buf[__len++] = *__beg;
	    ++__beg;
	  }
	_Rep* __r = _Rep::_S_create(__len, size_type(0), __a);
	_M_copy(__r->_M_refdata(), __buf, __len);
	__try
	  {
	    while (__beg != __end)
	      {
		if (__len == __r->_M_capacity)
		  {
		    // Allocate more space.
		    _Rep* __another = _Rep::_S_create(__len + 1, __len, __a);
		    _M_copy(__another->_M_refdata(), __r->_M_refdata(), __len);
		    __r->_M_destroy(__a);
		    __r = __another;
		  }
		__r->_M_refdata()[__len++] = *__beg;
		++__beg;
	      }
	  }
	__catch(...)
	  {
	    __r->_M_destroy(__a);
	    __throw_exception_again;
	  }
	__r->_M_set_length_and_sharable(__len);
	return __r->_M_refdata();
      }

  template<typename _CharT, typename _Traits, typename _Alloc>
    template <typename _InIterator>
      _CharT*
      basic_string<_CharT, _Traits, _Alloc>::
      _S_construct(_InIterator __beg, _InIterator __end, const _Alloc& __a,
		   forward_iterator_tag)
      {
#if _GLIBCXX_FULLY_DYNAMIC_STRING == 0
	if (__beg == __end && __a == _Alloc())
	  return _S_empty_rep()._M_refdata();
#endif
	// NB: Not required, but considered best practice.
	if (__gnu_cxx::__is_null_pointer(__beg) && __beg != __end)
	  __throw_logic_error(__N("basic_string::_S_construct null not valid"));

	const size_type __dnew = static_cast<size_type>(std::distance(__beg,
								      __end));
	// Check for out_of_range and length_error exceptions.
	_Rep* __r = _Rep::_S_create(__dnew, size_type(0), __a);
	__try
	  { _S_copy_chars(__r->_M_refdata(), __beg, __end); }
	__catch(...)
	  {
	    __r->_M_destroy(__a);
	    __throw_exception_again;
	  }
	__r->_M_set_length_and_sharable(__dnew);
	return __r->_M_refdata();
      }

  template<typename _CharT, typename _Traits, typename _Alloc>
    _CharT*
    basic_string<_CharT, _Traits, _Alloc>::
    _S_construct(size_type __n, _CharT __c, const _Alloc& __a)
    {
#if _GLIBCXX_FULLY_DYNAMIC_STRING == 0
      if (__n == 0 && __a == _Alloc())
	return _S_empty_rep()._M_refdata();
#endif
      // Check for out_of_range and length_error exceptions.
      _Rep* __r = _Rep::_S_create(__n, size_type(0), __a);
      if (__n)
	_M_assign(__r->_M_refdata(), __n, __c);

      __r->_M_set_length_and_sharable(__n);
      return __r->_M_refdata();
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const basic_string& __str)
    : _M_dataplus(__str._M_rep()->_M_grab(_Alloc(__str.get_allocator()),
					  __str.get_allocator()),
		  __str.get_allocator())
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const _Alloc& __a)
    : _M_dataplus(_S_construct(size_type(), _CharT(), __a), __a)
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const basic_string& __str, size_type __pos, size_type __n)
    : _M_dataplus(_S_construct(__str._M_data()
			       + __str._M_check(__pos,
						"basic_string::basic_string"),
			       __str._M_data() + __str._M_limit(__pos, __n)
			       + __pos, _Alloc()), _Alloc())
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const basic_string& __str, size_type __pos,
		 size_type __n, const _Alloc& __a)
    : _M_dataplus(_S_construct(__str._M_data()
			       + __str._M_check(__pos,
						"basic_string::basic_string"),
			       __str._M_data() + __str._M_limit(__pos, __n)
			       + __pos, __a), __a)
    { }

  // TBD: DPG annotate
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const _CharT* __s, size_type __n, const _Alloc& __a)
    : _M_dataplus(_S_construct(__s, __s + __n, __a), __a)
    { }

  // TBD: DPG annotate
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const _CharT* __s, const _Alloc& __a)
    : _M_dataplus(_S_construct(__s, __s ? __s + traits_type::length(__s) :
			       __s + npos, __a), __a)
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(size_type __n, _CharT __c, const _Alloc& __a)
    : _M_dataplus(_S_construct(__n, __c, __a), __a)
    { }

  // TBD: DPG annotate
  template<typename _CharT, typename _Traits, typename _Alloc>
    template<typename _InputIterator>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(_InputIterator __beg, _InputIterator __end, const _Alloc& __a)
    : _M_dataplus(_S_construct(__beg, __end, __a), __a)
    { }

#if __cplusplus >= 201103L
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(initializer_list<_CharT> __l, const _Alloc& __a)
    : _M_dataplus(_S_construct(__l.begin(), __l.end(), __a), __a)
    { }
#endif

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    assign(const basic_string& __str)
    {
      if (_M_rep() != __str._M_rep())
	{
	  // XXX MT
	  const allocator_type __a = this->get_allocator();
	  _CharT* __tmp = __str._M_rep()->_M_grab(__a, __str.get_allocator());
	  _M_rep()->_M_dispose(__a);
	  _M_data(__tmp);
	}
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    assign(const _CharT* __s, size_type __n)
    {
      __glibcxx_requires_string_len(__s, __n);
      _M_check_length(this->size(), __n, "basic_string::assign");
      if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
	return _M_replace_safe(size_type(0), this->size(), __s, __n);
      else
	{
	  // Work in-place.
	  const size_type __pos = __s - _M_data();
	  if (__pos >= __n)
	    _M_copy(_M_data(), __s, __n);
	  else if (__pos)
	    _M_move(_M_data(), __s, __n);
	  _M_rep()->_M_set_length_and_sharable(__n);
	  return *this;
	}
     }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(size_type __n, _CharT __c)
    {
      if (__n)
	{
	  _M_check_length(size_type(0), __n, "basic_string::append");	  
	  const size_type __len = __n + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    this->reserve(__len);
	  _M_assign(_M_data() + this->size(), __n, __c);
	  _M_rep()->_M_set_length_and_sharable(__len);
	}
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(const _CharT* __s, size_type __n)
    {
      __glibcxx_requires_string_len(__s, __n);
      if (__n)
	{
	  _M_check_length(size_type(0), __n, "basic_string::append");
	  const size_type __len = __n + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    {
	      if (_M_disjunct(__s))
		this->reserve(__len);
	      else
		{
		  const size_type __off = __s - _M_data();
		  this->reserve(__len);
		  __s = _M_data() + __off;
		}
	    }
	  _M_copy(_M_data() + this->size(), __s, __n);
	  _M_rep()->_M_set_length_and_sharable(__len);
	}
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(const basic_string& __str)
    {
      const size_type __size = __str.size();
      if (__size)
	{
	  const size_type __len = __size + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    this->reserve(__len);
	  _M_copy(_M_data() + this->size(), __str._M_data(), __size);
	  _M_rep()->_M_set_length_and_sharable(__len);
	}
      return *this;
    }    

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(const basic_string& __str, size_type __pos, size_type __n)
    {
      __str._M_check(__pos, "basic_string::append");
      __n = __str._M_limit(__pos, __n);
      if (__n)
	{
	  const size_type __len = __n + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    this->reserve(__len);
	  _M_copy(_M_data() + this->size(), __str._M_data() + __pos, __n);
	  _M_rep()->_M_set_length_and_sharable(__len);	  
	}
      return *this;
    }

   template<typename _CharT, typename _Traits, typename _Alloc>
     basic_string<_CharT, _Traits, _Alloc>&
     basic_string<_CharT, _Traits, _Alloc>::
     insert(size_type __pos, const _CharT* __s, size_type __n)
     {
       __glibcxx_requires_string_len(__s, __n);
       _M_check(__pos, "basic_string::insert");
       _M_check_length(size_type(0), __n, "basic_string::insert");
       if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
         return _M_replace_safe(__pos, size_type(0), __s, __n);
       else
         {
           // Work in-place.
           const size_type __off = __s - _M_data();
           _M_mutate(__pos, 0, __n);
           __s = _M_data() + __off;
           _CharT* __p = _M_data() + __pos;
           if (__s  + __n <= __p)
             _M_copy(__p, __s, __n);
           else if (__s >= __p)
             _M_copy(__p, __s + __n, __n);
           else
             {
	       const size_type __nleft = __p - __s;
               _M_copy(__p, __s, __nleft);
               _M_copy(__p + __nleft, __p + __n, __n - __nleft);
             }
           return *this;
         }
     }

   template<typename _CharT, typename _Traits, typename _Alloc>
     typename basic_string<_CharT, _Traits, _Alloc>::iterator
     basic_string<_CharT, _Traits, _Alloc>::
     erase(iterator __first, iterator __last)
     {
       _GLIBCXX_DEBUG_PEDASSERT(__first >= _M_ibegin() && __first <= __last
				&& __last <= _M_iend());

       // NB: This isn't just an optimization (bail out early when
       // there is nothing to do, really), it's also a correctness
       // issue vs MT, see libstdc++/40518.
       const size_type __size = __last - __first;
       if (__size)
	 {
	   const size_type __pos = __first - _M_ibegin();
	   _M_mutate(__pos, __size, size_type(0));
	   _M_rep()->_M_set_leaked();
	   return iterator(_M_data() + __pos);
	 }
       else
	 return __first;
     }

   template<typename _CharT, typename _Traits, typename _Alloc>
     basic_string<_CharT, _Traits, _Alloc>&
     basic_string<_CharT, _Traits, _Alloc>::
     replace(size_type __pos, size_type __n1, const _CharT* __s,
	     size_type __n2)
     {
       __glibcxx_requires_string_len(__s, __n2);
       _M_check(__pos, "basic_string::replace");
       __n1 = _M_limit(__pos, __n1);
       _M_check_length(__n1, __n2, "basic_string::replace");
       bool __left;
       if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
         return _M_replace_safe(__pos, __n1, __s, __n2);
       else if ((__left = __s + __n2 <= _M_data() + __pos)
		|| _M_data() + __pos + __n1 <= __s)
	 {
	   // Work in-place: non-overlapping case.
	   size_type __off = __s - _M_data();
	   __left ? __off : (__off += __n2 - __n1);
	   _M_mutate(__pos, __n1, __n2);
	   _M_copy(_M_data() + __pos, _M_data() + __off, __n2);
	   return *this;
	 }
       else
	 {
	   // Todo: overlapping case.
	   const basic_string __tmp(__s, __n2);
	   return _M_replace_safe(__pos, __n1, __tmp._M_data(), __n2);
	 }
     }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::_Rep::
    _M_destroy(const _Alloc& __a) throw ()
    {
      const size_type __size = sizeof(_Rep_base) +
	                       (this->_M_capacity + 1) * sizeof(_CharT);
      _Raw_bytes_alloc(__a).deallocate(reinterpret_cast<char*>(this), __size);
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    _M_leak_hard()
    {
#if _GLIBCXX_FULLY_DYNAMIC_STRING == 0
      if (_M_rep() == &_S_empty_rep())
	return;
#endif
      if (_M_rep()->_M_is_shared())
	_M_mutate(0, 0, 0);
      _M_rep()->_M_set_leaked();
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    _M_mutate(size_type __pos, size_type __len1, size_type __len2)
    {
      const size_type __old_size = this->size();
      const size_type __new_size = __old_size + __len2 - __len1;
      const size_type __how_much = __old_size - __pos - __len1;

      if (__new_size > this->capacity() || _M_rep()->_M_is_shared())
	{
	  // Must reallocate.
	  const allocator_type __a = get_allocator();
	  _Rep* __r = _Rep::_S_create(__new_size, this->capacity(), __a);

	  if (__pos)
	    _M_copy(__r->_M_refdata(), _M_data(), __pos);
	  if (__how_much)
	    _M_copy(__r->_M_refdata() + __pos + __len2,
		    _M_data() + __pos + __len1, __how_much);

	  _M_rep()->_M_dispose(__a);
	  _M_data(__r->_M_refdata());
	}
      else if (__how_much && __len1 != __len2)
	{
	  // Work in-place.
	  _M_move(_M_data() + __pos + __len2,
		  _M_data() + __pos + __len1, __how_much);
	}
      _M_rep()->_M_set_length_and_sharable(__new_size);
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    reserve(size_type __res)
    {
      if (__res != this->capacity() || _M_rep()->_M_is_shared())
        {
	  // Make sure we don't shrink below the current size
	  if (__res < this->size())
	    __res = this->size();
	  const allocator_type __a = get_allocator();
	  _CharT* __tmp = _M_rep()->_M_clone(__a, __res - this->size());
	  _M_rep()->_M_dispose(__a);
	  _M_data(__tmp);
        }
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    swap(basic_string& __s)
    {
      if (_M_rep()->_M_is_leaked())
	_M_rep()->_M_set_sharable();
      if (__s._M_rep()->_M_is_leaked())
	__s._M_rep()->_M_set_sharable();
      if (this->get_allocator() == __s.get_allocator())
	{
	  _CharT* __tmp = _M_data();
	  _M_data(__s._M_data());
	  __s._M_data(__tmp);
	}
      // The code below can usually be optimized away.
      else
	{
	  const basic_string __tmp1(_M_ibegin(), _M_iend(),
				    __s.get_allocator());
	  const basic_string __tmp2(__s._M_ibegin(), __s._M_iend(),
				    this->get_allocator());
	  *this = __tmp2;
	  __s = __tmp1;
	}
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::_Rep*
    basic_string<_CharT, _Traits, _Alloc>::_Rep::
    _S_create(size_type __capacity, size_type __old_capacity,
	      const _Alloc& __alloc)
    {
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 83.  String::npos vs. string::max_size()
      if (__capacity > _S_max_size)
	__throw_length_error(__N("basic_string::_S_create"));

      // The standard places no restriction on allocating more memory
      // than is strictly needed within this layer at the moment or as
      // requested by an explicit application call to reserve().

      // Many malloc implementations perform quite poorly when an
      // application attempts to allocate memory in a stepwise fashion
      // growing each allocation size by only 1 char.  Additionally,
      // it makes little sense to allocate less linear memory than the
      // natural blocking size of the malloc implementation.
      // Unfortunately, we would need a somewhat low-level calculation
      // with tuned parameters to get this perfect for any particular
      // malloc implementation.  Fortunately, generalizations about
      // common features seen among implementations seems to suffice.

      // __pagesize need not match the actual VM page size for good
      // results in practice, thus we pick a common value on the low
      // side.  __malloc_header_size is an estimate of the amount of
      // overhead per memory allocation (in practice seen N * sizeof
      // (void*) where N is 0, 2 or 4).  According to folklore,
      // picking this value on the high side is better than
      // low-balling it (especially when this algorithm is used with
      // malloc implementations that allocate memory blocks rounded up
      // to a size which is a power of 2).
      const size_type __pagesize = 4096;
      const size_type __malloc_header_size = 4 * sizeof(void*);

      // The below implements an exponential growth policy, necessary to
      // meet amortized linear time requirements of the library: see
      // http://gcc.gnu.org/ml/libstdc++/2001-07/msg00085.html.
      // It's active for allocations requiring an amount of memory above
      // system pagesize. This is consistent with the requirements of the
      // standard: http://gcc.gnu.org/ml/libstdc++/2001-07/msg00130.html
      if (__capacity > __old_capacity && __capacity < 2 * __old_capacity)
	__capacity = 2 * __old_capacity;

      // NB: Need an array of char_type[__capacity], plus a terminating
      // null char_type() element, plus enough for the _Rep data structure.
      // Whew. Seemingly so needy, yet so elemental.
      size_type __size = (__capacity + 1) * sizeof(_CharT) + sizeof(_Rep);

      const size_type __adj_size = __size + __malloc_header_size;
      if (__adj_size > __pagesize && __capacity > __old_capacity)
	{
	  const size_type __extra = __pagesize - __adj_size % __pagesize;
	  __capacity += __extra / sizeof(_CharT);
	  // Never allocate a string bigger than _S_max_size.
	  if (__capacity > _S_max_size)
	    __capacity = _S_max_size;
	  __size = (__capacity + 1) * sizeof(_CharT) + sizeof(_Rep);
	}

      // NB: Might throw, but no worries about a leak, mate: _Rep()
      // does not throw.
      void* __place = _Raw_bytes_alloc(__alloc).allocate(__size);
      _Rep *__p = new (__place) _Rep;
      __p->_M_capacity = __capacity;
      // ABI compatibility - 3.4.x set in _S_create both
      // _M_refcount and _M_length.  All callers of _S_create
      // in basic_string.tcc then set just _M_length.
      // In 4.0.x and later both _M_refcount and _M_length
      // are initialized in the callers, unfortunately we can
      // have 3.4.x compiled code with _S_create callers inlined
      // calling 4.0.x+ _S_create.
      __p->_M_set_sharable();
      return __p;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    _CharT*
    basic_string<_CharT, _Traits, _Alloc>::_Rep::
    _M_clone(const _Alloc& __alloc, size_type __res)
    {
      // Requested capacity of the clone.
      const size_type __requested_cap = this->_M_length + __res;
      _Rep* __r = _Rep::_S_create(__requested_cap, this->_M_capacity,
				  __alloc);
      if (this->_M_length)
	_M_copy(__r->_M_refdata(), _M_refdata(), this->_M_length);

      __r->_M_set_length_and_sharable(this->_M_length);
      return __r->_M_refdata();
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    resize(size_type __n, _CharT __c)
    {
      const size_type __size = this->size();
      _M_check_length(__size, __n, "basic_string::resize");
      if (__size < __n)
	this->append(__n - __size, __c);
      else if (__n < __size)
	this->erase(__n);
      // else nothing (in particular, avoid calling _M_mutate() unnecessarily.)
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    template<typename _InputIterator>
      basic_string<_CharT, _Traits, _Alloc>&
      basic_string<_CharT, _Traits, _Alloc>::
      _M_replace_dispatch(iterator __i1, iterator __i2, _InputIterator __k1,
			  _InputIterator __k2, __false_type)
      {
	const basic_string __s(__k1, __k2);
	const size_type __n1 = __i2 - __i1;
	_M_check_length(__n1, __s.size(), "basic_string::_M_replace_dispatch");
	return _M_replace_safe(__i1 - _M_ibegin(), __n1, __s._M_data(),
			       __s.size());
      }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    _M_replace_aux(size_type __pos1, size_type __n1, size_type __n2,
		   _CharT __c)
    {
      _M_check_length(__n1, __n2, "basic_string::_M_replace_aux");
      _M_mutate(__pos1, __n1, __n2);
      if (__n2)
	_M_assign(_M_data() + __pos1, __n2, __c);
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    _M_replace_safe(size_type __pos1, size_type __n1, const _CharT* __s,
		    size_type __n2)
    {
      _M_mutate(__pos1, __n1, __n2);
      if (__n2)
	_M_copy(_M_data() + __pos1, __s, __n2);
      return *this;
    }
   
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>
    operator+(const _CharT* __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    {
      __glibcxx_requires_string(__lhs);
      typedef basic_string<_CharT, _Traits, _Alloc> __string_type;
      typedef typename __string_type::size_type	  __size_type;
      const __size_type __len = _Traits::length(__lhs);
      __string_type __str;
      __str.reserve(__len + __rhs.size());
      __str.append(__lhs, __len);
      __str.append(__rhs);
      return __str;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>
    operator+(_CharT __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    {
      typedef basic_string<_CharT, _Traits, _Alloc> __string_type;
      typedef typename __string_type::size_type	  __size_type;
      __string_type __str;
      const __size_type __len = __rhs.size();
      __str.reserve(__len + 1);
      __str.append(__size_type(1), __lhs);
      __str.append(__rhs);
      return __str;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    copy(_CharT* __s, size_type __n, size_type __pos) const
    {
      _M_check(__pos, "basic_string::copy");
      __n = _M_limit(__pos, __n);
      __glibcxx_requires_string_len(__s, __n);
      if (__n)
	_M_copy(__s, _M_data() + __pos, __n);
      // 21.3.5.7 par 3: do not append null.  (good.)
      return __n;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find(const _CharT* __s, size_type __pos, size_type __n) const
    {
      __glibcxx_requires_string_len(__s, __n);
      const size_type __size = this->size();
      const _CharT* __data = _M_data();

      if (__n == 0)
	return __pos <= __size ? __pos : npos;

      if (__n <= __size)
	{
	  for (; __pos <= __size - __n; ++__pos)
	    if (traits_type::eq(__data[__pos], __s[0])
		&& traits_type::compare(__data + __pos + 1,
					__s + 1, __n - 1) == 0)
	      return __pos;
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find(_CharT __c, size_type __pos) const _GLIBCXX_NOEXCEPT
    {
      size_type __ret = npos;
      const size_type __size = this->size();
      if (__pos < __size)
	{
	  const _CharT* __data = _M_data();
	  const size_type __n = __size - __pos;
	  const _CharT* __p = traits_type::find(__data + __pos, __n, __c);
	  if (__p)
	    __ret = __p - __data;
	}
      return __ret;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    rfind(const _CharT* __s, size_type __pos, size_type __n) const
    {
      __glibcxx_requires_string_len(__s, __n);
      const size_type __size = this->size();
      if (__n <= __size)
	{
	  __pos = std::min(size_type(__size - __n), __pos);
	  const _CharT* __data = _M_data();
	  do
	    {
	      if (traits_type::compare(__data + __pos, __s, __n) == 0)
		return __pos;
	    }
	  while (__pos-- > 0);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    rfind(_CharT __c, size_type __pos) const _GLIBCXX_NOEXCEPT
    {
      size_type __size = this->size();
      if (__size)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  for (++__size; __size-- > 0; )
	    if (traits_type::eq(_M_data()[__size], __c))
	      return __size;
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_first_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      __glibcxx_requires_string_len(__s, __n);
      for (; __n && __pos < this->size(); ++__pos)
	{
	  const _CharT* __p = traits_type::find(__s, __n, _M_data()[__pos]);
	  if (__p)
	    return __pos;
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_last_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      __glibcxx_requires_string_len(__s, __n);
      size_type __size = this->size();
      if (__size && __n)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  do
	    {
	      if (traits_type::find(__s, __n, _M_data()[__size]))
		return __size;
	    }
	  while (__size-- != 0);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      __glibcxx_requires_string_len(__s, __n);
      for (; __pos < this->size(); ++__pos)
	if (!traits_type::find(__s, __n, _M_data()[__pos]))
	  return __pos;
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_first_not_of(_CharT __c, size_type __pos) const _GLIBCXX_NOEXCEPT
    {
      for (; __pos < this->size(); ++__pos)
	if (!traits_type::eq(_M_data()[__pos], __c))
	  return __pos;
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      __glibcxx_requires_string_len(__s, __n);
      size_type __size = this->size();
      if (__size)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  do
	    {
	      if (!traits_type::find(__s, __n, _M_data()[__size]))
		return __size;
	    }
	  while (__size--);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_last_not_of(_CharT __c, size_type __pos) const _GLIBCXX_NOEXCEPT
    {
      size_type __size = this->size();
      if (__size)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  do
	    {
	      if (!traits_type::eq(_M_data()[__size], __c))
		return __size;
	    }
	  while (__size--);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string<_CharT, _Traits, _Alloc>::
    compare(size_type __pos, size_type __n, const basic_string& __str) const
    {
      _M_check(__pos, "basic_string::compare");
      __n = _M_limit(__pos, __n);
      const size_type __osize = __str.size();
      const size_type __len = std::min(__n, __osize);
      int __r = traits_type::compare(_M_data() + __pos, __str.data(), __len);
      if (!__r)
	__r = _S_compare(__n, __osize);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string<_CharT, _Traits, _Alloc>::
    compare(size_type __pos1, size_type __n1, const basic_string& __str,
	    size_type __pos2, size_type __n2) const
    {
      _M_check(__pos1, "basic_string::compare");
      __str._M_check(__pos2, "basic_string::compare");
      __n1 = _M_limit(__pos1, __n1);
      __n2 = __str._M_limit(__pos2, __n2);
      const size_type __len = std::min(__n1, __n2);
      int __r = traits_type::compare(_M_data() + __pos1,
				     __str.data() + __pos2, __len);
      if (!__r)
	__r = _S_compare(__n1, __n2);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string<_CharT, _Traits, _Alloc>::
    compare(const _CharT* __s) const
    {
      __glibcxx_requires_string(__s);
      const size_type __size = this->size();
      const size_type __osize = traits_type::length(__s);
      const size_type __len = std::min(__size, __osize);
      int __r = traits_type::compare(_M_data(), __s, __len);
      if (!__r)
	__r = _S_compare(__size, __osize);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string <_CharT, _Traits, _Alloc>::
    compare(size_type __pos, size_type __n1, const _CharT* __s) const
    {
      __glibcxx_requires_string(__s);
      _M_check(__pos, "basic_string::compare");
      __n1 = _M_limit(__pos, __n1);
      const size_type __osize = traits_type::length(__s);
      const size_type __len = std::min(__n1, __osize);
      int __r = traits_type::compare(_M_data() + __pos, __s, __len);
      if (!__r)
	__r = _S_compare(__n1, __osize);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string <_CharT, _Traits, _Alloc>::
    compare(size_type __pos, size_type __n1, const _CharT* __s,
	    size_type __n2) const
    {
      __glibcxx_requires_string_len(__s, __n2);
      _M_check(__pos, "basic_string::compare");
      __n1 = _M_limit(__pos, __n1);
      const size_type __len = std::min(__n1, __n2);
      int __r = traits_type::compare(_M_data() + __pos, __s, __len);
      if (!__r)
	__r = _S_compare(__n1, __n2);
      return __r;
    }

  // 21.3.7.9 basic_string::getline and operators
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __in,
	       basic_string<_CharT, _Traits, _Alloc>& __str)
    {
      typedef basic_istream<_CharT, _Traits>		__istream_type;
      typedef basic_string<_CharT, _Traits, _Alloc>	__string_type;
      typedef typename __istream_type::ios_base         __ios_base;
      typedef typename __istream_type::int_type		__int_type;
      typedef typename __string_type::size_type		__size_type;
      typedef ctype<_CharT>				__ctype_type;
      typedef typename __ctype_type::ctype_base         __ctype_base;

      __size_type __extracted = 0;
      typename __ios_base::iostate __err = __ios_base::goodbit;
      typename __istream_type::sentry __cerb(__in, false);
      if (__cerb)
	{
	  __try
	    {
	      // Avoid reallocation for common case.
	      __str.erase();
	      _CharT __buf[128];
	      __size_type __len = 0;	      
	      const streamsize __w = __in.width();
	      const __size_type __n = __w > 0 ? static_cast<__size_type>(__w)
		                              : __str.max_size();
	      const __ctype_type& __ct = use_facet<__ctype_type>(__in.getloc());
	      const __int_type __eof = _Traits::eof();
	      __int_type __c = __in.rdbuf()->sgetc();

	      while (__extracted < __n
		     && !_Traits::eq_int_type(__c, __eof)
		     && !__ct.is(__ctype_base::space,
				 _Traits::to_char_type(__c)))
		{
		  if (__len == sizeof(__buf) / sizeof(_CharT))
		    {
		      __str.append(__buf, sizeof(__buf) / sizeof(_CharT));
		      __len = 0;
		    }
		  __buf[__len++] = _Traits::to_char_type(__c);
		  ++__extracted;
		  __c = __in.rdbuf()->snextc();
		}
	      __str.append(__buf, __len);

	      if (_Traits::eq_int_type(__c, __eof))
		__err |= __ios_base::eofbit;
	      __in.width(0);
	    }
	  __catch(__cxxabiv1::__forced_unwind&)
	    {
	      __in._M_setstate(__ios_base::badbit);
	      __throw_exception_again;
	    }
	  __catch(...)
	    {
	      // _GLIBCXX_RESOLVE_LIB_DEFECTS
	      // 91. Description of operator>> and getline() for string<>
	      // might cause endless loop
	      __in._M_setstate(__ios_base::badbit);
	    }
	}
      // 211.  operator>>(istream&, string&) doesn't set failbit
      if (!__extracted)
	__err |= __ios_base::failbit;
      if (__err)
	__in.setstate(__err);
      return __in;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    getline(basic_istream<_CharT, _Traits>& __in,
	    basic_string<_CharT, _Traits, _Alloc>& __str, _CharT __delim)
    {
      typedef basic_istream<_CharT, _Traits>		__istream_type;
      typedef basic_string<_CharT, _Traits, _Alloc>	__string_type;
      typedef typename __istream_type::ios_base         __ios_base;
      typedef typename __istream_type::int_type		__int_type;
      typedef typename __string_type::size_type		__size_type;

      __size_type __extracted = 0;
      const __size_type __n = __str.max_size();
      typename __ios_base::iostate __err = __ios_base::goodbit;
      typename __istream_type::sentry __cerb(__in, true);
      if (__cerb)
	{
	  __try
	    {
	      __str.erase();
	      const __int_type __idelim = _Traits::to_int_type(__delim);
	      const __int_type __eof = _Traits::eof();
	      __int_type __c = __in.rdbuf()->sgetc();

	      while (__extracted < __n
		     && !_Traits::eq_int_type(__c, __eof)
		     && !_Traits::eq_int_type(__c, __idelim))
		{
		  __str += _Traits::to_char_type(__c);
		  ++__extracted;
		  __c = __in.rdbuf()->snextc();
		}

	      if (_Traits::eq_int_type(__c, __eof))
		__err |= __ios_base::eofbit;
	      else if (_Traits::eq_int_type(__c, __idelim))
		{
		  ++__extracted;		  
		  __in.rdbuf()->sbumpc();
		}
	      else
		__err |= __ios_base::failbit;
	    }
	  __catch(__cxxabiv1::__forced_unwind&)
	    {
	      __in._M_setstate(__ios_base::badbit);
	      __throw_exception_again;
	    }
	  __catch(...)
	    {
	      // _GLIBCXX_RESOLVE_LIB_DEFECTS
	      // 91. Description of operator>> and getline() for string<>
	      // might cause endless loop
	      __in._M_setstate(__ios_base::badbit);
	    }
	}
      if (!__extracted)
	__err |= __ios_base::failbit;
      if (__err)
	__in.setstate(__err);
      return __in;
    }

  // Inhibit implicit instantiations for required instantiations,
  // which are defined via explicit instantiations elsewhere.
#if _GLIBCXX_EXTERN_TEMPLATE > 0
  extern template class basic_string<char>;
  extern template
    basic_istream<char>&
    operator>>(basic_istream<char>&, string&);
  extern template
    basic_ostream<char>&
    operator<<(basic_ostream<char>&, const string&);
  extern template
    basic_istream<char>&
    getline(basic_istream<char>&, string&, char);
  extern template
    basic_istream<char>&
    getline(basic_istream<char>&, string&);

#ifdef _GLIBCXX_USE_WCHAR_T
  extern template class basic_string<wchar_t>;
  extern template
    basic_istream<wchar_t>&
    operator>>(basic_istream<wchar_t>&, wstring&);
  extern template
    basic_ostream<wchar_t>&
    operator<<(basic_ostream<wchar_t>&, const wstring&);
  extern template
    basic_istream<wchar_t>&
    getline(basic_istream<wchar_t>&, wstring&, wchar_t);
  extern template
    basic_istream<wchar_t>&
    getline(basic_istream<wchar_t>&, wstring&);
#endif
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif
