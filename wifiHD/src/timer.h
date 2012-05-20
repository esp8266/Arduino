/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

enum {
        TIMEOUT_ONESHOT,
        TIMEOUT_PERIODIC
};

#define INVALID_TIMER_ID 0xFFFFFFFF

/* Handle timer overflows. Return 1 if the interval has passed. */
int timer_interval_passed(uint32_t old, uint32_t new, uint32_t diff);

void timer_tick();
void timer_init(void (*tick_isr) (void* ctx), void* ctx);
void timer_poll(void);
void timer_delay(uint32_t ms);
uint32_t timer_sched_timeout_cb(uint32_t ms, uint8_t type, void (*cb)(void *ctx), void* ctx);
uint32_t timer_mod(uint32_t id, uint32_t ms, uint8_t type, void (*cb)(void *ctx), void* ctx);
void timer_cancel_timeout(uint32_t id);
uint32_t timer_get_ms(void);

#endif
