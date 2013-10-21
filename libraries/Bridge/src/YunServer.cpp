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

#include <YunServer.h>
#include <YunClient.h>

YunServer::YunServer(uint16_t _p, BridgeClass &_b) :
  bridge(_b), port(_p), listening(false), useLocalhost(false) {
}

void YunServer::begin() {
  uint8_t tmp[] = {
    'N',
    (port >> 8) & 0xFF,
    port & 0xFF
  };
  uint8_t res[1];
  String address = F("127.0.0.1");
  if (!useLocalhost)
    address = F("0.0.0.0");
  bridge.transfer(tmp, 3, (const uint8_t *)address.c_str(), address.length(), res, 1);
  listening = (res[0] == 1);
}

YunClient YunServer::accept() {
  uint8_t cmd[] = {'k'};
  uint8_t res[1];
  unsigned int l = bridge.transfer(cmd, 1, res, 1);
  if (l == 0)
    return YunClient();
  return YunClient(res[0]);
}

size_t YunServer::write(uint8_t c) {
  uint8_t cmd[] = { 'b', c };
  bridge.transfer(cmd, 2);
  return 1;
}

