// core_esp8266_wiring_digital helper for std::function<...> dispatch

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace esp8266::FunctionalInterrupt {

using PlainFunction = std::function<void()>;

struct Tag {
};

using Function = std::function<void(Tag)>;

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

[[deprecated("3.2.x: use function type w/ ::Tag as argument")]]
inline void attachInterrupt(uint8_t pin, esp8266::FunctionalInterrupt::PlainFunction func, int mode) {
  attachInterrupt(pin, [func = std::move(func)](auto) { func(); }, mode);
}
