/*
 * Copyright (C) 2020 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __ESPHEAPMONITOR_H__
#define __ESPHEAPMONITOR_H__

#include <Arduino.h>

class HeapMonitor {

public:

  enum class HeapStatus
  {
    NOMINAL    = 0,
    LIMITED    = 1,
    CRITICAL   = 2
  };

  HeapMonitor(const uint32_t criticalHeapLevel, const uint32_t criticalHeapLevelBuffer);

  virtual ~HeapMonitor() = default; 

  /**
   * Set the maximum free heap level in bytes within which free heap size is considered critical. 
   */
  void setCriticalHeapLevel(const uint32_t freeHeapInBytes);
  uint32_t getCriticalHeapLevel() const;

  /**
   * Set the buffer of the critical heap level, within which free heap size is considered limited. 
   */
  void setCriticalHeapLevelBuffer(const uint32_t bufferInBytes);
  uint32_t getCriticalHeapLevelBuffer() const;

  HeapStatus getHeapStatus() const;

private:

  uint32_t _criticalHeapLevel;
  uint32_t _criticalHeapLevelBuffer;

};

#endif
