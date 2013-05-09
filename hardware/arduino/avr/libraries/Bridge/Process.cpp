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
  bridge.writeCommandInput(handle, &c, 1);
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
  buffered = bridge.readCommandOutput(handle, buffer, sizeof(buffer));
}

void Process::begin(String &command) {
  close();
  cmdline = new String(command);
}

void Process::begin(const char *command) {
  close();
  cmdline = new String(command);
}

void Process::addParameter(const char *param) {
  *cmdline += "\xFE";
  *cmdline += param;
}

void Process::addParameter(String &param) {
  *cmdline += "\xFE";
  *cmdline += param;
}

void Process::runAsynchronously() {
  handle = bridge.runCommand(*cmdline);
  delete cmdline;
  cmdline = NULL;
  
  started = true;
}

boolean Process::running() {
  return bridge.commandIsRunning(handle);
}

unsigned int Process::exitValue() {
  return bridge.commandExitValue(handle);
}

unsigned int Process::run() {
  runAsynchronously();
  while (running())
    delay(100);
  return exitValue();
}

void Process::close() {
  if (started)
    bridge.cleanCommand(handle);
  started = false;
}

