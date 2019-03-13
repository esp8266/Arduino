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
  using timeType = decltype(millis());
  static timeType time() {return millis();}
  static constexpr timeType toMillis = 1;
};

#ifdef CORE_MOCK
struct TimeCycle: public TimeMillis {};
#else

struct TimeCycle
{
  using timeType = decltype(ESP.getCycleCount());
  static timeType time() {return ESP.getCycleCount();}
  static constexpr timeType toMillis = F_CPU / 1000;
};

#endif // cpu cycles


} //TimePolicy

template <bool PeriodicT, typename YieldPolicyT = YieldPolicy::DoNothing, typename TimePolicyT = TimePolicy::TimeMillis>
class timeoutTemplate
{
public:
  using timeType = decltype(TimePolicyT::time());
  
  timeoutTemplate(timeType timeout) 
    : _timeout(timeout * TimePolicyT::toMillis), _start(TimePolicyT::time())
  {} 

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
 
protected:
  
  bool expiredRetrigger()
  {
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

using oneShotMs        = polledTimeout::timeoutTemplate<false>;
using periodicMs       = polledTimeout::timeoutTemplate<true>;
using oneShotMsYield   = polledTimeout::timeoutTemplate<false, YieldPolicy::YieldOrSkip>;
using periodicMsYield  = polledTimeout::timeoutTemplate<true,  YieldPolicy::YieldOrSkip>;
using oneShotCpu       = polledTimeout::timeoutTemplate<false, YieldPolicy::DoNothing,   TimePolicy::TimeCycle>;
using periodicCpu      = polledTimeout::timeoutTemplate<true,  YieldPolicy::DoNothing,   TimePolicy::TimeCycle>;
using oneShotCpuYield  = polledTimeout::timeoutTemplate<false, YieldPolicy::YieldOrSkip, TimePolicy::TimeCycle>;
using periodicCpuYield = polledTimeout::timeoutTemplate<true,  YieldPolicy::YieldOrSkip, TimePolicy::TimeCycle>;

// default / generic / backward compatibility
using oneShot = oneShotMs;
using periodic = periodicMs;

} //polledTimeout


/* A 1-shot timeout that auto-yields when in CONT can be built as follows:
 * using oneShotYield = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
 *
 * Other policies can be implemented by the user, e.g.: simple yield that panics in SYS, and the polledTimeout types built as needed as shown above, without modifying this file.
 */

}//esp8266

#endif
