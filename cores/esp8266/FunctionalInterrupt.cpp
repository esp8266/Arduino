#include <FunctionalInterrupt.h>


// Duplicate typedefs from core_esp8266_wiring_digital_c
typedef void (*voidFuncPtr)(void);

// Helper functions for Functional interrupt routines
extern "C" void ICACHE_RAM_ATTR __attachInterruptArg(uint8_t pin, voidFuncPtr userFunc, void*fp , int mode);

// Structure for communication
struct ArgStructure {
	std::function<void(void)> reqFunction;
};

void interruptFunctional(void* arg)
{
	((ArgStructure*)arg)->reqFunction();
}

void attachInterrupt(uint8_t pin, std::function<void(void)> intRoutine, int mode)
{
	// use the local interrupt routine which takes the ArgStructure as argument
	__attachInterruptArg (pin, (voidFuncPtr)interruptFunctional, new ArgStructure{intRoutine}, mode);
}
