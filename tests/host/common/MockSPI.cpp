
#include <SPI.h>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPI)
SPIClass SPI;
#endif

SPIClass::SPIClass ()
{
}

uint8_t SPIClass::transfer(uint8_t data)
{
	return data;
}

void SPIClass::begin()
{
}

void SPIClass::end()
{
}

void SPIClass::setFrequency(uint32_t freq)
{
	(void)freq;
}

void SPIClass::setHwCs(bool use)
{
	(void)use;
}
