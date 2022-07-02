/*
  Ticker.cpp - esp8266 library that calls functions periodically

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

#include "c_types.h"
#include "eagle_soc.h"
#include "osapi.h"

#include <Arduino.h>
#include "Ticker.h"

void Ticker::_attach(Ticker::Milliseconds milliseconds, bool repeat)
{
    if (_timer) {
        os_timer_disarm(_timer);
    } else {
        _timer = &_timer_internal;
    }

    os_timer_setfn(_timer,
        [](void* ptr) {
            auto* ticker = reinterpret_cast<Ticker*>(ptr);
            ticker->_static_callback();
        }, this);

    // whenever duration excedes this limit, make timer repeatable N times
    // in case it is really repeatable, it will reset itself and continue as usual
    _tick = callback_tick_t{};
    _tick.repeat = repeat;

    if (milliseconds > DurationMax) {
        _tick.total = 1;
        while (milliseconds > DurationMax) {
            _tick.total *= 2;
            milliseconds /= 2;
        }
        repeat = true;
    }

    os_timer_arm(_timer, milliseconds.count(), repeat);
}

void Ticker::detach()
{
    if (_timer) {
        os_timer_disarm(_timer);
        _timer = nullptr;
        _tick = callback_tick_t{};
        _callback = std::monostate{};
    }
}

bool Ticker::active() const
{
    return _timer != nullptr;
}

void Ticker::_static_callback()
{
    if (_tick.total) {
        ++_tick.count;
        if (_tick.count < _tick.total) {
            return;
        }
    }

    std::visit([](auto&& callback) {
        using T = std::decay_t<decltype(callback)>;
        if constexpr (std::is_same_v<T, callback_ptr_t>) {
            callback.func(callback.arg);
        } else if constexpr (std::is_same_v<T, callback_function_t>) {
            callback();
        }
    }, _callback);

    if (_tick.total) {
        _tick.count = 0;
    }

    if (!_tick.repeat) {
        detach();
    }
}
