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

#ifndef TICKER_H
#define TICKER_H

#include <Delegate.h>
#include <Schedule.h>
#include <ets_sys.h>

class Ticker
{
public:
    Ticker();
    ~Ticker();

    using callback_t = Delegate<void(), void*>;
    using callback_with_arg_t = void(void*);

    // callback will be called at following loop() after ticker fires
    void attach_scheduled(float seconds, callback_t callback)
    {
        _callback = std::bind(schedule_function, callback);
        _attach_ms(1000UL * seconds, true, _callback, _callback.arg());
    }

    // callback will be called in SYS ctx when ticker fires
    void attach(float seconds, callback_t callback)
    {
        _callback = std::move(callback);
        _attach_ms(1000UL * seconds, true, _callback, _callback.arg());
    }

    // callback will be called at following loop() after ticker fires
    void attach_ms_scheduled(uint32_t milliseconds, callback_t callback)
    {
        _callback = std::bind(schedule_function, callback);
        _attach_ms(milliseconds, true, _callback, _callback.arg());
    }

    // callback will be called at following yield() after ticker fires
    void attach_ms_scheduled_accurate(uint32_t milliseconds, callback_t callback)
    {
        _callback = std::bind(schedule_recurrent_function_us, [callback]() { callback(); return false; }, 0, nullptr);
        _attach_ms(milliseconds, true, _callback, _callback.arg());
    }

    // callback will be called in SYS ctx when ticker fires
    void attach_ms(uint32_t milliseconds, callback_t callback)
    {
        _callback = std::move(callback);
        _attach_ms(milliseconds, true, _callback, _callback.arg());
    }

    // callback will be called in SYS ctx when ticker fires
    template<typename TArg>
    void attach(float seconds, void (*callback)(TArg), TArg arg)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
        static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
        _attach_ms(1000UL * seconds, true, reinterpret_cast<callback_with_arg_t*>(callback), reinterpret_cast<void*>(arg));
#pragma GCC diagnostic pop
    }

    // callback will be called in SYS ctx when ticker fires
    template<typename TArg>
    void attach_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
        static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
        _attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t*>(callback), reinterpret_cast<void*>(arg));
#pragma GCC diagnostic pop
    }

    // callback will be called at following loop() after ticker fires
    void once_scheduled(float seconds, callback_t callback)
    {
        _callback = std::bind(schedule_function, callback);
        _attach_ms(1000UL * seconds, false, _callback, _callback.arg());
    }

    // callback will be called in SYS ctx when ticker fires
    void once(float seconds, callback_t callback)
    {
        _callback = std::move(callback);
        _attach_ms(1000UL * seconds, false, _callback, _callback.arg());
    }

    // callback will be called at following loop() after ticker fires
    void once_ms_scheduled(uint32_t milliseconds, callback_t callback)
    {
        _callback = std::bind(schedule_function, callback);
        _attach_ms(milliseconds, false, _callback, _callback.arg());
    }

    // callback will be called in SYS ctx when ticker fires
    void once_ms(uint32_t milliseconds, callback_t callback)
    {
        _callback = std::move(callback);
        _attach_ms(milliseconds, false, _callback, _callback.arg());
    }

    // callback will be called in SYS ctx when ticker fires
    template<typename TArg>
    void once(float seconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
        _attach_ms(1000UL * seconds, false, reinterpret_cast<callback_with_arg_t*>(callback), reinterpret_cast<void*>(arg));
    }

    // callback will be called in SYS ctx when ticker fires
    template<typename TArg>
    void once_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
        _attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t*>(callback), reinterpret_cast<void*>(arg));
    }

    void detach();
    bool active() const;

protected:
    void _attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t* callback, void* arg);

    ETSTimer* _timer;
    callback_t _callback = nullptr;

private:
    ETSTimer _etsTimer;
};


#endif //TICKER_H
