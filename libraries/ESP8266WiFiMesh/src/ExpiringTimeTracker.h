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

#ifndef __EXPIRINGTIMETRACKER_H__
#define __EXPIRINGTIMETRACKER_H__

#include <Arduino.h>
#include <PolledTimeout.h>

class ExpiringTimeTracker : private esp8266::polledTimeout::oneShotMs {
  
public:

  using calculatorType = std::function<uint32_t()>;

  virtual ~ExpiringTimeTracker() = default; 

  ExpiringTimeTracker(const uint32_t duration, const uint32_t creationTimeMs = millis());
  ExpiringTimeTracker(const calculatorType durationCalculator, const uint32_t creationTimeMs = millis());
  
  uint32_t duration() const;
  void setDuration(const uint32_t duration);
  void setDuration(const calculatorType durationCalculator);

  uint32_t remainingDuration() const;
  
  /**
   * Sets a new duration which includes the current elapsedTime(). This means elapsedTime() is not reset.
   * Note that reset() will use this new duration, including the saved elapsedTime().
   */
  void setRemainingDuration(const uint32_t remainingDuration);
  
  /**
   * Sets a new duration which includes the current elapsedTime(). This means elapsedTime() is not reset.
   * Note that reset() will use this new duration, including the saved elapsedTime().
   */
  void setRemainingDuration(const calculatorType remainingDurationCalculator);
  
  /**
   * Get the time since the ExpiringTimeTracker instance creation or the last reset(), whichever is more recent.
   */
  uint32_t elapsedTime() const;
  bool expired() const;
  void reset();
  void reset(const uint32_t newDuration);
  void reset(const calculatorType newDurationCalculator);
  explicit operator bool() const;
  
private:

  calculatorType _durationCalculator;

  void setTimeout(const uint32_t newUserTimeout);
  
  bool useCalculator = false;
};

#endif
