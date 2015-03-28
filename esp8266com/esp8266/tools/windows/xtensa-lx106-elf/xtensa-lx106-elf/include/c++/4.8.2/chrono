// <chrono> -*- C++ -*-

// Copyright (C) 2008-2013 Free Software Foundation, Inc.
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

/** @file include/chrono
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_CHRONO
#define _GLIBCXX_CHRONO 1

#pragma GCC system_header

#if __cplusplus < 201103L
# include <bits/c++0x_warning.h>
#else

#include <ratio>
#include <type_traits>
#include <limits>
#include <ctime>

#ifdef _GLIBCXX_USE_C99_STDINT_TR1

namespace std _GLIBCXX_VISIBILITY(default)
{
  /**
   * @defgroup chrono Time
   * @ingroup utilities
   *
   * Classes and functions for time.
   * @{
   */

  /** @namespace std::chrono
   *  @brief ISO C++ 2011 entities sub-namespace for time and date.
   */
  namespace chrono
  {
  _GLIBCXX_BEGIN_NAMESPACE_VERSION

    template<typename _Rep, typename _Period = ratio<1>>
      struct duration;

    template<typename _Clock, typename _Dur = typename _Clock::duration>
      struct time_point;

  _GLIBCXX_END_NAMESPACE_VERSION
  }

_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // 20.11.4.3 specialization of common_type (for duration, sfinae-friendly)
  
  template<typename _CT, typename _Period1, typename _Period2>
    struct __duration_common_type_wrapper
    {
    private:
      typedef __static_gcd<_Period1::num, _Period2::num> __gcd_num;
      typedef __static_gcd<_Period1::den, _Period2::den> __gcd_den;
      typedef typename _CT::type __cr;
      typedef ratio<__gcd_num::value,
        (_Period1::den / __gcd_den::value) * _Period2::den> __r;
    public:
      typedef __success_type<chrono::duration<__cr, __r>> type;
    };

  template<typename _Period1, typename _Period2>
    struct __duration_common_type_wrapper<__failure_type, _Period1, _Period2>
    { typedef __failure_type type; };

  template<typename _Rep1, typename _Period1, typename _Rep2, typename _Period2>
    struct common_type<chrono::duration<_Rep1, _Period1>,
             chrono::duration<_Rep2, _Period2>>
    : public __duration_common_type_wrapper<typename __member_type_wrapper<
             common_type<_Rep1, _Rep2>>::type, _Period1, _Period2>::type
    { };

  // 20.11.4.3 specialization of common_type (for time_point, sfinae-friendly)
  
  template<typename _CT, typename _Clock>
    struct __timepoint_common_type_wrapper
    {
      typedef __success_type<chrono::time_point<_Clock, typename _CT::type>>
        type;
    };

  template<typename _Clock>
    struct __timepoint_common_type_wrapper<__failure_type, _Clock>
    { typedef __failure_type type; };

  template<typename _Clock, typename _Duration1, typename _Duration2>
    struct common_type<chrono::time_point<_Clock, _Duration1>,
             chrono::time_point<_Clock, _Duration2>>
    : public __timepoint_common_type_wrapper<typename __member_type_wrapper<
             common_type<_Duration1, _Duration2>>::type, _Clock>::type
    { };

_GLIBCXX_END_NAMESPACE_VERSION

  namespace chrono
  {
  _GLIBCXX_BEGIN_NAMESPACE_VERSION

    // Primary template for duration_cast impl.
    template<typename _ToDur, typename _CF, typename _CR,
	     bool _NumIsOne = false, bool _DenIsOne = false>
      struct __duration_cast_impl
      {
	template<typename _Rep, typename _Period>
	  static constexpr _ToDur
	  __cast(const duration<_Rep, _Period>& __d)
	  {
	    typedef typename _ToDur::rep			__to_rep;
	    return _ToDur(static_cast<__to_rep>(static_cast<_CR>(__d.count())
	      * static_cast<_CR>(_CF::num)
	      / static_cast<_CR>(_CF::den)));
	  }
      };

    template<typename _ToDur, typename _CF, typename _CR>
      struct __duration_cast_impl<_ToDur, _CF, _CR, true, true>
      {
	template<typename _Rep, typename _Period>
	  static constexpr _ToDur
	  __cast(const duration<_Rep, _Period>& __d)
	  {
	    typedef typename _ToDur::rep			__to_rep;
	    return _ToDur(static_cast<__to_rep>(__d.count()));
	  }
      };

    template<typename _ToDur, typename _CF, typename _CR>
      struct __duration_cast_impl<_ToDur, _CF, _CR, true, false>
      {
	template<typename _Rep, typename _Period>
	  static constexpr _ToDur
	  __cast(const duration<_Rep, _Period>& __d)
	  {
	    typedef typename _ToDur::rep			__to_rep;
	    return _ToDur(static_cast<__to_rep>(
	      static_cast<_CR>(__d.count()) / static_cast<_CR>(_CF::den)));
	  }
      };

    template<typename _ToDur, typename _CF, typename _CR>
      struct __duration_cast_impl<_ToDur, _CF, _CR, false, true>
      {
	template<typename _Rep, typename _Period>
	  static constexpr _ToDur
	  __cast(const duration<_Rep, _Period>& __d)
	  {
	    typedef typename _ToDur::rep			__to_rep;
	    return _ToDur(static_cast<__to_rep>(
	      static_cast<_CR>(__d.count()) * static_cast<_CR>(_CF::num)));
	  }
      };

    template<typename _Tp>
      struct __is_duration
      : std::false_type
      { };

    template<typename _Rep, typename _Period>
      struct __is_duration<duration<_Rep, _Period>>
      : std::true_type
      { };

    /// duration_cast
    template<typename _ToDur, typename _Rep, typename _Period>
      constexpr typename enable_if<__is_duration<_ToDur>::value,
				   _ToDur>::type
      duration_cast(const duration<_Rep, _Period>& __d)
      {
	typedef typename _ToDur::period				__to_period;
	typedef typename _ToDur::rep				__to_rep;
	typedef ratio_divide<_Period, __to_period> 		__cf;
	typedef typename common_type<__to_rep, _Rep, intmax_t>::type
	  							__cr;
	typedef  __duration_cast_impl<_ToDur, __cf, __cr,
				      __cf::num == 1, __cf::den == 1> __dc;
	return __dc::__cast(__d);
      }

    /// treat_as_floating_point
    template<typename _Rep>
      struct treat_as_floating_point
      : is_floating_point<_Rep>
      { };

    /// duration_values
    template<typename _Rep>
      struct duration_values
      {
	static constexpr _Rep
	zero()
	{ return _Rep(0); }

	static constexpr _Rep
	max()
	{ return numeric_limits<_Rep>::max(); }

	static constexpr _Rep
	min()
	{ return numeric_limits<_Rep>::lowest(); }
      };

    template<typename _Tp>
      struct __is_ratio
      : std::false_type
      { };

    template<intmax_t _Num, intmax_t _Den>
      struct __is_ratio<ratio<_Num, _Den>>
      : std::true_type
      { };

    /// duration
    template<typename _Rep, typename _Period>
      struct duration
      {
	typedef _Rep						rep;
	typedef _Period 					period;

	static_assert(!__is_duration<_Rep>::value, "rep cannot be a duration");
	static_assert(__is_ratio<_Period>::value,
		      "period must be a specialization of ratio");
	static_assert(_Period::num > 0, "period must be positive");

	// 20.11.5.1 construction / copy / destroy
	constexpr duration() = default;

	// NB: Make constexpr implicit. This cannot be explicitly
	// constexpr, as any UDT that is not a literal type with a
	// constexpr copy constructor will be ill-formed.
	duration(const duration&) = default;

	template<typename _Rep2, typename = typename
	       enable_if<is_convertible<_Rep2, rep>::value
			 && (treat_as_floating_point<rep>::value
			     || !treat_as_floating_point<_Rep2>::value)>::type>
	  constexpr explicit duration(const _Rep2& __rep)
	  : __r(static_cast<rep>(__rep)) { }

	template<typename _Rep2, typename _Period2, typename = typename
	       enable_if<treat_as_floating_point<rep>::value
			 || (ratio_divide<_Period2, period>::den == 1
			     && !treat_as_floating_point<_Rep2>::value)>::type>
	  constexpr duration(const duration<_Rep2, _Period2>& __d)
	  : __r(duration_cast<duration>(__d).count()) { }

	~duration() = default;
	duration& operator=(const duration&) = default;

	// 20.11.5.2 observer
	constexpr rep
	count() const
	{ return __r; }

	// 20.11.5.3 arithmetic
	constexpr duration
	operator+() const
	{ return *this; }

	constexpr duration
	operator-() const
	{ return duration(-__r); }

	duration&
	operator++()
	{
	  ++__r;
	  return *this;
	}

	duration
	operator++(int)
	{ return duration(__r++); }

	duration&
	operator--()
	{
	  --__r;
	  return *this;
	}

	duration
	operator--(int)
	{ return duration(__r--); }

	duration&
	operator+=(const duration& __d)
	{
	  __r += __d.count();
	  return *this;
	}

	duration&
	operator-=(const duration& __d)
	{
	  __r -= __d.count();
	  return *this;
	}

	duration&
	operator*=(const rep& __rhs)
	{
	  __r *= __rhs;
	  return *this;
	}

	duration&
	operator/=(const rep& __rhs)
	{
	  __r /= __rhs;
	  return *this;
	}

	// DR 934.
	template<typename _Rep2 = rep>
	  typename enable_if<!treat_as_floating_point<_Rep2>::value,
			     duration&>::type
	  operator%=(const rep& __rhs)
	  {
	    __r %= __rhs;
	    return *this;
	  }

	template<typename _Rep2 = rep>
	  typename enable_if<!treat_as_floating_point<_Rep2>::value,
			     duration&>::type
	  operator%=(const duration& __d)
	  {
	    __r %= __d.count();
	    return *this;
	  }

	// 20.11.5.4 special values
	static constexpr duration
	zero()
	{ return duration(duration_values<rep>::zero()); }

	static constexpr duration
	min()
	{ return duration(duration_values<rep>::min()); }

	static constexpr duration
	max()
	{ return duration(duration_values<rep>::max()); }

      private:
	rep __r;
      };

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr typename common_type<duration<_Rep1, _Period1>,
				     duration<_Rep2, _Period2>>::type
      operator+(const duration<_Rep1, _Period1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      {
	typedef duration<_Rep1, _Period1>			__dur1;
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<__dur1,__dur2>::type	__cd;
	return __cd(__cd(__lhs).count() + __cd(__rhs).count());
      }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr typename common_type<duration<_Rep1, _Period1>,
				     duration<_Rep2, _Period2>>::type
      operator-(const duration<_Rep1, _Period1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      {
	typedef duration<_Rep1, _Period1>			__dur1;
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<__dur1,__dur2>::type	__cd;
	return __cd(__cd(__lhs).count() - __cd(__rhs).count());
      }

    template<typename _Rep1, typename _Rep2, bool =
	     is_convertible<_Rep2,
			    typename common_type<_Rep1, _Rep2>::type>::value>
      struct __common_rep_type { };

    template<typename _Rep1, typename _Rep2>
      struct __common_rep_type<_Rep1, _Rep2, true>
      { typedef typename common_type<_Rep1, _Rep2>::type type; };

    template<typename _Rep1, typename _Period, typename _Rep2>
      constexpr
      duration<typename __common_rep_type<_Rep1, _Rep2>::type, _Period>
      operator*(const duration<_Rep1, _Period>& __d, const _Rep2& __s)
      {
	typedef duration<typename common_type<_Rep1, _Rep2>::type, _Period>
	  __cd;
	return __cd(__cd(__d).count() * __s);
      }

    template<typename _Rep1, typename _Rep2, typename _Period>
      constexpr
      duration<typename __common_rep_type<_Rep2, _Rep1>::type, _Period>
      operator*(const _Rep1& __s, const duration<_Rep2, _Period>& __d)
      { return __d * __s; }

    template<typename _Rep1, typename _Period, typename _Rep2>
      constexpr duration<typename __common_rep_type<_Rep1, typename
	enable_if<!__is_duration<_Rep2>::value, _Rep2>::type>::type, _Period>
      operator/(const duration<_Rep1, _Period>& __d, const _Rep2& __s)
      {
	typedef duration<typename common_type<_Rep1, _Rep2>::type, _Period>
	  __cd;
	return __cd(__cd(__d).count() / __s);
      }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr typename common_type<_Rep1, _Rep2>::type
      operator/(const duration<_Rep1, _Period1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      {
	typedef duration<_Rep1, _Period1>			__dur1;
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<__dur1,__dur2>::type	__cd;
	return __cd(__lhs).count() / __cd(__rhs).count();
      }

    // DR 934.
    template<typename _Rep1, typename _Period, typename _Rep2>
      constexpr duration<typename __common_rep_type<_Rep1, typename
	enable_if<!__is_duration<_Rep2>::value, _Rep2>::type>::type, _Period>
      operator%(const duration<_Rep1, _Period>& __d, const _Rep2& __s)
      {
	typedef duration<typename common_type<_Rep1, _Rep2>::type, _Period>
	  __cd;
	return __cd(__cd(__d).count() % __s);
      }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr typename common_type<duration<_Rep1, _Period1>,
				     duration<_Rep2, _Period2>>::type
      operator%(const duration<_Rep1, _Period1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      {
	typedef duration<_Rep1, _Period1>			__dur1;
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<__dur1,__dur2>::type	__cd;
	return __cd(__cd(__lhs).count() % __cd(__rhs).count());
      }

    // comparisons
    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr bool
      operator==(const duration<_Rep1, _Period1>& __lhs,
		 const duration<_Rep2, _Period2>& __rhs)
      {
	typedef duration<_Rep1, _Period1>			__dur1;
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<__dur1,__dur2>::type	__ct;
	return __ct(__lhs).count() == __ct(__rhs).count();
      }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr bool
      operator<(const duration<_Rep1, _Period1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      {
	typedef duration<_Rep1, _Period1>			__dur1;
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<__dur1,__dur2>::type	__ct;
	return __ct(__lhs).count() < __ct(__rhs).count();
      }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr bool
      operator!=(const duration<_Rep1, _Period1>& __lhs,
		 const duration<_Rep2, _Period2>& __rhs)
      { return !(__lhs == __rhs); }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr bool
      operator<=(const duration<_Rep1, _Period1>& __lhs,
		 const duration<_Rep2, _Period2>& __rhs)
      { return !(__rhs < __lhs); }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr bool
      operator>(const duration<_Rep1, _Period1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      { return __rhs < __lhs; }

    template<typename _Rep1, typename _Period1,
	     typename _Rep2, typename _Period2>
      constexpr bool
      operator>=(const duration<_Rep1, _Period1>& __lhs,
		 const duration<_Rep2, _Period2>& __rhs)
      { return !(__lhs < __rhs); }

    /// nanoseconds
    typedef duration<int64_t, nano> 	nanoseconds;

    /// microseconds
    typedef duration<int64_t, micro> 	microseconds;

    /// milliseconds
    typedef duration<int64_t, milli> 	milliseconds;

    /// seconds
    typedef duration<int64_t> 		seconds;

    /// minutes
    typedef duration<int, ratio< 60>> 	minutes;

    /// hours
    typedef duration<int, ratio<3600>> 	hours;

    /// time_point
    template<typename _Clock, typename _Dur>
      struct time_point
      {
	typedef _Clock			  			clock;
	typedef _Dur		  				duration;
	typedef typename duration::rep	  			rep;
	typedef typename duration::period			period;

	constexpr time_point() : __d(duration::zero())
	{ }

	constexpr explicit time_point(const duration& __dur)
	: __d(__dur)
	{ }

	// conversions
	template<typename _Dur2>
	  constexpr time_point(const time_point<clock, _Dur2>& __t)
	  : __d(__t.time_since_epoch())
	  { }

	// observer
	constexpr duration
	time_since_epoch() const
	{ return __d; }

	// arithmetic
	time_point&
	operator+=(const duration& __dur)
	{
	  __d += __dur;
	  return *this;
	}

	time_point&
	operator-=(const duration& __dur)
	{
	  __d -= __dur;
	  return *this;
	}

	// special values
	static constexpr time_point
	min()
	{ return time_point(duration::min()); }

	static constexpr time_point
	max()
	{ return time_point(duration::max()); }

      private:
	duration __d;
      };

    /// time_point_cast
    template<typename _ToDur, typename _Clock, typename _Dur>
      constexpr typename enable_if<__is_duration<_ToDur>::value,
				   time_point<_Clock, _ToDur>>::type
      time_point_cast(const time_point<_Clock, _Dur>& __t)
      {
	typedef time_point<_Clock, _ToDur> 			__time_point;
	return __time_point(duration_cast<_ToDur>(__t.time_since_epoch()));
      }

    template<typename _Clock, typename _Dur1,
	     typename _Rep2, typename _Period2>
      constexpr time_point<_Clock,
	typename common_type<_Dur1, duration<_Rep2, _Period2>>::type>
      operator+(const time_point<_Clock, _Dur1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      {
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<_Dur1,__dur2>::type	__ct;
	typedef time_point<_Clock, __ct> 			__time_point;
	return __time_point(__lhs.time_since_epoch() + __rhs);
      }

    template<typename _Rep1, typename _Period1,
	     typename _Clock, typename _Dur2>
      constexpr time_point<_Clock,
	typename common_type<duration<_Rep1, _Period1>, _Dur2>::type>
      operator+(const duration<_Rep1, _Period1>& __lhs,
		const time_point<_Clock, _Dur2>& __rhs)
      { 
	typedef duration<_Rep1, _Period1>			__dur1;
	typedef typename common_type<__dur1,_Dur2>::type	__ct;
	typedef time_point<_Clock, __ct> 			__time_point;
	return __time_point(__rhs.time_since_epoch() + __lhs); 
      }

    template<typename _Clock, typename _Dur1,
	     typename _Rep2, typename _Period2>
      constexpr time_point<_Clock,
	typename common_type<_Dur1, duration<_Rep2, _Period2>>::type>
      operator-(const time_point<_Clock, _Dur1>& __lhs,
		const duration<_Rep2, _Period2>& __rhs)
      { 
	typedef duration<_Rep2, _Period2>			__dur2;
	typedef typename common_type<_Dur1,__dur2>::type	__ct;
	typedef time_point<_Clock, __ct> 			__time_point;
	return __time_point(__lhs.time_since_epoch() -__rhs); 
      }

    template<typename _Clock, typename _Dur1, typename _Dur2>
      constexpr typename common_type<_Dur1, _Dur2>::type
      operator-(const time_point<_Clock, _Dur1>& __lhs,
		const time_point<_Clock, _Dur2>& __rhs)
      { return __lhs.time_since_epoch() - __rhs.time_since_epoch(); }

    template<typename _Clock, typename _Dur1, typename _Dur2>
      constexpr bool
      operator==(const time_point<_Clock, _Dur1>& __lhs,
		 const time_point<_Clock, _Dur2>& __rhs)
      { return __lhs.time_since_epoch() == __rhs.time_since_epoch(); }

    template<typename _Clock, typename _Dur1, typename _Dur2>
      constexpr bool
      operator!=(const time_point<_Clock, _Dur1>& __lhs,
		 const time_point<_Clock, _Dur2>& __rhs)
      { return !(__lhs == __rhs); }

    template<typename _Clock, typename _Dur1, typename _Dur2>
      constexpr bool
      operator<(const time_point<_Clock, _Dur1>& __lhs,
		const time_point<_Clock, _Dur2>& __rhs)
      { return  __lhs.time_since_epoch() < __rhs.time_since_epoch(); }

    template<typename _Clock, typename _Dur1, typename _Dur2>
      constexpr bool
      operator<=(const time_point<_Clock, _Dur1>& __lhs,
		 const time_point<_Clock, _Dur2>& __rhs)
      { return !(__rhs < __lhs); }

    template<typename _Clock, typename _Dur1, typename _Dur2>
      constexpr bool
      operator>(const time_point<_Clock, _Dur1>& __lhs,
		const time_point<_Clock, _Dur2>& __rhs)
      { return __rhs < __lhs; }

    template<typename _Clock, typename _Dur1, typename _Dur2>
      constexpr bool
      operator>=(const time_point<_Clock, _Dur1>& __lhs,
		 const time_point<_Clock, _Dur2>& __rhs)
      { return !(__lhs < __rhs); }


    // Clocks. 

    // Why nanosecond resolution as the default?  
    // Why have std::system_clock always count in the higest
    // resolution (ie nanoseconds), even if on some OSes the low 3
    // or 9 decimal digits will be always zero? This allows later
    // implementations to change the system_clock::now()
    // implementation any time to provide better resolution without
    // changing function signature or units.

    // To support the (forward) evolution of the library's defined
    // clocks, wrap inside inline namespace so that the current
    // defintions of system_clock, steady_clock, and
    // high_resolution_clock types are uniquely mangled. This way, new
    // code can use the latests clocks, while the library can contain
    // compatibility definitions for previous versions.  At some
    // point, when these clocks settle down, the inlined namespaces
    // can be removed.  XXX GLIBCXX_ABI Deprecated
    inline namespace _V2 {

    /**
     *  @brief System clock.
     *
     *  Time returned represents wall time from the system-wide clock.
    */
     struct system_clock
    {
      typedef chrono::nanoseconds     				duration;
      typedef duration::rep    					rep;
      typedef duration::period 					period;
      typedef chrono::time_point<system_clock, duration> 	time_point;

      static_assert(system_clock::duration::min()
		    < system_clock::duration::zero(),
		    "a clock's minimum duration cannot be less than its epoch");

      static constexpr bool is_steady = false;

      static time_point
      now() noexcept;

      // Map to C API
      static std::time_t
      to_time_t(const time_point& __t) noexcept
      {
	return std::time_t(duration_cast<chrono::seconds>
			   (__t.time_since_epoch()).count());
      }

      static time_point
      from_time_t(std::time_t __t) noexcept
      {
	typedef chrono::time_point<system_clock, seconds>	__from;
	return time_point_cast<system_clock::duration>
	       (__from(chrono::seconds(__t)));
      }
    };


    /**
     *  @brief Monotonic clock
     *
     *  Time returned has the property of only increasing at a uniform rate.
    */
    struct steady_clock
    {
      typedef chrono::nanoseconds 				duration;
      typedef duration::rep	  				rep;
      typedef duration::period	  				period;
      typedef chrono::time_point<steady_clock, duration> 	time_point;

      static constexpr bool is_steady = true;

      static time_point
      now() noexcept;
    };


    /**
     *  @brief Highest-resolution clock
     *
     *  This is the clock "with the shortest tick period." Alias to
     *  std::system_clock until higher-than-nanosecond definitions
     *  become feasible.
    */
    using high_resolution_clock = system_clock;

  } // end inline namespace _V2

  _GLIBCXX_END_NAMESPACE_VERSION
  } // namespace chrono

  // @} group chrono
} // namespace

#endif //_GLIBCXX_USE_C99_STDINT_TR1

#endif // C++11

#endif //_GLIBCXX_CHRONO
