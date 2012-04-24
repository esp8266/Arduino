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

SPIClass::SPIClass(Spi *_spi, uint32_t _id, void(*_initCb)(void), uint32_t *_ss) :
	spi(_spi), id(_id), initCb(_initCb)
{
	for (int i=0; i<SPI_CHANNELS_NUM; i++)
		ssPins[i] = _ss[i];
}

void SPIClass::begin() {
	initCb();

	// Set CS on NPCS3
	SPI_Configure(spi, id, SPI_MR_MSTR | SPI_MR_PS);
	SPI_Enable(spi);
	setClockDivider(1);
	setDataMode(0);
}

void SPIClass::addSlave(uint8_t _channel) {
	uint32_t pin = ssPins[_channel];
	if (pin == 0)
		return;
	PIO_Configure(g_APinDescription[pin].pPort,
		g_APinDescription[pin].ulPinType,
		g_APinDescription[pin].ulPin,
		g_APinDescription[pin].ulPinConfiguration);
}

void SPIClass::end() {
	SPI_Disable(spi);
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
	setBitOrder(bitOrder, 0);
	setBitOrder(bitOrder, 1);
	setBitOrder(bitOrder, 2);
	setBitOrder(bitOrder, 3);
}

void SPIClass::setBitOrder(uint8_t bitOrder, uint8_t _channel) {
	// Not supported
}

void SPIClass::setDataMode(uint8_t _mode) {
	setDataMode(_mode, 0);
	setDataMode(_mode, 1);
	setDataMode(_mode, 2);
	setDataMode(_mode, 3);
}

void SPIClass::setDataMode(uint8_t _mode, uint8_t _channel) {
	mode[_channel] = _mode | SPI_CSR_CSAAT;
	SPI_ConfigureNPCS(spi, _channel, mode[_channel] | SPI_CSR_SCBR(divider[_channel]));
}

void SPIClass::setClockDivider(uint8_t _divider) {
	setClockDivider(_divider, 0);
	setClockDivider(_divider, 1);
	setClockDivider(_divider, 2);
	setClockDivider(_divider, 3);
}

void SPIClass::setClockDivider(uint8_t _divider, uint8_t _channel) {
	divider[_channel] = _divider;
	SPI_ConfigureNPCS(spi, _channel, mode[_channel] | SPI_CSR_SCBR(divider[_channel]));
}

byte SPIClass::transfer(byte _data, uint8_t _channel, bool _last) {
	uint32_t d = _data | SPI_PCS(_channel);
	if (_last)
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

uint32_t SPI_0_SS[] = {
	PIN_SPI_SS0,
	PIN_SPI_SS1,
	PIN_SPI_SS2,
	PIN_SPI_SS3
};

SPIClass SPI_0(SPI_INTERFACE, SPI_INTERFACE_ID, SPI_0_Init, SPI_0_SS);
#endif
