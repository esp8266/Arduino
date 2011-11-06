/*
 * DueWire.cpp - TWI/I2C library Arduino Due
 * Copyright (c) 2011 Cristian Maglie.  All right reserved.
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

extern "C" {
#include <string.h>
#include "twi.h"
}

#include "Wire.h"

static inline bool TWI_FailedAcknowledge(Twi *pTwi) {
	return pTwi->TWI_SR & TWI_SR_NACK;
}

static inline bool TWI_WaitTransferComplete(Twi *_twi, uint32_t _timeout) {
	while (!TWI_TransferComplete(_twi)) {
		if (TWI_FailedAcknowledge(_twi))
			return false;
		if (--_timeout == 0)
			return false;
	}
	return true;
}

static inline bool TWI_WaitByteSent(Twi *_twi, uint32_t _timeout) {
	while (!TWI_ByteSent(_twi)) {
		if (TWI_FailedAcknowledge(_twi))
			return false;
		if (--_timeout == 0)
			return false;
	}
	return true;
}

static inline bool TWI_WaitByteReceived(Twi *_twi, uint32_t _timeout) {
	while (!TWI_ByteReceived(_twi)) {
		if (TWI_FailedAcknowledge(_twi))
			return false;
		if (--_timeout == 0)
			return false;
	}
	return true;
}

static inline bool TWI_STATUS_SVREAD(uint32_t status) {
	return (status & TWI_SR_SVREAD) == TWI_SR_SVREAD;
}

DueWire::DueWire(Twi *_twi, void(*_beginCb)(void)) :
	twi(_twi), rxBufferIndex(0), rxBufferLength(0), txAddress(0),
			txBufferLength(0), srvBufferIndex(0), srvBufferLength(0), status(
					UNINITIALIZED), onBeginCallback(_beginCb) {
	// Empty
}

void DueWire::begin(void) {
	if (onBeginCallback)
		onBeginCallback();
	TWI_ConfigureMaster(twi, TWI_CLOCK, VARIANT_MCK);
	status = MASTER_IDLE;
}

void DueWire::begin(uint8_t address) {
	if (onBeginCallback)
		onBeginCallback();
	TWI_ConfigureSlave(twi, address);
	status = SLAVE_IDLE;
	TWI_EnableIt(twi, TWI_IER_RXRDY | TWI_IER_TXRDY | TWI_IER_TXCOMP);
}

void DueWire::begin(int address) {
	begin((uint8_t) address);
}

uint8_t DueWire::requestFrom(uint8_t address, uint8_t quantity) {
	if (quantity > BUFFER_LENGTH)
		quantity = BUFFER_LENGTH;

	// perform blocking read into buffer
	int readed = 0;
	TWI_StartRead(twi, address, 0, 0);
	do {
		// Stop condition must be set during the reception of last byte
		if (readed + 1 == quantity)
			TWI_SendSTOPCondition( twi);

		TWI_WaitByteReceived(twi, RECV_TIMEOUT);
		rxBuffer[readed++] = TWI_ReadByte(twi);
	} while (readed < quantity);
	TWI_WaitTransferComplete(twi, RECV_TIMEOUT);

	// set rx buffer iterator vars
	rxBufferIndex = 0;
	rxBufferLength = readed;

	return readed;
}

uint8_t DueWire::requestFrom(int address, int quantity) {
	return requestFrom((uint8_t) address, (uint8_t) quantity);
}

void DueWire::beginTransmission(uint8_t address) {
	status = MASTER_SEND;

	// save address of target and empty buffer
	txAddress = address;
	txBufferLength = 0;
}

void DueWire::beginTransmission(int address) {
	beginTransmission((uint8_t) address);
}

uint8_t DueWire::endTransmission(void) {
	// transmit buffer (blocking)
	TWI_StartWrite(twi, txAddress, 0, 0, txBuffer[0]);
	TWI_WaitByteSent(twi, XMIT_TIMEOUT);
	int sent = 1;
	while (sent < txBufferLength) {
		TWI_WriteByte(twi, txBuffer[sent++]);
		TWI_WaitByteSent(twi, XMIT_TIMEOUT);
	}
	TWI_Stop( twi);
	TWI_WaitTransferComplete(twi, XMIT_TIMEOUT);

	// empty buffer
	txBufferLength = 0;

	status = MASTER_IDLE;
	return sent;
}

void DueWire::write(uint8_t data) {
	if (status == MASTER_SEND) {
		if (txBufferLength >= BUFFER_LENGTH)
			return;
		txBuffer[txBufferLength++] = data;
	} else {
		if (srvBufferLength >= BUFFER_LENGTH)
			return;
		srvBuffer[srvBufferIndex++] = data;
	}
}

void DueWire::write(const uint8_t *data, size_t quantity) {
	if (status == MASTER_SEND) {
		for (size_t i = 0; i < quantity; ++i) {
			if (txBufferLength >= BUFFER_LENGTH)
				return;
			txBuffer[txBufferLength++] = data[i];
		}
	} else {
		for (size_t i = 0; i < quantity; ++i) {
			if (srvBufferLength >= BUFFER_LENGTH)
				return;
			srvBuffer[srvBufferLength++] = data[i];
		}
	}
}

void DueWire::write(const char *data) {
	write((uint8_t*) data, strlen(data));
}

int DueWire::available(void) {
	return rxBufferLength - rxBufferIndex;
}

int DueWire::read(void) {
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex++];
	return -1;
}

int DueWire::peek(void) {
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex];
	return -1;
}

void DueWire::flush(void) {
	// Do nothing, use endTransmission(..) to force
	// data transfer.
}

void DueWire::onReceive(void(*function)(int)) {
	onReceiveCallback = function;
}

void DueWire::onRequest(void(*function)(void)) {
	onRequestCallback = function;
}

void DueWire::onService(void) {
	// Retrieve interrupt status
	uint32_t sr = TWI_GetMaskedStatus(twi);

	// SLAVE_IDLE status
	if (status == SLAVE_IDLE) {
		// Detect if we should go into RECV or SEND status
		srvBufferLength = 0;
		if (TWI_STATUS_SVREAD(sr)) {
			status = SLAVE_RECV;
		} else {
			srvBufferIndex = 0;
			status = SLAVE_SEND;

			// Alert calling program to generate a response ASAP
			if (onRequestCallback)
				onRequestCallback();
			else
				// create a default 1-byte response
				write((uint8_t) 0);
		}
	}

	// SLAVE_RECV status: receiving packet
	if (status == SLAVE_RECV) {
		if (TWI_STATUS_RXRDY(sr)) {
			if (srvBufferLength < BUFFER_LENGTH)
				srvBuffer[srvBufferLength++] = TWI_ReadByte(twi);
		}
		if (TWI_STATUS_TXCOMP(sr)) {
			// Receive completed
			status = SLAVE_IDLE;

			// Alert calling program
			if (onReceiveCallback) {
				// Copy data into rxBuffer
				// (allows to receive another packet while
				// the main application reads actual data)
				for (uint8_t i = 0; i < srvBufferLength; ++i)
					rxBuffer[i] = srvBuffer[i];
				rxBufferIndex = 0;
				rxBufferLength = srvBufferLength;

				onReceiveCallback( rxBufferLength);
			}
		}
	}

	// SLAVE_SEND status: sending packet
	if (status == SLAVE_SEND) {
		if (TWI_STATUS_TXRDY(sr)) {
			uint8_t c = 0;
			if (srvBufferIndex < srvBufferLength)
				c = srvBuffer[srvBufferIndex++];
			TWI_WriteByte(twi, c);
		}

		if (TWI_STATUS_TXCOMP(sr)) {
			// Transmission complete
			status = SLAVE_IDLE;
		}
	}
}

#if WIRE_INTERFACES_COUNT > 0
static void Wire_Init(void) {
	PMC_EnablePeripheral( WIRE_INTERFACE_ID);
	PIO_Configure(g_APinDescription[PIN_WIRE_SDA].pPort,
			g_APinDescription[PIN_WIRE_SDA].ulPinType,
			g_APinDescription[PIN_WIRE_SDA].ulPin,
			g_APinDescription[PIN_WIRE_SDA].ulPinConfiguration);
	PIO_Configure(g_APinDescription[PIN_WIRE_SCL].pPort,
			g_APinDescription[PIN_WIRE_SCL].ulPinType,
			g_APinDescription[PIN_WIRE_SCL].ulPin,
			g_APinDescription[PIN_WIRE_SCL].ulPinConfiguration);
}

DueWire Wire = DueWire(WIRE_INTERFACE, Wire_Init);

void TWI1_IrqHandler(void) {
	Wire.onService();
}
#endif

#if WIRE_INTERFACES_COUNT > 1
static void Wire1_Init(void) {
	PMC_EnablePeripheral( WIRE1_INTERFACE_ID);
	PIO_Configure(g_APinDescription[PIN_WIRE1_SDA].pPort,
			g_APinDescription[PIN_WIRE1_SDA].ulPinType,
			g_APinDescription[PIN_WIRE1_SDA].ulPin,
			g_APinDescription[PIN_WIRE1_SDA].ulPinConfiguration);
	PIO_Configure(g_APinDescription[PIN_WIRE1_SCL].pPort,
			g_APinDescription[PIN_WIRE1_SCL].ulPinType,
			g_APinDescription[PIN_WIRE1_SCL].ulPin,
			g_APinDescription[PIN_WIRE1_SCL].ulPinConfiguration);
}

DueWire Wire1 = DueWire(WIRE1_INTERFACE, Wire1_Init);

void TWI0_IrqHandler(void) {
	Wire1.onService();
}
#endif
