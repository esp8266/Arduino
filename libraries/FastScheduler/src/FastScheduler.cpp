#include "FastScheduler.h"
#include <PolledTimeout.h>
#include <Ticker.h>
#ifdef ESP8266
#include <interrupts.h>
#include <coredecls.h>
#else
#include <mutex>
#endif
#include "circular_queue/circular_queue_mp.h"

extern "C"
{
    void esp_loop()
    {
        loop();
        run_scheduled_functions(SCHEDULE_FUNCTION_FROM_LOOP);
    }

#ifdef ESP8266
    void __esp_yield();

    extern "C" void esp_yield()
    {
        __esp_yield();
        run_scheduled_functions(SCHEDULE_FUNCTION_WITHOUT_YIELDELAYCALLS);
    }
#endif
}

typedef std::function<bool(void)> mFuncT;

struct scheduled_fn_t
{
    mFuncT mFunc = nullptr;
    esp8266::polledTimeout::periodicFastUs callNow;
    schedule_e policy = SCHEDULE_FUNCTION_FROM_LOOP;
    scheduled_fn_t() : callNow(esp8266::polledTimeout::periodicFastUs::alwaysExpired) { }
};

// anonymous namespace provides compilation-unit internal linkage
namespace {
    static circular_queue_mp<scheduled_fn_t> schedule_queue(SCHEDULED_FN_MAX_COUNT);
    static esp8266::polledTimeout::periodicFastMs yieldSchedulerNow(100); // yield every 100ms
    static schedule_e activePolicy;

    class FastSchedulerTicker;
    // A local heap for Ticker instances to prevent global heap exhaustion
    class TickerHeap : public circular_queue_mp<FastSchedulerTicker*> {
    public:
        TickerHeap(const size_t capacity) : circular_queue_mp<FastSchedulerTicker*>(capacity)
        {
            heap = new char[capacity * sizeof(Ticker)];
            for (size_t i = 0; i < capacity; ++i) push(reinterpret_cast<FastSchedulerTicker*>(heap + i * sizeof(Ticker)));
        }
        ~TickerHeap()
        {
            delete heap;
        }
    protected:
        char* heap;
    };
    static TickerHeap tickerHeap(SCHEDULED_FN_MAX_COUNT);

    class FastSchedulerTicker : public Ticker
    {
    public:
        static void operator delete(void* ptr) {
            tickerHeap.push(static_cast<FastSchedulerTicker*>(ptr));
        }
    };
    static_assert(sizeof(FastSchedulerTicker) == sizeof(Ticker), "sizeof(FastSchedulerTicker) != sizeof(Ticker)");

    void ticker_scheduled(FastSchedulerTicker* ticker, const std::function<bool(void)>& fn, uint32_t repeat_us, schedule_e policy)
    {
#ifdef ESP8266
        auto repeat_ms = (repeat_us + 500) / 1000;
        ticker->once_ms(repeat_ms, [ticker, fn, repeat_us, policy]()
#else
        ticker->once_us(repeat_us, [ticker, fn, repeat_us, policy]()
#endif
            {
                if (!schedule_function([ticker, fn, repeat_us, policy]()
                    {
                        if (fn()) ticker_scheduled(ticker, fn, repeat_us, policy);
                        else delete ticker;
                        return false;
                    }, policy))
                {
                    ticker_scheduled(ticker, fn, repeat_us, policy);
                }
            });
    }

#ifdef ESP8266
    constexpr uint32_t TICKER_MIN_US = 5000;
#else
    constexpr uint32_t TICKER_MIN_US = 5000;
#endif
};

bool IRAM_ATTR schedule_recurrent_function_us(std::function<bool(void)>&& fn, uint32_t repeat_us, schedule_e policy)
{
    if (repeat_us >= TICKER_MIN_US)
    {
        // failure to aquire a Ticker must be returned to caller now. Specifically, allocating
        // Tickers from inside the scheduled function doesn't work, any exhaustion of the scheduler
        // can dead-lock it forever.
        auto tickerPlace = tickerHeap.pop();
        if (!tickerPlace) return false;
        auto ticker = new(tickerPlace) FastSchedulerTicker;
        if (!schedule_function([ticker, fn, repeat_us, policy]()
            {
                ticker_scheduled(ticker, std::move(fn), repeat_us, policy);
                return false;
            }, SCHEDULE_FUNCTION_WITHOUT_YIELDELAYCALLS))
        {
            delete ticker;
            return false;
        }
            return true;
    }
    else
    {
        scheduled_fn_t item;
        item.mFunc = std::move(fn);
        if (repeat_us) item.callNow.reset(repeat_us);
        item.policy = policy;
        return schedule_queue.push(std::move(item));
    }
}

bool IRAM_ATTR schedule_recurrent_function_us(const std::function<bool(void)>& fn, uint32_t repeat_us, schedule_e policy)
{
    return schedule_recurrent_function_us(std::function<bool(void)>(fn), repeat_us, policy);
}

bool IRAM_ATTR schedule_function(std::function<void(void)>&& fn, schedule_e policy)
{
    return schedule_recurrent_function_us([fn]() { fn(); return false; }, 0, policy);
}

bool IRAM_ATTR schedule_function(const std::function<void(void)>& fn, schedule_e policy)
{
    return schedule_function(std::function<void(void)>(fn), policy);
}

bool run_function(scheduled_fn_t& func)
{
    if (yieldSchedulerNow) {
#if defined(ESP8266)
        esp_schedule();
        cont_yield(g_pcont);
#elif defined(ESP32)
        vPortYield();
#else
        yield();
#endif
    }
    return
        (func.policy != SCHEDULE_FUNCTION_WITHOUT_YIELDELAYCALLS && activePolicy != SCHEDULE_FUNCTION_FROM_LOOP)
        || !func.callNow
        || func.mFunc();
}

void run_scheduled_functions(schedule_e policy)
{
    // Note to the reader:
    // There is no exposed API to remove a scheduled function:
    // Scheduled functions are removed only from this function, and
    // its purpose is that it is never called from an interrupt
    // (always on cont stack).

    static std::atomic<bool> fence(false);
#ifdef ESP8266
    {
        esp8266::InterruptLock lockAllInterruptsInThisScope;
        if (fence.load()) {
            // prevent any recursive calls from yield()
            return;
        }
        fence.store(true);
    }
#else
    if (fence.exchange(true)) return;
#endif

    yieldSchedulerNow.reset(100);
    activePolicy = policy;

    // run scheduled function:
    // - when its schedule policy allows it anytime
    // - or if we are called at loop() time
    // and
    // - its time policy allows it
    schedule_queue.for_each_rev_requeue(run_function);
    fence.store(false);
}
