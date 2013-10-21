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
  bridge(Bridge), inBuffered(0), inReadPos(0), inBuffer(NULL),
  autoFlush(true)
{
  // Empty
}

// Constructor with a user provided BridgeClass instance
ConsoleClass::ConsoleClass(BridgeClass &_b) :
  bridge(_b), inBuffered(0), inReadPos(0), inBuffer(NULL),
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

size_t ConsoleClass::write(const uint8_t *buff, size_t size) {
  if (autoFlush) {
    // TODO: do it in a more efficient way
    uint8_t *tmp = new uint8_t[size + 1];
    tmp[0] = 'P';
    memcpy(tmp + 1, buff, size);
    bridge.transfer(tmp, size + 1);
    delete[] tmp;
    return size;
  } else {
    while (size > 0) {
      outBuffer[outBuffered++] = *buff++;
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

void ConsoleClass::noBuffer() {
  if (autoFlush)
    return;
  delete[] outBuffer;
  autoFlush = true;
}

void ConsoleClass::buffer(uint8_t size) {
  noBuffer();
  if (size == 0)
    return;
  outBuffer = new uint8_t[size + 1];
  outBuffer[0] = 'P'; // WRITE tag
  outBufferSize = size + 1;
  outBuffered = 1;
  autoFlush = false;
}

bool ConsoleClass::connected() {
  uint8_t tmp = 'a';
  bridge.transfer(&tmp, 1, &tmp, 1);
  return tmp == 1;
}

int ConsoleClass::available() {
  // Look if there is new data available
  doBuffer();
  return inBuffered;
}

int ConsoleClass::read() {
  doBuffer();
  if (inBuffered == 0)
    return -1; // no chars available
  else {
    inBuffered--;
    return inBuffer[inReadPos++];
  }
}

int ConsoleClass::peek() {
  doBuffer();
  if (inBuffered == 0)
    return -1; // no chars available
  else
    return inBuffer[inReadPos];
}

void ConsoleClass::doBuffer() {
  // If there are already char in buffer exit
  if (inBuffered > 0)
    return;

  // Try to buffer up to 32 characters
  inReadPos = 0;
  uint8_t tmp[] = { 'p', BUFFER_SIZE };
  inBuffered = bridge.transfer(tmp, 2, inBuffer, BUFFER_SIZE);
}

void ConsoleClass::begin() {
  bridge.begin();
  end();
  inBuffer = new uint8_t[BUFFER_SIZE];
}

void ConsoleClass::end() {
  noBuffer();
  if (inBuffer) {
    delete[] inBuffer;
    inBuffer = NULL;
  }
}

ConsoleClass Console;
