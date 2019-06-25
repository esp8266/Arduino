
#include <assert.h>

#include "Schedule.h"
#include "PolledTimeout.h"
#include "interrupts.h"
#include "coredecls.h"

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
    recurrent_fn_t (esp8266::polledTimeout::periodicFastUs interval): callNow(interval) { }
};

static recurrent_fn_t* rFirst = nullptr; // fifo not needed

// Returns a pointer to an unused sched_fn_t,
// or if none are available allocates a new one,
// or nullptr if limit is reached
IRAM_ATTR // called from ISR
static scheduled_fn_t* get_fn_unsafe ()
{
    scheduled_fn_t* result = nullptr;
    // try to get an item from unused items list
    if (sUnused)
    {
        result = sUnused;
        sUnused = sUnused->mNext;
        result->mNext = nullptr;
    }
    // if no unused items, and count not too high, allocate a new one
    else if (sCount < SCHEDULED_FN_MAX_COUNT)
    {
        result = (scheduled_fn_t*)malloc(sizeof(scheduled_fn_t));
        if (result)
            ++sCount;
    }
    return result;
}

static void recycle_fn_unsafe (scheduled_fn_t* fn)
{
    fn->mFunc = nullptr; // special overload in c++ std lib
    fn->mNext = sUnused;
    sUnused = fn;
}

IRAM_ATTR // (not only) called from ISR
bool schedule_function (const std::function<void(void)>& fn)
{
    esp8266::InterruptLock lockAllInterruptsInThisScope;

    scheduled_fn_t* item = get_fn_unsafe();
    if (!item)
        return false;

    item->mFunc = fn;

    if (sFirst)
        sLast->mNext = item;
    else
        sFirst = item;
    sLast = item;

    return true;
}

bool schedule_recurrent_function_us (const std::function<bool(void)>& fn, uint32_t repeat_us)
{
    assert(repeat_us < decltype(recurrent_fn_t::callNow)::neverExpires); //~26800000us (26.8s)

    esp8266::InterruptLock lockAllInterruptsInThisScope;

    recurrent_fn_t* item = new recurrent_fn_t(repeat_us);
    if (!item)
        return false;

    item->mFunc = fn;

    if (rFirst)
    {
        item->mNext = rFirst;
        rFirst = item;
    }
    else
        rFirst = item;

    return true;
}

void run_scheduled_functions ()
{
    esp8266::polledTimeout::periodicFastMs yieldNow(100); // yield every 100ms

    while (sFirst)
    {
        sFirst->mFunc();

        {
            esp8266::InterruptLock lockAllInterruptsInThisScope;

            auto to_recycle = sFirst;
            sFirst = sFirst->mNext;
            if (!sFirst)
                sLast = nullptr;
            recycle_fn_unsafe(to_recycle);
        }

        if (yieldNow)
        {
            // because scheduled function are allowed to last:
            // this is yield() in cont stack:
            esp_schedule();
            cont_yield(g_pcont);
        }
    }
}

void run_scheduled_recurrent_functions ()
{
    // Note to the reader:
    // There is no exposed API to remove a scheduled function:
    // Scheduled functions are removed only from this function, and
    // its purpose is that it is never called from an interrupt
    // (always on cont stack).

    if (!rFirst)
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

    recurrent_fn_t* prev = nullptr;
    recurrent_fn_t* current = rFirst;

    while (current)
    {
        if (current->callNow && !current->mFunc())
        {
            // remove function from stack
            esp8266::InterruptLock lockAllInterruptsInThisScope;

            auto to_ditch = current;

            if (prev)
            {
                current = current->mNext;
                prev->mNext = current;
            }
            else
            {
                rFirst = rFirst->mNext;
                current = rFirst;
            }

            delete(to_ditch);
        }
        else
        {
            prev = current;
            current = current->mNext;
        }
    }

    fence = false;
}
