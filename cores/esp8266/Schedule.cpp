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
static scheduled_fn_t* get_fn() {
    InterruptLock lockAllInterruptsInThisScope;

    scheduled_fn_t* result = NULL;
    // try to get an item from unused items list
    if (sFirstUnused) {
        result = sFirstUnused;
        sFirstUnused = result->mNext;
        if (sFirstUnused == NULL) {
            sLastUnused = NULL;
        }
    }
    // if no unused items, and count not too high, allocate a new one
    else if (sCount != SCHEDULED_FN_MAX_COUNT) {
        result = new scheduled_fn_t;
        result->mNext = NULL;
        ++sCount;
    }
    return result;
}

static void recycle_fn(scheduled_fn_t* fn)
{
    InterruptLock lockAllInterruptsInThisScope;

    if (!sLastUnused) {
        sFirstUnused = fn;
    }
    else {
        sLastUnused->mNext = fn;
    }
    fn->mNext = NULL;
    sLastUnused = fn;
}

IRAM_ATTR // called from ISR
bool schedule_function_us(mFuncT fn, uint32_t repeat_us)
{
    InterruptLock lockAllInterruptsInThisScope;

    scheduled_fn_t* item = get_fn();
    if (!item) {
        return false;
    }
    item->mFunc = fn;
    item->mNext = NULL;
    if (!sFirst) {
        sFirst = item;
    }
    else {
        sLast->mNext = item;
    }
    sLast = item;

    if (repeat_us)
        item->callNow.reset(repeat_us);

    return true;
}

IRAM_ATTR // called from ISR
bool schedule_function(std::function<void(void)> fn)
{
    return schedule_function_us([&fn](){ fn(); return false; }, 0);
}

void run_scheduled_functions()
{
	scheduled_fn_t* lastRecurring = nullptr;
	scheduled_fn_t* toCall = sFirst;
    while (toCall) {
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
