#ifndef FUNCTIONALINTERRUPT_H
#define FUNCTIONALINTERRUPT_H

#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <ScheduledFunctions.h>

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
}

// Structures for communication

struct InterruptInfo {
	uint8_t pin = 0;
	uint8_t value = 0;
	uint32_t micro = 0;
};

struct FunctionInfo {
    std::function<void(void)> reqFunction = nullptr;
	std::function<void(InterruptInfo)> reqScheduledFunction = nullptr;
};

struct ArgStructure {
	InterruptInfo* interruptInfo = nullptr;
	FunctionInfo* functionInfo = nullptr;
};

static ScheduledFunctions* scheduledInterrupts;
void attachInterrupt(uint8_t pin, std::function<void(void)> intRoutine, int mode);
void attachScheduledInterrupt(uint8_t pin, std::function<void(InterruptInfo)> scheduledIntRoutine, int mode);


#endif //INTERRUPTS_H
