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

#include <Arduino.h>
#include <Stream.h>

class BridgeClass: public Stream {
public:
  BridgeClass(Stream &_stream);
  void begin();
  
  // Methods to handle processes on the linux side
  uint8_t runCommand(String &command);
  bool commandIsRunning(uint8_t handle);
  unsigned int commandExitValue(uint8_t handle);
  void cleanCommand(uint8_t handle);
  
  unsigned int commandOutputAvailable(uint8_t handle);
  unsigned int readCommandOutput(uint8_t handle, uint8_t *buff, unsigned int size);
  unsigned int readCommandOutput(uint8_t handle, char *buff, unsigned int size)
    { return readCommandOutput(handle, reinterpret_cast<uint8_t *>(buff), size); }

  void writeCommandInput(uint8_t handle, const uint8_t *buff, unsigned int size);
  void writeCommandInput(uint8_t handle, const char *buff, unsigned int size)
    { writeCommandInput(handle, reinterpret_cast<const uint8_t *>(buff), size); }
  
  // Methods to handle files
  uint8_t fileOpen(String &file, uint8_t mode, uint8_t &err);
  void fileClose(uint8_t handle);

  unsigned int fileRead(uint8_t handle, uint8_t *buff, unsigned int size, uint8_t &err);
  unsigned int fileRead(uint8_t handle, char *buff, unsigned int size, uint8_t &err)
    { return fileRead(handle, reinterpret_cast<uint8_t *>(buff), size, err); }

  void fileWrite(uint8_t handle, const uint8_t *buff, unsigned int size, uint8_t &err);
  void fileWrite(uint8_t handle, const char *buff, unsigned int size, uint8_t &err)
    { fileWrite(handle, reinterpret_cast<const uint8_t *>(buff), size, err); }

  void fileSeek(uint8_t handle, uint32_t position, uint8_t &err);

  // Methods to handle mailbox messages
  unsigned int readMessage(uint8_t *buffer, unsigned int size);
  void writeMessage(const uint8_t *buffer, unsigned int size);
  unsigned int messageAvailable();
  
  // Methods to handle key/value datastore
  void put(const char *key, const char *value);
  unsigned int get(const char *key, uint8_t *buff, unsigned int size);
  unsigned int get(const char *key, char *value, unsigned int maxlen)
    { get(key, reinterpret_cast<uint8_t *>(value), maxlen); }
  
  // Print methods (proxy to "stream" object) [CM: are these really needed?]
  size_t write(uint8_t c) { return stream.write(c); }
  size_t write(const uint8_t *buffer, size_t size)
    { return stream.write(buffer, size); }

  // Stream methods (proxy to "stream" object) [CM: are these really needed?]
  int available() { return stream.available(); }
  int read() { return stream.read(); }
  int peek() { return stream.peek(); }
  void flush() { stream.flush(); }

  // Trasnfer a frame (with error correction and response)
  uint8_t transfer(const uint8_t *buff1, uint16_t len1,
                   const uint8_t *buff2, uint16_t len2,
                   const uint8_t *buff3, uint16_t len3,
                   uint8_t *rxbuff, uint16_t rxlen);
  // multiple inline versions of the same function to allow efficient frame concatenation
  uint8_t transfer(const uint8_t *buff1, uint16_t len1)
  { transfer(buff1, len1, NULL, 0); }
  uint8_t transfer(const uint8_t *buff1, uint16_t len1,
                   uint8_t *rxbuff, uint16_t rxlen)
  { transfer(buff1, len1, NULL, 0, rxbuff, rxlen); }
  uint8_t transfer(const uint8_t *buff1, uint16_t len1,
                   const uint8_t *buff2, uint16_t len2,
                   uint8_t *rxbuff, uint16_t rxlen)
  { transfer(buff1, len1, buff2, len2, NULL, 0, rxbuff, rxlen); }
private:
  uint8_t index;
  int timedRead(unsigned int timeout);
  void dropAll();
  
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
};

// This subclass uses a serial port Stream
class SerialBridgeClass : public BridgeClass {
public:
  SerialBridgeClass(HardwareSerial &_serial)
      : BridgeClass(_serial), serial(_serial) { 
    // Empty
  }
  
  void begin() {
    serial.begin(57600);
    BridgeClass::begin();
  }
  
private:
  HardwareSerial &serial;
};

extern SerialBridgeClass Bridge;

#endif /* BRIDGE_H_ */

#include <Console.h>
#include <Process.h>
