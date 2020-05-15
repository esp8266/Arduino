/*
 * UtilityFunctions
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

#include "UtilityFunctions.h"
#include <esp8266_peri.h>

namespace MeshUtilityFunctions
{
  bool macEqual(const uint8_t *macOne, const uint8_t *macTwo)
  {
    for(int i = 0; i <= 5; ++i)
    {
      if(macOne[i] != macTwo[i])
      {
        return false;
      }
    }
  
    return true;
  }
  
  uint64_t randomUint64()
  {
    return (((uint64_t)RANDOM_REG32 << 32) | (uint64_t)RANDOM_REG32);
  }

  template <typename T>
  T *getMapValue(std::map<uint64_t, T> &mapIn, const uint64_t keyIn)
  {
    typename std::map<uint64_t, T>::iterator mapIterator = mapIn.find(keyIn);

    if(mapIterator != mapIn.end())
    {
      return &mapIterator->second;
    }

    return nullptr;
  }
}
