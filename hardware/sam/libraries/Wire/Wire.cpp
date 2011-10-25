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
#include <inttypes.h>
#include "twi.h"
}

#include "Wire.h"

static inline void TWI_WaitTransferComplete(Twi *_twi) {
	while (!TWI_TransferComplete(_twi))
		;
}

static inline void TWI_WaitByteSent(Twi *_twi) {
	while (!TWI_ByteSent(_twi))
		;
}

static inline void TWI_WaitByteReceived(Twi *_twi) {
	while (!TWI_ByteReceived(_twi))
		;
}

static inline bool TWI_STATUS_SVREAD(uint32_t status) {
	return (status & TWI_SR_SVREAD) == TWI_SR_SVREAD;
}

DueWire::DueWire(Twi *_twi) :
	twi(_twi), rxBufferIndex(0), rxBufferLength(0), txAddress(0),
			txBufferLength(0), srvBufferIndex(0), srvBufferLength(0), status(
					UNINITIALIZED) {
	// Empty
}

void DueWire::begin(void) {
	// TODO: correct clock values
	TWI_ConfigureMaster(twi, 200000, 200000);
	status = MASTER_IDLE;
}

void DueWire::begin(uint8_t address) {
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
		// Stop condition must be set during the receprion of last byte
		if (readed + 1 == quantity)
			TWI_SendSTOPCondition( twi);

		TWI_WaitByteReceived( twi);
		rxBuffer[readed++] = TWI_ReadByte(twi);
	} while (readed < quantity);
	TWI_WaitTransferComplete( twi);

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
	TWI_WaitByteSent( twi);
	int sent = 1;
	while (sent < txBufferLength) {
		TWI_WriteByte(twi, txBuffer[sent++]);
		TWI_WaitByteSent(twi);
	}
	TWI_Stop(twi);
	TWI_WaitTransferComplete(twi);

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
	// XXX: to be implemented.
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

	// Detect if we should go into RECV or SEND status
	if (status == SLAVE_IDLE) {
		srvBufferLength = 0;
		if (TWI_STATUS_SVREAD(sr)) {
			status = SLAVE_RECV;
		} else {
			srvBufferIndex = 0;
			status = SLAVE_SEND;

			// Alert calling program to generate a response ASAP
			if (onRequestCallback != NULL)
				onRequestCallback();
			else
				// default response
				write((uint8_t) 0);
		}
	}

	// Receive packet
	if (status == SLAVE_RECV) {
		if (TWI_STATUS_RXRDY(sr)) {
			if (srvBufferLength < BUFFER_LENGTH)
				srvBuffer[srvBufferLength++] = TWI_ReadByte(twi);
		}
		if (TWI_STATUS_TXCOMP(sr)) {
			// Receive completed
			status = SLAVE_IDLE;

			// Alert calling program
			if (onReceiveCallback != NULL) {
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

	// Send packet
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

DueWire Wire = DueWire(TWI0);
DueWire Wire2 = DueWire(TWI1);

void TWI0_IrqHandler(void) {
	Wire.onService();
}

void TWI1_IrqHandler(void) {
	Wire2.onService();
}
