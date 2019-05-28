
#include <assert.h>

#include "Schedule.h"
#include "PolledTimeout.h"
#include "interrupts.h"

typedef std::function<bool(void)> mFuncT;

struct scheduled_fn_t
{
    scheduled_fn_t* mNext = nullptr;
    mFuncT mFunc;
    esp8266::polledTimeout::periodicFastUs callNow;
    schedule_e policy;

    scheduled_fn_t() : callNow(esp8266::polledTimeout::periodicFastUs::alwaysExpired) { }
};

static scheduled_fn_t* sFirst = nullptr;
static scheduled_fn_t* sLast = nullptr;
static scheduled_fn_t* sUnused = nullptr;
static int sCount = 0;

IRAM_ATTR // called from ISR
static scheduled_fn_t* get_fn_unsafe()
{
    scheduled_fn_t* result = nullptr;
    // try to get an item from unused items list
    if (sUnused)
    {
        result = sUnused;
        sUnused = sUnused->mNext;
        result->mNext = nullptr;
        result->callNow.reset(esp8266::polledTimeout::periodicFastUs::alwaysExpired);
    }
    // if no unused items, and count not too high, allocate a new one
    else if (sCount < SCHEDULED_FN_MAX_COUNT)
    {
        result = new scheduled_fn_t;
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
bool schedule_function_us(std::function<bool(void)>&& fn, uint32_t repeat_us, schedule_e policy)
{
    assert(repeat_us < decltype(scheduled_fn_t::callNow)::neverExpires); //~26800000us (26.8s)

    InterruptLock lockAllInterruptsInThisScope;

    scheduled_fn_t* item = get_fn_unsafe();
    if (!item)
        return false;

    if (repeat_us)
        item->callNow.reset(repeat_us);
    item->policy = policy;
    item->mFunc = fn;

    if (sFirst)
        sLast->mNext = item;
    else
        sFirst = item;
    sLast = item;

    return true;
}

//IRAM_ATTR // (not only) called from ISR
//bool schedule_function_us(const std::function<bool(void)>& fn, uint32_t repeat_us, schedule_e policy)
//{
//    return schedule_function_us(std::function<bool(void)>(fn), repeat_us, policy);
//}

IRAM_ATTR // called from ISR
bool schedule_function(std::function<void(void)>&& fn, schedule_e policy)
{
    return schedule_function_us([fn]() { fn(); return false; }, 0, policy);
}

//IRAM_ATTR // called from ISR
//bool schedule_function(const std::function<void(void)>& fn, schedule_e policy)
//{
//    return schedule_function(std::function<void(void)>(fn), policy);
//}

void run_scheduled_functions(schedule_e policy)
{
    // Note to the reader:
    // There is no exposed API to remove a scheduled function:
    // Scheduled functions are removed only from this function, and
    // its purpose is that it is never called from an interrupt
    // (always on cont stack).

    static bool fence = false;
    {
        InterruptLock lockAllInterruptsInThisScope;
        if (fence)
            // prevent recursive calls from yield()
            return;
        fence = true;
    }

    scheduled_fn_t* lastRecurring = nullptr;
    scheduled_fn_t* nextCall = sFirst;
    while (nextCall)
    {
        scheduled_fn_t* toCall = nextCall;
        nextCall = nextCall->mNext;

        // run scheduled function:
        // - when its schedule policy allows it anytime
        // - or if we are called at loop() time
        // and
        // - its time policy allows it
        if (   (   toCall->policy == SCHEDULED_FUNCTION_WITHOUT_YIELDELAYCALLS
                || policy == SCHEDULED_FUNCTION_ONCE_PER_LOOP)
            && toCall->callNow)
        {
            if (toCall->mFunc())
            {
                // function stays in list
                lastRecurring = toCall;
            }
            else
            {
                // function removed from list
                InterruptLock lockAllInterruptsInThisScope;

                if (sFirst == toCall)
                    sFirst = sFirst->mNext;
                else if (lastRecurring)
                    lastRecurring->mNext = toCall->mNext;

                if (sLast == toCall)
                    sLast = lastRecurring;

                recycle_fn_unsafe(toCall);
            }
        }
        else
            // function stays in list
            lastRecurring = toCall;
    }

    fence = false;
}
