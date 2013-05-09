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
  BridgeClass(Stream &_stream) : index(0), stream(_stream), started(false) {
    // Empty
  }

  void begin();
  uint8_t runCommand(String &command);

  bool commandIsRunning(uint8_t handle);

  unsigned int commandExitValue(uint8_t handle);
  
  void cleanCommand(uint8_t handle);
  
  unsigned int commandOutputAvailable(uint8_t handle);
  unsigned int readCommandOutput(uint8_t handle, uint8_t *buff, unsigned int size);
  unsigned int readCommandOutput(uint8_t handle, char *buff, unsigned int size)
    { return readCommandOutput(handle, reinterpret_cast<uint8_t *>(buff), size); }

  void writeCommandInput(uint8_t handle, uint8_t *buff, unsigned int size);
  void writeCommandInput(uint8_t handle, char *buff, unsigned int size)
    { writeCommandInput(handle, reinterpret_cast<uint8_t *>(buff), size); }
  
  // Print methods
  size_t write(uint8_t c) { return stream.write(c); }
  size_t write(const uint8_t *buffer, size_t size)
    { return stream.write(buffer, size); }

  // Stream methods
  int available() { return stream.available(); }
  int read() { return stream.read(); }
  int peek() { return stream.peek(); }
  void flush() { stream.flush(); }

  uint8_t transfer(uint8_t *buff, uint8_t len, uint8_t *rxbuff=NULL, uint8_t rxlen=0);
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
  static const char CMD_RECV = 0x00;
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
    serial.begin(115200);
    BridgeClass::begin();
  }
  
private:
  HardwareSerial &serial;
};

extern SerialBridgeClass Bridge;

#endif /* BRIDGE_H_ */
