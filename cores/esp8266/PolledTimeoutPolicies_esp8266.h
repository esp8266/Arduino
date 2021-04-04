#ifndef __POLLEDTIMEOUTPOLICIES_ESP8266_H__
#define __POLLEDTIMEOUTPOLICIES_ESP8266_H__


/*
 PolledTimeoutPolicies.h - Policy classes specific to the ESP8266 
 meant for use with polledTimeout

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

#include <PolledTimeoutTemplate.h>
#include <core_esp8266_features.h>

namespace YieldPolicy
{

  struct YieldOrSkip
  {
    static void execute() {delay(0);}
  };

} //YieldPolicy




namespace TimeSourcePolicy
{

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

} //TimeSourcePolicy





#endif
