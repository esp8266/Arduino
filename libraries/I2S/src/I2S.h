/*
  Based off of ArduinoCore-SAMD I2S interface.  Modified for the
  ESP8266 by Earle F. Philhower, III <earlephilhower@yahoo.com>

  Copyright (c) 2016 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _I2S_H_INCLUDED
#define _I2S_H_INCLUDED

#include <Arduino.h>
#include <core_esp8266_i2s.h>

typedef enum {
  I2S_PHILIPS_MODE // Only mode allowed for now by the core
} i2s_mode_t;

class I2SClass : public Stream
{
public:
  // By default only transmit and drive the clock pins
  I2SClass(bool enableTransmit = true, bool enableRecv = false,
           bool driveClocks = true);

  // Only 16 and 24 bitsPerSample are allowed by the hardware
  // 24-bit is MSB-aligned, with 0x00 in the lowest byte of each element.
  int begin(i2s_mode_t mode, long sampleRate, int bitsPerSample);
  void end();

  // from Stream
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();

  // from Print (see notes on write() below)
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buffer, size_t size);
  virtual int availableForWrite();

  // Read up to size samples from the I2S device.  Non-blocking, will read
  // from 0...size samples and return the count read.  Be sure your app handles
  // the partial read case (i.e. yield()ing and trying to read more).
  int read(void* buffer, size_t size);

  // Write a single sample to the I2S device.  Blocking until write succeeds
  size_t write(int32_t);
  // Write up to size samples to the I2S device.  Non-blocking, will write
  // from 0...size samples and return that count.  Be sure your app handles
  // partial writes (i.e. by yield()ing and then retrying to write the
  // remaining data.
  size_t write(const void *buffer, size_t size);

  // Note that these callback are called from **INTERRUPT CONTEXT** and hence
  // must be both stored in IRAM and not perform anything that's not legal in
  // an interrupt
  void onTransmit(void(*)(void));
  void onReceive(void(*)(void));

private:
  int _bps;
  bool _running;
  bool _enableTx;
  bool _enableRx;
  bool _driveClk;
  void (*_onTransmit)(void);
  void (*_onReceive)(void);
  // Support for peek() on read path
  uint32_t _peekedData;
  int _havePeeked;
  // Support for ::write(x) on 16b wuantities
  uint32_t _writtenData;
  bool _writtenHalf;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_I2S)
extern I2SClass I2S;
#endif

#endif

