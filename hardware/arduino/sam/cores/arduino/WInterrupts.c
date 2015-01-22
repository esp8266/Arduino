/*
  Copyright (c) 2011-2012 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "WInterrupts.h"

typedef void (*interruptCB)(void);

static interruptCB callbacksPioA[32];
static interruptCB callbacksPioB[32];
static interruptCB callbacksPioC[32];
static interruptCB callbacksPioD[32];

/* Configure PIO interrupt sources */
static void __initialize() {
	int i;
	for (i=0; i<32; i++) {
		callbacksPioA[i] = NULL;
		callbacksPioB[i] = NULL;
		callbacksPioC[i] = NULL;
		callbacksPioD[i] = NULL;
	}

	pmc_enable_periph_clk(ID_PIOA);
	NVIC_DisableIRQ(PIOA_IRQn);
	NVIC_ClearPendingIRQ(PIOA_IRQn);
	NVIC_SetPriority(PIOA_IRQn, 0);
	NVIC_EnableIRQ(PIOA_IRQn);

	pmc_enable_periph_clk(ID_PIOB);
	NVIC_DisableIRQ(PIOB_IRQn);
	NVIC_ClearPendingIRQ(PIOB_IRQn);
	NVIC_SetPriority(PIOB_IRQn, 0);
	NVIC_EnableIRQ(PIOB_IRQn);

	pmc_enable_periph_clk(ID_PIOC);
	NVIC_DisableIRQ(PIOC_IRQn);
	NVIC_ClearPendingIRQ(PIOC_IRQn);
	NVIC_SetPriority(PIOC_IRQn, 0);
	NVIC_EnableIRQ(PIOC_IRQn);

	pmc_enable_periph_clk(ID_PIOD);
	NVIC_DisableIRQ(PIOD_IRQn);
	NVIC_ClearPendingIRQ(PIOD_IRQn);
	NVIC_SetPriority(PIOD_IRQn, 0);
	NVIC_EnableIRQ(PIOD_IRQn);
}


void attachInterrupt(uint32_t pin, void (*callback)(void), uint32_t mode)
{
	static int enabled = 0;
	if (!enabled) {
		__initialize();
		enabled = 1;
	}

	// Retrieve pin information
	Pio *pio = g_APinDescription[pin].pPort;
	uint32_t mask = g_APinDescription[pin].ulPin;
	uint32_t pos = 0;

	uint32_t t;
	for (t = mask; t>1; t>>=1, pos++)
		;

	// Set callback function
	if (pio == PIOA)
		callbacksPioA[pos] = callback;
	if (pio == PIOB)
		callbacksPioB[pos] = callback;
	if (pio == PIOC)
		callbacksPioC[pos] = callback;
	if (pio == PIOD)
		callbacksPioD[pos] = callback;

	// Configure the interrupt mode
	if (mode == CHANGE) {
		// Disable additional interrupt mode (detects both rising and falling edges)
		pio->PIO_AIMDR = mask;
	} else {
		// Enable additional interrupt mode
		pio->PIO_AIMER = mask;

		// Select mode of operation
		if (mode == LOW) {
			pio->PIO_LSR = mask;    // "Level" Select Register
			pio->PIO_FELLSR = mask; // "Falling Edge / Low Level" Select Register
		}
		if (mode == HIGH) {
			pio->PIO_LSR = mask;    // "Level" Select Register
			pio->PIO_REHLSR = mask; // "Rising Edge / High Level" Select Register
		}
		if (mode == FALLING) {
			pio->PIO_ESR = mask;    // "Edge" Select Register
			pio->PIO_FELLSR = mask; // "Falling Edge / Low Level" Select Register
		}
		if (mode == RISING) {
			pio->PIO_ESR = mask;    // "Edge" Select Register
			pio->PIO_REHLSR = mask; // "Rising Edge / High Level" Select Register
		}
	}

	// Enable interrupt
	pio->PIO_IER = mask;
}

void detachInterrupt(uint32_t pin)
{
	// Retrieve pin information
	Pio *pio = g_APinDescription[pin].pPort;
	uint32_t mask = g_APinDescription[pin].ulPin;

	// Disable interrupt
	pio->PIO_IDR = mask;
}

#ifdef __cplusplus
extern "C" {
#endif

void PIOA_Handler(void) {
	uint32_t isr = PIOA->PIO_ISR;
	uint32_t i;
	for (i=0; i<32; i++, isr>>=1) {
		if ((isr & 0x1) == 0)
			continue;
		if (callbacksPioA[i])
			callbacksPioA[i]();
	}
}

void PIOB_Handler(void) {
	uint32_t isr = PIOB->PIO_ISR;
	uint32_t i;
	for (i=0; i<32; i++, isr>>=1) {
		if ((isr & 0x1) == 0)
			continue;
		if (callbacksPioB[i])
			callbacksPioB[i]();
	}
}

void PIOC_Handler(void) {
	uint32_t isr = PIOC->PIO_ISR;
	uint32_t i;
	for (i=0; i<32; i++, isr>>=1) {
		if ((isr & 0x1) == 0)
			continue;
		if (callbacksPioC[i])
			callbacksPioC[i]();
	}
}

void PIOD_Handler(void) {
	uint32_t isr = PIOD->PIO_ISR;
	uint32_t i;
	for (i=0; i<32; i++, isr>>=1) {
		if ((isr & 0x1) == 0)
			continue;
		if (callbacksPioD[i])
			callbacksPioD[i]();
	}
}

#ifdef __cplusplus
}
#endif
