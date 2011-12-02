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

SPIClass::SPIClass(Spi *_spi, uint32_t _id) : spi(_spi), id(_id) {
	// Empty
}

void SPIClass::begin() {
	// Set CS on NPCS3
	SPI_Configure(spi, id, SPI_MR_MSTR | SPI_MR_PS | SPI_MR_PCS(0x07));
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
SPIClass SPI0(SPI_INTERFACE, SPI_INTERFACE_ID);
#endif
