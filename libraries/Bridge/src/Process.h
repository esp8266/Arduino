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

#ifndef PROCESS_H_
#define PROCESS_H_

#include <Bridge.h>

class Process : public Stream {
  public:
    // Constructor with a user provided BridgeClass instance
    Process(BridgeClass &_b = Bridge) :
      bridge(_b), started(false), buffered(0), readPos(0) { }
    ~Process();

    void begin(const String &command);
    void addParameter(const String &param);
    unsigned int run();
    void runAsynchronously();
    boolean running();
    unsigned int exitValue();
    void close();

    unsigned int runShellCommand(const String &command);
    void runShellCommandAsynchronously(const String &command);

    operator bool () {
      return started;
    }

    // Stream methods
    // (read from process stdout)
    int available();
    int read();
    int peek();
    // (write to process stdin)
    size_t write(uint8_t);
    void flush();
    // TODO: add optimized function for block write

  private:
    BridgeClass &bridge;
    unsigned int handle;
    String *cmdline;
    boolean started;

  private:
    void doBuffer();
    uint8_t buffered;
    uint8_t readPos;
    static const int BUFFER_SIZE = 64;
    uint8_t buffer[BUFFER_SIZE];

};

#endif
