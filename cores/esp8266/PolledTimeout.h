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

#include <Arduino.h>

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

} //YieldPolicy

namespace TimePolicy
{

struct TimeMillis
{
  // time policy in milli-seconds based on millis()

  using timeType = decltype(millis());
  static timeType time() {return millis();}
  static constexpr timeType toUnitMul = 1;
  static constexpr timeType toUnitDiv = 1;

  // rollover: 48 days, setting max to 24 days to fit in signed scalar (below is = 2^31 - 1)
  static constexpr timeType timeMax() { return (((((timeType)1) << ((sizeof(timeType) * 8) - 2)) - 1) << 1) + 1; }
};

struct TimeFastMillis
{
  // time policy in milli-seconds based on ESP.getCycleCount()

  using timeType = decltype(ESP.getCycleCount());
  static timeType time() {return ESP.getCycleCount();}
  static constexpr timeType toUnitMul = F_CPU / 1000;
  static constexpr timeType toUnitDiv = 1;

  // rollover: @80Mhz:53.6s @160Mhz:26.8s
  // setting max to half of it to ensure full range is never reached
  // - this particular time measurement is intended to be called very often
  //   (every loop, every yield)
  // - this max is larger than internal watchdogs
  static constexpr timeType timeMax() { return (((timeType)1) << ((sizeof(timeType) * 8) - 2)) / (toUnitMul / 2); }
};

struct TimeFastMicros
{
  // time policy in micro-seconds based on ESP.getCycleCount()

  using timeType = decltype(ESP.getCycleCount());
  static timeType time() {return ESP.getCycleCount();}
  static constexpr timeType toUnitMul = F_CPU / 1000000;
  static constexpr timeType toUnitDiv = 1;

  // rollover: @80Mhz:53.6s @160Mhz:26.8s
  // setting max to half of it to ensure full range is never reached
  // - this particular time measurement is intended to be called very often
  //   (every loop, every yield)
  // - this max is larger than internal watchdogs
  static constexpr timeType timeMax() { return (((timeType)1) << ((sizeof(timeType) * 8) - 2)) / (toUnitMul / 2); }
};

struct TimeFastNanos
{
  // time policy in nano-seconds based on ESP.getCycleCount()

  using timeType = decltype(ESP.getCycleCount());
  static timeType time() {return ESP.getCycleCount();}
  static constexpr timeType toUnitMul = F_CPU / 40000000;
  static constexpr timeType toUnitDiv = 25;

  // timeMax is max (signed 32 bits) / toUnitMul = (2^31 / 4(@160MHz)) = 0.536 seconds
  static constexpr timeType timeMax() { return (((((timeType)1) << ((sizeof(timeType) * 8) - 2 - 2)) - 1) << 1) + 1; }
};

} //TimePolicy

template <bool PeriodicT, typename YieldPolicyT = YieldPolicy::DoNothing, typename TimePolicyT = TimePolicy::TimeMillis>
class timeoutTemplate
{
public:
  using timeType = typename TimePolicyT::timeType;
  
  static constexpr timeType timeMax ()
  {
    return TimePolicyT::timeMax();
  }

  timeoutTemplate(timeType timeout) 
    : _timeout((timeout * TimePolicyT::toUnitMul) / TimePolicyT::toUnitDiv), _start(TimePolicyT::time())
  {
    if (timeout > timeMax())
    {
      _timeout = timeMax();
      ::printf((PGM_P)F("polledTimeout: overflow\r\n"));
    }
  }

  bool expired()
  {
    YieldPolicyT::execute(); //in case of DoNothing: gets optimized away
    if(PeriodicT)           //in case of false: gets optimized away
      return expiredRetrigger();
    return expiredOneShot();
  }
  
  operator bool()
  {
    return expired(); 
  }
  
  void reset(const timeType newTimeout)
  {
    _timeout = newTimeout;
    reset();
  }

  void reset()
  {
    _start = TimePolicyT::time();
  }

  timeType getTimeout() const
  {
    return _timeout;
  }
  
  bool checkExpired(const timeType t) const
  {
    return (t - _start) >=  _timeout;
  }

  static constexpr timeType getTimeMax()
  {
    return TimePolicyT::timeMax();
  }

protected:
  
  bool expiredRetrigger()
  {
    if (!_timeout)
      return true;
    timeType current = TimePolicyT::time();
    if(checkExpired(current))
    {
      unsigned long n = (current - _start) / _timeout; //how many _timeouts periods have elapsed, will usually be 1 (current - _start >= _timeout)
      _start += n  * _timeout;
      return true;
    }
    return false;
  }
  
  bool expiredOneShot() const
  {
    return checkExpired(TimePolicyT::time());
  }
  
  timeType _timeout;
  timeType _start;
};

// legacy type names, deprecated (unit is milliseconds)

using oneShot = polledTimeout::timeoutTemplate<false> /*__attribute__((deprecated("use oneShotMs")))*/;
using periodic = polledTimeout::timeoutTemplate<true> /*__attribute__((deprecated("use periodicMs")))*/;

// standard versions (based on millis())
// timeMax() is 24 days (~ 2000000000 ms)

using oneShotMs = polledTimeout::timeoutTemplate<false>;
using periodicMs = polledTimeout::timeoutTemplate<true>;

// "Fast" versions sacrifices time range for improved precision and reduced code size (factor ~100)
// timeMax() values:
// Ms: max is 13421       ms (13.4   s)
// Us: max is 13421772    us (13.4   s)
// Ns: max is   536870911 ns ( 0.536 s)

using oneShotFastMs = polledTimeout::timeoutTemplate<false, YieldPolicy::DoNothing, TimePolicy::TimeFastMillis>;
using periodicFastMs = polledTimeout::timeoutTemplate<true, YieldPolicy::DoNothing, TimePolicy::TimeFastMillis>;
using oneShotFastUs = polledTimeout::timeoutTemplate<false, YieldPolicy::DoNothing, TimePolicy::TimeFastMicros>;
using periodicFastUs = polledTimeout::timeoutTemplate<true, YieldPolicy::DoNothing, TimePolicy::TimeFastMicros>;
using oneShotFastNs = polledTimeout::timeoutTemplate<false, YieldPolicy::DoNothing, TimePolicy::TimeFastNanos>;
using periodicFastNs = polledTimeout::timeoutTemplate<true, YieldPolicy::DoNothing, TimePolicy::TimeFastNanos>;

} //polledTimeout


/* A 1-shot timeout that auto-yields when in CONT can be built as follows:
 * using oneShotYield = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
 *
 * Other policies can be implemented by the user, e.g.: simple yield that panics in SYS, and the polledTimeout types built as needed as shown above, without modifying this file.
 */

}//esp8266

#endif
