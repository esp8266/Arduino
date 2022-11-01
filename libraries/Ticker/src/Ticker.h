/*
  Ticker.h - esp8266 library that calls functions periodically

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

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <variant>

#include <Arduino.h>
#include <Schedule.h>
#include <ets_sys.h>

class Ticker
{
public:
    Ticker() = default;

    ~Ticker()
    {
        detach();
    }

    // TODO disable existing timr? =default to retain backwards compatibility
    Ticker(const Ticker&) = default;
    Ticker& operator=(const Ticker&) = default;

    // TODO re-arm or disable the timer? =default to retain backwards compatibility
    Ticker(Ticker&&) = default;
    Ticker& operator=(Ticker&&) = default;

    // Native SDK type, simple function with void* argument
    using callback_with_arg_t = void(*)(void*);

    // Our helper type to support any callable object
    // In case of a lambda with bound variable(s), it will be destroyed
    // either when the timer expires or detach() is called
    using callback_function_t = std::function<void()>;

    // callback will be called at following loop() after ticker fires
    void attach_scheduled(float seconds, callback_function_t callback)
    {
        _callback = [callback]() {
            schedule_function(callback);
        };
        _attach(Seconds(seconds), true);
    }

    // callback will be called in SYS ctx when ticker fires
    void attach(float seconds, callback_function_t callback)
    {
        _callback = std::move(callback);
        _attach(Seconds(seconds), true);
    }

    // callback will be called at following loop() after ticker fires
    void attach_ms_scheduled(uint32_t milliseconds, callback_function_t callback)
    {
        _callback = [callback]() {
            schedule_function(callback);
        };
        _attach(Milliseconds(milliseconds), true);
    }

    // callback will be called at following yield() after ticker fires
    void attach_ms_scheduled_accurate(uint32_t milliseconds, callback_function_t callback)
    {
        _callback = [callback]() {
            schedule_recurrent_function_us([callback]() {
                callback();
                return false;
            }, 0);
        };
        _attach(Milliseconds(milliseconds), true);
    }

    // callback will be called in SYS ctx when ticker fires
    void attach_ms(uint32_t milliseconds, callback_function_t callback)
    {
        _callback = std::move(callback);
        _attach(Milliseconds(milliseconds), true);
    }

    // only works with a function pointer. argument *may not* be larger than the size of the `void*`
    template <typename T>
    using callback_with_typed_arg_t = void(*)(T);

    // callback will still be called in SYS ctx when ticker fires
    template <typename T>
    void attach(float seconds, callback_with_typed_arg_t<T> callback, T arg)
    {
        _callback = callback_ptr_t(callback, arg);
        _attach(Seconds(seconds), true);
    }

    // callback will still be called in SYS ctx when ticker fires
    template <typename T>
    void attach_ms(uint32_t milliseconds, callback_with_typed_arg_t<T> callback, T arg)
    {
        _callback = callback_ptr_t(callback, arg);
        _attach(Milliseconds(milliseconds), true);
    }

    // callback will be called at following loop() after ticker fires
    void once_scheduled(float seconds, callback_function_t callback)
    {
        _callback = [callback]() { schedule_function(callback); };
        _attach(Seconds(seconds), false);
    }

    // callback will be called in SYS ctx when ticker fires
    void once(float seconds, callback_function_t callback)
    {
        _callback = std::move(callback);
        _attach(Seconds(seconds), false);
    }

    // callback will be called at following loop() after ticker fires
    void once_ms_scheduled(uint32_t milliseconds, callback_function_t callback)
    {
        _callback = [callback]() { schedule_function(callback); };
        _attach(Milliseconds(milliseconds), false);
    }

    // callback will be called in SYS ctx when ticker fires
    void once_ms(uint32_t milliseconds, callback_function_t callback)
    {
        _callback = std::move(callback);
        _attach(Milliseconds(milliseconds), false);
    }

    // callback will be called in SYS ctx when ticker fires
    template <typename T>
    void once(float seconds, callback_with_typed_arg_t<T> callback, T arg)
    {
        _callback = callback_ptr_t(callback, arg);
        _attach(Seconds(seconds), false);
    }

    // callback will be called in SYS ctx when ticker fires
    template <typename T>
    void once_ms(uint32_t milliseconds, callback_with_typed_arg_t<T> callback, T arg)
    {
        _callback = callback_ptr_t(callback, arg);
        _attach(Milliseconds(milliseconds), false);
    }

    // if active(), disables currently running timer
    void detach();

    bool active() const;

    explicit operator bool() const {
        return active();
    }

protected:
    // internals use this as duration
    using Milliseconds = std::chrono::duration<uint32_t, std::ratio<1, 1000>>;

    // we allow a floating point as input as well
    // float -> u32 has some precision issues, though
    using Seconds = std::chrono::duration<float, std::ratio<1>>;

    // NONOS SDK timer object duration cannot be longer than 6870947 (0x68D7A3)
    // when that's the case, we split execution into multiple 'ticks'
    static constexpr auto DurationMax = Milliseconds(6870947);

    struct callback_tick_t
    {
        uint32_t total = 0;
        uint32_t count = 0;
    };

    void _static_callback();

    void _attach(Milliseconds milliseconds, bool repeat);
    void _attach(Seconds seconds, bool repeat)
    {
        _attach(std::chrono::duration_cast<Milliseconds>(seconds), repeat);
    }

    ETSTimer* _timer = nullptr;

private:
        // XXX undefined behaviour? fix next pr
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
    struct callback_ptr_t
    {
        template <typename T>
        callback_ptr_t(callback_with_typed_arg_t<T> func, T arg) :
            func(reinterpret_cast<callback_with_arg_t>(func)),
            arg(reinterpret_cast<void*>(arg))
        {
            static_assert(sizeof(T) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
        }

        callback_with_arg_t func = nullptr;
        void* arg = nullptr;
    };
#pragma GCC diagnostic pop

    using callback_data_t = std::variant<
        std::monostate,
        callback_ptr_t,
        callback_function_t>;

    callback_data_t _callback;

    std::unique_ptr<callback_tick_t> _tick;
    bool _repeat = false;

    ETSTimer _timer_internal;
};
