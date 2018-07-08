#include <FunctionalInterrupt.h>
#include <Schedule.h>
#include "Arduino.h"
#include <ScheduledFunctions.h>

// Duplicate typedefs from core_esp8266_wiring_digital_c
typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrArg)(void*);

// Helper functions for Functional interrupt routines
extern "C" void ICACHE_RAM_ATTR __attachInterruptArg(uint8_t pin, voidFuncPtr userFunc, void*fp , int mode);


void interruptFunctional(void* arg)
{
    ArgStructure* localArg = (ArgStructure*)arg;
	if (localArg->functionInfo->reqScheduledFunction)
	{
		schedule_function(std::bind(localArg->functionInfo->reqScheduledFunction,InterruptInfo(*(localArg->interruptInfo))));
//      scheduledInterrupts->scheduleFunctionReg(std::bind(localArg->functionInfo->reqScheduledFunction,InterruptInfo(*(localArg->interruptInfo))), false, true);
	}
	if (localArg->functionInfo->reqFunction)
	{
	  localArg->functionInfo->reqFunction();
	}
}

extern "C"
{
   void cleanupFunctional(void* arg)
   {
	 ArgStructure* localArg = (ArgStructure*)arg;
	 delete (FunctionInfo*)localArg->functionInfo;
     delete (InterruptInfo*)localArg->interruptInfo;
	 delete localArg;
   }
}

void attachInterrupt(uint8_t pin, std::function<void(void)> intRoutine, int mode)
{
	// use the local interrupt routine which takes the ArgStructure as argument

	InterruptInfo* ii = nullptr;

	FunctionInfo* fi = new FunctionInfo;
	fi->reqFunction = intRoutine;

	ArgStructure* as = new ArgStructure;
	as->interruptInfo = ii;
	as->functionInfo = fi;

	__attachInterruptArg (pin, (voidFuncPtr)interruptFunctional, as, mode);
}

void attachScheduledInterrupt(uint8_t pin, std::function<void(InterruptInfo)> scheduledIntRoutine, int mode)
{
	if (!scheduledInterrupts)
	{
		scheduledInterrupts = new ScheduledFunctions(32);
	}
	InterruptInfo* ii = new InterruptInfo;

	FunctionInfo* fi = new FunctionInfo;
	fi->reqScheduledFunction = scheduledIntRoutine;

	ArgStructure* as = new ArgStructure;
	as->interruptInfo = ii;
	as->functionInfo = fi;

	__attachInterruptArg (pin, (voidFuncPtr)interruptFunctional, as, mode);
}
