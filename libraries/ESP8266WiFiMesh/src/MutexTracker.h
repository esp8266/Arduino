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

#ifndef __MUTEXTRACKER_H__
#define __MUTEXTRACKER_H__

#include <functional>

/**
 * A SLIM (Scope LImited Manager)/Scope-Bound Resource Management/RAII class to manage the state of a mutex.
 */
class MutexTracker
{
  public:

    /**
    * Attempts to capture the mutex. Use the mutexCaptured() method to check success.
    */
    MutexTracker(bool &mutexToCapture);

    /**
    * Attempts to capture the mutex. Use the mutexCaptured() method to check success.
    * 
    * @param destructorHook A function to hook into the MutexTracker destructor. Will be called when the MutexTracker instance is being destroyed, after the mutex has been released.
    */
    MutexTracker(bool &mutexToCapture, std::function<void()> destructorHook);

    ~MutexTracker();

    bool mutexCaptured();

    /**
     * Set the mutex free to roam the binary plains, giving new MutexTrackers a chance to capture it.
     */
    void releaseMutex();

  private:

    bool *_capturedMutex = nullptr;
    std::function<void()> _destructorHook = [](){ };

    /**
     * Attempt to capture the mutex.
     * 
     * @return True if mutex was caught (meaning no other instance is holding the mutex). False otherwise.
     */
    bool attemptMutexCapture(bool &mutexToCapture);
};

#endif
