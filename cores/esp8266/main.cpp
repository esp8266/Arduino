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
#include "cont.h"
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

cont_t g_cont;

os_event_t loop_queue[LOOP_QUEUE_SIZE];

bool g_setup_done = false;

extern "C" void loop_schedule()
{
    system_os_post(LOOP_TASK_PRIORITY, 0, 0);
}

static void loop_wrapper()
{
    if (!g_setup_done)
    {
        g_setup_done = true;
        setup();
    }

    loop();
    loop_schedule();
}

static void loop_task(os_event_t *events)
{
    cont_run(&g_cont, &loop_wrapper);
    if (cont_check(&g_cont) != 0)
    {
        abort();
    }
}

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

static void do_global_ctors(void)
{
    void (**p)(void);
    for (p = &__init_array_start; p != &__init_array_end; ++p)
            (*p)();
}

void init_done()
{
    loop_schedule();
    
    int i = ((char*)__init_array_end) - (char*)__init_array_start;
    os_printf("\r\nInit array size: %d\r\n", i);
}

extern "C" {
void user_init(void)
{
    do_global_ctors();
    uart_div_modify(0, UART_CLK_FREQ / (115200));

    init();

    initVariant();

    cont_init(&g_cont);

    system_os_task( loop_task,
                    LOOP_TASK_PRIORITY,
                    loop_queue,
                    LOOP_QUEUE_SIZE);

    system_init_done_cb(&init_done);
}
}

