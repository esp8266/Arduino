/*
 yieldUntil.h
 Copyright (c) 2021 esp8266/Arduino

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

#ifndef __YIELDUNTIL_H
#define __YIELDUNTIL_H

#include <Arduino.h>            // yield()
#include <PolledTimeout.h>

namespace esp8266 {
namespace utils {

// Let a chance for recurrent scheduled functions to run during a waiting delay
// returns false if timeout is reached before user-provided callback returns true.

template <typename CB>
bool yieldUntil (CB&& until, uint32_t timeoutMs) {
    esp8266::polledTimeout::oneShotFastMs timeOut(timeoutMs);
    while (true) {
        if (until())
            return true;
        if (timeOut)
            return false;
        yield();
    }
}

}; // ns utils
}; // ns esp8266

#endif // __YIELDUNTIL_H
