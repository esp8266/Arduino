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
	spi(_spi), id(_id), initCb(_initCb)
{
	initCb();

	SPI_Configure(spi, id, SPI_MR_MSTR | SPI_MR_PS);
	SPI_Enable(spi);
}

void SPIClass::begin(uint8_t _pin) {
	if (_pin == 0)
		return;
	PIO_Configure(
		g_APinDescription[_pin].pPort,
		g_APinDescription[_pin].ulPinType,
		g_APinDescription[_pin].ulPin,
		g_APinDescription[_pin].ulPinConfiguration);
	setClockDivider(_pin, 1);
	setDataMode(_pin, SPI_MODE0);
}

void SPIClass::end() {
	SPI_Disable(spi);
}

//void SPIClass::setBitOrder(uint8_t _bitOrder, uint8_t _channel) {
//	// Not supported
//}

void SPIClass::setDataMode(uint8_t _pin, uint8_t _mode) {
	uint32_t _channel = SPI_PIN_TO_SPI_CHANNEL(_pin);
	mode[_channel] = _mode | SPI_CSR_CSAAT;
	SPI_ConfigureNPCS(spi, _channel, mode[_channel] | SPI_CSR_SCBR(divider[_channel]));
}

void SPIClass::setClockDivider(uint8_t _pin, uint8_t _divider) {
	uint32_t _channel = SPI_PIN_TO_SPI_CHANNEL(_pin);
	divider[_channel] = _divider;
	SPI_ConfigureNPCS(spi, _channel, mode[_channel] | SPI_CSR_SCBR(divider[_channel]));
}

byte SPIClass::transfer(byte _pin, uint8_t _data, SPITransferMode _mode) {
	uint32_t _channel = SPI_PIN_TO_SPI_CHANNEL(_pin);
	uint32_t d = _data | SPI_PCS(_channel);
	if (_mode == SPI_LAST)
		d |= SPI_TDR_LASTXFER;

	// SPI_Write(spi, _channel, _data);
    while ((spi->SPI_SR & SPI_SR_TDRE) == 0)
    	;
    spi->SPI_TDR = d;

    // return SPI_Read(spi);
    while ((spi->SPI_SR & SPI_SR_RDRF) == 0)
    	;
    d = spi->SPI_RDR;
    return d & 0xFF;
}

void SPIClass::attachInterrupt(void) {
	// Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void) {
	// Should be disableInterrupt()
}

#if SPI_INTERFACES_COUNT > 0
static void SPI_0_Init(void) {
	PIO_Configure(
			g_APinDescription[PIN_SPI_MOSI].pPort,
			g_APinDescription[PIN_SPI_MOSI].ulPinType,
			g_APinDescription[PIN_SPI_MOSI].ulPin,
			g_APinDescription[PIN_SPI_MOSI].ulPinConfiguration);
	PIO_Configure(
			g_APinDescription[PIN_SPI_MISO].pPort,
			g_APinDescription[PIN_SPI_MISO].ulPinType,
			g_APinDescription[PIN_SPI_MISO].ulPin,
			g_APinDescription[PIN_SPI_MISO].ulPinConfiguration);
	PIO_Configure(
			g_APinDescription[PIN_SPI_SCK].pPort,
			g_APinDescription[PIN_SPI_SCK].ulPinType,
			g_APinDescription[PIN_SPI_SCK].ulPin,
			g_APinDescription[PIN_SPI_SCK].ulPinConfiguration);
}

SPIClass SPI(SPI_INTERFACE, SPI_INTERFACE_ID, SPI_0_Init);
#endif
