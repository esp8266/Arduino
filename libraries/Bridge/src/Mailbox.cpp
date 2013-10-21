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

#include <Mailbox.h>

unsigned int MailboxClass::readMessage(uint8_t *buff, unsigned int size) {
  uint8_t tmp[] = { 'm' };
  return bridge.transfer(tmp, 1, buff, size);
}

void MailboxClass::readMessage(String &str, unsigned int maxLength) {
  uint8_t tmp[] = { 'm' };
  // XXX: Is there a better way to create the string?
  uint8_t buff[maxLength + 1];
  int l = bridge.transfer(tmp, 1, buff, maxLength);
  buff[l] = 0;
  str = (const char *)buff;
}

void MailboxClass::writeMessage(const uint8_t *buff, unsigned int size) {
  uint8_t cmd[] = {'M'};
  bridge.transfer(cmd, 1, buff, size, NULL, 0);
}

void MailboxClass::writeMessage(const String& str) {
  writeMessage((uint8_t*) str.c_str(), str.length());
}

void MailboxClass::writeJSON(const String& str) {
  uint8_t cmd[] = {'J'};
  bridge.transfer(cmd, 1, (uint8_t*) str.c_str(), str.length(), NULL, 0);
}

unsigned int MailboxClass::messageAvailable() {
  uint8_t tmp[] = {'n'};
  uint8_t res[2];
  bridge.transfer(tmp, 1, res, 2);
  return (res[0] << 8) + res[1];
}

MailboxClass Mailbox(Bridge);
