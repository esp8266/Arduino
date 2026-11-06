// core_esp8266_wiring_digital helper for std::function<...> dispatch

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace esp8266::FunctionalInterrupt {

using Function = std::function<void()>;

struct InterruptInfo {
	uint8_t pin;
	uint8_t value;
	uint32_t micro;
};

using ScheduledFunction = std::function<void(InterruptInfo)>;

struct FunctionInfo {
    Function reqFunction;
	ScheduledFunction reqScheduledFunction;
};

}  // namespace esp8266::FunctionalInterrupt

void attachInterrupt(uint8_t pin, esp8266::FunctionalInterrupt::Function, int mode);
void attachScheduledInterrupt(uint8_t pin, esp8266::FunctionalInterrupt::ScheduledFunction, int mode);
