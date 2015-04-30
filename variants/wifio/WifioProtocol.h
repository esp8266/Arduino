/* 
  WifioProtocol.h - virtual IO protocol for the WIFIO board

  Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
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
*/

#ifndef WIFIO_PROTOCOL_H
#define WIFIO_PROTOCOL_H

#include <stdint.h>
#include "Arduino.h"

inline void protocolError()
{
  delay(5);
  twi_stop();
}

namespace wifio {

const uint8_t MAGIC = 0x05;

enum Command {
  CMD_NONE,
  CMD_SYNC,
  CMD_PINMODE,
  CMD_DIGITALWRITE,
  CMD_DIGITALREAD,
  CMD_DIGITALREADRESULT,
  CMD_ANALOGWRITE,
  CMD_ANALOGREAD,
  CMD_ANALOGREADRESULT,
  CMD_INVALID,
};

struct CommandHeader {
  uint8_t       magic  : 4;
  uint8_t       cmd    : 4;
};

struct SyncCommand {
  CommandHeader header;
  uint8_t       val    : 7;
  uint8_t       parity : 1;
};

struct PinModeCommand {
  CommandHeader header;
  uint8_t       pin    : 5;
  uint8_t       mode   : 2;
  uint8_t       parity : 1;
};

struct DigitalWriteCommand {
  CommandHeader header;
  uint8_t       pin    : 5;
  uint8_t       value  : 1;
  uint8_t       unused : 1;
  uint8_t       parity : 1;
};

struct DigitalReadCommand {
  CommandHeader header;
  uint8_t       pin    : 5;
  uint8_t       unused : 2;
  uint8_t       parity : 1;
};

struct DigitalReadResultCommand {
  CommandHeader header;
  uint8_t       pin    : 5;
  uint8_t       value  : 1;
  uint8_t       unused : 1;
  uint8_t       parity : 1;
};

struct AnalogWriteCommand {
  CommandHeader header;
  uint8_t       value;
  uint8_t       pin    : 5;
  uint8_t       unused : 2;
  uint8_t       parity : 1;
};

struct AnalogReadCommand {
  CommandHeader header;
  uint8_t       pin    : 5;
  uint8_t       unused : 2;
  uint8_t       parity : 1;
};

struct AnalogReadResultCommand {
  CommandHeader header;
  uint8_t      pin    : 5;
  uint8_t      val_h  : 3;
  uint8_t      val_l  : 7;
  uint8_t      parity : 1;
};


template<typename T>
void sendCommand(uint8_t addr, T& t)
{
  // TODO: calculate parity
  t.parity = 1;
  if (twi_writeTo(addr, reinterpret_cast<uint8_t*>(&t), sizeof(T), true) != sizeof(T))
  {
    protocolError();
  }
}

template<typename TC, typename TR>
bool sendCommandWaitForReply(uint8_t addr, TC& c, Command rt, TR& r, int32_t d_us)
{
  c.parity = 1;
  if (twi_writeTo(addr, reinterpret_cast<uint8_t*>(&c), sizeof(TC), true) != sizeof(TC))
  {
    protocolError();
    return false;
  }
  if (d_us)
  {
    delayMicroseconds(d_us);
  }
  if (twi_readFrom(addr, reinterpret_cast<uint8_t*>(&r), sizeof(TR), true) != sizeof(TR) || r.header.cmd != rt)
  {
    protocolError();
    return false;
  }
  return true;
}


}// namespace wifio

#endif//WIFIO_PROTOCOL_H

