/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

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

#include <Arduino.h>
extern "C" {
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
}

//Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (*func)()) { return 0; }

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

extern void loop();
extern void setup();

#define LOOP_TASK_PRIORITY 0
#define LOOP_QUEUE_SIZE    1

os_event_t loop_queue[LOOP_QUEUE_SIZE];

void loop_task(os_event_t *events)
{
    loop();
    system_os_post(LOOP_TASK_PRIORITY, 0, 0);
}

extern "C" {
void user_init(void)
{
    init();

    initVariant();

    system_os_task( loop_task,
                    LOOP_TASK_PRIORITY,
                    loop_queue,
                    LOOP_QUEUE_SIZE);
    setup();
    loop_task(0);
}
}



