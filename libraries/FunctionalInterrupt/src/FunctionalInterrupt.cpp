#include "FunctionalInterrupt.h"
#include <Schedule.h>
#include <Arduino.h>

namespace
{

    struct InterruptScheduleFunctionalArg
    {
        uint8_t pin;
        std::function<void(const InterruptInfo&)> scheduledIntRoutine;
    };
    struct ScheduleLambdaArg
    {
        std::function<void(const InterruptInfo&)> scheduledIntRoutine;
        InterruptInfo interruptInfo;
    };

    void IRAM_ATTR interruptScheduleFunctional(const InterruptScheduleFunctionalArg& arg)
    {
        ScheduleLambdaArg lambdaArg{ arg.scheduledIntRoutine, { arg.pin } };
        lambdaArg.interruptInfo.value = digitalRead(arg.pin);
        lambdaArg.interruptInfo.micro = micros();
        schedule_function(std::bind(lambdaArg.scheduledIntRoutine, lambdaArg.interruptInfo));
    }

}

void attachScheduledInterrupt(uint8_t pin, const std::function<void(const InterruptInfo&)>& scheduledIntRoutine, int mode)
{
    if (scheduledIntRoutine)
    {
        InterruptScheduleFunctionalArg arg{ pin, scheduledIntRoutine };
        attachInterrupt(pin, std::bind(interruptScheduleFunctional, arg), mode);
    }
}
