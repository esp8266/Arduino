/*
  Copyright (c) 2012 Arduino.  All right reserved.

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

#include <Arduino.h>
#include "Reset.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((long_call, section (".ramfunc")))
void banzai() {
	// Disable all interrupts
	__disable_irq();

	// Set bootflag to run SAM-BA bootloader at restart
	const int EEFC_FCMD_CGPB = 0x0C;
	const int EEFC_KEY = 0x5A;
	while (EFC0->EEFC_FSR & EEFC_FSR_FRDY == 0);
	EFC0->EEFC_FCR =
		EEFC_FCR_FCMD(EEFC_FCMD_CGPB) |
		EEFC_FCR_FARG(1) |
		EEFC_FCR_FKEY(EEFC_KEY);
	while (EFC0->EEFC_FSR & EEFC_FSR_FRDY == 0);

	// From here flash memory is no more available.

	// Memory swap needs some time to stabilize
	for (uint32_t i=0; i<1000000; i++)
		// force compiler to not optimize this
		__asm__ __volatile__("");

	// BANZAIIIIIII!!!
	const int RSTC_KEY = 0xA5;
	RSTC->RSTC_CR =
		RSTC_CR_KEY(RSTC_KEY) |
		RSTC_CR_PROCRST |
		RSTC_CR_PERRST;

	while (true);
}

static int ticks = -1;

void initiateReset(int _ticks) {
	ticks = _ticks;
}

void cancelReset() {
	ticks = -1;
}

void tickReset() {
	if (ticks == -1)
		return;
	ticks--;
	if (ticks == 0)
		banzai();
}

#ifdef __cplusplus
}
#endif
