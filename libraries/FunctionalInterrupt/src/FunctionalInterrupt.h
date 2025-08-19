#ifndef FUNCTIONALINTERRUPT_H
#define FUNCTIONALINTERRUPT_H

#include <functional>

// Structures for communication

struct InterruptInfo
{
    InterruptInfo(uint8_t _pin) : pin(_pin) {}
    const uint8_t pin;
    uint8_t value = 0;
    uint32_t micro = 0;
};

void attachScheduledInterrupt(uint8_t pin, const std::function<void(const InterruptInfo&)>& scheduledIntRoutine, int mode);

#endif //FUNCTIONALINTERRUPT_H
