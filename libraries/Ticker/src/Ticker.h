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
    // Our helper type to support any callable object
    // In case of a lambda with bound variable(s), it will be destroyed
    // either when the timer expires or detach() is called
    using callback_function_t = std::function<void()>;

    // Native SDK type, simple function with void* argument
    using callback_with_arg_t = void(*)(void*);

    // Helper type to allow type coercion on function argument
    // Only works with a function pointer. Argument *must not* be larger than the size of the `void*`
    template <typename T>
    using remove_cvref_t = typename std::remove_cv_t<
            typename std::remove_reference_t<T>>;

    template <typename T, typename Y = remove_cvref_t<T>>
    using callback_with_typed_arg_t = void(*)(Y);

    Ticker() = default;
    ~Ticker();

    Ticker(const Ticker&);
    Ticker& operator=(const Ticker&);

    Ticker(Ticker&&) noexcept;
    Ticker& operator=(Ticker&&) noexcept;

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

    // callback will still be called in SYS ctx when ticker fires
    template <typename Func, typename Arg>
    void attach(float seconds, Func func, Arg arg)
    {
        _callback = make_callback_ptr(func, arg);
        _attach(Seconds(seconds), true);
    }

    // callback will still be called in SYS ctx when ticker fires
    template <typename Func, typename Arg>
    void attach_ms(uint32_t milliseconds, Func func, Arg arg)
    {
        _callback = make_callback_ptr(func, arg);
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
    template <typename Func, typename Arg>
    void once(float seconds, Func func, Arg arg)
    {
        _callback = make_callback_ptr(func, arg);
        _attach(Seconds(seconds), false);
    }

    // callback will be called in SYS ctx when ticker fires
    template <typename Func, typename Arg>
    void once_ms(uint32_t milliseconds, Func func, Arg arg)
    {
        _callback = make_callback_ptr(func, arg);
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

    std::unique_ptr<callback_tick_t> _tick;
    bool _repeat = false;

    ETSTimer* _timer = nullptr;

private:
    struct callback_ptr_t
    {
        callback_with_arg_t func;
        void* arg;
    };

    // original implementation inluded type coersion of integer values that would fit into uintptr_t
    // to avoid writing these in our every method, use a generic type that automatically converts it
    // (XXX it is a weird hack, though, consider removing this in the future and prever void* instead)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
    template <typename T, typename Y = remove_cvref_t<T>>
    static callback_ptr_t make_callback_ptr(callback_with_typed_arg_t<Y> func, T arg) {
        static_assert(sizeof(Y) <= sizeof(void*), "");
        return callback_ptr_t{
            .func = reinterpret_cast<callback_with_arg_t>(func),
            .arg = reinterpret_cast<void*>(arg),
        };
    }
#pragma GCC diagnostic pop

    using callback_data_t = std::variant<
        std::monostate,
        callback_ptr_t,
        callback_function_t>;

    callback_data_t _callback;
    ETSTimer _timer_internal{};
};
