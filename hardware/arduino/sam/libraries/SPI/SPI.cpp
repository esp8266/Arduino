/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

SPIClass::SPIClass(Spi *_spi, uint32_t _id, void(*_initCb)(void)) :
	spi(_spi), id(_id), initCb(_initCb) {
	// Empty
}

void SPIClass::begin() {
	initCb();

	// Set CS on NPCS3
	SPI_Configure(spi, id, SPI_MR_MSTR | SPI_MR_PCS(0x07));
	SPI_Enable( spi);
	setClockDivider(1);
}

void SPIClass::end() {
	SPI_Disable( spi);
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
	// Not supported
}

void SPIClass::setDataMode(uint8_t _mode) {
	mode = _mode;
	SPI_ConfigureNPCS(spi, 3, mode | SPI_CSR_SCBR(divider));
}

void SPIClass::setClockDivider(uint8_t _divider) {
	divider = _divider;
	SPI_ConfigureNPCS(spi, 3, mode | SPI_CSR_SCBR(divider));
}

byte SPIClass::transfer(byte _data) {
	SPI_Write(spi, 0, _data);
	return SPI_Read(spi);
}

void SPIClass::attachInterrupt(void) {
	// Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void) {
	// Should be disableInterrupt()
}

#if SPI_INTERFACES_COUNT > 0
static void SPI_0_Init(void) {
	PIO_Configure(g_APinDescription[PIN_SPI_MOSI].pPort,
			g_APinDescription[PIN_SPI_MOSI].ulPinType,
			g_APinDescription[PIN_SPI_MOSI].ulPin,
			g_APinDescription[PIN_SPI_MOSI].ulPinConfiguration);
	PIO_Configure(g_APinDescription[PIN_SPI_MISO].pPort,
			g_APinDescription[PIN_SPI_MISO].ulPinType,
			g_APinDescription[PIN_SPI_MISO].ulPin,
			g_APinDescription[PIN_SPI_MISO].ulPinConfiguration);
	PIO_Configure(g_APinDescription[PIN_SPI_SCK].pPort,
			g_APinDescription[PIN_SPI_SCK].ulPinType,
			g_APinDescription[PIN_SPI_SCK].ulPin,
			g_APinDescription[PIN_SPI_SCK].ulPinConfiguration);
}

SPIClass SPI_0(SPI_INTERFACE, SPI_INTERFACE_ID, SPI_0_Init);
#endif
