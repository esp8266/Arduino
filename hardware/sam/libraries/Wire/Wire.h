/*
 * DueWire.h - TWI/I2C library for Arduino Due
 * Copyright (c) 2011 Cristian Maglie <c.maglie@bug.st>.
 * All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DueWire_h
#define DueWire_h

#include <inttypes.h>
#include "Stream.h"
#include "twi.h"

#define BUFFER_LENGTH 32

class DueWire: public Stream {
public:
	DueWire(Twi *);
	void begin();
	void begin(uint8_t);
	void begin(int);
	void beginTransmission(uint8_t);
	void beginTransmission(int);
	uint8_t endTransmission(void);
	uint8_t requestFrom(uint8_t, uint8_t);
	uint8_t requestFrom(int, int);
	virtual void write(uint8_t);
	virtual void write(const char *);
	virtual void write(const uint8_t *, size_t);
	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);
	void onReceive(void(*)(int));
	void onRequest(void(*)(void));

	void onService(void);

private:
	// RX Buffer
	uint8_t rxBuffer[BUFFER_LENGTH];
	uint8_t rxBufferIndex;
	uint8_t rxBufferLength;

	// TX Buffer
	uint8_t txAddress;
	uint8_t txBuffer[BUFFER_LENGTH];
	uint8_t txBufferLength;

	// Service buffer
	uint8_t srvBuffer[BUFFER_LENGTH];
	uint8_t srvBufferIndex;
	uint8_t srvBufferLength;

	// Callback user functions
	void (*onRequestCallback)(void);
	void (*onReceiveCallback)(int);

	// TWI instance
	Twi *twi;

	// TWI state
	static const uint8_t UNINITIALIZED = 0;
	static const uint8_t MASTER_IDLE = 1;
	static const uint8_t MASTER_SEND = 2;
	static const uint8_t MASTER_RECV = 3;
	static const uint8_t SLAVE_IDLE = 4;
	static const uint8_t SLAVE_RECV = 5;
	static const uint8_t SLAVE_SEND = 6;
	uint8_t status;
};

extern DueWire Wire;
extern DueWire Wire2;

#endif

