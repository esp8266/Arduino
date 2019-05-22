
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

static scheduled_fn_t* sFirst = 0;
static scheduled_fn_t* sLast = 0;

static scheduled_fn_t* sFirstUnused = 0;
static scheduled_fn_t* sLastUnused = 0;

static int sCount = 0;

IRAM_ATTR // called from ISR
static scheduled_fn_t* get_fn_unsafe()
{
    scheduled_fn_t* result = nullptr;
    // try to get an item from unused items list
    if (sFirstUnused)
    {
        result = sFirstUnused;
        sFirstUnused = sFirstUnused->mNext;
        if (sFirstUnused == nullptr)
        {
            sLastUnused = nullptr;
        }
    }
    // if no unused items, and count not too high, allocate a new one
    else if (sCount != SCHEDULED_FN_MAX_COUNT)
    {
        result = new scheduled_fn_t;
        result->mNext = nullptr;
        ++sCount;
    }
    return result;
}

static void recycle_fn(scheduled_fn_t* fn)
{
    InterruptLock lockAllInterruptsInThisScope;

    if (!sLastUnused)
    {
        sFirstUnused = fn;
    }
    else
    {
        sLastUnused->mNext = fn;
    }
    fn->mNext = nullptr;
    sLastUnused = fn;
}

IRAM_ATTR // called from ISR
bool schedule_function_us(mFuncT fn, uint32_t repeat_us)
{
    assert(repeat_us < decltype(scheduled_fn_t::callNow)::neverExpires); //26800000us (26.8s)

    InterruptLock lockAllInterruptsInThisScope;

    scheduled_fn_t* item = get_fn_unsafe();
    if (!item)
        return false;

    if (repeat_us)
        item->callNow.reset(repeat_us);

    item->mFunc = fn;
    item->mNext = nullptr;
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
    // Scheduled functions are removed in this function, and
    // its purpose is that it is never called from an interrupt /
    // always called from cont stack.

    scheduled_fn_t* lastRecurring = nullptr;
    scheduled_fn_t* toCall = sFirst;
    while (toCall)
    {
        scheduled_fn_t* item = toCall;
        toCall = item->mNext;
        if (item->callNow)
        {
            if (item->mFunc())
            {
                lastRecurring = item;
            }
            else
            {
                {
                    InterruptLock lockAllInterruptsInThisScope;
                    if (sFirst == item)
                        sFirst = item->mNext;
                    if (sLast == item)
                        sLast = lastRecurring;
                }

                item->mFunc = mFuncT();
                recycle_fn(item);
            }
        }
    }
}
