/*
  Based off of ArduinoCore-SAMD I2S interface.  Modified for the
  ESP8266 by Earle F. Philhower, III <earlephilhower@yahoo.com>

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

#include <Arduino.h>
#include "I2S.h"

I2SClass::I2SClass(bool enableTransmit, bool enableRecv, bool driveClocks) {
  _enableTx = enableTransmit;
  _enableRx = enableRecv;
  _driveClk = driveClocks;
  _running = false;
  _onTransmit = nullptr;
  _onReceive = nullptr;
  _havePeeked = 0;
  _peekedData = 0;
  _bps = 0;
  _writtenHalf = false;
}

int I2SClass::begin(int mode, long sampleRate, int bitsPerSample) {
  if ( _running || (mode != I2S_PHILIPS_MODE) || ( (bitsPerSample != 16) && (bitsPerSample != 24) ) ) {
    return 0;
  }
  if (!i2s_rxtxdrive_begin(_enableRx, _enableTx, _driveClk, _driveClk)) {
    return 0;
  }
  i2s_set_rate(sampleRate);
  i2s_set_callback(_onTransmit);
  i2s_rx_set_callback(_onReceive);
  _bps = bitsPerSample;
  _running = true;
  return 1;
}

void I2SClass::end() {
  if (_running) {
    i2s_end();
  }
  i2s_set_callback(nullptr);
  i2s_rx_set_callback(nullptr);
  _running = false;
}

void I2SClass::onTransmit(void(*fcn)(void)) {
  i2s_set_callback(fcn);
  _onTransmit = fcn;
}

void I2SClass::onReceive(void(*fcn)(void)) {
  i2s_rx_set_callback(fcn);
  _onReceive = fcn;
}

int I2SClass::available() {
  if (!_running) return 0;
  return i2s_rx_available();
}

int I2SClass::availableForWrite() {
  if (!_running) return 0;
  return i2s_available();
}

void I2SClass::flush() {
  /* No-op */
}

int I2SClass::read() {
  if (!_running) return -1;
  if (_havePeeked) {
    if (_bps == 16) {
      _havePeeked--;
      int ret = _peekedData;
      _peekedData >>= 16;
      return ret;
    } else {
      _havePeeked = 0;
      return _peekedData;
    }
  }
  // Avoid code duplication by just peeking and calling read() again
  peek();
  return read();
}

int I2SClass::peek() {
  if (!_running) return -1;
  if (_havePeeked) {
    if (_bps == 16) {
      int16_t sample = (int16_t)_peekedData; // Will extends sign on return
      return sample;
    } else {
      return _peekedData;
    }
  }
  int16_t l, r;
  i2s_read_sample(&l, &r, true);
  _peekedData = ((int)l << 16) | (0xffff & (int)r);
  _havePeeked = 2; // We now have 2 16-bit quantities which can also be used as 1 32-bit(24-bit)
  if (_bps == 16) {
    return r;
  } else {
    return _peekedData;
  }
}

int I2SClass::read(void *buffer, size_t size) {
  if (!_running) return -1;
  int cnt = 0;

  if ( ((_bps == 24) && (size % 4)) || ((_bps == 16) && (size % 2)) || (size < 2) ) {
    return 0; // Invalid, can only read in units of samples
  }
  // Make sure any peeked data is consumed first
  if (_havePeeked) {
    if (_bps == 16) {
      while (_havePeeked && size) {
        uint16_t *p = (uint16_t *)buffer;
        *(p++) = _peekedData;
        _peekedData >>= 16;
        _havePeeked--;
        buffer = (void *)p;
        size -= 2;
        cnt += 2;
      }
    } else {
      uint32_t *p = (uint32_t *)buffer;
      *(p++) = _peekedData;
      buffer = (void *)p;
      size -= 4;
      cnt += 4;
    }
  }
  // Now just non-blocking read up to the remaining size
  int16_t l, r;
  int16_t *p = (int16_t *)buffer;
  while (size && i2s_read_sample(&l, &r, false)) {
    *(p++) = l;
    size--;
    cnt++;
    if (size) {
      *(p++) = r;
      size--;
      cnt++;
    } else {
      // We read a simple we can't return, stuff it in the peeked data
      _havePeeked = 1;
      _peekedData = r;
    }
  }
  return cnt;
}

size_t I2SClass::write(uint8_t s) {
  if (!_running) return 0;
  return write((int32_t)s);
}

size_t I2SClass::write(const uint8_t *buffer, size_t size) {
  return write((const void *)buffer, size);
}

size_t I2SClass::write(int32_t s) {
  if (!_running) return 0;
  // Because our HW really wants 32b writes, store any 16b writes until another
  // 16b write comes in and then send the combined write down.
  if (_bps == 16) {
    if (_writtenHalf) {
      _writtenData <<= 16;
      _writtenData |= 0xffff & s;
      _writtenHalf = false;
      return i2s_write_sample(_writtenData) ? 1 : 0;
    } else {
      _writtenHalf = true;
      _writtenData = s & 0xffff;
      return 1;
    }
  } else {
    return i2s_write_sample((uint32_t)s) ? 1 : 0;
  }
}

// SAMD core has this as non-blocking
size_t I2SClass::write(const void *buffer, size_t size) {
  if (!_running) return 0;
  return i2s_write_buffer_nb((int16_t *)buffer, size / 2);
}


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LITTLEFS)
I2SClass I2S;
#endif

