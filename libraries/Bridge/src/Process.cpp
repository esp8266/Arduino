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

#include <Process.h>

Process::~Process() {
  close();
}

size_t Process::write(uint8_t c) {
  uint8_t cmd[] = {'I', handle, c};
  bridge.transfer(cmd, 3);
  return 1;
}

void Process::flush() {
}

int Process::available() {
  // Look if there is new data available
  doBuffer();
  return buffered;
}

int Process::read() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else {
    buffered--;
    return buffer[readPos++];
  }
}

int Process::peek() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else
    return buffer[readPos];
}

void Process::doBuffer() {
  // If there are already char in buffer exit
  if (buffered > 0)
    return;

  // Try to buffer up to 32 characters
  readPos = 0;
  uint8_t cmd[] = {'O', handle, sizeof(buffer)};
  buffered = bridge.transfer(cmd, 3, buffer, sizeof(buffer));
}

void Process::begin(const String &command) {
  close();
  cmdline = new String(command);
}

void Process::addParameter(const String &param) {
  *cmdline += "\xFE";
  *cmdline += param;
}

void Process::runAsynchronously() {
  uint8_t cmd[] = {'R'};
  uint8_t res[2];
  bridge.transfer(cmd, 1, (uint8_t*)cmdline->c_str(), cmdline->length(), res, 2);
  handle = res[1];

  delete cmdline;
  cmdline = NULL;

  if (res[0] == 0) // res[0] contains error code
    started = true;
}

boolean Process::running() {
  uint8_t cmd[] = {'r', handle};
  uint8_t res[1];
  bridge.transfer(cmd, 2, res, 1);
  return (res[0] == 1);
}

unsigned int Process::exitValue() {
  uint8_t cmd[] = {'W', handle};
  uint8_t res[2];
  bridge.transfer(cmd, 2, res, 2);
  return (res[0] << 8) + res[1];
}

unsigned int Process::run() {
  runAsynchronously();
  while (running())
    delay(100);
  return exitValue();
}

void Process::close() {
  if (started) {
    uint8_t cmd[] = {'w', handle};
    bridge.transfer(cmd, 2);
  }
  started = false;
}

unsigned int Process::runShellCommand(const String &command) {
  runShellCommandAsynchronously(command);
  while (running())
    delay(100);
  return exitValue();
}

void Process::runShellCommandAsynchronously(const String &command) {
  begin("/bin/ash");
  addParameter("-c");
  addParameter(command);
  runAsynchronously();
}

// This method is currently unused
//static unsigned int __commandOutputAvailable(uint8_t handle) {
//  uint8_t cmd[] = {'o', handle};
//  uint8_t res[1];
//  Bridge.transfer(cmd, 2, res, 1);
//  return res[0];
//}

