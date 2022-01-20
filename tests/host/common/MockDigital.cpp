/*
  digital.c - wiring digital implementation for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
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
#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"
#include "c_types.h"
#include "eagle_soc.h"
#include "ets_sys.h"
#include "user_interface.h"
#include "core_esp8266_waveform.h"
#include "interrupts.h"

extern "C"
{
    static uint8_t _mode[17];
    static uint8_t _gpio[17];

    extern void    pinMode(uint8_t pin, uint8_t mode)
    {
        if (pin < 17)
        {
            _mode[pin] = mode;
        }
    }

    extern void digitalWrite(uint8_t pin, uint8_t val)
    {
        if (pin < 17)
        {
            _gpio[pin] = val;
        }
    }

    extern int digitalRead(uint8_t pin)
    {
        if (pin < 17)
        {
            return _gpio[pin] != 0;
        }
        else
        {
            return 0;
        }
    }
};
