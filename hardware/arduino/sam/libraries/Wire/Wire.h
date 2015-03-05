/*
 * TwoWire.h - TWI/I2C library for Arduino Due
 * Copyright (c) 2011 Cristian Maglie <c.maglie@arduino.cc>
 * All rights reserved.
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

#ifndef TwoWire_h
#define TwoWire_h

// Include Atmel CMSIS driver
#include <include/twi.h>

#include "Stream.h"
#include "variant.h"

#define BUFFER_LENGTH 32

class TwoWire : public Stream {
public:
	TwoWire(Twi *twi, void(*begin_cb)(void));
	void begin();
	void begin(uint8_t);
	void begin(int);
	void setClock(uint32_t);
	void beginTransmission(uint8_t);
	void beginTransmission(int);
	uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t);
	uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
	uint8_t requestFrom(int, int);
    uint8_t requestFrom(int, int, int);
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *, size_t);
	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);
	void onReceive(void(*)(int));
	void onRequest(void(*)(void));

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;

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

	// Called before initialization
	void (*onBeginCallback)(void);

	// TWI instance
	Twi *twi;

	// TWI state
	enum TwoWireStatus {
		UNINITIALIZED,
		MASTER_IDLE,
		MASTER_SEND,
		MASTER_RECV,
		SLAVE_IDLE,
		SLAVE_RECV,
		SLAVE_SEND
	};
	TwoWireStatus status;

	// TWI clock frequency
	static const uint32_t TWI_CLOCK = 100000;
	uint32_t twiClock;

	// Timeouts (
	static const uint32_t RECV_TIMEOUT = 100000;
	static const uint32_t XMIT_TIMEOUT = 100000;
};

#if WIRE_INTERFACES_COUNT > 0
extern TwoWire Wire;
#endif
#if WIRE_INTERFACES_COUNT > 1
extern TwoWire Wire1;
#endif

#endif

