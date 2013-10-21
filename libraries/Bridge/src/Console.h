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

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <Bridge.h>

class ConsoleClass : public Stream {
  public:
    // Default constructor uses global Bridge instance
    ConsoleClass();
    // Constructor with a user provided BridgeClass instance
    ConsoleClass(BridgeClass &_b);
    ~ConsoleClass();

    void begin();
    void end();

    void buffer(uint8_t size);
    void noBuffer();

    bool connected();

    // Stream methods
    // (read from console socket)
    int available();
    int read();
    int peek();
    // (write to console socket)
    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
    void flush();

    operator bool () {
      return connected();
    }

  private:
    BridgeClass &bridge;

    void doBuffer();
    uint8_t inBuffered;
    uint8_t inReadPos;
    static const int BUFFER_SIZE = 32;
    uint8_t *inBuffer;

    bool autoFlush;
    uint8_t outBuffered;
    uint8_t outBufferSize;
    uint8_t *outBuffer;
};

extern ConsoleClass Console;

#endif
