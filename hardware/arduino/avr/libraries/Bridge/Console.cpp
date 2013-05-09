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

#include <Console.h>

// Default constructor uses global Bridge instance
ConsoleClass::ConsoleClass() : 
  bridge(Bridge), buffered(0), readPos(0), buffer(NULL)
{
  // Empty
}

// Constructor with a user provided BridgeClass instance
ConsoleClass::ConsoleClass(BridgeClass &_b) : 
  bridge(_b), buffered(0), readPos(0), buffer(NULL),
  autoFlush(true)
{
  // Empty
}

ConsoleClass::~ConsoleClass() {
  end();
}

size_t ConsoleClass::write(uint8_t c) {
  if (autoFlush) {
    uint8_t tmp[] = { 'P', c };
    bridge.transfer(tmp, 2);
    return 1;
  } else {
    outBuffer[outBuffered++] = c;
    if (outBuffered == outBufferSize)
      flush();
  }
}

size_t ConsoleClass::write(const uint8_t *buffer, size_t size) {
  if (autoFlush) {
    // TODO: do it in a more efficient way
    uint8_t *tmp = new uint8_t[size+1];
    tmp[0] = 'P';
    memcpy(tmp+1, buffer, size);
    bridge.transfer(tmp, size+1);
    delete[] tmp;
    return size;
  } else {
    while (size > 0) {
      outBuffer[outBuffered++] = *buffer++;
      size--;
      if (outBuffered == outBufferSize)
        flush();
    }
  }
}

void ConsoleClass::flush() {
  if (autoFlush)
    return;
  
  bridge.transfer(outBuffer, outBuffered);
  outBuffered = 1;
}

void ConsoleClass::setBuffer(uint8_t size) {
  if (size==0) {
    if (!autoFlush) {
      delete[] outBuffer;
      autoFlush = true;
    }
  } else {
    if (autoFlush)
      setBuffer(0);
    outBuffer = new uint8_t[size+1];
    outBuffer[0] = 'P'; // WRITE tag
    outBufferSize = size+1;
    outBuffered = 1;
  }
}

bool ConsoleClass::connected() {
  uint8_t tmp = 'a';
  bridge.transfer(&tmp, 1, &tmp, 1);
  return tmp==1;
}

int ConsoleClass::available() {
  // Look if there is new data available
  doBuffer();
  return buffered;
}

int ConsoleClass::read() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else {
    buffered--;
    return buffer[readPos++];
  }
}

int ConsoleClass::peek() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else
    return buffer[readPos];
}

void ConsoleClass::doBuffer() {
  // If there are already char in buffer exit
  if (buffered > 0)
    return;

  // Try to buffer up to 32 characters
  readPos = 0;
  uint8_t tmp[] = { 'p', BUFFER_SIZE };
  buffered = bridge.transfer(tmp, 2, buffer, BUFFER_SIZE);
}

void ConsoleClass::begin() {
  bridge.begin();
  end();
  buffer = new uint8_t[BUFFER_SIZE];
}

void ConsoleClass::end() {
  if (autoFlush) {
    setBuffer(0);
  }
  if (buffer) {
    delete[] buffer;
    buffer = NULL;
  }
}

ConsoleClass Console;
