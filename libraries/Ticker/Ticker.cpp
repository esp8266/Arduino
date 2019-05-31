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

#include <stddef.h>
#include <stdint.h>

#include "c_types.h"
#include "eagle_soc.h"
#include "osapi.h"

static const int ONCE   = 0;
static const int REPEAT = 1;

#include "Ticker.h"

Ticker::Ticker()
: _timer(nullptr)
{
}

Ticker::~Ticker()
{
	detach();
}

void Ticker::_attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, void* arg)
{
	if (_timer)
	{
		os_timer_disarm(_timer);
	}
	else
	{
		_timer = new(_etsTimerMemory) ETSTimer;
	}

	os_timer_setfn(_timer, reinterpret_cast<ETSTimerFunc*>(callback), arg);
	os_timer_arm(_timer, milliseconds, (repeat)?REPEAT:ONCE);
}

void Ticker::detach()
{
	if (!_timer)
		return;

	os_timer_disarm(_timer);
	_timer->~ETSTimer();
	_timer = nullptr;
	_callback_function = nullptr;
}

bool Ticker::active() const
{
	return (bool)_timer;
}

void Ticker::_static_callback(void* arg)
{
	Ticker* _this = reinterpret_cast<Ticker*>(arg);
	if (_this == nullptr)
	{
		return;
	}
	if (_this->_callback_function)
	{
		_this->_callback_function();
	}
}
