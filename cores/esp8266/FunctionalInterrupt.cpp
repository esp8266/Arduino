// core_esp8266_wiring_digital helper for std::function<...> dispatch

#include "Arduino.h"
#include "Schedule.h"

#include "FunctionalInterrupt.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
}

namespace esp8266::FunctionalInterrupt {
namespace {

struct ArgStructure {
	InterruptInfo* interruptInfo;
	FunctionInfo* functionInfo;
};

}

void IRAM_ATTR updateArg(void* varg, uint8_t i, uint8_t value, uint32_t micro) {
  ArgStructure* arg = reinterpret_cast<ArgStructure*>(varg);
  if (arg && arg->interruptInfo)
  {
    arg->interruptInfo->pin = i;
    arg->interruptInfo->value = value;
    arg->interruptInfo->micro = micro;
  }
}

void IRAM_ATTR dispatchFunction(void* varg)
{
  ArgStructure* arg = reinterpret_cast<ArgStructure*>(varg);
  arg->functionInfo->reqFunction(Tag{});
}

void IRAM_ATTR dispatchScheduled(void* varg)
{
  ArgStructure* arg = reinterpret_cast<ArgStructure*>(varg);
  schedule_function(std::bind(arg->functionInfo->reqScheduledFunction, InterruptInfo(*(arg->interruptInfo))));
}

// see core_esp8266_wiring_digital.cpp, these are referenced there
void cleanup(void* varg)
{
  ArgStructure* arg = reinterpret_cast<ArgStructure*>(varg);
  delete (FunctionInfo*)arg->functionInfo;
  delete (InterruptInfo*)arg->interruptInfo;
  delete arg;
}

// these would be used by the the local interrupt routine which takes the ArgStructure as argument

void prepareFunction(void** funcPtr, void** argPtr, Function intRoutine)
{
	InterruptInfo* ii = nullptr;

	FunctionInfo* fi = new FunctionInfo;
	fi->reqFunction = std::move(intRoutine);

	ArgStructure* as = new ArgStructure;
	as->interruptInfo = ii;
	as->functionInfo = fi;

    (*funcPtr) = (void*)&dispatchFunction;
    (*argPtr) = as;
}

void prepareScheduledFunction(void** funcPtr, void** argPtr, ScheduledFunction intRoutine)
{
	InterruptInfo* ii = new InterruptInfo;

	FunctionInfo* fi = new FunctionInfo;
	fi->reqScheduledFunction = std::move(intRoutine);

	ArgStructure* as = new ArgStructure;
	as->interruptInfo = ii;
	as->functionInfo = fi;

    (*funcPtr) = (void*)&dispatchScheduled;
    (*argPtr) = as;
}

}  // namespace esp8266::FunctionalInterrupt

extern "C" void __attachInterruptFunctionalArg(uint8_t, void (*)(void*), void*, int, bool functional);

void attachInterrupt(uint8_t pin, esp8266::FunctionalInterrupt::Function intRoutine, int mode)
{
  void* func;
  void* arg;
  esp8266::FunctionalInterrupt::prepareFunction(&func, &arg, std::move(intRoutine));
  __attachInterruptFunctionalArg(pin, reinterpret_cast<void(*)(void*)>(func), arg, mode, true);
}

void attachScheduledInterrupt(uint8_t pin, esp8266::FunctionalInterrupt::ScheduledFunction intRoutine, int mode)
{
  void* func;
  void* arg;
  esp8266::FunctionalInterrupt::prepareScheduledFunction(&func, &arg, std::move(intRoutine));
  __attachInterruptFunctionalArg(pin, reinterpret_cast<void(*)(void*)>(func), arg, mode, true);
}
