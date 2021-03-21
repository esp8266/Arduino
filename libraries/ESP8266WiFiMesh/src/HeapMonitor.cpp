/*
 * Copyright (C) 2019 Anders Löfgren
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

#include "HeapMonitor.h"

HeapMonitor::HeapMonitor(const uint32_t criticalHeapLevel, const uint32_t criticalHeapLevelBuffer) : 
  _criticalHeapLevel(criticalHeapLevel), _criticalHeapLevelBuffer(criticalHeapLevelBuffer)
{  }

void HeapMonitor::setCriticalHeapLevel(const uint32_t freeHeapInBytes)
{
  _criticalHeapLevel = freeHeapInBytes;
}

uint32_t HeapMonitor::getCriticalHeapLevel() const
{
  return _criticalHeapLevel;
}

void HeapMonitor::setCriticalHeapLevelBuffer(const uint32_t bufferInBytes)
{
  _criticalHeapLevelBuffer = bufferInBytes;
}

uint32_t HeapMonitor::getCriticalHeapLevelBuffer() const
{
  return _criticalHeapLevelBuffer;
}

HeapMonitor::HeapStatus HeapMonitor::getHeapStatus() const
{
  HeapStatus heapStatus = HeapStatus::NOMINAL;
  
  uint32_t freeHeap = ESP.getFreeHeap();
  
  if(freeHeap <= getCriticalHeapLevel())
    heapStatus = HeapStatus::CRITICAL;
  else if(freeHeap <= getCriticalHeapLevel() + getCriticalHeapLevelBuffer())
    heapStatus = HeapStatus::LIMITED;

  return heapStatus;
}
