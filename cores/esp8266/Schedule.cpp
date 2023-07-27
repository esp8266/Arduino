/*
 Schedule.cpp - Scheduled functions.
 Copyright (c) 2020 esp8266/Arduino
 
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

#include <assert.h>

#include "Schedule.h"
#include "PolledTimeout.h"
#include "interrupts.h"

typedef std::function<void(void)> mSchedFuncT;
struct scheduled_fn_t
{
    scheduled_fn_t* mNext = nullptr;
    mSchedFuncT mFunc;
};

static scheduled_fn_t* sFirst = nullptr;
static scheduled_fn_t* sLast = nullptr;
static scheduled_fn_t* sUnused = nullptr;
static int sCount = 0;

typedef std::function<bool(void)> mRecFuncT;
struct recurrent_fn_t
{
    recurrent_fn_t* mNext = nullptr;
    mRecFuncT mFunc;
    esp8266::polledTimeout::periodicFastUs callNow;
    std::function<bool(void)> alarm = nullptr;
    recurrent_fn_t(esp8266::polledTimeout::periodicFastUs interval) : callNow(interval) { }
};

static recurrent_fn_t* rFirst = nullptr;
static recurrent_fn_t* rLast = nullptr;
// The target time for scheduling the next timed recurrent function 
static decltype(micros()) rTarget;

constexpr decltype(micros()) HALF_MAX_MICROS = ~static_cast<decltype(micros())>(0) >> 1;

// Returns a pointer to an unused sched_fn_t,
// or if none are available allocates a new one,
// or nullptr if limit is reached
IRAM_ATTR // called from ISR
static scheduled_fn_t* get_fn_unsafe()
{
    scheduled_fn_t* result = nullptr;
    // try to get an item from unused items list
    if (sUnused)
    {
        result = sUnused;
        sUnused = sUnused->mNext;
    }
    // if no unused items, and count not too high, allocate a new one
    else if (sCount < SCHEDULED_FN_MAX_COUNT)
    {
        result = new (std::nothrow) scheduled_fn_t;
        if (result)
            ++sCount;
    }
    return result;
}

static void recycle_fn_unsafe(scheduled_fn_t* fn)
{
    fn->mFunc = nullptr; // special overload in c++ std lib
    fn->mNext = sUnused;
    sUnused = fn;
}

IRAM_ATTR // (not only) called from ISR
bool schedule_function(const std::function<void(void)>& fn)
{
    if (!fn)
        return false;

    esp8266::InterruptLock lockAllInterruptsInThisScope;

    scheduled_fn_t* item = get_fn_unsafe();
    if (!item)
        return false;

    item->mFunc = fn;
    item->mNext = nullptr;

    if (sFirst)
        sLast->mNext = item;
    else
        sFirst = item;
    sLast = item;

    return true;
}

IRAM_ATTR // (not only) called from ISR
bool schedule_recurrent_function_us(const std::function<bool(void)>& fn,
    decltype(micros()) repeat_us, const std::function<bool(void)>& alarm)
{
    assert(repeat_us < decltype(recurrent_fn_t::callNow)::neverExpires); //~26800000us (26.8s)

    if (!fn)
        return false;

    recurrent_fn_t* item = new (std::nothrow) recurrent_fn_t(repeat_us);
    if (!item)
        return false;

    item->mFunc = fn;
    item->alarm = alarm;

    esp8266::InterruptLock lockAllInterruptsInThisScope;

    // prevent new item overwriting an already expired rTarget.
    const auto now = micros();
    const auto itemRemaining = item->callNow.remaining();
    const int32_t remaining = rTarget - now;
    if (!rFirst || (remaining > 0 && static_cast<decltype(micros())>(remaining) > itemRemaining))
    {
        rTarget = now + itemRemaining;
    }

    if (rLast)
    {
        rLast->mNext = item;
    }
    else
    {
        rFirst = item;
    }
    rLast = item;

    return true;
}

decltype(micros()) get_scheduled_recurrent_delay_us()
{
    if (!rFirst) return HALF_MAX_MICROS;
    // handle already expired rTarget.
    const int32_t remaining = rTarget - micros();
    return (remaining > 0) ? static_cast<decltype(micros())>(remaining) : 0;
}

decltype(micros()) get_scheduled_delay_us()
{
    return sFirst ? 0 : HALF_MAX_MICROS;
}

void run_scheduled_functions()
{
    // prevent scheduling of new functions during this run
    auto stop = sLast;
    bool done = false;
    while (sFirst && !done)
    {
        done = sFirst == stop;

        sFirst->mFunc();

        {
            // remove function from stack
            esp8266::InterruptLock lockAllInterruptsInThisScope;

            auto to_recycle = sFirst;

            // removing rLast
            if (sLast == sFirst)
                sLast = nullptr;

            sFirst = sFirst->mNext;

            recycle_fn_unsafe(to_recycle);
        }

        // scheduled functions might last too long for watchdog etc.
        // yield() is allowed in scheduled functions, therefore
        // recursion into run_scheduled_recurrent_functions() is permitted
        optimistic_yield(100000);
    }
}

void run_scheduled_recurrent_functions()
{
    esp8266::polledTimeout::periodicFastMs yieldNow(100); // yield every 100ms

    // Note to the reader:
    // There is no exposed API to remove a scheduled function:
    // Scheduled functions are removed only from this function, and
    // its purpose is that it is never called from an interrupt
    // (always on cont stack).

    auto current = rFirst;
    if (!current)
        return;

    static bool fence = false;
    {
        // fence is like a mutex but as we are never called from ISR,
        // locking is useless here. Leaving comment for reference.
        //esp8266::InterruptLock lockAllInterruptsInThisScope;

        if (fence)
            // prevent recursive calls from yield()
            // (even if they are not allowed)
            return;
        fence = true;
    }

    decltype(rLast) stop;
    recurrent_fn_t* prev = nullptr;
    bool done;

    // prevent scheduling of new functions during this run
    stop = rLast;
    rTarget = micros() + HALF_MAX_MICROS;

    do
    {
        done = current == stop;
        const bool wakeup = current->alarm && current->alarm();
        bool callNow = current->callNow;

        if ((wakeup || callNow) && !current->mFunc())
        {
            // remove function from stack
            esp8266::InterruptLock lockAllInterruptsInThisScope;

            auto to_ditch = current;

            // removing rLast
            if (rLast == current)
                rLast = prev;

            current = current->mNext;
            if (prev)
            {
                prev->mNext = current;
            }
            else
            {
                rFirst = current;
            }

            delete(to_ditch);
        }
        else
        {
            esp8266::InterruptLock lockAllInterruptsInThisScope;

            // prevent current item overwriting an already expired rTarget.
            const auto now = micros();
            const auto currentRemaining = current->callNow.remaining();
            const int32_t remaining = rTarget - now;
            if (remaining > 0 && static_cast<decltype(micros())>(remaining) > currentRemaining)
            {
                rTarget = now + currentRemaining;
            }

            prev = current;
            current = current->mNext;
        }

        if (yieldNow)
        {
            // because scheduled functions might last too long for watchdog etc,
            // this is yield() in cont stack, but need to call cont_suspend directly
            // to prevent recursion into run_scheduled_recurrent_functions()
            esp_schedule();
            cont_suspend(g_pcont);
        }
    } while (current && !done);

    fence = false;
}
