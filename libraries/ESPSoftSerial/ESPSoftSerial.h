/* 
  ESPSoftRx - Timer1 based Software Serial for ESP8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _ESPSoftSerial_h_
#define _ESPSoftSerial_h_

class ESPSoftSerialClass : public Stream {
  public:
    ESPSoftSerialClass(){}
    ~ESPSoftSerialClass(){}
    void begin(uint32_t bitrate, uint8_t rx_pin, uint8_t tx_pin);
    int available(void) override ;
    int peek(void) override ;
    int read(void) override ;
    size_t read(char* dst, size_t len);
    void flush(void) override ;
    size_t write(uint8_t * data, size_t len);
    size_t write(uint8_t data) override ;
};

extern ESPSoftSerialClass SoftSerial;

#endif
