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

void AudioClass::begin(uint32_t sampleRate) {
	const uint32_t T = VARIANT_MCK / sampleRate;
	dac->begin(T);

	currentBuffer = 0;
}

void AudioClass::end() {
	dac->end();
}

uint32_t *AudioClass::cook(const uint32_t *buffer, size_t size) {
	if (currentBuffer == 0) {
		// Use buffer0
		for (int i = 0; i < size; i++)
			buffer0[i] = buffer[i] | 0x10000000;
		currentBuffer = 1;
		return buffer0;
	} else {
		// Use buffer1
		for (int i = 0; i < size; i++)
			buffer1[i] = buffer[i] | 0x10000000;
		currentBuffer = 0;
		return buffer1;
	}
}

AudioClass Audio(DAC);
