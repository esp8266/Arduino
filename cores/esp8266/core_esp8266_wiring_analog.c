/* 
  analog.c - analogRead implementation for esp8266

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
#include "wiring_private.h"
#include "pins_arduino.h"

void analogReference(uint8_t mode) {}

extern int __analogRead(uint8_t pin) {
  if(pin == 17){
    //return system_adc_read();
    uint8_t i;
    uint16_t data[8];

    rom_i2c_writeReg_Mask(0x6C,2,0,5,5,1);

    ESP8266_REG(0xD5C) |= (1 << 21);
    while ((ESP8266_REG(0xD50) & (7 << 24)) > 0);
    ESP8266_REG(0xD50) &= ~(1 << 1);
    ESP8266_REG(0xD50) |= (1 << 1);
    delayMicroseconds(2);
    while ((ESP8266_REG(0xD50) & (7 << 24)) > 0);

    read_sar_dout(data);
    rom_i2c_writeReg_Mask(0x6C,2,0,5,5,1);

    while ((ESP8266_REG(0xD50) & (7 << 24)) > 0);
    ESP8266_REG(0xD5C) &= ~(1 << 21);
    ESP8266_REG(0xD60) |= (1 << 0);
    ESP8266_REG(0xD60) &= ~(1 << 0);

    uint16_t tout = 0;
    for (i = 0; i < 8; i++) tout += data[i];
    return tout >> 4;//tout is 10 bits fraction
  }
  return digitalRead(pin) * 1023;
}

extern int analogRead(uint8_t pin) __attribute__ ((weak, alias("__analogRead")));
