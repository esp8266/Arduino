// Random number extensions -*- C++ -*-

// Copyright (C) 2012-2013 Free Software Foundation, Inc.
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

/** @file ext/random.tcc
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{ext/random}
 */

#ifndef _EXT_RANDOM_TCC
#define _EXT_RANDOM_TCC 1

#pragma GCC system_header


namespace __gnu_cxx _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4>
    void simd_fast_mersenne_twister_engine<_UIntType, __m,
					   __pos1, __sl1, __sl2, __sr1, __sr2,
					   __msk1, __msk2, __msk3, __msk4,
					   __parity1, __parity2, __parity3,
					   __parity4>::
    seed(_UIntType __seed)
    {
      _M_state32[0] = static_cast<uint32_t>(__seed);
      for (size_t __i = 1; __i < _M_nstate32; ++__i)
	_M_state32[__i] = (1812433253UL
			   * (_M_state32[__i - 1] ^ (_M_state32[__i - 1] >> 30))
			   + __i);
      _M_pos = state_size;
      _M_period_certification();
    }


  namespace {

    inline uint32_t _Func1(uint32_t __x)
    {
      return (__x ^ (__x >> 27)) * UINT32_C(1664525);
    }

    inline uint32_t _Func2(uint32_t __x)
    {
      return (__x ^ (__x >> 27)) * UINT32_C(1566083941);
    }

  }


  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4>
    template<typename _Sseq>
      typename std::enable_if<std::is_class<_Sseq>::value>::type
      simd_fast_mersenne_twister_engine<_UIntType, __m,
					__pos1, __sl1, __sl2, __sr1, __sr2,
					__msk1, __msk2, __msk3, __msk4,
					__parity1, __parity2, __parity3,
					__parity4>::
      seed(_Sseq& __q)
      {
	size_t __lag;

	if (_M_nstate32 >= 623)
	  __lag = 11;
	else if (_M_nstate32 >= 68)
	  __lag = 7;
	else if (_M_nstate32 >= 39)
	  __lag = 5;
	else
	  __lag = 3;
	const size_t __mid = (_M_nstate32 - __lag) / 2;

	std::fill(_M_state32, _M_state32 + _M_nstate32, UINT32_C(0x8b8b8b8b));
	uint32_t __arr[_M_nstate32];
	__q.generate(__arr + 0, __arr + _M_nstate32);

	uint32_t __r = _Func1(_M_state32[0] ^ _M_state32[__mid]
			      ^ _M_state32[_M_nstate32  - 1]);
	_M_state32[__mid] += __r;
	__r += _M_nstate32;
	_M_state32[__mid + __lag] += __r;
	_M_state32[0] = __r;

	for (size_t __i = 1, __j = 0; __j < _M_nstate32; ++__j)
	  {
	    __r = _Func1(_M_state32[__i]
			 ^ _M_state32[(__i + __mid) % _M_nstate32]
			 ^ _M_state32[(__i + _M_nstate32 - 1) % _M_nstate32]);
	    _M_state32[(__i + __mid) % _M_nstate32] += __r;
	    __r += __arr[__j] + __i;
	    _M_state32[(__i + __mid + __lag) % _M_nstate32] += __r;
	    _M_state32[__i] = __r;
	    __i = (__i + 1) % _M_nstate32;
	  }
	for (size_t __j = 0; __j < _M_nstate32; ++__j)
	  {
	    const size_t __i = (__j + 1) % _M_nstate32;
	    __r = _Func2(_M_state32[__i]
			 + _M_state32[(__i + __mid) % _M_nstate32]
			 + _M_state32[(__i + _M_nstate32 - 1) % _M_nstate32]);
	    _M_state32[(__i + __mid) % _M_nstate32] ^= __r;
	    __r -= __i;
	    _M_state32[(__i + __mid + __lag) % _M_nstate32] ^= __r;
	    _M_state32[__i] = __r;
	  }

	_M_pos = state_size;
	_M_period_certification();
      }


  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4>
    void simd_fast_mersenne_twister_engine<_UIntType, __m,
					   __pos1, __sl1, __sl2, __sr1, __sr2,
					   __msk1, __msk2, __msk3, __msk4,
					   __parity1, __parity2, __parity3,
					   __parity4>::
    _M_period_certification(void)
    {
      static const uint32_t __parity[4] = { __parity1, __parity2,
					    __parity3, __parity4 };
      uint32_t __inner = 0;
      for (size_t __i = 0; __i < 4; ++__i)
	if (__parity[__i] != 0)
	  __inner ^= _M_state32[__i] & __parity[__i];

      if (__builtin_parity(__inner) & 1)
	return;
      for (size_t __i = 0; __i < 4; ++__i)
	if (__parity[__i] != 0)
	  {
	    _M_state32[__i] ^= 1 << (__builtin_ffs(__parity[__i]) - 1);
	    return;
	  }
      __builtin_unreachable();
    }


  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4>
    void simd_fast_mersenne_twister_engine<_UIntType, __m,
					   __pos1, __sl1, __sl2, __sr1, __sr2,
					   __msk1, __msk2, __msk3, __msk4,
					   __parity1, __parity2, __parity3,
					   __parity4>::
    discard(unsigned long long __z)
    {
      while (__z > state_size - _M_pos)
	{
	  __z -= state_size - _M_pos;

	  _M_gen_rand();
	}

      _M_pos += __z;
    }


#ifndef  _GLIBCXX_OPT_HAVE_RANDOM_SFMT_GEN_READ

  namespace {

    template<size_t __shift>
      inline void __rshift(uint32_t *__out, const uint32_t *__in)
      {
	uint64_t __th = ((static_cast<uint64_t>(__in[3]) << 32)
			 | static_cast<uint64_t>(__in[2]));
	uint64_t __tl = ((static_cast<uint64_t>(__in[1]) << 32)
			 | static_cast<uint64_t>(__in[0]));

	uint64_t __oh = __th >> (__shift * 8);
	uint64_t __ol = __tl >> (__shift * 8);
	__ol |= __th << (64 - __shift * 8);
	__out[1] = static_cast<uint32_t>(__ol >> 32);
	__out[0] = static_cast<uint32_t>(__ol);
	__out[3] = static_cast<uint32_t>(__oh >> 32);
	__out[2] = static_cast<uint32_t>(__oh);
      }


    template<size_t __shift>
      inline void __lshift(uint32_t *__out, const uint32_t *__in)
      {
	uint64_t __th = ((static_cast<uint64_t>(__in[3]) << 32)
			 | static_cast<uint64_t>(__in[2]));
	uint64_t __tl = ((static_cast<uint64_t>(__in[1]) << 32)
			 | static_cast<uint64_t>(__in[0]));

	uint64_t __oh = __th << (__shift * 8);
	uint64_t __ol = __tl << (__shift * 8);
	__oh |= __tl >> (64 - __shift * 8);
	__out[1] = static_cast<uint32_t>(__ol >> 32);
	__out[0] = static_cast<uint32_t>(__ol);
	__out[3] = static_cast<uint32_t>(__oh >> 32);
	__out[2] = static_cast<uint32_t>(__oh);
      }


    template<size_t __sl1, size_t __sl2, size_t __sr1, size_t __sr2,
	     uint32_t __msk1, uint32_t __msk2, uint32_t __msk3, uint32_t __msk4>
      inline void __recursion(uint32_t *__r,
			      const uint32_t *__a, const uint32_t *__b,
			      const uint32_t *__c, const uint32_t *__d)
      {
	uint32_t __x[4];
	uint32_t __y[4];

	__lshift<__sl2>(__x, __a);
	__rshift<__sr2>(__y, __c);
	__r[0] = (__a[0] ^ __x[0] ^ ((__b[0] >> __sr1) & __msk1)
		  ^ __y[0] ^ (__d[0] << __sl1));
	__r[1] = (__a[1] ^ __x[1] ^ ((__b[1] >> __sr1) & __msk2)
		  ^ __y[1] ^ (__d[1] << __sl1));
	__r[2] = (__a[2] ^ __x[2] ^ ((__b[2] >> __sr1) & __msk3)
		  ^ __y[2] ^ (__d[2] << __sl1));
	__r[3] = (__a[3] ^ __x[3] ^ ((__b[3] >> __sr1) & __msk4)
		  ^ __y[3] ^ (__d[3] << __sl1));
      }

  }


  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4>
    void simd_fast_mersenne_twister_engine<_UIntType, __m,
					   __pos1, __sl1, __sl2, __sr1, __sr2,
					   __msk1, __msk2, __msk3, __msk4,
					   __parity1, __parity2, __parity3,
					   __parity4>::
    _M_gen_rand(void)
    {
      const uint32_t *__r1 = &_M_state32[_M_nstate32 - 8];
      const uint32_t *__r2 = &_M_state32[_M_nstate32 - 4];
      static constexpr size_t __pos1_32 = __pos1 * 4;

      size_t __i;
      for (__i = 0; __i < _M_nstate32 - __pos1_32; __i += 4)
	{
	  __recursion<__sl1, __sl2, __sr1, __sr2,
		      __msk1, __msk2, __msk3, __msk4>
	    (&_M_state32[__i], &_M_state32[__i],
	     &_M_state32[__i + __pos1_32], __r1, __r2);
	  __r1 = __r2;
	  __r2 = &_M_state32[__i];
	}

      for (; __i < _M_nstate32; __i += 4)
	{
	  __recursion<__sl1, __sl2, __sr1, __sr2,
		      __msk1, __msk2, __msk3, __msk4>
	    (&_M_state32[__i], &_M_state32[__i],
	     &_M_state32[__i + __pos1_32 - _M_nstate32], __r1, __r2);
	  __r1 = __r2;
	  __r2 = &_M_state32[__i];
	}

      _M_pos = 0;
    }

#endif

#ifndef _GLIBCXX_OPT_HAVE_RANDOM_SFMT_OPERATOREQUAL
  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4>
    bool
    operator==(const __gnu_cxx::simd_fast_mersenne_twister_engine<_UIntType,
	       __m, __pos1, __sl1, __sl2, __sr1, __sr2,
	       __msk1, __msk2, __msk3, __msk4,
	       __parity1, __parity2, __parity3, __parity4>& __lhs,
	       const __gnu_cxx::simd_fast_mersenne_twister_engine<_UIntType,
	       __m, __pos1, __sl1, __sl2, __sr1, __sr2,
	       __msk1, __msk2, __msk3, __msk4,
	       __parity1, __parity2, __parity3, __parity4>& __rhs)
    {
      typedef __gnu_cxx::simd_fast_mersenne_twister_engine<_UIntType,
	       __m, __pos1, __sl1, __sl2, __sr1, __sr2,
	       __msk1, __msk2, __msk3, __msk4,
	       __parity1, __parity2, __parity3, __parity4> __engine;
      return (std::equal(__lhs._M_stateT,
			 __lhs._M_stateT + __engine::state_size,
			 __rhs._M_stateT)
	      && __lhs._M_pos == __rhs._M_pos);
    }
#endif

  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4,
	   typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const __gnu_cxx::simd_fast_mersenne_twister_engine<_UIntType,
	       __m, __pos1, __sl1, __sl2, __sr1, __sr2,
	       __msk1, __msk2, __msk3, __msk4,
	       __parity1, __parity2, __parity3, __parity4>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits> __ostream_type;
      typedef typename __ostream_type::ios_base __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::dec | __ios_base::fixed | __ios_base::left);
      __os.fill(__space);

      for (size_t __i = 0; __i < __x._M_nstate32; ++__i)
	__os << __x._M_state32[__i] << __space;
      __os << __x._M_pos;

      __os.flags(__flags);
      __os.fill(__fill);
      return __os;
    }


  template<typename _UIntType, size_t __m,
	   size_t __pos1, size_t __sl1, size_t __sl2,
	   size_t __sr1, size_t __sr2,
	   uint32_t __msk1, uint32_t __msk2,
	   uint32_t __msk3, uint32_t __msk4,
	   uint32_t __parity1, uint32_t __parity2,
	   uint32_t __parity3, uint32_t __parity4,
	   typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       __gnu_cxx::simd_fast_mersenne_twister_engine<_UIntType,
	       __m, __pos1, __sl1, __sl2, __sr1, __sr2,
	       __msk1, __msk2, __msk3, __msk4,
	       __parity1, __parity2, __parity3, __parity4>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits> __istream_type;
      typedef typename __istream_type::ios_base __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      for (size_t __i = 0; __i < __x._M_nstate32; ++__i)
	__is >> __x._M_state32[__i];
      __is >> __x._M_pos;

      __is.flags(__flags);
      return __is;
    }

#endif // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

  /**
   * Iteration method due to M.D. J<o:>hnk.
   *
   * M.D. J<o:>hnk, Erzeugung von betaverteilten und gammaverteilten
   * Zufallszahlen, Metrika, Volume 8, 1964
   */
  template<typename _RealType>
    template<typename _UniformRandomNumberGenerator>
      typename beta_distribution<_RealType>::result_type
      beta_distribution<_RealType>::
      operator()(_UniformRandomNumberGenerator& __urng,
		 const param_type& __param)
      {
	std::__detail::_Adaptor<_UniformRandomNumberGenerator, result_type>
	  __aurng(__urng);

	result_type __x, __y;
	do
	  {
	    __x = std::exp(std::log(__aurng()) / __param.alpha());
	    __y = std::exp(std::log(__aurng()) / __param.beta());
	  }
	while (__x + __y > result_type(1));

	return __x / (__x + __y);
      }

  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      beta_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __param)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	std::__detail::_Adaptor<_UniformRandomNumberGenerator, result_type>
	  __aurng(__urng);

	while (__f != __t)
	  {
	    result_type __x, __y;
	    do
	      {
		__x = std::exp(std::log(__aurng()) / __param.alpha());
		__y = std::exp(std::log(__aurng()) / __param.beta());
	      }
	    while (__x + __y > result_type(1));

	    *__f++ = __x / (__x + __y);
	  }
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const __gnu_cxx::beta_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.alpha() << __space << __x.beta();

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       __gnu_cxx::beta_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __alpha_val, __beta_val;
      __is >> __alpha_val >> __beta_val;
      __x.param(typename __gnu_cxx::beta_distribution<_RealType>::
		param_type(__alpha_val, __beta_val));

      __is.flags(__flags);
      return __is;
    }


  template<std::size_t _Dimen, typename _RealType>
    template<typename _InputIterator1, typename _InputIterator2>
      void
      normal_mv_distribution<_Dimen, _RealType>::param_type::
      _M_init_full(_InputIterator1 __meanbegin, _InputIterator1 __meanend,
		   _InputIterator2 __varcovbegin, _InputIterator2 __varcovend)
      {
	__glibcxx_function_requires(_InputIteratorConcept<_InputIterator1>)
	__glibcxx_function_requires(_InputIteratorConcept<_InputIterator2>)
	std::fill(std::copy(__meanbegin, __meanend, _M_mean.begin()),
		  _M_mean.end(), _RealType(0));

	// Perform the Cholesky decomposition
	auto __w = _M_t.begin();
	for (size_t __j = 0; __j < _Dimen; ++__j)
	  {
	    _RealType __sum = _RealType(0);

	    auto __slitbegin = __w;
	    auto __cit = _M_t.begin();
	    for (size_t __i = 0; __i < __j; ++__i)
	      {
		auto __slit = __slitbegin;
		_RealType __s = *__varcovbegin++;
		for (size_t __k = 0; __k < __i; ++__k)
		  __s -= *__slit++ * *__cit++;

		*__w++ = __s /= *__cit++;
		__sum += __s * __s;
	      }

	    __sum = *__varcovbegin - __sum;
	    if (__builtin_expect(__sum <= _RealType(0), 0))
	      std::__throw_runtime_error(__N("normal_mv_distribution::"
					     "param_type::_M_init_full"));
	    *__w++ = std::sqrt(__sum);

	    std::advance(__varcovbegin, _Dimen - __j);
	  }
      }

  template<std::size_t _Dimen, typename _RealType>
    template<typename _InputIterator1, typename _InputIterator2>
      void
      normal_mv_distribution<_Dimen, _RealType>::param_type::
      _M_init_lower(_InputIterator1 __meanbegin, _InputIterator1 __meanend,
		    _InputIterator2 __varcovbegin, _InputIterator2 __varcovend)
      {
	__glibcxx_function_requires(_InputIteratorConcept<_InputIterator1>)
	__glibcxx_function_requires(_InputIteratorConcept<_InputIterator2>)
	std::fill(std::copy(__meanbegin, __meanend, _M_mean.begin()),
		  _M_mean.end(), _RealType(0));

	// Perform the Cholesky decomposition
	auto __w = _M_t.begin();
	for (size_t __j = 0; __j < _Dimen; ++__j)
	  {
	    _RealType __sum = _RealType(0);

	    auto __slitbegin = __w;
	    auto __cit = _M_t.begin();
	    for (size_t __i = 0; __i < __j; ++__i)
	      {
		auto __slit = __slitbegin;
		_RealType __s = *__varcovbegin++;
		for (size_t __k = 0; __k < __i; ++__k)
		  __s -= *__slit++ * *__cit++;

		*__w++ = __s /= *__cit++;
		__sum += __s * __s;
	      }

	    __sum = *__varcovbegin++ - __sum;
	    if (__builtin_expect(__sum <= _RealType(0), 0))
	      std::__throw_runtime_error(__N("normal_mv_distribution::"
					     "param_type::_M_init_full"));
	    *__w++ = std::sqrt(__sum);
	  }
      }

  template<std::size_t _Dimen, typename _RealType>
    template<typename _InputIterator1, typename _InputIterator2>
      void
      normal_mv_distribution<_Dimen, _RealType>::param_type::
      _M_init_diagonal(_InputIterator1 __meanbegin, _InputIterator1 __meanend,
		       _InputIterator2 __varbegin, _InputIterator2 __varend)
      {
	__glibcxx_function_requires(_InputIteratorConcept<_InputIterator1>)
	__glibcxx_function_requires(_InputIteratorConcept<_InputIterator2>)
	std::fill(std::copy(__meanbegin, __meanend, _M_mean.begin()),
		  _M_mean.end(), _RealType(0));

	auto __w = _M_t.begin();
	size_t __step = 0;
	while (__varbegin != __varend)
	  {
	    std::fill_n(__w, __step, _RealType(0));
	    __w += __step++;
	    if (__builtin_expect(*__varbegin < _RealType(0), 0))
	      std::__throw_runtime_error(__N("normal_mv_distribution::"
					     "param_type::_M_init_diagonal"));
	    *__w++ = std::sqrt(*__varbegin++);
	  }
      }

  template<std::size_t _Dimen, typename _RealType>
    template<typename _UniformRandomNumberGenerator>
      typename normal_mv_distribution<_Dimen, _RealType>::result_type
      normal_mv_distribution<_Dimen, _RealType>::
      operator()(_UniformRandomNumberGenerator& __urng,
		 const param_type& __param)
      {
	result_type __ret;

	_M_nd.__generate(__ret.begin(), __ret.end(), __urng);

	auto __t_it = __param._M_t.crbegin();
	for (size_t __i = _Dimen; __i > 0; --__i)
	  {
	    _RealType __sum = _RealType(0);
	    for (size_t __j = __i; __j > 0; --__j)
	      __sum += __ret[__j - 1] * *__t_it++;
	    __ret[__i - 1] = __sum;
	  }

	return __ret;
      }

  template<std::size_t _Dimen, typename _RealType>
    template<typename _ForwardIterator, typename _UniformRandomNumberGenerator>
      void
      normal_mv_distribution<_Dimen, _RealType>::
      __generate_impl(_ForwardIterator __f, _ForwardIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __param)
      {
	__glibcxx_function_requires(_Mutable_ForwardIteratorConcept<
				    _ForwardIterator>)
	while (__f != __t)
	  *__f++ = this->operator()(__urng, __param);
      }

  template<size_t _Dimen, typename _RealType>
    bool
    operator==(const __gnu_cxx::normal_mv_distribution<_Dimen, _RealType>&
	       __d1,
	       const __gnu_cxx::normal_mv_distribution<_Dimen, _RealType>&
	       __d2)
    {
      return __d1._M_param == __d2._M_param && __d1._M_nd == __d2._M_nd;
    }

  template<size_t _Dimen, typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const __gnu_cxx::normal_mv_distribution<_Dimen, _RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      auto __mean = __x._M_param.mean();
      for (auto __it : __mean)
	__os << __it << __space;
      auto __t = __x._M_param.varcov();
      for (auto __it : __t)
	__os << __it << __space;

      __os << __x._M_nd;

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<size_t _Dimen, typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       __gnu_cxx::normal_mv_distribution<_Dimen, _RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      std::array<_RealType, _Dimen> __mean;
      for (auto& __it : __mean)
	__is >> __it;
      std::array<_RealType, _Dimen * (_Dimen + 1) / 2> __varcov;
      for (auto& __it : __varcov)
	__is >> __it;

      __is >> __x._M_nd;

      __x.param(typename normal_mv_distribution<_Dimen, _RealType>::
		param_type(__mean.begin(), __mean.end(),
			   __varcov.begin(), __varcov.end()));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      rice_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __p)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	while (__f != __t)
	  {
	    typename std::normal_distribution<result_type>::param_type
	      __px(__p.nu(), __p.sigma()), __py(result_type(0), __p.sigma());
	    result_type __x = this->_M_ndx(__px, __urng);
	    result_type __y = this->_M_ndy(__py, __urng);
#if _GLIBCXX_USE_C99_MATH_TR1
	    *__f++ = std::hypot(__x, __y);
#else
	    *__f++ = std::sqrt(__x * __x + __y * __y);
#endif
	  }
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const rice_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.nu() << __space << __x.sigma();
      __os << __space << __x._M_ndx;
      __os << __space << __x._M_ndy;

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       rice_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __nu_val, __sigma_val;
      __is >> __nu_val >> __sigma_val;
      __is >> __x._M_ndx;
      __is >> __x._M_ndy;
      __x.param(typename rice_distribution<_RealType>::
		param_type(__nu_val, __sigma_val));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      nakagami_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __p)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	typename std::gamma_distribution<result_type>::param_type
	  __pg(__p.mu(), __p.omega() / __p.mu());
	while (__f != __t)
	  *__f++ = std::sqrt(this->_M_gd(__pg, __urng));
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const nakagami_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.mu() << __space << __x.omega();
      __os << __space << __x._M_gd;

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       nakagami_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __mu_val, __omega_val;
      __is >> __mu_val >> __omega_val;
      __is >> __x._M_gd;
      __x.param(typename nakagami_distribution<_RealType>::
		param_type(__mu_val, __omega_val));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      pareto_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __p)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	result_type __mu_val = __p.mu();
	result_type __malphinv = -result_type(1) / __p.alpha();
	while (__f != __t)
	  *__f++ = __mu_val * std::pow(this->_M_ud(__urng), __malphinv);
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const pareto_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.alpha() << __space << __x.mu();
      __os << __space << __x._M_ud;

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       pareto_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __alpha_val, __mu_val;
      __is >> __alpha_val >> __mu_val;
      __is >> __x._M_ud;
      __x.param(typename pareto_distribution<_RealType>::
		param_type(__alpha_val, __mu_val));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _UniformRandomNumberGenerator>
      typename k_distribution<_RealType>::result_type
      k_distribution<_RealType>::
      operator()(_UniformRandomNumberGenerator& __urng)
      {
	result_type __x = this->_M_gd1(__urng);
	result_type __y = this->_M_gd2(__urng);
	return std::sqrt(__x * __y);
      }

  template<typename _RealType>
    template<typename _UniformRandomNumberGenerator>
      typename k_distribution<_RealType>::result_type
      k_distribution<_RealType>::
      operator()(_UniformRandomNumberGenerator& __urng,
		 const param_type& __p)
      {
	typename std::gamma_distribution<result_type>::param_type
	  __p1(__p.lambda(), result_type(1) / __p.lambda()),
	  __p2(__p.nu(), __p.mu() / __p.nu());
	result_type __x = this->_M_gd1(__p1, __urng);
	result_type __y = this->_M_gd2(__p2, __urng);
	return std::sqrt(__x * __y);
      }

  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      k_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __p)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	typename std::gamma_distribution<result_type>::param_type
	  __p1(__p.lambda(), result_type(1) / __p.lambda()),
	  __p2(__p.nu(), __p.mu() / __p.nu());
	while (__f != __t)
	  {
	    result_type __x = this->_M_gd1(__p1, __urng);
	    result_type __y = this->_M_gd2(__p2, __urng);
	    *__f++ = std::sqrt(__x * __y);
	  }
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const k_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.lambda() << __space << __x.mu() << __space << __x.nu();
      __os << __space << __x._M_gd1;
      __os << __space << __x._M_gd2;

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       k_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __lambda_val, __mu_val, __nu_val;
      __is >> __lambda_val >> __mu_val >> __nu_val;
      __is >> __x._M_gd1;
      __is >> __x._M_gd2;
      __x.param(typename k_distribution<_RealType>::
		param_type(__lambda_val, __mu_val, __nu_val));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      arcsine_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __p)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	result_type __dif = __p.b() - __p.a();
	result_type __sum = __p.a() + __p.b();
	while (__f != __t)
	  {
	    result_type __x = std::sin(this->_M_ud(__urng));
	    *__f++ = (__x * __dif + __sum) / result_type(2);
	  }
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const arcsine_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.a() << __space << __x.b();
      __os << __space << __x._M_ud;

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       arcsine_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __a, __b;
      __is >> __a >> __b;
      __is >> __x._M_ud;
      __x.param(typename arcsine_distribution<_RealType>::
		param_type(__a, __b));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _UniformRandomNumberGenerator>
      typename hoyt_distribution<_RealType>::result_type
      hoyt_distribution<_RealType>::
      operator()(_UniformRandomNumberGenerator& __urng)
      {
	result_type __x = this->_M_ad(__urng);
	result_type __y = this->_M_ed(__urng);
	return (result_type(2) * this->q()
		  / (result_type(1) + this->q() * this->q()))
	       * std::sqrt(this->omega() * __x * __y);
      }

  template<typename _RealType>
    template<typename _UniformRandomNumberGenerator>
      typename hoyt_distribution<_RealType>::result_type
      hoyt_distribution<_RealType>::
      operator()(_UniformRandomNumberGenerator& __urng,
		 const param_type& __p)
      {
	result_type __q2 = __p.q() * __p.q();
	result_type __num = result_type(0.5L) * (result_type(1) + __q2);
	typename __gnu_cxx::arcsine_distribution<result_type>::param_type
	  __pa(__num, __num / __q2);
	result_type __x = this->_M_ad(__pa, __urng);
	result_type __y = this->_M_ed(__urng);
	return (result_type(2) * __p.q() / (result_type(1) + __q2))
	       * std::sqrt(__p.omega() * __x * __y);
      }

  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      hoyt_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __p)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	result_type __2q = result_type(2) * __p.q();
	result_type __q2 = __p.q() * __p.q();
	result_type __q2p1 = result_type(1) + __q2;
	result_type __num = result_type(0.5L) * __q2p1;
	result_type __omega = __p.omega();
	typename __gnu_cxx::arcsine_distribution<result_type>::param_type
	  __pa(__num, __num / __q2);
	while (__f != __t)
	  {
	    result_type __x = this->_M_ad(__pa, __urng);
	    result_type __y = this->_M_ed(__urng);
	    *__f++ = (__2q / __q2p1) * std::sqrt(__omega * __x * __y);
	  }
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const hoyt_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.q() << __space << __x.omega();
      __os << __space << __x._M_ad;
      __os << __space << __x._M_ed;

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       hoyt_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __q, __omega;
      __is >> __q >> __omega;
      __is >> __x._M_ad;
      __is >> __x._M_ed;
      __x.param(typename hoyt_distribution<_RealType>::
		param_type(__q, __omega));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      triangular_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __param)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	while (__f != __t)
	  *__f++ = this->operator()(__urng, __param);
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const __gnu_cxx::triangular_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.a() << __space << __x.b() << __space << __x.c();

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       __gnu_cxx::triangular_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __a, __b, __c;
      __is >> __a >> __b >> __c;
      __x.param(typename __gnu_cxx::triangular_distribution<_RealType>::
		param_type(__a, __b, __c));

      __is.flags(__flags);
      return __is;
    }


  template<typename _RealType>
    template<typename _OutputIterator,
	     typename _UniformRandomNumberGenerator>
      void
      von_mises_distribution<_RealType>::
      __generate_impl(_OutputIterator __f, _OutputIterator __t,
		      _UniformRandomNumberGenerator& __urng,
		      const param_type& __param)
      {
	__glibcxx_function_requires(_OutputIteratorConcept<_OutputIterator>)

	while (__f != __t)
	  *__f++ = this->operator()(__urng, __param);
      }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const __gnu_cxx::von_mises_distribution<_RealType>& __x)
    {
      typedef std::basic_ostream<_CharT, _Traits>  __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __os.flags();
      const _CharT __fill = __os.fill();
      const std::streamsize __precision = __os.precision();
      const _CharT __space = __os.widen(' ');
      __os.flags(__ios_base::scientific | __ios_base::left);
      __os.fill(__space);
      __os.precision(std::numeric_limits<_RealType>::max_digits10);

      __os << __x.mu() << __space << __x.kappa();

      __os.flags(__flags);
      __os.fill(__fill);
      __os.precision(__precision);
      return __os;
    }

  template<typename _RealType, typename _CharT, typename _Traits>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is,
	       __gnu_cxx::von_mises_distribution<_RealType>& __x)
    {
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      const typename __ios_base::fmtflags __flags = __is.flags();
      __is.flags(__ios_base::dec | __ios_base::skipws);

      _RealType __mu, __kappa;
      __is >> __mu >> __kappa;
      __x.param(typename __gnu_cxx::von_mises_distribution<_RealType>::
		param_type(__mu, __kappa));

      __is.flags(__flags);
      return __is;
    }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace


#endif // _EXT_RANDOM_TCC
