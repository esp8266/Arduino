#include "Schedule.h"
#include "PolledTimeout.h"

typedef std::function<bool(void)> mFuncT;

struct scheduled_fn_t
{
    scheduled_fn_t* mNext;
    mFuncT mFunc;
    esp8266::polledTimeout::periodicFastUs callNow;

    scheduled_fn_t(): callNow(esp8266::polledTimeout::periodicFastUs::alwaysExpired) { }
};

static scheduled_fn_t* sFirst = 0;
static scheduled_fn_t* sFirstUnused = 0;

static int sCount = 0;

static scheduled_fn_t* get_fn() {
    scheduled_fn_t* result = NULL;
    // try to get an item from unused items list
    if (sFirstUnused) {
        result = sFirstUnused;
        sFirstUnused = result->mNext;
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
    fn->mNext = sFirstUnused;
    sFirstUnused = fn;
}

bool schedule_function_us(mFuncT fn, uint32_t repeat_us)
{
    scheduled_fn_t* item = get_fn();
    if (!item) {
        return false;
    }
    item->mFunc = fn;
    item->mNext = sFirst;
    sFirst = item;
    if (repeat_us)
        item->callNow.reset(repeat_us);
    return true;
}

bool schedule_function(std::function<void(void)> fn)
{
    return schedule_function_us([&fn](){ fn(); return false; }, 0);
}

void run_scheduled_functions()
{
    scheduled_fn_t* toCall = sFirst;
    while (toCall) {
        scheduled_fn_t* item = toCall;
        toCall = item->mNext;
        if (item->callNow && !item->mFunc()) {
            if (sFirst == item)
                sFirst = item->mNext;
            item->mFunc = mFuncT();
            recycle_fn(item);
        }
    }
}
