/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id$
*/

#include "wiring_private.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "cont.h"

extern cont_t g_cont;
extern void loop_schedule();

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
    loop_schedule();
}

void delay(unsigned long ms)
{
    os_timer_setfn(&delay_timer, (os_timer_func_t*) &delay_end, 0);
    os_timer_arm(&delay_timer, ms, ONCE);
    cont_yield(&g_cont);
    os_timer_disarm(&delay_timer);
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
    // system_timer_reinit();
}
