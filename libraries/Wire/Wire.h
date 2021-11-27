/*
    TwoWire.h - TWI/I2C library for Arduino & Wiring
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
*/

#ifndef TwoWire_h
#define TwoWire_h

/*
    If you need multiple TwoWire instances for multiple I2C master interfaces,
    define TWOWIRE_MASTER_ONLY (and optionally NO_GLOBAL_TWOWIRE). Then, you can
    freely instantiate the TwoWire class. If you do NOT define TWOWIRE_MASTER_ONLY,
    all the class called TwoWire will behave like singleton-like (existing behaviour)
    and all instances will share the same Master-or-Slave core TWI implementation 
     -> any call to begin(...) will change pins used for all instances.

    You can use different configuration in separate compilation units, e.g., use
    TWOWIRE_MASTER_ONLY in most of the code, and don't use it only in the
    compilation unit where you need I2C slave capabilities.
*/

#include <inttypes.h>
#include "Stream.h"
#include <memory>

// forward declaration, no need to load twi.h here!

class TwiMaster;

#ifndef I2C_BUFFER_LENGTH
#define I2C_BUFFER_LENGTH 128
#endif

class TwoWireBase : public Stream
{
private:
    std::unique_ptr<TwiMaster> twiMaster;
protected:
    // creates new TwiMaster in heap
    TwoWireBase(uint8_t rxBufferSize, uint8_t txBufferSize);
    // uses existing TwiMaster ptr (usefull for MasterOrSlave implementaion) and existing buffers
    TwoWireBase(TwiMaster* twiPtr, uint8_t rxBufferSize, uint8_t txBufferSize, uint8_t* rxBuffer, uint8_t* txBuffer);

    inline TwiMaster& getTwiMaster();
    // release the TwiMaster ptr from the unique_ptr, usefull when a global variable is used
    inline void releaseTwiMaster();

    // cashing of pins - needed for libraries that call begin() which might reset the non-default PIN settings
    uint8_t lastSdaPin = SDA;
    uint8_t lastSclPin = SCL;

    uint8_t rxBufferSize;
    std::unique_ptr<uint8_t[]> rxBuffer;
    uint8_t rxBufferIndex;
    uint8_t rxBufferLength;

    uint8_t txAddress;
    uint8_t txBufferSize;
    std::unique_ptr<uint8_t[]> txBuffer;
    uint8_t txBufferIndex;
    uint8_t txBufferLength;

    uint8_t transmitting;

public:
    void begin(int sda, int scl);
    void begin();
    void pins(int sda, int scl) __attribute__((deprecated)); // use begin(sda, scl) in new code
    void setClock(uint32_t);
    void setClockStretchLimit(uint32_t);
    void beginTransmission(uint8_t);
    void beginTransmission(int);
    uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t);
    size_t requestFrom(uint8_t address, size_t size, bool sendStop);
    uint8_t status();

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

    using Print::write;
};

// we could use typedefs the other way, and also do it the other way arround (using TwoWire = TwoWireMaster / TwoWireMasterOrSlave)
// but some libraries expecting TwoWire to be a class and do forward declarations of it (which are incompatible with typedefs)
// which breaks if TwoWire is only a typedef! So we need to create the following hack:
#ifdef TWOWIRE_MASTER_ONLY
#define TwoWireMaster TwoWire
#else
#define TwoWireMasterOrSlave TwoWire
#endif

// this is expected to be used with multiple instances for different pins
class TwoWireMaster : public TwoWireBase
{
public:
    TwoWireMaster(uint8_t rxBufferSize = I2C_BUFFER_LENGTH, uint8_t txBufferSize = I2C_BUFFER_LENGTH);
    // singleton global instance constructor
    TwoWireMaster(uint8_t rxBufferSize, uint8_t txBufferSize, uint8_t* rxBuffer, uint8_t* txBuffer);
};

// this is expected to be used a singleton, it uses the singleton instance of TwiMaster, twiMasterSingleton, from twi.h
class TwoWireMasterOrSlave : public TwoWireBase
{
private:
    void (*user_onRequest)(void);
    void (*user_onReceive)(size_t);

    static void onRequestService(void*);
    static void onReceiveService(uint8_t*, size_t, void*);

public:
    // singleton global instance constructor
    TwoWireMasterOrSlave();
    // singleton global instance constructor
    TwoWireMasterOrSlave(uint8_t rxBufferSize, uint8_t txBufferSize, uint8_t* rxBuffer, uint8_t* txBuffer);
    // releases the pointers to TwiMaster
    ~TwoWireMasterOrSlave();

    using TwoWireBase::begin;
    void begin(int sda, int scl, uint8_t address);
    void begin(uint8_t address);
    void begin(int address);

    void onReceive(void (*)(int));      // arduino api
    void onReceive(void (*)(size_t));   // legacy esp8266 backward compatibility
    void onRequest(void (*)(void));
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern TwoWire Wire;
#endif

#ifndef TwoWire_h_IMPLEMENTATION

#ifdef TWOWIRE_MASTER_ONLY
#undef TwoWireMaster
using TwoWireMaster = TwoWire;
#else
#undef TwoWireMasterOrSlave
using TwoWireMasterOrSlave = TwoWire;
#endif

#endif // TwoWire_h_IMPLEMENTATION

#endif // TwoWire_h

