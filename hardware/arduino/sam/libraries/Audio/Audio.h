/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@bug.st>
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

class AudioClass : public Print {
public:
	AudioClass(Dacc *_dac, uint32_t _dacId) : dac(_dac), dacId(_dacId) { };
	void begin(uint32_t sampleRate);
	void end();

	virtual size_t write(uint8_t c)                           { /* not implemented */ };
	virtual size_t write(const uint8_t *buffer, size_t size)  { return write((uint32_t*) buffer, size/4) * 4; };
	virtual size_t write(const uint16_t *buffer, size_t size) { return write((uint32_t*) buffer, size/2) * 2; };
	virtual size_t write(const int16_t *buffer, size_t size)  { return write((uint32_t*) buffer, size/2) * 2; };
	virtual size_t write(const uint32_t *buffer, size_t size);

private:
	uint32_t buffer0[1024];
	uint32_t buffer1[1024];
	int currentBuffer;

	uint32_t *cook(const uint32_t *buffer, size_t size);

	Dacc *dac;
	uint32_t dacId;
};

extern AudioClass Audio;

#endif
