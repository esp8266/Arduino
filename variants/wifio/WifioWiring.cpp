/* 
  WifioWiring.cpp - Wiring API implementation for the WIFIO board

  Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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
#include "WifioProtocol.h"


namespace wifio {
    const int WIFIO_AVR_SLAVE_ADDR=0x23;
    
    void pinMode(int avrPin, int mode) {
        PinModeCommand cmd { MAGIC, CMD_PINMODE, static_cast<uint8_t>(avrPin), static_cast<uint8_t>(mode) };
        sendCommand(WIFIO_AVR_SLAVE_ADDR, cmd);
    }
    
    void digitalWrite(int avrPin, int value) {
        DigitalWriteCommand cmd { MAGIC, CMD_DIGITALWRITE, static_cast<uint8_t>(avrPin), static_cast<uint8_t>(value) };
        sendCommand(WIFIO_AVR_SLAVE_ADDR, cmd);
    }
    
    int digitalRead(int avrPin) {
        DigitalReadCommand cmd { MAGIC, CMD_DIGITALREAD, static_cast<uint8_t>(avrPin) };
        DigitalReadResultCommand reply = { 0 };
        sendCommandWaitForReply(WIFIO_AVR_SLAVE_ADDR, cmd, CMD_DIGITALREADRESULT, reply, 0);
        return reply.value;
    }
    
    void analogWrite(int avrPin, int value) {
        AnalogWriteCommand cmd { MAGIC, CMD_ANALOGWRITE, static_cast<uint8_t>(value), static_cast<uint8_t>(avrPin) };
        sendCommand(WIFIO_AVR_SLAVE_ADDR, cmd);
    }
    
    int analogRead(int avrPin) {
        AnalogReadCommand cmd { MAGIC, CMD_ANALOGREAD, static_cast<uint8_t>(avrPin) };
        AnalogReadResultCommand reply = { 0 };
        sendCommandWaitForReply(WIFIO_AVR_SLAVE_ADDR, cmd, CMD_ANALOGREADRESULT, reply, 120);
        return ((reply.val_h << 7) | reply.val_l);
    }
    
} // namespace wifio

extern "C" void __pinMode(uint8_t pin, uint8_t mode);
extern "C" void __digitalWrite(uint8_t pin, uint8_t val);
extern "C" int  __digitalRead(uint8_t pin);
extern "C" void __analogWrite(uint8_t pin, int val);
// extern "C" void __attachInterrupt(uint8_t pin, voidFuncPtr handler, int mode);
// extern "C" void __detachInterrupt(uint8_t pin);
extern "C" void __initPins();
extern "C" int  __analogRead(uint8_t pin);


extern "C" void pinMode(uint8_t pin, uint8_t mode){
  if (pin >= ESP_PINS_OFFSET) {    // esp pin
    __pinMode(pin - ESP_PINS_OFFSET, mode);
  }
  else {  // AVR pin
    wifio::pinMode(pin, mode);
  }
}

extern "C" void digitalWrite(uint8_t pin, uint8_t value) {
  if (pin >= ESP_PINS_OFFSET) {    // esp pin
    __digitalWrite(pin - ESP_PINS_OFFSET, value);
  }
  else {  // AVR pin
    wifio::digitalWrite(pin, value);
  }
}

extern "C" int digitalRead(uint8_t pin) {
  if (pin >= ESP_PINS_OFFSET) {
    return __digitalRead(pin - ESP_PINS_OFFSET);
  }
  else {
    return wifio::digitalRead(pin);
  }
}

extern "C" int analogRead(uint8_t pin) {
  if (pin >= ESP_PINS_OFFSET) {
    return __analogRead(pin - ESP_PINS_OFFSET);
  }
  else {
    return wifio::analogRead(pin);
  }
}

extern "C" void analogWrite(uint8_t pin, int value) {
  if (pin >= ESP_PINS_OFFSET) {
    __analogWrite(pin - ESP_PINS_OFFSET, value);
  }
  else {
    wifio::analogWrite(pin, value);
  }
}


void initVariant() { 
  twi_setClock(100000);
  twi_init(SDA, SCL);
}
