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

#include "MutexTracker.h"

std::shared_ptr<bool> MutexTracker::_captureBan = std::make_shared<bool>(false);

std::shared_ptr<bool> MutexTracker::captureBan()
{
  return _captureBan;
}

MutexTracker::MutexTracker(const std::shared_ptr<bool> &mutexToCapture) 
{
  attemptMutexCapture(mutexToCapture);
}

MutexTracker::MutexTracker(const std::shared_ptr<bool> &mutexToCapture, const std::function<void()> destructorHook) : MutexTracker(mutexToCapture)
{
  _destructorHook = destructorHook;
}

MutexTracker::~MutexTracker()
{
  releaseMutex();
  _destructorHook();
}

bool MutexTracker::mutexFree(const std::shared_ptr<bool> &mutex)
{
  if(mutex != nullptr && !(*mutex))
    return true;

  return false;
}

bool MutexTracker::mutexCaptured(const std::shared_ptr<bool> &mutex)
{
  if(mutex != nullptr && (*mutex))
    return true;

  return false;
}

bool MutexTracker::mutexCaptured() const
{
  return mutexCaptured(_capturedMutex);
}

void MutexTracker::releaseMutex()
{
  if(mutexCaptured())
  {
    *_capturedMutex = false;
    _capturedMutex.reset();
  }
}

bool MutexTracker::attemptMutexCapture(const std::shared_ptr<bool> &mutexToCapture)
{
  if(mutexFree(captureBan()) && mutexFree(mutexToCapture))
  {
    _capturedMutex = mutexToCapture;
    *_capturedMutex = true;
    return true;
  }
  
  return false;
}
