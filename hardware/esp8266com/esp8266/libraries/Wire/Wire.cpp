/*
  TwoWire.cpp - TWI/I2C library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified December 2014 by Ivan Grokhotkov (ivan@esp8266.com) - esp8266 support
  Modified April 2015 by Hrsto Gochkov (ficeto@ficeto.com) - alternative esp8266 support
*/

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include "twi.h"
#include "Wire.h"

// Initialize Class Variables //////////////////////////////////////////////////

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
uint8_t TwoWire::txBufferLength = 0;

uint8_t TwoWire::transmitting = 0;
void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

static int default_sda_pin = SDA;
static int default_scl_pin = SCL;

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire(){}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(int sda, int scl){
  default_sda_pin = sda;
  default_scl_pin = scl;
  twi_init(sda, scl);
  flush();
}

void TwoWire::pins(int sda, int scl){
  default_sda_pin = sda;
  default_scl_pin = scl;
}

void TwoWire::begin(void){
  begin(default_sda_pin, default_scl_pin);
}

void TwoWire::begin(uint8_t address){
  // twi_setAddress(address);
  // twi_attachSlaveTxEvent(onRequestService);
  // twi_attachSlaveRxEvent(onReceiveService);
  begin();
}

void TwoWire::begin(int address){
  begin((uint8_t)address);
}

void TwoWire::setClock(uint32_t frequency){
  twi_setClock(frequency);
}

size_t TwoWire::requestFrom(uint8_t address, size_t size, bool sendStop){
  if(size > BUFFER_LENGTH){
    size = BUFFER_LENGTH;
  }
  size_t read = (twi_readFrom(address, rxBuffer, size, sendStop) == 0)?size:0;
  rxBufferIndex = 0;
  rxBufferLength = read;
  return read;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop){
  return requestFrom(address, static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity){
  return requestFrom(address, static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity){
  return requestFrom(static_cast<uint8_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop){
  return requestFrom(static_cast<uint8_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

void TwoWire::beginTransmission(uint8_t address){
  transmitting = 1;
  txAddress = address;
  txBufferIndex = 0;
  txBufferLength = 0;
}

void TwoWire::beginTransmission(int address){
  beginTransmission((uint8_t)address);
}

uint8_t TwoWire::endTransmission(uint8_t sendStop){
  int8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, sendStop);
  txBufferIndex = 0;
  txBufferLength = 0;
  transmitting = 0;
  return ret;
}

uint8_t TwoWire::endTransmission(void){
  return endTransmission(true);
}

size_t TwoWire::write(uint8_t data){
  if(transmitting){
    if(txBufferLength >= BUFFER_LENGTH){
      setWriteError();
      return 0;
    }
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    txBufferLength = txBufferIndex;
  } else {
    // i2c_slave_transmit(&data, 1);
  }
  return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity){
  if(transmitting){
    for(size_t i = 0; i < quantity; ++i){
      if(!write(data[i])) return i;
    }
  }else{
    // i2c_slave_transmit(data, quantity);
  }
  return quantity;
}

int TwoWire::available(void){
  int result = rxBufferLength - rxBufferIndex;

  if (!result) {
    // yielding here will not make more data "available",
    // but it will prevent the system from going into WDT reset
    optimistic_yield(1000);
  }

  return result;
}

int TwoWire::read(void){
  int value = -1;
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }
  return value;
}

int TwoWire::peek(void){
  int value = -1;
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }
  return value;
}

void TwoWire::flush(void){
  rxBufferIndex = 0;
  rxBufferLength = 0;
  txBufferIndex = 0;
  txBufferLength = 0;
}

void TwoWire::onReceiveService(uint8_t* inBytes, int numBytes)
{
  // don't bother if user hasn't registered a callback
  // if(!user_onReceive){
  //   return;
  // }
  // // don't bother if rx buffer is in use by a master requestFrom() op
  // // i know this drops data, but it allows for slight stupidity
  // // meaning, they may not have read all the master requestFrom() data yet
  // if(rxBufferIndex < rxBufferLength){
  //   return;
  // }
  // // copy twi rx buffer into local read buffer
  // // this enables new reads to happen in parallel
  // for(uint8_t i = 0; i < numBytes; ++i){
  //   rxBuffer[i] = inBytes[i];
  // }
  // // set rx iterator vars
  // rxBufferIndex = 0;
  // rxBufferLength = numBytes;
  // // alert user program
  // user_onReceive(numBytes);
}

void TwoWire::onRequestService(void){
  // // don't bother if user hasn't registered a callback
  // if(!user_onRequest){
  //   return;
  // }
  // // reset tx buffer iterator vars
  // // !!! this will kill any pending pre-master sendTo() activity
  // txBufferIndex = 0;
  // txBufferLength = 0;
  // // alert user program
  // user_onRequest();
}

void TwoWire::onReceive( void (*function)(int) ){
  //user_onReceive = function;
}

void TwoWire::onRequest( void (*function)(void) ){
  //user_onRequest = function;
}

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();
