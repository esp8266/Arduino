/*
 * Copyright (c) 2012 Arduino LLC. All right reserved.
 * Audio library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef AUDIO_H
#define AUDIO_H

#include "Arduino.h"
#include "Print.h"
#include "DAC.h"

class AudioClass : public Print {
public:
	AudioClass(DACClass &_dac) : dac(&_dac) { };
	void prepare(int16_t *buffer, int S, int volume);
	void begin(uint32_t sampleRate, uint32_t msPreBuffer);
	void end();

	virtual size_t write(uint8_t c)                         { /* not implemented */ };
	virtual size_t write(const uint8_t *data, size_t size)  { return write(reinterpret_cast<const uint32_t*>(data), size/4) * 4; };
	virtual size_t write(const uint16_t *data, size_t size) { return write(reinterpret_cast<const uint32_t*>(data), size/2) * 2; };
	virtual size_t write(const int16_t *data, size_t size)  { return write(reinterpret_cast<const uint32_t*>(data), size/2) * 2; };
	virtual size_t write(const uint32_t *data, size_t size);

	void debug() {
//		Serial1.print(running-buffer, DEC);
//		Serial1.print(" ");
//		Serial1.print(current-buffer, DEC);
//		Serial1.print(" ");
//		Serial1.print(next-buffer, DEC);
//		Serial1.print(" ");
//		Serial1.println(last-buffer, DEC);
	}
private:
	void enqueue();
	static void onTransmitEnd(void *me);
	uint32_t bufferSize;
	uint32_t *buffer;
	uint32_t *half;
	uint32_t *last;
	uint32_t *volatile running;
	uint32_t *volatile next;

	uint32_t *cook(const uint32_t *buffer, size_t size);

	DACClass *dac;
};

extern AudioClass Audio;

#endif
