/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@bug.st>
 * Audio library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "Audio.h"

class LockDAC {
public:
	LockDAC(DACClass *_dac) : dac(_dac)
	           { dac->disableInterrupts(); };
	~LockDAC() { dac->enableInterrupts(); };
	DACClass *dac;
};

void AudioClass::begin(uint32_t sampleRate, uint32_t msPreBuffer) {
	// Allocate a buffer to keep msPreBuffer milliseconds of audio
	bufferSize = msPreBuffer * sampleRate / 1000;
	if (bufferSize < 2048)
		bufferSize = 2048;
	buffer = (uint32_t *) malloc(bufferSize * sizeof(uint32_t));
	last = buffer + bufferSize;

	// Buffering starts from the beginning
	running = last;
	current = buffer;
	next = buffer;

	// Run DAC
	dac->begin(VARIANT_MCK / sampleRate);
	dac->setOnTransmitEnd_CB(onTransmitEnd, this);
}

void AudioClass::end() {
	dac->end();
	free(buffer);
}

size_t AudioClass::write(const uint32_t *data, size_t size) {
	LockDAC lock(dac);
	//Serial1.print("WRI(");
	const uint32_t TAG = 0x10000000;
	int i;
	for (i=0; i < size && next != running; i++) {
		*next = data[i] | TAG;
		next++;

		// Wrap around circular buffer
		if (next == last)
			next = buffer;
	}
	debug();
	if (dac->canQueue()) {
		enqueue();
		debug();
	}
	//Serial1.print(")");
	return i;
}

void AudioClass::enqueue() {
	if (!dac->canQueue())
		// DMA queue full
		return;

	if (current == next)
		// No data to enqueue
		return;

	// If wrapping happened
	if (next < current) {

		uint32_t size = last - current;

		if (size < 1024) {
			// enqueue the last part of the circular buffer
			dac->queueBuffer(current, size);
			current = buffer;
			next = buffer;
		} else {
			// Enqueue only a block of 512
			dac->queueBuffer(current, 512);
			current += 512;
		}
		return;
	}

	bool aboutToWrap = (last - next) < 512;
	uint32_t size = next - current;

	// If buffered data is less than 512 bytes
	if (size < 512) {

		// Enqueue all
		dac->queueBuffer(current, size);

		if (aboutToWrap)
			next = buffer;
		current = next;

	} else {

		if (aboutToWrap && size < 1024) {
			// Enqueue all
			dac->queueBuffer(current, size);
			next = buffer;
			current = buffer;
		} else {
			// Enqueue only a block of 512
			dac->queueBuffer(current, 512);
			current += 512;
		}

	}
}

void AudioClass::onTransmitEnd(void *me) {
	AudioClass *audio = reinterpret_cast<AudioClass *>(me);

	//Serial1.print("INT(");
	audio->enqueue();

	// Update running pointer
	audio->running = audio->dac->getCurrentQueuePointer();

	audio->debug();
	//Serial1.print(")");
}

AudioClass Audio(DAC);
