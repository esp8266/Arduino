/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@bug.st>
 * DAC library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <DAC.h>

void DACClass::begin(uint32_t period) {
	// Enable clock for DAC
	pmc_enable_periph_clk(dacId);

	dacc_reset(dac);

	// Set transfer mode to double word
	dacc_set_transfer_mode(dac, 1);

	// Power save:
	// sleep mode  - 0 (disabled)
	// fast wakeup - 0 (disabled)
	dacc_set_power_save(dac, 0, 0);

	// DAC refresh/startup timings:
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
	const uint32_t TC = period / 8;
	TC_SetRA(TC0, 1, TC / 2);
	TC_SetRC(TC0, 1, TC);
	TC_Start(TC0, 1);

	// Configure clock source for DAC (2 = TC0 Output Chan. 1)
	dacc_set_trigger(dac, 2);

	// Configure pins
	PIO_Configure(g_APinDescription[DAC0].pPort,
			g_APinDescription[DAC0].ulPinType,
			g_APinDescription[DAC0].ulPin,
			g_APinDescription[DAC0].ulPinConfiguration);
	PIO_Configure(g_APinDescription[DAC1].pPort,
			g_APinDescription[DAC1].ulPinType,
			g_APinDescription[DAC1].ulPin,
			g_APinDescription[DAC1].ulPinConfiguration);

	// Enable interrupt controller for DAC
	dacc_disable_interrupt(dac, 0xFFFFFFFF);
	NVIC_DisableIRQ(isrId);
	NVIC_ClearPendingIRQ(isrId);
	NVIC_SetPriority(isrId, 0);
	NVIC_EnableIRQ(isrId);
}

void DACClass::end() {
	TC_Stop(TC0, 1);
	NVIC_DisableIRQ(isrId);
	dacc_disable_channel(dac, 0);
	dacc_disable_channel(dac, 1);
}

bool DACClass::canQueue() {
	return (dac->DACC_TNCR == 0);
}

size_t DACClass::queueBuffer(const uint32_t *buffer, size_t size) {
	// Try the first PDC buffer
	if ((dac->DACC_TCR == 0) && (dac->DACC_TNCR == 0)) {
		dac->DACC_TPR = (uint32_t) buffer;
		dac->DACC_TCR = size;
		dac->DACC_PTCR = DACC_PTCR_TXTEN;
		if (cb)
			dacc_enable_interrupt(dac, DACC_IER_ENDTX);
		return size;
	}

	// Try the second PDC buffer
	if (dac->DACC_TNCR == 0) {
		dac->DACC_TNPR = (uint32_t) buffer;
		dac->DACC_TNCR = size;
		dac->DACC_PTCR = DACC_PTCR_TXTEN;
		if (cb)
			dacc_enable_interrupt(dac, DACC_IER_ENDTX);
		return size;
	}

	// PDC buffers full, try again later...
	return 0;
}

void DACClass::setOnTransmitEnd_CB(OnTransmitEnd_CB _cb, void *_data) {
	cb = _cb;
	cbData = _data;
	if (!cb)
		dacc_disable_interrupt(dac, DACC_IDR_ENDTX);
}

void DACClass::onService() {
	uint32_t sr = dac->DACC_ISR;
	if (sr & DACC_ISR_ENDTX) {
		// There is a free slot, enqueue data
		dacc_disable_interrupt(dac, DACC_IDR_ENDTX);
		if (cb)
			cb(cbData);
	}
}

DACClass DAC(DACC_INTERFACE, DACC_INTERFACE_ID, DACC_ISR_ID);

void DACC_ISR_HANDLER(void) {
	DAC.onService();
}

