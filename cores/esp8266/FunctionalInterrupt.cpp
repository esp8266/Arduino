#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include <Schedule.h>

// Duplicate typedefs from core_esp8266_wiring_digital_c
typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrArg)(void*);

// Helper functions for Functional interrupt routines
extern "C" void __attachInterruptFunctionalArg(uint8_t pin, voidFuncPtr userFunc, void*fp, int mode, bool functional);


void ICACHE_RAM_ATTR interruptFunctional(void* arg)
{
    ArgStructure* localArg = (ArgStructure*)arg;
	if (localArg->functionInfo->reqScheduledFunction)
	{
		schedule_function(std::bind(localArg->functionInfo->reqScheduledFunction,InterruptInfo(*(localArg->interruptInfo))));
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

	__attachInterruptFunctionalArg(pin, (voidFuncPtr)interruptFunctional, as, mode, true);
}

void attachScheduledInterrupt(uint8_t pin, std::function<void(InterruptInfo)> scheduledIntRoutine, int mode)
{
	InterruptInfo* ii = new InterruptInfo;

	FunctionInfo* fi = new FunctionInfo;
	fi->reqScheduledFunction = scheduledIntRoutine;

	ArgStructure* as = new ArgStructure;
	as->interruptInfo = ii;
	as->functionInfo = fi;

	__attachInterruptFunctionalArg(pin, (voidFuncPtr)interruptFunctional, as, mode, true);
}
