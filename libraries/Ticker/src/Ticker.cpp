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

// ETSTimer is part of the instance, and we don't have any state besides
// the things required for the callback. Allow copies and moves, but
// disable any member copies and default-init + detach() instead.

Ticker::~Ticker()
{
    detach();
}

Ticker::Ticker(const Ticker&)
{
}

Ticker& Ticker::operator=(const Ticker&)
{
    detach();
    return *this;
}

Ticker::Ticker(Ticker&& other) noexcept
{
    other.detach();
}

Ticker& Ticker::operator=(Ticker&& other) noexcept
{
    other.detach();
    detach();
    return *this;
}

void Ticker::_attach(Ticker::Milliseconds milliseconds, bool repeat)
{
    if (_timer) {
        os_timer_disarm(_timer);
    } else {
        _timer = &_timer_internal;
    }

    os_timer_setfn(_timer,
        [](void* ptr) {
            reinterpret_cast<Ticker*>(ptr)->_static_callback();
        }, this);

    _repeat = repeat;

    // whenever duration excedes this limit, make timer repeatable N times
    // in case it is really repeatable, it will reset itself and continue as usual
    uint32_t total = 0;
    if (milliseconds > DurationMax) {
        total = 1;
        while (milliseconds > DurationMax) {
            total *= 2;
            milliseconds /= 2;
        }
        _tick.reset(new callback_tick_t{
            .total = total,
            .count = 0,
        });
        repeat = true;
    }

    os_timer_arm(_timer, milliseconds.count(), repeat);
}

void Ticker::detach()
{
    if (_timer) {
        os_timer_disarm(_timer);
        _timer = nullptr;
        _tick.reset(nullptr);
        _callback = std::monostate{};
    }
}

bool Ticker::active() const
{
    return _timer != nullptr;
}

void Ticker::_static_callback()
{
    if (_tick) {
        ++_tick->count;
        if (_tick->count < _tick->total) {
            return;
        }
    }

    // it is technically allowed to call either schedule or detach
    // *during* callback execution. allow both to happen
    decltype(_callback) tmp;
    std::swap(tmp, _callback);

    std::visit([](auto&& callback) {
        using T = std::decay_t<decltype(callback)>;
        if constexpr (std::is_same_v<T, callback_ptr_t>) {
            callback.func(callback.arg);
        } else if constexpr (std::is_same_v<T, callback_function_t>) {
            callback();
        }
    }, tmp);

    // ...and move ourselves back only when object is in a valid state
    // * ticker was not detached, zeroing timer pointer
    // * nothing else replaced callback variant
    if ((_timer == nullptr) || !std::holds_alternative<std::monostate>(_callback)) {
        return;
    }

    std::swap(tmp, _callback);

    if (_repeat) {
        if (_tick) {
            _tick->count = 0;
        }
        return;
    }

    detach();
}
