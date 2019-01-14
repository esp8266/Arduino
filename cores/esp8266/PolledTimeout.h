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


template <bool PeriodicT, typename YieldPolicyT = YieldPolicy::DoNothing>  
class timeoutTemplate
{
public:
  using timeType = decltype(millis());
  
  timeoutTemplate(timeType timeout) 
    : _timeout(timeout), _start(millis())  
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
    _start = millis(); 
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
    timeType current = millis();
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
    return checkExpired(millis());
  }
  
  timeType _timeout;
  timeType _start;
};

using oneShot = polledTimeout::timeoutTemplate<false>;
using periodic = polledTimeout::timeoutTemplate<true>;

} //polledTimeout


/* A 1-shot timeout that auto-yields when in CONT can be built as follows:
 * using oneShotYield = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
 *
 * Other policies can be implemented by the user, e.g.: simple yield that panics in SYS, and the polledTimeout types built as needed as shown above, without modifying this file.
 */

}//esp8266

#endif
