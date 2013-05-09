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

void BridgeClass::begin() {
  if (started)
    return;
  started = true;
  
  // TODO: A more robust restart
  // Bridge startup:
  // - If the bridge is not running starts it safely
  print(CTRL_C);
  print(F("\n"));
  delay(500);
  // Wait for OpenWRT message
  // "Press enter to activate console"
  print(F("\ncd /root\n"));
  delay(100);
  print(F("python bridge.py\n"));
  delay(1000);
  dropAll();
  
  // - If the bridge was already running previous commands
  //   are ignored as "invalid packets".
  
  // Reset the brigde
  uint8_t cmd[] = {'X','X'};
  transfer(cmd, 2);
}

uint8_t BridgeClass::runCommand(String &command) {
  // TODO: do it in a more efficient way
  String cmd = "R" + command;
  uint8_t res[1];
  transfer((uint8_t*)cmd.c_str(), cmd.length(), res, 1);
  return res[0];
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
                                    uint8_t *buff, unsigned int size) {
  // TODO: do it in a more efficient way
  uint8_t *tmp = new uint8_t[size+2];
  tmp[0] = 'I';
  tmp[1] = handle;
  memcpy(tmp+2, buff, size);
  transfer(tmp, size+2);
  delete[] tmp;
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

uint8_t BridgeClass::transfer(uint8_t *buff, uint8_t len, 
                              uint8_t *rxbuff, uint8_t rxlen) 
{
  for ( ; ; delay(100), dropAll() /* Delay for retransmission */) {
    // Send packet
    crcReset();
    write((char)0xFF);              // Start of packet (0xFF)
    crcUpdate(0xFF);
    write((char)index);             // Message index
    crcUpdate(index);
    write((char)len);               // Message length
    crcUpdate(len);
    for (uint8_t i=0; i<len; i++) { // Payload
      write((char)buff[i]);
      crcUpdate(buff[i]);
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
    uint8_t l = timedRead(5);
    if (l < 0)
      continue;
    crcUpdate(l);

    // Recv data
    for (uint8_t i=0; i<l; i++) {
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
