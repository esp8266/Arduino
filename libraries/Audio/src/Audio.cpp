/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@arduino.cc>
 * Audio library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "Audio.h"

void AudioClass::begin(uint32_t sampleRate, uint32_t msPreBuffer) {
	// Allocate a buffer to keep msPreBuffer milliseconds of audio
	bufferSize = msPreBuffer * sampleRate / 1000;
	if (bufferSize < 1024)
		bufferSize = 1024;
	buffer = (uint32_t *) malloc(bufferSize * sizeof(uint32_t));
	half = buffer + bufferSize / 2;
	last = buffer + bufferSize;

	// Buffering starts from the beginning
	running = buffer;
	next = buffer;

	// Start DAC
	dac->begin(VARIANT_MCK / sampleRate);
	dac->setOnTransmitEnd_CB(onTransmitEnd, this);
}

void AudioClass::end() {
	dac->end();
	free( buffer);
}

void AudioClass::prepare(int16_t *buffer, int S, int volume){
    uint16_t *ubuffer = (uint16_t*) buffer;
    for (int i=0; i<S; i++) {
        // set volume amplitude (signed multiply)
        buffer[i] = buffer[i] * volume / 1024;
        // convert from signed 16 bit to unsigned 12 bit for DAC.
        ubuffer[i] += 0x8000;
        ubuffer[i] >>= 4;
    }
}

size_t AudioClass::write(const uint32_t *data, size_t size) {
	const uint32_t TAG = 0x10000000;
	int i;
	for (i = 0; i < size; i++) {
		*next = data[i] | TAG;
		next++;

		if (next == half || next == last) {
			enqueue();
			while (next == running)
				;
		}
	}

	return i;
}

void AudioClass::enqueue() {
	if (!dac->canQueue()) {
		// DMA queue full
		return;
	}

	if (next == half) {
		// Enqueue the first half
		dac->queueBuffer(buffer, bufferSize / 2);
	} else {
		// Enqueue the second half
		dac->queueBuffer(half, bufferSize / 2);
		next = buffer; // wrap around
	}
}

void AudioClass::onTransmitEnd(void *_me) {
	AudioClass *me = reinterpret_cast<AudioClass *> (_me);
	if (me->running == me->buffer)
		me->running = me->half;
	else
		me->running = me->buffer;
}

AudioClass Audio(DAC);
