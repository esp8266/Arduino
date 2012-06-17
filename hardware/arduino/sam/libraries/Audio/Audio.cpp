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

static void TC_SetRA(Tc *tc, uint32_t chan, uint32_t v) {
	tc->TC_CHANNEL[chan].TC_RA = v;
}

static void TC_SetRB(Tc *tc, uint32_t chan, uint32_t v) {
	tc->TC_CHANNEL[chan].TC_RB = v;
}

static void TC_SetRC(Tc *tc, uint32_t chan, uint32_t v) {
	tc->TC_CHANNEL[chan].TC_RC = v;
}

void AudioClass::begin(uint32_t sampleRate) {
	// Enable clock for DAC
	pmc_enable_periph_clk(dacId);

	dacc_reset(dac);

	// Set transfer mode to double word
	dacc_set_transfer_mode(dac, 1);

	// Power save:
	// sleep mode  - 0 (disabled)
	// fast wakeup - 0 (disabled)
	dacc_set_power_save(dac, 0, 0);

	// ADC refresh/startup timings:
	// refresh        - 0x08 (1024*8 dacc clocks)
	// max speed mode -    0 (disabled)
	// startup time   - 0x10 (1024 dacc clocks)
	dacc_set_timing(dac, 0x08, 0, DACC_MR_STARTUP_1024);

	// Flexible channel selection with tags
	dacc_enable_flexible_selection(dac);

	// Set up analog current
	dacc_set_analog_control(dac,
			DACC_ACR_IBCTLCH0(0x02) |
			DACC_ACR_IBCTLCH1(0x02) |
			DACC_ACR_IBCTLDACCORE(0x01));

	// Enable output channels
	dacc_enable_channel(dac, 0);
	dacc_enable_channel(dac, 1);


	// Configure Timer Counter to trigger DAC
	// --------------------------------------
	pmc_enable_periph_clk(ID_TC1);
	TC_Configure(TC0, 1,
		TC_CMR_TCCLKS_TIMER_CLOCK2 |  // Clock at MCR/8
		TC_CMR_WAVE |                 // Waveform mode
		TC_CMR_WAVSEL_UP_RC |         // Counter running up and reset when equals to RC
		TC_CMR_ACPA_SET | TC_CMR_ACPC_CLEAR);
	const uint32_t TC = VARIANT_MCK / 8 / sampleRate;
	TC_SetRA(TC0, 1, TC / 2);
	TC_SetRC(TC0, 1, TC);
	TC_Start(TC0, 1);

	// Configure clock source for DAC (2 = TC0 Output Chan. 1)
	dacc_set_trigger(dac, 2);

	// Configure pins
	PIO_Configure(g_APinDescription[DA0].pPort,
			g_APinDescription[DA0].ulPinType,
			g_APinDescription[DA0].ulPin,
			g_APinDescription[DA0].ulPinConfiguration);
	PIO_Configure(g_APinDescription[DA1].pPort,
			g_APinDescription[DA1].ulPinType,
			g_APinDescription[DA1].ulPin,
			g_APinDescription[DA1].ulPinConfiguration);

	currentBuffer = 0;
}

void AudioClass::end() {
	TC_Stop(TC0, 1);
	dacc_disable_channel(dac, 0);
	dacc_disable_channel(dac, 1);
}

size_t AudioClass::write(const uint32_t *buffer, size_t size) {
	// Try the first PDC buffer
	if ((dac->DACC_TCR == 0) && (dac->DACC_TNCR == 0)) {
		dac->DACC_TPR = (uint32_t) cook(buffer, size);
		dac->DACC_TCR = size;
		dac->DACC_PTCR = DACC_PTCR_TXTEN;
		return size;
	}

	// Try the second PDC buffer
	if (dac->DACC_TNCR == 0) {
		dac->DACC_TNPR = (uint32_t) cook(buffer, size);
		dac->DACC_TNCR = size;
		dac->DACC_PTCR = DACC_PTCR_TXTEN;
		return size;
	}

	// PDC buffers full, try again later...
	return 0;
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

AudioClass Audio(DACC_INTERFACE, DACC_INTERFACE_ID);

//void DACC_IrqHandler(void) {
//	uint32_t sr = DACC_INTERFACE->DACC_ISR;
//
//	if (sr & DACC_ISR_ENDTX) {
//		/* End of transmission */
//		Audio.write(wBuffer, SAMPLES);
//	}
//}
