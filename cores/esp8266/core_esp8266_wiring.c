/* 
  core_esp8266_wiring.c - implementation of Wiring API for esp8266

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


#include "wiring_private.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "cont.h"

extern void esp_schedule();
extern void esp_yield();

static os_timer_t delay_timer;
#define ONCE 0
#define REPEAT 1

unsigned long millis()
{
    unsigned long m = system_get_time() / 1000;
    return m;
}

void delay_end(void* arg)
{
    esp_schedule();
}

void delay(unsigned long ms)
{
    if (ms)
    {
        os_timer_setfn(&delay_timer, (os_timer_func_t*) &delay_end, 0);
        os_timer_arm(&delay_timer, ms, ONCE);
    }
    else
    {
        esp_schedule();
    }
    esp_yield();
    if (ms)
    {
        os_timer_disarm(&delay_timer);
    }
}

unsigned long micros() 
{
    unsigned long m = system_get_time();
    return m;
}

void delayMicroseconds(unsigned int us)
{
    os_delay_us(us);
}

void init()
{
    initPins();
}
