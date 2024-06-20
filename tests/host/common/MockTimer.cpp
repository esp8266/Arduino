/*
 Arduino emulation - tools
 Copyright (c) 2018 david gauchard. All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal with the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 - Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimers.

 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimers in the
   documentation and/or other materials provided with the distribution.

 - The names of its contributors may not be used to endorse or promote
   products derived from this Software without specific prior written
   permission.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS WITH THE SOFTWARE.
*/

#include <ets_sys.h>

#include <ctime>
#include <csignal>

#include <chrono>
#include <mutex>
#include <forward_list>

namespace mock
{
namespace timer
{
    namespace
    {

        // since attach() uses user-provided ETSTimer* to store the data,
        // the only possible time type is u32 and not the common host i64
        using Microseconds = std::chrono::duration<uint32_t, std::micro>;
        using Milliseconds = std::chrono::duration<uint32_t, std::milli>;

        ETSTimer*  timers { nullptr };
        std::mutex m;

        // ...but, it is possible to upcast to i64 and compare timestamps vs.
        // possibly maintaining two timer lists for before and after overflow

        // checks whether lhs 'timestamp' is after rhs
        constexpr bool after(Microseconds lhs, Microseconds rhs)
        {
            return (int64_t(rhs.count()) - int64_t(lhs.count())) < 0ll;
        }

        bool after(ETSTimer* lhs, ETSTimer* rhs)
        {
            return after(Microseconds { lhs->timer_expire }, Microseconds { rhs->timer_expire });
        }

        // TODO move this to micros() and millis()?
        Microseconds steady_clock_now()
        {
            using clock    = std::chrono::high_resolution_clock;
            const auto now = clock::now().time_since_epoch();

            const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now);
            const auto max    = clock::rep { std::numeric_limits<uint32_t>::max() };

            return Microseconds { static_cast<uint32_t>(micros.count() % max) };
        }

        void detach(const std::lock_guard<std::mutex>&, ETSTimer* user_timer)
        {
            ETSTimer* prev = nullptr;
            ETSTimer* head = timers;
            while (head != nullptr)
            {
                if ((user_timer == head) && (prev != nullptr))
                {
                    prev->timer_next = head->timer_next;
                    break;
                }

                prev = head;
                head = head->timer_next;
            }

            user_timer->timer_expire = 0;
            user_timer->timer_next   = nullptr;
        }

        void detach(ETSTimer* user_timer)
        {
            auto lock = std::lock_guard { m };
            detach(lock, user_timer);
        }

        // aka timer_insert(). attach timer to the current list of pointers,
        // making sure it is sorted based on expiration time (earliest to latest)
        void attach_sorted(ETSTimer* timer)
        {
            ETSTimer* prev = nullptr;
            ETSTimer* head = timers;
            while (head != nullptr)
            {
                if (after(head, timer))
                {
                    break;
                }

                prev = head;
                head = head->timer_next;
            }

            if (prev != nullptr)
            {
                prev->timer_next = timer;
            }
            else
            {
                timers = timer;
            }

            timer->timer_next = head;
        }

        void attach(ETSTimer* user_timer, Microseconds duration, bool repeat)
        {
            if (duration.count() == 0)
            {
                return;
            }

            auto lock = std::lock_guard { m };
            detach(lock, user_timer);

            const auto expire        = duration + steady_clock_now();
            user_timer->timer_expire = expire.count();

            user_timer->timer_period = repeat ? duration.count() : 0;

            attach_sorted(user_timer);
        }

        void attach(ETSTimer* timer, int duration, int type, int isMillisecondsTimer)
        {
            const bool repeat = (type == 1);

            Microseconds musec;
            if (isMillisecondsTimer == 1)
            {
                musec = Milliseconds { duration };
            }
            else
            {
                musec = Microseconds { duration };
            }

            attach(timer, musec, repeat);
        }

        void setfn(ETSTimer* timer, ETSTimerFunc* func, void* arg)
        {
            timer->timer_func = func;
            timer->timer_arg  = arg;
        }

        struct Expired
        {
            ETSTimerFunc* func { nullptr };
            void*         arg { nullptr };

            Expired() = default;

            explicit operator bool() const
            {
                return func != nullptr;
            }

            void operator()() const
            {
                if (func != nullptr)
                {
                    func(arg);
                }
            }
        };

        void take_expired(Expired& out, const std::lock_guard<std::mutex>&)
        {
            ETSTimer* expired { nullptr };
            auto      now = steady_clock_now();

            if (after(now, Microseconds { timers->timer_expire }))
            {
                expired             = timers;
                timers              = timers->timer_next;
                expired->timer_next = nullptr;

                if (expired->timer_period != 0)
                {
                    expired->timer_expire = now.count() + expired->timer_period;
                    attach_sorted(expired);
                }
            }

            if (expired != nullptr)
            {
                out.func = expired->timer_func;
                out.arg  = expired->timer_arg;
            }
        }

    }  // namespace

    void loop()
    {
        Expired expired;

        if (timers != nullptr)
        {
            auto lock = std::lock_guard { m };
            if (timers != nullptr)
            {
                take_expired(expired, lock);
            }
        }

        expired();
    }

}  // namespace timer
}  // namespace mock

extern "C"
{
    void ets_timer_arm_new(ETSTimer* timer, int duration, int type, int isMillisecondsTimer)
    {
        mock::timer::attach(timer, duration, type, isMillisecondsTimer);
    }

    void ets_timer_setfn(ETSTimer* timer, ETSTimerFunc* fn, void* parg)
    {
        mock::timer::setfn(timer, fn, parg);
    }

    void ets_timer_disarm(ETSTimer* timer)
    {
        mock::timer::detach(timer);
    }

}  // extern "C"
