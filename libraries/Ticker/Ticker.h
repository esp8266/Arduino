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

#include <functional>
#include <Schedule.h>
#include <ets_sys.h>

class Ticker
{
public:
	Ticker();
	~Ticker();

	typedef void (*callback_with_arg_t)(void *);
	typedef std::function<void(void)> callback_function_t;

	void attach_scheduled(float seconds, callback_function_t callback)
	{
		attach(seconds, [callback]() { schedule_function(callback); });
	}

	void attach(float seconds, callback_function_t callback)
	{
		_callback_function = std::move(callback);
		_attach_s(seconds, true, _static_callback, this);
	}

	void attach_ms_scheduled(uint32_t milliseconds, callback_function_t callback)
	{
		attach_ms(milliseconds, [callback]() { schedule_function(callback); });
	}

	void attach_ms(uint32_t milliseconds, callback_function_t callback)
	{
		_callback_function = std::move(callback);
		_attach_ms(milliseconds, true, _static_callback, this);
	}

	template<typename TArg>
	void attach(float seconds, void (*callback)(TArg), TArg arg)
	{
		static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
		// C-cast serves two purposes:
		// static_cast for smaller integer types,
		// reinterpret_cast + const_cast for pointer types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
		_attach_s(seconds, true, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
#pragma GCC diagnostic push
	}

	template<typename TArg>
	void attach_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
	{
		static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
		// We know now that we can do a CB and pass back a uint32 for the receiver to interpret as they wish.  Disable GCC 9 warning about this
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
		_attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
#pragma GCC diagnostic push
	}

	void once_scheduled(float seconds, callback_function_t callback)
	{
		once(seconds, [callback]() { schedule_function(callback); });
	}

	void once(float seconds, callback_function_t callback)
	{
		_callback_function = std::move(callback);
		_attach_s(seconds, false, _static_callback, this);
	}

	void once_ms_scheduled(uint32_t milliseconds, callback_function_t callback)
	{
		once_ms(milliseconds, [callback]() { schedule_function(callback); });
	}

	void once_ms(uint32_t milliseconds, callback_function_t callback)
	{
		_callback_function = std::move(callback);
		_attach_ms(milliseconds, false, _static_callback, this);
	}

	template<typename TArg>
	void once(float seconds, void (*callback)(TArg), TArg arg)
	{
		static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
		_attach_s(seconds, false, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
	}

	template<typename TArg>
	void once_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
	{
		static_assert(sizeof(TArg) <= sizeof(void*), "attach() callback argument size must be <= sizeof(void*)");
		_attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
	}

	void detach();
	bool active() const;

protected:
	void _attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, void* arg);
	static void _static_callback(void* arg);

	ETSTimer* _timer;
	callback_function_t _callback_function = nullptr;

private:
	void _attach_s(float seconds, bool repeat, callback_with_arg_t callback, void* arg);
	ETSTimer _etsTimer;
};


#endif//TICKER_H
