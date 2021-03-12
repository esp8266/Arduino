/*
    TwoWire.cpp - TWI/I2C library for Arduino & Wiring
    Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
    Modified December 2014 by Ivan Grokhotkov (ivan@esp8266.com) - esp8266 support
    Modified April 2015 by Hrsto Gochkov (ficeto@ficeto.com) - alternative esp8266 support
    Modified January 2017 by Bjorn Hammarberg (bjoham@esp8266.com) - i2c slave support
*/

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}

#include "twi.h"
#define TwoWire_h_IMPLEMENTATION
#include "Wire.h"


//Some boards don't have these pins available, and hence don't support Wire.
//Check here for compile-time error.
#if !defined(PIN_WIRE_SDA) || !defined(PIN_WIRE_SCL)
#error Wire library is not supported on this board
#endif

// Private Methods /////////////////////////////////////////////////////////////
// Constructors ////////////////////////////////////////////////////////////////

TwoWireBase::TwoWireBase(uint8_t rxBufferSize, uint8_t txBufferSize)
    :
    twiMaster(new TwiMaster),
    rxBufferSize(rxBufferSize),
    rxBuffer(new uint8_t[rxBufferSize]),
    txBufferSize(txBufferSize),
    txBuffer(new uint8_t[txBufferSize])
{
}

TwoWireBase::TwoWireBase(TwiMaster* twiPtr, uint8_t rxBufferSize, uint8_t txBufferSize, uint8_t* rxBuffer, uint8_t* txBuffer)
    :
    twiMaster{twiPtr},
    rxBufferSize{rxBufferSize},
    rxBuffer{rxBuffer},
    txBufferSize{txBufferSize},
    txBuffer{txBuffer}
{
}

inline TwiMaster& TwoWireBase::getTwiMaster()
{
    return *twiMaster;
}
inline void TwoWireBase::releaseTwiMaster()
{
    twiMaster.release();
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWireBase::begin(int sda, int scl)
{
    lastSdaPin = sda;
    lastSclPin = scl;
    getTwiMaster().init(sda, scl);
    flush();
}

void TwoWireBase::begin(void)
{
    begin(lastSdaPin, lastSclPin);
}

void TwoWireBase::pins(int sda, int scl)
{
    lastSdaPin = sda;
    lastSclPin = scl;
}

uint8_t TwoWireBase::status()
{
    return getTwiMaster().status();
}

void TwoWireBase::setClock(uint32_t frequency)
{
    getTwiMaster().setClock(frequency);
}

void TwoWireBase::setClockStretchLimit(uint32_t limit)
{
    getTwiMaster().setClockStretchLimit(limit);
}

size_t TwoWireBase::requestFrom(uint8_t address, size_t size, bool sendStop)
{
    if (size > rxBufferSize)
    {
        size = rxBufferSize;
    }
    size_t read = (getTwiMaster().readFrom(address, rxBuffer.get(), size, sendStop) == 0) ? size : 0;
    rxBufferIndex = 0;
    rxBufferLength = read;
    return read;
}

uint8_t TwoWireBase::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom(address, static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWireBase::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom(address, static_cast<size_t>(quantity), true);
}

uint8_t TwoWireBase::requestFrom(int address, int quantity)
{
    return requestFrom(static_cast<uint8_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWireBase::requestFrom(int address, int quantity, int sendStop)
{
    return requestFrom(static_cast<uint8_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

void TwoWireBase::beginTransmission(uint8_t address)
{
    transmitting = 1;
    txAddress = address;
    txBufferIndex = 0;
    txBufferLength = 0;
}

void TwoWireBase::beginTransmission(int address)
{
    beginTransmission((uint8_t)address);
}

uint8_t TwoWireBase::endTransmission(uint8_t sendStop)
{
    int8_t ret = getTwiMaster().writeTo(txAddress, txBuffer.get(), txBufferLength, sendStop);
    txBufferIndex = 0;
    txBufferLength = 0;
    transmitting = 0;
    return ret;
}

uint8_t TwoWireBase::endTransmission(void)
{
    return endTransmission(true);
}

size_t TwoWireBase::write(uint8_t data)
{
    if (transmitting)
    {
        if (txBufferLength >= txBufferSize)
        {
            setWriteError();
            return 0;
        }
        txBuffer[txBufferIndex] = data;
        ++txBufferIndex;
        txBufferLength = txBufferIndex;
    }
    else
    {
        setWriteError();
        return 0;
    }
    return 1;
}

size_t TwoWireBase::write(const uint8_t *data, size_t quantity)
{
    if (transmitting)
    {
        for (size_t i = 0; i < quantity; ++i)
        {
            if (!write(data[i]))
            {
                return i;
            }
        }
    }
    else
    {
        setWriteError();
        return 0;
    }
    return quantity;
}

int TwoWireBase::available(void)
{
    int result = rxBufferLength - rxBufferIndex;

    if (!result)
    {
        // yielding here will not make more data "available",
        // but it will prevent the system from going into WDT reset
        optimistic_yield(1000);
    }

    return result;
}

int TwoWireBase::read(void)
{
    int value = -1;
    if (rxBufferIndex < rxBufferLength)
    {
        value = rxBuffer[rxBufferIndex];
        ++rxBufferIndex;
    }
    return value;
}

int TwoWireBase::peek(void)
{
    int value = -1;
    if (rxBufferIndex < rxBufferLength)
    {
        value = rxBuffer[rxBufferIndex];
    }
    return value;
}

void TwoWireBase::flush(void)
{
    rxBufferIndex = 0;
    rxBufferLength = 0;
    txBufferIndex = 0;
    txBufferLength = 0;
}

// Master-only Constructors ////////////////////////////////////////////////////

TwoWireMaster::TwoWireMaster(uint8_t rxBufferSize, uint8_t txBufferSize)
    : TwoWireBase(rxBufferSize, txBufferSize)
{}

TwoWireMaster::TwoWireMaster(uint8_t rxBufferSize, uint8_t txBufferSize, uint8_t* rxBuffer, uint8_t* txBuffer)
    : TwoWireBase(new TwiMaster{}, rxBufferSize, txBufferSize, rxBuffer, txBuffer)
{}

// Master-or-Slave Constructors ////////////////////////////////////////////////

TwoWireMasterOrSlave::TwoWireMasterOrSlave(uint8_t rxBufferSize, uint8_t txBufferSize, uint8_t* rxBuffer, uint8_t* txBuffer)
    : TwoWireBase(&twiMasterSingleton, rxBufferSize, txBufferSize, rxBuffer, txBuffer)
{}

TwoWireMasterOrSlave::~TwoWireMasterOrSlave()
{
    releaseTwiMaster();
}

// Master-or-Slave Public Methods //////////////////////////////////////////////

void TwoWireMasterOrSlave::begin(int sda, int scl, uint8_t address)
{
    twi_setAddress(address);
    twi_attachSlaveTxEventWithTarget(onRequestService);
    twi_attachSlaveRxEventWithTarget(onReceiveService);
    begin(sda, scl);
}

void TwoWireMasterOrSlave::begin(uint8_t address)
{
    twi_setAddress(address);
    twi_attachSlaveTxEventWithTarget(onRequestService);
    twi_attachSlaveRxEventWithTarget(onReceiveService);
    begin();
}
void TwoWireMasterOrSlave::begin(int address)
{
    begin((uint8_t)address);
}

void TwoWireMasterOrSlave::onReceiveService(uint8_t* inBytes, size_t numBytes, void* targetObject)
{
    auto& instance = *(TwoWireMasterOrSlave*)targetObject;

    // return if targetObject (an instance of TwoWireMasterOrSlave) was not set/received correctly
    // don't bother if user hasn't registered a callback
    if (targetObject == nullptr || !instance.user_onReceive)
    {
        return;
    }
    // // don't bother if rx buffer is in use by a master requestFrom() op
    // // i know this drops data, but it allows for slight stupidity
    // // meaning, they may not have read all the master requestFrom() data yet
    // if(rxBufferIndex < rxBufferLength){
    //   return;
    // }

    // copy twi rx buffer into local read buffer
    // this enables new reads to happen in parallel
    for (uint8_t i = 0; i < numBytes; ++i)
    {
        instance.rxBuffer[i] = inBytes[i];
    }

    // set rx iterator vars
    instance.rxBufferIndex = 0;
    instance.rxBufferLength = numBytes;

    // alert user program
    instance.user_onReceive(numBytes);
}

void TwoWireMasterOrSlave::onRequestService(void* targetObject)
{
    auto& instance = *(TwoWireMasterOrSlave*)targetObject;

    // return if targetObject (an instance of TwoWireMasterOrSlave) was not set/received correctly
    // don't bother if user hasn't registered a callback
    if (targetObject == nullptr || !instance.user_onRequest)
    {
        return;
    }

    // reset tx buffer iterator vars
    // !!! this will kill any pending pre-master sendTo() activity
    instance.txBufferIndex = 0;
    instance.txBufferLength = 0;

    // alert user program
    instance.user_onRequest();
}

void TwoWireMasterOrSlave::onReceive(void (*function)(int))
{
    // arduino api compatibility fixer:
    // really hope size parameter will not exceed 2^31 :)
    static_assert(sizeof(int) == sizeof(size_t), "something is wrong in Arduino kingdom");
    user_onReceive = reinterpret_cast<void(*)(size_t)>(function);
}

void TwoWireMasterOrSlave::onReceive(void (*function)(size_t))
{
    user_onReceive = function;
    twi_enableSlaveModeWithTarget(this);
}

void TwoWireMasterOrSlave::onRequest(void (*function)(void))
{
    user_onRequest = function;
    twi_enableSlaveModeWithTarget(this);
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
static uint8_t _rxBuffer[I2C_BUFFER_LENGTH];
static uint8_t _txBuffer[I2C_BUFFER_LENGTH];

TwoWire Wire{I2C_BUFFER_LENGTH, I2C_BUFFER_LENGTH, _rxBuffer, _txBuffer};

TwoWireMasterOrSlave::TwoWireMasterOrSlave()
    : TwoWireBase(&twiMasterSingleton, I2C_BUFFER_LENGTH, I2C_BUFFER_LENGTH, _txBuffer, _rxBuffer)
{}

#else

TwoWireMasterOrSlave::TwoWireMasterOrSlave()
    : TwoWireBase(&twiMasterSingleton, I2C_BUFFER_LENGTH, I2C_BUFFER_LENGTH, new uint8_t[I2C_BUFFER_LENGTH], new uint8_t[I2C_BUFFER_LENGTH])
{}

#endif
