/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include "variant.h"
#include <stdio.h>

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01

enum SPITransferMode {
	SPI_CONTINUE,
	SPI_LAST
};

class SPIClass {
  public:
	SPIClass(Spi *_spi, uint32_t _id, void(*_initCb)(void));

	//byte transfer(uint8_t _data, SPITransferMode _mode = SPI_LAST) { transfer(BOARD_PIN_SS_DEFAULT, _data, _mode); }
	byte transfer(byte _channel, uint8_t _data, SPITransferMode _mode = SPI_LAST);

	// SPI Configuration methods

	void attachInterrupt(void);
	void detachInterrupt(void);

	void begin(uint8_t _channel);
	//void begin(void) { begin(BOARD_PIN_SS_DEFAULT); };
	void end(void);

	// These methods sets a parameter on a single pin
	//void setBitOrder(uint8_t _channel, uint8_t);
	void setDataMode(uint8_t _channel, uint8_t);
	void setClockDivider(uint8_t _channel, uint8_t);

	// These methods sets the same parameters but on default pin PIN_SPI_SS_DEFAULT
	//void setBitOrder(uint8_t _order) { setBitOrder(PIN_SPI_SS_DEFAULT, _order); };
	//void setDataMode(uint8_t _mode) { setDataMode(BOARD_PIN_SS_DEFAULT, _mode); };
	//void setClockDivider(uint8_t _div) { setClockDivider(BOARD_PIN_SS_DEFAULT, _div); };

  private:
	Spi *spi;
	uint32_t id;
	uint32_t divider[SPI_CHANNELS_NUM];
	uint32_t mode[SPI_CHANNELS_NUM];
	void (*initCb)(void);
};

#if SPI_INTERFACES_COUNT > 0
extern SPIClass SPI;
#endif

#endif
