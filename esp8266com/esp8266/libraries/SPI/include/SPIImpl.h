#ifndef SPIIMPL_H
#define SPIIMPL_H

#include <stdint.h>

class SPIImpl
{
public:
	virtual void begin() = 0;
	virtual uint8_t transfer(uint8_t data) = 0;
	virtual void transfer(void *buf, size_t count) = 0;
	virtual void end() = 0;

	virtual void setBitOrder(uint8_t bitOrder) = 0;
	virtual void setDataMode(uint8_t dataMode) = 0;
	virtual void setClockDivider(uint8_t clockDiv) = 0;

};


#endif//SPIIMPL_H
