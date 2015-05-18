/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef BRIDGE_H_
#define BRIDGE_H_

#ifndef BRIDGE_BAUDRATE
#define BRIDGE_BAUDRATE 250000
#endif

#include <Arduino.h>
#include <Stream.h>

class BridgeClass {
  public:
    BridgeClass(Stream &_stream);
    void begin();

    // Methods to handle key/value datastore
    void put(const char *key, const char *value);
    void put(const String &key, const String &value)
    {
      put(key.c_str(), value.c_str());
    }
    unsigned int get(const char *key, uint8_t *buff, unsigned int size);
    unsigned int get(const char *key, char *value, unsigned int maxlen)
    {
      return get(key, reinterpret_cast<uint8_t *>(value), maxlen);
    }

    // Trasnfer a frame (with error correction and response)
    uint16_t transfer(const uint8_t *buff1, uint16_t len1,
                      const uint8_t *buff2, uint16_t len2,
                      const uint8_t *buff3, uint16_t len3,
                      uint8_t *rxbuff, uint16_t rxlen);
    // multiple inline versions of the same function to allow efficient frame concatenation
    uint16_t transfer(const uint8_t *buff1, uint16_t len1)
    {
      return transfer(buff1, len1, NULL, 0);
    }
    uint16_t transfer(const uint8_t *buff1, uint16_t len1,
                      uint8_t *rxbuff, uint16_t rxlen)
    {
      return transfer(buff1, len1, NULL, 0, rxbuff, rxlen);
    }
    uint16_t transfer(const uint8_t *buff1, uint16_t len1,
                      const uint8_t *buff2, uint16_t len2,
                      uint8_t *rxbuff, uint16_t rxlen)
    {
      return transfer(buff1, len1, buff2, len2, NULL, 0, rxbuff, rxlen);
    }

    uint16_t getBridgeVersion()
    {
      return bridgeVersion;
    }

    static const int TRANSFER_TIMEOUT = 0xFFFF;

  private:
    uint8_t index;
    int timedRead(unsigned int timeout);
    void dropAll();
    uint16_t bridgeVersion;

  private:
    void crcUpdate(uint8_t c);
    void crcReset();
    void crcWrite();
    bool crcCheck(uint16_t _CRC);
    uint16_t CRC;

  private:
    static const char CTRL_C = 3;
    Stream &stream;
    bool started;
    uint8_t max_retries;
};

// This subclass uses a serial port Stream
class SerialBridgeClass : public BridgeClass {
  public:
    SerialBridgeClass(HardwareSerial &_serial)
      : BridgeClass(_serial), serial(_serial) {
      // Empty
    }

    void begin(unsigned long baudrate = BRIDGE_BAUDRATE) {
      serial.begin(baudrate);
      BridgeClass::begin();
    }

  private:
    HardwareSerial &serial;
};

extern SerialBridgeClass Bridge;

#endif /* BRIDGE_H_ */

#include <Console.h>
#include <Process.h>
