// -*- C++ -*- header.

// Copyright (C) 2008, 2009
// Free Software Foundation, Inc.
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

/** @file bits/atomic_2.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _GLIBCXX_ATOMIC_2_H
#define _GLIBCXX_ATOMIC_2_H 1

#pragma GCC system_header

// _GLIBCXX_BEGIN_NAMESPACE(std)

// 2 == __atomic2 == Always lock-free
// Assumed:
// _GLIBCXX_ATOMIC_BUILTINS_1
// _GLIBCXX_ATOMIC_BUILTINS_2
// _GLIBCXX_ATOMIC_BUILTINS_4
// _GLIBCXX_ATOMIC_BUILTINS_8
namespace __atomic2
{
  /// atomic_flag
  struct atomic_flag : public __atomic_flag_base
  {
    atomic_flag() = default;
    ~atomic_flag() = default;
    atomic_flag(const atomic_flag&) = delete;
    atomic_flag& operator=(const atomic_flag&) volatile = delete;

    // Conversion to ATOMIC_FLAG_INIT.
    atomic_flag(bool __i): __atomic_flag_base({ __i }) { }

    bool
    test_and_set(memory_order __m = memory_order_seq_cst)
    {
      // Redundant synchronize if built-in for lock is a full barrier.
      if (__m != memory_order_acquire && __m != memory_order_acq_rel)
	__sync_synchronize();
      return __sync_lock_test_and_set(&_M_i, 1);
    }

    void
    clear(memory_order __m = memory_order_seq_cst)
    {
      __glibcxx_assert(__m != memory_order_consume);
      __glibcxx_assert(__m != memory_order_acquire);
      __glibcxx_assert(__m != memory_order_acq_rel);

      __sync_lock_release(&_M_i);
      if (__m != memory_order_acquire && __m != memory_order_acq_rel)
	__sync_synchronize();
    }
  };


  /// 29.4.2, address types
  struct atomic_address
  {
  private:
    void* _M_i;

  public:
    atomic_address() = default;
    ~atomic_address() = default;
    atomic_address(const atomic_address&) = delete;
    atomic_address& operator=(const atomic_address&) volatile = delete;

    atomic_address(void* __v) { _M_i = __v; }

    bool
    is_lock_free() const
    { return true; }

    void
    store(void* __v, memory_order __m = memory_order_seq_cst)
    {
      __glibcxx_assert(__m != memory_order_acquire);
      __glibcxx_assert(__m != memory_order_acq_rel);
      __glibcxx_assert(__m != memory_order_consume);

      if (__m == memory_order_relaxed)
	_M_i = __v;
      else
	{
	  // write_mem_barrier();
	  _M_i = __v;
	  if (__m == memory_order_seq_cst)
	    __sync_synchronize();
	}
    }

    void*
    load(memory_order __m = memory_order_seq_cst) const
    {
      __glibcxx_assert(__m != memory_order_release);
      __glibcxx_assert(__m != memory_order_acq_rel);

      __sync_synchronize();
      void* __ret = _M_i;
      __sync_synchronize();
      return __ret;
    }

    void*
    exchange(void* __v, memory_order __m = memory_order_seq_cst)
    {
      // XXX built-in assumes memory_order_acquire.
      return __sync_lock_test_and_set(&_M_i, __v);
    }

    bool
    compare_exchange_weak(void*& __v1, void* __v2, memory_order __m1,
			  memory_order __m2)
    { return compare_exchange_strong(__v1, __v2, __m1, __m2); }

    bool
    compare_exchange_weak(void*& __v1, void* __v2,
			  memory_order __m = memory_order_seq_cst)
    {
      return compare_exchange_weak(__v1, __v2, __m,
				   __calculate_memory_order(__m));
    }

    bool
    compare_exchange_strong(void*& __v1, void* __v2, memory_order __m1,
			    memory_order __m2)
    {
      __glibcxx_assert(__m2 != memory_order_release);
      __glibcxx_assert(__m2 != memory_order_acq_rel);
      __glibcxx_assert(__m2 <= __m1);

      void* __v1o = __v1;
      void* __v1n = __sync_val_compare_and_swap(&_M_i, __v1o, __v2);

      // Assume extra stores (of same value) allowed in true case.
      __v1 = __v1n;
      return __v1o == __v1n;
    }

    bool
    compare_exchange_strong(void*& __v1, void* __v2,
			  memory_order __m = memory_order_seq_cst)
    {
      return compare_exchange_strong(__v1, __v2, __m,
				     __calculate_memory_order(__m));
    }

    void*
    fetch_add(ptrdiff_t __d, memory_order __m = memory_order_seq_cst)
    { return __sync_fetch_and_add(&_M_i, __d); }

    void*
    fetch_sub(ptrdiff_t __d, memory_order __m = memory_order_seq_cst)
    { return __sync_fetch_and_sub(&_M_i, __d); }

    operator void*() const
    { return load(); }

    void*
    operator=(void* __v)
    {
      store(__v);
      return __v;
    }

    void*
    operator+=(ptrdiff_t __d)
    { return __sync_add_and_fetch(&_M_i, __d); }

    void*
    operator-=(ptrdiff_t __d)
    { return __sync_sub_and_fetch(&_M_i, __d); }
  };

  // 29.3.1 atomic integral types
  // For each of the integral types, define atomic_[integral type] struct
  //
  // atomic_bool     bool
  // atomic_char     char
  // atomic_schar    signed char
  // atomic_uchar    unsigned char
  // atomic_short    short
  // atomic_ushort   unsigned short
  // atomic_int      int
  // atomic_uint     unsigned int
  // atomic_long     long
  // atomic_ulong    unsigned long
  // atomic_llong    long long
  // atomic_ullong   unsigned long long
  // atomic_char16_t char16_t
  // atomic_char32_t char32_t
  // atomic_wchar_t  wchar_t

  // Base type.
  // NB: Assuming _ITp is an integral scalar type that is 1, 2, 4, or 8 bytes,
  // since that is what GCC built-in functions for atomic memory access work on.
  template<typename _ITp>
    struct __atomic_base
    {
    private:
      typedef _ITp 	__integral_type;

      __integral_type 	_M_i;

    public:
      __atomic_base() = default;
      ~__atomic_base() = default;
      __atomic_base(const __atomic_base&) = delete;
      __atomic_base& operator=(const __atomic_base&) volatile = delete;

      // Requires __integral_type convertible to _M_base._M_i.
      __atomic_base(__integral_type __i) { _M_i = __i; }

      operator __integral_type() const
      { return load(); }

      __integral_type
      operator=(__integral_type __i)
      {
	store(__i);
	return __i;
      }

      __integral_type
      operator++(int)
      { return fetch_add(1); }

      __integral_type
      operator--(int)
      { return fetch_sub(1); }

      __integral_type
      operator++()
      { return __sync_add_and_fetch(&_M_i, 1); }

      __integral_type
      operator--()
      { return __sync_sub_and_fetch(&_M_i, 1); }

      __integral_type
      operator+=(__integral_type __i)
      { return __sync_add_and_fetch(&_M_i, __i); }

      __integral_type
      operator-=(__integral_type __i)
      { return __sync_sub_and_fetch(&_M_i, __i); }

      __integral_type
      operator&=(__integral_type __i)
      { return __sync_and_and_fetch(&_M_i, __i); }

      __integral_type
      operator|=(__integral_type __i)
      { return __sync_or_and_fetch(&_M_i, __i); }

      __integral_type
      operator^=(__integral_type __i)
      { return __sync_xor_and_fetch(&_M_i, __i); }

      bool
      is_lock_free() const
      { return true; }

      void
      store(__integral_type __i, memory_order __m = memory_order_seq_cst)
      {
	__glibcxx_assert(__m != memory_order_acquire);
	__glibcxx_assert(__m != memory_order_acq_rel);
	__glibcxx_assert(__m != memory_order_consume);

	if (__m == memory_order_relaxed)
	  _M_i = __i;
	else
	  {
	    // write_mem_barrier();
	    _M_i = __i;
	    if (__m == memory_order_seq_cst)
	      __sync_synchronize();
	  }
      }

      __integral_type
      load(memory_order __m = memory_order_seq_cst) const 
      {
	__glibcxx_assert(__m != memory_order_release);
	__glibcxx_assert(__m != memory_order_acq_rel);

	__sync_synchronize();
	__integral_type __ret = _M_i;
	__sync_synchronize();
	return __ret;
      }

      __integral_type
      exchange(__integral_type __i, memory_order __m = memory_order_seq_cst)
      {
	// XXX built-in assumes memory_order_acquire.
	return __sync_lock_test_and_set(&_M_i, __i);
      }

      bool
      compare_exchange_weak(__integral_type& __i1, __integral_type __i2,
			    memory_order __m1, memory_order __m2)
      { return compare_exchange_strong(__i1, __i2, __m1, __m2); }

      bool
      compare_exchange_weak(__integral_type& __i1, __integral_type __i2,
			    memory_order __m = memory_order_seq_cst)
      {
	return compare_exchange_weak(__i1, __i2, __m,
				     __calculate_memory_order(__m));
      }

      bool
      compare_exchange_strong(__integral_type& __i1, __integral_type __i2,
			      memory_order __m1, memory_order __m2) 
      {
	__glibcxx_assert(__m2 != memory_order_release);
	__glibcxx_assert(__m2 != memory_order_acq_rel);
	__glibcxx_assert(__m2 <= __m1);

	__integral_type __i1o = __i1;
	__integral_type __i1n = __sync_val_compare_and_swap(&_M_i, __i1o, __i2);

	// Assume extra stores (of same value) allowed in true case.
	__i1 = __i1n;
	return __i1o == __i1n;
      }

      bool
      compare_exchange_strong(__integral_type& __i1, __integral_type __i2,
			      memory_order __m = memory_order_seq_cst)
      {
	return compare_exchange_strong(__i1, __i2, __m,
				       __calculate_memory_order(__m));
      }

      __integral_type
      fetch_add(__integral_type __i,
		memory_order __m = memory_order_seq_cst)
      { return __sync_fetch_and_add(&_M_i, __i); }

      __integral_type
      fetch_sub(__integral_type __i,
		memory_order __m = memory_order_seq_cst) 
      { return __sync_fetch_and_sub(&_M_i, __i); }

      __integral_type
      fetch_and(__integral_type __i,
		memory_order __m = memory_order_seq_cst) 
      { return __sync_fetch_and_and(&_M_i, __i); }

      __integral_type
      fetch_or(__integral_type __i,
	       memory_order __m = memory_order_seq_cst) 
      { return __sync_fetch_and_or(&_M_i, __i); }

      __integral_type
      fetch_xor(__integral_type __i,
		memory_order __m = memory_order_seq_cst)
      { return __sync_fetch_and_xor(&_M_i, __i); }
    };


  /// atomic_bool
  // NB: No operators or fetch-operations for this type.
  struct atomic_bool
  {
  private:
    __atomic_base<bool>	_M_base;

  public:
    atomic_bool() = default;
    ~atomic_bool() = default;
    atomic_bool(const atomic_bool&) = delete;
    atomic_bool& operator=(const atomic_bool&) volatile = delete;

    atomic_bool(bool __i) : _M_base(__i) { }

    bool
    operator=(bool __i)
    { return _M_base.operator=(__i); }

    operator bool() const 
    { return _M_base.load(); }

    bool
    is_lock_free() const
    { return _M_base.is_lock_free(); }

    void
    store(bool __i, memory_order __m = memory_order_seq_cst)
    { _M_base.store(__i, __m); }

    bool
    load(memory_order __m = memory_order_seq_cst) const
    { return _M_base.load(__m); }

    bool
    exchange(bool __i, memory_order __m = memory_order_seq_cst)
    { return _M_base.exchange(__i, __m); }

    bool
    compare_exchange_weak(bool& __i1, bool __i2, memory_order __m1,
			  memory_order __m2)
    { return _M_base.compare_exchange_weak(__i1, __i2, __m1, __m2); }

    bool
    compare_exchange_weak(bool& __i1, bool __i2,
			  memory_order __m = memory_order_seq_cst)
    { return _M_base.compare_exchange_weak(__i1, __i2, __m); }

    bool
    compare_exchange_strong(bool& __i1, bool __i2, memory_order __m1,
			    memory_order __m2)
    { return _M_base.compare_exchange_strong(__i1, __i2, __m1, __m2); }


    bool
    compare_exchange_strong(bool& __i1, bool __i2,
			    memory_order __m = memory_order_seq_cst)
    { return _M_base.compare_exchange_strong(__i1, __i2, __m); }
  };
} // namespace __atomic2

// _GLIBCXX_END_NAMESPACE

#endif
