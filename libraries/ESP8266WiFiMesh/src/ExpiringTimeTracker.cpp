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

#include "ExpiringTimeTracker.h"

ExpiringTimeTracker::ExpiringTimeTracker(const uint32_t duration, const uint32_t creationTimeMs) :
  timeoutTemplate(0)
{ 
  setDuration(duration);
  _start = creationTimeMs;
}

ExpiringTimeTracker::ExpiringTimeTracker(const calculatorType durationCalculator, const uint32_t creationTimeMs) :
  timeoutTemplate(0)
{ 
  setDuration(durationCalculator);
  _start = creationTimeMs;
}

uint32_t ExpiringTimeTracker::duration() const
{
  if(useCalculator)
    return _durationCalculator();

  return getTimeout();
}

IRAM_ATTR // called from ISR
void ExpiringTimeTracker::setTimeout(const uint32_t newUserTimeout)
{
   _timeout = newUserTimeout;
   _neverExpires = (newUserTimeout > timeMax()); // newUserTimeout < 0 is always false for uint32_t
}

void ExpiringTimeTracker::setDuration(const uint32_t duration)
{
  setTimeout(duration);
  useCalculator = false;
}

void ExpiringTimeTracker::setDuration(const calculatorType durationCalculator)
{
  _durationCalculator = durationCalculator;
  useCalculator = true;
}

void ExpiringTimeTracker::setRemainingDuration(const uint32_t remainingDuration)
{
  setDuration(elapsedTime() + remainingDuration);
}

void ExpiringTimeTracker::setRemainingDuration(const calculatorType remainingDurationCalculator)
{
  uint32_t currentElapsedTime = elapsedTime();
  setDuration([remainingDurationCalculator, currentElapsedTime](){ return currentElapsedTime + remainingDurationCalculator(); });
}

uint32_t ExpiringTimeTracker::remainingDuration() const
{
  uint32_t remainingDuration = 0;
  
  if(!expired()) // If expired, overflow will probably occur for remainingDuration calculation.
  {
    remainingDuration = duration() - elapsedTime();
  }

  return remainingDuration;
}

uint32_t ExpiringTimeTracker::elapsedTime() const
{
  return millis() - _start;
}

bool ExpiringTimeTracker::expired() const
{  
  if(useCalculator)
    return elapsedTime() >= duration();

  return expiredOneShot();
}

void ExpiringTimeTracker::reset()
{
  timeoutTemplate::reset();
}

void ExpiringTimeTracker::reset(const uint32_t newDuration)
{
  setDuration(newDuration);
  ExpiringTimeTracker::reset();
}

void ExpiringTimeTracker::reset(const calculatorType newDurationCalculator)
{
  setDuration(newDurationCalculator);
  ExpiringTimeTracker::reset();
}

ExpiringTimeTracker::operator bool() const
{
  return ExpiringTimeTracker::expired();
}
