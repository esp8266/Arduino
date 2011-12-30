/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

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

#include <stdint.h>
#include <rtc.h>
#include <intc.h>
#include "timer.h"


struct timeout_t {
        U32 tick; 
        U32 expire_at_tick;
        Bool expired;
        U8 type;
        void (*cb)(void* ctx);
        void* ctx;
};

struct timer_t {
        volatile U32 tick;
        struct timeout_t timeout[8];
        void (*tick_isr) (void* ctx);
        const U32 MS_PER_TICK;
	void *ctx;
};

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

static __attribute__((__interrupt__)) void irq_handler(void);

static struct timer_t TIMER = {
        .tick = 0,
        .MS_PER_TICK = TIMER_HZ,
        .timeout = { { 0 } },
};

void timer_init(void (*tick_isr) (void* ctx), void* ctx)
{
        struct timer_t* priv = &TIMER;
        uint8_t id;
        
        INTC_register_interrupt(&irq_handler, AVR32_RTC_IRQ, AVR32_INTC_INT0);
        if (!rtc_init(&AVR32_RTC, RTC_OSC_RC, 0))
                Assert(0);

	priv->tick_isr = tick_isr;
	priv->ctx = ctx;
        rtc_set_top_value(&AVR32_RTC, 115 * priv->MS_PER_TICK / 2);
        rtc_enable_interrupt(&AVR32_RTC);
        rtc_enable(&AVR32_RTC);

        for (id = 0; id < ARRAY_SIZE(priv->timeout); id++)
                priv->timeout[id].expired = TRUE;
}


U32 timer_get_ms(void)
{
        struct timer_t* priv = &TIMER;
        return priv->tick * priv->MS_PER_TICK;
}

void timer_delay(U32 ms)
{
        struct timer_t* priv = &TIMER;
        U32 expire_at_tick = priv->tick + ms / priv->MS_PER_TICK;
        while (priv->tick < expire_at_tick);
}

/** 
 * Called from application main loop to invoke any scheduled timeout cbs.
 * This function might be called as often as possible rather than at each tick
 * to support the timeout value '0', e.g a timeout within less than one tick.
 * 
 */
void timer_poll(void)
{
        struct timer_t* priv = &TIMER;
        U8 i;

        for (i = 0; i < ARRAY_SIZE(priv->timeout); i++) {
                struct timeout_t* tmo = &priv->timeout[i];
                if (tmo->expired)
                        continue;
        
                if (tmo->expire_at_tick > priv->tick)
                        continue;

                if (tmo->cb)
                        tmo->cb(tmo->ctx);
                
                if (tmo->type == TIMEOUT_PERIODIC)
                        tmo->expire_at_tick = priv->tick + tmo->tick;
                else
                        tmo->expired = TRUE;
        }
}

static U32 timer_sched_timeout(U32 ms, U8 type)
{
        struct timer_t* priv = &TIMER;
        struct timeout_t* tmo;
        U8 id;

        Assert(type == TIMEOUT_ONESHOT || type == TIMEOUT_PERIODIC);
        
        for (id = 0; id < ARRAY_SIZE(priv->timeout); id++) {
                tmo = &priv->timeout[id];
                if (tmo->expired)
                        break;
        }

        Assert(id != ARRAY_SIZE(priv->timeout));

        tmo->tick = ms / priv->MS_PER_TICK;
        tmo->expire_at_tick = priv->tick + tmo->tick;
        tmo->type = type;
        tmo->expired = FALSE;
        return id;
}

U32 timer_sched_timeout_cb(U32 ms, U8 type, void (*cb)(void *ctx), void* ctx)
{
        struct timer_t* priv = &TIMER;
        struct timeout_t* tmo;
        U8 id;

        Assert(cb);
        id = timer_sched_timeout(ms, type);
        tmo = &priv->timeout[id];

        tmo->cb = cb;
        tmo->ctx = ctx;
        return id;
}


void timer_cancel_timeout(U32 id)
{
        struct timer_t* priv = &TIMER;
        struct timeout_t* tmo;
    
        tmo = &priv->timeout[id];
        tmo->expired = TRUE;
}

static __attribute__((__interrupt__)) void irq_handler(void)
{
        volatile avr32_rtc_t *rtc = &AVR32_RTC;
        struct timer_t* priv = &TIMER;
        priv->tick++;

        if(priv->tick_isr)
                priv->tick_isr(priv->ctx);
        
        rtc->icr = AVR32_RTC_ICR_TOPI_MASK;
        rtc->isr;
}
