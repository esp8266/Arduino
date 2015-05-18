/*
 * Copyright (c) 2012 Arduino LLC. All right reserved.
 * DAC library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef DAC_INCLUDED
#define DAC_INCLUDED

#include "Arduino.h"

typedef void (*OnTransmitEnd_CB)(void *data);

class DACClass
{
public:
	DACClass(Dacc *_dac, uint32_t _dacId, IRQn_Type _isrId) :
		dac(_dac), dacId(_dacId), isrId(_isrId), cb(NULL) { };
	void begin(uint32_t period);
	void end();
	bool canQueue();
	size_t queueBuffer(const uint32_t *buffer, size_t size);
	uint32_t *getCurrentQueuePointer();
	void setOnTransmitEnd_CB(OnTransmitEnd_CB _cb, void *data);
	void onService();

	void enableInterrupts()  { NVIC_EnableIRQ(isrId); };
	void disableInterrupts() { NVIC_DisableIRQ(isrId); };

private:
	Dacc *dac;
	uint32_t dacId;
	IRQn_Type isrId;
	OnTransmitEnd_CB cb;
	void *cbData;
};

extern DACClass DAC;

#endif
