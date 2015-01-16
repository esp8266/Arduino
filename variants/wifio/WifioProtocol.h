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
  Serial.flush();
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
void sendCommand(Stream& stream, T& t)
{
  // TODO: calculate parity
  t.parity = 1;
  stream.write(reinterpret_cast<const uint8_t*>(&t), sizeof(T));
}

template<typename T>
bool expectCommand(Stream& stream, enum Command cmd, T& t)
{
  size_t nIt = 0;
  t = { 0 };
  while (stream.available() < sizeof(T))
  {
    if (++nIt == 100) {
      return false;
    }
    delayMicroseconds(10);
  }
  uint8_t* p = reinterpret_cast<uint8_t*>(&t);
  for (size_t i = 0; i < sizeof(T); ++i)
    p[i] = stream.read();
  // TODO : check parity
  if (static_cast<uint8_t>(cmd) != t.header.cmd) {
    protocolError();
    return false;
  }
  return true;
}

inline Command peekCommand(Stream& stream)
{
  if (!stream.available())
    return CMD_NONE;

  uint8_t b = stream.peek();
  CommandHeader* h = reinterpret_cast<CommandHeader*>(&b);
  if (h->magic != MAGIC)
    return CMD_INVALID;
  return static_cast<Command>(h->cmd);
}

}// namespace wifio

#endif//WIFIO_PROTOCOL_H

