
#include <assert.h>

#include "Schedule.h"
#include "PolledTimeout.h"
#include "interrupts.h"

typedef std::function<bool(void)> mFuncT;

struct scheduled_fn_t
{
    scheduled_fn_t* mNext;
    mFuncT mFunc;
    esp8266::polledTimeout::periodicFastUs callNow;

    scheduled_fn_t(): callNow(esp8266::polledTimeout::periodicFastUs::alwaysExpired) { }
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
    }
    // if no unused items, and count not too high, allocate a new one
    else if (sCount < SCHEDULED_FN_MAX_COUNT)
    {
        result = new scheduled_fn_t;
        ++sCount;
    }
    result->mNext = nullptr;
    return result;
}

static void recycle_fn_unsafe(scheduled_fn_t* fn)
{
    fn->mFunc = mFuncT();
    fn->mNext = sUnused;
    sUnused = fn;
}

IRAM_ATTR // called from ISR
bool schedule_function_us(mFuncT fn, uint32_t repeat_us)
{
    assert(repeat_us < decltype(scheduled_fn_t::callNow)::neverExpires); //~26800000us (26.8s)

    InterruptLock lockAllInterruptsInThisScope;

    scheduled_fn_t* item = get_fn_unsafe();
    if (!item)
        return false;

    if (repeat_us)
        item->callNow.reset(repeat_us);

    item->mFunc = fn;
    if (sFirst)
        sLast->mNext = item;
    else
        sFirst = item;
    sLast = item;

    return true;
}

IRAM_ATTR // called from ISR
bool schedule_function(std::function<void(void)> fn)
{
    return schedule_function_us([&fn](){ fn(); return false; }, 0);
}

void run_scheduled_functions()
{
    // Note to the reader:
    // There is no exposed API to remove a scheduled function:
    // Scheduled functions are removed only from this function, and
    // its purpose is that it is never called from an interrupt
    // (always on cont stack).

    scheduled_fn_t* lastRecurring = nullptr;
    scheduled_fn_t* toCall = sFirst;
    while (toCall)
    {
        scheduled_fn_t* item = toCall;
        toCall = toCall->mNext;
        if (item->callNow)
        {
            if (item->mFunc())
            {
                lastRecurring = item;
            }
            else
            {
                InterruptLock lockAllInterruptsInThisScope;
                if (sFirst == item)
                    sFirst = sFirst->mNext;
                if (sLast == item)
                    sLast = lastRecurring;

                recycle_fn_unsafe(item);
            }
        }
    }
}
