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
static os_timer_t micros_overflow_timer;
static uint32_t micros_at_last_overflow_tick = 0;
static uint32_t micros_overflow_count = 0;
#define ONCE 0
#define REPEAT 1

void delay_end(void* arg) {
    (void) arg;
    esp_schedule();
}

void delay(unsigned long ms) {
    if(ms) {
        os_timer_setfn(&delay_timer, (os_timer_func_t*) &delay_end, 0);
        os_timer_arm(&delay_timer, ms, ONCE);
    } else {
        esp_schedule();
    }
    esp_yield();
    if(ms) {
        os_timer_disarm(&delay_timer);
    }
}

void micros_overflow_tick(void* arg) {
    (void) arg;
    uint32_t m = system_get_time();
    if(m < micros_at_last_overflow_tick)
        ++micros_overflow_count;
    micros_at_last_overflow_tick = m;
}

unsigned long ICACHE_RAM_ATTR millis() {
    uint32_t m = system_get_time();
    uint32_t c = micros_overflow_count + ((m < micros_at_last_overflow_tick) ? 1 : 0);
    return c * 4294967 + m / 1000;
}

unsigned long ICACHE_RAM_ATTR micros() {
    return system_get_time();
}

uint64_t ICACHE_RAM_ATTR micros64() {
    uint32_t low32_us = system_get_time();
    uint32_t high32_us = micros_overflow_count + ((low32_us < micros_at_last_overflow_tick) ? 1 : 0);
    uint64_t duration64_us = (uint64_t)high32_us << 32 | low32_us;
    return duration64_us;
}

void ICACHE_RAM_ATTR delayMicroseconds(unsigned int us) {
    os_delay_us(us);
}

void init() {
    initPins();
    timer1_isr_init();
    os_timer_setfn(&micros_overflow_timer, (os_timer_func_t*) &micros_overflow_tick, 0);
    os_timer_arm(&micros_overflow_timer, 60000, REPEAT);
}
