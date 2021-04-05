#ifndef __POLLEDTIMING_H__
#define __POLLEDTIMING_H__


/*
 PolledTimeout.h - Encapsulation of a polled Timeout

 Copyright (c) 2018 Daniel Salazar. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <c_types.h>               // IRAM_ATTR
#include <limits>                  // std::numeric_limits
#include <type_traits>             // std::is_unsigned
#include <core_esp8266_features.h>

namespace esp8266
{


namespace polledTimeout
{

namespace YieldPolicy
{

struct DoNothing
{
  static void execute() {}
};

struct YieldOrSkip
{
  static void execute() {delay(0);}
};

template <unsigned long delayMs>
struct YieldAndDelayMs
{
  static void execute() {delay(delayMs);}
};

} //YieldPolicy

namespace TimePolicy
{

struct TimeSourceMillis
{
  // time policy in milli-seconds based on millis()

  using timeType = decltype(millis());
  static timeType time() {return millis();}
  static constexpr timeType ticksPerSecond    = 1000;
  static constexpr timeType ticksPerSecondMax = 1000;
};

struct TimeSourceCycles
{
  // time policy based on esp_get_cycle_count()
  // this particular time measurement is intended to be called very often
  // (every loop, every yield)

  using timeType = decltype(esp_get_cycle_count());
  static timeType time() {return esp_get_cycle_count();}
  static constexpr timeType ticksPerSecond    = esp_get_cpu_freq_mhz() * 1000000UL;     // 80'000'000 or 160'000'000 Hz
  static constexpr timeType ticksPerSecondMax = 160000000; // 160MHz
};

template <typename TimeSourceType, unsigned long long second_th>
    // "second_th" units of timeType for one second
struct TimeUnit
{
  using timeType = typename TimeSourceType::timeType;

#if __GNUC__ < 5
  // gcc-4.8 cannot compile the constexpr-only version of this function
  // using #defines instead luckily works
  static constexpr timeType computeRangeCompensation ()
  {
    #define number_of_secondTh_in_one_tick ((1.0 * second_th) / ticksPerSecond)
    #define fractional (number_of_secondTh_in_one_tick - (long)number_of_secondTh_in_one_tick)

    return ({
      fractional == 0?
        1: // no need for compensation
        (number_of_secondTh_in_one_tick / fractional) + 0.5; // scalar multiplier allowing exact division
    });

    #undef number_of_secondTh_in_one_tick
    #undef fractional
  }
#else
  static constexpr timeType computeRangeCompensation ()
  {
    return ({
      constexpr double number_of_secondTh_in_one_tick = (1.0 * second_th) / ticksPerSecond;
      constexpr double fractional = number_of_secondTh_in_one_tick - (long)number_of_secondTh_in_one_tick;
      fractional == 0?
        1: // no need for compensation
        (number_of_secondTh_in_one_tick / fractional) + 0.5; // scalar multiplier allowing exact division
    });
  }
#endif

  static constexpr timeType ticksPerSecond         = TimeSourceType::ticksPerSecond;
  static constexpr timeType ticksPerSecondMax      = TimeSourceType::ticksPerSecondMax;
  static constexpr timeType rangeCompensate        = computeRangeCompensation();
  static constexpr timeType user2UnitMultiplierMax = (ticksPerSecondMax * rangeCompensate) / second_th;
  static constexpr timeType user2UnitMultiplier    = (ticksPerSecond    * rangeCompensate) / second_th;
  static constexpr timeType user2UnitDivider       = rangeCompensate;
  // std::numeric_limits<timeType>::max() is reserved
  static constexpr timeType timeMax                = (std::numeric_limits<timeType>::max() - 1) / user2UnitMultiplierMax;

  static timeType toTimeTypeUnit (const timeType userUnit) {return (userUnit * user2UnitMultiplier) / user2UnitDivider;}
  static timeType toUserUnit (const timeType internalUnit) {return (internalUnit * user2UnitDivider) / user2UnitMultiplier;}
  static timeType time () {return TimeSourceType::time();}
};

using TimeMillis     = TimeUnit< TimeSourceMillis,       1000 >;
using TimeFastMillis = TimeUnit< TimeSourceCycles,       1000 >;
using TimeFastMicros = TimeUnit< TimeSourceCycles,    1000000 >;
using TimeFastNanos  = TimeUnit< TimeSourceCycles, 1000000000 >;

} //TimePolicy

template <bool PeriodicT, typename YieldPolicyT = YieldPolicy::DoNothing, typename TimePolicyT = TimePolicy::TimeMillis>
class timeoutTemplate
{
public:
  using timeType = typename TimePolicyT::timeType;
  static_assert(std::is_unsigned<timeType>::value == true, "timeType must be unsigned");

  static constexpr timeType neverExpires    = std::numeric_limits<timeType>::max();
  static constexpr timeType rangeCompensate = TimePolicyT::rangeCompensate; //debug

  timeoutTemplate(const timeType userTimeout)
  {
    reset(userTimeout);
  }

  IRAM_ATTR // fast
  bool expired()
  {
    bool hasExpired = PeriodicT ? expiredRetrigger() : expiredOneShot();
    if (!hasExpired) //in case of DoNothing: gets optimized away
      YieldPolicyT::execute();
    return hasExpired;
  }

  IRAM_ATTR // fast
  operator bool()
  {
    return expired();
  }

  bool canExpire () const
  {
    return !_neverExpires;
  }

  bool canWait () const
  {
    return !(_timeout == 0 || _oneShotExpired);
  }

  // Resets, will trigger after this new timeout.
  IRAM_ATTR // called from ISR
  void reset(const timeType newUserTimeout)
  {
    reset();
    _timeout = TimePolicyT::toTimeTypeUnit(newUserTimeout);
    _neverExpires = (newUserTimeout < 0) || (newUserTimeout > timeMax());
  }

  // Resets, will trigger after the timeout previously set.
  IRAM_ATTR // called from ISR
  void reset()
  {
    _start = TimePolicyT::time();
    _oneShotExpired = false;
  }

  // Resets to just expired so that on next poll the check will immediately trigger for the user,
  // also change timeout (after next immediate trigger).
  IRAM_ATTR // called from ISR
  void resetAndSetExpired (const timeType newUserTimeout)
  {
    reset(newUserTimeout);
    _start -= _timeout;
  }

  // Resets to just expired so that on next poll the check will immediately trigger for the user.
  IRAM_ATTR // called from ISR
  void resetAndSetExpired ()
  {
    reset();
    _start -= _timeout;
  }

  void resetToNeverExpires ()
  {
    _timeout = 1; // because canWait() has precedence
    _neverExpires = true;
  }

  timeType getTimeout() const
  {
    return TimePolicyT::toUserUnit(_timeout);
  }

  IRAM_ATTR // fast
  timeType remaining() const
  {
    if (_neverExpires)
      return timeMax();
    timeType current = TimePolicyT::time();
    if (checkExpired(current))
      return TimePolicyT::toUserUnit(0);
    return TimePolicyT::toUserUnit(_timeout - (current - _start));
  }
  
  static constexpr timeType timeMax()
  {
    return TimePolicyT::timeMax;
  }

private:

  IRAM_ATTR // fast
  bool checkExpired(const timeType internalUnit) const
  {
    // canWait() is not checked here
    // returns "oneshot has expired", otherwise returns "can expire" and "time has expired"
    return _oneShotExpired || (!_neverExpires && ((internalUnit - _start) >= _timeout));
  }

protected:

  IRAM_ATTR // fast
  bool expiredRetrigger()
  {
    if (!canWait())
      return true;

    timeType current = TimePolicyT::time();
    if(checkExpired(current))
    {
      timeType n = (current - _start) / _timeout; //how many _timeouts periods have elapsed, will usually be 1 (current - _start >= _timeout)
      _start += n  * _timeout;
      return true;
    }
    return false;
  }

  IRAM_ATTR // fast
  bool expiredOneShot() const
  {
    // returns "always expired" or "has expired"
    if (!canWait()) return true;
    if (checkExpired(TimePolicyT::time()))
    {
      _oneShotExpired = true;
      return true;
    }
    return false;
  }

  timeType _timeout;
  timeType _start;
  bool _neverExpires;
  mutable bool _oneShotExpired;
};

// legacy type names, deprecated (unit is milliseconds)

using oneShot = polledTimeout::timeoutTemplate<false> /*__attribute__((deprecated("use oneShotMs")))*/;
using periodic = polledTimeout::timeoutTemplate<true> /*__attribute__((deprecated("use periodicMs")))*/;

// standard versions (based on millis())
// timeMax() is 49.7 days ((2^32)-2 ms)

using oneShotMs = polledTimeout::timeoutTemplate<false>;
using periodicMs = polledTimeout::timeoutTemplate<true>;

// Time policy based on esp_get_cycle_count(), and intended to be called very often:
// "Fast" versions sacrifices time range for improved precision and reduced execution time (by 86%)
// (cpu cycles for ::expired(): 372 (millis()) vs 52 (esp_get_cycle_count()))
// timeMax() values:
// Ms: max is 26843       ms (26.8  s)
// Us: max is 26843545    us (26.8  s)
// Ns: max is  1073741823 ns ( 1.07 s)
// (time policy based on esp_get_cycle_count() is intended to be called very often)

using oneShotFastMs = polledTimeout::timeoutTemplate<false, YieldPolicy::DoNothing, TimePolicy::TimeFastMillis>;
using periodicFastMs = polledTimeout::timeoutTemplate<true, YieldPolicy::DoNothing, TimePolicy::TimeFastMillis>;
using oneShotFastUs = polledTimeout::timeoutTemplate<false, YieldPolicy::DoNothing, TimePolicy::TimeFastMicros>;
using periodicFastUs = polledTimeout::timeoutTemplate<true, YieldPolicy::DoNothing, TimePolicy::TimeFastMicros>;
using oneShotFastNs = polledTimeout::timeoutTemplate<false, YieldPolicy::DoNothing, TimePolicy::TimeFastNanos>;
using periodicFastNs = polledTimeout::timeoutTemplate<true, YieldPolicy::DoNothing, TimePolicy::TimeFastNanos>;

} //polledTimeout


/* A 1-shot timeout that auto-yields when in CONT can be built as follows:
 * using oneShotYieldMs = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
 *
 * Other policies can be implemented by the user, e.g.: simple yield that panics in SYS, and the polledTimeout types built as needed as shown above, without modifying this file.
 */

}//esp8266

#endif
