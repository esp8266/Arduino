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

#include <YunClient.h>

YunClient::YunClient(int _h, BridgeClass &_b) :
  bridge(_b), handle(_h), opened(true), buffered(0) {
}

YunClient::YunClient(BridgeClass &_b) :
  bridge(_b), handle(0), opened(false), buffered(0) {
}

YunClient::~YunClient() {
}

YunClient& YunClient::operator=(const YunClient &_x) {
  opened = _x.opened;
  handle = _x.handle;
  return *this;
}

void YunClient::stop() {
  if (opened) {
    uint8_t cmd[] = {'j', handle};
    bridge.transfer(cmd, 2);
  }
  opened = false;
}

void YunClient::doBuffer() {
  // If there are already char in buffer exit
  if (buffered > 0)
    return;

  // Try to buffer up to 32 characters
  readPos = 0;
  uint8_t cmd[] = {'K', handle, sizeof(buffer)};
  buffered = bridge.transfer(cmd, 3, buffer, sizeof(buffer));
}

int YunClient::available() {
  // Look if there is new data available
  doBuffer();
  return buffered;
}

int YunClient::read() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else {
    buffered--;
    return buffer[readPos++];
  }
}

int YunClient::read(uint8_t *buff, size_t size) {
  int readed = 0;
  do {
    if (buffered == 0) {
      doBuffer();
      if (buffered == 0)
        return readed;
    }
    buff[readed++] = buffer[readPos++];
    buffered--;
  } while (readed < size);
  return readed;
}

int YunClient::peek() {
  doBuffer();
  if (buffered == 0)
    return -1; // no chars available
  else
    return buffer[readPos];
}

size_t YunClient::write(uint8_t c) {
  if (!opened)
    return 0;
  uint8_t cmd[] = {'l', handle, c};
  bridge.transfer(cmd, 3);
  return 1;
}

size_t YunClient::write(const uint8_t *buf, size_t size) {
  if (!opened)
    return 0;
  uint8_t cmd[] = {'l', handle};
  bridge.transfer(cmd, 2, buf, size, NULL, 0);
  return size;
}

void YunClient::flush() {
}

uint8_t YunClient::connected() {
  if (!opened)
    return false;
  uint8_t cmd[] = {'L', handle};
  uint8_t res[1];
  bridge.transfer(cmd, 2, res, 1);
  return (res[0] == 1);
}

int YunClient::connect(IPAddress ip, uint16_t port) {
  String address;
  address.reserve(18);
  address += ip[0];
  address += '.';
  address += ip[1];
  address += '.';
  address += ip[2];
  address += '.';
  address += ip[3];
  return connect(address.c_str(), port);
}

int YunClient::connect(const char *host, uint16_t port) {
  uint8_t tmp[] = {
    'C',
    (port >> 8) & 0xFF,
    port & 0xFF
  };
  uint8_t res[1];
  int l = bridge.transfer(tmp, 3, (const uint8_t *)host, strlen(host), res, 1);
  if (l == 0)
    return 0;
  handle = res[0];

  // wait for connection
  uint8_t tmp2[] = { 'c', handle };
  uint8_t res2[1];
  while (true) {
    bridge.transfer(tmp2, 2, res2, 1);
    if (res2[0] == 0)
      break;
    delay(1);
  }
  opened = true;

  // check for successful connection
  if (connected())
    return 1;

  stop();
  handle = 0;
  return 0;
}

