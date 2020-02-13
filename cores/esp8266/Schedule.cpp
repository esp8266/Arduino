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
#include "MultiDelegate.h"
#include "PolledTimeout.h"
#include "interrupts.h"
#include "coredecls.h"

using mSchedFuncT = Delegate<void(), void*>;
// queue specialization: cache erased nodes, call each once next round. 
MultiDelegate<mSchedFuncT, true> schedFuncs;

class mRecFuncT : public Delegate<bool(), void*>
{
public:
    using base_type = Delegate<bool(), void*>;
    mRecFuncT() : callNow(esp8266::polledTimeout::periodicFastUs::neverExpires) {}
    mRecFuncT(esp8266::polledTimeout::periodicFastUs interval) : callNow(interval) { }
    using base_type::operator=;
    esp8266::polledTimeout::periodicFastUs callNow;
    Delegate<bool(), void*> alarm = nullptr;
    // return true to keep, false to erase.
    bool IRAM_ATTR operator()()
    {
        const bool wakeup = alarm && alarm();
        bool callNow = this->callNow;
        return !(wakeup || callNow) || base_type::operator()();
    }
};
// non-queue specialization: heap new/delete, use explicit erase. 
MultiDelegate<mRecFuncT> recFuncs;

IRAM_ATTR // (not only) called from ISR
bool schedule_function(const Delegate<void(), void*>& fn)
{
    return schedFuncs.add(fn);
}

IRAM_ATTR // (not only) called from ISR
bool schedule_recurrent_function_us(const Delegate<bool(), void*>& fn,
    uint32_t repeat_us, const Delegate<bool(), void*>& alarm)
{
    assert(repeat_us < decltype(mRecFuncT::callNow)::neverExpires); //~26800000us (26.8s)

    if (!fn)
        return false;

    mRecFuncT func(repeat_us);
    func = fn;
    func.alarm = alarm;

    return recFuncs.add(std::move(func));
}

void run_scheduled_functions()
{
    schedFuncs();
}

void run_scheduled_recurrent_functions()
{
    auto it = recFuncs.begin();
    if (!it)
        return;

    static std::atomic<bool> fence(false);
    // prevent recursive calls
    if (fence.load()) return;
    fence.store(true);

    auto end = recFuncs.end();
    do
    {
        if ((*it)())
            ++it;
        else
            it = recFuncs.erase(it);
        // running callbacks might last too long for watchdog etc.
        optimistic_yield(10000);
    } while (it != end);

    fence.store(false);
}
