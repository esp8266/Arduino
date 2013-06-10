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

#include "Bridge.h"

BridgeClass::BridgeClass(Stream &_stream) : index(0), stream(_stream), started(false) {
  // Empty
}

void BridgeClass::begin() {
  if (started)
    return;
  started = true;
  
  // TODO: A more robust restart
  
  // Wait for Atheros bootloader to finish startup
  do {
    dropAll();
    delay(1100);
  } while (available()>0);
  
  // Bridge startup:
  // - If the bridge is not running starts it safely
  print(CTRL_C);
  delay(250);
  print(F("\n"));
  delay(500);
  print(F("\n"));
  delay(750);
  // Wait for OpenWRT message
  // "Press enter to activate console"
  print(F("run-bridge\n"));
  delay(500);
  dropAll();
  
  // - If the bridge was already running previous commands
  //   are ignored as "invalid packets".
  
  // Reset the brigde
  uint8_t cmd[] = {'X','X'};
  transfer(cmd, 2);
}

uint8_t BridgeClass::runCommand(String &command, uint8_t &err) {
  uint8_t cmd[] = {'R'};
  uint8_t res[2];
  transfer(cmd, 1, (uint8_t*)command.c_str(), command.length(), res, 2);
  err = res[0];
  return res[1];
}

bool BridgeClass::commandIsRunning(uint8_t handle) {
  uint8_t cmd[] = {'r', handle};
  uint8_t res[1];
  transfer(cmd, 2, res, 1);
  return (res[0] == 1);
}

unsigned int BridgeClass::commandExitValue(uint8_t handle) {
  uint8_t cmd[] = {'W', handle};
  uint8_t res[2];
  transfer(cmd, 2, res, 2);
  return (res[0] << 8) + res[1];
}

void BridgeClass::cleanCommand(uint8_t handle) {
  uint8_t cmd[] = {'w', handle};
  transfer(cmd, 2);
}

unsigned int BridgeClass::commandOutputAvailable(uint8_t handle) {
  uint8_t cmd[] = {'o', handle};
  uint8_t res[1];
  transfer(cmd, 2, res, 1);
  return res[0];
}

unsigned int BridgeClass::readCommandOutput(uint8_t handle, 
                                            uint8_t *buffer, unsigned int size) {
  if (size > 255)
    size = 255;
  uint8_t cmd[] = {'O', handle, size};
  return transfer(cmd, 3, buffer, size);
}

void BridgeClass::writeCommandInput(uint8_t handle, 
                                    const uint8_t *buff, unsigned int size) {
  uint8_t cmd[] = {'I', handle};
  transfer(cmd, 2, buff, size, NULL, 0);
}

unsigned int BridgeClass::readMessage(uint8_t *buff, unsigned int size) {
  uint8_t tmp[] = { 'm' };
  return transfer(tmp, 1, buff, size);
}

void BridgeClass::writeMessage(const uint8_t *buff, unsigned int size) {
  uint8_t cmd[] = {'M'};
  transfer(cmd, 1, buff, size, NULL, 0);
}

unsigned int BridgeClass::messageAvailable() {
  uint8_t tmp[] = {'n'};
  uint8_t res[2];
  transfer(tmp, 1, res, 2);
  return (res[0] << 8) + res[1];
}

void BridgeClass::put(const char *key, const char *value) {
  // TODO: do it in a more efficient way
  String cmd = "D";
  cmd += key;
  cmd += "\xFE";
  cmd += value;
  transfer((uint8_t*)cmd.c_str(), cmd.length());
}

unsigned int BridgeClass::get(const char *key, uint8_t *value, unsigned int maxlen) {
  uint8_t cmd[] = {'d'};
  unsigned int l = transfer(cmd, 1, (uint8_t *)key, strlen(key), value, maxlen);
  if (l < maxlen)
    value[l] = 0; // Zero-terminate string
  return l;
}

void BridgeClass::crcUpdate(uint8_t c) {
  CRC = CRC ^ c;
  CRC = (CRC >> 8) + (CRC << 8);
}

void BridgeClass::crcReset() {
  CRC = 0xAAAA;
}

void BridgeClass::crcWrite() {
  write((char)(CRC >> 8));
  write((char)(CRC & 0xFF));
}

bool BridgeClass::crcCheck(uint16_t _CRC) {
  return CRC == _CRC;
}

uint16_t BridgeClass::transfer(const uint8_t *buff1, uint16_t len1,
                 const uint8_t *buff2, uint16_t len2,
                 const uint8_t *buff3, uint16_t len3,
                 uint8_t *rxbuff, uint16_t rxlen)
{
  uint16_t len = len1 + len2 + len3;
  for ( ; ; delay(100), dropAll() /* Delay for retransmission */) {
    // Send packet
    crcReset();
    write((char)0xFF);                // Start of packet (0xFF)
    crcUpdate(0xFF);
    write((char)index);               // Message index
    crcUpdate(index);
    write((char)((len >> 8) & 0xFF)); // Message length (hi)
    crcUpdate((len >> 8) & 0xFF);
    write((char)(len & 0xFF));        // Message length (lo)
    crcUpdate(len & 0xFF);
    for (uint16_t i=0; i<len1; i++) {  // Payload
      write((char)buff1[i]);
      crcUpdate(buff1[i]);
    }
    for (uint16_t i=0; i<len2; i++) {  // Payload
      write((char)buff2[i]);
      crcUpdate(buff2[i]);
    }
    for (uint16_t i=0; i<len3; i++) {  // Payload
      write((char)buff3[i]);
      crcUpdate(buff3[i]);
    }
    crcWrite();                     // CRC
  
    // Wait for ACK in 100ms
    if (timedRead(100) != 0xFF)
      continue;
    crcReset();
    crcUpdate(0xFF);
    
    // Check packet index
    if (timedRead(5) != index)
      continue;
    crcUpdate(index);
    
    // Recv len
    int lh = timedRead(5);
    if (lh < 0)
      continue;
    crcUpdate(lh);
    int ll = timedRead(5);
    if (ll < 0)
      continue;
    crcUpdate(ll);
    uint16_t l = lh;
    l <<= 8;
    l += ll;

    // Recv data
    for (uint16_t i=0; i<l; i++) {
      int c = timedRead(5);
      if (c < 0)
        continue;
      // Cut received data if rxbuffer is too small
      if (i < rxlen)
        rxbuff[i] = c;
      crcUpdate(c);
    }
    
    // Check CRC
    int crc_hi = timedRead(5);
    if (crc_hi < 0)
      continue;
    int crc_lo = timedRead(5);
    if (crc_lo < 0)
      continue;
    if (!crcCheck((crc_hi<<8)+crc_lo))
      continue;
    
    // Increase index
    index++;
    
    // Return bytes received
    if (l > rxlen)
      return rxlen;
    return l;
  }
}

int BridgeClass::timedRead(unsigned int timeout) {
  int c;
  unsigned long _startMillis = millis();
  do {
    c = read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < timeout);
  return -1;     // -1 indicates timeout
}

void BridgeClass::dropAll() {
  while (available() > 0) {
    read();
  }
}

// Bridge instance
#ifdef __AVR_ATmega32U4__
  // Leonardo variants (where HardwareSerial is Serial1)
  SerialBridgeClass Bridge(Serial1);
#else
  SerialBridgeClass Bridge(Serial);
#endif
