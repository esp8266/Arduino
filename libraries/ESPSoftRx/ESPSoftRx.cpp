/* 
  ESPSoftRx - Timer1 based Software Serial RX for ESP8266

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
#include "Arduino.h"
#include "ESPSoftRx.h"
#include <cbuf.h>

extern "C" uint32_t interrupt_reg;

static cbuf _srx_rx_buf(SS_BUFFER_SIZE);
static uint8_t _srx_rx_pin;
static uint8_t _srx_rx_value;
static uint8_t _srx_rx_index;
static uint32_t _srx_timer_period;
static uint32_t _srx_start_period;

void ICACHE_RAM_ATTR _srx_on_start(){
  if(_srx_start_period)
    delayMicroseconds(_srx_start_period);
  T1C = (1 << TCTE) | (1 << TCPD) | (1 << TCAR);
  T1I = 0;
  T1L = _srx_timer_period;
  TEIE |= TEIE1;
  _srx_rx_value = 0;
  _srx_rx_index = 0;
  interrupt_reg &= ~(1 << _srx_rx_pin);
}

void ICACHE_RAM_ATTR _srx_read_bit(){
  if(_srx_rx_index < 8){
    _srx_rx_value |= GPIP(_srx_rx_pin) << _srx_rx_index++;
  } else if(_srx_rx_index == 8){
    _srx_rx_buf.write(_srx_rx_value);
    _srx_rx_value = 0;
    _srx_rx_index = 0;
    T1C = 0;
    T1I = 0;
    interrupt_reg |= (1 << _srx_rx_pin);
  }
}

void ESPSoftRxClass::begin(uint32_t bitrate, uint8_t rx_pin){
  _srx_rx_pin = rx_pin;
  attachInterrupt(_srx_rx_pin, _srx_on_start, FALLING);
  _srx_rx_buf.flush();
  _srx_timer_period = 5000000 / bitrate;
  _srx_start_period = 0;
  if(_srx_timer_period >= 30)
    _srx_start_period = (_srx_timer_period / 10) - 4;
  T1C = 0;
  T1I = 0;
  ETS_FRC1_INTR_DISABLE();
  ETS_FRC_TIMER1_INTR_ATTACH(_srx_read_bit, NULL);
  ETS_FRC1_INTR_ENABLE();
  GPF(_srx_rx_pin) = GPFFS(GPFFS_GPIO(_srx_rx_pin)) | (1 << GPFPU);
  GPEC = (1 << _srx_rx_pin);
  GPC(_srx_rx_pin) = (GPC(_srx_rx_pin) & (0xF << GPCI)) | (1 << GPCD);
  interrupt_reg |= (1 << _srx_rx_pin);
}
int ESPSoftRxClass::available(){
  return _srx_rx_buf.getSize();
}
int ESPSoftRxClass::peek(){
  return _srx_rx_buf.peek();
}
int ESPSoftRxClass::read(){
  return _srx_rx_buf.read();
}
size_t ESPSoftRxClass::read(char* dst, size_t len){
  return _srx_rx_buf.read(dst, len);
}
void ESPSoftRxClass::flush(){
  return _srx_rx_buf.flush();
}

ESPSoftRxClass SerialRx;
