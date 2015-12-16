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
#include "Arduino.h"
#include "ESPSoftSerial.h"
#include <cbuf.h>

#define SS_BUFFER_SIZE 256

extern "C" uint32_t interrupt_reg;

enum { IDLE=0, RX, TX };
static uint8_t _ss_phase;
static uint8_t _ss_rx_pin;
static uint8_t _ss_tx_pin;

static cbuf _ss_rx_buf(256);
static uint8_t _ss_rx_value;
static uint8_t _ss_rx_index;
static uint32_t _ss_timer_period;
static uint32_t _ss_start_period;

static cbuf _ss_tx_buf(256);
static uint8_t _ss_tx_value = 0;
static uint8_t _ss_tx_index = 0;


void _ss_mode_tx();
void _ss_mode_rx();

// TX START
void ICACHE_RAM_ATTR _ss_write_bit(){
  if(_ss_phase != TX)
    return;
  if(!_ss_tx_index){
    GPOC = (1 << _ss_tx_pin); //Start Bit
    _ss_tx_value = _ss_tx_buf.read();
    _ss_tx_index++;
  } else if(_ss_tx_index < 9){
    if(_ss_tx_value & (1 << (_ss_tx_index++ - 1)))
      GPOS = (1 << _ss_tx_pin);
    else
      GPOC = (1 << _ss_tx_pin);
  } else {
    GPOS = (1 << _ss_tx_pin); //Stop Bit
    _ss_tx_index = 0;
    if(!_ss_tx_buf.getSize())
      _ss_mode_rx();
  }
}

void ICACHE_RAM_ATTR _ss_mode_tx(){
  if(!_ss_tx_buf.getSize())
      return _ss_mode_rx();
  interrupt_reg &= ~(1 << _ss_rx_pin);
  GPF(_ss_tx_pin) = GPFFS(GPFFS_GPIO(_ss_tx_pin));
  GPC(_ss_tx_pin) = (GPC(_ss_tx_pin) & (0xF << GPCI));
  GPES = (1 << _ss_tx_pin);
  GPOS = (1 << _ss_tx_pin);
  
  ETS_FRC1_INTR_DISABLE();
  ETS_FRC_TIMER1_INTR_ATTACH(_ss_write_bit, NULL);
  T1C = (1 << TCTE) | (1 << TCPD) | (1 << TCAR);
  T1I = 0;
  T1L = _ss_timer_period;
  ETS_FRC1_INTR_ENABLE();
  _ss_tx_index = 0;
  TEIE |= TEIE1;
  _ss_phase = TX;
} 
// TX END

// RX START
void ICACHE_RAM_ATTR _ss_on_start(){
  if(_ss_start_period)
    delayMicroseconds(_ss_start_period);
  T1C = (1 << TCTE) | (1 << TCPD) | (1 << TCAR);
  T1I = 0;
  T1L = _ss_timer_period;
  TEIE |= TEIE1;
  _ss_rx_value = 0;
  _ss_rx_index = 0;
  _ss_phase = RX;
  interrupt_reg &= ~(1 << _ss_rx_pin);
}

void ICACHE_RAM_ATTR _ss_read_bit(){
  if(_ss_rx_index < 8){
    _ss_rx_value |= GPIP(_ss_rx_pin) << _ss_rx_index++;
  } else if(_ss_rx_index == 8){
    _ss_rx_buf.write(_ss_rx_value);
    _ss_rx_value = 0;
    _ss_rx_index = 0;
    if(!_ss_tx_buf.getSize()){
      T1C = 0;
      T1I = 0;
      _ss_phase = IDLE;
      interrupt_reg |= (1 << _ss_rx_pin);
    } else {
      _ss_mode_tx();
    }
  }
}

void ICACHE_RAM_ATTR _ss_mode_rx(){
  T1C = 0;
  T1I = 0;
  ETS_FRC1_INTR_DISABLE();
  ETS_FRC_TIMER1_INTR_ATTACH(_ss_read_bit, NULL);
  ETS_FRC1_INTR_ENABLE();
  _ss_phase = IDLE;
  GPF(_ss_rx_pin) = GPFFS(GPFFS_GPIO(_ss_rx_pin)) | (1 << GPFPU);
  GPEC = (1 << _ss_rx_pin);
  GPC(_ss_rx_pin) = (GPC(_ss_rx_pin) & (0xF << GPCI)) | (1 << GPCD);
  interrupt_reg |= (1 << _ss_rx_pin);
} 
// RX END

static void _ss_begin(uint32_t bitrate, uint8_t rx_pin, uint8_t tx_pin){
  _ss_rx_pin = rx_pin;
  _ss_tx_pin = tx_pin;
  attachInterrupt(_ss_rx_pin, _ss_on_start, FALLING);
  _ss_rx_buf.flush();
  _ss_timer_period = 5000000 / bitrate;
  _ss_start_period = 0;
  if(_ss_timer_period >= 30)
    _ss_start_period = (_ss_timer_period / 10) - 4;
  _ss_mode_rx();
}

void ESPSoftSerialClass::begin(uint32_t bitrate, uint8_t rx_pin, uint8_t tx_pin){
  _ss_begin(bitrate, rx_pin, tx_pin);
}
int ESPSoftSerialClass::available(void) {
  return _ss_rx_buf.getSize();
}
int ESPSoftSerialClass::peek(void) {
  return _ss_rx_buf.peek();
}
int ESPSoftSerialClass::read(void) {
  return _ss_rx_buf.read();
}
size_t ESPSoftSerialClass::read(char* dst, size_t len){
  return _ss_rx_buf.read(dst, len);
}
void ESPSoftSerialClass::flush(void) {
  _ss_rx_buf.flush();
}
size_t ESPSoftSerialClass::write(uint8_t * data, size_t len) {
  size_t w = _ss_tx_buf.write((const char*)data, len);
  if(_ss_phase == IDLE)
    _ss_mode_tx();
  return w;
}
size_t ESPSoftSerialClass::write(uint8_t data) {
  _ss_tx_buf.write(data);
  if(_ss_phase == IDLE)
    _ss_mode_tx();
  return 1;
}

ESPSoftSerialClass SoftSerial;
